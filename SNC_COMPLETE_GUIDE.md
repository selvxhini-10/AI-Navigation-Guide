# Smart Navigation Cane (SNC) - Complete Integration Guide

## Overview

The Smart Navigation Cane is an AI-powered walking stick system combining:
- **Hardware**: ESP32-CAM (camera) + Arduino (sensors/processor)
- **Backend**: FastAPI Python server for detection, navigation, and device management
- **Frontend**: Unreal Engine 5.4 mobile app (iOS/Android) for user interface

## Project Architecture

```
┌─────────────────────────────────────────────────┐
│  Mobile App (Unreal Engine 5.4)                 │
│  - Real-time detection alerts                   │
│  - Turn-by-turn navigation                      │
│  - Device status monitoring                     │
│  - Audio/haptic feedback                        │
└─────────────┬───────────────────────────────────┘
              │
              │ REST API (HTTP/JSON)
              │
┌─────────────▼───────────────────────────────────┐
│  Backend API (FastAPI)                          │
│  - Detection Processing                         │
│  - Navigation Management                        │
│  - Device Coordination                          │
│  - Session Management                           │
└─────────────┬───────────────────────────────────┘
              │
      ┌───────┴────────┐
      │                │
┌─────▼────┐      ┌───▼──────────┐
│ ESP32-CAM│      │ Arduino + GPS│
│ (Camera) │      │ (Sensors)    │
└──────────┘      └──────────────┘
```

## Backend API Complete Reference

### 1. Health & Status Endpoints

#### Check Backend Health
```bash
GET /health
```
**Response:**
```json
{
  "status": "healthy",
  "version": "2.0.0",
  "timestamp": "2024-11-15T10:30:00"
}
```

### 2. Object Detection Endpoints

#### Upload Camera Frame (from ESP32-CAM)
```bash
POST /api/detection/camera/upload
Content-Type: application/json

{
  "device_id": "esp32_001",
  "timestamp": "2024-11-15T10:30:00",
  "frame_base64": "iVBORw0KGgoAAAANSUhEUgAAAAEAAAAB...",
  "metadata": {
    "resolution": "640x480",
    "fps": 30
  }
}
```

#### Get Latest Detection Results
```bash
GET /api/detection/latest
```
**Response:**
```json
{
  "frame_id": "frame_001",
  "timestamp": "2024-11-15T10:30:00",
  "objects": [
    {
      "object_id": "obj_001",
      "label": "person",
      "confidence": 0.95,
      "bbox": {"x": 100, "y": 150, "width": 50, "height": 100},
      "distance": 2.5,
      "position": {"x": 160, "y": 200}
    },
    {
      "object_id": "obj_002",
      "label": "car",
      "confidence": 0.87,
      "bbox": {"x": 200, "y": 180, "width": 80, "height": 60},
      "distance": 5.0,
      "position": {"x": 240, "y": 210}
    }
  ],
  "frame_size": {"width": 640, "height": 480},
  "processing_time_ms": 45
}
```

#### Process Detection Results (from ML Model)
```bash
POST /api/detection/process
Content-Type: application/json

{
  "frame_id": "frame_001",
  "timestamp": "2024-11-15T10:30:00",
  "objects": [...],
  "frame_size": {"width": 640, "height": 480},
  "processing_time_ms": 150
}
```

#### Get Detection Stream Status
```bash
GET /api/detection/stream/latest
```

### 3. Navigation & GPS Endpoints

#### Start Navigation Route
```bash
POST /api/navigation/start-route
Content-Type: application/json

{
  "origin": {
    "latitude": 40.7128,
    "longitude": -74.0060,
    "altitude": 10.0
  },
  "destination": {
    "latitude": 40.7589,
    "longitude": -73.9851,
    "altitude": 15.0
  }
}
```
**Response:**
```json
{
  "route_id": "route_001",
  "status": "started",
  "total_distance": 5000,
  "estimated_duration": 3600,
  "instructions": [
    "Head north on Main Street for 500 meters",
    "Turn right onto Market Avenue",
    "Continue for 300 meters to destination",
    "Destination is on your left"
  ]
}
```

#### Update User Location & Get Next Instruction
```bash
PUT /api/navigation/route/{route_id}/update-location
Content-Type: application/json

{
  "latitude": 40.7150,
  "longitude": -74.0045,
  "altitude": 10.0
}
```
**Response:**
```json
{
  "route_id": "route_001",
  "current_instruction": "Turn right onto Market Avenue",
  "step_number": 2,
  "total_steps": 4,
  "distance_remaining": 800,
  "duration_remaining": 480,
  "status": "active"
}
```

#### End Navigation
```bash
POST /api/navigation/route/{route_id}/end
```

#### Report Obstacle
```bash
POST /api/navigation/obstacle-alert
Content-Type: application/json

{
  "alert_type": "obstacle",
  "severity": "high",
  "description": "Large pothole in path",
  "detected_objects": [
    {
      "object_id": "obj_001",
      "label": "pothole",
      "confidence": 0.92
    }
  ]
}
```

#### Get Active Obstacles
```bash
GET /api/navigation/obstacles
```

### 4. Device Management Endpoints

#### Register Device
```bash
POST /api/device/register
Content-Type: application/json

{
  "device_id": "esp32_001",
  "device_type": "esp32cam",
  "metadata": {
    "model": "ESP32-CAM",
    "firmware": "1.0.0"
  }
}
```

#### Send Device Heartbeat
```bash
POST /api/device/heartbeat/{device_id}?battery_level=85.5
```

#### Get Device Status
```bash
GET /api/device/{device_id}
```
**Response:**
```json
{
  "device_id": "esp32_001",
  "device_type": "esp32cam",
  "battery_level": 85.5,
  "connectivity": "online",
  "last_heartbeat": "2024-11-15T10:29:55",
  "active_sensors": ["camera", "accelerometer"],
  "errors": []
}
```

#### Report Device Error
```bash
POST /api/device/{device_id}/error?error_message="Camera frame drop rate high"
```

### 5. Session Management Endpoints

#### Start Session
```bash
POST /api/session/start?user_id="user_001"
```
**Response:**
```json
{
  "session_id": "session_001",
  "user_id": "user_001",
  "start_time": "2024-11-15T10:30:00",
  "status": "active"
}
```

#### Add Device to Session
```bash
POST /api/session/{session_id}/add-device?device_id="esp32_001"
```

#### End Session
```bash
POST /api/session/{session_id}/end
```

## Frontend (Unreal Engine 5.4) Integration

### Key Classes

#### USNCBackendManager
Central hub for all backend API communication.

**Example Usage:**
```cpp
// Initialize backend
USNCBackendManager::InitializeSNCBackend(TEXT("http://192.168.1.100:8000"));

// Start navigation
USNCBackendManager::StartNavigation(
  40.7128,    // Origin latitude
  -74.0060,   // Origin longitude
  40.7589,    // Dest latitude
  -73.9851,   // Dest longitude
  OnNavigationComplete
);

// Get latest detection
USNCBackendManager::GetLatestDetection(OnDetectionReceived);

// Report obstacle
USNCBackendManager::ReportObstacle(
  TEXT("obstacle"),
  TEXT("high"),
  TEXT("Pedestrian crossing"),
  OnObstacleReported
);
```

### Data Structures

#### FDetectedObject
```cpp
USTRUCT(BlueprintType)
struct FDetectedObject {
  FString Label;        // "person", "car", "obstacle", etc.
  float Confidence;     // 0.0 - 1.0
  float Distance;       // meters
  FString PositionX;
  FString PositionY;
};
```

#### FGPSLocation
```cpp
USTRUCT(BlueprintType)
struct FGPSLocation {
  double Latitude;
  double Longitude;
  float Altitude;
  float Heading;
};
```

#### FNavigationInstruction
```cpp
USTRUCT(BlueprintType)
struct FNavigationInstruction {
  FString Instruction;
  float DistanceMeters;
  int32 DurationSeconds;
};
```

## Hardware Configuration

### ESP32-CAM Setup

1. **Flash Firmware:**
```bash
# Use Arduino IDE or PlatformIO
# Install ESP32 board support
# Upload camera stream firmware
```

2. **Configure WiFi:**
```cpp
// Arduino sketch snippet
#include <WiFi.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

void setup() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
```

3. **Upload Camera Frames:**
```cpp
// Send frames to backend
void uploadFrame(uint8_t* frame, size_t size) {
  HTTPClient http;
  http.begin(client, "http://192.168.1.100:8000/api/detection/camera/upload");
  http.addHeader("Content-Type", "application/json");
  
  // Base64 encode frame
  // Send POST request
  http.POST(jsonPayload);
  http.end();
}
```

### Arduino Setup

1. **Configure Sensors:**
```cpp
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Wire.begin();
  mpu.initialize();
}

void loop() {
  // Read sensor data
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  
  // Send heartbeat to backend
  sendHeartbeat();
}
```

2. **Send Device Heartbeat:**
```cpp
void sendHeartbeat() {
  HTTPClient http;
  http.begin("http://192.168.1.100:8000/api/device/heartbeat/arduino_001?battery_level=85.5");
  http.POST("");
  http.end();
}
```

## Mobile App Features

### Detection Alerts
- Real-time object detection notifications
- Confidence-based filtering
- Distance-based warnings
- Automatic audio/haptic feedback

### Navigation Assistance
- Turn-by-turn voice guidance
- Visual route display
- Distance and duration remaining
- Auto-advance to next instruction

### Device Monitoring
- Battery level indicators
- Connection status
- Error notifications
- Device recalibration prompts

### Safety Features
- Obstacle detection and warnings
- Emergency stop button
- Low battery alerts
- Connection loss handling

## Deployment Guide

### Local Testing
```bash
# Terminal 1: Backend
cd backend
python -m venv venv
.\venv\Scripts\Activate.ps1
pip install -r requirements.txt
python main.py

# Terminal 2: Mock device simulation
python scripts/simulate_device.py

# Terminal 3: Unreal Editor
# Open frontend/UnrealMobileAR.uproject
# Set Backend URL to http://localhost:8000
# Play in editor
```

### Production Deployment

#### Backend (AWS/Azure/GCP)
```bash
# Using Docker
docker build -t snc-backend .
docker run -p 8000:8000 snc-backend

# OR use managed services
# AWS: ECS + ALB
# Azure: App Service + Container Registry
# GCP: Cloud Run
```

#### Mobile App
```bash
# iOS
# Xcode → Product → Archive
# Upload to App Store

# Android
# Android Studio → Build → Generate Signed Bundle
# Upload to Google Play Store
```

## Performance Optimization

### Backend
- Use YOLOv8 Nano for embedded inference (~5-10ms)
- Cache routes locally on device
- Batch detection frames for efficiency
- Use Redis for session caching

### Mobile App
- Compress detection alerts
- Batch API requests
- Cache navigation routes
- Local storage for offline mode

### Hardware
- Reduce frame resolution for faster processing
- Increase ESP32-CAM buffer for stable streaming
- Use firmware compression
- Implement power-saving modes

## Troubleshooting

### Connection Issues
```
Error: "Request failed or invalid response"
Solution:
1. Verify backend is running: http://localhost:8000/health
2. Check firewall allows port 8000
3. Verify device is on same network
4. Check backend logs for errors
```

### Detection Not Working
```
Error: No objects detected
Solution:
1. Verify ESP32-CAM is uploading frames
2. Check frame resolution is adequate
3. Verify detection model is loaded
4. Monitor /api/detection/latest endpoint
```

### Navigation Errors
```
Error: "Invalid GPS coordinates"
Solution:
1. Verify latitude is -90 to 90
2. Verify longitude is -180 to 180
3. Check GPS sensor calibration
4. Verify location permissions on mobile
```

### Device Connection Lost
```
Error: "Device offline"
Solution:
1. Check device WiFi connection
2. Restart device
3. Verify firewall isn't blocking
4. Check device battery level
5. Restart backend server
```

## Security Best Practices

### API Security
- [ ] Add API key authentication
- [ ] Implement rate limiting per device
- [ ] Use JWT for session tokens
- [ ] Encrypt sensitive data in transit (HTTPS)
- [ ] Validate all input data

### Data Privacy
- [ ] Don't log sensitive location data
- [ ] Encrypt user data at rest
- [ ] Implement data retention policies
- [ ] Add GDPR compliance measures
- [ ] Use anonymous session IDs

### Device Security
- [ ] Secure boot for Arduino
- [ ] Signed firmware updates
- [ ] Disable debug interfaces in production
- [ ] Implement watchdog timers
- [ ] Regular security audits

## Future Enhancements

1. **ML Improvements**
   - Custom object detection models
   - Obstacle prediction
   - Behavior learning

2. **Hardware Upgrades**
   - 360° camera coverage
   - Depth sensor integration
   - LiDAR support

3. **Software Features**
   - Offline mode with local routing
   - Multi-language support
   - User preference learning
   - Social features for cane users

4. **Integration**
   - Google Maps Platform integration
   - Emergency services integration
   - Health monitoring APIs
   - Smart home device control

---

**Happy Smart Navigation! 🚀**
