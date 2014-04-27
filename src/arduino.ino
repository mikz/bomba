// (minutes * 60 + seconds) * 1000 = time in miliseconds
const unsigned long timer = (90L * 60L + 50L) * 1000L;

// controls wether to print deubbging info to serial port 
const bool print_pins = true;
const bool print_buttons = false;
const bool print_clock = true;

// configuration of output/input pins
const int segment_pins[6] = { 2, 3, 4, 5, 6 };
const int number_pins[7] = { 7, 8, 9, 10, 11, 12, 13 };
const int button_pins[5] = { A0, A1, A2, A3, A4 };

const int siren_pin = A5;

// multiplex rate controls how many times to rotate between segments before updating displayed number
const int multiplex_rate = 6;

// help vars below
bool ticking = true;
bool started = true;

const int buttons = sizeof(button_pins)/sizeof(int);
const int segments = sizeof(segment_pins)/sizeof(int);
const int numbers = sizeof(number_pins)/sizeof(int);

unsigned long int remaining = timer;

int current_segment = 0;

int display[6];
unsigned long boot;

const byte segment_numbers[10][7] = {
    { 1,1,1,1,1,1,0 },  // = 0
    { 0,1,1,0,0,0,0 },  // = 1
    { 1,1,0,1,1,0,1 },  // = 2
    { 1,1,1,1,0,0,1 },  // = 3
    { 0,1,1,0,0,1,1 },  // = 4
    { 1,0,1,1,0,1,1 },  // = 5
    { 1,0,1,1,1,1,1 },  // = 6
    { 1,1,1,0,0,0,0 },  // = 7
    { 1,1,1,1,1,1,1 },  // = 8
    { 1,1,1,0,0,1,1 }   // = 9
};

// enable Serial << streaming
template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } 

void setup() {
    Serial.begin(9600);
    
    boot = millis();
    
    for(int i=0; i < segments; i++) {
      pinMode(segment_pins[i], OUTPUT);
    }

    for(int i=0; i < numbers; i++) {
      pinMode(number_pins[i], OUTPUT);
    }
    
    for(int i=0; i < buttons; i++) {
      pinMode(button_pins[i], INPUT);
    }
};

void loop() {
  if(started)
    tick();
};

void tick() {
    if(remaining <= 0) {
        remaining = 0;
        update();
        ticking = false;
    }

    if (ticking) {
        update();
        multiplex();
    } else {
        multiplex();
        beep();
    }
};

int next_segment() {
    int next_segment = current_segment + 1;
    if (next_segment >= segments) {
        return 0;
    } else {
        return next_segment;
    }
};

void update_clock(int first, int second, int third) {
	if(print_clock) {
            Serial << first << ":" << second << ":" << third << "\n";
	}

    display[0] = first / 10;
    display[1] = first % 10;
    display[2] = second / 10;
    display[3] = second % 10;
    display[4] = third / 10;
    display[5] = third % 10;
};

unsigned long elapsed() {
  return millis() - boot;
};

void update() {
	unsigned long elps = elapsed();

    remaining = timer - elps;
    if(print_clock) {
	    Serial << "Timer: " << timer << " Remaining: " << remaining << " Elapsed: " << elps << "\n";
    }
    int miliseconds = remaining % 1000L;
    int seconds_left = remaining / 1000L;
    
    int hours  =  seconds_left / 3600L;
    int minutes = (seconds_left / 60L) % 60L;
    int seconds = seconds_left % 60L;
    
    if (print_clock) {
	    Serial << "Hour: " << hours << " Minutes: " << minutes << " Seconds: " << seconds << " Miliseconds: " << miliseconds << "\n";
    }
    if (hours > 0) { // show hours:minutes:seconds
      update_clock(hours, minutes, seconds);
    } else { // show minutes:seconds:miliseconds 
      update_clock(minutes, seconds, miliseconds / 100);
    }
};

void show_segment(int segment) {
    Serial << "Showing segment: " << segment << "\n";
  
    fade_segments();
    show_number(display[segment]);
    turn_on(segment);
};

int read_button(int button) {
    int pin = button_pins[button];
    int value = analogRead(pin);
    return value;
};

void multiplex() {
    for(int i=0; i < multiplex_rate; i++) {
        show_segment(current_segment);
        current_segment = next_segment();

        for(int j=0; j < buttons; j++) {
		int button_value = read_button(j);
		if (print_buttons) {
			Serial << "Button: " << j << " = " << button_value << "\n";
		}
        }
    }
};

void show_number(int number) {
    Serial << "Showing number: " << number << "\n";
    for(int i=0; i < 7; i++) {
      int pin = segment_pins[i];
      bool on = segment_numbers[number][i];
      on ? turn_on(pin) : turn_off(pin);
    }  
};

void fade_segments() {
    for(int i=0; i <  sizeof(segment_pins)/sizeof(int); i++) {
      turn_off(segment_pins[i]);
    }
};

void turn_on(int pin) {
  digitalWrite(pin, HIGH);
  if (print_pins) {
      Serial << "Turn ON pin: " << pin << "\n";
  }
};

void turn_off(int pin) {
  digitalWrite(pin, LOW);
  if (print_pins) {
      Serial << "Turn OFF pin: " << pin << "\n";
  }
};

void beep() {
};
