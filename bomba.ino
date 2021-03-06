// (minutes * 60 + seconds) * 1000 = time in miliseconds
const unsigned long timer_minutes = 30L;
const unsigned long timer_seconds = 10L;
const unsigned long wrong_code_penalty_minutes = 20L;
const int valid_code[] = {1,2,3,4,5,6};

// blinking after bomb explodes
const int blink_pause = 300; // turned off delay
const int blink_multiplex_rate = 2000; // how many iterations before pause
const int beep_after_explode = 3;

// multiplex rate controls how many times to rotate between segments before updating displayed number
const int multiplex_rate = 2000;
const int button_read_freq = 5;

// controls wether to print debugging info to serial port
const bool print_pins = false;
const bool print_buttons = false;
const bool print_raw_buttons = false;
const bool print_clock = false;
const bool print_segment = false;
const bool print_blink = false;
const bool print_number = false;
const bool print_code = false;

// configuration of output/input pins
const int segment_pins[] = { 2, 3, 4, 5, 6, 1 };
const int number_pins[] = { 7, 8, 9, 10, 11, 12, 13 };
const int button_pins[] = { A0, A1, A2, A3, A4, A5 };

const int siren_pin = 0;

// help vars below
bool ticking = false;
bool armed = true;
bool started = false;

const bool serial = print_pins || print_buttons || print_clock || print_segment || print_blink || print_number || print_raw_buttons || print_code;

const unsigned long timer = (timer_minutes * 60L + timer_seconds) * 1000L;

const int buttons = sizeof(button_pins)/sizeof(int);
const int segments = sizeof(segment_pins)/sizeof(int);
const int numbers = sizeof(number_pins)/sizeof(int);
const int code_size = sizeof(valid_code)/sizeof(int);

int last_button_values[buttons] = {};
int code_position = 0;
int explode_beeps = 0;

unsigned long penalty = 0;

unsigned long remaining = timer;

int current_segment = 0;
int current_button = 0;

int display[6];
unsigned long boot;

const byte segment_numbers[][7] =
{
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

// ordered like button_pins
// HIGH, LOW
const int button_mapping[][2] =
{
    { 2, 1},
    { 3, 4},
    { 5, 6},
    { 8, 7},
    { 0, 9},
    { 10, 11}
};

// enable Serial << streaming
template<class T> inline Print &operator <<(Print &obj, T arg)
{
    obj.print(arg);
    return obj;
}

void setup()
{
    if (serial)
    {
        Serial.begin(9600);
    }

    pinMode(siren_pin, OUTPUT);
    digitalWrite(siren_pin, LOW);

    for(int i=0; i < segments; i++)
    {
        pinMode(segment_pins[i], OUTPUT);
    }

    for(int i=0; i < numbers; i++)
    {
        pinMode(number_pins[i], OUTPUT);
    }

    for(int i=0; i < buttons; i++)
    {
        pinMode(button_pins[i], INPUT);
    }
};

void loop()
{
    if(started)
    {
        tick();
    }
    else
    {
        wait_for_start();
    }
};

void wait_for_start()
{
    int button;

    int hours, minutes, seconds, miliseconds;
    extract_clock(timer, hours, minutes, seconds, miliseconds);
    update_clock(hours, minutes, seconds);

    do
    {
        multiplex(1);
        button = read_next_button();
    }
    while (button == -1);

    boot = millis();

    started = true;
    ticking = true;
    armed = true;
};

void tick()
{
    if(remaining == 0)
    {
        ticking = false;
        update();
    }

    if (ticking)
    {
        update();
        multiplex(multiplex_rate);
    }
    else
    {
        multiplex(blink_multiplex_rate);

        if (armed && explode_beeps++ < beep_after_explode)
        {
            beepStart();
        }

        blink();

        if (armed)
        {
            beepEnd();
        }
    }
};

int next_segment()
{
    int next_segment = current_segment + 1;

    if (next_segment >= segments)
    {
        return 0;
    }
    else
    {
        return next_segment;
    }
};

void update_clock(int first, int second, int third)
{
    if(print_clock)
    {
        Serial << first << ":" << second << ":" << third << "\n";
    }

    display[0] = first / 10;
    display[1] = first % 10;
    display[2] = second / 10;
    display[3] = second % 10;
    display[4] = third / 10;
    display[5] = third % 10;
};

unsigned long elapsed()
{
    return millis() - boot + penalty;
};

void extract_clock(long time, int &hours, int &minutes, int &seconds, int &miliseconds)
{
    int seconds_left = time / 1000L;
    miliseconds = time % 1000L;

    hours  =  seconds_left / 3600L;
    minutes = (seconds_left / 60L) % 60L;
    seconds = seconds_left % 60L;

    if (print_clock)
    {
        Serial << "Hour: " << hours << " Minutes: " << minutes << " Seconds: " << seconds << " Miliseconds: " << miliseconds << "\n";
    }

};

void update()
{
    unsigned long elps = elapsed();

    if(timer > elps)
    {
        remaining = timer - elps;
    }
    else
    {
        remaining = 0;
    }

    if(print_clock)
    {
        Serial << "Timer: " << timer << " Remaining: " << remaining << " Elapsed: " << elps << "\n";
    }

    int hours, minutes, seconds, miliseconds;
    extract_clock(remaining, hours, minutes, seconds, miliseconds);

    if (hours > 0)   // show hours:minutes:seconds
    {
        update_clock(hours, minutes, seconds);
    }
    else     // show minutes:seconds:miliseconds
    {
        update_clock(minutes, seconds, miliseconds / 10);
    }
};

void show_segment(int segment)
{
    if(print_segment)
    {
        Serial << "Showing segment: " << segment << "\n";
    }

    fade_segments();
    show_number(display[segment]);
    turn_on(segment_pins[segment]);
};

int button_index(int value)
{

    if (value > 600 && value < 800)
    {
        return 1;
    }

    if (value > 900)
    {
        return 0;
    }
}
bool button_pressed(int value)
{
    return value > 100;
}

int read_next_button()
{
    int next_button = (current_button + 1) % buttons;
    current_button = next_button;
    int button_value = read_button(current_button);
    int index;
    int number = -1;

    if (print_raw_buttons)
    {
        Serial << "Button: " << current_button << " = " << button_value << "\n";
    }

    if (button_pressed(button_value))
    {
        number = button_mapping[current_button][button_index(button_value)];
    }

    if (last_button_values[current_button] == number)
    {
        return -1;
    }
    else
    {
        last_button_values[current_button] = number;
        return number;
    }
};

int read_button(int button)
{
    int pin = button_pins[button];
    int value = analogRead(pin);
    return value;
};

void wrong_code_penalty()
{
    penalty += wrong_code_penalty_minutes * 60L * 1000L;
}

void reset_code()
{
    code_position = 0;
    beep(200);
    delay(200);
    beep(200);
    return;
}

void try_code(int number)
{
    if (number == 11 || number == 10)
    {
        return reset_code();

    }

    bool valid;
    int expected = valid_code[code_position++];

    if (print_code)
    {
        Serial << "Expected code: " << expected << ", got: " << number << "\n";
    }

    valid = expected == number;


    if (!valid)
    {
        beep(1000);
        code_position = 0;
        wrong_code_penalty();
        return;
    }


    if (code_position == code_size)
    {
        ticking = false;
        armed = false;
    }

};

void multiplex(int rate)
{
    for(int i=0; i < rate; i++)
    {
        show_segment(current_segment);
        current_segment = next_segment();

        // +1 for not reading buttons on every first call
        if((i+1)%button_read_freq == 0 && ticking)
        {

            int number = read_next_button();

            if (number >= 0)
            {
                if (print_buttons)
                {
                    Serial << "Pressed number: " << number << "\n";
                }

                beep(10);

                try_code(number);
            }
        }

    }
};

void blink()
{
    fade_segments();
    delay(blink_pause);

    if (print_blink)
    {
        Serial << "BOOM! Blink.\n";
    }
};

void show_number(int number)
{
    if(print_number)
    {
        Serial << "Showing number: " << number << "\n";
    }

    for(int i=0; i < numbers; i++)
    {
        int pin = number_pins[i];
        bool on = segment_numbers[number][i];
        on ? turn_on(pin) : turn_off(pin);
    }
};

void fade_segments()
{
    for(int i=0; i <  sizeof(segment_pins)/sizeof(int); i++)
    {
        turn_off(segment_pins[i]);
    }
};

void turn_on(int pin)
{
    digitalWrite(pin, HIGH);

    if (print_pins)
    {
        Serial << "Turn ON pin: " << pin << "\n";
    }
};

void turn_off(int pin)
{
    digitalWrite(pin, LOW);

    if (print_pins)
    {
        Serial << "Turn OFF pin: " << pin << "\n";
    }
};

void beep(int pause)
{
    beepStart();
    fade_segments();
    delay(pause);
    beepEnd();

}

void beepStart()
{
    digitalWrite(siren_pin, HIGH);
}

void beepEnd()
{
    digitalWrite(siren_pin, LOW);
};
