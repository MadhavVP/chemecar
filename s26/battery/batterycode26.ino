/*
Code for the 2023 Purdue Chem-E-Car powered by an alkaline battery for the regional competition.
Created by Kenneth Meng, Madhav Valiyaparambil, Alice Hai Last updated March 1, 2024
*/

#define PHOTO_PIN A1
#define SWITCH_PIN A0 
#define VOLTAGE_PIN A3

#define WARMWHITE_LED 12
#define GREEN_LED 11
#define YELLOW_LED 10

#define AIN1 5
#define AIN2 6
#define BIN1 7
#define BIN2 8

#define NUM_TO_AVERAGE 120 // how many data points to average to determine the initial value
#define NUM_TO_SKIP 85 // number of data points at the beginning to skip to account for change in light level from opening the ball valve


// variables for keeping track of time for recording data without using delay()
unsigned long startTime;
unsigned long currentTime;
unsigned long previousTime = 0; 

void setup() {
  // start serial communication
  Serial.begin(9600);

  // set up pins
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(WARMWHITE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
}

void loop() {

  int photoOut;
  unsigned long photoTotal = 0;
  float initialPhotoAvg = 1;
  int runningStatus = 1;
  float photoFraction = 0;
  int i = 1;
  int photo1 = 0;
  int photo2 = 0;
  int photo3 = 0;
  float photoAvg = 0;
  int thresh = 0;

  digitalWrite(WARMWHITE_LED, LOW); // warm white LED off indicates the car will start

  /*while(true) { //Code to check if photoresistor is working
    photoOut = analogRead(PHOTO_PIN);
    photo1 = analogRead(PHOTO_PIN);
    photo2 = analogRead(PHOTO_PIN);
    photo3 = analogRead(PHOTO_PIN);

    photoAvg = (float) (photo1 + photo2 + photo3)/3;
    Serial.print(photoAvg);
    Serial.print("\n");
    delay(200);
  }*/
  
  Serial.println("Open ball valve to start.");
  while (!digitalRead(SWITCH_PIN)){ // reed switch is open (magnet is close to it), keep car stationary
    delay(200);
    if(analogRead(PHOTO_PIN) != 0) // if the photoresistor is connected (0 if unconnected)
    {
      digitalWrite(YELLOW_LED, HIGH); // turn on the yellow LED
    } else
    {
      digitalWrite(YELLOW_LED, LOW); // turn off the yellow LED
    }
  }

  Serial.println("Ball valve was opened, starting car");
  digitalWrite(GREEN_LED, HIGH); // turn on green LED to indicate ball valve opening was detected
  startTime = millis();

  do{
    currentTime = millis() - startTime; // calculate time since the car was started
    if (currentTime > 12000) // 8.5-second delay to allow the operator to fully open the ball valve
    {
      runMotors(); // run the motors to drive the car forward
    }
    photoOut = analogRead(PHOTO_PIN); // get photoresistor output

    photoFraction = (float) photoOut / initialPhotoAvg;
    if (initialPhotoAvg == 1) {
      Serial.print(currentTime/1000.0); Serial.print(","); Serial.print(photoOut); Serial.print(","); Serial.print(" "); Serial.println("");
    }
    else {
      Serial.print(currentTime/1000.0); Serial.print(","); Serial.print(photoOut); Serial.print(","); Serial.print(photoFraction); Serial.println(""); // output time and value
    }
    //Serial.print(photoOut);Serial.println("");

    if ((i < NUM_TO_AVERAGE) && (i > NUM_TO_SKIP)) { //calculate total of first N values
      photoTotal = photoTotal + photoOut;
    }

    if (i == NUM_TO_SKIP) {
      Serial.println("Starting to avg data");
    }

    i++; // increment i by 1 every 0.1 seconds

    if (i == NUM_TO_AVERAGE){ // calculate average of first N values
      initialPhotoAvg = (float) photoTotal / (i-1-NUM_TO_SKIP);
      Serial.print("Ending avg data. Avg is: ");
      Serial.print(initialPhotoAvg);
      Serial.println("");
      //initialPhotoAvg = (float) photoTotal / (i-1);
    }

    photo1 = analogRead(PHOTO_PIN);
    photo2 = analogRead(PHOTO_PIN);
    photo3 = analogRead(PHOTO_PIN);

    photoAvg = (float) (photo1 + photo2 + photo3)/3;

    if (i > NUM_TO_AVERAGE && (((float) photoAvg / initialPhotoAvg) <= 0.5)){ // if the value is less than 50% of the initial average, stop the car
      thresh++;
      if (thresh >= 10) {
        Serial.print("Stopped car at time = ");
        Serial.print((millis()-startTime)/1000.0);
        Serial.print(" s");
        Serial.println("");
        runningStatus = 0;
        thresh = 0;
      }
    }

    if (!digitalRead(SWITCH_PIN)) // ball valve is closed, emergency shutdown
    {
      runningStatus = 0;
      Serial.println("Ball valve was closed, stopping car");
    }

    delay(100); // data collection every 0.1 seconds
  }while(runningStatus);

  stopMotors(); // stops the motors
  Serial.print("Car was stopped at time = "); Serial.print(currentTime/1000.0); Serial.print("s"); Serial.println("");
  digitalWrite(GREEN_LED, LOW); // turn off green LED to indicate car has stopped
  
  Serial.println("Close Ball Valve to reset.");

  while (digitalRead(SWITCH_PIN)){ // reed switch is closed (magnet is not close to it), wait
    delay(200);
  }
}

// runs the motors one way
void runMotors() {

  // car go backward
  /*
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  */

  // one motor slower; backward
  /*
  analogWrite(AIN1, 200);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  */

  // one motor slower; forward
  digitalWrite(AIN2, HIGH);
  digitalWrite(AIN1, LOW);
  analogWrite(BIN2, 130);
  digitalWrite(BIN1, LOW);
  //car go forward
  /*
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH); 
  */
}

// stops the motors
void stopMotors(){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}
