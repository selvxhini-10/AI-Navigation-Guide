# Smart Navigation Cane (SNC) Backend API

FastAPI-based backend for AI-powered walking stick with object detection and live GPS guidance.

## Project Overview

The Smart Navigation Cane system consists of:
- **Hardware**: ESP32-CAM for video capture, Arduino processor for sensor integration
- **Backend**: FastAPI API server for processing and coordination
- **Frontend**: Mobile app (iOS/Android) for user interface and navigation

## Project Structure

```
backend/
├── main.py                   # Application entry point
├── requirements.txt          # Python dependencies
├── .env.example             # Environment variables template
├── README.md                # This file
└── app/
    ├── config.py            # Configuration settings
    ├── models.py            # Pydantic data models for SNC
    └── routes/
        ├── health.py        # Health check endpoints
        ├── detection.py     # Camera and object detection endpoints
        ├── navigation.py    # GPS and Google Maps guidance endpoints
        └── device.py        # Arduino/ESP32-CAM device management
```

## Setup Instructions

### 1. Install Python 3.9+

Ensure you have Python 3.9 or higher installed.

### 2. Create Virtual Environment

```bash
python -m venv venv
.\venv\Scripts\Activate.ps1  # Windows PowerShell
# or
source venv/bin/activate    # Linux/Mac
```

### 3. Install Dependencies

```bash
pip install -r requirements.txt
```

### 4. Configure Environment

```bash
cp .env.example .env
# Edit .env with your configuration
```

### 5. Run the Server

```bash
python main.py
```

The API will be available at `http://localhost:8000`
- Interactive API docs: `http://localhost:8000/docs`
- Alternative docs: `http://localhost:8000/redoc`

## API Modules

### Health Check Endpoints
- `GET /health` - Health status
- `GET /ready` - Readiness status

### Object Detection (Camera Input)
- `POST /api/detection/camera/upload` - Upload frame from ESP32-CAM
- `POST /api/detection/process` - Store object detection results
- `GET /api/detection/latest` - Get latest detection results
- `GET /api/detection/{frame_id}` - Get detection for specific frame
- `GET /api/detection/device/{device_id}` - Get device detection history
- `DELETE /api/detection/{frame_id}` - Delete detection record

### Navigation & GPS Guidance (Google Maps Integration)
- `POST /api/navigation/start-route` - Start navigation to destination
- `GET /api/navigation/route/{route_id}` - Get current route status
- `PUT /api/navigation/route/{route_id}/update-location` - Update user location and get next instruction
- `POST /api/navigation/route/{route_id}/end` - End navigation
- `GET /api/navigation/active-routes` - Get all active routes
- `POST /api/navigation/obstacle-alert` - Report obstacle/hazard
- `GET /api/navigation/obstacles` - Get active obstacles

### Device Management (Arduino + ESP32-CAM)
- `POST /api/device/register` - Register new device
- `POST /api/device/heartbeat/{device_id}` - Send device heartbeat
- `GET /api/device/{device_id}` - Get device status
- `GET /api/device/list` - List all devices
- `PUT /api/device/{device_id}/update-sensors` - Update active sensors
- `POST /api/device/{device_id}/error` - Report device error
- `POST /api/device/{device_id}/clear-errors` - Clear device errors
- `POST /api/session/start` - Start navigation session
- `GET /api/session/{session_id}` - Get session details
- `POST /api/session/{session_id}/add-device` - Add device to session
- `POST /api/session/{session_id}/end` - End session

## Data Models

### DetectedObject
```json
{
  "object_id": "obj_001",
  "label": "person",
  "confidence": 0.95,
  "bbox": {"x": 100, "y": 150, "width": 50, "height": 100},
  "distance": 2.5,
  "position": {"x": 160, "y": 200}
}
```

### ObjectDetectionFrame
```json
{
  "frame_id": "frame_001",
  "timestamp": "2024-11-15T10:30:00",
  "image_path": "saved_frames/frame_001.jpg",
  "objects": [...],
  "frame_size": {"width": 640, "height": 480},
  "processing_time_ms": 150
}
```

### GPSLocation
```json
{
  "latitude": 40.7128,
  "longitude": -74.0060,
  "altitude": 10.5,
  "accuracy": 5.0,
  "heading": 90,
  "speed": 1.5
}
```

### NavigationGuidance
```json
{
  "route_id": "route_001",
  "origin": {...},
  "destination": {...},
  "instructions": [
    "Head north on Main Street for 500 meters",
    "Turn right onto Market Avenue"
  ],
  "distance_remaining": 1200,
  "duration_remaining": 900,
  "current_step": 0,
  "waypoints": [...]
}
```

### DeviceStatus
```json
{
  "device_id": "esp32_001",
  "timestamp": "2024-11-15T10:30:00",
  "device_type": "esp32cam",
  "battery_level": 85.5,
  "connectivity": "online",
  "last_heartbeat": "2024-11-15T10:29:50",
  "active_sensors": ["camera", "accelerometer"],
  "errors": []
}
```

## Hardware Integration

### ESP32-CAM Setup
1. Flash with appropriate firmware for video streaming
2. Configure WiFi connection to same network as backend
3. Set API endpoint URL in device configuration
4. Device will periodically upload frames via `/api/detection/camera/upload`

### Arduino Setup
1. Configure with ESP32-CAM communication interface
2. Implement sensor reading (accelerometer, button input, etc.)
3. Send device status via `/api/device/heartbeat/{device_id}`
4. Report errors via `/api/device/{device_id}/error`

## Mobile App Integration

The mobile app connects to:
1. **Detection Module**: Receive real-time object detection alerts
2. **Navigation Module**: Get turn-by-turn directions
3. **Device Module**: Monitor device status and battery

Example flow:
```
Mobile App → POST /api/session/start → Session Created
Mobile App → POST /api/device/register → Arduino/ESP32 Registered
Mobile App → POST /api/navigation/start-route → Navigation Started
ESP32-CAM → POST /api/detection/camera/upload → Frame Uploaded
Backend → Processes frame → Detects objects
Mobile App → GET /api/detection/latest → Receives alerts
```

## Development

### Running with Hot Reload

```bash
uvicorn main:app --reload --host 0.0.0.0 --port 8000
```

### API Documentation

Once the server is running, visit:
- Swagger UI: http://localhost:8000/docs
- ReDoc: http://localhost:8000/redoc

## Object Detection Integration

The backend accepts processed detection results. To integrate with an AI model:

1. **YOLOv8 Integration** (recommended for embedded):
```python
from ultralytics import YOLO
model = YOLO("yolov8n.pt")  # Nano model for edge devices
results = model(frame)
```

2. **TensorFlow Lite** (for Arduino/ESP32):
   - Use lightweight quantized models
   - Run inference on device or send to backend

3. **POST detection results to backend**:
```bash
curl -X POST "http://localhost:8000/api/detection/process" \
  -H "Content-Type: application/json" \
  -d '{...detection_frame_json...}'
```

## Google Maps Integration (Production)

For production, integrate Google Maps API:

```python
import googlemaps

gmaps = googlemaps.Client(key='YOUR_API_KEY')
directions_result = gmaps.directions(origin, destination)
```

Currently using mock data. Replace in `backend/app/routes/navigation.py`

## Error Handling

The backend returns appropriate HTTP status codes:

| Code | Meaning | Example |
|------|---------|---------|
| 200 | OK | Successful GET/PUT request |
| 201 | Created | Successful POST request |
| 204 | No Content | Successful DELETE request |
| 400 | Bad Request | Invalid JSON payload |
| 404 | Not Found | Resource not found |
| 500 | Server Error | Unhandled exception |

## Performance Tips

1. Use **YOLOv8 Nano** for real-time detection on edge devices
2. Implement **frame batching** to reduce API calls
3. Cache **navigation routes** to reduce redundant API requests
4. Use **compression** for image transmission (JPEG)
5. Implement **exponential backoff** for device connectivity

## CORS Configuration

For local development, all origins are allowed. For production:

```python
# In main.py
allow_origins=[
    "https://yourdomain.com",
    "https://app.yourdomain.com",
]
```

## Database Integration (Future)

Currently using in-memory storage. To add persistence:

1. Install SQLAlchemy: `pip install sqlalchemy`
2. Create database models in `app/database.py`
3. Update routes to use database instead of in-memory storage
4. Recommended: PostgreSQL for scalability

## Security Considerations (Production)

- [ ] Replace `CORS_ORIGINS=*` with specific domains
- [ ] Add API key authentication
- [ ] Implement JWT tokens for sessions
- [ ] Use HTTPS/TLS for all communications
- [ ] Add rate limiting
- [ ] Validate all image uploads
- [ ] Store sensitive data (API keys) securely
- [ ] Implement device authentication

## Troubleshooting

### Device Connection Issues
- Verify devices are on same network as backend
- Check firewall settings
- Monitor `/api/device/heartbeat` responses

### Object Detection Not Working
- Verify camera frames are being uploaded
- Check `/api/detection/latest` for recent frames
- Validate detection model is running

### Navigation Issues
- Verify GPS coordinates are valid (latitude -90 to 90, longitude -180 to 180)
- Check Google Maps API key (if using real API)
- Monitor `/api/navigation/active-routes` for active sessions

### High Latency
- Check network bandwidth
- Reduce frame resolution
- Batch multiple operations
- Consider edge processing (on ESP32)

## License

MIT

## Support

For issues:
1. Check the troubleshooting section above
2. Review API documentation at `/docs`
3. Check device logs
4. Review backend logs in console
