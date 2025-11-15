/*
 * ESP32-CAM MJPEG Streaming Server - Station Mode
 * Connects to university/home WiFi network
 * Accessible by all devices on the same network
 */

#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"
#include "esp_wpa2.h"  // For eduroam WPA2 Enterprise

// ===== CONFIGURE YOUR WIFI HERE =====
// For eduroam (WPA2 Enterprise)
const char* ssid = "eduroam";
const char* EAP_IDENTITY = "youruserid@uwaterloo.ca";  // Your UWaterloo email
const char* EAP_USERNAME = "youruserid@uwaterloo.ca";  // Same as identity
const char* EAP_PASSWORD = "your_password";            // Your UWaterloo password
// ====================================

// For regular WiFi (comment out the above and use this instead)
/*
const char* ssid = "YourWiFiName";
const char* password = "YourPassword";
*/

WebServer server(80);
camera_config_t config;

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

  String boundary = "frameboundary";
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: multipart/x-mixed-replace; boundary=" + boundary);
  client.println("Access-Control-Allow-Origin: *");  // Enable CORS
  client.println("Connection: close");
  client.println();

  while(client.connected()) {
    camera_fb_t *fb = esp_camera_fb_get();
    if(!fb) {
      Serial.println("Camera capture failed");
      delay(100);
      continue;
    }

    client.println("--" + boundary);
    client.println("Content-Type: image/jpeg");
    client.print("Content-Length: ");
    client.println(fb->len);
    client.println();
    client.write(fb->buf, fb->len);
    client.println();

    esp_camera_fb_return(fb);
    delay(50);  // ~20 FPS
  }
  
  Serial.println("Client disconnected");
}

// Root handler - Simple test page
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

  // Camera configuration
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Frame size and quality
  config.frame_size = FRAMESIZE_VGA;  // 640x480 - good balance
  config.jpeg_quality = 12;           // 10-63, lower = better quality
  config.fb_count = 2;                // Double buffering

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    ESP.restart();
  }
  Serial.println("✓ Camera initialized successfully");

  // Adjust camera settings for better quality
  sensor_t * s = esp_camera_sensor_get();
  if (s != NULL) {
    s->set_brightness(s, 0);     // -2 to 2
    s->set_contrast(s, 0);       // -2 to 2
    s->set_saturation(s, 0);     // -2 to 2
    s->set_hmirror(s, 0);        // 0 = disable, 1 = enable
    s->set_vflip(s, 0);          // 0 = disable, 1 = enable
  }

  // Connect to WiFi
  Serial.println("\nConnecting to eduroam WiFi...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Identity: ");
  Serial.println(EAP_IDENTITY);
  
  // Configure WPA2 Enterprise for eduroam
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_USERNAME, strlen(EAP_USERNAME));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wifi_sta_wpa2_ent_enable();
  
  WiFi.begin(ssid);
  
  /* 
   * For regular WiFi (WPA2-PSK), use this instead:
   * 
   * WiFi.mode(WIFI_STA);
   * WiFi.begin(ssid, password);
   */

  // Wait for connection with timeout
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {  // Increased timeout for eduroam
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi connected successfully!");
    Serial.println("=================================");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Stream URL: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/stream");
    Serial.print("Status URL: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/status");
    Serial.println("=================================\n");
  } else {
    Serial.println("\n✗ WiFi connection failed!");
    Serial.println("Check your SSID and password");
    ESP.restart();
  }

  // Configure web server routes
  server.on("/", handle_root);
  server.on("/stream", handle_stream);
  server.on("/status", handle_status);
  
  // Start server
  server.begin();
  Serial.println("✓ HTTP server started on port 80");
  Serial.println("\nReady to stream! Open the IP address in your browser.\n");
}

void loop() {
  server.handleClient();
  
  // Reconnect WiFi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    WiFi.reconnect();
    delay(1000);
  }
}
