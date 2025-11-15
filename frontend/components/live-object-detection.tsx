'use client'

import { useEffect, useRef, useState } from 'react'
import { Card } from '@/components/ui/card'
import { Button } from '@/components/ui/button'
import { Badge } from '@/components/ui/badge'
import { Volume2, VolumeX, Camera, CameraOff } from 'lucide-react'

interface Detection {
  class: string
  score: number
  bbox: [number, number, number, number] // [x, y, width, height]
}

export function LiveObjectDetection() {
  const imgRef = useRef<HTMLImageElement>(null)
  const canvasRef = useRef<HTMLCanvasElement>(null)
  const [isStreaming, setIsStreaming] = useState(false)
  const [isDetecting, setIsDetecting] = useState(false)
  const [audioEnabled, setAudioEnabled] = useState(true)
  const [detections, setDetections] = useState<Detection[]>([])
  const [model, setModel] = useState<any>(null)
  const [fps, setFps] = useState(0)
  const [isCapturing, setIsCapturing] = useState(false)
  const animationFrameRef = useRef<number>()
  const lastFrameTimeRef = useRef<number>(0)
  const speechSynthesisRef = useRef<SpeechSynthesisUtterance | null>(null)
  const lastSpokenRef = useRef<string>('')
  const lastSpeakTimeRef = useRef<number>(0)
  const snapshotIntervalRef = useRef<NodeJS.Timeout | null>(null)

  // Load TensorFlow.js and COCO-SSD model
  useEffect(() => {
    const loadModel = async () => {
      try {
        // Dynamically import TensorFlow.js libraries
        const tf = await import('@tensorflow/tfjs')
        const cocoSsd = await import('@tensorflow-models/coco-ssd')
        
        // Set backend to WebGL for better performance
        await tf.setBackend('webgl')
        await tf.ready()
        
        // Load COCO-SSD model
        const loadedModel = await cocoSsd.load({
          base: 'mobilenet_v2' // Faster, use 'lite_mobilenet_v2' for even faster
        })
        
        setModel(loadedModel)
        console.log('COCO-SSD model loaded successfully')
      } catch (error) {
        console.error('Error loading model:', error)
      }
    }

    loadModel()

    // Initialize speech synthesis
    if (typeof window !== 'undefined' && 'speechSynthesis' in window) {
      speechSynthesisRef.current = new SpeechSynthesisUtterance()
      speechSynthesisRef.current.rate = 1.2
      speechSynthesisRef.current.pitch = 1.0
      speechSynthesisRef.current.volume = 1.0
    }

    return () => {
      if (animationFrameRef.current) {
        cancelAnimationFrame(animationFrameRef.current)
      }
      stopStream()
    }
  }, [])

  const startStream = async () => {
    try {
      const img = imgRef.current
      const canvas = canvasRef.current
      if (!img || !canvas) return

      // Set canvas size  
      canvas.width = 640
      canvas.height = 480

      setIsStreaming(true)
      
      let lastCaptureTime = 0
      
      // Poll for new captures every 500ms (button-triggered, so no need for high frequency)
      snapshotIntervalRef.current = setInterval(async () => {
        try {
          // Fetch with cache-busting and check for new capture
          const response = await fetch(`http://localhost:8000/api/stream/snapshot?t=${Date.now()}`)
          
          if (response.ok) {
            const captureTime = response.headers.get('X-Capture-Time')
            const newTime = captureTime ? parseInt(captureTime) : Date.now()
            
            // Only update if it's a new capture
            if (newTime > lastCaptureTime) {
              lastCaptureTime = newTime
              const blob = await response.blob()
              img.src = URL.createObjectURL(blob)
              console.log('New image received from button press')
            }
          }
        } catch (err) {
          console.error('Failed to fetch snapshot:', err)
        }
      }, 500)
      
      // When image loads, it will trigger detection
      img.onload = () => {
        if (!isStreaming) return
        
        // Draw image to canvas for detection
        const ctx = canvas.getContext('2d')
        if (ctx) {
          ctx.drawImage(img, 0, 0, canvas.width, canvas.height)
        }
      }
      
      img.onerror = () => {
        console.error('Failed to load snapshot')
      }
      
    } catch (error) {
      console.error('Error starting stream:', error)
      alert('Failed to access camera stream.')
    }
  }

  const stopStream = () => {
    if (snapshotIntervalRef.current) {
      clearInterval(snapshotIntervalRef.current)
      snapshotIntervalRef.current = null
    }
    setIsStreaming(false)
    setIsDetecting(false)
    
    if (animationFrameRef.current) {
      cancelAnimationFrame(animationFrameRef.current)
    }
  }

  const speakAlert = (text: string) => {
    if (!audioEnabled || !speechSynthesisRef.current) return
    
    const now = Date.now()
    // Prevent speaking the same thing within 3 seconds
    if (text === lastSpokenRef.current && now - lastSpeakTimeRef.current < 3000) {
      return
    }
    
    // Cancel any ongoing speech
    window.speechSynthesis.cancel()
    
    speechSynthesisRef.current.text = text
    window.speechSynthesis.speak(speechSynthesisRef.current)
    
    lastSpokenRef.current = text
    lastSpeakTimeRef.current = now
  }

  const getDistanceEstimate = (bbox: [number, number, number, number]): string => {
    const [x, y, width, height] = bbox
    const area = width * height
    const canvas = canvasRef.current
    if (!canvas) return 'unknown distance'
    
    const totalArea = canvas.width * canvas.height
    const proportion = area / totalArea
    
    if (proportion > 0.3) return 'very close'
    if (proportion > 0.15) return 'close'
    if (proportion > 0.05) return 'medium distance'
    return 'far away'
  }

  const getPosition = (bbox: [number, number, number, number]): string => {
    const [x, y, width, height] = bbox
    const canvas = canvasRef.current
    if (!canvas) return 'center'
    
    const centerX = x + width / 2
    const canvasCenter = canvas.width / 2
    
    if (centerX < canvasCenter * 0.4) return 'on your left'
    if (centerX > canvasCenter * 1.6) return 'on your right'
    return 'ahead'
  }

  const detectFrame = async () => {
    if (!model || !isDetecting) return

    const canvas = canvasRef.current
    if (!canvas) return

    const ctx = canvas.getContext('2d')
    if (!ctx) return

    try {
      // Calculate FPS
      const now = performance.now()
      const elapsed = now - lastFrameTimeRef.current
      if (elapsed > 0) {
        setFps(Math.round(1000 / elapsed))
      }
      lastFrameTimeRef.current = now

      // Run object detection on canvas (already has image from img.onload)
      const predictions = await model.detect(canvas)
      setDetections(predictions)

      // Draw bounding boxes and labels
      predictions.forEach((prediction: Detection) => {
        const [x, y, width, height] = prediction.bbox
        
        // Draw bounding box
        ctx.strokeStyle = '#7c3aed'
        ctx.lineWidth = 3
        ctx.strokeRect(x, y, width, height)
        
        // Draw label background
        ctx.fillStyle = '#7c3aed'
        const label = `${prediction.class} ${Math.round(prediction.score * 100)}%`
        ctx.font = '16px Inter, sans-serif'
        const textWidth = ctx.measureText(label).width
        ctx.fillRect(x, y - 25, textWidth + 10, 25)
        
        // Draw label text
        ctx.fillStyle = '#ffffff'
        ctx.fillText(label, x + 5, y - 7)
      })

      // Speak important detections (high confidence objects close to user)
      const importantDetections = predictions.filter(
        (p: Detection) => p.score > 0.6 && p.bbox[2] * p.bbox[3] > 10000
      )

      if (importantDetections.length > 0) {
        const detection = importantDetections[0]
        const distance = getDistanceEstimate(detection.bbox)
        const position = getPosition(detection.bbox)
        const alert = `${detection.class} ${position}, ${distance}`
        speakAlert(alert)
      }

      // Continue detection loop
      animationFrameRef.current = requestAnimationFrame(detectFrame)
    } catch (error) {
      console.error('Detection error:', error)
      animationFrameRef.current = requestAnimationFrame(detectFrame)
    }
  }

  const toggleDetection = () => {
    if (!isDetecting) {
      setIsDetecting(true)
      detectFrame()
    } else {
      setIsDetecting(false)
      if (animationFrameRef.current) {
        cancelAnimationFrame(animationFrameRef.current)
      }
    }
  }

  return (
    <section className="py-24 relative">
      <div className="container mx-auto px-4 lg:px-8">
        <div className="max-w-5xl mx-auto">
          <div className="text-center mb-12">
            <h2 className="text-4xl md:text-5xl font-bold mb-6">
              Live <span className="text-gradient">Object Detection</span>
            </h2>
            <p className="text-lg text-muted-foreground leading-relaxed">
              Real-time AI-powered object detection with audio guidance
            </p>
          </div>

          <Card className="p-6 bg-card border-border">
            {/* Snapshot Stream Display */}
            <div className="relative bg-black rounded-lg overflow-hidden mb-6" style={{ aspectRatio: '4/3' }}>
              <img
                ref={imgRef}
                className="absolute inset-0 w-full h-full object-contain"
                style={{ display: 'none' }}
                alt="ESP32-CAM Stream"
              />
              <canvas
                ref={canvasRef}
                className="absolute inset-0 w-full h-full object-contain"
                style={{ display: isStreaming ? 'block' : 'none' }}
              />
              
              {!isStreaming && (
                <div className="absolute inset-0 flex items-center justify-center">
                  <div className="text-center">
                    <CameraOff className="w-16 h-16 text-muted-foreground mx-auto mb-4" />
                    <p className="text-muted-foreground">Camera not active</p>
                  </div>
                </div>
              )}

              {/* FPS Counter */}
              {isDetecting && (
                <div className="absolute top-4 left-4 bg-black/70 backdrop-blur-sm rounded px-3 py-1">
                  <p className="text-sm font-mono text-white">{fps} FPS</p>
                </div>
              )}

              {/* Detection Status */}
              {isStreaming && (
                <div className="absolute top-4 right-4">
                  <Badge variant={isDetecting ? "default" : "secondary"}>
                    {isDetecting ? 'Detecting' : 'Paused'}
                  </Badge>
                </div>
              )}
            </div>

            {/* Controls */}
            <div className="flex flex-wrap gap-3 mb-6">
              <Button
                onClick={isStreaming ? stopStream : startStream}
                variant={isStreaming ? "destructive" : "default"}
                className="flex-1 sm:flex-none"
              >
                {isStreaming ? (
                  <>
                    <CameraOff className="w-4 h-4 mr-2" />
                    Stop Camera
                  </>
                ) : (
                  <>
                    <Camera className="w-4 h-4 mr-2" />
                    Start Camera
                  </>
                )}
              </Button>

              <Button
                onClick={toggleDetection}
                disabled={!isStreaming || !model}
                variant="secondary"
                className="flex-1 sm:flex-none"
              >
                {isDetecting ? 'Pause Detection' : 'Start Detection'}
              </Button>

              <Button
                onClick={() => setAudioEnabled(!audioEnabled)}
                variant="outline"
                size="icon"
              >
                {audioEnabled ? (
                  <Volume2 className="w-4 h-4" />
                ) : (
                  <VolumeX className="w-4 h-4" />
                )}
              </Button>
            </div>

            {/* Model Status */}
            <div className="mb-6">
              <div className="flex items-center gap-2">
                <div className={`w-2 h-2 rounded-full ${model ? 'bg-green-500' : 'bg-yellow-500 animate-pulse'}`} />
                <p className="text-sm text-muted-foreground">
                  {model ? 'AI Model Ready (COCO-SSD)' : 'Loading AI model...'}
                </p>
              </div>
            </div>

            {/* Detections List */}
            {detections.length > 0 && (
              <div className="space-y-2">
                <h3 className="text-sm font-semibold mb-3">Detected Objects:</h3>
                <div className="grid grid-cols-2 sm:grid-cols-3 md:grid-cols-4 gap-2">
                  {detections.map((detection, idx) => (
                    <div
                      key={idx}
                      className="bg-secondary/30 rounded-lg px-3 py-2"
                    >
                      <p className="text-sm font-medium truncate">{detection.class}</p>
                      <p className="text-xs text-muted-foreground">
                        {Math.round(detection.score * 100)}% confidence
                      </p>
                    </div>
                  ))}
                </div>
              </div>
            )}

            {/* Instructions */}
            <div className="mt-6 p-4 bg-secondary/20 rounded-lg">
              <h4 className="text-sm font-semibold mb-2">Instructions:</h4>
              <ul className="text-sm text-muted-foreground space-y-1">
                <li>• Click "Start Camera" to begin video stream</li>
                <li>• Click "Start Detection" to enable AI object detection</li>
                <li>• Audio alerts will announce detected objects with position and distance</li>
                <li>• Toggle audio with the speaker icon</li>
              </ul>
            </div>
          </Card>
        </div>
      </div>
    </section>
  )
}
