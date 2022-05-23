//int inPin = 7;  // the pin number for input (for me a push button)
int ledPin = 2; 

int current;         // Current state of the button
                     // (LOW is pressed b/c i'm using the pullup resistors)
long millis_held;    // How long the button was held (milliseconds)
long secs_held;      // How long the button was held (seconds)
long prev_secs_held; // How long the button was held in the previous check
byte previous = HIGH;
unsigned long firstTime; // how long since the button was first pressed 


void setup() {
  Serial.begin(115200);         // Use serial for debugging
  pinMode(ledPin, OUTPUT);
  //digitalWrite(inPin, HIGH);  // Turn on 20k pullup resistors to simplify switch input
}

void loop() {
  if(touchRead(T0) < 30){
    current = HIGH; }
    else {current = LOW;}

  // if the button state changes to pressed, remember the start time 
  if (current == LOW && previous == HIGH && (millis() - firstTime) > 200) {
    firstTime = millis();
  }

  millis_held = (millis() - firstTime);
  secs_held = millis_held / 1000;

  // This if statement is a basic debouncing tool, the button must be pushed for at least
  // 100 milliseconds in a row for it to be considered as a push.
  if (millis_held > 50) {

    if (current == LOW && secs_held > prev_secs_held) {
      ledblink(1, 50, ledPin); // Each second the button is held blink the indicator led
    }

    // check if the button was released since we last checked
    if (current == HIGH && previous == LOW) {
      // HERE YOU WOULD ADD VARIOUS ACTIONS AND TIMES FOR YOUR OWN CODE
      // ===============================================================================

      
      
      // Button pressed for less than 1 second, one long LED blink
      if (secs_held <= 0) {
        ledblink(1,750,ledPin);
      }

      // If the button was held for 3-6 seconds blink LED 10 times
      if (secs_held >= 1 && secs_held < 3) {
        ledblink(10,200,ledPin); 
      }

      // Button held for 1-3 seconds, print out some info
      if (secs_held >= 3) {
        Serial.print("It Works!!! Seconds held: ");
        Serial.print(secs_held);
        Serial.print("   Milliseconds held: ");
        Serial.println(millis_held);
      }
      // ===============================================================================
    }
  }

  previous = current;
  prev_secs_held = secs_held;
}

// Just a simple helper function to blink an led in various patterns
void ledblink(int times, int lengthms, int pinnum){
  for (int x=0; x<times;x++) {
    digitalWrite(pinnum, HIGH);
    delay (lengthms);
    digitalWrite(pinnum, LOW);
    delay(lengthms);
  }
}
