#define DAC_PIN A0

const int aux0Pin = 11;
const int aux1Pin = 3;
const int aux2Pin = 9;
const int txPin = 10;
const int currPhasePin = 7;
const int start_stop_Pin = 4;
const int to_fro_Pin = 13;

// Interface Vars
int aux[3] = {0,0,0};
int tx = 1;
int currPhase = 0;
int start_stop = 0;
int to_fro = 0;

// --- WAVEFORM TABLES ---
const int tableSize = 16;
const int tableSize2 = 64; 
float sineTable[tableSize];
float sineTablePI[tableSize];
float output[tableSize2];
float output2[tableSize2];

float inputCheckDelta = 8.333 * pow(10,3) / PI;
int antenna_num;
float amplitudeMod[8] = {1, 0.5, 0.4, 0.3, 10, 0, 0, 0};
int prevPhase = 0;
bool current_shift = false;


int mode = 0;
float amplitude;
float phaseShift;
float period1;
float period2;
unsigned long now;
unsigned long lastInput;
int tableIndex = 0;
unsigned long lastUpdate = 0;
bool stop = false;
bool flag1 = true;
bool flag2 = false;
bool flag3 = false;
int count = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");

  pinMode(aux0Pin, INPUT);
  pinMode(aux1Pin, INPUT);
  pinMode(aux2Pin, INPUT);
  pinMode(txPin, INPUT);
  pinMode(currPhasePin, INPUT);
  pinMode(start_stop_Pin, INPUT);
  pinMode(to_fro_Pin, INPUT);

  analogWriteResolution(10);

  amplitude = 51.0;
  phaseShift = 0;
  CarrierSignal(sineTable, tableSize, amplitude, phaseShift);
  phaseShift = PI;
  CarrierSignal(sineTablePI, tableSize, amplitude, phaseShift);
  ScanningBeam(output, tableSize2, true);   // TO scan
  ScanningBeam(output2, tableSize2, false); // FRO scan
}

void loop() {
  now = micros();
  period1 = (64) / tableSize;
  period2 = (6 * 1000) / tableSize2;

  if((int)(now - lastInput) >= 40){
    prevPhase = currPhase;
    lastInput = now;
    currPhase = digitalRead(currPhasePin);
    if(currPhase != prevPhase) {
      current_shift = !current_shift;
    }
    antenna_num = convert_to_int(aux);
    tx = digitalRead(txPin);
    start_stop = digitalRead(start_stop_Pin);
    to_fro = digitalRead(to_fro_Pin);
    aux[0] = digitalRead(aux0Pin);
    aux[1] = digitalRead(aux1Pin);
    aux[2] = digitalRead(aux2Pin);

  }

  if(!tx) {
    analogWrite(DAC_PIN, 511);
  }

  if ((now - lastUpdate >= period1) && (!start_stop) && (!to_fro) && (tx)) {
    lastUpdate = now;
    int value;
    if (!currPhase) {
      value = (int)(amplitudeMod[antenna_num]*sineTable[tableIndex]);
      // amplitudeMod[antenna_num]
    } else {
      value = (int)(amplitudeMod[antenna_num]*sineTablePI[tableIndex]);
    }
    analogWrite(DAC_PIN, value);
    tableIndex = (tableIndex + 1);
    if (tableIndex >= tableSize) {
      tableIndex = 0;
    }
  }
  if ((now - lastUpdate >= period2) && (start_stop) && (tx)) {
    lastUpdate = now;
    int value;
    if (to_fro) {
      value = (int)(output[tableIndex]);
    } else {
      value = (int)(output2[tableIndex]);
    }
    analogWrite(DAC_PIN, value);
    tableIndex = (tableIndex + 1);
    if (tableIndex >= tableSize2) {
      tableIndex = 0;
    }
  }
}

void CarrierSignal(float* table, int size, float amp, float phaseShift) {
  for (int i = 0; i < size; i++) {
    table[i] = (amp * (sin(2 * PI * i / size + phaseShift) + 1)) + 460;  
  }
}

float sinc(float x) {
  if (x == 0.0) return 1.0;
  return sin(PI * x) / (PI * x);  
}

void ScanningBeam(float* output, int size, float toFro) {
  float thetaMin = -62.0;
  float thetaMax = 62.0;
  float thetaR = -10;
  float bandWidth = 2;
  float t_us, t_s, theta, envelope, carrier, modulated, combined;
    switch (mode) {
    case 0:  // Azimuth
      thetaMin = -62.0;
      thetaMax = 62.0;
      thetaR = 12;
      bandWidth = 2;
      break;
    case 1:  // Elevation
      thetaMin = -1.5;
      thetaMax = 29.5;
      thetaR= 3;
      bandWidth = 1.5;
      break;
    case 2:  // BAZ
      thetaMin = -42.0;
      thetaMax = 42.0;
      thetaR = 12;
      bandWidth = 2;
      break;
  }
  for (int i = 0; i < size; i++) {
    t_us = (float)i * 500 / size;
    t_s = t_us * 1e-6;
    if (toFro) {
      theta = thetaMin + ((thetaMax - thetaMin) * i) / (size - 1);
      envelope = sinc((thetaR - theta) / bandWidth);
    } else {
      theta = thetaMax - ((thetaMax - thetaMin) * i) / (size - 1);
      envelope = sinc(((thetaR) - theta) / bandWidth);
    }
    carrier = sin(2.0 * PI * 75000 * t_s);
    modulated = envelope * carrier;
    combined = 2*(2*modulated + 10*envelope + (-5*envelope)); // All in one
    output[i] = constrain((uint16_t)(511 + combined * 511), 0, 1023);
  }
}

int convert_to_int(int* array) {
  int intValue = 0;
  for (int i = 0; i < 3; i++) {
    intValue += array[i] * pow(2, 2 - i);
  }
  return intValue;
}
