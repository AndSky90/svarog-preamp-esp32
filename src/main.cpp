#include <Arduino.h>
#include "WiFi.h"
// #include "WebSocketsServer.h"

// Wifi settings block
#define WiFi_rst 0 // WiFi credential reset pin (0 = Boot button on ESP32)

String ssid; // string variable to store ssid
String pss;  // string variable to store password
unsigned long wifiResetMs;

// Wifi retry block
unsigned long wifiDisconnectPrevMs = 0;
unsigned long wifiDisconnectInterval = 30000;

// WebServer block
const char *serverName = "ws://svarog.preamp";
WebSocketsServer webSocket = WebSocketsServer(356);


bool isWifiWsEnabled = false;

bool eepromInitialized();
void writeSsidToFlash(String s);
void writePassToFlash(String s);
String readSsidFromFlash();
String readPassFromFlash();
void eraseSsidAndPass();



// Called when receiving any WebSocket message
void onWebSocketEvent(
    uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED: // Client has disconnected
    Serial.printf("[%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED: // New client has connected
  {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Connection from ", num);
    Serial.println(ip.toString());
  }
  break;
  case WStype_TEXT: // Echo text message back to client
    Serial.printf("[%u] Text: %s\n", num, payload);
    webSocket.sendTXT(num, payload);
    break;

  case WStype_BIN: // For everything else: do nothing
    Serial.printf("[%u] Bin: %s\n", num, payload);

  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
  default:
    break;
  }
}

void setupIfWifiConnected()
{
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  isWifiWsEnabled = true;
}

void setup()
{
  Serial.begin(115200);
  pinMode(WiFi_rst, INPUT);

  // Wire.begin(21, 22, 100000); // sda, scl, clock speed
  // Wire.beginTransmission(MPU_ADDR);
  //  Wire.write(0x6B);  // PWR_MGMT_1 register
  //  Wire.write(0);     // set to zero (wakes up the MPU−6050)
  //  Wire.endTransmission(true);
  //  Serial.println("Setup complete");

  if (!eepromInitialized())
  {
    Serial.println("failed to init EEPROM");
    delay(1000);
  }
  else
  {
    ssid = readSsidFromFlash();
    Serial.print("SSID = " + ssid);
    pss = readPassFromFlash();
    Serial.print("pss = " + pss);
  }

  WiFi.begin(ssid.c_str(), pss.c_str());
  delay(6000); // Wait for a while till ESP connects to WiFi

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi Connected");
    setupIfWifiConnected();
  }
  else
  {
    WiFi.mode(WIFI_AP_STA); // Init WiFi as Station, start SmartConfig
    WiFi.beginSmartConfig();

    Serial.println("Waiting for SmartConfig."); // Wait for SmartConfig packet from mobile

    while (!WiFi.smartConfigDone())
    {
      delay(1000);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("SmartConfig received.");

    // Wait for WiFi to connect to AP
    Serial.println("Waiting for WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(",");
    }

    Serial.print("WiFi Connected. IP Address: ");
    Serial.println(WiFi.localIP());

    // read the connected WiFi SSID and password
    ssid = WiFi.SSID();
    pss = WiFi.psk();
    Serial.print("SSID:" + ssid);
    Serial.print("PSS:" + pss);
    writeSsidToFlash(ssid);
    writePassToFlash(pss);
    Serial.println("Store SSID & PSS in Flash");
    setupIfWifiConnected();
  }
}

void loop()
{

  unsigned long wifiDisconnectCurrentMs = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (wifiDisconnectCurrentMs - wifiDisconnectPrevMs >= wifiDisconnectInterval))
  {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    wifiDisconnectPrevMs = wifiDisconnectCurrentMs;
  }

  if (isWifiWsEnabled)
  {
    webSocket.loop();
  }

  wifiResetMs = millis();
  while (digitalRead(WiFi_rst) == LOW)
  {
    // Wait till boot button is pressed
  }
  // check the button press time if it is greater than 3sec clear wifi cred and restart ESP
  if (millis() - wifiResetMs >= 1000)
  {
    Serial.println("Reseting the WiFi credentials");
    eraseSsidAndPass();
    Serial.println("Wifi credentials erased, Restarting the ESP");
    delay(500);
    ESP.restart();
  }
}
