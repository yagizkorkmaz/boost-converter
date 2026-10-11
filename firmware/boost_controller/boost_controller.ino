// Boost converter controller - ATtiny85, 8 MHz internal oscillator
//
// Reads the output voltage through a resistor divider on A3 and steps the
// PWM duty cycle on PB1 up or down until the reading matches the set point.

#define F_CPU 8000000
int pwm = 1;                     // PWM output: PB1 (pin 6)
int feedbackinput = A3;          // Feedback input: A3 (PB3 / pin 2)
int feedbackinputval;
int pwmval;

// Divider ratio: 2.2k / (12k + 1k + 2.2k) = 0.1447
// ADC value for the target output voltage:
const int hedef_adc = 1020;      // set point, about 34.4 V with a 5 V reference

void setup() {
  // PWM frequency (Timer0 fast PWM, about 31 kHz)
  TCCR0A = 2 << COM0A0 | 2 << COM0B0 | 3 << WGM00;
  TCCR0B = 0 << WGM02 | 1 << CS00;  // No prescaler
  TCCR1 = 0 << PWM1A | 0 << COM1A0 | 1 << CS10;
  GTCCR = 1 << PWM1B | 2 << COM1B0;

  pinMode(pwm, OUTPUT);
  pinMode(feedbackinput, INPUT);
  digitalWrite(pwm, LOW);
  pwmval = 0;
}

void loop() {
  feedbackinputval = analogRead(feedbackinput);

  // Output too low: increase duty cycle
  while (feedbackinputval < hedef_adc) {
    if (pwmval < 255) {
      pwmval++;
      analogWrite(pwm, pwmval);
    }
    feedbackinputval = analogRead(feedbackinput);
  }

  // Output too high: decrease duty cycle
  while (feedbackinputval > hedef_adc) {
    if (pwmval > 0) {
      pwmval--;
      analogWrite(pwm, pwmval);
    }
    feedbackinputval = analogRead(feedbackinput);
  }
}
