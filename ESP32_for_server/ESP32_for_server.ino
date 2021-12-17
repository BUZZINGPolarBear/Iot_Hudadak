#define SWAP 0 // sw access point

#include <WiFi.h>
#include <AWS_IOT.h>
#include <Arduino_JSON.h>
AWS_IOT testButton;

// Replace with your network credentials
#if SWAP
const char* ssid = "ESP32-AP";
const char* password = "123456789"; // password should be long!!
#else
const char *ssid = "Juni WIFI";
const char *password = "wnsgnlRj";
#endif

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
// Auxiliar variables to store the current output state
String warmLightState = "off";
String humidPumpState = "off";
String feedState = "off";
String feedwaterState="off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

char HOST_ADDRESS[] = "a3llcbaumch20d-ats.iot.ap-northeast-2.amazonaws.com";
char CLIENT_ID[]= "ESP32ForTemperature";
char sTOPIC_NAME[]= "$aws/things/ESP32_BME280/shadow/update/delta"; // subscribe topic name
char pTOPIC_NAME[]= "$aws/things/ESP32_BME280/shadow/update"; // publish topic name
int temp=0;
int humid=0;
int sound=0;
int touchPin=0;
int depth = 0;

int status = WL_IDLE_STATUS;
int msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
  strncpy(rcvdPayload,payLoad,payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void publishTopics(String warmLightState, String humidPumpState, String waterPumpState, String feedState)
{
  String temp = "{\"state\":{\"userSelected\":{\"temp\":\"" + warmLightState + "\", \"humid\":\"" + humidPumpState + "\", \"water\":\"" + waterPumpState + "\", \"feed\":\"" + feedState +"\"}}}";
  Serial.println(temp);
  char toChar[1000];
  strcpy(toChar, temp.c_str());
  sprintf(payload,toChar);
  for(int i=0; i<3; i++)
  {//publish 실패를 대비한 3회 전송
    if(testButton.publish(pTOPIC_NAME,payload) == 0) {
      Serial.print("Publish Message:");
      Serial.println(payload);
    }
    else
      Serial.println("Publish failed");
  }
}
void setup()
{
  Serial.begin(115200);
#if SWAP
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();

#else
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.getMode());
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode()); //++choi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to wifi");
 if(testButton.connect(HOST_ADDRESS,CLIENT_ID)== 0) {
  Serial.println("Connected to AWS");
  delay(1000);
  if(0==testButton.subscribe(sTOPIC_NAME,mySubCallBackHandler)) {
    Serial.println("Subscribe Successfull");
  }
  else {
    Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
    while(1);
    }
  }
  else {
    Serial.println("AWS connection failed, Check the HOST Address");
    while(1);
  }
#endif
  server.begin();
}

void loop()
{
  WiFiClient client = server.available(); // Listen for incoming clients
  if (client)
  { // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    { // loop while the client's connected
      currentTime = millis();
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        header += c;
        if (c == '\n')
        
        { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response: 
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            //turns the GPIOs on and off
    
             if (header.indexOf("GET /warmLight/on") >= 0)
            {
              Serial.println("warm Light on");
              warmLightState = "on";
            }
            else if (header.indexOf("GET /warmLight/off") >= 0)
            {
              Serial.println("warm Light off");
              warmLightState = "off";
              }
            if (header.indexOf("GET /humidPump/on") >= 0)
            {
              Serial.println("humid Pump on");
              humidPumpState = "on";
              }
            else if (header.indexOf("GET /humidPump/off") >= 0)
            {
              Serial.println("humid Pump off");
              humidPumpState = "off";
              }
            if (header.indexOf("GET /feed") >= 0)
            {
              Serial.println("feed Chicks");
              feedState = "on";
              }
            if (header.indexOf("GET /feedwater") >= 0)
            {
              Serial.println("feed water for Chicks");
              feedwaterState = "on";
            }
              publishTopics(warmLightState, humidPumpState, feedState, feedwaterState);
            
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html lang=\"ko\">");
            client.println("<!DOCTYPE html>");
            client.println("<html lang=\"ko\"><head>");
            
            client.println("<meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
            client.println("<link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bulma@0.9.3/css/bulma.min.css\"><link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">");
            client.println("<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin><link href=\"https://fonts.googleapis.com/css2?family=Dongle&display=swap\" rel=\"stylesheet\">");
            
            client.println("<title>후다닭</title><style>");
            
            client.println("body{font-family: 'Dongle', sans-serif;}");


            client.println("#mainStatusBox{background-color: white;width: 500px; margin: 5px auto;text-align: center;}");
            client.println("#chick {width : 200px;height : 150px;margin-left : 30px;}");
            client.println("#mainStatusText{width : 500px;align-items: center;justify-content: center;}");
            client.println(".chickBoxStatus{font-family: 'Dongle', sans-serif;border: none;width: 150px;font-size: 35px;}");
            client.println(".text_size {width: 450px;height : 50px;font-size: 35px;margin-left : 100px;} ");
            client.println(".buttonTable{width: 100%;} ");
            client.println("#BTN {font-family: 'Dongle', sans-serif;font-size: 30px;width: 200px;height: 100px;margin : 20px;}</style></head> ");

            client.println("<body><div id=\"mainStatusBox\">");
            client.println("<img src = 'https://s3.ap-northeast-2.amazonaws.com/daara2021.03.15test/chickImage.png' id = \"chick\">");
            client.println("<div id = \"mainStatusText\">");
            if(msgReceived == 1)
            {
              msgReceived = 0;
              Serial.print("Received Message:");
              Serial.println(rcvdPayload);
              // Parse JSON
              JSONVar myObj = JSON.parse(rcvdPayload);
              JSONVar state = myObj["state"];
              temp = state ["temp"];
              humid = state ["humid"];
              sound = state ["sound"];
              depth = state ["depth"];
              touchPin = state ["touchPin"];
            }
              client.println("<p class = \"text_size\">육추실 온도 : <input type=\"text\" class=\"chickBoxStatus\" id=\"tempStatus\" value=\"" + String(temp) + "도\" readonly></p>");
              client.println("<p class = \"text_size\">육추실 습도 : <input type=\"text\" class=\"chickBoxStatus\" id=\"humidStatus\" value=\"" + String(humid) +"%\" readonly></p>");
              if(10<depth) client.println("<p class = \"text_size\">먹이 양 : <input type=\"text\" class=\"chickBoxStatus\" id=\"foodStatus\" value=\"적음\" style=\"color: red;\" readonly></p> <div id=\"mainStatusBtn\">");
              else if(depth<=12) client.println("<p class = \"text_size\">먹이 양 : <input type=\"text\" class=\"chickBoxStatus\" id=\"foodStatus\" value=\"보통\" style=\"color: green;\" readonly></p> <div id=\"mainStatusBtn\">");
              else client.println("<p class = \"text_size\">먹이 양 : <input type=\"text\" class=\"chickBoxStatus\" id=\"foodStatus\" value=\"많음\" readonly></p> <div id=\"mainStatusBtn\">");
              if(sound<20) client.println("<p class = \"text_size\">활동성 : <input type=\"text\" class=\"chickBoxStatus\" id=\"chickStatus\" value=\"적정\" style=\"color: green; readonly></p> <div id=\"mainStatusBtn\">");
              else if(sound<25) client.println("<p class = \"text_size\">활동성 : <input type=\"text\" class=\"chickBoxStatus\" id=\"chickStatus\" value=\"중간\" style=\" readonly></p> <div id=\"mainStatusBtn\">");
              else client.println("<p class = \"text_size\">활동성 : <input type=\"text\" class=\"chickBoxStatus\" id=\"chickStatus\" value=\"많음\" style=\"color: red; readonly></p> <div id=\"mainStatusBtn\">");
           
              client.println("<table class=\"buttonTable\"> <thead>");


            if (warmLightState == "off") {
              client.println("<tr> <td> <a href = \"/warmLight/on\"/><button id = \"BTN\" class = \"button is-warning\">온열등 ON</button></a>");
            }
            else {
              client.println("<tr> <td> <a href = \"/warmLight/off\"/><button id = \"BTN\" class = \"button is-warning\">온열등 OFF</button></a>");
            }
            if (humidPumpState=="off")
            {
              client.println("<a href = \"/humidPump/on\"/><button id = \"BTN\"class=\"button is-warning\" >가습 하기</button></a> </td></tr>");
            }
            else
            {
              client.println("<a href = \"/humidPump/off\"/><button id = \"BTN\"class=\"button is-warning\" >가습 중단하기</button></a> </td></tr>");
            }
            
            client.println("<tr><td><a href = \"/feed\"/><button id = \"BTN\"class=\"button is-warning\" >먹이 급여</button></a>");
            client.println("<a href=\"https://s3.ap-northeast-2.amazonaws.com/daara2021.03.15test/Inner_HudadakCase_status.jpg\" target=\"_blank\"><button id = \"BTN\"class=\"button is-warning\" onclick=\"\" >사육장 내부 확인</button></a></td> </tr> </thead>");  
            client.println("</table> </div>  </div>  </body> </html>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break; //** if (currentLine.length() == 0) {
           }
          else
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } //** if (c == '\n') {
        else if (c != '\r'){                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      } //* if (client.available()){
    }   //** while

    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  } //** if (client) {
} //** loop() 
