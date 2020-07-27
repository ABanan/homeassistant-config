/*
 * MySensors relay control
 */

// Enable debug prints to serial monitor
#define MY_DEBUG
#define MY_GATEWAY_SERIAL

#include <MySensors.h>

#define SN "Relay"
#define SV "0.1"

#define RELAY_1_PIN 6  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define RELAY_1_CHILD_ID 10

#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay


void setup()
{
    pinMode(RELAY_1_PIN, OUTPUT);
    digitalWrite(RELAY_1_PIN, HIGH);
}

void presentation()
{
    sendSketchInfo(SN, SV);
    present(RELAY_1_CHILD_ID, S_BINARY);
}

void loop()
{
    
}

void receive(const MyMessage &message)
{
    // We only expect one type of message from controller. But we better check anyway.
    if (message.getType()==V_STATUS) {
        // Change relay state
        digitalWrite(message.getSensor()-RELAY_1_CHILD_ID+RELAY_1_PIN, message.getBool()?RELAY_ON:RELAY_OFF);

        // Write some debug info
        Serial.print("Incoming change for sensor:");
        Serial.print(message.getSensor());
        Serial.print(", New status: ");
        Serial.println(message.getBool());
    }
}
