'use client'

import { Card } from '@/components/ui/card'
import { Heart, Users, Globe } from 'lucide-react'

export function MissionSection() {
  return (
    <section id="mission" className="py-24 relative overflow-hidden">
      <div className="absolute inset-0 bg-gradient-to-b from-background via-secondary/30 to-background" />
      
      <div className="container relative z-10 mx-auto px-4 lg:px-8">
        <div className="max-w-3xl mx-auto text-center mb-16">
          <h2 className="text-4xl md:text-5xl font-bold mb-6">
            Our <span className="text-gradient">Mission</span>
          </h2>
          <p className="text-lg text-muted-foreground leading-relaxed">
            We believe everyone deserves the freedom to navigate the world independently. VisionGuide is dedicated to empowering blind and visually impaired individuals with cutting-edge AI technology that transforms how they experience their surroundings.
          </p>
        </div>

        <div className="grid md:grid-cols-3 gap-8">
          <Card className="p-8 bg-card border-border hover:border-primary/50 transition-all group">
            <div className="relative mb-6">
              <Heart className="w-12 h-12 text-primary group-hover:scale-110 transition-transform" />
              <div className="absolute inset-0 bg-primary/20 blur-2xl rounded-full" />
            </div>
            <h3 className="text-xl font-bold mb-4">Accessibility First</h3>
            <p className="text-muted-foreground leading-relaxed">
              Designing technology that prioritizes the needs of visually impaired users, ensuring intuitive interaction through audio and haptic feedback.
            </p>
          </Card>

          <Card className="p-8 bg-card border-border hover:border-primary/50 transition-all group">
            <div className="relative mb-6">
              <Users className="w-12 h-12 text-primary group-hover:scale-110 transition-transform" />
              <div className="absolute inset-0 bg-primary/20 blur-2xl rounded-full" />
            </div>
            <h3 className="text-xl font-bold mb-4">Community Impact</h3>
            <p className="text-muted-foreground leading-relaxed">
              Supporting over 285 million visually impaired people worldwide with affordable, reliable navigation assistance technology.
            </p>
          </Card>

          <Card className="p-8 bg-card border-border hover:border-primary/50 transition-all group">
            <div className="relative mb-6">
              <Globe className="w-12 h-12 text-primary group-hover:scale-110 transition-transform" />
              <div className="absolute inset-0 bg-primary/20 blur-2xl rounded-full" />
            </div>
            <h3 className="text-xl font-bold mb-4">Global Innovation</h3>
            <p className="text-muted-foreground leading-relaxed">
              Leveraging AI and edge computing to create next-generation assistive devices that work anywhere, anytime.
            </p>
          </Card>
        </div>

        {/* Video Banner Placeholder */}
        <div className="mt-16 rounded-2xl overflow-hidden border border-border/50 relative group">
          <div className="aspect-video bg-secondary/50 flex items-center justify-center">
            <div className="absolute inset-0 bg-[url('/person-using-wearable-vision-assistance-device.jpg')] bg-cover bg-center opacity-40" />
            <div className="relative z-10 text-center">
              <div className="w-20 h-20 rounded-full bg-primary/20 backdrop-blur-sm border border-primary/50 flex items-center justify-center mx-auto mb-4 group-hover:scale-110 transition-transform cursor-pointer">
                <div className="w-0 h-0 border-l-[20px] border-l-primary border-t-[12px] border-t-transparent border-b-[12px] border-b-transparent ml-1" />
              </div>
              <p className="text-sm text-muted-foreground">Watch Our Mission Video</p>
            </div>
          </div>
        </div>
      </div>
    </section>
  )
}
