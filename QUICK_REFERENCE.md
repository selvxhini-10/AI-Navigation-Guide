# Smart Navigation Cane - Quick Reference

## 🎯 System at a Glance

**Purpose**: AI-powered walking stick with real-time object detection and GPS navigation

**Hardware**: ESP32-CAM (camera) + Arduino (sensors)
**Backend**: FastAPI (Python)
**Frontend**: Unreal Engine 5.4 (C++)

## 🚀 30-Second Setup

```bash
# Backend
cd backend && python -m venv venv
.\venv\Scripts\Activate.ps1 && pip install -r requirements.txt && python main.py

# Frontend
# Open frontend/UnrealMobileAR.uproject in Unreal Editor 5.4
# Create Blueprint: BP_SNCCharacter from SNCBackendManager
# Set Backend URL to http://localhost:8000
# Press Play
```

Backend: http://localhost:8000/docs

## 📡 API Endpoints (Quick Reference)

### Detection (Object Recognition)
```
POST /api/detection/camera/upload    ← ESP32-CAM sends frames
GET  /api/detection/latest           ← Get latest detections
POST /api/detection/process          ← Process detection results
```

### Navigation (GPS Guidance)
```
POST /api/navigation/start-route           ← Begin navigation
PUT  /api/navigation/route/{id}/update-loc ← Update location
GET  /api/navigation/route/{id}            ← Get current instruction
POST /api/navigation/obstacle-alert        ← Report obstacle
```

### Device Management (Hardware)
```
POST /api/device/register              ← Register ESP32/Arduino
POST /api/device/heartbeat/{id}        ← Send keep-alive ping
GET  /api/device/{id}                  ← Check device status
POST /api/device/{id}/error            ← Report device error
```

### Sessions (User Sessions)
```
POST /api/session/start              ← Start navigation session
POST /api/session/{id}/add-device    ← Add device to session
POST /api/session/{id}/end           ← End session
```

## 💻 Unreal C++ API (Quick Reference)

```cpp
// Initialize
USNCBackendManager::InitializeSNCBackend(TEXT("http://localhost:8000"));

// Detection
USNCBackendManager::GetLatestDetection(OnDetectionReceived);

// Navigation
USNCBackendManager::StartNavigation(lat1, lon1, lat2, lon2, OnComplete);
USNCBackendManager::UpdateUserLocation(routeId, currentLat, currentLon, OnUpdate);
USNCBackendManager::ReportObstacle(TEXT("obstacle"), TEXT("high"), TEXT("Pothole"), OnComplete);

// Device
USNCBackendManager::RegisterDevice(TEXT("esp32_001"), TEXT("esp32cam"), OnComplete);
USNCBackendManager::SendDeviceHeartbeat(TEXT("esp32_001"), 85.5, OnComplete);
USNCBackendManager::GetDeviceStatus(TEXT("esp32_001"), OnComplete);

// Session
USNCBackendManager::StartSession(TEXT("user_001"), OnComplete);
USNCBackendManager::AddDeviceToSession(sessionId, deviceId, OnComplete);
USNCBackendManager::EndSession(sessionId, OnComplete);
```

## 📁 File Structure Essentials

```
backend/
├── main.py              ← Start here! Run to start server
├── app/models.py        ← Data structures (edit for new data types)
└── app/routes/
    ├── detection.py     ← Camera/ML endpoints
    ├── navigation.py    ← GPS/Maps endpoints
    └── device.py        ← Hardware endpoints

frontend/
├── UnrealMobileAR.uproject   ← Main project file
└── Source/SmartNavigationCane/
    ├── Public/SNCBackendManager.h     ← Add functions here
    └── Private/SNCBackendManager.cpp  ← Implement functions here
```

## 🔌 Data Structures

### Detection Frame
```json
{
  "frame_id": "frame_001",
  "objects": [
    {"label": "person", "confidence": 0.95, "distance": 2.5}
  ]
}
```

### GPS Location
```json
{
  "latitude": 40.7128,
  "longitude": -74.0060,
  "altitude": 10.0
}
```

### Navigation Instruction
```json
{
  "current_instruction": "Turn right",
  "distance_remaining": 500,
  "duration_remaining": 300
}
```

## ⚙️ Configuration Files

### Backend (.env)
```
DATABASE_URL=sqlite:///./ar_backend.db
API_PORT=8000
API_HOST=0.0.0.0
DEBUG=True
CORS_ORIGINS=*
```

### Frontend (Blueprint)
```
Backend URL: http://localhost:8000
Device ID: esp32_001 (or your device ID)
Session ID: (created at runtime)
```

### Hardware (Arduino/ESP32)
```
WiFi SSID: YOUR_NETWORK
WiFi Password: YOUR_PASSWORD
Backend URL: http://192.168.1.100:8000
Device Type: esp32cam or arduino
```

## 🐛 Common Issues & Fixes

| Problem | Check | Fix |
|---------|-------|-----|
| Backend won't start | Port 8000 free? | `netstat -ano \| findstr :8000` |
| No connection | Backend running? | Visit `http://localhost:8000/health` |
| Device offline | WiFi connected? | Check device logs |
| No detections | Camera uploading? | POST to `/api/detection/camera/upload` |
| Navigation errors | GPS valid? | Lat: -90 to 90, Lon: -180 to 180 |

## 📊 API Response Codes

| Code | Meaning |
|------|---------|
| 200 | OK - Success |
| 201 | Created - Resource created |
| 204 | No Content - Deleted |
| 400 | Bad Request - Invalid data |
| 404 | Not Found - Resource missing |
| 500 | Server Error - Backend issue |

## 🔒 Security Checklist

- [ ] Use HTTPS in production
- [ ] Add API keys for device authentication
- [ ] Validate all input data
- [ ] Rate limit API endpoints
- [ ] Encrypt sensitive data
- [ ] Use environment variables for secrets
- [ ] Regular security audits

## 🧪 Testing Flow

```
1. Start Backend
   python main.py

2. Check Health
   curl http://localhost:8000/health

3. Test Detection
   curl -X POST http://localhost:8000/api/detection/process

4. Test Navigation
   curl -X POST http://localhost:8000/api/navigation/start-route

5. Test Device
   curl -X POST http://localhost:8000/api/device/register

6. Open Frontend
   Unreal Editor → Play
```

## 📚 Documentation Links

| Document | Purpose |
|----------|---------|
| `SNC_COMPLETE_GUIDE.md` | Full system guide + API reference |
| `INTEGRATION_GUIDE.md` | Backend-frontend connection |
| `backend/README.md` | Backend setup + details |
| `frontend/README.md` | Frontend setup + deployment |

## 🚀 Deployment Checklist

### Backend
- [ ] Set `DEBUG=False` in .env
- [ ] Use `HTTPS` instead of HTTP
- [ ] Add proper `CORS_ORIGINS`
- [ ] Set up database (PostgreSQL)
- [ ] Add API key authentication
- [ ] Deploy to cloud (AWS/Azure/GCP)

### Frontend
- [ ] Update `Backend URL` to production server
- [ ] Test on real devices (iOS/Android)
- [ ] Submit to App Store / Google Play
- [ ] Set up crash reporting
- [ ] Monitor usage analytics

### Hardware
- [ ] Flash final firmware
- [ ] Configure production WiFi
- [ ] Set backend URL
- [ ] Test end-to-end
- [ ] Deploy to devices

## 💡 Pro Tips

1. **Use SwaggerUI for testing**: `http://localhost:8000/docs`
2. **Check logs**: Backend logs in terminal
3. **Monitor devices**: `GET /api/device/list`
4. **Cache routes locally**: Reduce API calls
5. **Batch detections**: Send multiple frames at once
6. **Use webhooks**: For real-time updates (future)

## 🆘 Emergency Commands

```bash
# Restart backend
python main.py

# Check if port 8000 is in use
netstat -ano | findstr :8000

# Kill process on port 8000 (Windows)
taskkill /PID <PID> /F

# Clear all devices
curl -X GET http://localhost:8000/api/device/list

# Check latest detection
curl http://localhost:8000/api/detection/latest

# List active routes
curl http://localhost:8000/api/navigation/active-routes
```

---

**Quick Reference v1.0 - Last Updated: Nov 2024**
