# Smart Navigation Cane (SNC)

AI-Powered Walking Stick With Object Detection & Live Google Maps Guidance

## 🎯 Project Overview

Smart Navigation Cane is a complete assistance system for visually impaired users combining:

- **🎥 Real-time Object Detection**: ESP32-CAM captures video, AI detects obstacles, pedestrians, vehicles
- **🗺️ Live Navigation**: Google Maps integration with turn-by-turn audio guidance
- **📱 Web Application**: React.js and Tailwind CSS web app for user interaction
- **⚙️ Embedded System**: Arduino + ESP32-CAM microcontroller integration

## 🏗️ System Architecture

```
Smart Navigation Cane (Wearable Hardware)
├── ESP32-CAM (Camera Module)
│   └── Real-time video capture @ 30 FPS
└── Arduino (Microcontroller)
    ├── GPS/GNSS Module
    ├── Accelerometer/Gyro
    ├── Button Input
    └── Vibration Motor

                    ↓ WiFi/Network ↓

FastAPI Backend Server (Python)
├── Object Detection Processing
├── Navigation Management
├── Google Maps Integration
├── Device Coordination
└── Session Management

                    ↓ REST API ↓

Mobile App (React.js)
├── Real-time Alerts (detection)
├── Voice Guidance (navigation)
├── Visual Display
└── Device Status Monitor
```

## 📦 Project Structure

```
AI-Navigation-Guide /
├── backend/                      # FastAPI Python Backend
│   ├── main.py                  # Application entry point
│   ├── requirements.txt         # Dependencies
│   ├── .env.example
│   └── app/
│       ├── config.py
│       ├── models.py            # SNC data models
│       └── routes/
│           ├── health.py
│           ├── detection.py     # Object detection endpoints
│           ├── navigation.py    # GPS guidance endpoints
│           └── device.py        # Device management endpoints
│
├── frontend/                     # Mobile App
│   ├── Home.tsx
│   ├── Source/
│   │   └── SmartNavigationCane/
│   │       ├── Public/
│   │       │   └── SNCBackendManager.h
│   │       └── Private/
│   │           └── SNCBackendManager.cpp
│   └── Content/                 # Game assets
│
├── SNC_COMPLETE_GUIDE.md         # Comprehensive guide
├── INTEGRATION_GUIDE.md          # Backend-frontend connection
└── README.md                     # This file
```

## 🚀 Quick Start

### Backend Setup (FastAPI)

```bash
# Navigate to backend
cd backend

# Create virtual environment
python -m venv venv
.\venv\Scripts\Activate.ps1

# Install dependencies
pip install -r requirements.txt

# Run backend
python main.py
```

Backend runs on: `http://localhost:8000`
- API Docs: `http://localhost:8000/docs`
- ReDoc: `http://localhost:8000/redoc`

### Hardware Setup (ESP32-CAM + Arduino)

```bash
# ESP32-CAM: Flash with camera streaming firmware
# Arduino: Upload with sensor reading sketch
# Configure WiFi SSID/password
# Devices will automatically connect to backend
```

## 🔌 API Endpoints Quick Reference

### Object Detection
```
POST   /api/detection/camera/upload    Upload frame from ESP32-CAM
GET    /api/detection/latest           Get latest detection results
POST   /api/detection/process          Store detection results
GET    /api/detection/{frame_id}       Get specific frame detection
```

### Navigation & GPS
```
POST   /api/navigation/start-route              Start navigation
GET    /api/navigation/route/{route_id}         Get route status
PUT    /api/navigation/route/{id}/update-loc    Update location
POST   /api/navigation/obstacle-alert           Report obstacle
GET    /api/navigation/obstacles                Get active obstacles
```

### Device Management
```
POST   /api/device/register                    Register device
POST   /api/device/heartbeat/{id}              Send heartbeat
GET    /api/device/{device_id}                 Get device status
POST   /api/device/{id}/error                  Report error
```

### Sessions
```
POST   /api/session/start                      Start session
POST   /api/session/{id}/add-device            Add device
POST   /api/session/{id}/end                   End session
```

See [SNC_COMPLETE_GUIDE.md](./SNC_COMPLETE_GUIDE.md) for detailed API reference.

## 📱 Mobile App Features

### Real-time Detection Alerts
- Detects: pedestrians, vehicles, obstacles, hazards
- Confidence-based filtering
- Distance estimation
- Audio/haptic feedback

### Turn-by-Turn Navigation
- Google Maps integration
- Voice-guided directions
- Progress tracking
- Distance/time remaining
- Waypoint navigation

### Device Status Monitoring
- Battery level indicators
- Connection status
- Sensor health
- Error notifications

### Safety Features
- Emergency stop button
- Low battery alerts
- Connection loss handling
- Obstacle avoidance suggestions

## ⚡ Key Technologies

### Backend
- **Framework**: FastAPI 0.104.1
- **Server**: Uvicorn
- **Language**: Python 3.9+
- **Data**: Pydantic models
- **ML**: YOLOv8 integration ready

### Frontend
- **Engine**: React.js Web Application
- **Language**: C++
- **Platforms**: iOS 14.0+, Android 8.0+
- **Networking**: HTTP/REST

### Hardware
- **Camera**: ESP32-CAM (OV2640)
- **Processor**: Arduino-compatible MCU
- **Sensors**: GPS, accelerometer, gyro
- **Connectivity**: WiFi

## 📊 Data Models

### DetectedObject
```json
{
  "label": "person",
  "confidence": 0.95,
  "distance": 2.5,
  "position": {"x": 160, "y": 200}
}
```

### GPSLocation
```json
{
  "latitude": 40.7128,
  "longitude": -74.0060,
  "altitude": 10.5,
  "heading": 90
}
```

### NavigationInstruction
```json
{
  "instruction": "Turn right on Market Avenue",
  "distance_meters": 500,
  "duration_seconds": 300
}
```

## 🔧 Development Workflow

### Backend Development
```bash
# Terminal 1: Run backend with auto-reload
uvicorn main:app --reload --host 0.0.0.0 --port 8000

# Terminal 2: Monitor logs
# Check http://localhost:8000/docs for live testing
```

### Frontend Development
```bash
# Edit C++ in Visual Studio
```

### Hardware Development
```bash
# Arduino IDE / PlatformIO
# Edit firmware for ESP32-CAM
# Upload and monitor serial output
```

## 📖 Documentation

- **[SNC_COMPLETE_GUIDE.md](./SNC_COMPLETE_GUIDE.md)** - Full system guide with API reference
- **[INTEGRATION_GUIDE.md](./INTEGRATION_GUIDE.md)** - Backend-frontend integration steps
- **[backend/README.md](./backend/README.md)** - Backend API details
- **[frontend/README.md](./frontend/README.md)** - Frontend setup and deployment

## 🐛 Troubleshooting

### Backend Connection Failed
```
Check: backend running on http://localhost:8000/health
Fix: Verify port 8000 is not in use, firewall allows connections
```

### No Detections Showing
```
Check: ESP32-CAM uploading frames to /api/detection/camera/upload
Fix: Verify camera is connected and WiFi connection is active
```

### Navigation Not Working
```
Check: GPS coordinates are valid (lat -90 to 90, lon -180 to 180)
Fix: Enable location permissions on mobile app
```

### Device Offline
```
Check: Device battery level, WiFi connectivity
Fix: Restart device, check backend logs
```

See [SNC_COMPLETE_GUIDE.md](./SNC_COMPLETE_GUIDE.md#troubleshooting) for more.

## 🔒 Security Considerations

- [ ] Use HTTPS in production
- [ ] Add API key authentication
- [ ] Implement JWT for sessions
- [ ] Validate all inputs
- [ ] Rate limit API endpoints
- [ ] Encrypt sensitive data
- [ ] Use signed firmware updates
- [ ] Implement device authentication

## 🚢 Deployment

### Backend
```bash
# Docker
docker build -t snc-backend .
docker run -p 8000:8000 snc-backend

# Or use cloud: AWS ECS, Azure App Service, GCP Cloud Run
```

### Mobile App
```bash
# iOS: Xcode → Archive → Upload to App Store
# Android: Android Studio → Build → Generate Signed Bundle → Google Play
```

### Hardware
```bash
# Flash firmware via Arduino IDE/PlatformIO
# Configure WiFi and backend URL
# Deploy to production devices
```

## 🎓 Next Steps

1. **Integrate Object Detection Model**
   - Use YOLOv8 Nano for edge inference
   - Quantize for ESP32 deployment

2. **Add Real Google Maps API**
   - Replace mock navigation with real routes
   - Implement address autocomplete

3. **Enhance User Experience**
   - Add user preferences/settings
   - Implement feedback system
   - Create user profiles

4. **Scale Infrastructure**
   - Set up database (PostgreSQL)
   - Implement caching (Redis)
   - Add monitoring (Prometheus, Grafana)

5. **Mobile App Polish**
   - Add voice input for commands
   - Implement offline mode
   - Create accessibility features
   - Add emergency SOS

## 📝 License

MIT

## 👥 Support & Contributing

For questions, issues, or contributions:
1. Check the documentation in `SNC_COMPLETE_GUIDE.md`
2. Review backend logs: `backend/main.py` output
3. Check device logs: Arduino Serial Monitor
4. Review API: `http://localhost:8000/docs`

---

**Smart Navigation Cane - Empowering Independence Through Technology 🚀**
