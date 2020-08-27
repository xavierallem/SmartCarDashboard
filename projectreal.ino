#include <Wire.h>  
#include "SSD1306Wire.h" `

#include <TimeLib.h>
#include "OLEDDisplayUi.h"
#define BLYNK_PRINT Serial
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h>
#include  <ArduinoJson.h>
#include "ESP8266WiFi.h"
char auth[] = "qbkHYD2QySLVFY-PULNurivHlFmvRWQG";

char ssid[] = "andu";
char pass[] = "12345678";
WidgetMap myMap(V1);

const char* Host = "www.unwiredlabs.com";
String endpoint = "/v2/process.php";

String token = "360f7e4741e79e";
String jsonString = "{\n";

double latitude = 0.0;
double longitude = 0.0;
double accuracy = 0.0;
int myindex = 0;
const int API_TIMEOUT = 10000;
int A =1;
#include "images.h"
SSD1306Wire  display(0x3c, D3, D5);
OLEDDisplayUi ui ( &display );

int screenW = 128;
int screenH = 64;
int clockCenterX = screenW/2;
int clockCenterY = ((screenH-16)/2)+16;   // top yellow part is 16 px height
int clockRadius = 23;

// utility function for digital clock display: prints leading 0
String twoDigits(int digits){
  if(digits < 10) {
    String i = '0'+String(digits);
    return i;
  }
  else {
    return String(digits);
  }
}

#define DEMO_DURATION 3000
typedef void (*Demo)(void);

int demoMode = 0;
int counter = 1;
int counter1 = 1;
int a;
const int IRSensor=D0;
int b=1;

void clockOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {

}

void analogClockFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
//  ui.disableIndicator();

  // Draw the clock face
//  display->drawCircle(clockCenterX + x, clockCenterY + y, clockRadius);
  display->drawCircle(clockCenterX + x, clockCenterY + y, 2);
  //
  //hour ticks
  for( int z=0; z < 360;z= z + 30 ){
  //Begin at 0° and stop at 360°
    float angle = z ;
    angle = ( angle / 57.29577951 ) ; //Convert degrees to radians
    int x2 = ( clockCenterX + ( sin(angle) * clockRadius ) );
    int y2 = ( clockCenterY - ( cos(angle) * clockRadius ) );
    int x3 = ( clockCenterX + ( sin(angle) * ( clockRadius - ( clockRadius / 8 ) ) ) );
    int y3 = ( clockCenterY - ( cos(angle) * ( clockRadius - ( clockRadius / 8 ) ) ) );
    display->drawLine( x2 + x , y2 + y , x3 + x , y3 + y);
  }

  // display second hand
  float angle = second() * 6 ;
  angle = ( angle / 57.29577951 ) ; //Convert degrees to radians
  int x3 = ( clockCenterX + ( sin(angle) * ( clockRadius - ( clockRadius / 5 ) ) ) );
  int y3 = ( clockCenterY - ( cos(angle) * ( clockRadius - ( clockRadius / 5 ) ) ) );
  display->drawLine( clockCenterX + x , clockCenterY + y , x3 + x , y3 + y);
  //
  // display minute hand
  angle = minute() * 6 ;
  angle = ( angle / 57.29577951 ) ; //Convert degrees to radians
  x3 = ( clockCenterX + ( sin(angle) * ( clockRadius - ( clockRadius / 4 ) ) ) );
  y3 = ( clockCenterY - ( cos(angle) * ( clockRadius - ( clockRadius / 4 ) ) ) );
  display->drawLine( clockCenterX + x , clockCenterY + y , x3 + x , y3 + y);
  //
  // display hour hand
  angle = hour() * 30 + int( ( minute() / 12 ) * 6 )   ;
  angle = ( angle / 57.29577951 ) ; //Convert degrees to radians
  x3 = ( clockCenterX + ( sin(angle) * ( clockRadius - ( clockRadius / 2 ) ) ) );
  y3 = ( clockCenterY - ( cos(angle) * ( clockRadius - ( clockRadius / 2 ) ) ) );
  display->drawLine( clockCenterX + x , clockCenterY + y , x3 + x , y3 + y);
}

void digitalClockFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  String timenow = String(hour())+":"+twoDigits(minute())+":"+twoDigits(second());
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_24);
  display->drawString(clockCenterX + x , clockCenterY + y, timenow );
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { analogClockFrame, digitalClockFrame };

// how many frames are there?
int frameCount = 2;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { clockOverlay };
int overlaysCount = 1;

void setup() {
 Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);


  myMap.clear();
  Blynk.run();


  
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

	// The ESP is capable of rendering 60fps in 80Mhz mode
	// but that won't give you much time for anything else
	// run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(60);

	// Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(TOP);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  ui.setOverlays(overlays, overlaysCount);

  // Initialising the UI will init the display too.
  ui.init();

  display.flipScreenVertically();

  unsigned long secsSinceStart = millis();
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long epoch = secsSinceStart - seventyYears * SECS_PER_HOUR;
  setTime(epoch);

}
void drawProgressBarDemo() {
  display.setFont(ArialMT_Plain_10);
  int progress = (counter / 5) % 100;
  
  display.drawProgressBar(0, 32, 120, 10, progress);

  
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, String(progress) + "%");
}
void drawFontFaceDemo() {
  
display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 0, "ERROR!!!");



}
void drawImageDemo() {
    
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}



void loop() {
  display.clear();
  if(b==1)
  {int A=1;
  lol();
  display.clear();
    Demo demos[] = {    drawImageDemo};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;
  demos[demoMode]();

  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(10, 128, String(millis()));
 
  display.display();

  if (millis() - timeSinceLastModeSwitch > DEMO_DURATION) {
    demoMode = (demoMode + 1)  % demoLength;
    timeSinceLastModeSwitch = millis();
     b=0;
     delay(5000);
  }
  }
  else
  {
   if(digitalRead(IRSensor)==HIGH) {
  int remainingTimeBudget = ui.update();

 if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    delay(remainingTimeBudget);
    b=0;
    delay(10);

  }
  else{
     b=0;
    }
   }
 else
  {
    display.clear();
    Demo demos[] = {    drawFontFaceDemo};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;
  demos[demoMode]();

  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(10, 128, String(millis()));
 
  display.display();

  if (millis() - timeSinceLastModeSwitch > DEMO_DURATION) {
    demoMode = (demoMode + 1)  % demoLength;
    timeSinceLastModeSwitch = millis();
     b=0;
    
  }
    
  }
  
  }

}
void lol(){
  if (A==1)
  {
    
    Serial.println("Finding Location");
    while (latitude == 0 && longitude == 0)
    {
      my_location();
    }
    latlongi();
    Serial.println("Location Found Sending Location to owner");
    myMap.location(myindex, latitude, longitude, "value");
    Blynk.run();
    Serial.println("Starting ");
    A=0;
    
  }
  else
  {
    
    
    latitude = 0.0;
    longitude = 0.0;
    latlongi();
    myMap.clear();
    Blynk.run();
    Serial.println("Cleared Output");
  }
  delay(500);
}
void latlongi()
{
  Serial.println();
  Serial.print("Latitude = ");
  Serial.println(latitude, 6);
  Serial.print("Longitude = ");
  Serial.println(longitude, 6);
  Serial.print("Accuracy = ");
  Serial.println(accuracy);

}


void my_location()
{
  char bssid[6];
  DynamicJsonBuffer jsonBuffer;

  
  int n = WiFi.scanNetworks();
  Serial.println("scan done");

  if (n == 0 ) {
    
  } else {
    
  }

  // now build the jsonString...
  jsonString = "{\n";
  jsonString += "\"token\" : \"";
  jsonString += token;
  jsonString += "\",\n";
  jsonString += "\"id\" : \"device01\",\n";
  jsonString += "\"wifi\": [\n";
  for (int j = 0; j < n; ++j) {
    jsonString += "{\n";
    jsonString += "\"bssid\" : \"";
    jsonString += (WiFi.BSSIDstr(j));
    jsonString += "\",\n";
    jsonString += "\"signal\": ";
    jsonString += WiFi.RSSI(j);
    jsonString += "\n";
    if (j < n - 1) {
      jsonString += "},\n";
    } else {
      jsonString += "}\n";
    }
  }
  jsonString += ("]\n");
  jsonString += ("}\n");
  // Serial.println(jsonString);

  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(API_TIMEOUT);
  
  if (client.connect(Host, 443)) {
   
    client.println("POST " + endpoint + " HTTP/1.1");
    client.println("Host: " + (String)Host);
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("User-Agent: Arduino/1.0");
    client.print("Content-Length: ");
    client.println(jsonString.length());
    client.println();
    client.print(jsonString);
    delay(500);
  }
  else
  {
    Serial.println(client.connect(Host, 443));
  }

  
  while (client.available()) {
    String line = client.readStringUntil('\r');
    JsonObject& root = jsonBuffer.parseObject(line);
    if (root.success()) {
      latitude    = root["lat"];
      longitude   = root["lon"];
      accuracy    = root["accuracy"];
      
    }
  }

  
  client.stop();
  }
