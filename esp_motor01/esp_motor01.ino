// ESP32 Code for Stepper Motor Control with Predefined Speed and Angle

// Pin definitions
#define STEP_PIN_XY GPIO_NUM_13
#define DIR_PIN_XY GPIO_NUM_12
#define EN_PIN_XY GPIO_NUM_14

#define STEP_PIN_Z GPIO_NUM_18
#define DIR_PIN_Z GPIO_NUM_19
#define EN_PIN_Z GPIO_NUM_21

// Motor control parameters
const int motor_speed_xy = 500; // Speed in microseconds per step (adjust for desired speed)
const int motor_steps_xy = 200; // Number of steps for each movement

const int motor_speed_z = 800; // Speed in microseconds per step (adjust for desired speed)
const int motor_steps_z = 500; // Number of steps for each movement

// Direction constants
#define CW 1
#define CCW 0

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

  // Disable the motor drivers initially
  digitalWrite(EN_PIN_XY, HIGH);
  digitalWrite(EN_PIN_Z, HIGH);
}

void loop() {
  // Enable motor_z and run predefined movements
  digitalWrite(EN_PIN_Z, LOW);

  // Rotate motor_z clockwise
  rotateStepper(STEP_PIN_Z, DIR_PIN_Z, CW, motor_steps_z, motor_speed_z);
  delay(3000); // Wait 3 seconds

  // Rotate motor_z counter-clockwise
  rotateStepper(STEP_PIN_Z, DIR_PIN_Z, CCW, motor_steps_z, motor_speed_z);
  delay(3000); // Wait 3 seconds

  // Disable motor_z driver
  digitalWrite(EN_PIN_Z, HIGH);
  delay(2000); // Wait 2 seconds before the next loop

  // Uncomment the following section to enable motor_xy movements
  
  digitalWrite(EN_PIN_XY, LOW);

  // Rotate motor_xy clockwise
  rotateStepper(STEP_PIN_XY, DIR_PIN_XY, CW, motor_steps_xy, motor_speed_xy);
  delay(3000); // Wait 3 seconds

  // Rotate motor_xy counter-clockwise
  rotateStepper(STEP_PIN_XY, DIR_PIN_XY, CCW, motor_steps_xy, motor_speed_xy);
  delay(3000); // Wait 3 seconds

  digitalWrite(EN_PIN_XY, HIGH);
  
}

void rotateStepper(int stepPin, int dirPin, int direction, int steps, int speed) {
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
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(speed);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(speed);
  }
}
