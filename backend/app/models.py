"""
Pydantic models for Smart Navigation Cane (SNC)
Request/response validation for camera input, object detection, and navigation
"""
from pydantic import BaseModel, Field
from typing import Optional, List, Dict, Any
from datetime import datetime

class HealthStatus(BaseModel):
    """Health check response model"""
    status: str
    version: str
    timestamp: datetime

class DetectedObject(BaseModel):
    """Model for detected object from AI"""
    object_id: str = Field(..., description="Unique object identifier")
    label: str = Field(..., description="Object classification label")
    confidence: float = Field(..., ge=0.0, le=1.0, description="Detection confidence (0-1)")
    bbox: Dict[str, float] = Field(..., description="Bounding box {x, y, width, height}")
    distance: Optional[float] = Field(None, description="Estimated distance in meters")
    position: Dict[str, float] = Field(default={"x": 0, "y": 0}, description="Position in frame")

class ObjectDetectionFrame(BaseModel):
    """Model for object detection results from a frame"""
    frame_id: str
    timestamp: datetime
    image_path: Optional[str] = Field(None, description="Path to saved frame image")
    objects: List[DetectedObject] = Field(default=[], description="Detected objects in frame")
    frame_size: Dict[str, int] = Field(..., description="Frame dimensions {width, height}")
    processing_time_ms: float = Field(..., description="Time taken to process frame")

    class Config:
        from_attributes = True

class CameraFrameUpload(BaseModel):
    """Model for uploading camera frames from ESP32-CAM"""
    device_id: str = Field(..., description="ESP32-CAM device identifier")
    timestamp: datetime
    frame_base64: Optional[str] = Field(None, description="Base64 encoded frame image")
    frame_url: Optional[str] = Field(None, description="URL to frame image")
    metadata: Dict[str, Any] = Field(default={}, description="Additional metadata")

class GPSLocation(BaseModel):
    """Model for GPS location data"""
    latitude: float = Field(..., ge=-90, le=90, description="Latitude coordinate")
    longitude: float = Field(..., ge=-180, le=180, description="Longitude coordinate")
    altitude: Optional[float] = Field(None, description="Altitude in meters")
    accuracy: Optional[float] = Field(None, description="GPS accuracy in meters")
    heading: Optional[float] = Field(None, ge=0, le=360, description="Direction heading in degrees")
    speed: Optional[float] = Field(None, ge=0, description="Speed in m/s")

class NavigationGuidance(BaseModel):
    """Model for navigation guidance from Google Maps"""
    route_id: str = Field(..., description="Unique route identifier")
    origin: GPSLocation = Field(..., description="Starting location")
    destination: GPSLocation = Field(..., description="Target destination")
    instructions: List[str] = Field(..., description="Turn-by-turn navigation instructions")
    distance_remaining: float = Field(..., description="Remaining distance in meters")
    duration_remaining: int = Field(..., description="Estimated remaining time in seconds")
    current_step: int = Field(..., description="Current instruction step index")
    waypoints: List[GPSLocation] = Field(default=[], description="Route waypoints")
    map_url: Optional[str] = Field(None, description="URL to map visualization")

class ObstacleAlert(BaseModel):
    """Model for obstacle/hazard alerts"""
    alert_id: str
    timestamp: datetime
    alert_type: str = Field(..., description="Type of alert (obstacle, hazard, pedestrian, etc.)")
    severity: str = Field(..., description="Severity level (low, medium, high, critical)")
    description: str = Field(..., description="Description of the obstacle/hazard")
    location: Optional[GPSLocation] = Field(None, description="Location of obstacle")
    recommended_action: str = Field(..., description="Recommended action for user")
    detected_objects: List[DetectedObject] = Field(default=[], description="Related detected objects")

class DeviceStatus(BaseModel):
    """Model for device status (Arduino + ESP32-CAM)"""
    device_id: str
    timestamp: datetime
    device_type: str = Field(..., description="Device type (arduino, esp32cam, mobile)")
    battery_level: Optional[float] = Field(None, ge=0, le=100, description="Battery percentage")
    connectivity: str = Field(..., description="Connection status (online, offline, error)")
    last_heartbeat: datetime
    active_sensors: List[str] = Field(default=[], description="Active sensors (camera, gps, etc.)")
    errors: List[str] = Field(default=[], description="Any active error messages")

class Session(BaseModel):
    """Model for user session/navigation session"""
    session_id: str
    user_id: Optional[str] = Field(None, description="User identifier")
    start_time: datetime
    end_time: Optional[datetime] = None
    current_location: Optional[GPSLocation] = None
    destination: Optional[GPSLocation] = None
    active_devices: List[DeviceStatus] = Field(default=[], description="Connected devices")
    detected_obstacles: List[str] = Field(default=[], description="Obstacle IDs encountered")
    navigation_mode: str = Field(default="idle", description="Current mode (idle, navigating, paused)")

    class Config:
        from_attributes = True
