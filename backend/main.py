"""
Smart Navigation Cane (SNC) Backend API
FastAPI server for AI-powered walking stick with object detection and live GPS guidance
Main entry point for the API server
"""
import uvicorn
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from contextlib import asynccontextmanager
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Import routes
from app.routes import health, detection, navigation, device

@asynccontextmanager
async def lifespan(app: FastAPI):
    """Lifespan context manager for startup and shutdown events"""
    logger.info("Starting up Smart Navigation Cane Backend API Server")
    logger.info("Available modules: Camera Detection, GPS Navigation, Device Management")
    yield
    logger.info("Shutting down Smart Navigation Cane Backend API Server")

# Create FastAPI application
app = FastAPI(
    title="Smart Navigation Cane (SNC) Backend",
    description="Backend API for AI-powered walking stick with object detection and live GPS guidance",
    version="2.0.0",
    lifespan=lifespan
)

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Update with specific origins in production
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Include routers
app.include_router(health.router, tags=["Health"])
app.include_router(detection.router, prefix="/api/detection", tags=["Object Detection"])
app.include_router(navigation.router, prefix="/api/navigation", tags=["Navigation & GPS"])
app.include_router(device.router, prefix="/api/device", tags=["Device Management"])

# Root endpoint
@app.get("/")
async def root():
    """Root endpoint with API information"""
    return {
        "name": "Smart Navigation Cane (SNC) Backend API",
        "version": "2.0.0",
        "description": "AI-powered walking stick with object detection and live GPS guidance",
        "modules": {
            "detection": "Camera and object detection from ESP32-CAM",
            "navigation": "GPS-based navigation with Google Maps integration",
            "device": "Device management for Arduino and ESP32-CAM"
        },
        "docs": "/docs",
        "redoc": "/redoc"
    }

if __name__ == "__main__":
    uvicorn.run(
        "main:app",
        host="0.0.0.0",
        port=8000,
        reload=True,
        log_level="info"
    )
