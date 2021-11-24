#define SWAP 0 // sw access point

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "Juni Wifi";
const char* password = "wnsgnlRj";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;


// Auxiliar variables to store the current output state
String output16State = "off";
String output17State = "off";

// Assign output variables to GPIO pins
const int output16 = 16;
const int output17 = 17;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup()
{
  Serial.begin(115200);

  // Initialize the output variables as outputs
  pinMode(output16, OUTPUT);
  pinMode(output17, OUTPUT);

  // Set outputs to LOW
  digitalWrite(output16, LOW);
  digitalWrite(output17, LOW);
#if SWAP
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
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
            // turns the GPIOs on and off
            if (header.indexOf("GET /16/on") >= 0)
            {
              Serial.println("GPIO 16 on");
              output16State = "on";
              digitalWrite(output16, HIGH);
            }
            else if (header.indexOf("GET /16/off") >= 0)
            {
              Serial.println("GPIO 16 off");
              output16State = "off";
              digitalWrite(output16, LOW);
            }
            else if (header.indexOf("GET /17/on") >= 0)
            {
              Serial.println("GPIO 17 on");
              output17State = "on";
              digitalWrite(output17, HIGH);
            }
            else if (header.indexOf("GET /17/off") >= 0)
            {
              Serial.println("GPIO 17 off");
              output17State = "off";
              digitalWrite(output17, LOW);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html lang=\"ko\">");
            client.println("<!DOCTYPE html>");
            client.println("<html lang=\"ko\"><head>");
            client.println("<meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
            client.println("<link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bulma@0.9.3/css/bulma.min.css\"><link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">");
            client.println("<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin><link href=\"https://fonts.googleapis.com/css2?family=Cute+Font&display=swap\" rel=\"stylesheet\">");
            client.println("<title>후다닭</title><style>");
            client.println("body{font-family: 'Cute Font', cursive;} ");
            client.println("#mainStatusBox{background-color: white; width: 400px; height: 400px; margin: 50px auto; padding: 30px; text-align: center; }");
            client.println(".buttonTable{margin: auto;}");
            client.println(".chickBoxStatus{  font-family: 'Cute Font', cursive;font-size: 20px;border: none;width: 50px; font-size: 14px; }");
            client.println(" #BTN {font-family: 'Cute Font', cursive; font-size: 17px; width: 100px; height: 50px; margin : 20px 25px 10px 25px; } </style>");
                            
            client.println("</head><body style=""><div id=\"mainStatusBox\">");
            client.println("<img src = 'https://s3.ap-northeast-2.amazonaws.com/daara2021.03.15test/chickImage.png' style=\"width: 70px; height: 50px;margin: 15px 0px;\">");
            client.println("<div id = \"mainStatusText\">");
            client.println("<p>육추실 온도 : <input type=\"text\" class=\"chickBoxStatus\" id=\"tempStatus\" value=\"36.5도\" readonly></p>");
            client.println("<p>육추실 습도 : <input type=\"text\" class=\"chickBoxStatus\" id=\"humidStatus\" value=\"36.5%\" readonly></p>");
            client.println("<p>먹이 양 : <input type=\"text\" class=\"chickBoxStatus\" id=\"foodStatus\" value=\"36.5℃\" readonly></p>");
            client.println("<p>병아리 활동성 : <input type=\"text\" class=\"chickBoxStatus\" id=\"chickStatus\" value=\"36.5℃\" readonly></p> <div id=\"mainStatusBtn\">");
            client.println("<table class=\"buttonTable\"> <thead>");
            client.println("<tr> <td> <button id = \"BTN\" class=\"button is-warning\" >온열등 ON</button><button id = \"BTN\"class=\"button is-warning\" >가습 하기</button> </td></tr>");
            client.println("<tr><td><button id = \"BTN\"class=\"button is-warning\" >먹이 급여</button><button id = \"BTN\"class=\"button is-warning\" >물 주기</button> </td> </tr> </thead>");                
            client.println("</table> </div>  </div>  </body> </html>");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
          
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  } //** if (client) {
} //** loop() {