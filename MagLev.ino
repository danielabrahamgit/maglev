#define HALL_GND A7
#define HALL_DATA A5
#define G_MINUS 2
#define G_PLUS 7
#define EN 11
#define PUSH_MAX 140
#define PULL_MAX 250


const int setVal = 467;
const float kp = 40;
const float ki = 0.00006;
const int kd =  40;
const int averageSize = 15;

int dataPoints = 0;
int error = 0;
long controlVal = 0;
long sum = 0;
int prevError = 0;

long movingAverage[averageSize];

void setup() {
  pinMode(HALL_GND, OUTPUT);
  pinMode(G_MINUS, OUTPUT);
  pinMode(G_PLUS,  OUTPUT);
  pinMode(EN, OUTPUT);
  digitalWrite(HALL_GND, LOW);

  for (int i = 0; i < averageSize; i++) {
    movingAverage[i] = 0;
  }

  TCCR2A =  _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B =  _BV(CS20);

  Serial.begin(2000000);
}

int getAverage(int x) {
  int total = x;
  for (int i = averageSize - 1; i > 0; i--) {
    movingAverage[i] = movingAverage[i - 1];
    total += movingAverage[i - 1];
  }
  movingAverage[0] = x;
  return total / averageSize;
}

void magnetDirection(int controlVal) {
  if (controlVal > 0) {
    PORTD |= (1 << 7);
    PORTD &= ~(1 << 2);
  } else {
    PORTD |= (1 << 2);
    PORTD &= ~(1 << 7);
  }
}

void loop() {
  //Get Error
  error = analogRead(HALL_DATA) - setVal;

  error = getAverage(error);

  //Check if the device is being used
  if (dataPoints < 2000) {
    //Calculate control PID val
    controlVal = -((kp * error) + (ki * sum) + (kd * (error - prevError))); 
    //Udate PID Stuff
    sum += error;
    prevError = error;
        
    //Anti-windup
    if (controlVal < -PUSH_MAX) {
      controlVal = -PUSH_MAX;
    } else if (controlVal > PULL_MAX) {
      controlVal = PULL_MAX;
    }
    Serial.print(error);
    Serial.print("\t");
    Serial.println(controlVal);
        
    //Flips magnet if negative
    magnetDirection(controlVal);
    //PWM Signal 
    OCR2A = abs(controlVal);
    //dataPoints++;
  } else {
    OCR2A = 0;
  }
  
}
