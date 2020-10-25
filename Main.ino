/*
  This Program is meant to Demo
  Reading/Writing to the Serial Bus
  Default rate of 9600 BAUD is used.

*/

// Any Libraries Needed to test Stuff.
// GO HERE
#include <LiquidCrystal.h>

// Defines or Variable setting
// GO HERE
#define R 10;
#define G 11;
#define B 12;

//LCD Display Pinouts
#define RS 6;
#define EN 5;
#define D4 41;
#define D5 37;
#define D6 33;
#define D7 29;


int Alp, Brav, Char;
int Rin, Bin, Gin;

// Serial R/W Variables
char InArray[20];
int Index = 0;
int bytein = 0; // for incoming serial data

// Some Sample Color RGB Arrays
int _RED[] = { 100, 0, 0 };
int _GREEN[] = { 0, 100, 0 };
int _BLUE[] = { 0, 0, 100 };

int _PURPLE[] = { 255, 0, 255 };
int _YELLOW[] = { 255, 255, 0 };
int _CYAN[] = { 0, 255, 255 };
int _TEAL[] = { 0, 128, 128 };
int _ORANGE[] = { 255, 140, 0 };

LiquidCrystal LCD(6, 5, 41, 37, 33, 29);

void setup() {
    Serial.begin(9600);
    LCD.begin(16, 2);
    LCD.print("Starting up...");
    pinMode(R, OUTPUT);
    pinMode(G, OUTPUT);
    pinMode(B, OUTPUT);
    LED_COLOR(_RED);
    Alp = 2;
    Brav = 2;
    Char = 2;

    Rin = 0;
    Bin = 0;
    Gin = 0;

    delay(1000);
    LED_COLOR(_GREEN);
    LCD.setCursor(0, 0);
    LCD.print("LAST DATA REC:");
}

// Sets the RGB Led based on an INT array.
void LED_COLOR(int rgb[]) {
    analogWrite(R, rgb[0]);
    analogWrite(G, rgb[1]);
    analogWrite(B, rgb[2]);
}

void loop() {

    LED_COLOR(_BLUE);
    // Checks if Any Serial Input is availible
    if (Serial.available() > 0) {
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("LAST DATA REC:");
        // While there is byte-sized input. Store it in an Array
        while (Serial.available() > 0) {
            // read the incoming byte:
            bytein = Serial.read();
            // IF the byte is '10'/ENTER, Replace with space. We don't save them here
            if (bytein == 10) {
                bytein = ' ';
            }
            else {
                //Saving the BYTE in value to an Array in RAM.
                InArray[Index] = bytein;
                Index++;
            }
            // This is a Possible Array OOB Fix.
            if (Index > 19) {
                Index = 0;
            }
        }
        String data = InArray;

        // Clear the Char Array to avoid overwriting same message.
        for (Index != 0; --Index;) {
            InArray[Index] = '\0';
        }
        // say what you got:
        Serial.print("I received: ");
        Serial.println(data);
        LCD.setCursor(0, 1);
        LCD.print(data);
        delay(500);
    }
    else {
        delay(1000);
        LED_COLOR(_GREEN);
    }
}