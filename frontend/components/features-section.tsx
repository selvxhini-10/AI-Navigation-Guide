'use client'

import { Card } from '@/components/ui/card'
import { Camera, Cpu, Navigation2, Volume2, Vibrate, Map } from 'lucide-react'

export function FeaturesSection() {
  const features = [
    {
      icon: Camera,
      title: 'ESP32-Cam Module',
      description: 'High-resolution camera captures real-time visual data for instant object recognition and environment analysis.',
      spec: 'ESP32-CAM / Raspberry Pi Camera'
    },
    {
      icon: Cpu,
      title: 'Edge AI Processing',
      description: 'Powerful embedded processors run YOLO and MobileNet models directly on device for zero-latency detection.',
      spec: 'Raspberry Pi / ESP32 / Jetson Nano'
    },
    {
      icon: Navigation2,
      title: 'Smart Object Detection',
      description: 'AI-powered recognition identifies obstacles, people, and objects with precise distance measurements in real-time.',
      spec: 'YOLO / MobileNet / Edge-tuned models'
    },
    {
      icon: Map,
      title: 'Google Maps Integration',
      description: 'Seamless navigation through companion mobile app with turn-by-turn directions and location awareness.',
      spec: 'GPS + Google Maps API'
    },
    {
      icon: Volume2,
      title: 'Audio Feedback',
      description: 'Crystal-clear voice guidance through Bluetooth earbuds provides spatial awareness and navigation instructions.',
      spec: 'Bluetooth 5.0 Connectivity'
    },
    {
      icon: Vibrate,
      title: 'Haptic Warnings',
      description: 'Tactile vibration patterns alert users to obstacles and navigation cues for intuitive understanding.',
      spec: 'Multi-pattern Haptic Engine'
    }
  ]

  return (
    <section id="features" className="py-24 relative">
      <div className="container mx-auto px-4 lg:px-8">
        <div className="max-w-3xl mx-auto text-center mb-16">
          <h2 className="text-4xl md:text-5xl font-bold mb-6">
            Advanced <span className="text-gradient">Features</span>
          </h2>
          <p className="text-lg text-muted-foreground leading-relaxed">
            VisionGuide combines cutting-edge hardware and AI to deliver comprehensive navigation assistance that adapts to your environment.
          </p>
        </div>

        <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-6">
          {features.map((feature, index) => (
            <Card 
              key={index} 
              className="p-6 bg-card border-border hover:border-primary/50 transition-all group relative overflow-hidden"
            >
              <div className="absolute top-0 right-0 w-32 h-32 bg-primary/5 rounded-full blur-3xl -translate-y-16 translate-x-16 group-hover:scale-150 transition-transform" />
              
              <div className="relative mb-4">
                <feature.icon className="w-10 h-10 text-primary group-hover:scale-110 transition-transform" />
              </div>
              
              <h3 className="text-xl font-bold mb-2">{feature.title}</h3>
              <p className="text-muted-foreground text-sm mb-4 leading-relaxed">{feature.description}</p>
              
              <div className="inline-block px-3 py-1 rounded-full bg-primary/10 border border-primary/20">
                <span className="text-xs font-medium text-primary">{feature.spec}</span>
              </div>
            </Card>
          ))}
        </div>

        {/* Video Banner */}
        <div className="mt-16 rounded-2xl overflow-hidden border border-border/50 relative group">
          <div className="aspect-video bg-secondary/50 flex items-center justify-center">
            <div className="absolute inset-0 bg-[url('/ai-object-detection-real-time-camera-feed.jpg')] bg-cover bg-center opacity-30" />
            <div className="relative z-10 text-center">
              <div className="w-20 h-20 rounded-full bg-primary/20 backdrop-blur-sm border border-primary/50 flex items-center justify-center mx-auto mb-4 group-hover:scale-110 transition-transform cursor-pointer">
                <div className="w-0 h-0 border-l-[20px] border-l-primary border-t-[12px] border-t-transparent border-b-[12px] border-b-transparent ml-1" />
              </div>
              <p className="text-sm text-muted-foreground">See Features in Action</p>
            </div>
          </div>
        </div>
      </div>
    </section>
  )
}
