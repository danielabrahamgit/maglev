#define LED 13
#define G_MINUS 2
#define G_PLUS 7
#define EN 11
#define HALL_DATA A3
#define SET_PIN A6
#define GND A4
#define MAX_PUSH 255
#define MAX_PULL 255

int set_point = 600;
double Kp = 100, Ki = 20, Kd = 100;
double Ts = 1;

int uk = 0, uk1 = 0;
int ek, ek1, ek2;

float a =  Kp + (Ki * Ts / 2.0) + (Kd / Ts);
float b = -Kp + (Ki * Ts / 2.0) - (2.0 * Kd / Ts);
float c =  Kd / Ts;

boolean debug = false ;

LowPass outputFilter(1);
LowPass inputFilter(1);

void setup()
{
  pinMode(G_MINUS, OUTPUT);
  pinMode(G_PLUS,  OUTPUT);
  pinMode(GND, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(EN,  OUTPUT);

  digitalWrite(GND, LOW);

  TCCR2A =  _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS20);

  if (debug) {
    Serial.begin(2000000);
  }
}

void magnetDirection(int controlVal) {
  if (controlVal >= 0) {
    PORTD |= (1 << 7);
    PORTD &= ~(1 << 2);
  } else {
    PORTD |= (1 << 2);
    PORTD &= ~(1 << 7);
  }
}

void loop()
{
  set_point = analogRead(SET_PIN);
  ek = inputFilter.getNext(analogRead(HALL_DATA)) - set_point;
  uk = uk1 + (a * ek) + (b * ek1) + (c * ek2);
  ek2 = ek1;
  ek1 = ek;
  uk1 = uk;

  if (uk < -MAX_PUSH) {
    uk = -MAX_PUSH;
  } else if (uk > MAX_PULL) {
    uk = MAX_PULL;
  }
  uk = outputFilter.getNext(uk);
  magnetDirection(uk);
  OCR2A = abs(uk);

  if (debug) {
    Serial.print(set_point);
    Serial.print("\t");
    Serial.print(ek);
    Serial.print("\t");
    Serial.println(uk);
  }
}
