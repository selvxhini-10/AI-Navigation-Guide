'use client'

import { Suspense } from 'react'
import { Canvas } from '@react-three/fiber'
import { OrbitControls, Environment, Float } from '@react-three/drei'
import { Card } from '@/components/ui/card'

function WearableDevice() {
  return (
    <Float speed={2} rotationIntensity={0.5} floatIntensity={0.5}>
      <group>
        {/* Main body */}
        <mesh position={[0, 0, 0]}>
          <boxGeometry args={[2, 0.3, 1]} />
          <meshStandardMaterial color="#7c3aed" metalness={0.8} roughness={0.2} />
        </mesh>
        
        {/* Camera module */}
        <mesh position={[-0.6, 0.2, 0.5]}>
          <cylinderGeometry args={[0.15, 0.15, 0.3]} />
          <meshStandardMaterial color="#1e1b4b" metalness={0.9} roughness={0.1} />
        </mesh>
        <mesh position={[-0.6, 0.35, 0.5]}>
          <sphereGeometry args={[0.12]} />
          <meshStandardMaterial color="#3b82f6" metalness={0.5} roughness={0.3} emissive="#3b82f6" emissiveIntensity={0.5} />
        </mesh>
        
        {/* ESP32 chip */}
        <mesh position={[0.3, 0.2, 0]}>
          <boxGeometry args={[0.5, 0.05, 0.5]} />
          <meshStandardMaterial color="#1e293b" metalness={0.7} roughness={0.3} />
        </mesh>
        
        {/* Processor indicator */}
        <mesh position={[0.7, 0.2, 0]}>
          <boxGeometry args={[0.3, 0.08, 0.3]} />
          <meshStandardMaterial color="#22d3ee" metalness={0.6} roughness={0.2} emissive="#22d3ee" emissiveIntensity={0.3} />
        </mesh>
        
        {/* Connection points */}
        {[-0.8, -0.4, 0, 0.4, 0.8].map((x, i) => (
          <mesh key={i} position={[x, 0, -0.5]}>
            <cylinderGeometry args={[0.05, 0.05, 0.1]} />
            <meshStandardMaterial color="#fbbf24" metalness={0.9} roughness={0.1} />
          </mesh>
        ))}
        
        {/* Glow effect */}
        <pointLight position={[-0.6, 0.5, 0.5]} intensity={2} color="#7c3aed" distance={3} />
        <pointLight position={[0.7, 0.3, 0]} intensity={1.5} color="#22d3ee" distance={2} />
      </group>
    </Float>
  )
}

function Scene3D() {
  return (
    <group>
      <WearableDevice />
    </group>
  )
}

export function ProductDemo() {
  return (
    <section id="demo" className="py-24 relative">
      <div className="container mx-auto px-4 lg:px-8">
        <div className="max-w-3xl mx-auto text-center mb-16">
          <h2 className="text-4xl md:text-5xl font-bold mb-6">
            Interactive <span className="text-gradient">3D Model</span>
          </h2>
          <p className="text-lg text-muted-foreground leading-relaxed">
            Explore our wearable device in 3D. Drag to rotate, scroll to zoom, and discover the hardware that powers intelligent navigation.
          </p>
        </div>

        <Card className="p-4 bg-card border-border overflow-hidden">
          <div className="w-full h-[600px] rounded-lg overflow-hidden bg-secondary/30">
            <Canvas camera={{ position: [4, 2, 4], fov: 50 }}>
              <Suspense fallback={null}>
                <ambientLight intensity={0.5} />
                <spotLight position={[10, 10, 10]} angle={0.15} penumbra={1} intensity={1} />
                <spotLight position={[-10, -10, -10]} angle={0.15} penumbra={1} intensity={0.5} />
                <Environment preset="city" />
                <Scene3D />
                <OrbitControls 
                  enablePan={false} 
                  minDistance={3} 
                  maxDistance={8}
                  autoRotate
                  autoRotateSpeed={0.5}
                />
              </Suspense>
            </Canvas>
          </div>
        </Card>

        {/* Object Detection Demo */}
        <div className="grid md:grid-cols-2 gap-8 mt-16">
          <Card className="p-6 bg-card border-border">
            <h3 className="text-xl font-bold mb-4">Real-Time Object Detection</h3>
            <div className="aspect-video bg-secondary/30 rounded-lg mb-4 relative overflow-hidden">
              <div className="absolute inset-0 bg-[url('/ai-object-detection-bounding-boxes.jpg')] bg-cover bg-center opacity-50" />
              <div className="absolute top-4 left-4 px-3 py-1 rounded bg-primary/90 backdrop-blur-sm">
                <p className="text-xs font-mono text-primary-foreground">Person - 2.5m</p>
              </div>
              <div className="absolute top-16 left-4 px-3 py-1 rounded bg-primary/90 backdrop-blur-sm">
                <p className="text-xs font-mono text-primary-foreground">Door - 3.8m</p>
              </div>
              <div className="absolute top-28 right-4 px-3 py-1 rounded bg-primary/90 backdrop-blur-sm">
                <p className="text-xs font-mono text-primary-foreground">Chair - 1.2m</p>
              </div>
            </div>
            <p className="text-sm text-muted-foreground leading-relaxed">
              AI identifies objects in real-time with precise distance measurements, providing instant spatial awareness.
            </p>
          </Card>

          <Card className="p-6 bg-card border-border">
            <h3 className="text-xl font-bold mb-4">Navigation Interface</h3>
            <div className="aspect-video bg-secondary/30 rounded-lg mb-4 relative overflow-hidden">
              <div className="absolute inset-0 bg-[url('/gps-navigation-route-map.jpg')] bg-cover bg-center opacity-50" />
              <div className="absolute inset-0 flex items-center justify-center">
                <div className="bg-card/90 backdrop-blur-sm rounded-lg p-4 max-w-xs border border-border">
                  <p className="text-sm font-medium mb-2">Turn right in 50 meters</p>
                  <div className="flex items-center gap-2 text-xs text-muted-foreground">
                    <div className="w-2 h-2 rounded-full bg-primary animate-pulse" />
                    <span>Active Navigation</span>
                  </div>
                </div>
              </div>
            </div>
            <p className="text-sm text-muted-foreground leading-relaxed">
              Companion app provides turn-by-turn directions with audio guidance synced to real-time location data.
            </p>
          </Card>
        </div>
      </div>
    </section>
  )
}
