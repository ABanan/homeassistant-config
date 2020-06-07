/* 
 * Vents controler
 * 
 * Documentation: https://www.mysensors.org
 * Support Forum: https://forum.mysensors.org
 */

#define MY_GATEWAY_SERIAL

#include <MySensors.h>
#define SN "Vents_controller"
#define SV "0.41"

// Actuators for moving the cover up and down respectively.
#define COVER_UP_ACTUATOR_PIN 2
#define COVER_DOWN_ACTUATOR_PIN 3
#define BUTTON_PIN 8;

#define CHILD_ID 0

const int open_time = 60;      //opening time in seconds
int current_percentage = 0;    //otwarcie początkowe
int new_percentage = 0;        //otwarcie docelowe
unsigned long current_time;
unsigned long target_time;
bool last_button_state;
unsigned long button_pushed_time = 0;
int button_pushed_for = 0;

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
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);
  Serial.println("Setup completed.");
}

void presentation() {
  sendSketchInfo(SN, SV);

  present(CHILD_ID, S_COVER);
}

void loop() {
  
  current_time = millis();

  if (current_time > target_time) {
    digitalWrite(COVER_UP_ACTUATOR_PIN, HIGH);    //turn relay off
    digitalWrite(COVER_DOWN_ACTUATOR_PIN, HIGH);  //turn relay off
  } 
  if (current_time > target_time && state != IDLE) {
    state = IDLE;
    sendState();
  }
  
  if (digitalRead(BUTTON_PIN) = HIGH) {
    
  }
  
}

void receive(const MyMessage &message) {
  if (message.type == V_UP) {
    new_percentage = 99;
    Serial.println("Moving cover up.");
    state = UP;
    sendState();
    
    target_time = millis() + (new_percentage - current_percentage) * open_time * 10;
    digitalWrite(COVER_UP_ACTUATOR_PIN, LOW);
  }

  if (message.type == V_DOWN) {
    new_percentage = 0;
    Serial.println("Moving cover down.");
    state = DOWN;
    sendState();
    
    target_time = millis() + (current_percentage - new_percentage) * open_time * 10;
    digitalWrite(COVER_DOWN_ACTUATOR_PIN, LOW);
  }

  if (message.type == V_STOP) {
    Serial.println("Stopping cover.");
    state = IDLE;
    sendState();

    digitalWrite(COVER_UP_ACTUATOR_PIN, HIGH);    //turn relay off
    digitalWrite(COVER_DOWN_ACTUATOR_PIN, HIGH);  //turn relay off
  }

  if (message.type == V_PERCENTAGE) {
    new_percentage = atoi(message.data);
    
    Serial.print("Adjusting to new position: ");
    Serial.println(new_percentage);
    
    if (current_percentage < new_percentage) {
      Serial.println("Moving cover up.");
      state = UP;
      sendState();
      
      target_time = millis() + (new_percentage - current_percentage) * open_time * 10;
      digitalWrite(COVER_UP_ACTUATOR_PIN, LOW);
      current_percentage = new_percentage;
    }
    else if (current_percentage > new_percentage) {
      Serial.println("Moving cover down.");
      state = DOWN;
      sendState();
      
      target_time = millis() + (current_percentage - new_percentage) * open_time * 10;
      digitalWrite(COVER_DOWN_ACTUATOR_PIN, LOW);
      current_percentage = new_percentage;
    }
    else {
      Serial.println("Doing nothing.");
      state = IDLE;
      sendState();
    }
  }
}
