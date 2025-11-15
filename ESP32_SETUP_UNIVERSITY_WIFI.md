# ESP32-CAM Setup Guide - University WiFi (Station Mode)

## Overview
This guide configures ESP32-CAM to connect to university WiFi so both team members can access the stream from their own computers.

## Architecture
```
ESP32-CAM (University WiFi) → Backend (Proxy) → Frontend (TensorFlow.js Detection)
    ↓                              ↓                      ↓
10.36.X.X:80/stream         localhost:8000/api      localhost:3000
```

## Step-by-Step Setup

### Step 1: Upload Arduino Code to ESP32-CAM

1. **Open Arduino IDE**
2. **Load the sketch**: `backend/esp32_cam_station_mode.ino`
3. **Configure WiFi credentials** (lines 12-13):
   ```cpp
   const char* ssid = "eduroam";          // Your university WiFi SSID
   const char* password = "your_password"; // Your WiFi password
   ```

4. **For eduroam/enterprise WiFi** (uncomment lines 16-19 and 156-162):
   ```cpp
   #define EAP_IDENTITY "your_username@uwaterloo.ca"
   #define EAP_PASSWORD "your_password"
   ```

5. **Upload to ESP32-CAM**:
   - Connect ESP32-CAM to USB-to-Serial adapter
   - Select Board: "AI Thinker ESP32-CAM"
   - Select Port: COM port of adapter
   - Press "Upload"

6. **Get ESP32's IP Address**:
   - Open Serial Monitor (115200 baud)
   - ESP32 will connect to WiFi and print:
     ```
     ✓ WiFi connected successfully!
     =================================
     IP Address: 10.36.47.XXX
     Stream URL: http://10.36.47.XXX/stream
     =================================
     ```
   - **WRITE DOWN THIS IP ADDRESS!** You'll need it for the backend.

### Step 2: Configure Backend

1. **Create `.env` file** in `backend/` folder:
   ```bash
   cp .env.example .env
   ```

2. **Edit `.env` file** and update ESP32's IP:
   ```env
   ESP32_CAM_URL=http://10.36.47.XXX/stream
   ```
   Replace `10.36.47.XXX` with the IP from Serial Monitor.

3. **Install python-dotenv** (if not already):
   ```bash
   pip install python-dotenv
   ```

4. **Update `main.py`** to load environment variables (add at top):
   ```python
   from dotenv import load_dotenv
   load_dotenv()
   ```

### Step 3: Test the Setup

1. **Test ESP32 directly** (from browser):
   - Go to: `http://10.36.47.XXX/` (ESP32's IP)
   - You should see test page with video stream
   - Go to: `http://10.36.47.XXX/status`
   - Should show JSON status

2. **Start Backend**:
   ```bash
   cd backend
   python main.py
   ```

3. **Test backend proxy**:
   - Go to: `http://localhost:8000/api/stream/status`
   - Should show ESP32 status
   - Go to: `http://localhost:8000/api/stream/mjpeg`
   - Should show video stream

4. **Start Frontend**:
   ```bash
   cd frontend
   pnpm dev
   ```

5. **Test live detection**:
   - Open: `http://localhost:3000`
   - Scroll to "Live Object Detection"
   - Click "Start Camera"
   - Click "Start Detection"
   - Should see bounding boxes and hear audio alerts!

## Troubleshooting

### ESP32 won't connect to WiFi
- Check SSID and password are correct
- For eduroam, make sure EAP identity/password are correct
- Try simpler WiFi first (mobile hotspot)
- Check Serial Monitor for error messages

### Backend can't reach ESP32
- Verify ESP32's IP with `ping 10.36.47.XXX`
- Check ESP32 is still connected (LED should be on)
- Make sure `.env` file has correct IP
- Try accessing ESP32 directly in browser first

### Frontend shows "Failed to load stream"
- Check backend is running (`python main.py`)
- Check backend console for errors
- Try: `http://localhost:8000/api/stream/status`
- Check browser console for network errors

### Stream is slow/laggy
- Reduce frame size in Arduino (line 138):
  ```cpp
  config.frame_size = FRAMESIZE_QVGA;  // Smaller = faster
  ```
- Increase JPEG quality number (line 139):
  ```cpp
  config.jpeg_quality = 15;  // Higher = more compression
  ```
- Check WiFi signal strength

## Network Architecture

**Everyone on University WiFi:**
- ESP32-CAM: `10.36.47.XXX:80/stream`
- Person 1 Backend: `localhost:8000` (proxies ESP32)
- Person 1 Frontend: `localhost:3000` (uses Person 1's backend)
- Person 2 Backend: `localhost:8000` (proxies ESP32)
- Person 2 Frontend: `localhost:3000` (uses Person 2's backend)

Each person runs their own backend+frontend, all connected to the same ESP32-CAM!

## Tips

- Keep Serial Monitor open to see ESP32 status
- ESP32 will auto-reconnect if WiFi drops
- Frame rate ~20 FPS (configurable in Arduino, line 70)
- Stream uses ~1-2 Mbps bandwidth
- TensorFlow.js runs entirely in browser (no backend AI needed)

## What's Next?

Once working, you can:
1. Adjust camera settings (brightness, contrast) in Arduino
2. Change detection thresholds in `live-object-detection.tsx`
3. Add more audio phrases
4. Implement backend AI for heavier models (YOLOv8, SAM)
5. Add detection logging to database
