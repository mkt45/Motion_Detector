/*******************************************************
arduino_motion detection v.0.1


1st Oct 2014
********************************************************/
 
// define some values used by the panel and buttons
#define trigPin 12                                       // Pin 12 trigger output
#define echoPin 2                                      // Pin 2 Echo input
#define motionLEDPin 13                          // Pin 13 motion detector indicator output

volatile int range_cm;                                   // The distance detected in centimeters

#define MOTION_BASE_SIZE      24                                            // Length of the motion base samples array
#define MOTION_CURRENT_SIZE   6                                          // Length of the motion current value array
volatile int motion_base_array[MOTION_BASE_SIZE];                   // Motion base array
volatile int motion_current_array[MOTION_CURRENT_SIZE];         // Motion current value array
volatile int motion_base;                                                              // Long term motion base value
volatile int motion_current;                                                           // Current motion distance value
volatile bool motion_detected = false;                                           // True when motion output triggering        


void setup()
{  
  pinMode(trigPin, OUTPUT);                           // Trigger pin set to output
  pinMode(echoPin, INPUT);                            // Echo pin set to input
  pinMode(motionLEDPin, OUTPUT);               // Motion detector indicator pin set to output
 
  Serial.begin (9600);                                // Initialise the serial monitor output 
}

void loop()
{
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  
  motion_detector()                  // activating motion detection

  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  if (distance < 4) {  // This is where the LED On/Off happens
    digitalWrite(led,HIGH); // When the Red condition is met, the Green LED should turn off
  digitalWrite(led2,LOW);
}
  else {
    digitalWrite(led,LOW);
    digitalWrite(led2,HIGH);
  }
  if (distance >= 200 || distance <= 0){
    Serial.println("Out of range");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  delay(500);
}

// --------------------------
// motion_detector() 
// Detects when an object approaches the sensor and triggers the output.
//
// The distance sensor often flips between reading objects at different
// distances. This makes it difficult to use individual sensor readings
// for motion detection. To mitigate the multi reading problem the readings
// are accumulated and averaged before use.
//
// Two averages are maintained. The first average has a long time constant and
// is used as the base reading. The second average has a shorter time constant
// to react quickly to changes. The difference in the two average values can
// be used to detect motion.
// --------------------------
void motion_detector()
{
  int i;
  static int base_ind = 0;                                 // Index into the base array
  static int current_ind = 0;                              // Index into the current array
  int base_sum;                                              // Sum of the base array
  int current_sum;                                          // Sum of the current array
  int range;                                                    // Local value of the distance
  int diff;                                                        // Difference between base and current
  
  range = range_cm;                                     // Copy the distance value
  
  if (range > 1000) 
  {
  range = 1000;                                                         // Limit the range value
  }
  
  
  motion_base_array[base_ind++] = range;                // Insert value into the base array
  
  if (base_ind >= MOTION_BASE_SIZE)                     // Wrap the base index to the start
  {
    base_ind = 0;
  }
  
  base_sum = 0;                                                       // Start the base value processing 
  for (i = 0; i < MOTION_BASE_SIZE; i++)
  {
    base_sum += motion_base_array[i];                      // Sum the contents of the array
  }
  
  motion_base = base_sum / MOTION_BASE_SIZE;   // Divide by the array size

  motion_current_array[current_ind++] = range;         // Insert value into the current array
  
  if (current_ind >= MOTION_CURRENT_SIZE)              // Wrap the current index to the start
  {
    current_ind = 0;
  }

  current_sum = 0;                                    // Start the current value processing
  for (i = 0; i < MOTION_CURRENT_SIZE; i++)
  {
    current_sum += motion_current_array[i];           // Sum the contents of the array
  }
  
  motion_current = current_sum / MOTION_CURRENT_SIZE;// Divide by the array size
  
  diff = motion_base - motion_current;               // Compute the difference
  if (diff > 40)                                                  // Check for movement towards the sensor
  {
    motion_detected = true;                               // Action Trigger
  }
}