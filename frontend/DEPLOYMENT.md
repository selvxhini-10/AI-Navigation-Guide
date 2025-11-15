# Frontend Deployment Guide

## 🎯 Deployment Overview

VisionGuide frontend is a Next.js 16 application that can be deployed to multiple platforms. This guide covers the most popular deployment options.

## 🚀 Pre-Deployment Checklist

- [ ] All dependencies installed: `pnpm install`
- [ ] Project builds successfully: `pnpm build`
- [ ] Linting passes: `pnpm lint` (optional)
- [ ] Environment variables configured
- [ ] Backend API URL verified
- [ ] TypeScript strict mode passes
- [ ] No console errors or warnings

## 1️⃣ Vercel (Recommended - Easiest)

### Setup

```bash
# Install Vercel CLI
npm i -g vercel

# Deploy (first time)
vercel

# Follow prompts:
# - Link to GitHub repo (recommended)
# - Set project name
# - Framework: Next.js
# - Source directory: frontend/
```

### Environment Variables

In Vercel Dashboard:
1. Go to Settings → Environment Variables
2. Add:
   ```
   NEXT_PUBLIC_API_BASE_URL=https://your-backend-api.com
   NEXT_PUBLIC_APP_VERSION=1.0.0
   ```

### Continuous Deployment

```bash
# After GitHub setup, deploy automatically on push
git push origin main
```

### Production URL
```
https://your-project.vercel.app
```

---

## 2️⃣ Docker + AWS ECS

### Build Docker Image

```bash
# Build image
docker build -t vision-guide:latest .

# Test locally
docker run -p 3000:3000 \
  -e NEXT_PUBLIC_API_BASE_URL=http://localhost:8000 \
  vision-guide:latest
```

### Push to AWS ECR

```bash
# Create ECR repository
aws ecr create-repository --repository-name vision-guide

# Get login token
aws ecr get-login-password --region us-east-1 | \
  docker login --username AWS --password-stdin \
  123456789.dkr.ecr.us-east-1.amazonaws.com

# Tag image
docker tag vision-guide:latest \
  123456789.dkr.ecr.us-east-1.amazonaws.com/vision-guide:latest

# Push image
docker push 123456789.dkr.ecr.us-east-1.amazonaws.com/vision-guide:latest
```

### Deploy to ECS

```bash
# Create ECS task definition (JSON)
aws ecs register-task-definition --cli-input-json file://task-definition.json

# Create/update service
aws ecs create-service --cluster default --service-name vision-guide \
  --task-definition vision-guide:1 --desired-count 2
```

### ECS Task Definition Example
```json
{
  "family": "vision-guide",
  "networkMode": "awsvpc",
  "requiresCompatibilities": ["FARGATE"],
  "cpu": "256",
  "memory": "512",
  "containerDefinitions": [
    {
      "name": "vision-guide",
      "image": "123456789.dkr.ecr.us-east-1.amazonaws.com/vision-guide:latest",
      "portMappings": [
        {
          "containerPort": 3000,
          "hostPort": 3000,
          "protocol": "tcp"
        }
      ],
      "environment": [
        {
          "name": "NEXT_PUBLIC_API_BASE_URL",
          "value": "https://api.yourdomain.com"
        },
        {
          "name": "NODE_ENV",
          "value": "production"
        }
      ],
      "logConfiguration": {
        "logDriver": "awslogs",
        "options": {
          "awslogs-group": "/ecs/vision-guide",
          "awslogs-region": "us-east-1",
          "awslogs-stream-prefix": "ecs"
        }
      }
    }
  ]
}
```

---

## 3️⃣ Docker Compose (Local/Self-Hosted)

### Single Command Deploy

```bash
# From frontend directory
docker-compose up -d

# View logs
docker-compose logs -f frontend

# Stop
docker-compose down
```

### Access
- Frontend: http://localhost:3000
- Backend: http://localhost:8000

---

## 4️⃣ Azure App Service

### Prerequisites
```bash
# Install Azure CLI
# https://docs.microsoft.com/en-us/cli/azure/install-azure-cli

# Login
az login

# Create resource group
az group create -n vision-guide-rg -l eastus
```

### Deploy

```bash
# Create App Service plan
az appservice plan create -n vision-guide-plan \
  -g vision-guide-rg --sku B2 --is-linux

# Create web app
az webapp create -n vision-guide-app \
  -g vision-guide-rg -p vision-guide-plan \
  --runtime "NODE:20-lts"

# Configure environment variables
az webapp config appsettings set \
  -n vision-guide-app -g vision-guide-rg \
  --settings \
    NEXT_PUBLIC_API_BASE_URL="https://your-backend.azurewebsites.net" \
    NODE_ENV="production"

# Deploy from GitHub
az webapp deployment source config-zip \
  -n vision-guide-app -g vision-guide-rg \
  --src dist.zip
```

---

## 5️⃣ Google Cloud Run

### Prerequisites
```bash
# Install Google Cloud SDK
# https://cloud.google.com/sdk/docs/install

# Authenticate
gcloud auth login

# Set project
gcloud config set project YOUR_PROJECT_ID
```

### Deploy

```bash
# Build and push to Container Registry
gcloud builds submit --tag gcr.io/YOUR_PROJECT_ID/vision-guide

# Deploy to Cloud Run
gcloud run deploy vision-guide \
  --image gcr.io/YOUR_PROJECT_ID/vision-guide \
  --platform managed \
  --region us-central1 \
  --allow-unauthenticated \
  --set-env-vars NEXT_PUBLIC_API_BASE_URL=https://backend.example.com
```

---

## 6️⃣ Netlify

### Setup

```bash
# Install Netlify CLI
npm i -g netlify-cli

# Deploy (first time)
netlify deploy --prod

# Follow prompts
```

### Build Settings

In Netlify Dashboard:
1. Site Settings → Build & Deploy
2. Build command: `pnpm build`
3. Publish directory: `.next`
4. Node version: 20

### Environment Variables

In Site Settings → Build & Deploy → Environment:
```
NEXT_PUBLIC_API_BASE_URL=https://your-backend-api.com
```

---

## 7️⃣ DigitalOcean App Platform

### Deploy via GitHub

1. Connect GitHub repo to DigitalOcean
2. Create new app
3. Select frontend directory
4. Set build command: `pnpm build`
5. Set run command: `pnpm start`
6. Add environment variables:
   ```
   NEXT_PUBLIC_API_BASE_URL=https://backend.example.com
   ```

### Manual Deployment

```bash
# Install doctl CLI
brew install doctl  # macOS

# Authenticate
doctl auth init

# Create app spec
cat > app.yaml << EOF
name: vision-guide
services:
- name: web
  github:
    repo: username/Unreal-Mobile-AR
    branch: main
  build_command: pnpm build
  run_command: pnpm start
  environment_slug: node-js
  http_port: 3000
EOF

# Deploy
doctl apps create --spec app.yaml
```

---

## 📊 Performance Optimization

### Vercel Analytics

```typescript
// Already configured in layout.tsx
import { Analytics } from '@vercel/analytics/next'

export default function RootLayout() {
  return (
    <html>
      <body>
        {children}
        <Analytics />
      </body>
    </html>
  )
}
```

### Image Optimization

Images are optimized automatically by Next.js:
```typescript
import Image from 'next/image'

export default function Hero() {
  return (
    <Image
      src="/hero.jpg"
      alt="Hero"
      width={1200}
      height={600}
      priority
    />
  )
}
```

### Bundle Analysis

```bash
# Install analyzer
npm i --save-dev @next/bundle-analyzer

# Add to next.config.js
const withBundleAnalyzer = require('@next/bundle-analyzer')({
  enabled: process.env.ANALYZE === 'true',
})

module.exports = withBundleAnalyzer({})

# Run analysis
ANALYZE=true pnpm build
```

---

## 🔒 Security Checklist

- [ ] HTTPS enabled (all platforms)
- [ ] API base URL uses HTTPS
- [ ] No sensitive data in client code
- [ ] Environment variables not exposed
- [ ] CSP headers configured
- [ ] CORS properly configured on backend
- [ ] Rate limiting enabled
- [ ] DDoS protection enabled (if on cloud)

---

## 📡 CDN Configuration

### Vercel CDN
- Automatic with Vercel deployment
- Free with Vercel Pro

### Cloudflare

```bash
# Add DNS records
# CNAME: www → your-app.vercel.app

# Enable settings:
# - Auto Minify
# - Brotli compression
# - HTTP/3
```

---

## 🔄 Zero-Downtime Updates

### Vercel
```bash
git push origin main
# Automatic deployment with preview URLs
# Production updates are instant
```

### Docker
```bash
# Build new image
docker build -t vision-guide:v2 .

# Start new container
docker run -d -p 3000:3000 vision-guide:v2

# Use load balancer to switch traffic
# Stop old container
docker stop <old-container-id>
```

---

## 🆘 Troubleshooting

| Issue | Solution |
|-------|----------|
| Build fails | Check Node version (18+), run `pnpm install --force` |
| 504 Gateway Timeout | Backend not responding, check `NEXT_PUBLIC_API_BASE_URL` |
| Static assets 404 | Run `pnpm build`, check `public/` folder |
| Environment vars not loading | Verify deployment config, restart app |
| High memory usage | Increase container resources, optimize images |
| Slow builds | Use pnpm cache, reduce dependencies |

---

## 📞 Support

For deployment issues:
1. Check logs: `pnpm build` locally first
2. Verify backend connectivity
3. Check environment variables
4. Review platform-specific documentation
5. Clear cache and rebuild

---

**Deployment Guide v1.0**
