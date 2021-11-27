#include "esp_camera.h"
#include <WiFi.h>
#include <ArduinoWebsockets.h>

#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#include "camera_pins.h"

const char* ssid = "RoverNet";
const char* password = "Phobos5760";
const char* websocket_server_host = "10.0.0.1";
const uint16_t websocket_server_port = 8888;

using namespace websockets;
WebsocketsClient client;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;

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
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 10; 
    config.fb_count = 2;
    Serial.println("It has PSRAM");
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }



  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  while(!client.connect(websocket_server_host, websocket_server_port, "/")) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Websocket Connected!");
}

void loop() {
  camera_fb_t *fb = esp_camera_fb_get();
  if(!fb)
  {
    Serial.println("Camera capture fail!");
    esp_camera_fb_return(fb);
    return;
  }
  if(fb->format != PIXFORMAT_JPEG)
  {
    Serial.println("NON Jpeg data not implemented!");
  }
  client.sendBinary((const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);
  delay(30); //extra
}
