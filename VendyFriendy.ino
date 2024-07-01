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

uint8_t drawers[3] = { sensorValueA, sensorValueB, sensorValueC };
uint8_t columns[6] = { columns[0], columns[1], columns[2], columns[3], columns[4], columns[5] };

int columnCounter = 0;
int whichColumn = -1;
int prevColumn = -1;
int drawerCounter = 0;
int whichDrawer = -1;

int VENDING_STATE = 0;

unsigned long vendingTime = 0;
unsigned long currentMillis = 0;
const long interval = 2600;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
  Wire.begin();
  while (!Serial) delay(10);  // Wait for console to open

  Serial.println("Adafruit ADS7830 initializing - by Limor Fried/Ladyada");

  // Possible arguments to begin():
  // begin(); // Uses default I2C address 0x48 and Wire
  // begin(0x49); // Uses I2C address 0x49 and Wire
  // begin(0x48, &Wire1); // Uses I2C address 0x48 and Wire1

  if (!ad7830A.begin(0x48)) {  // SET I2C ADDRESS HERE
    Serial.println("Failed to initialize ADS7830-A!");
    while (1)
      ;
  }
  if (!ad7830B.begin(0x49)) {  // SET I2C ADDRESS HERE
    Serial.println("Failed to initialize ADS7830-B!");
    while (1)
      ;
  }
  Serial.println("start Vendy Friendy...");
}

void loop() {
  switch (VENDING_STATE) {
    case 0:
      // read the analog value for the columns to see if anything is vending
      columns[0] = ad7830A.readADCsingle(aPin3);
      columns[1] = ad7830A.readADCsingle(aPin4);
      columns[2] = ad7830A.readADCsingle(aPin5);
      columns[3] = ad7830A.readADCsingle(aPin6);
      columns[4] = ad7830A.readADCsingle(aPin7);
      columns[5] = ad7830B.readADCsingle(bPin0);

      // IMPORTANT VENDING DETECTION CODE HERE
      whichColumn = columnTest();
      if (whichColumn != -1) {
        //Serial.print("Maybe column ");
        //Serial.println(whichColumn+1);
        if (SameOrNewColumn()) {
          if (columnCounter > 2) {
            //Serial.print(", testing for drawer, result: ");
            whichDrawer = drawerTest();
            if (whichDrawer == -1) {
              //Serial.println("No drawer");
            } else {
              //Serial.println(whichDrawer);
              confirmVending(whichColumn, whichDrawer);
            }
          } else {
            //Serial.println(" maybe.");
          }
        } else {
          //Serial.println(" nope.");
        }
      }

      delay(50);
      break;

    // If Vending is actively happening, check the time
    case 1:
      currentMillis = millis();
      if (currentMillis - vendingTime >= interval) {
        Serial.println("Vending time over!");
        VENDING_STATE = 0;
        Wire.beginTransmission(8);
        Wire.write('d');
        Wire.endTransmission();
      } else {
        delay(50);
      }
      break;
    default:
      VENDING_STATE = 0;
      break;
  }
      /*
      // print the results to the Serial Monitor (uncomment this block for debugging)
      drawers[0] = ad7830A.readADCsingle(aPin0);
      drawers[1] = ad7830A.readADCsingle(aPin1);
      drawers[2] = ad7830A.readADCsingle(aPin2);
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
    //*/
}

int columnTest() {
  for (int i = 0; i < 6; i++) {
    if (columns[i] > 3 && columns[i] < 10) {  // define range of voltage on column while vending
      return i;                               // return the active column (if any)
    }
  }
  columnCounter = 0;  // otherwise reset the counters
  prevColumn = -1;
  return -1;
}

bool SameOrNewColumn() {
  if (whichColumn == prevColumn) {  // If a column has been in the range for a subsequent time
    columnCounter++;
    return true;
  } else if (prevColumn == -1) {  // If a column enters the range for the first time
    prevColumn = whichColumn;
    return true;
  } else {  // If a column is in range but different from previous column
    prevColumn = whichColumn;
    whichColumn = -1;
    columnCounter = 0;
    return false;
  }
}

int drawerTest() {
  drawers[0] = ad7830A.readADCsingle(aPin0);
  drawers[1] = ad7830A.readADCsingle(aPin1);
  drawers[2] = ad7830A.readADCsingle(aPin2);
  for (int i = 0; i < 3; i++) {
    if (drawers[i] > 130) {
      return i;
    }
  }
  return -1;
}

void confirmVending(int c, int d) {
  char drawers[] = {'a', 'b', 'c'};
  Serial.print("Vending! ");
  vendingTime = millis();
  VENDING_STATE = 1;
  Wire.beginTransmission(8);
  Wire.write(drawers[d]);
  Wire.endTransmission();
  switch (d) {
    case 0:
      Serial.print("A-");
      break;
    case 1:
      Serial.print("B-");
      break;
    case 2:
      Serial.print("C-");
      break;
    default:
      Serial.print("Bad drawer match - ");
      break;
  }
  Serial.println(c + 1);
}
