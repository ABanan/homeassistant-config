/*
 * MySensors relay control
 */

// Enable debug prints to serial monitor
#define MY_DEBUG
#define MY_GATEWAY_SERIAL

#include <MySensors.h>
#include <MyConfig.h>

#define SN "Relay"
#define SV "0.2"
#define MY_NODE_ID 100

#define RELAY_1_PIN 6  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define RELAY_1_CHILD_ID 10

#define RELAY_ON 0  // GPIO value to write to turn on attached relay
#define RELAY_OFF 1 // GPIO value to write to turn off attached relay

bool initialValueSent = false;
bool state = false;

MyMessage msg(RELAY_1_CHILD_ID, V_STATUS);

void setup()
{
    pinMode(RELAY_1_PIN, OUTPUT);
    digitalWrite(RELAY_1_PIN, RELAY_OFF);
}

void presentation()
{
    sendSketchInfo(SN, SV);
    present(RELAY_1_CHILD_ID, S_BINARY);
}

void loop()
{
  if (!initialValueSent) {
    Serial.println("Sending initial value");
    send(msg.set(state?RELAY_ON:RELAY_OFF));
    Serial.println("Requesting initial value from controller");
    request(RELAY_1_CHILD_ID, V_STATUS);
    wait(2000, C_SET, V_STATUS);
  }
}

void receive(const MyMessage &message)
{
  // We only expect one type of message from controller. But we better check anyway.
  if (message.getType()==V_STATUS) {
    if (!initialValueSent) {
      Serial.println("Receiving initial value from controller");
      initialValueSent = true;
    }
    // Change relay state
    digitalWrite(message.getSensor()-RELAY_1_CHILD_ID+RELAY_1_PIN, message.getBool()?RELAY_ON:RELAY_OFF);

    // Write some debug info
    Serial.print("Incoming change for sensor:");
    Serial.print(message.getSensor());
    Serial.print(", New status: ");
    Serial.println(message.getBool());
  }
  if (message.isAck()) {
    Serial.println("This is an ack from gateway");
  }
}
