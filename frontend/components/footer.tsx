import { Eye } from 'lucide-react'
import Link from 'next/link'

export function Footer() {
  const links = {
    product: ['Features', 'Technology', 'Pricing', 'Roadmap'],
    company: ['About', 'Mission', 'Team', 'Careers'],
    resources: ['Documentation', 'Support', 'Blog', 'Community'],
    legal: ['Privacy', 'Terms', 'Accessibility', 'Licenses']
  }

  return (
    <footer className="border-t border-border bg-secondary/20">
      <div className="container mx-auto px-4 lg:px-8 py-16">
        <div className="grid md:grid-cols-2 lg:grid-cols-6 gap-12 mb-12">
          <div className="lg:col-span-2">
            <Link href="/" className="flex items-center gap-2 mb-4">
              <Eye className="w-8 h-8 text-primary" />
              <span className="text-xl font-bold text-gradient">VisionGuide</span>
            </Link>
            <p className="text-sm text-muted-foreground leading-relaxed max-w-xs">
              Empowering independence through intelligent navigation technology for the visually impaired.
            </p>
          </div>

          {Object.entries(links).map(([category, items]) => (
            <div key={category}>
              <h3 className="font-bold text-sm uppercase mb-4 text-foreground">
                {category}
              </h3>
              <ul className="space-y-2">
                {items.map((item) => (
                  <li key={item}>
                    <Link 
                      href="#" 
                      className="text-sm text-muted-foreground hover:text-foreground transition-colors"
                    >
                      {item}
                    </Link>
                  </li>
                ))}
              </ul>
            </div>
          ))}
        </div>

        <div className="pt-8 border-t border-border flex flex-col md:flex-row justify-between items-center gap-4">
          <p className="text-sm text-muted-foreground">
            © 2025 VisionGuide. All rights reserved.
          </p>
          <div className="flex items-center gap-6">
            <Link href="#" className="text-sm text-muted-foreground hover:text-foreground transition-colors">
              Twitter
            </Link>
            <Link href="#" className="text-sm text-muted-foreground hover:text-foreground transition-colors">
              LinkedIn
            </Link>
            <Link href="#" className="text-sm text-muted-foreground hover:text-foreground transition-colors">
              GitHub
            </Link>
          </div>
        </div>
      </div>
    </footer>
  )
}
