int timer = 60;
int segments[4];
int max_segments = sizeof(segments)/sizeof(int);

int current_segment = 0;
int multiplex_rate = 100;
int multiplex_delay = 1000 / multiplex_rate;
bool ticking = true;

void setup() {
  Serial.begin(9600);
  
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


  update();
  
  // multiplex loop
  
  multiplex();
  
  // subtract second and finish
  if (ticking) {
    timer -= 1;
  }
}

void update() {
  int minutes = timer / 60;
  int seconds = timer % 60;
  
  segments[0] = minutes / 10;
  segments[1] = minutes % 10;
  segments[2] = seconds / 10;
  segments[3] = seconds % 10;
  
  Serial.print("timer:");
  Serial.println(timer);
}

void show_segment(int segment) {
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


void show_number(int number) {
  Serial.print("Showing: ");
  Serial.println(number);
  
  for(int i = 4; i <= 10; i++) {
    digitalWrite(i, LOW);
  }
  
  switch(number) {
    case 0:
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);
      break;
    case 1:
      digitalWrite(5, HIGH);
      break;
    case 2:
      digitalWrite(6, HIGH);
      break;
    case 3:
      digitalWrite(7, HIGH);
      break;
    case 4:
      digitalWrite(8, HIGH);
      break;
    case 5:
      digitalWrite(9, HIGH);
      break;
    case 6:
      digitalWrite(10, HIGH);
      break;
    case 7:
      digitalWrite(5, HIGH);
      break;
    case 8:
      digitalWrite(6, HIGH);
      break;
    case 9:
      digitalWrite(7, HIGH);
      break;
  }
}


void activate_segment(int segment) {
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  
  Serial.print("activating segment");
  Serial.println(segment);
  digitalWrite(segment, HIGH);
  /**
  switch(segment) {
    case 0:
      digitalWrite(segment, HIGH);
      break;
    case 1:
      digitalWrite(0, HIGH);
      break;
    case 2:
    digitalWrite(0, HIGH);
      break;
    case 3:
      break;
  } **/
}
    
