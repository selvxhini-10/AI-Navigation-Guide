#!/bin/bash

# VisionGuide Frontend Setup Script
# This script sets up the frontend environment and installs dependencies

set -e

echo "🚀 VisionGuide Frontend Setup"
echo "=============================="

# Check Node.js
if ! command -v node &> /dev/null; then
    echo "❌ Node.js is not installed. Please install Node.js 18+ from https://nodejs.org"
    exit 1
fi

NODE_VERSION=$(node -v)
echo "✅ Node.js $NODE_VERSION found"

# Check pnpm
if ! command -v pnpm &> /dev/null; then
    echo "📦 Installing pnpm..."
    npm install -g pnpm
fi

PNPM_VERSION=$(pnpm -v)
echo "✅ pnpm $PNPM_VERSION found"

# Install dependencies
echo ""
echo "📦 Installing dependencies..."
pnpm install

# Create .env.local if it doesn't exist
if [ ! -f .env.local ]; then
    echo ""
    echo "🔧 Creating .env.local from template..."
    cp .env.example .env.local
    echo "✅ Created .env.local"
    echo ""
    echo "⚠️  Please update .env.local with your configuration:"
    echo "   - NEXT_PUBLIC_API_BASE_URL (default: http://localhost:8000)"
    echo "   - Other optional variables"
else
    echo "✅ .env.local already exists"
fi

# Build the project
echo ""
echo "🔨 Building project..."
pnpm build

echo ""
echo "✅ Setup complete!"
echo ""
echo "📝 Next steps:"
echo "   1. Update .env.local if needed"
echo "   2. Run: pnpm dev"
echo "   3. Open: http://localhost:3000"
echo ""
