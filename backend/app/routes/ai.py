"""
AI processing endpoints
Handles image processing with YOLO object detection
"""
from fastapi import APIRouter, UploadFile, File, HTTPException
from fastapi.responses import FileResponse, Response
from app.services.ai_detection import ai_service
from pathlib import Path
import logging

logger = logging.getLogger(__name__)

router = APIRouter()

@router.post("/detect")
async def detect_objects(file: UploadFile = File(...)):
    """
    Process uploaded image with AI model for object detection
    
    Args:
        file: Image file to process
        
    Returns:
        Detection results with bounding boxes, description, and audio
    """
    try:
        # Read image data
        image_data = await file.read()
        
        logger.info(f"Processing image: {file.filename} ({len(image_data)} bytes)")
        
        # Process with AI model
        result = ai_service.process_image(image_data)
        
        return {
            "status": "success",
            "detections": result["detections"],
            "description": result["description"],
            "image_url": result["image_url"],
            "audio_url": result["audio_url"],
            "count": result["count"]
        }
    
    except Exception as e:
        logger.error(f"Error processing image: {e}")
        raise HTTPException(status_code=500, detail=str(e))

@router.get("/detected_image.jpg")
async def get_detected_image():
    """Serve the latest detected image with bounding boxes"""
    image_path = Path("detected_outputs/detected_image.jpg")
    if not image_path.exists():
        raise HTTPException(status_code=404, detail="No detected image available")
    return FileResponse(image_path, media_type="image/jpeg")

@router.get("/detected_audio.mp3")
async def get_detected_audio():
    """Serve the latest audio description"""
    audio_path = Path("detected_outputs/detected_audio.mp3")
    if not audio_path.exists():
        raise HTTPException(status_code=404, detail="No audio available")
    return FileResponse(audio_path, media_type="audio/mpeg")

@router.post("/trigger-capture")
async def trigger_esp32_capture():
    """
    Trigger ESP32-CAM to capture an image
    Proxies the request to the ESP32-CAM device
    """
    try:
        import requests
        import os
        
        esp32_url = os.getenv("ESP32_CAM_URL", "http://192.168.4.1:80/stream")
        trigger_url = esp32_url.replace('/stream', '/trigger-capture')
        
        response = requests.post(trigger_url, timeout=5)
        
        if response.status_code == 200:
            return response.json()
        else:
            raise HTTPException(status_code=response.status_code, detail="ESP32 capture failed")
    
    except Exception as e:
        logger.error(f"Error triggering ESP32 capture: {e}")
        raise HTTPException(status_code=500, detail=str(e))

@router.get("/test-image")
async def get_test_image():
    """Serve test image for development"""
    test_image_path = Path("backend/test.jpg")
    if not test_image_path.exists():
        # Try alternative path
        test_image_path = Path("test.jpg")
    
    if not test_image_path.exists():
        raise HTTPException(status_code=404, detail="Test image not found")
    
    return FileResponse(test_image_path, media_type="image/jpeg")
