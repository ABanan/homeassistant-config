/*
 * MySensors relay control 0.5
 * Dodano:
 * - drugi przekaznik,
 * - wysylanie wartości poczatkowej dla obu przekaznikow (wymagane przez HA), 
 * - zabezpieczenie czasowe przed wlaczeniem przekaznika na stale, 
 */

// Enable debug prints to serial monitor
#define MY_DEBUG
#define MY_GATEWAY_SERIAL

#include <MySensors.h>
#include <MyConfig.h>

#define SN "Relay"
#define SV "0.5"
#define MY_NODE_ID 100

#define RELAY_1_PIN 6  // Arduino Digital I/O pin number for first relay
#define RELAY_2_PIN 7 
#define RELAY_1_CHILD_ID 10
#define RELAY_2_CHILD_ID 11

#define RELAY_ON 0  // GPIO value to write to turn on attached relay
#define RELAY_OFF 1 // GPIO value to write to turn off attached relay

bool initialValue1Sent = false;
bool initialValue2Sent = false;
bool state = false;
unsigned long time = 0;

MyMessage relay_1_msg(RELAY_1_CHILD_ID, V_STATUS);
MyMessage relay_2_msg(RELAY_2_CHILD_ID, V_STATUS);

void setup()
{
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  digitalWrite(RELAY_1_PIN, RELAY_OFF);
  digitalWrite(RELAY_2_PIN, RELAY_OFF);
}

void presentation()
{
  sendSketchInfo(SN, SV);
  present(RELAY_1_CHILD_ID, S_BINARY);
  present(RELAY_2_CHILD_ID, S_BINARY);
}

void loop()
{
  if (!initialValue1Sent) {
    Serial.println("Sending initial value for RELAY_1");
    send(relay_1_msg.set(state?RELAY_ON:RELAY_OFF));
    Serial.println("Requesting initial value from controller");
    request(RELAY_1_CHILD_ID, V_STATUS);
    wait(20000, C_SET, V_STATUS);
  }
  if (!initialValue2Sent) {
    Serial.println("Sending initial value for RELAY_2");
    send(relay_2_msg.set(state?RELAY_ON:RELAY_OFF));
    Serial.println("Requesting initial value from controller");
    request(RELAY_2_CHILD_ID, V_STATUS);
    wait(20000, C_SET, V_STATUS);
  }
  if (time + 3*60*1000 < millis()) {
    digitalWrite(RELAY_1_PIN, RELAY_OFF);
    digitalWrite(RELAY_2_PIN, RELAY_OFF);
    send(relay_1_msg.set(!RELAY_OFF));
    send(relay_2_msg.set(!RELAY_OFF));
  }
  wait(20*000);
}

void receive(const MyMessage &message)
{
  // We only expect one type of message from controller. But we better check anyway.
  if (message.getType() == V_STATUS) {
    time = millis();
    
    if (message.getSensor() == RELAY_1_CHILD_ID) {
      if (!initialValue1Sent) {
        Serial.println("Receiving initial value from controller for RELAY_1");
        initialValue1Sent = true;
      }
      
      digitalWrite(RELAY_1_PIN, message.getBool()?RELAY_ON:RELAY_OFF);
      send(relay_1_msg.set(message.getBool()?!RELAY_ON:!RELAY_OFF));
      
      Serial.print("Incoming change for sensor:");
      Serial.print(message.getSensor());
      Serial.print(", New status: ");
      Serial.println(message.getBool());
    }
    
    if (message.getSensor() == RELAY_2_CHILD_ID) {
      if (!initialValue2Sent) {
        Serial.println("Receiving initial value from controller for RELAY_2");
        initialValue2Sent = true;
      }
      
      digitalWrite(RELAY_2_PIN, message.getBool()?RELAY_ON:RELAY_OFF);
      send(relay_2_msg.set(message.getBool()?!RELAY_ON:!RELAY_OFF));
      
      Serial.print("Incoming change for sensor:");
      Serial.print(message.getSensor());
      Serial.print(", New status: ");
      Serial.println(message.getBool());
    }
  }
  if (message.isAck()) {
    Serial.println("This is an ack from gateway");
  }
}
