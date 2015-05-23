/*
 *** USAGE
 *
 * This program provides a serial interface adapter to the Medoc TSA-II.
 *
 * When connected to a serial port, it will communicate at 9600 baud.
 * To instruct the TSA to start, send a single '*' character (with no line break).
 * To receive output from the TSA, listen for a '#' character.
 *
 * The status LED will blink:
 *  - 4x when the device first starts
 *  - 3x when the TSA was instructed to start
 *  - 2x when output was received from the TSA
 *  - 1x if an unknown command or character is received
 *
 *
 *** NOTES
 *
 * See the Medoc TSA-II FAQ (http://www.medoc-web.com/support/faq)
 *
 *  TTL input option:
 *    The TSA can be started by an external ±5V, 100-2000msec trigger.
 *    The trigger must be set to External in the CoVAS program algorithm.
 *
 *  TTL output option:
 *    TTL output is a ±5V, duration 50-3000msec pulse (set via Utility-»Setup).
 *    TTL output must be enabled in the CoVAS program algorithm.
 *
 * The '±' part is probably an error.
 *
 *
 *** COPYRIGHT
 *
 * Written by Devon Rueckner for Carissa Cascio
 * Vanderbilt University Medical Center
 * 2015
 *
 */


// I/O pins
#define TSA_OUTPUT_PIN  2  // to TSA-II TTL Out
#define TSA_INPUT_PIN   3  // to TSA-II TTL In
#define STATUS_PIN      4  // green LED indicator
#define ACTIVITY_PIN    5  // yellow LED indicator

// magic character for signaling
#define TRIGGER_CHAR    '#'
#define CONNECT_CHAR    '!'

// `setup` runs once when you press reset or power on.
void setup() {
    // initialize
    pinMode(TSA_OUTPUT_PIN, INPUT);
    pinMode(TSA_INPUT_PIN, OUTPUT);
    pinMode(STATUS_PIN, OUTPUT);
    pinMode(ACTIVITY_PIN, OUTPUT);
    Serial.begin(9600);

    // indicate a successful initialization
    digitalWrite(STATUS_PIN, HIGH);
}

// `receivedPulse` returns true only for a 50-3000ms pulse
bool receivedPulse() {
    unsigned long start = millis();

    do {
        // check if the pulse has ended
        if (!digitalRead(TSA_OUTPUT_PIN)) {
            // make sure the pulse was longer than 50ms
            return millis() - start > 50;
        }
    } while (millis() - start < 3000);

    // the pulse lasted longer than 3000ms
    // wait for the pulse to end and return.
    while(digitalRead(TSA_OUTPUT_PIN));
    return false;
}

// `sendTriggerPulse` sends a 200ms pulse to the TSA
void sendTriggerPulse() {
    digitalWrite(TSA_INPUT_PIN, HIGH);
    delay(200);
    digitalWrite(TSA_INPUT_PIN, LOW);
}

// `blink` flashes an LED n times
void blink(int pin, int nBlinks, bool leaveOn) {
    for (int i = 0; i < nBlinks; i++) {
        digitalWrite(pin, HIGH);
        delay(200);
        digitalWrite(pin, LOW);
        delay(200);
    }
    digitalWrite(pin, leaveOn);
}

// `loop` runs repeatedly
void loop() {

    // check if the TSA appears to be sending a signal
    if (digitalRead(TSA_OUTPUT_PIN)) {
        // wait for the pulse to end
        bool pulsed = receivedPulse();
        // if we got a valid pulse, send a signal back
        if (pulsed) {
            Serial.print(TRIGGER_CHAR);
            blink(ACTIVITY_PIN, 2, false);
        }
    }

    // check if we received a start command
    if (Serial.available() > 0) {
        // read the incoming byte:
        char cmd = Serial.read();
        // trigger command
        if (cmd == TRIGGER_CHAR) {
            sendTriggerPulse();
            blink(ACTIVITY_PIN, 1, false);
        }
        else if (cmd == CONNECT_CHAR) {
            blink(STATUS_PIN, 1, true);
        }
        else {
            // received an unknown command
            blink(STATUS_PIN, 2, true);
        }
    }
}
