import { Navigation } from '@/components/navigation'
import { HeroSection } from '@/components/hero-section'
import { MissionSection } from '@/components/mission-section'
import { FeaturesSection } from '@/components/features-section'
import { TechnologySection } from '@/components/technology-section'
import { ProductDemo } from '@/components/product-demo'
import { LiveObjectDetection } from '@/components/live-object-detection'
import { CTASection } from '@/components/cta-section'
import { Footer } from '@/components/footer'

export default function Home() {
  return (
    <div className="min-h-screen bg-background">
      <Navigation />
      <main>
        <HeroSection />
        <MissionSection />
        <FeaturesSection />
        <TechnologySection />
        <ProductDemo />
        <LiveObjectDetection />
        <CTASection />
      </main>
      <Footer />
    </div>
  )
}
