/* 
 * Vents controler
 * 
 * Documentation: https://www.mysensors.org
 * Support Forum: https://forum.mysensors.org
 */

#define MY_GATEWAY_SERIAL

#include <MySensors.h>
#define SN "Vents_controller"
#define SV "0.5"

// Actuators for moving the cover up and down respectively.
#define LEFT_VENT_UP_PIN 2
#define LEFT_VENT_DOWN_PIN 3
#define RIGHT_VENT_UP_PIN 4
#define RIGHT_VENT_DOWN_PIN 5
#define BUTTON_PIN 8

#define CHILD_ID 0

const int open_time = 20;      //opening time in seconds
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
  pinMode(LEFT_VENT_UP_PIN, OUTPUT);
  pinMode(LEFT_VENT_DOWN_PIN, OUTPUT);
  pinMode(RIGHT_VENT_UP_PIN, OUTPUT);
  pinMode(RIGHT_VENT_DOWN_PIN, OUTPUT);
  digitalWrite(LEFT_VENT_UP_PIN, HIGH);     //turn relay off
  digitalWrite(LEFT_VENT_DOWN_PIN, HIGH);   //turn relay off
  digitalWrite(RIGHT_VENT_UP_PIN, HIGH);    //turn relay off
  digitalWrite(RIGHT_VENT_DOWN_PIN, HIGH);  //turn relay off
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
  digitalWrite(LEFT_VENT_UP_PIN, HIGH);     //turn relay off
  digitalWrite(LEFT_VENT_DOWN_PIN, HIGH);   //turn relay off
  digitalWrite(RIGHT_VENT_UP_PIN, HIGH);    //turn relay off
  digitalWrite(RIGHT_VENT_DOWN_PIN, HIGH);  //turn relay off
  }
  if (current_time > target_time && state != IDLE) {
    state = IDLE;
    sendState();
  }
}

void receive(const MyMessage &message) {
  if (message.type == V_UP) {
    new_percentage = 99;
    Serial.println("Moving cover up.");
    state = UP;
    sendState();

    digitalWrite(LEFT_VENT_DOWN_PIN, HIGH);   //turn relay off
    digitalWrite(RIGHT_VENT_DOWN_PIN, HIGH);  //turn relay off
    delay(100);
    digitalWrite(LEFT_VENT_UP_PIN, LOW);
    digitalWrite(RIGHT_VENT_UP_PIN, LOW);
    
    target_time = millis() + (new_percentage - current_percentage) * open_time * 10;
    current_percentage = new_percentage;
  }

  if (message.type == V_DOWN) {
    new_percentage = 0;
    Serial.println("Moving cover down.");
    state = DOWN;
    sendState();

    digitalWrite(LEFT_VENT_UP_PIN, HIGH);     //turn relay off
    digitalWrite(RIGHT_VENT_UP_PIN, HIGH);    //turn relay off
    delay(100);
    digitalWrite(LEFT_VENT_DOWN_PIN, LOW);
    digitalWrite(RIGHT_VENT_DOWN_PIN, LOW);

    target_time = millis() + (current_percentage - new_percentage) * open_time * 10;
    current_percentage = new_percentage;
  }

  if (message.type == V_STOP) {
    Serial.println("Stopping cover.");
    state = IDLE;
    sendState();

    digitalWrite(LEFT_VENT_UP_PIN, HIGH);     //turn relay off
    digitalWrite(LEFT_VENT_DOWN_PIN, HIGH);   //turn relay off
    digitalWrite(RIGHT_VENT_UP_PIN, HIGH);    //turn relay off
    digitalWrite(RIGHT_VENT_DOWN_PIN, HIGH);  //turn relay off
  }

  if (message.type == V_PERCENTAGE) {
    new_percentage = atoi(message.data);
    
    Serial.print("Adjusting to new position: ");
    Serial.println(new_percentage);
    
    if (current_percentage < new_percentage) {
      Serial.println("Moving cover up.");
      state = UP;
      sendState();
      
      digitalWrite(LEFT_VENT_DOWN_PIN, HIGH);   //turn relay off
      digitalWrite(RIGHT_VENT_DOWN_PIN, HIGH);  //turn relay off
      delay(100);
      digitalWrite(LEFT_VENT_UP_PIN, LOW);
      digitalWrite(RIGHT_VENT_UP_PIN, LOW);

      target_time = millis() + (new_percentage - current_percentage) * open_time * 10;
      current_percentage = new_percentage;
    }
    else if (current_percentage > new_percentage) {
      Serial.println("Moving cover down.");
      state = DOWN;
      sendState();

      digitalWrite(LEFT_VENT_UP_PIN, HIGH);     //turn relay off
      digitalWrite(RIGHT_VENT_UP_PIN, HIGH);    //turn relay off
      delay(100);
      digitalWrite(LEFT_VENT_DOWN_PIN, LOW);
      digitalWrite(RIGHT_VENT_DOWN_PIN, LOW);

      target_time = millis() + (current_percentage - new_percentage) * open_time * 10;
      current_percentage = new_percentage;
    }
    else {
      Serial.println("Doing nothing.");
      state = IDLE;
      sendState();
    }
  }
}
