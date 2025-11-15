"""
Camera and object detection endpoints
Handles frames from ESP32-CAM and processes object detection
"""
from fastapi import APIRouter, status, HTTPException, UploadFile, File
from app.models import (
    ObjectDetectionFrame, DetectedObject, CameraFrameUpload
)
from typing import List
from datetime import datetime
import io
import base64
from pathlib import Path

router = APIRouter()

# Mock storage for detected frames and objects
detected_frames_store: List[dict] = []
frame_id_counter = 1

@router.post("/camera/upload", status_code=status.HTTP_201_CREATED)
async def upload_camera_frame(frame_data: CameraFrameUpload):
    """
    Upload camera frame from ESP32-CAM with embedded metadata
    
    Args:
        frame_data: Camera frame upload payload
    
    Returns:
        Confirmation with frame processing status
    """
    global frame_id_counter
    
    frame_id = f"frame_{frame_id_counter}"
    frame_id_counter += 1
    
    # Create directory for frames if it doesn't exist
    frames_dir = Path("saved_frames")
    frames_dir.mkdir(exist_ok=True)
    
    # Save base64 frame if provided
    frame_path = None
    if frame_data.frame_base64:
        try:
            frame_bytes = base64.b64decode(frame_data.frame_base64)
            frame_path = frames_dir / f"{frame_id}.jpg"
            with open(frame_path, "wb") as f:
                f.write(frame_bytes)
        except Exception as e:
            raise HTTPException(status_code=400, detail=f"Invalid frame data: {str(e)}")
    
    frame_record = {
        "frame_id": frame_id,
        "device_id": frame_data.device_id,
        "timestamp": frame_data.timestamp,
        "frame_path": str(frame_path) if frame_path else None,
        "metadata": frame_data.metadata,
        "status": "received"
    }
    detected_frames_store.append(frame_record)
    
    return {
        "status": "received",
        "frame_id": frame_id,
        "message": "Frame queued for object detection processing"
    }

@router.get("/detection/latest")
async def get_latest_detection():
    """Get the latest object detection results"""
    if not detected_frames_store:
        raise HTTPException(status_code=404, detail="No detection results available")
    
    latest_frame = detected_frames_store[-1]
    return latest_frame

@router.get("/detection/{frame_id}")
async def get_detection_results(frame_id: str):
    """
    Get object detection results for a specific frame
    
    Args:
        frame_id: Frame identifier
    
    Returns:
        Detection results with detected objects
    """
    for frame in detected_frames_store:
        if frame["frame_id"] == frame_id:
            return frame
    
    raise HTTPException(status_code=404, detail=f"Frame {frame_id} not found")

@router.post("/detection/process", response_model=ObjectDetectionFrame)
async def process_detection_frame(detection_result: ObjectDetectionFrame):
    """
    Store object detection results from ML model
    
    This endpoint receives processed detection results from the AI model
    running on the device or server
    
    Args:
        detection_result: Detection results with objects and frame info
    
    Returns:
        Stored detection frame with processing details
    """
    detection_dict = detection_result.dict()
    detection_dict["stored_at"] = datetime.utcnow()
    detected_frames_store.append(detection_dict)
    
    return detection_result

@router.get("/detection/stream/latest")
async def get_detection_stream():
    """Get latest detection stream (can be used for real-time updates)"""
    if not detected_frames_store:
        return {
            "status": "no_data",
            "message": "No detection data available yet"
        }
    
    latest = detected_frames_store[-1]
    return {
        "status": "active",
        "latest_frame_id": latest.get("frame_id"),
        "timestamp": latest.get("timestamp"),
        "object_count": len(latest.get("objects", []))
    }

@router.delete("/detection/{frame_id}")
async def delete_detection_frame(frame_id: str):
    """Delete a specific detection frame record"""
    for i, frame in enumerate(detected_frames_store):
        if frame["frame_id"] == frame_id:
            detected_frames_store.pop(i)
            return {"status": "deleted", "frame_id": frame_id}
    
    raise HTTPException(status_code=404, detail=f"Frame {frame_id} not found")

@router.get("/detection/device/{device_id}")
async def get_device_detections(device_id: str, limit: int = 10):
    """
    Get recent detection frames from a specific device
    
    Args:
        device_id: ESP32-CAM device identifier
        limit: Maximum number of recent frames to return
    
    Returns:
        List of recent detection frames from device
    """
    device_frames = [
        frame for frame in detected_frames_store 
        if frame.get("device_id") == device_id
    ]
    
    return device_frames[-limit:] if device_frames else []
