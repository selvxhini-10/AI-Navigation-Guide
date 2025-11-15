# Frontend Web Application - VisionGuide

This is the Next.js web application frontend for the Smart Navigation Cane (SNC) system, rebranded as **VisionGuide** - AI Navigation for the Visually Impaired.

## 🚀 Project Setup

### Prerequisites
- Node.js 18+ 
- pnpm 8+ (or npm/yarn)

### Installation

```bash
# Install dependencies
pnpm install

# Copy environment template
cp .env.example .env.local

# Update .env.local with your backend URL
# NEXT_PUBLIC_API_BASE_URL=http://localhost:8000
```

### Development

```bash
# Start development server (port 3000)
pnpm dev

# Open in browser
open http://localhost:3000
```

### Production Build

```bash
# Build for production
pnpm build

# Start production server
pnpm start
```

## 📁 Project Structure

```
frontend/
├── app/                          # Next.js App Router
│   ├── layout.tsx               # Root layout with metadata
│   ├── page.tsx                 # Home page
│   └── globals.css              # Global styles
├── components/                   # React components
│   ├── navigation.tsx           # Top navigation bar
│   ├── hero-section.tsx         # Hero/landing section
│   ├── mission-section.tsx      # Mission statement
│   ├── features-section.tsx     # Feature showcase
│   ├── technology-section.tsx   # Technology stack
│   ├── product-demo.tsx         # Interactive demo
│   ├── cta-section.tsx          # Call-to-action
│   ├── footer.tsx               # Footer
│   ├── theme-provider.tsx       # Theme context
│   └── ui/                      # shadcn/ui components
├── lib/
│   └── utils.ts                 # Utility functions (cn)
├── hooks/                        # Custom React hooks
│   ├── use-mobile.ts            # Mobile detection
│   └── use-toast.ts             # Toast notifications
├── styles/
│   └── globals.css              # Global CSS with Tailwind
├── public/                       # Static assets
├── package.json                 # Dependencies
├── tsconfig.json                # TypeScript configuration
├── next.config.mjs              # Next.js configuration
├── tailwind.config.ts           # Tailwind CSS config
├── postcss.config.mjs           # PostCSS configuration
├── components.json              # shadcn/ui configuration
└── .env.example                 # Environment template
```

## 🎨 Technology Stack

- **Framework**: Next.js 16.0.3 (React 19.2.0)
- **Language**: TypeScript 5.x
- **Styling**: Tailwind CSS 4.1.9 + PostCSS
- **UI Components**: shadcn/ui (Radix UI based)
- **Forms**: React Hook Form + Zod validation
- **Animations**: Tailwind CSS Animate
- **Charts**: Recharts
- **3D Graphics**: Three.js + React Three Fiber (for demos)
- **Package Manager**: pnpm (recommended)

## 🔧 Configuration Files

### `.env.local`
Create from `.env.example` and configure:
- `NEXT_PUBLIC_API_BASE_URL` - Backend API endpoint
- `NEXT_PUBLIC_ENABLE_DEMO_MODE` - Toggle demo mode

### `next.config.mjs`
- TypeScript build errors ignored (set to false for stricter builds)
- Image optimization disabled for static hosting

### `tailwind.config.ts`
- Custom color theme (dark purple/blue palette)
- Design tokens for consistent styling

### `tsconfig.json`
- Path aliases: `@/*` points to root directory
- Strict mode enabled
- Module resolution: bundler

## 📦 Key Dependencies

### UI & Components
- `@radix-ui/*` - Accessible UI components
- `lucide-react` - Icon library
- `clsx` + `tailwind-merge` - Class name utilities

### Forms & Data
- `react-hook-form` - Form state management
- `zod` - Data validation
- `date-fns` - Date utilities

### 3D & Visualization
- `three` - 3D graphics
- `@react-three/fiber` - React renderer for Three.js
- `@react-three/drei` - Helpers for Three.js
- `recharts` - Data visualization

### Other
- `next-themes` - Dark mode support
- `sonner` - Toast notifications
- `@vercel/analytics` - Usage analytics

## 🚀 Deployment

### Vercel (Recommended)
```bash
# Connect GitHub repo to Vercel
# Environment variables: Set NEXT_PUBLIC_API_BASE_URL to production backend

vercel deploy
```

### Docker
```dockerfile
FROM node:20-alpine AS builder
WORKDIR /app
COPY package.json pnpm-lock.yaml ./
RUN pnpm install
COPY . .
RUN pnpm build

FROM node:20-alpine
WORKDIR /app
COPY --from=builder /app/.next ./.next
COPY --from=builder /app/node_modules ./node_modules
COPY package.json next.config.mjs ./
EXPOSE 3000
CMD ["pnpm", "start"]
```

### Static Export (AWS S3, Netlify, etc.)
```bash
# Update next.config.mjs
# export const output = "export"

pnpm build
# Output in: out/
```

## 🔌 Backend Integration

The frontend communicates with the SNC Backend API:

```typescript
// API calls example
const API_BASE = process.env.NEXT_PUBLIC_API_BASE_URL

// Get latest detections
fetch(`${API_BASE}/api/detection/latest`)

// Start navigation
fetch(`${API_BASE}/api/navigation/start-route`, {
  method: 'POST',
  body: JSON.stringify({ origin: [lat1, lon1], destination: [lat2, lon2] })
})

// Register device
fetch(`${API_BASE}/api/device/register`, {
  method: 'POST',
  body: JSON.stringify({ device_id: 'esp32_001', device_type: 'esp32cam' })
})
```

## 🧪 Development Workflow

### Hot Reload
Development server includes hot reload - changes are reflected instantly.

### Linting
```bash
pnpm lint
```

### Type Checking
TypeScript checks run during development and build.

### Component Development
1. Create component in `components/`
2. Use shadcn/ui components for consistency
3. Follow Tailwind CSS utility-first approach
4. Test with TypeScript strict mode

## 📱 Responsive Design

- Mobile-first approach using Tailwind CSS
- Breakpoints: `sm` (640px), `md` (768px), `lg` (1024px), `xl` (1280px)
- Tested on iOS (Safari) and Android (Chrome)

## ♿ Accessibility

- Semantic HTML
- ARIA labels on interactive elements
- Keyboard navigation support
- Screen reader friendly
- Color contrast compliance

## 🔒 Security

- CSP headers configured
- No sensitive data in client-side code
- Environment variables for secrets
- Input validation with Zod
- XSS protection built-in

## 📊 Monitoring & Analytics

- Vercel Analytics integrated (optional)
- PostHog integration ready (optional)
- Error tracking ready for implementation

## 🐛 Troubleshooting

| Issue | Solution |
|-------|----------|
| Build fails | `pnpm install --force` then `pnpm build` |
| Port 3000 in use | `lsof -i :3000` (macOS) or `netstat -ano \| findstr :3000` (Windows) |
| Backend not found | Check `NEXT_PUBLIC_API_BASE_URL` in `.env.local` |
| Styles not loading | Clear `.next` folder: `rm -rf .next` |
| TypeScript errors | Run `pnpm tsc --noEmit` to check |

## 📚 Resources

- [Next.js Documentation](https://nextjs.org/docs)
- [Tailwind CSS Documentation](https://tailwindcss.com/docs)
- [shadcn/ui Components](https://ui.shadcn.com/)
- [React Documentation](https://react.dev)
- [TypeScript Handbook](https://www.typescriptlang.org/docs/)

## 📝 Component Documentation

### Navigation Component
- Responsive navbar with scroll detection
- Mobile hamburger menu
- Links to main sections

### Hero Section
- Eye-catching introduction
- Primary CTA button
- Background imagery

### Features Section
- Feature cards with icons
- Grid layout (responsive)
- Icons from lucide-react

### Technology Section
- Tech stack showcase
- Architecture diagram ready
- Integration points

### Product Demo
- Interactive 3D visualization (Three.js)
- Feature walkthrough
- Live demo controls

### CTA Section
- Secondary call-to-action
- Download/signup prompts
- Contact information

## 🔄 CI/CD Pipeline

Recommended GitHub Actions workflow:
```yaml
name: Deploy
on: [push]
jobs:
  build-and-deploy:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - uses: pnpm/action-setup@v2
      - uses: actions/setup-node@v3
        with:
          node-version: '20'
          cache: 'pnpm'
      - run: pnpm install
      - run: pnpm lint
      - run: pnpm build
      - name: Deploy
        run: vercel deploy --prod
```

## 📄 License

Part of the Smart Navigation Cane (SNC) project - VisionGuide initiative

---

**Last Updated**: November 2024
**Current Version**: 1.0.0
