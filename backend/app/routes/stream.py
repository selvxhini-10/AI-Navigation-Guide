"""
ESP32-CAM MJPEG Stream Proxy and Frame Processing
Handles MJPEG stream from ESP32-CAM and provides proxy endpoint
"""
from fastapi import APIRouter, Response
from fastapi.responses import StreamingResponse
import requests
import cv2
import numpy as np
from typing import AsyncGenerator
import logging
import os

logger = logging.getLogger(__name__)

router = APIRouter()

# ESP32-CAM stream URL (configurable via environment variable)
# Set this after uploading Arduino code and getting the ESP32's IP
ESP32_CAM_STREAM_URL = os.getenv("ESP32_CAM_URL", "http://192.168.4.1:80/stream")

@router.get("/mjpeg")
async def proxy_mjpeg_stream():
    """
    Proxy MJPEG stream from ESP32-CAM to frontend
    This allows frontend to access the stream without CORS issues
    """
    async def generate():
        try:
            logger.info(f"Connecting to ESP32-CAM stream at {ESP32_CAM_STREAM_URL}")
            response = requests.get(ESP32_CAM_STREAM_URL, stream=True, timeout=10)
            
            if response.status_code != 200:
                logger.error(f"ESP32-CAM returned status {response.status_code}")
                return
            
            logger.info("Stream connected, proxying data...")
            
            for chunk in response.iter_content(chunk_size=4096):
                if chunk:
                    yield chunk
                    
        except requests.exceptions.ConnectionError as e:
            logger.error(f"Connection error: {e}")
        except requests.exceptions.Timeout as e:
            logger.error(f"Timeout error: {e}")
        except Exception as e:
            logger.error(f"Stream error: {e}")
    
    return StreamingResponse(
        generate(),
        media_type="multipart/x-mixed-replace; boundary=frame"
    )

@router.get("/status")
async def get_stream_status():
    """Check if ESP32-CAM stream is available"""
    try:
        response = requests.get(ESP32_CAM_STREAM_URL, timeout=2)
        return {
            "status": "online",
            "stream_url": ESP32_CAM_STREAM_URL,
            "accessible": response.status_code == 200
        }
    except Exception as e:
        return {
            "status": "offline",
            "stream_url": ESP32_CAM_STREAM_URL,
            "accessible": False,
            "error": str(e)
        }

@router.post("/process-frame")
async def process_frame_backend(frame_data: dict):
    """
    Optional: Process frame with backend AI (for heavier models)
    Frontend can send frames here for processing
    """
    # This is optional - frontend does detection with TensorFlow.js
    # Use this for heavier models like YOLOv8, SAM, etc.
    
    return {
        "status": "received",
        "message": "Backend AI processing not yet implemented",
        "note": "Frontend handles detection with TensorFlow.js COCO-SSD"
    }
