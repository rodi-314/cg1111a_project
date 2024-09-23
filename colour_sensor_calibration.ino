#define RGB_WAIT 200 // Delay before the next RGB colour turns on to allow LDR to stabilise (in milliseconds)
#define LDR_WAIT 10 // Delay before taking another LDR reading (in milliseconds) 
#define SAMPLE_DELAY 5000 // Delay to get sample ready (in milliseconds)
#define LDR 0 // LDR sensor pin at A0
#define LDR_NO_OF_READINGS 5
#define LOW 0
#define HIGH 255

// Define decoder select pins
#define DECODER_SELECT_1 A2
#define DECODER_SELECT_2 A3
int decoderSelect1Array[] = {HIGH, LOW, HIGH};
int decoderSelect2Array[] = {LOW, HIGH, HIGH};

// Arrays to hold RGB values
float colourArray[] = {0, 0, 0};
float whiteArray[] = {0, 0, 0};
float blackArray[] = {0.0, 0.0, 0.0};
float greyDiff[] = {0.0, 0.0, 0.0};

// For formatting values printed
char colourStr[3][5] = {"R = ", "G = ", "B = "};
char intToColour[6][7] = {
  "WHITE ",
  "RED   ",
  "ORANGE",
  "GREEN ",
  "BLUE  ",
  "PURPLE"
};

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
int readColour() {
  // Turn on one colour at a time and LDR reads 5 times for each colour
  for (int c = 0; c < 3; c += 1) {    
    Serial.print(colourStr[c]);
    
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

    // Print the value for the current colour LED, which corresponds to either the R, G or B of the RGB code
    Serial.print(int(colourArray[c]));
    Serial.print(", ");
  } 
  Serial.println();

  return getColour();
}

// Function to set white balance and black balance (calibration)
void setBalance() {
  // Scan the white sample and set white balance
  Serial.println("Put White Sample For Calibration...");
  delay(SAMPLE_DELAY); // Delay for five seconds to get sample ready
  
  // Go through one colour at a time, set the maximum reading for each colour - red, green and blue to the white array
  for (int c = 0; c < 3; c += 1) {
    // Set decoder select pins to the appropriate voltages to turn on current RGB LED
    digitalWrite(DECODER_SELECT_1, decoderSelect1Array[c]);
    digitalWrite(DECODER_SELECT_2, decoderSelect2Array[c]);
    
    // Delay before the next RGB colour turns on to allow LDR to stabilise
    delay(RGB_WAIT); 

    // Get the average of 5 consecutive readings for white
    whiteArray[c] = getAvgReading(LDR_NO_OF_READINGS);

    // Turn off RGB LEDs after readings are taken
    digitalWrite(DECODER_SELECT_1, LOW);
    digitalWrite(DECODER_SELECT_2, LOW);
    
    // Delay before the next RGB colour turns on to allow LDR to stabilise
    delay(RGB_WAIT); 
  }

  // Scan black sample and set black balance
  Serial.println("Put Black Sample For Calibration...");
  delay(SAMPLE_DELAY); // Delay for five seconds to get sample ready 
  
  // Go through one colour at a time, set the minimum reading for red, green and blue to the black array
  for (int c = 0; c < 3; c += 1) {
    // Set decoder select pins to the appropriate voltages to turn on current RGB LED
    digitalWrite(DECODER_SELECT_1, decoderSelect1Array[c]);
    digitalWrite(DECODER_SELECT_2, decoderSelect2Array[c]);
    
    // Delay before the next RGB colour turns on to allow LDR to stabilise
    delay(RGB_WAIT); 

    // Get the average of 5 consecutive readings for black
    blackArray[c] = getAvgReading(LDR_NO_OF_READINGS);
    
    // Turn off RGB LEDs after readings are taken
    digitalWrite(DECODER_SELECT_1, LOW);
    digitalWrite(DECODER_SELECT_2, LOW);
    
    // Delay before the next RGB colour turns on to allow LDR to stabilise
    delay(RGB_WAIT); 

    // The difference between the maximum and the minimum gives the range
    greyDiff[c] = whiteArray[c] - blackArray[c];

    // Print blackArray and greyDiff for calibration
    Serial.println(blackArray[c]);
    Serial.println(greyDiff[c]);
  }

  // Delay another 5 seconds to get colour samples ready
  Serial.println("Colour Sensor Is Ready!");
  delay(SAMPLE_DELAY);
}

void setup() {
  // Initialise decoder select pins
  pinMode(DECODER_SELECT_1, OUTPUT);
  pinMode(DECODER_SELECT_2, OUTPUT);
  digitalWrite(DECODER_SELECT_1, LOW);
  digitalWrite(DECODER_SELECT_2, LOW);

  Serial.begin(9600); // Begin serial communication
  setBalance(); // Calibration
}

void loop() {
  int colour = readColour();
  Serial.println(colour);

  // Print colour name
  if (colour >= 0) {
    Serial.println(intToColour[colour]);
  }
}