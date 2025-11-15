# Frontend Project Structure & Import Guide

## 📁 Complete Directory Tree

```
frontend/
├── app/                              # Next.js App Router (Server & Client)
│   ├── layout.tsx                   # Root layout wrapper
│   ├── page.tsx                     # Home page component
│   └── globals.css                  # Global Tailwind CSS
│
├── components/                       # Reusable React Components
│   ├── navigation.tsx               # Top navbar with scroll detection
│   ├── hero-section.tsx             # Landing hero section
│   ├── mission-section.tsx          # Mission statement section
│   ├── features-section.tsx         # Features showcase grid
│   ├── technology-section.tsx       # Technology stack display
│   ├── product-demo.tsx             # Interactive 3D demo
│   ├── cta-section.tsx              # Call-to-action section
│   ├── footer.tsx                   # Page footer
│   ├── theme-provider.tsx           # Dark mode theme provider
│   └── ui/                          # shadcn/ui component library
│       ├── button.tsx               # Button component
│       ├── card.tsx                 # Card component
│       ├── input.tsx                # Form input
│       ├── dialog.tsx               # Modal dialog
│       ├── form.tsx                 # React Hook Form wrapper
│       ├── select.tsx               # Dropdown select
│       ├── tabs.tsx                 # Tab component
│       └── [other ui components].tsx
│
├── lib/                              # Utility functions & constants
│   └── utils.ts                     # Class name utilities (cn function)
│
├── hooks/                            # Custom React Hooks
│   ├── use-mobile.ts                # Detect mobile viewport
│   └── use-toast.ts                 # Toast notification hook
│
├── styles/                           # Global styles
│   └── globals.css                  # Base CSS variables & Tailwind
│
├── public/                           # Static assets
│   ├── favicon.ico                  # Browser tab icon
│   ├── icon.svg                     # Icon for metadata
│   ├── apple-icon.png               # iOS home screen icon
│   ├── icon-light-32x32.png         # Light theme icon
│   ├── icon-dark-32x32.png          # Dark theme icon
│   └── [other static files]
│
├── Configuration Files
│   ├── package.json                 # Project dependencies & scripts
│   ├── pnpm-lock.yaml               # Dependency lock file
│   ├── tsconfig.json                # TypeScript configuration
│   ├── next.config.mjs              # Next.js configuration
│   ├── tailwind.config.ts           # Tailwind CSS configuration
│   ├── postcss.config.mjs           # PostCSS configuration
│   ├── components.json              # shadcn/ui configuration
│   └── .env.example                 # Environment variables template
│
├── Docker & Deployment
│   ├── Dockerfile                   # Production Docker image
│   ├── docker-compose.yml           # Multi-service compose file
│   ├── setup.sh                     # Linux setup script
│   ├── setup.bat                    # Windows setup script
│   └── DEPLOYMENT.md                # Deployment guide
│
├── Documentation
│   ├── README.md                    # Project overview
│   ├── DEPLOYMENT_CHECKLIST.md      # Pre-deployment verification
│   ├── .gitignore                   # Git ignore patterns
│   └── [this file]
│
└── Version Control
    └── .git/                        # Git repository
```

---

## 🔀 Import Paths & Aliases

### Import Configuration (tsconfig.json)

```json
{
  "compilerOptions": {
    "paths": {
      "@/*": ["./*"]
    }
  }
}
```

### Path Aliases (components.json)

```json
{
  "aliases": {
    "components": "@/components",
    "utils": "@/lib/utils",
    "ui": "@/components/ui",
    "lib": "@/lib",
    "hooks": "@/hooks"
  }
}
```

---

## ✅ Import Examples

### Correct Imports ✓

```typescript
// Component imports
import { Navigation } from '@/components/navigation'
import { HeroSection } from '@/components/hero-section'

// UI component imports
import { Button } from '@/components/ui/button'
import { Card, CardContent } from '@/components/ui/card'

// Utility imports
import { cn } from '@/lib/utils'

// Hook imports
import { useMobile } from '@/hooks/use-mobile'
import { useToast } from '@/hooks/use-toast'
```

### Incorrect Imports ✗

```typescript
// ❌ Don't use relative imports from root
import { Navigation } from './components/navigation'

// ❌ Don't import from sub-paths without alias
import Button from 'components/ui/button'

// ❌ Don't duplicate the @/ prefix
import { cn } from '@@/lib/utils'
```

---

## 📦 Component Structure Pattern

### Component File Template

```typescript
// components/example-component.tsx

'use client'  // Mark as client component if needed

import { useState } from 'react'
import { Button } from '@/components/ui/button'
import { cn } from '@/lib/utils'

interface ExampleComponentProps {
  title: string
  description?: string
  className?: string
}

export function ExampleComponent({
  title,
  description,
  className,
}: ExampleComponentProps) {
  const [state, setState] = useState(false)

  return (
    <div className={cn('space-y-4', className)}>
      <h2 className="text-2xl font-bold">{title}</h2>
      {description && <p className="text-muted-foreground">{description}</p>}
      <Button onClick={() => setState(!state)}>
        {state ? 'Hide' : 'Show'} Details
      </Button>
    </div>
  )
}
```

---

## 🎯 Module Dependencies Map

```
app/page.tsx (Home)
├── Navigation (component)
├── HeroSection (component)
├── MissionSection (component)
├── FeaturesSection (component)
├── TechnologySection (component)
├── ProductDemo (component)
│   └── Three.js (3D rendering)
├── CTASection (component)
└── Footer (component)

Navigation
├── Button (ui component)
├── useMobile (hook)
└── Link (Next.js)

Common UI Components
├── Button
├── Card
├── Input
├── Dialog
├── Form
├── Select
├── Tabs
└── [other shadcn components]

Utilities
├── cn() - Class merge utility
├── useMobile() - Viewport detection
└── useToast() - Notification system
```

---

## 🏗️ Directory Organization Rules

### `/app` - Next.js Pages & Layout
- Contains `layout.tsx` (required)
- Contains `page.tsx` (required)
- Server components by default
- Use `'use client'` for interactive components

### `/components` - Reusable UI Components
- Pure React components
- Can be server or client components
- Organized by feature/page
- `ui/` subdirectory for shadcn components

### `/lib` - Utility Functions
- Pure functions, no React dependencies
- Constants and helper functions
- Always sync/non-async

### `/hooks` - Custom React Hooks
- React-specific utilities
- Can use state, effects, other hooks
- Reusable logic extraction

### `/public` - Static Assets
- Images, icons, fonts
- Served at root path
- Immutable files only

---

## 📋 Dependency Management

### Adding New Dependencies

```bash
# Install package
pnpm add package-name

# Install dev dependency
pnpm add -D package-name

# Install shadcn component
pnpm dlx shadcn-ui@latest add component-name
```

### Current Major Dependencies

| Package | Version | Purpose |
|---------|---------|---------|
| next | 16.0.3 | React framework |
| react | 19.2.0 | UI library |
| typescript | ^5 | Type checking |
| tailwindcss | ^4.1.9 | Styling |
| @radix-ui/* | Latest | UI components |
| react-hook-form | ^7.60.0 | Form management |
| zod | 3.25.76 | Validation |
| three | latest | 3D graphics |

---

## 🔧 TypeScript Path Resolution

### File: tsconfig.json

```json
{
  "compilerOptions": {
    "paths": {
      "@/*": ["./*"]
    }
  }
}
```

This makes `@/` resolve to the project root directory.

### Example Resolution

```
@/components/ui/button
↓
./components/ui/button.tsx
```

---

## 📱 Component Organization by Feature

### Landing Page Components
```
components/
├── hero-section.tsx
├── mission-section.tsx
├── features-section.tsx
├── technology-section.tsx
├── product-demo.tsx
├── cta-section.tsx
└── footer.tsx
```

### Layout Components
```
components/
├── navigation.tsx
├── footer.tsx
└── theme-provider.tsx
```

### UI Library
```
components/ui/
├── button.tsx
├── card.tsx
├── input.tsx
├── dialog.tsx
└── [many more...]
```

---

## 🚀 Build Process

### Development Build
```bash
pnpm dev
```
- Fast reload
- Source maps enabled
- No optimization

### Production Build
```bash
pnpm build
```
1. TypeScript compilation
2. Component optimization
3. Code splitting
4. Image optimization
5. CSS minification
6. Bundle analysis

### Output
```
.next/
├── static/chunks/  # JavaScript bundles
├── static/css/     # CSS files
├── static/media/   # Images/fonts
└── server/         # Server-side code
```

---

## 🔍 Verification Checklist

Before deployment, verify:

- [ ] All imports use `@/` path aliases
- [ ] No circular dependencies
- [ ] TypeScript compilation clean: `pnpm tsc --noEmit`
- [ ] Build succeeds: `pnpm build`
- [ ] No broken imports in components
- [ ] Environment variables configured
- [ ] Components render without errors
- [ ] Navigation works correctly
- [ ] Responsive design verified

---

## 📚 Key Files to Modify

### To Add New Page
1. Create file in `app/`
2. Export default component
3. Use in navigation

### To Add New Component
1. Create file in `components/`
2. Use `'use client'` if interactive
3. Import in parent component

### To Add New UI Element
1. Run: `pnpm dlx shadcn-ui@latest add element-name`
2. Import from `@/components/ui/element-name`
3. Use in components

### To Update Styles
1. Modify `app/globals.css`
2. Or use Tailwind classes in components

---

## 🔗 File Dependencies Graph

```
app/layout.tsx (Root)
├── analytics (Vercel)
└── globals.css (Tailwind)

app/page.tsx (Home)
├── Navigation
│   ├── useMobile hook
│   └── Button UI
├── HeroSection
│   └── Button UI
├── MissionSection
│   └── Card UI
├── FeaturesSection
│   └── Card UI
├── TechnologySection
├── ProductDemo
│   └── Three.js
├── CTASection
│   └── Button UI
└── Footer

All components use:
├── Tailwind CSS classes
├── cn() utility
└── shadcn/ui components
```

---

## 🎨 Styling System

### CSS Variable Hierarchy

```
globals.css (CSS Variables)
    ↓
tailwind.config.ts (Tailwind Theme)
    ↓
app/globals.css (Utility Classes)
    ↓
Component Classes
```

### Available CSS Variables

```css
--background
--foreground
--card / --card-foreground
--primary / --primary-foreground
--secondary / --secondary-foreground
--accent / --accent-foreground
--muted / --muted-foreground
--destructive / --destructive-foreground
--border
--input
--ring
```

---

## ✨ Best Practices

1. **Always use `@/` aliases** for imports
2. **Keep components small** (< 200 lines)
3. **Use TypeScript** for type safety
4. **Follow naming conventions**: PascalCase for components, camelCase for functions
5. **Use shadcn/ui** for consistent styling
6. **Add `'use client'`** only when needed
7. **Avoid prop drilling** - use context for global state
8. **Memoize expensive components** with `React.memo()`

---

**Project Structure v1.0 - Updated November 2024**
