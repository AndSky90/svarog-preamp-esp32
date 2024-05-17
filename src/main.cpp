#include <Arduino.h>
#include "WiFi.h"
// #include "WebSocketsServer.h"
#include <ArduinoWebsockets.h>

// Wifi settings block

const char *ssid; // wifi ssid
const char *pss;  // wifi pass
const char *serverName = "ws://svarog.preamp";

// Wifi retry block
unsigned long wifiDisconnectPrevMs = 0;
unsigned long wifiDisconnectInterval = 30000;

using namespace websockets;

// WebServer block

WebsocketsServer server;

bool isWifiWsEnabled = false;
bool haveClient = false;

// Interfaces

bool eepromInitialized();
void writeSsidToFlash(String s);
void writePassToFlash(String s);
String readSsidFromFlash();
String readPassFromFlash();
void finishFlash();
void pinSetup();
void loopResetWifiPin();

//без сна, тк в первом же стейте отключается блютус
//Methods

void loopWebSocket()
{
  auto client = server.accept();
  if (client.available())
  {
    auto msg = client.readBlocking();

    Serial.println(msg.data()); // message
    //  Serial.println(msg.rawData()); //message raw data
    //  Serial.println(msg.c_str()); //message raw data

    client.send("Echo: " + msg.data());

    client.close();
  }

  delay(100);
}
// Called when receiving any WebSocket message
/* void onWebSocketEvent(
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
    break;

  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
  default:
    break;
  }
} */

void setupIfWifiConnected()
{
  server.listen(356);
  Serial.print("Is server live? ");
  isWifiWsEnabled = server.available();
  Serial.println(isWifiWsEnabled);

}

void setup()
{
  pinSetup();
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  // Wire.begin(21, 22, 100000); // sda, scl, clock speed
  // Wire.beginTransmission(MPU_ADDR);
  //  Wire.write(0x6B);  // PWR_MGMT_1 register
  //  Wire.write(0);     // set to zero (wakes up the MPU−6050)
  //  Wire.endTransmission(true);
  //  Serial.println("Setup complete");

  if (!eepromInitialized())
  {
    Serial.println("failed to init EEPROM");
    delay(500);
  }
  else
  {
    ssid = readSsidFromFlash().c_str();
    Serial.print("SSID = " + String(ssid));
    pss = readPassFromFlash().c_str();
    Serial.print("pss = " + String(pss));
  }

  WiFi.begin(ssid, pss);
  delay(5000); // Wait for a while till ESP connects to WiFi

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
    ssid = WiFi.SSID().c_str();
    pss = WiFi.psk().c_str();
    Serial.print("SSID = " + WiFi.SSID());
    Serial.print("PSS:" + WiFi.psk());
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
    loopWebSocket();
  }

 loopResetWifiPin();
}
