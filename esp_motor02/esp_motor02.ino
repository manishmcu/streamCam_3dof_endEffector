// ESP32 Code for Stepper Motor Control with Predefined Speed and Angle

// Pin definitions
#define STEP_PIN_XY GPIO_NUM_13
#define DIR_PIN_XY GPIO_NUM_12
#define EN_PIN_XY GPIO_NUM_14

#define STEP_PIN_Z GPIO_NUM_18
#define DIR_PIN_Z GPIO_NUM_19
#define EN_PIN_Z GPIO_NUM_21

#define LIMIT_SWITCH_XY GPIO_NUM_33
#define LIMIT_SWITCH_Z GPIO_NUM_32

// Motor control parameters
const int motor_speed_xy = 500; // Speed in microseconds per step (adjust for desired speed)
const int motor_speed_z = 800; // Speed in microseconds per step for both up and down
const int safety_line_z = 500; // Safety line at 500 steps for motor_z

// Direction constants
#define CW 1
#define CCW 0

// Variables to track current step positions
long current_steps_xy = 0;
long current_steps_z = 0;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Configure pins for motor_xy
  pinMode(STEP_PIN_XY, OUTPUT);
  pinMode(DIR_PIN_XY, OUTPUT);
  pinMode(EN_PIN_XY, OUTPUT);

  // Configure pins for motor_z
  pinMode(STEP_PIN_Z, OUTPUT);
  pinMode(DIR_PIN_Z, OUTPUT);
  pinMode(EN_PIN_Z, OUTPUT);

  // Configure limit switches
  pinMode(LIMIT_SWITCH_XY, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_Z, INPUT_PULLUP);

  // Disable the motor drivers initially
  digitalWrite(EN_PIN_XY, HIGH);
  digitalWrite(EN_PIN_Z, HIGH);

  // Perform calibration during first boot
  calibrateMotors();
}

void loop() {
  // Check if data is available on the serial port
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // Read input until newline
    input.trim(); // Remove any leading or trailing whitespace

    // Parse input format: motor_name,steps
    int delimiterIndex = input.indexOf(',');
    if (delimiterIndex != -1) {
      String motorName = input.substring(0, delimiterIndex);
      int steps = input.substring(delimiterIndex + 1).toInt();

      if (motorName == "motor_xy") {
        Serial.print("Rotating motor_xy by ");
        Serial.print(steps);
        Serial.println(" steps.");
        digitalWrite(EN_PIN_XY, LOW);
        rotateStepper(STEP_PIN_XY, DIR_PIN_XY, (steps > 0 ? CW : CCW), abs(steps), motor_speed_xy, current_steps_xy);
        digitalWrite(EN_PIN_XY, HIGH);
      } else if (motorName == "motor_z") {
        Serial.print("Rotating motor_z by ");
        Serial.print(steps);
        Serial.println(" steps.");
        digitalWrite(EN_PIN_Z, LOW);
        if (steps > 0) {
          rotateMotorZ(CW, abs(steps));
        } else {
          rotateMotorZ(CCW, abs(steps));
        }
        digitalWrite(EN_PIN_Z, HIGH);
      } else {
        Serial.println("Invalid motor name. Use motor_xy or motor_z.");
      }
    } else {
      Serial.println("Invalid input format. Use motor_name,steps.");
    }
  }

  delay(100); // Small delay to avoid flooding serial communication
}

void rotateStepper(int stepPin, int dirPin, int direction, int steps, int speed, long &current_steps) {
  // Validate inputs
  if (direction != CW && direction != CCW) {
    Serial.println("Invalid direction. Use CW or CCW.");
    return;
  }
  if (speed <= 0) {
    Serial.println("Invalid speed. Must be greater than 0.");
    return;
  }

  // Set motor direction
  digitalWrite(dirPin, direction);

  // Perform steps
  for (int i = 0; i < steps; i++) {
    if (digitalRead(LIMIT_SWITCH_XY) != LOW) {
      // Provide warning instead of breaking
      Serial.println("Warning: Limit switch triggered for motor_xy. Continuing execution.");
    }

    digitalWrite(stepPin, HIGH);
    delayMicroseconds(speed);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(speed);
    current_steps += (direction == CW ? 1 : -1);
  }
}

void rotateMotorZ(int direction, int steps) {
  // Validate direction
  if (direction != CW && direction != CCW) {
    Serial.println("Invalid direction for motor_z. Use CW or CCW.");
    return;
  }

  // Set motor direction
  digitalWrite(DIR_PIN_Z, direction);

  // Perform steps
  for (int i = 0; i < steps; i++) {
    
    if (digitalRead(LIMIT_SWITCH_Z) == LOW) {

      digitalWrite(DIR_PIN_Z, CCW);
     for (int i = 0; i < 150; i++) {
      digitalWrite(STEP_PIN_Z, HIGH);
      delayMicroseconds(motor_speed_z);
      digitalWrite(STEP_PIN_Z, LOW);
      delayMicroseconds(motor_speed_z);
      i+=1;} // Reverting 150 steps back
      digitalWrite(EN_PIN_Z, HIGH); // Provide breaking
      
      Serial.println("Warning: Limit switch >> motor_z.");
    }

    digitalWrite(STEP_PIN_Z, HIGH);
    delayMicroseconds(motor_speed_z);
    digitalWrite(STEP_PIN_Z, LOW);
    delayMicroseconds(motor_speed_z);

    current_steps_z += (direction == CW ? 1 : -1);
  }
}

void calibrateMotors() {
  Serial.println("Calibrating motors...");

  // Calibrate motor_xy
  digitalWrite(EN_PIN_XY, LOW);
  while (digitalRead(LIMIT_SWITCH_XY) != LOW) {
    rotateStepper(STEP_PIN_XY, DIR_PIN_XY, CCW, 1, motor_speed_xy, current_steps_xy);
    // delay(17); // Allow sensor to stabilize
  }
  Serial.println("motor_xy at zero position.");
  rotateStepper(STEP_PIN_XY, DIR_PIN_XY, CW, 50, motor_speed_xy, current_steps_xy);
  digitalWrite(EN_PIN_XY, HIGH);

  // Calibrate motor_z
  digitalWrite(EN_PIN_Z, LOW);
  while (digitalRead(LIMIT_SWITCH_Z) != LOW) {
    rotateMotorZ(CW, 1);
//    delay(17); // Allow sensor to stabilize
  }
  Serial.println("motor_z at zero position.");
  rotateMotorZ(CCW, 50);
  digitalWrite(EN_PIN_Z, HIGH);

  Serial.println("Calibration complete.");
}
