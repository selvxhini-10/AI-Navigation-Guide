/*
 * ESP32-CAM MJPEG Streaming Server - Mobile Hotspot Version
 * Simple and reliable - connects to phone's mobile hotspot
 * No complex authentication needed
 */

#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"

// ===== CONFIGURE YOUR WIFI HERE =====
const char* ssid = "iPhone";      // Your phone's hotspot name
const char* password = "helloniyati";      // Your hotspot password
// ====================================

WebServer server(80);

// Last captured image storage
camera_fb_t* lastCapture = NULL;
unsigned long lastCaptureTime = 0;

// Camera pins for AI-Thinker ESP32-CAM
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// MJPEG Stream handler
void handle_stream() {
  WiFiClient client = server.client();
  
  Serial.println("Stream client connected");

  // Send HTTP headers
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
  client.println("Access-Control-Allow-Origin: *");
  client.println();
  client.flush();

  unsigned long lastFrame = millis();
  int frameCount = 0;

  while(client.connected()) {
    camera_fb_t *fb = esp_camera_fb_get();
    if(!fb) {
      Serial.println("Camera capture failed");
      delay(100);
      continue;
    }

    if(fb->format != PIXFORMAT_JPEG) {
      Serial.println("Non-JPEG frame");
      esp_camera_fb_return(fb);
      continue;
    }

    // Send frame
    client.println("--frame");
    client.println("Content-Type: image/jpeg");
    client.printf("Content-Length: %u\r\n\r\n", fb->len);
    
    size_t written = client.write(fb->buf, fb->len);
    client.println();
    client.flush();
    
    esp_camera_fb_return(fb);
    
    frameCount++;
    if(millis() - lastFrame > 5000) {
      Serial.printf("Streaming: %d frames in 5s\n", frameCount);
      frameCount = 0;
      lastFrame = millis();
    }
    
    if(written != fb->len || !client.connected()) {
      Serial.println("Client disconnected or send failed");
      break;
    }
    
    delay(33);  // ~30 FPS
  }
  
  Serial.println("Stream ended");
}

// Single frame (JPEG) - returns last captured image or captures new one
void handle_capture() {
  // If there's a stored capture, return it
  if (lastCapture != NULL) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Content-Type", "image/jpeg");
    server.sendHeader("Content-Length", String(lastCapture->len));
    server.sendHeader("X-Capture-Time", String(lastCaptureTime));
    server.send(200);
    WiFiClient client = server.client();
    client.write(lastCapture->buf, lastCapture->len);
    Serial.println("Served captured image");
    return;
  }
  
  // Otherwise capture a new frame
  camera_fb_t *fb = esp_camera_fb_get();
  if(!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }
  if(fb->format != PIXFORMAT_JPEG) {
    esp_camera_fb_return(fb);
    server.send(500, "text/plain", "Non-JPEG frame");
    return;
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Content-Type", "image/jpeg");
  server.sendHeader("Content-Length", String(fb->len));
  server.send(200);
  WiFiClient client = server.client();
  client.write(fb->buf, fb->len);
  esp_camera_fb_return(fb);
  Serial.println("Captured and served new image");
}

// Trigger capture via software button
void handle_trigger() {
  // Free previous capture if exists
  if (lastCapture != NULL) {
    esp_camera_fb_return(lastCapture);
  }
  
  // Capture new image
  lastCapture = esp_camera_fb_get();
  lastCaptureTime = millis();
  
  if (lastCapture) {
    Serial.println("📸 Software button pressed - Image captured!");
    Serial.printf("Image size: %u bytes\n", lastCapture->len);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"status\":\"captured\",\"size\":" + String(lastCapture->len) + ",\"time\":" + String(lastCaptureTime) + "}");
  } else {
    Serial.println("❌ Capture failed!");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(500, "application/json", "{\"status\":\"error\",\"message\":\"capture failed\"}");
  }
}

// Debug info endpoint
void handle_debug() {
  String info;
  info.reserve(256);
  info += "{\n";
  info += "  \"ip\": \"" + WiFi.localIP().toString() + "\",\n";
  info += "  \"rssi\": " + String(WiFi.RSSI()) + ",\n";
  info += "  \"heap_free\": " + String(ESP.getFreeHeap()) + ",\n";
  info += "  \"psram_size\": " + String(ESP.getPsramSize()) + ",\n";
  info += "  \"psram_free\": " + String(ESP.getFreePsram()) + ",\n";
  info += "  \"stream_active\": false\n"; // simplistic flag; enhance if needed
  info += "}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", info);
}

// Root handler
void handle_root() {
  String html = "<!DOCTYPE html><html><head>"
                "<meta name='viewport' content='width=device-width, initial-scale=1'>"
                "<title>ESP32-CAM Stream</title>"
                "<style>body{font-family:Arial;text-align:center;margin:20px;}"
                "h2{color:#7c3aed;}img{max-width:100%;border:2px solid #7c3aed;}</style>"
                "</head><body>"
                "<h2>ESP32-CAM MJPEG Stream</h2>"
                "<p>Stream URL: <code>http://" + WiFi.localIP().toString() + "/stream</code></p>"
                "<img src='/stream' style='width:100%;max-width:800px;'>"
                "<p>Connected to: " + String(ssid) + "</p>"
                "</body></html>";
  server.send(200, "text/html", html);
}

// Status endpoint
void handle_status() {
  String json = "{";
  json += "\"status\":\"online\",";
  json += "\"ssid\":\"" + String(ssid) + "\",";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  json += "\"stream_url\":\"http://" + WiFi.localIP().toString() + "/stream\"";
  json += "}";
  
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\n\n=================================");
  Serial.println("ESP32-CAM MJPEG Streaming Server");
  Serial.println("=================================\n");

  // Camera configuration (AI Thinker)
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
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
  config.frame_size = FRAMESIZE_QVGA;  // smaller size for faster streaming
  config.jpeg_quality = 12;
  config.fb_count = 2;

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    while(true) { delay(1000); }
  }
  Serial.println("✓ Camera initialized");

  // Adjust camera settings
  sensor_t * s = esp_camera_sensor_get();
  if (s != NULL) {
    s->set_brightness(s, 0);
    s->set_contrast(s, 0);
    s->set_saturation(s, 0);
  }

  // Connect to WiFi
  Serial.println("\nConnecting to WiFi...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi connected!");
    Serial.println("=================================");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Stream URL: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/stream");
    Serial.println("=================================\n");
  } else {
    Serial.println("\n✗ WiFi connection failed!");
    Serial.println("Check SSID and password");
    while(true) { delay(1000); }
  }

  // Start web server
  server.on("/", handle_root);
  server.on("/stream", handle_stream);
  server.on("/status", handle_status);
  server.on("/capture.jpg", handle_capture);
  server.on("/trigger-capture", handle_trigger);  // Software capture trigger
  server.on("/debug", handle_debug);
  server.begin();
  
  // Setup button interrupt
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
  
  Serial.println("✓ Server started on port 80");
  Serial.println("✓ Button configured on GPIO13");
  Serial.println("Ready to capture! Press button to take photo.\n");
}

void loop() {
  server.handleClient();
  
  // Check if button was pressed
  if (captureRequested) {
    captureRequested = false;
    
    // Free previous capture if exists
    if (lastCapture != NULL) {
      esp_camera_fb_return(lastCapture);
    }
    
    // Capture new image
    lastCapture = esp_camera_fb_get();
    if (lastCapture) {
      Serial.println("📸 Button pressed - Image captured!");
      Serial.printf("Image size: %u bytes\n", lastCapture->len);
    } else {
      Serial.println("❌ Capture failed!");
    }
  }
  
  // Auto-reconnect if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    WiFi.reconnect();
    delay(1000);
  }
}
