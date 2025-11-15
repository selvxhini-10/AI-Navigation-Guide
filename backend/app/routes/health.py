"""
Health check endpoints
"""
from fastapi import APIRouter, status
from datetime import datetime
from app.models import HealthStatus

router = APIRouter()

@router.get("/health", status_code=status.HTTP_200_OK)
async def health_check() -> HealthStatus:
    """
    Health check endpoint
    
    Returns:
        HealthStatus: Current health status of the API
    """
    return HealthStatus(
        status="healthy",
        version="1.0.0",
        timestamp=datetime.utcnow()
    )

@router.get("/ready", status_code=status.HTTP_200_OK)
async def readiness_check() -> dict:
    """
    Readiness check endpoint
    
    Returns:
        dict: Readiness status
    """
    return {
        "ready": True,
        "timestamp": datetime.utcnow()
    }
