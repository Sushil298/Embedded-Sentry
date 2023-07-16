// -------------------------------------------------------//
// ************* Embedded Challenge: EMBEDDED SYSTEMS ****//
// ************* Sushil Bohara (sb7702) ******************//
// ************* Brynn Erin Dafoe (bed6705) **************//
// *************  Nishat Kabita (nsk393) *****************//
// ------------------ May 15, 2023 -----------------------//
//--------------------------------------------------------//
// This is the program for gesture controlled lock using 
// Adafruit Circuit Playground Classic board. 
// --------------------------------------------------------//


#include <Adafruit_CircuitPlayground.h>
#include <EEPROM.h>


// define maximum number of data points 
#define MAX_SIZE 80

// define minimum number of data points
#define min_SIZE 5

// threshold of dynamic time warping distance
#define DTW_THRESHOLD 8
    // Can be decreased to increase the sensitivity


#define RECORDING_ADDR 0 // Memory address in EEPROM to start saving data


#define normalizeFactor 10.0     // to normalize the accel data
const int flickerInterval = 100; // Flicker interval in milliseconds


// Global Variables
float gestureData[MAX_SIZE];    // gesture data
int gestureLength = 0;          // gesture lenght
float storedGesture[MAX_SIZE];  // Stored gesture in EEPROM
int storedGestureLength = 0;    // Stored gesture length
float attemptData[MAX_SIZE];    // Atttempted data to unlock
int windowSize = 5;             // window size for moving average filter
// float dtw[MAX_SIZE][MAX_SIZE];


// Function Definitions
void recordGesture();
void unlockGesture();
float dynamicTimeWarping(float a[], int n, float b[], int m);
void saveGestureToEEPROM();
void loadGestureFromEEPROM();
void UnlockAttemptResult(bool attempt);
void MovingAverage(float inputArray[], int arraySize);
void recordingVisual(uint8_t r, uint8_t g, uint8_t b);


void setup()
{
 CircuitPlayground.begin();
 Serial.begin(115200);
}


void loop()
{ // When left button is pressed
 if (CircuitPlayground.leftButton())
 {
   delay(5); // Human button pressing time
   if (CircuitPlayground.leftButton())
   {   // Check again, to avoid false triggers
     Serial.println("Left button pressed, recording gesture...");   
     recordGesture(); // function call to record data
   }
 }
 // When right button is pressed
 else if (CircuitPlayground.rightButton())
 {
   delay(5); // Human button pressing time
   if (CircuitPlayground.rightButton())
   {    // Check again, to avoid false triggers
     Serial.println("Right button pressed, unlocking gesture..."); 
     unlockGesture();  // function call to attempt unlock
   }
 }
}

// Function to record gesture pattern
void recordGesture()
{
 CircuitPlayground.clearPixels();
 CircuitPlayground.playTone(440, 100); // beep to indicate recording start
 gestureLength = 0;
 float accX, accY, accZ;
 while (CircuitPlayground.leftButton() && gestureLength <= MAX_SIZE)
 {
   recordingVisual(255, 255, 0); // yellow color
   accX = CircuitPlayground.motionX();
   accY = CircuitPlayground.motionY();
   accZ = CircuitPlayground.motionZ();
   gestureData[gestureLength] = (sqrt(accX * accX + accY * accY + accZ * accZ)) /normalizeFactor;
   gestureLength++;
   delay(30); // adjust to control gesture recording speed
 }


 //  call Moving average function 
 MovingAverage(gestureData, gestureLength);

 // Print the recorded data
 Serial.println("---------- Recorded Lock ----------- ");
 for (int i = 0; i < gestureLength; i++)
 {
   Serial.println(gestureData[i]);
 }

 CircuitPlayground.playTone(880, 100); // beep to indicate recording end
 saveGestureToEEPROM(); // save data to EEPROM 
}


void unlockGesture()
{
 CircuitPlayground.clearPixels();
 CircuitPlayground.playTone(440, 100); // beep to indicate recording start
 int attemptLength = 0;
 float accX, accY, accZ;

 while (CircuitPlayground.rightButton() && attemptLength <= MAX_SIZE)
 {
   recordingVisual(0,0,255);
   accX = CircuitPlayground.motionX();
   accY = CircuitPlayground.motionY();
   accZ = CircuitPlayground.motionZ();
   float motionValue = (sqrt(accX * accX + accY * accY + accZ * accZ)) / normalizeFactor;


   // to avoid the recording of nan value
   if (!isnan(motionValue))
   {
     attemptData[attemptLength] = motionValue;
     attemptLength++;
   }


   delay(30); // adjust this delay to control gesture recording speed
 }

 // Call moving average function
 MovingAverage(attemptData, attemptLength);

 // Print the attempted pattern 
 Serial.println("---------- Attempted Data----------- ");
 for (int i = 0; i < attemptLength; i++)
 {
   Serial.println(attemptData[i]);
 }

 CircuitPlayground.playTone(880, 100); // beep to indicate recording end

 // Load the lock pattern saved in EEPROM
 loadGestureFromEEPROM();

 CircuitPlayground.clearPixels();

 // Pass the correct lock and attempt lock to dynamicTimeWarping function  to compare 
 float dtwDistance = dynamicTimeWarping(storedGesture, storedGestureLength, attemptData, attemptLength);

 // Print the comparision result
 Serial.print("---------------------- DTW Distance ----------------------- ");
 Serial.print("Value: ");
 Serial.println(dtwDistance);

// Condition to Unlock
 if (dtwDistance < DTW_THRESHOLD && attemptLength >= min_SIZE)
 {
   UnlockAttemptResult(true);
 }
 // condition to not unlock
 else
 {
   UnlockAttemptResult(false);
 }
}


// Dyanamic Time Warping function (source wikipedia)
float dynamicTimeWarping(float a[], int n, float b[], int m)
{
 float *dtw = new float[m];
 float prev, temp;


 for (int j = 0; j < m; j++)
 {
   dtw[j] = fabs(a[0] - b[j]);
   if (j > 0)
   {
     dtw[j] += dtw[j - 1];
   }
 }

 for (int i = 1; i < n; i++)
 {
   prev = dtw[0];
   dtw[0] += fabs(a[i] - b[0]);


   for (int j = 1; j < m; j++)
   {
     temp = dtw[j];
     dtw[j] = fmin(fmin(prev, dtw[j]), dtw[j - 1]) + fabs(a[i] - b[j]);
     prev = temp;
   }
 }

 float result = dtw[m - 1];
 delete[] dtw;
 return result;
}


// Function to save data to EEPROM
void saveGestureToEEPROM()
{
 int addr = RECORDING_ADDR;
 EEPROM.put(addr, gestureLength);
 addr += sizeof(int);
 for (int i = 0; i < gestureLength; i++)
 {
   EEPROM.put(addr, gestureData[i]);
   addr += sizeof(float);
 }
}


// Function to load the saved data from EEPROM
void loadGestureFromEEPROM()
{
 int addr = RECORDING_ADDR;
 EEPROM.get(addr, storedGestureLength);
 addr += sizeof(int);
 for (int i = 0; i < storedGestureLength; i++)
 {
   EEPROM.get(addr, storedGesture[i]);
   addr += sizeof(float);
 }
}


// Sound Effect and Visuals 
void UnlockAttemptResult(bool attempt)
{
 if (attempt == true)
 { 
   CircuitPlayground.clearPixels();
   CircuitPlayground.playTone(1320, 100); // success sound  (beep)
   for (int i = 0; i < 10; i++)
   {
     CircuitPlayground.setPixelColor(i, 0, 255, 0); // green LED
     delay(200);
   }
 }
 else
 {
   CircuitPlayground.playTone(100, 100);
   delay(100); // double beeps 
   CircuitPlayground.playTone(100, 100);

   for (int i = 0; i < 10; i++)
   {
     CircuitPlayground.setPixelColor(i, 255, 0, 0); // red LED
   }
   delay(200);
 }

 CircuitPlayground.clearPixels();
}


// Moving Average Function 
void MovingAverage(float inputArray[], int arraySize)
{
 // Ensure the input array has at least 5 elements
 if (arraySize < windowSize)
 {
   Serial.println("Error: Input array size must be at least 5.");
   return;
 }

 for (int i = 0; i <= arraySize - windowSize; i++)
 {
   float sum = 0;
   for (int j = i; j < i + windowSize; j++)
   {
     sum += inputArray[j];
   }
   inputArray[i] = sum / windowSize;
 }
}


void recordingVisual(uint8_t r, uint8_t g, uint8_t b)
{
 for (int i = 0; i < 10; i++)
 { // Light up the Neopixels
   CircuitPlayground.setPixelColor(i, r, g, b);
 }
 CircuitPlayground.clearPixels();
}



