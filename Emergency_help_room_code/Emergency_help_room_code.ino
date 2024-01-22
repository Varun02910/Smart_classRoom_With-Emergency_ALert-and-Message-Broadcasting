//Medical Room...
#include "painlessMesh.h"
#include <Arduino_JSON.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial BLE(13, 15); //D7,D8

LiquidCrystal_I2C lcd(0x27, 16, 4);

const int sos_button = 14;//D5
const int buzzer = 12;//D6

//MESH Details
#define MESH_PREFIX "COMMESH" //name for yor Mesh
#define MESH_PASSWORD "12345678" //password for your mesh
#define MESH_PORT 5555 //Define Port

//Room number for this node
String roomNumber = "M1";

//String to send other nodes with sensor value:
String room = "";
String message = "";
String emergency = "0";

Scheduler userScheduler;//to control your personal task
painlessMesh mesh;

//user stub
//void sendMessage();//prototype
void getReadings();//prototype for broadcasting message

//create tasks: to send _message and get reading
Task taskSendMessage(TASK_MILLISECOND * 100, TASK_FOREVER, &getReadings);

void getReadings() {
  JSONVar jsonReading;
//  lcd.setCursor(2, 1);
//  lcd.print("Good Morning!!!");
//  lcd.setCursor(0, 2);
//  lcd.print("Have a good Day..");
  room = "";
  message = "";


  while (BLE.available()) {
    char a = BLE.read();
    room = room + a;

    if (a == ' ') {
      while (BLE.available()) {
        char a = BLE.read();
        message = message + a;
      }
    }
  }
  room.trim();

  if (digitalRead(sos_button)) {
    room = roomNumber;
    message = "There is an Emergency in Room Number " + room + ".";
    emergency = "1";
    jsonReading["node"] = roomNumber;
    jsonReading["room"] = room;
    jsonReading["message"] = message;
    jsonReading["emergency"] = emergency;
    String broadcast_message = JSON.stringify(jsonReading);
//    mesh.sendBroadcast(broadcast_message);
//    for (int i = 0; i < 10; i++) {
//      digitalWrite(buzzer, HIGH);
//      delay(100);
//      digitalWrite(buzzer, LOW);
//      delay(100);
//    }
    
  }

  

  if (room == roomNumber or room == "All") {
    
    jsonReading["node"] = roomNumber;
  jsonReading["room"] = room;
  jsonReading["message"] = message;
  jsonReading["emergency"] = emergency;

  String broadcast_message = JSON.stringify(jsonReading);
    mesh.sendBroadcast(broadcast_message);
    Serial.print("Room Number: ");
    Serial.println(room);
    Serial.print("Message: ");
    Serial.println(message);

//    // Split the string into substrings of 20 characters
//    int substringLength = 20;
//    int numberOfLines = ceil(message.length() / (float)substringLength);
//
//    for (int i = 0; i < numberOfLines; i++) {
//      // Calculate the starting index for each substring
//      int startIndex = i * substringLength;
//
//      // Extract the substring of 20 characters
//      String currentLine = message.substring(startIndex, min(startIndex + substringLength, int(message.length())));
//
//      // Print or store the substring as needed
//      Serial.print("Line ");
//      Serial.print(i + 1);
//      Serial.print(": ");
//      Serial.println(currentLine);
//      lcd.setCursor(1, 0);
//      lcd.print("!!Message Alert!!");
//      lcd.setCursor(0, i + 1);
//      lcd.print(currentLine);
//
//    }
//    for (int i = 0; i < 10; i++) {
//      digitalWrite(buzzer, HIGH);
//      delay(100);
//      digitalWrite(buzzer, LOW);
//      delay(100);
//    }
//    delay(5000);
//    lcd.clear();

  }
  else if (room != "") {
    jsonReading["node"] = roomNumber;
  jsonReading["room"] = room;
  jsonReading["message"] = message;
  jsonReading["emergency"] = emergency;

  String broadcast_message = JSON.stringify(jsonReading);
    mesh.sendBroadcast(broadcast_message);
  }
}



void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("recieved from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  String node = myObject["node"];
  String  Room = myObject["room"];
  String mesg = myObject["message"];
  String emerg = myObject["emergency"];

  Serial.print("Node from : ");
  Serial.println(node);
  Serial.print("Room to: ");
  Serial.println(Room);

  if ( emerg == "1") {//this part is only for acadmic room and medical room.
    lcd.clear();
    // Split the string into substrings of 20 characters
    int substringLength = 20;
    int numberOfLines = ceil(mesg.length() / (float)substringLength);

    for (int i = 0; i < numberOfLines; i++) {
      // Calculate the starting index for each substring
      int startIndex = i * substringLength;

      // Extract the substring of 20 characters
      String currentLine = mesg.substring(startIndex, min(startIndex + substringLength, int(mesg.length())));

      // Print or store the substring as needed
      Serial.print("Line ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(currentLine);
      lcd.setCursor(1, 0);
      lcd.print("!!Message Alert!!");
      lcd.setCursor(0, i + 1);
      lcd.print(currentLine);
    }
    for (int i = 0; i < 10; i++) {
      digitalWrite(buzzer, HIGH);
      delay(100);
      digitalWrite(buzzer, LOW);
      delay(100);
    }
    delay(5000);
    lcd.clear();
  }
  else if (Room == roomNumber or Room == "All") {
    lcd.clear();
    Serial.print("Message: ");
    Serial.println(mesg);
    // Split the string into substrings of 20 characters
    int substringLength = 20;
    int numberOfLines = ceil(mesg.length() / (float)substringLength);

    for (int i = 0; i < numberOfLines; i++) {
      // Calculate the starting index for each substring
      int startIndex = i * substringLength;

      // Extract the substring of 20 characters
      String currentLine = mesg.substring(startIndex, min(startIndex + substringLength, int(mesg.length())));

      // Print or store the substring as needed
      Serial.print("Line ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(currentLine);
      lcd.setCursor(1, 0);
      lcd.print("!!Message Alert!!");
      lcd.setCursor(0, i + 1);
      lcd.print(currentLine);
    }
    for (int i = 0; i < 10; i++) {
      digitalWrite(buzzer, HIGH);
      delay(100);
      digitalWrite(buzzer, LOW);
      delay(100);
    }
    delay(5000);
    lcd.clear();
  }
}

void newConnectionCallback(uint32_t nodeID) {
  Serial.printf("New Connection, Node ID = %u\n", nodeID);
}

void changedConnectionCallback() {
  Serial.printf("Changed Connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. offset = %d\n", mesh.getNodeTime(), offset);
}



void setup() {
  Serial.begin(115200);
  BLE.begin(9600);

   pinMode(sos_button, INPUT);
   pinMode(buzzer, OUTPUT);

  lcd.init();
  lcd.clear();
  lcd.backlight();  //Make sure backlight is ON


  mesh.setDebugMsgTypes(ERROR | STARTUP); //set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();

}

void loop() {

  mesh.update();

  Serial.print("Room Number: ");
  Serial.println(room);
  Serial.print("Message: ");
  Serial.println(message);
  if (message == "") {
    lcd.setCursor(2, 1);
    lcd.print("Good Morning!!!");
    lcd.setCursor(0, 2);
    lcd.print("Have a good Day..");
  }
  else {

    lcd.clear();
    // Split the string into substrings of 20 characters
    int substringLength = 20;
    int numberOfLines = ceil(message.length() / (float)substringLength);

    for (int i = 0; i < numberOfLines; i++) {
      // Calculate the starting index for each substring
      int startIndex = i * substringLength;

      // Extract the substring of 20 characters
      String currentLine = message.substring(startIndex, min(startIndex + substringLength, int(message.length())));

      // Print or store the substring as needed
      Serial.print("Line ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(currentLine);
      lcd.setCursor(1, 0);
      lcd.print("!!Message Alert!!");
      lcd.setCursor(0, i + 1);
      lcd.print(currentLine);

    }
    for (int i = 0; i < 10; i++) {
      digitalWrite(buzzer, HIGH);
      delay(100);
      digitalWrite(buzzer, LOW);
      delay(100);
    }
    delay(5000);
    lcd.clear();
  }
}
