#include <Wire.h>
#include <Adafruit_ADS7830.h>

Adafruit_ADS7830 ad7830A;
Adafruit_ADS7830 ad7830B;

const int aPin0 = 0;  // Row A
const int aPin1 = 1;  // Row B
const int aPin2 = 2;  // Row C
const int aPin3 = 3;  // Column 1
const int aPin4 = 4;  // Column 2
const int aPin5 = 5;  // Column 3
const int aPin6 = 6;  // Column 4
const int aPin7 = 7;  // Column 5
const int bPin0 = 0;  // Column 6

uint8_t sensorValueA = 0;  // to store the value read from the 7830
uint8_t sensorValueB = 0;
uint8_t sensorValueC = 0;
uint8_t sensorValue1 = 0;
uint8_t sensorValue2 = 0;
uint8_t sensorValue3 = 0;
uint8_t sensorValue4 = 0;
uint8_t sensorValue5 = 0;
uint8_t sensorValue6 = 0;

uint8_t drawers[3] = {sensorValueA, sensorValueB, sensorValueC};
uint8_t columns[6] = {columns[0], columns[1], columns[2], columns[3], columns[4], columns[5]};

int columnCounter = 0;
int whichColumn = -1;
int prevColumn = -1;
int drawerCounter = 0;
int whichDrawer = -1;


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
  while (!Serial) delay(10); // Wait for console to open
  
  Serial.println("Adafruit ADS7830 initializing - by Limor Fried/Ladyada");

  // Possible arguments to begin():
  // begin(); // Uses default I2C address 0x48 and Wire
  // begin(0x49); // Uses I2C address 0x49 and Wire
  // begin(0x48, &Wire1); // Uses I2C address 0x48 and Wire1

  if (!ad7830A.begin(0x48)) {    // SET I2C ADDRESS HERE
    Serial.println("Failed to initialize ADS7830-A!");
    while (1);
  }
  if (!ad7830B.begin(0x49)) {    // SET I2C ADDRESS HERE
    Serial.println("Failed to initialize ADS7830-B!");
    while (1);
  }
  Serial.println("start Vendy Friendy...");
}

void loop() {
  // read the analog in value:
  drawers[0] = ad7830A.readADCsingle(aPin0);  // 400+ for vending drawer motor
  drawers[1] = ad7830A.readADCsingle(aPin1);  // 400+ for vending drawer motor
  drawers[2] = ad7830A.readADCsingle(aPin2);  // 400+ for vending drawer motor
  columns[0] = ad7830A.readADCsingle(aPin3);  // 400+ for vending drawer motor
  columns[1] = ad7830A.readADCsingle(aPin4);  // 400+ for vending drawer motor
  columns[2] = ad7830A.readADCsingle(aPin5);  // 400+ for vending drawer motor
  columns[3] = ad7830A.readADCsingle(aPin6);  // 400+ for vending drawer motor
  columns[4] = ad7830A.readADCsingle(aPin7);  // 400+ for vending drawer motor
  columns[5] = ad7830B.readADCsingle(bPin0);  // 400+ for vending drawer motor

  whichColumn = columnTest();
  if (whichColumn != -1) {
    Serial.print("Maybe ");
    Serial.println(whichColumn);
    if (SameOrNewColumn()) {
      if (columnCounter > 3) {
        Serial.println("Test for which drawer");
      }
    }
  }

  // print the results to the Serial Monitor:
  Serial.print("sensors = ");
  //Serial.print((sensorValueA - columns[0] > 130 && sensorValueA - columns[0] < 144) ? "1-GO\t" : "STOP\t");
  //Serial.print((sensorValueA - columns[1] > 130 && sensorValueA - columns[1] < 144) ? "2-GO\t" : "STOP\t");
  //Serial.print((sensorValueA - columns[2] > 130 && sensorValueA - columns[2] < 144) ? "3-GO\t" : "STOP\t");
  Serial.print("A ");
  Serial.print(drawers[0]);
  Serial.print("\tB ");
  Serial.print(drawers[1]);
  Serial.print("\tC ");
  Serial.print(drawers[2]);
  Serial.print("\t1 ");
  Serial.print(columns[0]);
  Serial.print("\t2 ");
  Serial.print(columns[1]);
  Serial.print("\t3 ");
  Serial.print(columns[2]);
  Serial.print("\t4 ");
  Serial.print(columns[3]);
  Serial.print("\t5 ");
  Serial.print(columns[4]);
  Serial.print("\t6 ");
  Serial.print(columns[5]);
  Serial.print("\tCC ");
  Serial.println(columnCounter);

  //Serial.print("\t output = ");
  //Serial.println(outputValue);

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(400);
}

int columnTest() {
  for (int i = 0; i < 6; i++) {
    if (columns[i] > 0 && columns[i] < 10) {  // define range of voltage on column while vending
      return i;                               // return the active column (if any)
    }
  }
  columnCounter = 0;                          // otherwise reset the counters
  prevColumn = -1;
  return -1;
}

bool SameOrNewColumn() {
  if (whichColumn == prevColumn) {  // If a column has been in the range for a subsequent time
    columnCounter++;
    return true;
  }
  else if (prevColumn == -1) {      // If a column enters the range for the first time
    prevColumn = whichColumn;
    return true;
  }
  else {                            // If a column is in range but different from previous column
    prevColumn = whichColumn;
    whichColumn = -1;
    columnCounter = 0;
    return false;
  }
}

int drawerTest() {
  return -1;
}