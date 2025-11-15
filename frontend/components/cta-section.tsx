'use client'

import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input'
import { ArrowRight } from 'lucide-react'

export function CTASection() {
  return (
    <section className="py-24 relative overflow-hidden">
      <div className="absolute inset-0 gradient-accent opacity-10" />
      <div className="absolute inset-0 bg-[radial-gradient(circle_at_center,oklch(0.25_0.15_285)_0%,transparent_70%)]" />
      
      <div className="container relative z-10 mx-auto px-4 lg:px-8">
        <div className="max-w-3xl mx-auto text-center">
          <h2 className="text-4xl md:text-5xl font-bold mb-6">
            Ready to Experience the <span className="text-gradient">Future</span>?
          </h2>
          <p className="text-lg text-muted-foreground mb-8 leading-relaxed">
            Join our early access program and be among the first to receive VisionGuide. Help us shape the future of assistive technology.
          </p>

          <div className="flex flex-col sm:flex-row gap-4 max-w-md mx-auto mb-8">
            <Input 
              type="email" 
              placeholder="Enter your email" 
              className="bg-secondary border-border"
            />
            <Button className="bg-primary hover:bg-primary/90 text-primary-foreground whitespace-nowrap group">
              Get Early Access
              <ArrowRight className="ml-2 w-4 h-4 transition-transform group-hover:translate-x-1" />
            </Button>
          </div>

          <p className="text-sm text-muted-foreground">
            Join 5,000+ individuals on the waitlist • Expected Q3 2025 launch
          </p>
        </div>
      </div>
    </section>
  )
}
