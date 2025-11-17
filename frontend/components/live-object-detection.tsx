'use client'

import { useEffect, useRef, useState } from 'react'
import { Card } from '@/components/ui/card'
import { Button } from '@/components/ui/button'
import { Badge } from '@/components/ui/badge'
import { Volume2, Camera, Loader2, Eye } from 'lucide-react'

interface Detection {
  class: string
  confidence: number
  bbox: [number, number, number, number]
}

const API_BASE_URL = 'http://localhost:8000'

export function LiveObjectDetection() {
  const imgRef = useRef<HTMLImageElement>(null)
  const audioRef = useRef<HTMLAudioElement>(null)
  const [snapshot, setSnapshot] = useState<string | null>(null)
  const [detections, setDetections] = useState<Detection[]>([])
  const [description, setDescription] = useState<string>('')
  const [isProcessing, setIsProcessing] = useState(false)
  const [audioUrl, setAudioUrl] = useState<string | null>(null)
  const [error, setError] = useState<string | null>(null)
  const [useTestImage, setUseTestImage] = useState(true)

  // Load test image on mount
  useEffect(() => {
    if (useTestImage) {
      setSnapshot(`${API_BASE_URL}/api/ai/test-image?t=${Date.now()}`)
    }
  }, [useTestImage])

  const captureAndProcess = async () => {
    setIsProcessing(true)
    setError(null)
    
    try {
      let imageBlob: Blob
      
      if (useTestImage) {
        // Use test image for development
        console.log('📸 Using test image...')
        const imageResponse = await fetch(`${API_BASE_URL}/api/ai/test-image`)
        if (!imageResponse.ok) {
          throw new Error('Failed to fetch test image')
        }
        imageBlob = await imageResponse.blob()
      } else {
        // Real ESP32 flow
        console.log('📸 Triggering ESP32 capture...')
        const triggerResponse = await fetch(`${API_BASE_URL}/api/stream/trigger-capture`, {
          method: 'POST'
        })
        
        if (!triggerResponse.ok) {
          throw new Error('Failed to trigger ESP32 capture')
        }
        
        // Wait for ESP32 to process
        await new Promise(resolve => setTimeout(resolve, 500))
        
        // Get captured image
        console.log('🖼️  Fetching captured image...')
        const imageResponse = await fetch(`${API_BASE_URL}/api/stream/snapshot?t=${Date.now()}`)
        if (!imageResponse.ok) {
          throw new Error('Failed to fetch snapshot')
        }
        
        imageBlob = await imageResponse.blob()
      }
      
      const imageUrl = URL.createObjectURL(imageBlob)
      setSnapshot(imageUrl)
      
      // Send to AI model for processing
      console.log('🤖 Processing with AI model...')
      const formData = new FormData()
      formData.append('file', imageBlob, 'snapshot.jpg')
      
      const aiResponse = await fetch(`${API_BASE_URL}/api/ai/detect`, {
        method: 'POST',
        body: formData,
      })
      
      if (!aiResponse.ok) {
        throw new Error('AI processing failed')
      }
      
      const result = await aiResponse.json()
      console.log('✅ AI processing complete:', result)
      
      // Update UI with results
      setDetections(result.detections || [])
      setDescription(result.description || 'No objects detected')
      
      // Set audio URL
      if (result.audio_url) {
        setAudioUrl(`${API_BASE_URL}${result.audio_url}?t=${Date.now()}`)
      }
      
      // Display detected image with bounding boxes
      if (result.image_url) {
        setSnapshot(`${API_BASE_URL}${result.image_url}?t=${Date.now()}`)
      }
      
    } catch (err) {
      console.error('Error during capture and processing:', err)
      setError(err instanceof Error ? err.message : 'Processing failed')
    } finally {
      setIsProcessing(false)
    }
  }

  const playAudio = () => {
    if (audioRef.current && audioUrl) {
      audioRef.current.play().catch(err => {
        console.error('Error playing audio:', err)
        setError('Failed to play audio')
      })
    } else {
      setError('No audio available. Please capture and analyze an image first.')
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
              AI-powered object detection with audio descriptions
            </p>
          </div>

          <Card className="p-6 bg-card border-border">
            {/* Controls */}
            <div className="flex gap-4 flex-wrap justify-center mb-6">
              <Button
                onClick={captureAndProcess}
                size="lg"
                disabled={isProcessing}
                className="bg-green-600 hover:bg-green-700"
              >
                {isProcessing ? (
                  <>
                    <Loader2 className="mr-2 h-5 w-5 animate-spin" />
                    Processing...
                  </>
                ) : (
                  <>
                    <Camera className="mr-2 h-5 w-5" />
                    Capture & Analyze
                  </>
                )}
              </Button>

              <Button
                onClick={playAudio}
                size="lg"
                disabled={!audioUrl || isProcessing}
                className="bg-blue-600 hover:bg-blue-700"
              >
                <Volume2 className="mr-2 h-5 w-5" />
                Play Description
              </Button>

              <Button
                onClick={() => setUseTestImage(!useTestImage)}
                size="lg"
                variant="outline"
              >
                {useTestImage ? 'Use ESP32 Camera' : 'Use Test Image'}
              </Button>
            </div>

            {/* Error Message */}
            {error && (
              <div className="mb-6 p-3 bg-red-100 border border-red-400 text-red-700 rounded-lg">
                <p className="text-sm font-medium">⚠️ {error}</p>
              </div>
            )}

            {/* Image Display */}
            <div className="relative bg-black rounded-lg overflow-hidden mb-6" style={{ aspectRatio: '4/3' }}>
              {snapshot ? (
                <img
                  ref={imgRef}
                  src={snapshot}
                  alt="Detected objects"
                  className="w-full h-full object-contain"
                />
              ) : (
                <div className="absolute inset-0 flex items-center justify-center text-white">
                  <p className="text-lg">Click "Capture & Analyze" to begin</p>
                </div>
              )}
              
              {/* Processing Overlay */}
              {isProcessing && (
                <div className="absolute inset-0 bg-black/60 flex items-center justify-center">
                  <div className="text-center text-white">
                    <Loader2 className="h-12 w-12 animate-spin mx-auto mb-2" />
                    <p className="text-lg font-medium">Analyzing image...</p>
                  </div>
                </div>
              )}
            </div>

            {/* Detection Results */}
            {detections.length > 0 && (
              <div className="space-y-4">
                <div className="flex items-center gap-2">
                  <Eye className="h-6 w-6 text-violet-600" />
                  <h3 className="text-xl font-semibold">
                    Detected Objects ({detections.length})
                  </h3>
                </div>
                
                <div className="grid grid-cols-2 sm:grid-cols-3 md:grid-cols-4 gap-2">
                  {detections.map((det, idx) => (
                    <div
                      key={idx}
                      className="flex items-center justify-between p-3 bg-violet-50 rounded-lg"
                    >
                      <div className="flex items-center gap-2">
                        <div className="w-2 h-2 bg-violet-600 rounded-full"></div>
                        <span className="font-medium text-gray-900 text-sm">{det.class}</span>
                      </div>
                      <span className="text-xs text-gray-600">
                        {(det.confidence * 100).toFixed(0)}%
                      </span>
                    </div>
                  ))}
                </div>

                {/* Description */}
                {description && (
                  <div className="p-4 bg-blue-50 rounded-lg border border-blue-200">
                    <p className="text-gray-800 font-medium">{description}</p>
                  </div>
                )}
              </div>
            )}

            {/* Mode Badge */}
            <div className="mt-6 flex justify-center">
              <Badge variant={useTestImage ? 'secondary' : 'default'}>
                {useTestImage ? '🖼️  Test Image Mode' : '📷 ESP32-CAM Mode'}
              </Badge>
            </div>
          </Card>

          {/* Hidden Audio Element */}
          <audio ref={audioRef} src={audioUrl || undefined} />
        </div>
      </div>
    </section>
  )
}
