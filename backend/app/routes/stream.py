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

@router.get("/snapshot")
async def proxy_snapshot():
    """
    Proxy single JPEG snapshot from ESP32-CAM to frontend
    Much more reliable than continuous MJPEG stream
    """
    try:
        # Request single frame from ESP32-CAM
        snapshot_url = ESP32_CAM_STREAM_URL.replace('/stream', '/capture.jpg')
        logger.info(f"Fetching snapshot from {snapshot_url}")
        response = requests.get(snapshot_url, timeout=10)  # Increased timeout
        
        if response.status_code == 200:
            logger.info("Snapshot fetched successfully")
            return Response(
                content=response.content,
                media_type="image/jpeg",
                headers={
                    "Access-Control-Allow-Origin": "*",
                    "Cache-Control": "no-cache, no-store, must-revalidate"
                }
            )
        else:
            logger.error(f"ESP32-CAM returned status {response.status_code}")
            return Response(status_code=503, content="Camera unavailable")
            
    except requests.exceptions.Timeout:
        logger.error(f"Timeout fetching snapshot from ESP32-CAM at {snapshot_url}")
        return Response(status_code=504, content="Camera timeout")
    except requests.exceptions.ConnectionError as e:
        logger.error(f"Connection error to ESP32-CAM: {e}")
        return Response(status_code=503, content="Cannot connect to camera")
    except Exception as e:
        logger.error(f"Snapshot error: {e}")
        return Response(status_code=500, content=f"Error: {str(e)}")

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

@router.post("/trigger-capture")
async def trigger_capture():
    """Trigger ESP32-CAM to capture an image via software button"""
    try:
        import requests
        
        trigger_url = ESP32_CAM_STREAM_URL.replace('/stream', '/trigger-capture')
        logger.info(f"Triggering capture at {trigger_url}")
        response = requests.post(trigger_url, timeout=5)
        
        if response.status_code == 200:
            return response.json()
        else:
            logger.error(f"ESP32 returned status {response.status_code}")
            return Response(status_code=response.status_code, content="Trigger failed")
    
    except Exception as e:
        logger.error(f"Error triggering capture: {e}")
        return Response(status_code=500, content=str(e))

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
