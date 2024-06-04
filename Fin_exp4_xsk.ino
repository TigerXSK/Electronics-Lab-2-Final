int pinA = 13, pinB = 12, pinC = 11, pinD = 10, pinE = 9, pinF = 8, pinG = 7, pinP = 6;
int Com_left = 5, Com_right = 4;

// Distance
int Echo = 2, Trig = 3;
int distance;

// Temperature
float temp;

// Button
int Btn = 15;

// Debounce
int buttonState;
int lastButtonState = HIGH; // 初始化為 HIGH
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50000; // 50ms

// Measure distance or temperature
int Mode = 0; //0: distance, 1: temperature

// Display
String current_display_str;
int Pointer = 0;
unsigned long lastDisplayUpdateTime = 0;
const unsigned long displayUpdateInterval = 400000; // 400ms

void showSevenSeg(byte A, byte B, byte C, byte D, byte E, byte F, byte G, byte P);
void ShowNum(char chr);
void display(String str);

void setup() {
  pinMode(pinA, OUTPUT); pinMode(pinB, OUTPUT); pinMode(pinC, OUTPUT); pinMode(pinD, OUTPUT);
  pinMode(pinE, OUTPUT); pinMode(pinF, OUTPUT); pinMode(pinG, OUTPUT); pinMode(pinP, OUTPUT); 
  pinMode(Com_left, OUTPUT); pinMode(Com_right, OUTPUT);

  // Distance
  pinMode(Trig, OUTPUT); pinMode(Echo, INPUT);
  pinMode(Btn, INPUT);

  Serial.begin(9600);
}

void loop() {
  // Debounce
  int reading = digitalRead(Btn);
  if (reading != lastButtonState) {
    lastDebounceTime = micros();
  }
  if ((micros() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        Mode = 1 - Mode; // 切換模式 (0->1 或 1->0)
        Pointer = 0;
      }
    }
  }
  lastButtonState = reading;

  // Measure
  String display_str;
  if(Mode == 0) {
    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig, LOW);
    distance = (pulseIn(Echo, HIGH) / 29.41) / 2; //聲速:340 m/s, 34000 cm/s,  29.41 ?s/cm
    display_str = String(distance) + "Cm ";
  } 
  else if(Mode == 1) {
    // Temperature
    temp = analogRead(14); // A0 腳位 14 (插S)
    temp = (temp / 1023) * 5.0 * 100; //先將數位值轉換回類比(電壓)，再將電壓轉換為攝氏度 比例： 0.01 (V/?C)
    display_str = String(temp, 1) + "oC ";
  }

  // Display
  if (micros() - lastDisplayUpdateTime >= displayUpdateInterval) {
    current_display_str = display_str;
    lastDisplayUpdateTime = micros();
    Pointer ++;
    if(Pointer > (current_display_str.length() - 1)) Pointer = 0;

    Serial.print(current_display_str);
    Serial.print(", ");
    Serial.print(Pointer);
    Serial.print("\n");
  }
  display(current_display_str);

  // Serial.print(current_display_str);
  // Serial.print(", ");
  // Serial.print(display_str);
  // Serial.print(", ");
  // Serial.print(Mode);
  // Serial.print("\n");
}

void display(String str)
{
  int index_l = Pointer;
  int index_r = Pointer + 1;
  if(index_r > (str.length() - 1)) index_r = 0;
  
  char chr_l = str[index_l];
  char chr_r = str[index_r];

  // char chr_l = str[5];
  // char chr_r = str[6];

  // 左
  digitalWrite(Com_left, LOW);
  digitalWrite(Com_right, HIGH);
  ShowNum(chr_l);
  delayMicroseconds(5000); // 5ms per character

  // 右
  digitalWrite(Com_left, HIGH);
  digitalWrite(Com_right, LOW);
  ShowNum(chr_r);
  delayMicroseconds(5000); // 5ms per character
}

void showSevenSeg(byte A, byte B, byte C, byte D, byte E, byte F, byte G, byte P) 
{
  digitalWrite(pinA, A); digitalWrite(pinB, B); digitalWrite(pinC, C);digitalWrite(pinD, D); 
  digitalWrite(pinE, E); digitalWrite(pinF, F); digitalWrite(pinG, G); digitalWrite(pinP, P);
}

void ShowNum(char chr) 
{
  switch (chr) {   // low 的時候才會亮
    case '0':
      showSevenSeg(0, 0, 0, 0, 0, 0, 1, 1); // 0
      break;
    case '1':
      showSevenSeg(1, 0, 0, 1, 1, 1, 1, 1); // 1
      break;
    case '2':
      showSevenSeg(0, 0, 1, 0, 0, 1, 0, 1); // 2
      break;
    case '3':
      showSevenSeg(0, 0, 0, 0, 1, 1, 0, 1); // 3
      break;
    case '4':
      showSevenSeg(1, 0, 0, 1, 1, 0, 0, 1); // 4
      break;
    case '5':
      showSevenSeg(0, 1, 0, 0, 1, 0, 0, 1); // 5
      break;
    case '6':
      showSevenSeg(0, 1, 0, 0, 0, 0, 0, 1); // 6
      break;
    case '7':
      showSevenSeg(0, 0, 0, 1, 1, 0, 1, 1); // 7
      break;
    case '8':
      showSevenSeg(0, 0, 0, 0, 0, 0, 0, 1); // 8
      break;
    case '9':
      showSevenSeg(0, 0, 0, 0, 1, 0, 0, 1); // 9
      break;
    case 'C':
      showSevenSeg(0, 1, 1, 0, 0, 0, 1, 1); // C
      break;
    case 'm':
      showSevenSeg(1, 1, 0, 1, 0, 1, 0, 1); // m
      break;
    case ' ':
      showSevenSeg(1, 1, 1, 1, 1, 1, 1, 1); // space
      break;
    case '.':
      showSevenSeg(1, 1, 1, 1, 1, 1, 1, 0); // .
      break;
    case 'o':
      showSevenSeg(0, 0, 1, 1, 1, 0, 0, 1); // ?
      break;
    default:
      showSevenSeg(1, 0, 0, 1, 0, 0, 0, 1); // x
      break;
  }
}
