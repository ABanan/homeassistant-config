/* 
 * Vents controler
 * 
 * Documentation: https://www.mysensors.org
 * Support Forum: https://forum.mysensors.org
 */

#define MY_GATEWAY_SERIAL

#include <MySensors.h>
#define SN "Vents_controller"
#define SV "0.1"

// Actuators for moving the cover up and down respectively.
#define COVER_UP_ACTUATOR_PIN 2
#define COVER_DOWN_ACTUATOR_PIN 3

#define CHILD_ID 0

const int open_time = 100;      //opening time in seconds
int current_percentage = 0;  //otwarcie początkowe
int new_percentage = 0;      //otwarcie docelowe

// Internal representation of the cover state.
enum State {
  IDLE,
  UP, // Window covering. Up.
  DOWN, // Window covering. Down.
};

static int state = IDLE;
static int percentage = 0; // 0=cover is closed, 100=cover is open
static bool initial_state_sent = false;
MyMessage upMessage(CHILD_ID, V_UP);
MyMessage downMessage(CHILD_ID, V_DOWN);
MyMessage stopMessage(CHILD_ID, V_STOP);
MyMessage percentageMessage(CHILD_ID, V_PERCENTAGE);

void sendState() {
  // Send current state and status to gateway.
  send(upMessage.set(state == UP));
  send(downMessage.set(state == DOWN));
  send(stopMessage.set(state == IDLE));
  send(percentageMessage.set(percentage));
}

void setup() {
    digitalWrite(COVER_UP_ACTUATOR_PIN, LOW);
    digitalWrite(COVER_DOWN_ACTUATOR_PIN, LOW);
}

void presentation() {
  sendSketchInfo(SN, SV);

  present(CHILD_ID, S_COVER);
}

void loop() {
  if (!initial_state_sent) {
    sendState();
    initial_state_sent = true;
  }

  if (state == IDLE) {
    digitalWrite(COVER_UP_ACTUATOR_PIN, LOW);
    digitalWrite(COVER_DOWN_ACTUATOR_PIN, LOW);
  }

  if (state == UP) {
    Serial.println("Cover is up.");
    // Update status and state; send it to the gateway.
    percentage = 1;
    state = IDLE;
    sendState();
    // Actuators will be disabled in next loop() iteration.
  }

  if (state == DOWN) {
    Serial.println("Cover is down.");
    // Update status and state; send it to the gateway.
    percentage = 0;
    state = IDLE;
    sendState();
    // Actuators will be disabled in next loop() iteration.
  }
}

void receive(const MyMessage &message) {
  if (message.type == V_UP) {
    // Set state to covering up and send it back to the gateway.
    state = UP;
    sendState();
    Serial.println("Moving cover up.");

    // Activate actuator until the sensor returns HIGH in loop().
    digitalWrite(COVER_UP_ACTUATOR_PIN, HIGH);
  }

  if (message.type == V_DOWN) {
    // Set state to covering up and send it back to the gateway.
    state = DOWN;
    sendState();
    Serial.println("Moving cover down.");
    // Activate actuator until the sensor returns HIGH in loop().
    digitalWrite(COVER_DOWN_ACTUATOR_PIN, HIGH);
  }

  if (message.type == V_STOP) {
    // Set state to idle and send it back to the gateway.
    state = IDLE;
    sendState();
    Serial.println("Stopping cover.");

    // Actuators will be switched off in loop().
  }

    if (message.type == V_PERCENTAGE) {
    
    new_percentage = atoi(message.data);
    
    Serial.print("Adjusting to new position: ");
    Serial.println(new_percentage);
    
    if (current_percentage < new_percentage) {
      int adjust_time = (new_percentage - current_percentage) * open_time;
      digitalWrite(COVER_UP_ACTUATOR_PIN, HIGH);
      delay(adjust_time*1000);
      digitalWrite(COVER_UP_ACTUATOR_PIN, LOW);
    }
    else {
      int adjust_time = (current_percentage - new_percentage) * open_time;
      digitalWrite(COVER_DOWN_ACTUATOR_PIN, HIGH);
      delay(adjust_time*1000);
      digitalWrite(COVER_DOWN_ACTUATOR_PIN, LOW);
  }
  }
}

void adjustVents() {
  if (current_percentage < new_percentage) {
    int adjust_time = (new_percentage - current_percentage) * open_time;
    digitalWrite(COVER_UP_ACTUATOR_PIN, HIGH);
    delay(adjust_time*1000);
    digitalWrite(COVER_UP_ACTUATOR_PIN, LOW);
  }
  else {
    int adjust_time = (current_percentage - new_percentage) * open_time;
    digitalWrite(COVER_DOWN_ACTUATOR_PIN, HIGH);
    delay(adjust_time*1000);
    digitalWrite(COVER_DOWN_ACTUATOR_PIN, LOW);
  }
}
