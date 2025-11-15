@echo off
REM VisionGuide Frontend Setup Script (Windows)
REM This script sets up the frontend environment and installs dependencies

setlocal enabledelayedexpansion

echo.
echo 🚀 VisionGuide Frontend Setup
echo ==============================

REM Check Node.js
where node >nul 2>nul
if errorlevel 1 (
    echo ❌ Node.js is not installed. Please install Node.js 18+ from https://nodejs.org
    exit /b 1
)

for /f "tokens=*" %%i in ('node -v') do set NODE_VERSION=%%i
echo ✅ Node.js %NODE_VERSION% found

REM Check pnpm
where pnpm >nul 2>nul
if errorlevel 1 (
    echo 📦 Installing pnpm...
    call npm install -g pnpm
)

for /f "tokens=*" %%i in ('pnpm -v') do set PNPM_VERSION=%%i
echo ✅ pnpm %PNPM_VERSION% found

REM Install dependencies
echo.
echo 📦 Installing dependencies...
call pnpm install

REM Create .env.local if it doesn't exist
if not exist .env.local (
    echo.
    echo 🔧 Creating .env.local from template...
    copy .env.example .env.local
    echo ✅ Created .env.local
    echo.
    echo ⚠️  Please update .env.local with your configuration:
    echo    - NEXT_PUBLIC_API_BASE_URL (default: http://localhost:8000)
    echo    - Other optional variables
) else (
    echo ✅ .env.local already exists
)

REM Build the project
echo.
echo 🔨 Building project...
call pnpm build

echo.
echo ✅ Setup complete!
echo.
echo 📝 Next steps:
echo    1. Update .env.local if needed
echo    2. Run: pnpm dev
echo    3. Open: http://localhost:3000
echo.

endlocal
