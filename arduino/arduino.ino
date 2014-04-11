int timer = 70;
int segments[4];
int max_segments = sizeof(segments)/sizeof(int);

int current_segment = 0;
int multiplex_rate = 400;
int multiplex_delay = 1000 / multiplex_rate;
bool ticking = true;

void setup() {
  pinMode(0, OUTPUT);   
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

int next_segment() {
  int next_segment = current_segment + 1;
  if (next_segment >= max_segments) {
    return 0;
  } else {
    return next_segment;
  }
}

void loop() {
  if(timer <= 0) {
    timer = 0;
    update();
    ticking = false;
  }
  
  // subtract second and finish
  if (ticking) {
    
    update();
  
  // multiplex loop
  
    multiplex();
  
    timer -= 1;
  } else {
    multiplex();
  }
}


void update() {
  int minutes = timer / 60;
  int seconds = timer % 60;
  
  segments[0] = minutes / 10;
  segments[1] = minutes % 10;
  segments[2] = seconds / 10;
  segments[3] = seconds % 10;
}

void show_segment(int segment) {
  fade_segments();
  show_number(segments[segment]);
  activate_segment(segment);
}


void multiplex() {
  for(int i=0; i < multiplex_rate; i++) {    
    show_segment(current_segment);
    current_segment = next_segment();
    delay(multiplex_delay);
  }
}

byte numbers[10][7] = {
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

void show_number(int number) {
  for(int i=0; i < 7; i++) {
    digitalWrite(i + 4, numbers[number][i]);
  }
 
}

void fade_segments() {
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
}

void activate_segment(int segment) {
  digitalWrite(segment, HIGH);

}
    
