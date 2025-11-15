# ESP32-CAM & AI Detection Troubleshooting Guide

## Current Issues & Solutions

### Issue 1: ESP32 Camera Not Reachable (504 Gateway Timeout)

**Problem:** Backend cannot connect to ESP32-CAM at `http://172.20.10.2`

**Root Cause:** ESP32 is either:
- Not powered on
- Not connected to your iPhone hotspot
- Has a different IP address than configured

**Solution Steps:**

1. **Upload the Fixed ESP32 Code**
   - Open `backend/esp32_cam_hotspot.ino` in Arduino IDE
   - Verify WiFi credentials are correct:
     ```cpp
     const char* ssid = "iPhone";
     const char* password = "helloniyati";
     ```
   - Upload to ESP32-CAM

2. **Get the Actual IP Address**
   - Open Serial Monitor (Tools → Serial Monitor)
   - Set baud rate to 115200
   - Press the ESP32 reset button
   - Look for output like:
     ```
     ✓ WiFi connected!
     =================================
     IP Address: 172.20.10.X
     Stream URL: http://172.20.10.X/stream
     =================================
     ```

3. **Update Backend Configuration**
   - Edit `backend/.env` file
   - Update the IP address:
     ```env
     ESP32_CAM_URL=http://172.20.10.X/stream
     ```
   - Replace `X` with the actual IP from Serial Monitor

4. **Test Connectivity**
   - In your browser, visit: `http://172.20.10.X/status`
   - You should see JSON with ESP32 status
   - Test snapshot endpoint: `http://172.20.10.X/capture.jpg`

5. **Restart Backend Server**
   - Stop the running backend (Ctrl+C)
   - Start it again: `python main.py`

### Issue 2: AI Model Dependencies Missing

**Problem:** 
```
Error with YOLOv5: No module named 'torch'
```

**Solution:** Dependencies are being installed automatically. If installation fails:

```bash
# Activate virtual environment
cd backend
.\venv\Scripts\Activate.ps1

# Install dependencies
pip install torch torchvision gtts pyttsx3
```

**Note:** PyTorch is a large package (~2GB), first download will take time.

### Issue 3: Camera Configuration

**Status:** ✅ Already correct in your code

The ESP32 code already has the correct AI-Thinker pin configuration:
```cpp
config.pin_d0 = 5;
config.pin_d1 = 18;
config.pin_d2 = 19;
config.pin_d3 = 21;
config.pin_d4 = 36;
config.pin_d5 = 39;
config.pin_d6 = 34;
config.pin_d7 = 35;
config.pin_xclk = 0;
config.pin_pclk = 22;
config.pin_vsync = 25;
config.pin_href = 23;
config.pin_sscb_sda = 26;
config.pin_sscb_scl = 27;
config.pin_pwdn = 32;
config.pin_reset = -1;
config.xclk_freq_hz = 20000000;
config.pixel_format = PIXFORMAT_JPEG;
config.frame_size = FRAMESIZE_QVGA;
config.jpeg_quality = 12;
config.fb_count = 2;
```

## Quick Test Checklist

### ESP32 Hardware Test
- [ ] ESP32-CAM is powered (5V, sufficient current)
- [ ] Camera module is properly seated
- [ ] WiFi antenna connected (if external)
- [ ] Serial Monitor shows IP address
- [ ] Can access `http://ESP32_IP/status` in browser

### Backend Test
- [ ] Virtual environment activated
- [ ] All dependencies installed (`pip list | grep torch`)
- [ ] `.env` file has correct ESP32 IP
- [ ] Backend server running without errors
- [ ] Can access `http://localhost:8000/api/stream/status`

### Frontend Test
- [ ] Frontend dev server running (`npm run dev`)
- [ ] No CORS errors in browser console
- [ ] Can see video stream or snapshots
- [ ] Object detection boxes appear on detected objects

## Testing AI Detection Manually

Once dependencies are installed:

```bash
cd backend
.\venv\Scripts\Activate.ps1
python test_detection.py
```

**Expected Output:**
```
============================================================
Object Detection Test for Visually Impaired Navigation
============================================================

📷 Loading image: test.jpg
   Image size: 275x183

🔍 Detecting objects...
Using cache found in C:\Users\.../.cache\torch\hub\ultralytics_yolov5_master
   Found X objects
   1. person (confidence: 0.89)
   2. chair (confidence: 0.76)

🖼️  Drawing bounding boxes...
   Saved to: test_detected.jpg

📝 Generating description...
   Description: I detected 2 objects. Objects are: person in the center, nearby, chair on the left, far away.

🔊 Converting to audio...
   ✓ Audio saved to: detection_audio.mp3
```

## Common Issues

### ESP32 Won't Connect to WiFi
- Verify SSID and password are correct
- iPhone hotspot must be active
- Try moving ESP32 closer to phone
- Check if hotspot allows new devices

### Backend Timeout Persists
- Verify ESP32 IP hasn't changed
- Test with `curl http://ESP32_IP/capture.jpg`
- Check firewall isn't blocking connection
- Ensure both devices on same network

### AI Model Download Fails
- Check internet connection
- PyTorch downloads models from torch hub (first run only)
- May need to wait 5-10 minutes for first download
- Check disk space (need ~3GB free)

### No Objects Detected
- Test image must have recognizable objects
- YOLO detects 80 common object classes
- Try with a different test image
- Check image isn't corrupted

## Next Steps After Fix

1. **Verify end-to-end flow:**
   - ESP32 captures image
   - Backend receives snapshot
   - AI detects objects
   - Frontend displays results

2. **Test capture trigger:**
   - Software button: `http://ESP32_IP/trigger-capture`
   - Hardware button on GPIO13

3. **Monitor logs:**
   - ESP32: Serial Monitor
   - Backend: Terminal output
   - Frontend: Browser console

## Support Files

- ESP32 Code: `backend/esp32_cam_hotspot.ino`
- Backend Config: `backend/.env`
- AI Test: `backend/test_detection.py`
- Stream Route: `backend/app/routes/stream.py`
