#include <EEPROM.h>
// define the number of bytes you want to access
#define EEPROM_SIZE 80

// IoT07-1 ESP32 WebServer
#define SWAP 0 // sw access point
#include <string>

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
#if SWAP
  const char *ssid = "J's WIFI 2.4GHZ";
const char *password = "64196336"; // password should be long!!
#else
const char *ssid = "J's WIFI 2.4GHZ";
const char *password = "64196336";
#endif

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

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600 * 9; // 3600
const int daylightOffset_sec = 0;    // 3600

String hourZeroToTwelve = "<label class=\"label\">Hour</label><div class=\"select\"> <select name=\"hour\"> <option>00</option> <option>02</option> <option>03</option> <option>04</option> <option>05</option> <option>06</option> <option>07</option> <option>08</option> <option>09</option> <option>10</option> <option>11</option> <option>12</option> <option>13</option> <option>14</option> <option>15</option> <option>16</option> <option>17</option> <option>18</option> <option>19</option> <option>20</option> <option>21</option> <option>22</option> <option>23</option> </select> </div>";
String minuteZeroToSixty = "<label class=\"label\">Minute</label><div class=\"select\"> <select name=\"minute\"> <option>00</option> <option>01</option> <option>02</option> <option>03</option> <option>04</option> <option>05</option> <option>06</option> <option>07</option> <option>08</option> <option>09</option> <option>10</option> <option>11</option> <option>12</option> <option>13</option> <option>14</option> <option>15</option> <option>16</option> <option>17</option> <option>18</option> <option>19</option> <option>20</option> <option>21</option> <option>22</option> <option>23</option> <option>24</option> <option>25</option> <option>26</option> <option>27</option> <option>28</option> <option>29</option> <option>30</option> <option>31</option> <option>32</option> <option>33</option> <option>34</option> <option>35</option> <option>36</option> <option>37</option> <option>38</option> <option>39</option> <option>40</option> <option>41</option> <option>42</option> <option>43</option> <option>44</option> <option>45</option> <option>46</option> <option>47</option> <option>48</option> <option>49</option> <option>50</option> <option>51</option> <option>52</option> <option>53</option> <option>54</option> <option>55</option> <option>56</option> <option>57</option> <option>58</option> <option>59</option> </select> </div>";
String getHour = "";
String getMin = "";
String getMusic = "04060604044476764404767644767644040404";

const int ledChannel = 0;
const int resolution = 8;
const int buzPin = 23;
const int buttonPin = 15;
const int duty = 18;

byte sVal, sDel;
const int nFrq[]={262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466,  494, 0, 523};
const int nDelay[]={2000, 1500, 1000, 750, 500, 375, 250};
int str_len;
char toChar[1000];
int music_cnt=0;
int music_pair_index = 3;
int is_music_saved=false;
bool is_music_played=false;

void playNote(char note, int dur)
{
  int temp;
  if(note == 'c')
  {
    
    ledcSetup(ledChannel, 523, resolution);
    ledcWrite(ledChannel, duty);
    delay(dur);
    return;
  }
  else if(note == 'z')
  {
    ledcSetup(ledChannel, 0, resolution);
    delay(dur);
    return;
  }
  else temp = note-0x30;

  Serial.println(String(note) +" "+ String(dur));
  Serial.println("nFrq= " + String(nFrq[temp]) + ",Del= " + String(dur));
  ledcSetup(ledChannel, nFrq[temp], resolution);
  ledcWrite(ledChannel, duty);
  delay(dur);
}

String printLocalTime()
{
  struct tm timeinfo;
  String nowTime = ""; 
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return "시간을 불러올 수 없음.";
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  //Serial.println("Year: " + String(timeinfo.tm_year + 1900) + ", Month: " + String(timeinfo.tm_mon + 1));
  nowTime += ("Year:" + String(timeinfo.tm_year+ 1900) + " Month:" + String(timeinfo.tm_mon + 1));
  nowTime += (" Day: " + String(timeinfo.tm_mday)+ "  " + String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min) + ":" + String(timeinfo.tm_sec) + "\n");
  if(String(timeinfo.tm_hour) == getHour && String(timeinfo.tm_min) == getMin)
  {
    Serial.println("#######Saved Note is Playing.....#######");
    
    for(int i=3; i<EEPROM.read(2); i += 2)
    {
      sVal = EEPROM.read(i);
      sDel = EEPROM.read(i+1);
      Serial.println("-----------");
      Serial.println("Got " + String(sVal+'0'-0x30) + " AND " + String(sDel+'0'-0x30));
      playNote(sVal+'0', nDelay[sDel+'0'-0x30]);
    }
    playNote('z', 20000);
    EEPROM.write(0,0xAB);
    EEPROM.commit();
    is_music_played=true;
  }
  //Serial.println(nowTime);
  //client.println("<textarea class=\"textarea has-fixed-size\" rows=\"1\" style=\"text-align: center;\">");
  //client.println(String(timeinfo.tm_year + 1900) + "년 " + String(timeinfo.tm_mon + 1) + "월" + %A + " " + %H + "시 " + %M + "분 " + %S +"초"); 
  //client.println("</textarea>");
  
  
  //client.println("</textarea>");
  return nowTime;
}


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
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
#endif
  server.begin();

  ledcAttachPin(buzPin, ledChannel);
  pinMode(buttonPin, INPUT);  
  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
  pinMode(buttonPin, INPUT);
  if(EEPROM.read(0) == 0xAC) is_music_saved = true;
  EEPROM.commit();
}

void loop()
{
  WiFiClient client = server.available(); // Listen for incoming clients
  
  String old_Time = "";
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
            client.println("Refresh: 30\r");
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
            
            if (header.indexOf("GET /get?hour=") >= 0)
            {
              music_pair_index = 3;
              getHour = header.substring(header.indexOf("/get?hour=")+10, header.indexOf("/get?hour=")+12);
              Serial.println("Got Hour: " + getHour);
              getMin = header.substring(header.indexOf("&minute=")+8, header.indexOf("&minute=")+10);
              Serial.println("Got Min: " + getMin);

              str_len = getMusic.length();
              getMusic.toCharArray(toChar, str_len);
              
              Serial.println("Saving Notes.....");
              for(int i=0; i<getMusic.length()-2; i+=2)
              {
                EEPROM.write(music_pair_index++, byte(toChar[i]-'0'));
                Serial.println("Saved At: " + String(music_pair_index) + " Which is: " + EEPROM.read(music_pair_index-1));
                EEPROM.write(music_pair_index++, byte(toChar[i+1]-'0'));
                Serial.println("Saved At: " + String(music_pair_index) + " Which is: " + EEPROM.read(music_pair_index-1));
                EEPROM.commit();
              }
              EEPROM.write(2, str_len-2);
              Serial.println("문자열 길이: " + str_len-2); 
              EEPROM.write(0, 0xAC);
              EEPROM.commit();
              Serial.println("Music Saved.");
            }
           

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bulma@0.9.3/css/bulma.min.css\">");

            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50;border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            //display Clock interface
            client.println("<textarea class=\"textarea has-fixed-size\" rows=\"1\" style=\"text-align: center;\">");
            String now_Time = printLocalTime();
            if(now_Time != old_Time)
            {
              client.println(now_Time);
              old_Time = now_Time;
            }
            client.println("</textarea>");

            client.println("</br><h2>Set Alarm</h2>");
            client.println("<p>");
            client.println("<form action=\"/get\">");
        
            client.println(hourZeroToTwelve);
            client.println(minuteZeroToSixty);
            client.println("<br><input type=\"submit\" value=\"Submit\">");
            client.println("</form><div class=\"control\"></div></p>");

            // Display current state, and ON/OFF buttons for GPIO 16
            client.println("<p>GPIO 16 - State " + output16State + "</p>");

            // If the output16State is off, it displays the ON button
            if (output16State == "off")
            {
              client.println("<p><a href=\"/16/on\"><button class=\"button\">ON</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/16/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            // Display current state, and ON/OFF buttons for GPIO 17
            client.println("<p>GPIO 17 - State " + output17State + "</p>");
            // If the output17State is off, it displays the ON button
            if (output17State == "off")
            {
              client.println("<p><a href=\"/17/on\"><button class=\"button\">ON</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/17/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } //** if (currentLine.length() == 0) {
          else
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } //** if (c == '\n') {
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
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
} //** loop() {
