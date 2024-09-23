#include "MeMCore.h"

// Line Follower
  MeLineFollower lineFinder(PORT_2); // Assign lineFinder to RJ25 port 2

// LED Display
  #define LED_PIN 13 // Pin used for LED Display

  MeRGBLed led(0, 30); // Based on hardware connections on mCore; cannot change

  // Function to display the colour with the specified RGB values on the LED display
  void displayColour(int red, int green, int blue) {
    led.setColorAt(0, red, green, blue);
    led.setColorAt(1, red, green, blue); 
    led.show();
  }

// Colour Sensor
  #define RGB_WAIT 200 // Delay before the next RGB colour turns on to allow LDR to stabilise (in milliseconds)
  #define LDR_WAIT 10 // Delay before taking another LDR reading (in milliseconds) 
  #define LDR 0 // LDR sensor pin at A0
  #define LDR_NO_OF_READINGS 5
  #define LOW 0
  #define HIGH 255
  
  // Integer values for colours
  #define RED 1
  #define ORANGE 2
  #define GREEN 3
  #define BLUE 4 
  #define PURPLE 5

  // Define decoder select pins
  #define DECODER_SELECT_1 A2
  #define DECODER_SELECT_2 A3
  int decoderSelect1Array[] = {HIGH, LOW, HIGH};
  int decoderSelect2Array[] = {LOW, HIGH, HIGH};

  int idealRGBValues[6][3] = {
    {251, 255, 255}, // White
    {225, 57, 42}, // Red
    {249, 116, 54}, // Orange
    {53, 112, 59}, // Green
    {70, 166, 223}, // Blue
    {107, 107, 150} // Purple
  };
  #define INITIAL_MARGIN 20
  int margin = INITIAL_MARGIN; // Margin of error when comparing detected RGB values to ideal RGB values

  // Arrays to hold RGB values
  float colourArray[] = {0, 0, 0}; // Currently detected RGB values
  float blackArray[] = {85.0, 114.0, 97.0}; // RGB calibrated for black
  float greyDiff[] = {138.0, 173.0, 121.0}; // RGB difference between black and white

  // Function to normalise RGB values to between 0 and 255 (inclusive)
  int normaliseRGB(int value) {
    if (value < 0) {
      return 0;
    }
    if (value > 255) {
      return 255;
    }
    return value;
  }

  // Function to match current RGB values to a specific colour's RGB values
  bool colourMatch(int colour) {
    for (int rgb = 0; rgb < 3; rgb += 1) {
      if (colourArray[rgb] < idealRGBValues[colour][rgb] - margin ||
       colourArray[rgb] > idealRGBValues[colour][rgb] + margin) {
        return false; // Return false if one of the RGB values are not within the margin of error
      }
    }
    return true; // Return true if all RGB values are within the margin of error
  }

  // Function to get the colour that matches the detected RGB values
  int getColour() {
    // Attempt to match detected RGB values to all colours
    for (int colour = 0; colour < 6; colour += 1) {
      if (colourMatch(colour)) {
        return colour; // Return colour if the current colour matches the detected RGB values
      }
    }
    return -1; // Return -1 if no colours match the detected RGB values
  }

  // Function that returns the average reading for the requested number of times of LDR scanning
  int getAvgReading(int times) {      
    int total = 0;

    // Take the reading as many times as requested and add them up
    for (int i = 0; i < times; i += 1) {
      int reading = analogRead(LDR);
      total += reading;
      delay(LDR_WAIT);
    }
    // Calculate the average and return it
    return total / times;
  }

  // Function that detects the current RGB values of the colour beneath the mBot and stores them in colourArray
  void readColour() {
    // Set LED display to pink to signal colour-reading
    displayColour(255, 72, 83);

    // Turn on one colour at a time and LDR reads 5 times for each colour
    for (int c = 0; c < 3; c += 1) {
      // Set decoder select pins to the appropriate voltages to turn on current RGB LED
      digitalWrite(DECODER_SELECT_1, decoderSelect1Array[c]);
      digitalWrite(DECODER_SELECT_2, decoderSelect2Array[c]);

      // Delay before the next RGB colour turns on to allow LDR to stabilise
      delay(RGB_WAIT); 

      // Get the average of 5 consecutive readings for the current colour 
      colourArray[c] = getAvgReading(LDR_NO_OF_READINGS); 

      // Average reading returned minus the lowest value, divided by the maximum possible range, 
      // multiplied by 255, will give a value between 0 and 255 (inclusive), 
      // representing the value for the current reflectivity (i.e. the colour the LDR is exposed to)
      colourArray[c] = (colourArray[c] - blackArray[c]) / (greyDiff[c]) * 255;
      
      // Delay before the next RGB colour turns on to allow LDR to stabilise
      delay(RGB_WAIT);

      // Normalise detected RGB values to between 0 and 255 (inclusive)
      for (int c = 0; c < 3; c += 1) {
        colourArray[c] = normaliseRGB(int(colourArray[c]));
      }
    }

    // Turn off RGB LEDs after readings are taken
    digitalWrite(DECODER_SELECT_1, LOW);
    digitalWrite(DECODER_SELECT_2, LOW);
  }

// Motion
  #define TURN_DURATION 340 // Delay for mBot to turn 90 degrees (in milliseconds)
  #define STOP_DURATION 100 // Delay for mBot to remain stationary (in milliseconds)
  #define MOTOR_SPEED_DELTA 69 // Difference between left motor and right motor speed for nudging
  #define MOTOR_SPEED 220
  #define FORWARD_DURATION 800 // Delay for mBot to move forward by 1 grid

  MeDCMotor leftMotor(M1); // Assign leftMotor to port M1
  MeDCMotor rightMotor(M2); // Assign rightMotor to port M2

  void stopMotors(long milliseconds) {
    // Stop both motors
    leftMotor.stop(); 
    rightMotor.stop(); 

    delay(milliseconds); // Specify duration to stop mBot
  }

  void goStraight() {
    // Both motors move in the forward direction
    leftMotor.run(-MOTOR_SPEED);
    rightMotor.run(MOTOR_SPEED);
  }

  void goForward() {
    goStraight();
    delay(FORWARD_DURATION); // Delay for mBot to move forward by 1 grid
  }

  void turnLeft() {
    // Left motor moves backwards while right motor moves forwards
    leftMotor.run(MOTOR_SPEED);
    rightMotor.run(MOTOR_SPEED);

    delay(TURN_DURATION); // Delay for mBot to turn 90 degrees
    stopMotors(STOP_DURATION);
  }

  void turnRight() {
    // Left motor moves forwards while right motor moves backwards
    leftMotor.run(-MOTOR_SPEED);
    rightMotor.run(-MOTOR_SPEED);

    delay(TURN_DURATION); // Delay for mBot to turn 90 degrees
    stopMotors(STOP_DURATION);
  }

  void doubleLeft() {
    turnLeft();
    stopMotors(STOP_DURATION);
    goForward();
    stopMotors(STOP_DURATION);
    turnLeft();
  }

  void doubleRight() {
    turnRight();
    stopMotors(STOP_DURATION);
    goForward();
    stopMotors(STOP_DURATION);
    turnRight(); 
  }

  void spin() {
    // Left motor moves backwards while right motor moves forwards
    leftMotor.run(MOTOR_SPEED);
    rightMotor.run(MOTOR_SPEED);

    delay(TURN_DURATION * 2); // Delay for mBot to turn 180 degrees
    stopMotors(STOP_DURATION);
  }

  void nudgeLeft() {
    // Right motor moves faster than left motor to nudge left
    leftMotor.run(-MOTOR_SPEED + MOTOR_SPEED_DELTA);
    rightMotor.run(MOTOR_SPEED);
  }

  void nudgeRight() {
    // Left motor moves faster than right motor to nudge right
    leftMotor.run(-MOTOR_SPEED);
    rightMotor.run(MOTOR_SPEED - MOTOR_SPEED_DELTA);
  }

// Celebrate
  // Note frequencies (in Hz)
  #define G4 392
  #define A4 440
  #define B4 494
  #define C5 523
  #define D5 587
  #define E5 659
  #define G5 784

  // Note durations (in milliseconds)
  #define SEMIQUAVER 125
  #define QUAVER 250
  #define CROTCHET 500
  #define MINIM 1000

  MeBuzzer buzzer; // Create a buzzer object

  bool has_celebrated = false; // Boolean to check whether mBot has celebrated

  void celebrate() {
    has_celebrated = true;
    stopMotors(0); // Stop mBot for celebration

    // Play celebratory tune
    // Bar 1
    buzzer.tone(G4, SEMIQUAVER);
    buzzer.tone(A4, SEMIQUAVER);
    buzzer.tone(C5, SEMIQUAVER);
    buzzer.tone(A4, SEMIQUAVER);
    buzzer.tone(E5, 1.5 * QUAVER);
    buzzer.tone(E5, 1.5 * QUAVER);
    buzzer.tone(D5, 1.5 * CROTCHET);

    // Bar 2
    buzzer.tone(G4, SEMIQUAVER);
    buzzer.tone(A4, SEMIQUAVER);
    buzzer.tone(C5, SEMIQUAVER);
    buzzer.tone(A4, SEMIQUAVER);
    buzzer.tone(D5, 1.5 * QUAVER);
    buzzer.tone(D5, 1.5 * QUAVER);
    buzzer.tone(C5, 1.5 * CROTCHET);

    // Bars 3 and 4
    buzzer.tone(G4, SEMIQUAVER);
    buzzer.tone(A4, SEMIQUAVER);
    buzzer.tone(C5, SEMIQUAVER);
    buzzer.tone(A4, SEMIQUAVER);
    buzzer.tone(C5, CROTCHET);
    buzzer.tone(D5, QUAVER);
    buzzer.tone(B4, QUAVER);
    buzzer.tone(A4, QUAVER);
    buzzer.tone(G4, CROTCHET);
    buzzer.tone(G4, QUAVER);
    buzzer.tone(D5, CROTCHET);
    buzzer.tone(C5, MINIM);

    // Bar 5
    buzzer.tone(G4, SEMIQUAVER);
    buzzer.tone(A4, SEMIQUAVER);
    buzzer.tone(C5, SEMIQUAVER);
    buzzer.tone(A4, SEMIQUAVER);
    buzzer.tone(E5, 1.5 * QUAVER);
    buzzer.tone(E5, 1.5 * QUAVER);
    buzzer.tone(D5, 1.5 * CROTCHET);

    // Bar 6
    buzzer.tone(G4, SEMIQUAVER);
    buzzer.tone(A4, SEMIQUAVER);
    buzzer.tone(C5, SEMIQUAVER);
    buzzer.tone(A4, SEMIQUAVER);
    buzzer.tone(G5, CROTCHET);
    buzzer.tone(B4, QUAVER);
    buzzer.tone(C5, QUAVER);
    buzzer.tone(B4, QUAVER);
    buzzer.tone(A4, QUAVER);

    // Bars 7 and 8
    buzzer.tone(G4, SEMIQUAVER);
    buzzer.tone(A4, SEMIQUAVER);
    buzzer.tone(C5, SEMIQUAVER);
    buzzer.tone(A4, SEMIQUAVER);
    buzzer.tone(C5, CROTCHET);
    buzzer.tone(D5, QUAVER);
    buzzer.tone(B4, QUAVER);
    buzzer.tone(A4, QUAVER);
    buzzer.tone(G4, CROTCHET);
    buzzer.tone(G4, QUAVER);
    buzzer.tone(D5, CROTCHET);
    buzzer.tone(C5, MINIM);
  }

// Ultrasonic Sensor
  #define ULTRASONIC_PIN 12
  #define SPEED_OF_SOUND 0.0345 // In centimetres per microsecond
  #define IDEAL_ULTRASONIC_DISTANCE 10 // Ideal distance to the right wall such that the mBot is equidistant from the left and right wall
  #define MAX_ULTRASONIC_DISTANCE 20 // Maximum distance allowed for the ultrasonic sensor to detect (about the length of 1 grid) 

  float getUltrasonicDistance() {
    // Send sound wave to detect distance
    pinMode(ULTRASONIC_PIN, OUTPUT);
    digitalWrite(ULTRASONIC_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_PIN, LOW);

    pinMode(ULTRASONIC_PIN, INPUT);
    int microseconds = pulseIn(ULTRASONIC_PIN, HIGH); // Get time taken for ultrasonic sensor to detect reflected pulse
    return microseconds * SPEED_OF_SOUND / 2; // Distance detected by ultrasonic sensor
  }

// IR Sensor
  #define IR_DETECTOR_PIN A1
  #define IR_DELAY 10 // Delay between reading ambient reading and wall reading from IR detector (in milliseconds)
  #define IDEAL_IR_READING 6 // Ideal reading such that mBot is equidistant between left and right wall

  void turnOffIREmitter() {
    digitalWrite(DECODER_SELECT_1, LOW);
    digitalWrite(DECODER_SELECT_2, LOW);
  }

  void turnOnIREmitter() {
    digitalWrite(DECODER_SELECT_1, HIGH);
    digitalWrite(DECODER_SELECT_2, HIGH);
  }

  int getIRDetectorReading() {
    // Get ambient reading
    int ambientReading = analogRead(IR_DETECTOR_PIN);
    delayMicroseconds(IR_DELAY);

    // Get wall reading
    turnOnIREmitter();
    int wallReading = analogRead(IR_DETECTOR_PIN);
    turnOffIREmitter();

    // Return difference between ambient and wall reading
    return ambientReading - wallReading;
  }

void setup() {
  // Initialise IR detector pin
  pinMode(IR_DETECTOR_PIN, INPUT);

  // Initialise decoder select pins
  pinMode(DECODER_SELECT_1, OUTPUT);
  pinMode(DECODER_SELECT_2, OUTPUT);
  digitalWrite(DECODER_SELECT_1, LOW);
  digitalWrite(DECODER_SELECT_2, LOW);

  // Initialise pin for LED display
  led.setpin(LED_PIN);
}

void loop() {
  // Stop forever once celebrated
  if (has_celebrated) {
    return;
  }

  // Adjust movement (nudge left or right) to move in a straight line and avoid walls
  int ultrasonicDistance = (int) getUltrasonicDistance();
  int IRReading = getIRDetectorReading();
  
  // Nudge left if mBot is too close to the right wall (low ultrasonic sensor distance)
  // OR too far from the left wall (IR reading is equal to the ideal reading)
  if (ultrasonicDistance < IDEAL_ULTRASONIC_DISTANCE || (IRReading == IDEAL_IR_READING)) {
    nudgeLeft();
  
  // Nudge right if mBot is too close to the left wall (high IR reading)
  // OR too far from the right wall (ultrasonic sensor distance is between ideal distance, and maximum detectable distance)
  } else if (IRReading > IDEAL_IR_READING ||
   (IDEAL_ULTRASONIC_DISTANCE < ultrasonicDistance && ultrasonicDistance < MAX_ULTRASONIC_DISTANCE)) {
    nudgeRight();

  // Continue moving straight otherwise
  } else {
    goStraight();
  }

  // Check for black line
  int sensorState = lineFinder.readSensors();
  // Stop mBot when black line is detected
  if (sensorState == S1_IN_S2_IN) {
    stopMotors(0);
    
    // Detect colour beneath mBot
    int colour = -1;
    int totalReadings = 0;
    do {
      // Read colour once, and a second time if colour cannot be identified
      if (totalReadings < 2) {
        readColour();

      // If colour cannot be identified after 2 readings, increase margin of error for colour identification 
      } else {
        margin += 10;
      }
      colour = getColour();
      totalReadings += 1;
    } while (colour == -1); // Repeat until colour is identified
    margin = INITIAL_MARGIN; // Reset margin of error

    // Move based on colour
    if (colour == RED) {
      displayColour(255, 0, 0); // Show colour on LED display
      turnLeft();

    } else if (colour == GREEN) {
      displayColour(0, 255, 0);
      turnRight();

    } else if (colour == ORANGE) {
      displayColour(255, 41, 0);
      spin();
      
    } else if (colour == PURPLE) {
      displayColour(128, 0, 128);
      doubleLeft();

    } else if (colour == BLUE) {
      displayColour(0, 0, 255);
      doubleRight();

    } else {
      displayColour(69, 69, 69);
      celebrate();
    }
  }

  // Refresh rate (determines responsiveness of mBot)
  delay(10);
}
