"""
Device management endpoints
Handles Arduino and ESP32-CAM device status, connectivity, and configuration
"""
from fastapi import APIRouter, status, HTTPException
from app.models import DeviceStatus, Session
from typing import List, Optional
from datetime import datetime

router = APIRouter()

# Mock storage for devices and sessions
connected_devices: List[dict] = {}
active_sessions: List[dict] = []
session_id_counter = 1

@router.post("/device/register", status_code=status.HTTP_201_CREATED)
async def register_device(
    device_id: str,
    device_type: str,
    metadata: dict = {}
):
    """
    Register a new device (Arduino, ESP32-CAM, or mobile app)
    
    Args:
        device_id: Unique device identifier
        device_type: Type of device (arduino, esp32cam, mobile)
        metadata: Additional device metadata
    
    Returns:
        Device registration confirmation
    """
    device = {
        "device_id": device_id,
        "device_type": device_type,
        "registered_at": datetime.utcnow(),
        "last_heartbeat": datetime.utcnow(),
        "connectivity": "online",
        "battery_level": 100,
        "active_sensors": [],
        "errors": [],
        "metadata": metadata
    }
    
    connected_devices[device_id] = device
    
    return {
        "status": "registered",
        "device_id": device_id,
        "message": f"Device {device_id} registered successfully"
    }

@router.post("/device/heartbeat/{device_id}")
async def send_heartbeat(device_id: str, battery_level: Optional[float] = None):
    """
    Send device heartbeat to indicate it's still active
    
    Args:
        device_id: Device identifier
        battery_level: Current battery percentage
    
    Returns:
        Heartbeat acknowledgment
    """
    if device_id not in connected_devices:
        raise HTTPException(status_code=404, detail=f"Device {device_id} not registered")
    
    device = connected_devices[device_id]
    device["last_heartbeat"] = datetime.utcnow()
    device["connectivity"] = "online"
    
    if battery_level is not None:
        device["battery_level"] = battery_level
    
    return {
        "status": "acknowledged",
        "device_id": device_id,
        "timestamp": device["last_heartbeat"]
    }

@router.get("/device/{device_id}")
async def get_device_status(device_id: str):
    """Get current status of a specific device"""
    if device_id not in connected_devices:
        raise HTTPException(status_code=404, detail=f"Device {device_id} not registered")
    
    return connected_devices[device_id]

@router.get("/device/list")
async def list_all_devices():
    """Get list of all registered devices"""
    return list(connected_devices.values())

@router.put("/device/{device_id}/update-sensors")
async def update_device_sensors(device_id: str, active_sensors: List[str]):
    """
    Update list of active sensors on a device
    
    Args:
        device_id: Device identifier
        active_sensors: List of active sensor names (camera, gps, accelerometer, etc.)
    
    Returns:
        Updated device status
    """
    if device_id not in connected_devices:
        raise HTTPException(status_code=404, detail=f"Device {device_id} not registered")
    
    connected_devices[device_id]["active_sensors"] = active_sensors
    return connected_devices[device_id]

@router.post("/device/{device_id}/error")
async def report_device_error(device_id: str, error_message: str):
    """
    Report an error on a device
    
    Args:
        device_id: Device identifier
        error_message: Error description
    
    Returns:
        Error acknowledgment
    """
    if device_id not in connected_devices:
        raise HTTPException(status_code=404, detail=f"Device {device_id} not registered")
    
    device = connected_devices[device_id]
    if error_message not in device["errors"]:
        device["errors"].append(error_message)
    
    return {
        "status": "error_recorded",
        "device_id": device_id,
        "error": error_message
    }

@router.post("/device/{device_id}/clear-errors")
async def clear_device_errors(device_id: str):
    """Clear all errors on a device"""
    if device_id not in connected_devices:
        raise HTTPException(status_code=404, detail=f"Device {device_id} not registered")
    
    connected_devices[device_id]["errors"] = []
    return {"status": "errors_cleared", "device_id": device_id}

@router.post("/session/start", status_code=status.HTTP_201_CREATED)
async def start_session(user_id: Optional[str] = None):
    """
    Start a new navigation session
    
    Args:
        user_id: Optional user identifier
    
    Returns:
        Session details
    """
    global session_id_counter
    
    session_id = f"session_{session_id_counter}"
    session_id_counter += 1
    
    session = {
        "session_id": session_id,
        "user_id": user_id,
        "start_time": datetime.utcnow(),
        "end_time": None,
        "active_devices": [],
        "detected_obstacles": [],
        "navigation_mode": "idle",
        "status": "active"
    }
    
    active_sessions.append(session)
    
    return session

@router.get("/session/{session_id}")
async def get_session(session_id: str):
    """Get details for a specific session"""
    for session in active_sessions:
        if session["session_id"] == session_id:
            return session
    
    raise HTTPException(status_code=404, detail=f"Session {session_id} not found")

@router.post("/session/{session_id}/add-device")
async def add_device_to_session(session_id: str, device_id: str):
    """Add a device to an active session"""
    for session in active_sessions:
        if session["session_id"] == session_id:
            if device_id not in [d.get("device_id") for d in session["active_devices"]]:
                if device_id in connected_devices:
                    session["active_devices"].append(
                        {"device_id": device_id, "connected_at": datetime.utcnow()}
                    )
            return session
    
    raise HTTPException(status_code=404, detail=f"Session {session_id} not found")

@router.post("/session/{session_id}/end")
async def end_session(session_id: str):
    """End an active session"""
    for session in active_sessions:
        if session["session_id"] == session_id:
            session["end_time"] = datetime.utcnow()
            session["status"] = "completed"
            return session
    
    raise HTTPException(status_code=404, detail=f"Session {session_id} not found")

@router.get("/session/list/active")
async def list_active_sessions():
    """Get all active sessions"""
    return [s for s in active_sessions if s["status"] == "active"]
