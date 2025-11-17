'use client'

import { Button } from '@/components/ui/button'
import { ArrowRight, Play, Volume2, VolumeX } from 'lucide-react'
import { useState, useRef, useEffect } from 'react'

export function HeroSection() {
  const [isSpeaking, setIsSpeaking] = useState(false)
  const utteranceRef = useRef<SpeechSynthesisUtterance | null>(null)

  const handleTextToSpeech = () => {
    if (isSpeaking) {
      // Stop speaking
      window.speechSynthesis.cancel()
      setIsSpeaking(false)
      return
    }

    // Text content to read
    const textToRead = `
      Empowering Vision Through Intelligent Technology.
      Revolutionary wearable device combining ESP32 camera modules with advanced AI object detection 
      to provide real-time navigation assistance for the blind and visually impaired.
      Features include: AI Object Detection, Real-Time Distance Alerts, GPS Navigation, and Audio Feedback.
    `

    const utterance = new SpeechSynthesisUtterance(textToRead)
    utteranceRef.current = utterance

    // Configure voice
    utterance.rate = 0.9 // Slightly slower for clarity
    utterance.pitch = 1.0
    utterance.volume = 1.0

    // Event handlers
    utterance.onstart = () => setIsSpeaking(true)
    utterance.onend = () => setIsSpeaking(false)
    utterance.onerror = () => setIsSpeaking(false)

    // Speak
    window.speechSynthesis.speak(utterance)
  }

  // Cleanup on unmount
  useEffect(() => {
    return () => {
      window.speechSynthesis.cancel()
    }
  }, [])

  return (
    <section className="relative min-h-screen flex items-center justify-center overflow-hidden">
      {/* Video/Image Background Placeholder */}
      <div className="absolute inset-0 gradient-purple-black">
        <div className="absolute inset-0 bg-[url('/futuristic-ai-vision-technology-wearable-device.jpg')] bg-cover bg-center opacity-20" />
        <div className="absolute inset-0 bg-gradient-to-b from-transparent via-background/50 to-background" />
      </div>

      {/* Animated Grid */}
      <div className="absolute inset-0 bg-[linear-gradient(to_right,oklch(0.25_0.15_285)_1px,transparent_1px),linear-gradient(to_bottom,oklch(0.25_0.15_285)_1px,transparent_1px)] bg-[size:4rem_4rem] [mask-image:radial-gradient(ellipse_60%_50%_at_50%_0%,#000_70%,transparent_100%)] opacity-20" />

      <div className="container relative z-10 mx-auto px-4 lg:px-8 pt-20">
        <div className="max-w-4xl mx-auto text-center">
          <div className="inline-block mb-4 px-4 py-2 rounded-full bg-primary/10 border border-primary/20">
            <span className="text-sm font-medium text-primary">AI-Powered Navigation • ESP32 Technology</span>
          </div>

          <h1 className="text-5xl md:text-7xl lg:text-8xl font-bold mb-6 leading-tight">
            <span className="text-balance">Empowering Vision Through</span>{' '}
            <span className="text-gradient">Intelligent Technology</span>
          </h1>

          <p className="text-lg md:text-xl text-muted-foreground mb-8 max-w-2xl mx-auto leading-relaxed">
            Revolutionary wearable device combining ESP32 camera modules with advanced AI object detection to provide real-time navigation assistance for the blind and visually impaired.
          </p>

          <div className="flex flex-col sm:flex-row items-center justify-center gap-4">
            <Button size="lg" className="bg-primary hover:bg-primary/90 text-primary-foreground px-8 group">
              Explore Technology
              <ArrowRight className="ml-2 w-4 h-4 transition-transform group-hover:translate-x-1" />
            </Button>
            <Button size="lg" variant="outline" className="px-8 group">
              <Play className="mr-2 w-4 h-4" />
              Watch Demo
            </Button>
            <Button 
              size="lg" 
              variant="secondary" 
              className="px-8 group"
              onClick={handleTextToSpeech}
            >
              {isSpeaking ? (
                <>
                  <VolumeX className="mr-2 w-4 h-4" />
                  Stop Reading
                </>
              ) : (
                <>
                  <Volume2 className="mr-2 w-4 h-4" />
                  Read Aloud
                </>
              )}
            </Button>
          </div>

          {/* Stats */}
          <div className="grid grid-cols-2 md:grid-cols-4 gap-8 mt-16 pt-16 border-t border-border/50">
            <div>
              <div className="text-3xl md:text-4xl font-bold text-gradient mb-2">AI</div>
              <div className="text-sm text-muted-foreground">Object Detection</div>
            </div>
            <div>
              <div className="text-3xl md:text-4xl font-bold text-gradient mb-2">Real-Time</div>
              <div className="text-sm text-muted-foreground">Distance Alerts</div>
            </div>
            <div>
              <div className="text-3xl md:text-4xl font-bold text-gradient mb-2">GPS</div>
              <div className="text-sm text-muted-foreground">Navigation</div>
            </div>
            <div>
              <div className="text-3xl md:text-4xl font-bold text-gradient mb-2">Audio</div>
              <div className="text-sm text-muted-foreground">Feedback</div>
            </div>
          </div>
        </div>
      </div>

      {/* Scroll Indicator */}
      <div className="absolute bottom-8 left-1/2 -translate-x-1/2 animate-bounce">
        <div className="w-6 h-10 border-2 border-primary/50 rounded-full flex items-start justify-center p-2">
          <div className="w-1 h-3 bg-primary rounded-full" />
        </div>
      </div>
    </section>
  )
}
