#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Servo.h>

const char* ssid = "MayTheForceBeWithWiFi";
const char* password = "6xY6NeERnCxJKnyk";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200);  // 7200 seconds offset for Vilnius, Lithuania

ESP8266WebServer server(80);

Servo myservo;
int servoPin = 2;
int buttonPin = 4;
boolean servoState = false;
int lastButtonState = LOW;  // To store the last state of the button

int setHour1 = -1;
int setMinute1 = -1;
int setHour2 = -1;
int setMinute2 = -1;

boolean toggledTime1 = false;
boolean toggledTime2 = false;

// HTML content that will be sent to the client
const char* html = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>ESP8266 Servo Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; background-color: #e3e1e1; margin: 0; padding: 20px; color: #333; }
        .container { display: flex; flex-direction: column; gap: 20px; }
        .section { background-color: #fff; padding: 15px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        h1, h2 { color: #333; }
        p, a, input[type="time"], input[type="submit"] { font-size: 18px; }
        a { display: block; width: calc(100% - 20px); padding: 10px; margin: 10px 0; background-color: #0074d9; color: #fff; border: none; border-radius: 5px; text-align: center; cursor: pointer; text-decoration: none; }
        a:active { background-color: #0056b3; }
        input[type="submit"] { display: block; width: calc(100% - 20px); padding: 15px; margin: 10px 0; background-color: #0074d9; color: #fff; border: none; border-radius: 5px; cursor: pointer; }
        input[type="submit"]:active { background-color: #0056b3; }
        input[type="time"] { width: calc(100% - 20px); padding: 10px; margin-top: 5px; border: 1px solid #ccc; border-radius: 5px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="section">
            <h2>Switch</h2>
            <a href="/toggle">Open And Feed The Monster</a>
        </div>
        <div class="section">
            <h2>Set Time 1</h2>
            <p>Time 1: %TIME1%</p>
            %DELETE_TIME1%
            <form action="/setTime1" method="post">
                Set Time: <input type="time" name="setTime1">
                <input type="submit" value="Set Time">
            </form>
        </div>
        <div class="section">
            <h2>Set Time 2</h2>
            <p>Time 2: %TIME2%</p>
            %DELETE_TIME2%
            <form action="/setTime2" method="post">
                Set Time: <input type="time" name="setTime2">
                <input type="submit" value="Set Time">
            </form>
        </div>
        <div class="section">
            <p>Developed by: Paulius Jakonis</p>
        </div>
    </div>
</body>
</html>
)=====";

void handleRoot() {
  String time1 = (setHour1 != -1 && setMinute1 != -1) ? String(setHour1) + ":" + String(setMinute1) : "Not set";
  String time2 = (setHour2 != -1 && setMinute2 != -1) ? String(setHour2) + ":" + String(setMinute2) : "Not set";

  String deleteButton1 = (setHour1 != -1 && setMinute1 != -1) ? "<form action=\"/deleteTime1\" method=\"post\"><input type=\"submit\" value=\"Remove Time\"></form>" : "";
  String deleteButton2 = (setHour2 != -1 && setMinute2 != -1) ? "<form action=\"/deleteTime2\" method=\"post\"><input type=\"submit\" value=\"Remove Time\"></form>" : "";

  String page = String(html);
  page.replace("%TIME1%", time1);
  page.replace("%DELETE_TIME1%", deleteButton1);
  page.replace("%TIME2%", time2);
  page.replace("%DELETE_TIME2%", deleteButton2);
  server.send(200, "text/html", page);
}

void handleToggle() {
  if (servoState) {
    myservo.detach();  // Detach the servo
    servoState = false;
  } else {
    myservo.attach(servoPin);  // Attach the servo
    myservo.write(180);        // Move the servo to 180 degrees
    delay(3000);               // Wait for 1 second
    myservo.write(0);          // Move the servo back to 0 degrees
    delay(1000);               // Wait for 1 second
    myservo.detach();          // Detach the servo again
    servoState = false;
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSetTime1() {
  String time = server.arg("setTime1");
  int colonIndex = time.indexOf(':');
  if (colonIndex != -1 && colonIndex < time.length() - 1) {
    setHour1 = time.substring(0, colonIndex).toInt();
    setMinute1 = time.substring(colonIndex + 1).toInt();
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSetTime2() {
  String time = server.arg("setTime2");
  int colonIndex = time.indexOf(':');
  if (colonIndex != -1 && colonIndex < time.length() - 1) {
    setHour2 = time.substring(0, colonIndex).toInt();
    setMinute2 = time.substring(colonIndex + 1).toInt();
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Initialize the button pin as input with internal pull-up
  myservo.attach(servoPin);
  myservo.write(0);
  myservo.detach();

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start NTP client
  timeClient.begin();

  // Define routes for the web server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/toggle", HTTP_GET, handleToggle);
  server.on("/setTime1", HTTP_POST, handleSetTime1);
  server.on("/setTime2", HTTP_POST, handleSetTime2);
  server.on("/deleteTime1", HTTP_POST, []() {
    setHour1 = -1;
    setMinute1 = -1;
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/deleteTime2", HTTP_POST, []() {
    setHour2 = -1;
    setMinute2 = -1;
    server.sendHeader("Location", "/");
    server.send(303);
  });

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();  // Handle client requests
  timeClient.update();    // Update the time client

  // Read the button state
  int buttonState = digitalRead(buttonPin);
  // If the button is pressed, toggle the servo
  if (buttonState == LOW && lastButtonState == HIGH) {
    handleToggle();
  }
  lastButtonState = buttonState;  // Update the last button state

  // Check and act on the first set time
  if (!toggledTime1 && setHour1 != -1 && setMinute1 != -1) {
    if (timeClient.getHours() == setHour1 && timeClient.getMinutes() == setMinute1) {
      handleToggle();
      toggledTime1 = true;
    }
  }
  if (toggledTime1 && (timeClient.getHours() != setHour1 || timeClient.getMinutes() != setMinute1)) {
    toggledTime1 = false;
  }

  // Check and act on the second set time
  if (!toggledTime2 && setHour2 != -1 && setMinute2 != -1) {
    if (timeClient.getHours() == setHour2 && timeClient.getMinutes() == setMinute2) {
      handleToggle();
      toggledTime2 = true;
    }
  }
  if (toggledTime2 && (timeClient.getHours() != setHour2 || timeClient.getMinutes() != setMinute2)) {
    toggledTime2 = false;
  }
}
