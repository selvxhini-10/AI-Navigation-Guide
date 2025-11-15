'use client'

import { Card } from '@/components/ui/card'
import { Camera, Cpu, Volume2 } from 'lucide-react'

export function TechnologySection() {
  const techStack = [
    { category: 'Camera Hardware', items: ['ESP32-CAM Module', 'Raspberry Pi Camera v2', 'OV2640 Sensor'] },
    { category: 'Processing Units', items: ['ESP32 Microcontroller', 'Raspberry Pi 4', 'NVIDIA Jetson Nano'] },
    { category: 'AI Models', items: ['YOLOv8', 'MobileNet SSD', 'Edge-optimized TensorFlow Lite'] },
    { category: 'Connectivity', items: ['Bluetooth 5.0', 'Wi-Fi', 'GPS Module'] }
  ]

  return (
    <section id="technology" className="py-24 relative overflow-hidden">
      <div className="absolute inset-0 bg-gradient-to-b from-background via-secondary/20 to-background" />
      
      <div className="container relative z-10 mx-auto px-4 lg:px-8">
        <div className="max-w-3xl mx-auto text-center mb-16">
          <h2 className="text-4xl md:text-5xl font-bold mb-6">
            Technology <span className="text-gradient">Stack</span>
          </h2>
          <p className="text-lg text-muted-foreground leading-relaxed">
            Built with industry-leading components and AI frameworks to deliver reliable, real-time performance.
          </p>
        </div>

        <div className="grid md:grid-cols-2 lg:grid-cols-4 gap-6 mb-16">
          {techStack.map((tech, index) => (
            <Card key={index} className="p-6 bg-card border-border hover:border-primary/50 transition-all">
              <h3 className="text-lg font-bold mb-4 text-primary">{tech.category}</h3>
              <ul className="space-y-2">
                {tech.items.map((item, idx) => (
                  <li key={idx} className="text-sm text-muted-foreground flex items-start gap-2">
                    <span className="text-primary mt-1">•</span>
                    {item}
                  </li>
                ))}
              </ul>
            </Card>
          ))}
        </div>

        {/* Architecture Diagram Placeholder */}
        <Card className="p-8 bg-card border-border">
          <h3 className="text-2xl font-bold mb-6 text-center">System Architecture</h3>
          <div className="aspect-video bg-secondary/30 rounded-lg flex items-center justify-center relative overflow-hidden">
            <div className="absolute inset-0 bg-[url('/technical-diagram-esp32-ai-system-architecture.jpg')] bg-cover bg-center opacity-40" />
            <div className="relative z-10 grid grid-cols-3 gap-8 max-w-4xl mx-auto">
              <div className="text-center">
                <div className="w-20 h-20 rounded-lg bg-primary/20 border border-primary/50 backdrop-blur-sm flex items-center justify-center mx-auto mb-3">
                  <Camera className="w-10 h-10 text-primary" />
                </div>
                <p className="text-sm font-medium">Camera Input</p>
              </div>
              <div className="text-center">
                <div className="w-20 h-20 rounded-lg bg-primary/20 border border-primary/50 backdrop-blur-sm flex items-center justify-center mx-auto mb-3">
                  <Cpu className="w-10 h-10 text-primary" />
                </div>
                <p className="text-sm font-medium">AI Processing</p>
              </div>
              <div className="text-center">
                <div className="w-20 h-20 rounded-lg bg-primary/20 border border-primary/50 backdrop-blur-sm flex items-center justify-center mx-auto mb-3">
                  <Volume2 className="w-10 h-10 text-primary" />
                </div>
                <p className="text-sm font-medium">Audio Output</p>
              </div>
            </div>
          </div>
        </Card>
      </div>
    </section>
  )
}
