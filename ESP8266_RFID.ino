#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>

#define RST_PIN D1 // (D1) RST in module
#define SS_PIN D2 // (D0) SDA in module
#define LED D8 // LED Indikasi apakah akses diberikan atau tidak

const char* ssid     = "71160036";
const char* password = "vanguard999";
String uid = "";
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

WiFiServer server(80); // Set web server port number to 80


void granted(){
    Serial.println("");
    Serial.println("UID Recognized");
    Serial.println("Access Granted!");
    digitalWrite(LED, HIGH);
    delay(3000);
    digitalWrite(LED,LOW);
  }

void denied(){
   Serial.println("");
   Serial.println("Unknown UID");
   Serial.println("Access Denied!");
   digitalWrite(LED,HIGH);
   delay(500);
   digitalWrite(LED,LOW);
   delay(500);
   digitalWrite(LED,HIGH);
   delay(500);
   digitalWrite(LED,LOW);
   delay(500);
   digitalWrite(LED,HIGH);
   delay(500);
   digitalWrite(LED,LOW);
  }

void setup() {
  delay(4000);
  pinMode (LED,OUTPUT);
  Serial.begin(9600);   // Initiate a serial communication
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Use this IP to access the RFID scan page");
  server.begin();
  
  SPI.begin();      // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println("------------------------------------\n");
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients
  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<meta http-equiv='refresh' content='1'>");
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}</style></head>");

  // Web Page Heading
  client.println("<body><h1>ESP8266 Web Server</h1>");
  client.println("<h2>Put your UID card to scan</h2>");
  client.println("<p>Card UID : </p>");
  
  if (mfrc522.PICC_IsNewCardPresent()) { // Look for new cards
    if (mfrc522.PICC_ReadCardSerial()) { // Select one of the cards
      Serial.println("UID Scanned");
      Serial.print("Card UID : ");
      String content = "";

      for (byte i = 0; i < mfrc522.uid.size; i++) {
        // Dump a byte array as hex values to Serial
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);

        // Dump a byte array as hex values to String
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX)); //
      }

      content.toUpperCase(); // Make string become UPPERCASE
      mfrc522.PICC_HaltA();
      if (content.substring(1) == "74 42 07 89") { // Change UID of the card that you want to give access, for example A0 B1 C2 D3
        // Do something with access granted
        granted();
      }

      else {
        // Do something with access denied
        denied();
      }
      uid = content.substring(1);
      Serial.println("");
    }
  }
  client.println("<p>"+ uid + "</p>");
}
