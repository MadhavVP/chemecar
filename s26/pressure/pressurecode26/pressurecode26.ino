int ballvalve = 6;
int solenoid = 4;
int swtch = 12;
int photoin = A3;
int photoout = 2;
int samplesize = 1000;
int interval = 100;
int thresholdtostop = 30;
int offfirst = 1;
double multiplicativethreshold = 0.8;
void setup() {
  pinMode(ballvalve, OUTPUT);
  pinMode(solenoid, OUTPUT);
  pinMode(photoout, OUTPUT);
  pinMode(swtch, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  Serial.println("Starting code!");
  

  
  // digitalWrite(ballvalve, LOW);
  digitalWrite(solenoid, LOW);
  Serial.println("Set sol low");

  digitalWrite(photoout, HIGH);
  while (true) {
    int switchstate = digitalRead(swtch);
    if (switchstate == HIGH) { // Button pressed (connected to ground)
      digitalWrite(ballvalve, HIGH);
      digitalWrite(solenoid, LOW);
      Serial.println("Set sol low");
      delay(100);
      offfirst = 1;
      // break;
    } else if (offfirst == 1) {
      offfirst = 0;
      Serial.println("Turned ON, stopping started");
      Serial.print("Averaging values first "); Serial.print(samplesize); Serial.println(" values");
      long summedvals = 0;
      for (int i = 0; i < samplesize; i++) {
        summedvals += analogRead(photoin);
      }
      int avgval = summedvals / samplesize;
      Serial.print("Averaged photoresistor value is "); Serial.println(avgval);
      Serial.println("Starting car !");
      unsigned long prevtime = millis();
      unsigned long startime = prevtime;
      digitalWrite(ballvalve, LOW);
      digitalWrite(solenoid, HIGH);
      Serial.println("Set sol high");
      int countofmissed = 0;
      while (digitalRead(swtch) == LOW) {
          digitalWrite(photoout, HIGH);

          unsigned long curtime = millis();
          if (curtime - prevtime >= interval) {
            prevtime = curtime;
            int newphoval = analogRead(photoin); 
            Serial.print(curtime - startime); Serial.print(", "); Serial.println(newphoval);
            if (newphoval <= multiplicativethreshold * avgval) {
              countofmissed++;
              if (countofmissed >= thresholdtostop) {
                Serial.println("STOPPING CAR");
                digitalWrite(ballvalve, LOW);
                digitalWrite(solenoid, LOW);
                Serial.println("Set sol low");
                break;
              }
            } else {
              countofmissed = 0;
            }
          }
      
      }
      if (digitalRead(swtch) == HIGH) {
        digitalWrite(ballvalve, LOW);
        digitalWrite(solenoid, LOW);
        Serial.println("Set sol low");

        Serial.println("STOPPED CAR DUE TO SWITCH BEING TURNED OFF");
      }
    }
  }
}
