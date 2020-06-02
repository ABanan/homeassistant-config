/* 
 * Vents controler
 * 
 * Documentation: https://www.mysensors.org
 * Support Forum: https://forum.mysensors.org
 */

#define MY_GATEWAY_SERIAL

#include <MySensors.h>
#define SN "Vents_controller"
#define SV "0.4"

// Actuators for moving the cover up and down respectively.
#define COVER_UP_ACTUATOR_PIN 2
#define COVER_DOWN_ACTUATOR_PIN 3

#define CHILD_ID 0

const int open_time = 50;      //opening time in seconds
int current_percentage = 0;    //otwarcie początkowe
int new_percentage = 0;        //otwarcie docelowe

static int percentage = 0; // 0=cover is closed, 100=cover is open

MyMessage upMessage(CHILD_ID, V_UP);
MyMessage downMessage(CHILD_ID, V_DOWN);
MyMessage stopMessage(CHILD_ID, V_STOP);
MyMessage percentageMessage(CHILD_ID, V_PERCENTAGE);

// Internal representation of the cover state.
enum State {
  IDLE,
  UP,    // Window covering. Up.
  DOWN,  // Window covering. Down.
};

int state = IDLE;


void sendState() {
  // Send current state and status to gateway.
  send(upMessage.set(state == UP));
  send(downMessage.set(state == DOWN));
  send(stopMessage.set(state == IDLE));
  send(percentageMessage.set(current_percentage));
}

void setup() {
  pinMode(COVER_UP_ACTUATOR_PIN, OUTPUT);
  pinMode(COVER_DOWN_ACTUATOR_PIN, OUTPUT);
  digitalWrite(COVER_UP_ACTUATOR_PIN, HIGH);    //turn relay off
  digitalWrite(COVER_DOWN_ACTUATOR_PIN, HIGH);  //turn relay off

  Serial.println("Setup completed.");
}

void presentation() {
  sendSketchInfo(SN, SV);

  present(CHILD_ID, S_COVER);
}

void loop() {
  if (state == IDLE) {
    digitalWrite(COVER_UP_ACTUATOR_PIN, HIGH);    //turn relay off
    digitalWrite(COVER_DOWN_ACTUATOR_PIN, HIGH);  //turn relay off
  }
  
  if (state == UP && current_percentage < new_percentage ) {
    digitalWrite(COVER_UP_ACTUATOR_PIN, LOW);
    delay(1000);
    current_percentage = current_percentage + 100 / open_time;
    Serial.println("Cover moving.");
  }
  else if (state == UP) {
    state = IDLE;
  }
  
  if (state == DOWN && current_percentage > new_percentage ) {
    digitalWrite(COVER_DOWN_ACTUATOR_PIN, LOW);
    delay(1000);
    current_percentage = current_percentage - 100 / open_time;
  }  
  else if (state == DOWN) {
    state = IDLE;
  }  
}

void receive(const MyMessage &message) {
  if (message.type == V_UP) {
    new_percentage = 99;
    Serial.println("Moving cover up.");
    state = UP;
    sendState();
  }

  if (message.type == V_DOWN) {
    new_percentage = 0;
    Serial.println("Moving cover down.");
    state = DOWN;
    sendState();
  }

  if (message.type == V_STOP) {
    Serial.println("Stopping cover.");
    state = IDLE;
    sendState();
    // Actuators will be switched off in loop().
  }

  if (message.type == V_PERCENTAGE) {
    new_percentage = atoi(message.data);
    
    Serial.print("Adjusting to new position: ");
    Serial.println(new_percentage);
    
    if (current_percentage < new_percentage) {
      Serial.println("Moving cover up.");
      state = UP;
      sendState();
    }
    else {
      Serial.println("Moving cover down.");
      state = DOWN;
      sendState();
    }
  }
}
