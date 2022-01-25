#include "esp_camera.h"
#include <WiFi.h>
#include <ArduinoWebsockets.h>

#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#include "camera_pins.h"

const char* ssid = "RoverNet";
const char* password = "Phobos5760";
const char* websocket_server_host = "10.0.0.1";
const uint16_t websocket_server_port = 8888;

const byte trig_pin = 13;
const byte echo_pin = 15;
long duration;
int distance;

using namespace websockets;
WebsocketsClient client;

void setup() {
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

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
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 14; 
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 20;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    return;
  }
  
  WiFi.onEvent(wifiConnected,SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(wifiReconnect, SYSTEM_EVENT_STA_DISCONNECTED); 
  WiFi.begin(ssid, password);
}

int readSensor() {
      digitalWrite(trig_pin, LOW);
      delayMicroseconds(2);
     
      digitalWrite(trig_pin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig_pin, LOW);
      
      duration = pulseIn(echo_pin, HIGH);
      distance = duration * 0.034 / 2; //340m/s, s = t * v
      return distance;
 }


void captureAndSend() {
        camera_fb_t *fb = esp_camera_fb_get();
        if(!fb)
        {
          esp_camera_fb_return(fb);
        }
        client.sendBinary((const char*)fb->buf, fb->len);
        String msg = String(readSensor() , DEC);
        client.send(msg);
        esp_camera_fb_return(fb);
        delay(30); //extra
}

void loop() {
   captureAndSend();
}

void wifiConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  webSocketConnect();
}

void wifiReconnect(WiFiEvent_t event, WiFiEventInfo_t info){
  WiFi.begin(ssid, password);
}
void webSocketConnect(){
   while(!client.connect(websocket_server_host, websocket_server_port, "/")) {
    delay(500);
  }
}
