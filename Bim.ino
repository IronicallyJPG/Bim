/*

    PURPOSE         : A Game using Arduino(MEGA2560R3) /  1.8" 128x160 TFT_LCD_DISPLAY / Standard 5 Pin Analog Joystick
                     
    DATE STARTED    : 10-17-2020

    AUTHOR          : Bails

    SOURCE_VERSION  : BETA

*/
//===============================================================
// Any INCLUDES go here. ex: Libraries

// Faster TFT Library
#include <TFT_ST7735.h> // Graphics and font library for ST7735 driver chip <FROM BODMER-TFT Library!>
#include <SPI.h>        // SPI Hardware library.                            <FROM BODMER-TFT Library!>
#include <EEPROM.h>     // EEPROM Library for Reading/Writing PERSISTENT Data (Highscore's Mostly)

//===============================================================
// Any Hardware Pin DEFINES needed. ex: define red_led 5
// RGB LED
#define Rpin A0 
#define Gpin A2
#define Bpin A1

// ANALOG Joystick 
#define joyX A8  // Don't Swap these up.
#define joyY A9  // Control won't work right.
#define joyHat 7 // The HAT_SWCH can be used for anything. Currently just for a 'Debug Mode'

// TFT-LCD 
#define SCK 52
#define SDA 51
#define CS 39
#define DC 37
#define RESET 35 

// EEPROM Addresses needed. (in HEX)
#define scoreA 0x64
#define scoreB 0x78
#define scoreC 0x8C
#define BestScore A0

#define cheated 0xC8
#define infhealth 0xD2
#define inflives 0xDC

// Extra Defines for Clarity/use.
#define MENU_MODE 0
#define GAME_MODE 1
#define HIGHSCORE_MODE 2
#define CREDITS_MODE 3

#define MAX_GAME_TIME 300 // In Seconds
#define MAX_LIVES 3
#define BORDER_COLOR 0xFFFF


//===============================================================
// Any NON-HARDWARE DEFINES needed.   ex: define width 10
#define CursorposX 5
#define CursorposY 2

//===============================================================
// Hardware Variables HERE ex: LCD lcd = lcd();
TFT_ST7735 SCR = TFT_ST7735(); // The TFT SCREEN 'object' for us to call on to manipulate the 128x160 Screen we have.

// <RGB565> Bitmaps for use to Render. THIS GAME ONLY USES 10x10 SPRITES!

// Player BMP
const unsigned short Bim[] PROGMEM = {
    // 'Bim', 10x10 pixel data in RGB565
    0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 
    0xf9aa, 0x86bf, 0x005f, 0x005f, 0x86bf, 0x86bf, 0x005f, 0x005f, 0x86bf, 0xf9aa, 
    0xf9aa, 0x86bf, 0x86bf, 0x86bf, 0x86bf, 0x86bf, 0x86bf, 0x86bf, 0x86bf, 0xf9aa, 
    0xf9aa, 0x86bf, 0x005f, 0x86bf, 0x86bf, 0x86bf, 0x86bf, 0x005f, 0x86bf, 0xf9aa, 
    0xf9aa, 0x86bf, 0x005f, 0x005f, 0x005f, 0x005f, 0x005f, 0x005f, 0x86bf, 0xf9aa, 
    0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 0xf9aa, 
    0x0000, 0xf9aa, 0xf9aa, 0x0000, 0x0000, 0x0000, 0x0000, 0xf9aa, 0xf9aa, 0x0000, 
    0x0000, 0xf9aa, 0xf9aa, 0x0000, 0x0000, 0x0000, 0x0000, 0xf9aa, 0xf9aa, 0x0000,
    0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 
    0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff
};

// Apple 
const unsigned short Apple[] PROGMEM = {
    // 'apple', 10x10px  
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0420, 0x0420, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0xf945, 0x1be0, 0xf965, 0xfa69, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xfa08, 0xfaaa, 0xfa08, 0xfa49, 0xfa08, 0x0000, 0x0000, 0x0000,
    0x0000, 0xf28a, 0xf9c7, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0x0000, 0x0000,
    0x0000, 0x0000, 0xfa28, 0xf800, 0xf800, 0xf800, 0xf000, 0xd841, 0x0000, 0x0000,
    0x0000, 0x0000, 0xf800, 0xf800, 0xb0c3, 0xd861, 0xa8e3, 0xb8e3, 0x0000, 0x0000,
    0x0000, 0x0000, 0xd041, 0xb8a2, 0xf000, 0xc861, 0xd8e3, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

// TACO 
const unsigned short Taco[] PROGMEM = {
    // 'anger', 10x10px
    0x0000, 0x0000, 0x0000, 0xd6ab, 0xe688, 0xde87, 0xe669, 0xe688, 0xfff0, 0x0000,
    0x0000, 0x0000, 0xe6c8, 0x9d66, 0xd604, 0x9ce5, 0x7426, 0xc5ec, 0xbe2c, 0x0000,
    0xbde8, 0xa604, 0xb566, 0xd606, 0x9d04, 0xb528, 0xb449, 0xd52f, 0xddf2, 0xddd1, 
    0xdeeb, 0xada7, 0xb565, 0xa4e5, 0xc548, 0xaba9, 0xd5b3, 0xe6b8, 0xeed9, 0xde35, 
    0xa5c9, 0x7ce5, 0x7ca5, 0xad27, 0xb3c8, 0xd5b3, 0xde56, 0xde55, 0xde35, 0xde34, 
    0xad6c, 0x84a7, 0x5bc5, 0xb469, 0xbcae, 0xe676, 0xde14, 0xe676, 0xde14, 0xde14, 
    0xb42b, 0x8a86, 0x9b48, 0xa328, 0xde55, 0xde35, 0xe676, 0xde56, 0xd5d4, 0xddb2, 
    0xb42b, 0x8246, 0x9b27, 0xcd92, 0xe677, 0xd5f3, 0xd5d3, 0xddd5, 0x0000, 0x0000,
    0xb42b, 0x61a5, 0xa3aa, 0xe676, 0xd5f4, 0x8410, 0x0000, 0x0000, 0x0000, 0x0000,
    0xbcae, 0xa3ca, 0xac0c, 0xddf4, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

// Anger 
const unsigned short Mugger[] PROGMEM = {
    // 'supermugger', 10x10px
    0x0000, 0x0000, 0x0000, 0x9800, 0xf800, 0x9800, 0xf800, 0x0000, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x9800, 0x9800, 0xf800, 0x9800, 0x0000, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x9800, 0x0000, 0x0000, 0x9800, 0x0000, 0x9800, 0x9800, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x9800, 0x0000, 0x0000, 0x9800, 0xf800, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x932a, 0x0000, 0x0000, 0x0000, 0xed6f, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0xfe97, 0x0000, 0xed6f, 0xef5f, 0x4acf, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x9800, 0x7a08, 0xdcce, 0x7a08, 0x9800, 0x0000, 0x0000, 
    0x0000, 0x0000, 0xdcce, 0x9800, 0xf800, 0xf800, 0x9800, 0xfe00, 0x7a08, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x9800, 0x9800, 0x9800, 0x9800, 0x0000, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0xfe00, 0x4a49, 0x0000, 0xfe00, 0x0000, 0x0000, 0x0000
};


// THIS IS A TEMPORARY ARRAY FOR IMG_MANIPULATION 
// EX: Copying Array into here when making a custom sprite IN_GAME. ONLY ONE AT A TIME!
unsigned short TEMP[100] = {
    // 'supermugger', 10x10px
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};
//--





// Game Data
int diff = 1; // Difficulty
unsigned int playerData[] = {3,0,0};// { Lives, Score, timeplayed}
byte JoyDir[] = {0,0,0,0}; // UP,DOWN,LEFT,RIGHT

// Longs for tracking the delay between specific actions.
long food_ate = 0;          // The time in millis() when food was ate.
long food_last_frame = 0;   // Millis time from last frame updated for the food.


// STATE_0: MENU_MODE 
// STATE_1: GAMEPLAY_MODE
// STATE_2: HIGHSCORE_MODE
// STATE_3: CREDITS_MODE
byte gameState = GAME_MODE; // A CRITICAL Variable to keep track of how and what to render and utilize inputs. default is gameplay, TODO: MENU_MODE first.



// POS's 
// ALL Enemy DATA. With Draw/Update Flags.
//                    CO-ORDS              offset 8              offset 12               offset 16
//                 A    B    C    D     DRAW FLAGS[8-11]  RandomSPAWN Flags[12-15]    RandomMOVE Flags[16-19]
int EnemyPOS[] = { 0,0, 0,0, 0,0, 0,0     ,0,0,0,0,            0,0,0,0,                    0,0,0,0,};
int playerPOS[] = { 20, 20 };  // Player
int FoodPOS[] = { 0, 0 }; // Food 

// Game_Logic BOOLEAN FLAGS
bool mugged = 0;        // FLAG for Player Mugging Event
bool restart = 0;       // Initialize a restart.
bool makeNewFood = 1;   // FLAG to check if game needs to make a new food object.
bool superFood = 0;     // FLAG to check if SuperFood needs to be generated next food cycle.

// GAME_Draw BOOLEAN FLAGS
bool _drawScore = 1;
bool _drawPlayer = 1;

// Game STRING Variables       0         2         4         6         8
char scorecard[15] = {'S', 'C', 'O', 'R', 'E', ':', ' ', '0', '\0'};
char livesinfo[] =   {'L', 'I', 'V', 'E', 'S' ,':', ' ', '5', '\0' };

// A delay to control updating the GFX Stuff. NOT For Game-Actions
int inputDelay_Menu = 100; // Used for the Menu inputs, as not to 'Super Spam' them.
int inputDelay_Game = 10;
long lastInput = 0;
long timePassed = 0;

long timeEnemyMove = 0;
long timeEnemyPassed = 0;

int GameTimer = 0;
int counter = 0;

// Delay Timers for various things.
const int EnemyMoveDelay = 250; // A MINIMUM Delay for enemies to make a minimum movement.
const int food_delay = 300;     // A FIXED Delay for how soon food should gen after being taken.
const int superDelay = 5000;    // Delay between Super Foods spawns.
const int superScoreReq = 200;  // Minimum Score needed for SuperFoods to spawn
const int superOdds = 10;       // Out a 100

// Analog Storage variable. Nothing more. Made GLOBAL!
int xin = 0;
int yin = 0;

// These are the Border Walls. Should be no reason to change unless screen size changes, which is not a variable in this project.
const int BorderA[] = { 0,0, 128,15 };  // Top Border Bar ACCOUNTING FOR SCORE INFO
const int BorderB[] = { 0,158, 128,2 }; // Bottom Border Bar
const int BorderC[] = { 0,0, 2,160 };   // Left Side
const int BorderD[] = { 126,0, 2,160 }; // Right Side


const int spriteSize = 10; // ALL SPRITES WILL FOLLOW THIS.

const int pSize[] = { spriteSize, spriteSize }; // Size of the Player.


const int maxX = 115; // Bounds for Sprites so they don't escape the screen.
const int minX = 2;
const int maxY = 148;
const int minY = 15;


// Debug Stuff.
bool dbg = 0; // Will be used later. Right now only Toggles an LED-Purple shift.
int dbg_delay = 0; // The Delay for toggling the DBG toggle. To prevent spam.
//                  2     8   11                                   
char dbginfo[] = "L-0  SF-0  \0";
//--

//========================================================================================
//========================================================================================
//========================================================================================
// Setup stuff. ex: Hardware init's, default rgb colors, any calibration/detection needed
// TRY NOT TO USE CUSTOM FUNCTIONS HERE!
void setup() {
    setLED(255, 0, 0); // Red light shows that the game is NOT Ready. BUT That the Game has started setup.

    // Serial Should NOT be left in the final version. It is a debugging tool
    //Serial.begin(9600);

    // Pinmodes for the Analog Joystick
    pinMode(joyHat, INPUT_PULLUP); 
    pinMode(joyX, INPUT);
    pinMode(joyY, INPUT);

    

    // Init LCD to start outputing to it.
    SCR.init();
    SCR.fillScreen(TFT_BLACK);

    // Draw Some base Game Stuff to the Screen. Including Title Card.
    DrawBorders();                                   // Done to setup the game.
    SCR.setTextColor(TFT_WHITE);
    SCR.drawString("Welcome to BIM!",   10, 60, 2);
    SCR.drawString("By Bailey",         10, 80, 2);
    SCR.setTextColor(TFT_RED);
    SCR.drawString("V-BETA",           10, 100, 1); // Show the VERSION
    delay(2000);
    SCR.fillRect(10, 60, 100, 80, TFT_BLACK);       // This Blacks out the Title Card.
    //==========================================
    // CHECK IF JOYSTICK IS PRESENT, IF NOT, This is not playable.
    // TODO: Work out bugs, does not always see the lack of a joystick.
    JoyCheck:
    if (analogRead(joyX) <= 998 && analogRead(joyY) <= 998 && analogRead(joyX) >= 990 && analogRead(joyY) >= 990) {
        POPUP_NOJOYSTICK();
        goto JoyCheck;
    }
    // DO NOT PROCED WITHOUT JOYSTICK! Can't play without it lol

    drawIcon(Bim, playerPOS[0], playerPOS[1]);
    delay(500);     // Delay Before Handing control over to player.
    gameState = GAME_MODE; // Jumps Straight into the Main gameloop after Setup completes. TODO: Make this default to menu instead.
    setLED(0, 255, 0);// We show a GREEN light at the end to represent a GO Status for the program
}

//========================================================================================
//========================================================================================
//========================================================================================
//
//   ALL FUNCTIONS-GROUPS BELOW ARE IN ORDER OF
//   INPUT
//   DRAWING
//   LOGIC
//   EXTRA FUNCTIONS
//   THE LOOP FUNCTION
//
//========================================================================================
//======= INPUT/PLAYER MOVEMENT ==========================================================
//========================================================================================
// 3 Seperate Functions for adjusting the PLAYER POS.

// This Specific function is made to store/record the Analog input, 
// THEN start a chain of functions to update player POS. NOT Draw them.
//==
// TODO: REWRITE THIS to accomodate gameStates and getting input for them.
void READ_INPUT() {
    timePassed = millis() - lastInput;
    // NEW INPUT CODE
    // Game Play Input Control
    if (gameState == GAME_MODE && (timePassed > inputDelay_Game)) {
        lastInput = millis();
        rawInputRead(); // This function sets the JoyStick state that the rest of the code will 
        for (int i = 0; i <= 3; i++) {
            if (JoyDir[i] == 1) {
                MOVE_PLAYER(i); // The Actual Player Movement Logic is done. NOT Draw calls.
                break;
            }
        }
    }
    // Menu Input Control
    if (gameState == MENU_MODE && (timePassed > inputDelay_Menu)) {
        lastInput = millis();
        rawInputRead();
    }

    //Sets all the control flags back to 0. or OFF
    JoyDir[0] = 0;
    JoyDir[1] = 0;
    JoyDir[2] = 0;
    JoyDir[3] = 0;

    // DEBUG CODE - This Will be heavily altered or removed before final version.
    if (digitalRead(joyHat) == LOW) {
        ToggleDebugMode(); // This is obviously a Debug Toggle. Remove after DEV Complete
    }
}

// This Functions Reads the Raw Input and translates it to a stored button press variable.
void rawInputRead() {
    xin = analogRead(joyX);
    yin = analogRead(joyY);
    
    // Y-Axis Reading
    if ((yin > 600 || yin < 400)) {
        if (yin < 300) {
            JoyDir[0] = 1;
            JoyDir[1] = 0;
            JoyDir[2] = 0;
            JoyDir[3] = 0;
            return;
        }
        if (yin > 900) {
            JoyDir[1] = 1;
            JoyDir[0] = 0;
            JoyDir[2] = 0;
            JoyDir[3] = 0;
            return;
        }
    }
    // X-Axis Reading
    if ((xin > 700 || xin < 300)) {

        if (xin < 300) {
            JoyDir[3] = 1;
            JoyDir[1] = 0;
            JoyDir[2] = 0;
            JoyDir[0] = 0;
            return;
        }

        if (xin > 900) {
            JoyDir[2] = 1;
            JoyDir[1] = 0;
            JoyDir[0] = 0;
            JoyDir[3] = 0;
            return;
        }
    }
}

// This one is for translating the analog input to a specific action.
void MOVE_PLAYER(int dir) {
    if (gameState != 1)return;
    switch (dir) {
        default:
           // This should't happen.
           break;
        case 0: // UP
            updatePlayerPOS(playerPOS[0],(playerPOS[1]+1));
            break; 
        case 1: // DOWN
            updatePlayerPOS(playerPOS[0], (playerPOS[1]-1));
            break; 
        case 2: // LEFT
            updatePlayerPOS((playerPOS[0]-1), playerPOS[1]);
            break; 
        case 3: // RIGHT
            updatePlayerPOS((playerPOS[0]+1), playerPOS[1]);
            break;
    }
}

// This updates the playerPOS and saves the old POS for graphics to wipe it.
void updatePlayerPOS(int newX, int newY) {
    if (gameState != 1)return;
    SCR.fillRect(playerPOS[0], playerPOS[1], 10,10,TFT_BLACK);
    if ((newX > maxX) || (newX < minX) || (newY >= maxY) || (newY <= minY)) {
        // These Checks are for Making sure the player does NOT move OUT OF BOUNDS
        // This works by setting to OOB value to the nearest applicable value.
        if (newX > maxX) {
            newX = maxX;
        }
        if (newX < minX) {
            newX = minX;
        }
        if (newY > maxY) {
            newY = maxY;
        }
        if (newY < minY) {
            newY = minY;
        }
    }
    // Sets the player POS. 
    playerPOS[0] = newX;
    playerPOS[1] = newY;
    // Sets the flag to draw the player.
    _drawPlayer = true;
}

//========================================================================================
//======== GRAPHICS AND DRAWING ==========================================================
//========================================================================================

// Drawing Functions here.
// Player Drawing
void drawPlayer() {
    if (gameState != 1)return;
    if (_drawPlayer == true) {
        drawIcon(Bim, playerPOS[0], playerPOS[1]);    // THEN Draw at the NEW POS
        _drawPlayer = false;                  // Sets the FLAG to stop an unneccesary flaw.
    }
}

// Draws the bad guys, muggers.
// This Should be reworked later to support other flags.
void drawMuggers() {
    // If the gamestate is anything but standard play, don't draw enemies.
    if (gameState != 1)return;
    // The only info we check for drawing muggers is their draw flag.
    // This is set by logic functions.
    // After we draw, we set the draw flag to 0, as not to spam their draw calls.
    if (EnemyPOS[8] == 1) {
        drawIcon(Mugger, EnemyPOS[0], EnemyPOS[1]);
        EnemyPOS[8] = 0;
    }
    if (EnemyPOS[9] == 1) {
        drawIcon(Mugger, EnemyPOS[2], EnemyPOS[3]);
        EnemyPOS[9] = 0;
    }
    if (EnemyPOS[10] == 1) {
        drawIcon(Mugger, EnemyPOS[4], EnemyPOS[5]);
        EnemyPOS[10] = 0;
    }
    if (EnemyPOS[11] == 1) {
        drawIcon(Mugger, EnemyPOS[6], EnemyPOS[7]);
        EnemyPOS[11] = 0;
    }
}

// Draws the Food Items for Score
void drawFood() {
    if (gameState != 1)return;
    // DRAW a Food.
    if (makeNewFood == 0 && superFood == 0) {
        drawIcon(Apple, FoodPOS[0], FoodPOS[1]);
    }
    if (makeNewFood == 0 && superFood == 1) {
        drawIcon(Taco, FoodPOS[0], FoodPOS[1]);
    }
}

// Draws the Score Information
void drawScore() {
    if (gameState != 1)return;
    if (_drawScore == 0) {
        return; // Don't Run unless needed.
    }
    // Even BETTER ScoreCard tracker
    for (int i = 0; i < String(playerData[1]).length(); i++) {
        scorecard[7 + i] = String(playerData[1])[i];
        if (i > String(playerData[1]).length()) {
            scorecard[i + 1] = '\0';
        }
    }
    SCR.fillRect(BorderA[0], BorderA[1], BorderA[2], BorderA[3], BORDER_COLOR);
    SCR.setTextColor(TFT_DARKGREEN);
    SCR.drawString(scorecard, 2, 0, 2);
    _drawScore = 0;
}

// Draws a Border around the screen.
// Called when Drawing new border.
// MAKE SURE TO RE-DRAW UI WHEN THIS IS CALLED!
void DrawBorders() {
    SCR.fillRect(BorderA[0], BorderA[1], BorderA[2], BorderA[3], BORDER_COLOR);
    SCR.fillRect(BorderB[0], BorderB[1], BorderB[2], BorderB[3], BORDER_COLOR);
    SCR.fillRect(BorderC[0], BorderC[1], BorderC[2], BorderC[3], BORDER_COLOR);
    SCR.fillRect(BorderD[0], BorderD[1], BorderD[2], BorderD[3], BORDER_COLOR);
}

// Simple Screen Wipe to black.
void DrawBlackScreen() {
    SCR.fillScreen(TFT_BLACK);
    DrawBorders();
}

// Draws the MUGGED NOTIFICATION. Also Pausing the Game for 5 Seconds. After this Draw Call a Logic Function should take place.
void POPUP_MUGGED() {
    SCR.fillRect(0, 0, 128, 160, TFT_RED);
    SCR.setTextColor(TFT_BLACK);
    SCR.drawString("YOU HAVE BEEN",minX+10, minY+40, 2);
    SCR.drawString("MUGGED!", minX + 10, minY + 60, 2);
    SCR.drawString(livesinfo, minX + 10, minY + 90, 2);
    for (int i = 0; i < 10; i++) {
        delay(250);
        SCR.setTextColor(TFT_WHITE);
        SCR.drawString(livesinfo, minX + 10, minY + 90, 2);
        delay(250);
        SCR.setTextColor(TFT_BLACK);
        SCR.drawString(livesinfo, minX + 10, minY + 90, 2);
    }
}

// Game over popup/menu. Only gives player option of seeing highscores/entering their own.
void POPUP_GAMEOVER() {

}

// Show this when a new Highscore is Achieved. 
void POPUP_HIGHSCORE() {

}

// This is the Highscore MENU draw code. NOT the popup.
void MENU_HIGHSCORE() {

}

// Main Menu Draw code
void MENU_MAIN() {

}

// ERROR POPUPS! 
// NO JOYSTICK POPUP
void POPUP_NOJOYSTICK() {
    SCR.setTextColor(TFT_WHITE);
    SCR.drawString("ERROR!", 30, 60, 2);
    SCR.drawString("NO JOYSTICK!", 10, 80, 2);
    delay(500);
    SCR.fillRect(10, 60, 100, 80, TFT_BLACK);       // This Blacks out the popup
}

// A Debug Overlay. 
void DrawDebugInfo() {
    SCR.setTextColor(0xFFFF);
    dbginfo[2] = (playerData[0]+48);
    dbginfo[8] = (superFood+48);
    SCR.drawString(dbginfo, 5, 20, 1);
}

// Draw Bitmap function from the Faster TFT library. (Bodmer-ST7735)
// Maximum icon size is <<255x255>> pixels to avoid integer overflow. 
// NOTE: this Function currently only works with 10x10 Byte-Array-Images!
void drawIcon(const unsigned short* icon, int16_t x, int16_t y) {
 
    uint16_t  pix_buffer[64];   // 64-Pixel Buffer (16 bits per pixel)

    // Set up a window the right size to stream pixels into
    SCR.setAddrWindow(x, y, x + 10 - 1, y + 10 - 1);

    // Work out the number whole buffers to send
    uint16_t nb = ((uint16_t)10 * 10) / 64;

    // Fill and send "nb" buffers to TFT
    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < 64; j++) {
            pix_buffer[j] = pgm_read_word(&icon[i * 64 + j]);
        }
        SCR.pushColors(pix_buffer, 64);
    }

    // Work out number of pixels not yet sent
    uint16_t np = ((uint16_t)10 * 10) % 64;

    // Send any partial buffer left over
    if (np) {
        for (int i = 0; i < np; i++) pix_buffer[i] = pgm_read_word(&icon[nb * 64 + i]);
        SCR.pushColors(pix_buffer, np);
    }
}

// All Draw Calls into one General Function for the MainLoop to use.
// ONLY THE DRAW CALLS FOR STANDARD PLAY
void gameDrawCalls() {
    drawFood();    // Draw the Food Objects.
    drawMuggers();     // Draw Muggers
    drawScore();    // Draw Score 
    if (dbg==1) {
        DrawDebugInfo();
    }
}

//========================================================================================
//=============== LOGIC ==================================================================
//========================================================================================
// THIS CODE DOES NOT DO ANY DRAW CALLS. NO LOGIC SHOULD.


// Difficulty Updater
// Each IF should run ONCE. So we don't spam this code.
void Difficultyupdate() {
    if (playerData[1] > 49 && diff == 1 ) {
        EnemyPOS[12] = 1;
        diff = 2; return;
    }
    if(playerData[1] > 149 && diff == 2  ) {
        EnemyPOS[13] = 1;
        diff = 3; return;
    }
    if (playerData[1] > 349 && diff == 3 ) {
        EnemyPOS[14] = 1;
        diff = 4; return;
    }
    if (playerData[1] > 599 && diff == 4){
        EnemyPOS[15] = 1;
        diff = 5; return;
    }
    // POST Level 5 Difficulty increasing.
    // New Feature. Needs Testing. Every 150-Score = Difficulty increase.
    if (diff >= 5) {
        if (playerData[1] > (150*diff)) {
            diff++;
        }
    }
}

// Updates/Sets Bad Guys Logic/Positions
// CURRENTLY 2 MAJOR USES: Randomly spawns the muggers AND Randomly moves them.
void updateMuggers() {
    // Work through the Muggers ONE at a time.
    // First Check if they need to do a RandomSPAWN. THEN Check if they have a move pending.
    // 1st Enemy RandomSpawn Check
    //==============================================================================================
    if (EnemyPOS[12] == 1) {
        SCR.fillRect(EnemyPOS[0], EnemyPOS[1], 10, 10, TFT_BLACK);
    POS_A:
            EnemyPOS[0] = getRandom(minX + 30, maxX - 30);
            EnemyPOS[1] = getRandom(minY + 20, maxY - 20);
            if (Collision(EnemyPOS[0], EnemyPOS[1], playerPOS[0], playerPOS[1]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[2], EnemyPOS[3]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[4], EnemyPOS[5]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[6], EnemyPOS[7]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[0], EnemyPOS[1], FoodPOS[0], FoodPOS[1]) == true) {
                goto POS_A;
            }
            EnemyPOS[8] = 1;
            EnemyPOS[12] = 0;
    } 
    //=============================================================================================
    // 2nd Enemy RandomSpawn Check
    if (EnemyPOS[13] == 1) {
        SCR.fillRect(EnemyPOS[2], EnemyPOS[3], 10, 10, TFT_BLACK);
        POS_B:
            EnemyPOS[2] = getRandom(minX + 30, maxX - 30);
            EnemyPOS[3] = getRandom(minY + 20, maxY - 20);
            if (Collision(EnemyPOS[2], EnemyPOS[3], playerPOS[0], playerPOS[1]) == true) {
                goto POS_B;
            }
            if (Collision(EnemyPOS[2], EnemyPOS[3], EnemyPOS[0], EnemyPOS[1]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[2], EnemyPOS[3], EnemyPOS[4], EnemyPOS[5]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[2], EnemyPOS[3], EnemyPOS[6], EnemyPOS[7]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[2], EnemyPOS[3], FoodPOS[0], FoodPOS[1]) == true) {
                goto POS_A;
            }
            EnemyPOS[9] = 1;
            EnemyPOS[13] = 0;
    }
    //=============================================================================================
    // 3rd Enemy RandomSpawn Check
    if (EnemyPOS[14] == 1) {
        SCR.fillRect(EnemyPOS[4], EnemyPOS[5], 10, 10, TFT_BLACK);
        POS_C:
            EnemyPOS[4] = getRandom(minX + 30, maxX - 30);
            EnemyPOS[5] = getRandom(minY + 20, maxY - 20);
            if (Collision(EnemyPOS[4], EnemyPOS[5], playerPOS[0], playerPOS[1]) == true) {
                goto POS_C;
            }
            if (Collision(EnemyPOS[4], EnemyPOS[5], EnemyPOS[2], EnemyPOS[3]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[4], EnemyPOS[5], EnemyPOS[0], EnemyPOS[1]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[4], EnemyPOS[5], EnemyPOS[6], EnemyPOS[7]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[4], EnemyPOS[5], FoodPOS[0], FoodPOS[1]) == true) {
                goto POS_A;
            }
            EnemyPOS[10] = 1;
            EnemyPOS[14] = 0;
    }
    //=============================================================================================
    // 4th Enemy RandomSpawn Check
    if (EnemyPOS[15] == 1) {
        SCR.fillRect(EnemyPOS[6], EnemyPOS[7], 10, 10, TFT_BLACK);
        POS_D:
            EnemyPOS[6] = getRandom(minX + 30, maxX - 30);
            EnemyPOS[7] = getRandom(minY + 20, maxY - 20);
            if (Collision(EnemyPOS[6], EnemyPOS[7], playerPOS[0], playerPOS[1]) == true) {
                goto POS_D;
            }
            if (Collision(EnemyPOS[6], EnemyPOS[7], EnemyPOS[2], EnemyPOS[3]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[6], EnemyPOS[7], EnemyPOS[4], EnemyPOS[5]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[6], EnemyPOS[7], EnemyPOS[0], EnemyPOS[1]) == true) {
                goto POS_A;
            }
            if (Collision(EnemyPOS[6], EnemyPOS[7], FoodPOS[0], FoodPOS[1]) == true) {
                goto POS_A;
            }
            EnemyPOS[11] = 1;
            EnemyPOS[15] = 0;
    }
    //=============================================================================================

    // Check if any random moves get to occur.
    // Set the appropriate move flag(s)
    timeEnemyPassed = millis() - timeEnemyMove;
    if ((timeEnemyPassed > (EnemyMoveDelay-(10*diff))) && (diff > 1)) {
        timeEnemyMove = millis();
        switch (getRandom(0, 3)) {
        case 0:
            if (diff < 2)return;
            EnemyPOS[16] = 1;
            break;
        case 1:
            if (diff < 3)return;
            EnemyPOS[17] = 1;
            break;
        case 2:
            if (diff < 4)return;
            EnemyPOS[18] = 1;
            break;
        case 3:
            if (diff < 5)return;
            EnemyPOS[19] = 1;
            break;
        }
    }
    
    //=============================================================================================
    // 1st Enemy Move Check/Action
    if (EnemyPOS[16] == 1) {
        MoveMugger(0,1);
        EnemyPOS[16] = 0;
        if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[2], EnemyPOS[3]) == true) {
            EnemyPOS[9] = 1;
        }
        if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[4], EnemyPOS[5]) == true) {
            EnemyPOS[10] = 1;
        }
        if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[6], EnemyPOS[7]) == true) {
            EnemyPOS[11] = 1;
        }
    }
    //=============================================================================================
    // 2nd Enemy Move Check/Action
    if (EnemyPOS[17] == 1) {
        MoveMugger(2,3);
        EnemyPOS[17] = 0;
        if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[2], EnemyPOS[3]) == true) {
            EnemyPOS[8] = 1;
        }
        if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[4], EnemyPOS[5]) == true) {
            EnemyPOS[10] = 1;
        }
        if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[6], EnemyPOS[7]) == true) {
            EnemyPOS[11] = 1;
        }
    }
    //=============================================================================================
    // 3rd Enemy Move Check/Action
    if (EnemyPOS[18] == 1) {
        MoveMugger(4,5);
        EnemyPOS[18] = 0;
        if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[2], EnemyPOS[3]) == true) {
            EnemyPOS[9] = 1;
        }
        if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[4], EnemyPOS[5]) == true) {
            EnemyPOS[8] = 1;
        }
        if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[6], EnemyPOS[7]) == true) {
            EnemyPOS[11] = 1;
        }
    }
    //=============================================================================================
    // 4th Enemy Move Check/Action
    if (EnemyPOS[19] == 1) {
        MoveMugger(6,7);
        EnemyPOS[19] = 0;
        if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[2], EnemyPOS[3]) == true) {
            EnemyPOS[10] = 1;
        }
        if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[4], EnemyPOS[5]) == true) {
            EnemyPOS[9] = 1;
        }
        if (Collision(EnemyPOS[0], EnemyPOS[1], EnemyPOS[6], EnemyPOS[7]) == true) {
            EnemyPOS[8] = 1;
        }
    }
    
}

// Checks/Updates Food logic
// 90% of the code is picking a location for the next food to spawn. 
// 10% is for possibly spawning a SuperFood
void updateFood() {
    // Runs through the logic of making new food.
    if (makeNewFood == 1 && ((millis() - food_ate) > food_delay)) {
        if (diff > 1 && (getRandom(0,100)<(superOdds+(diff*2)))) { // Low odds for Super food.
           superFood = true;
        }
        else {
           superFood = false;
        }
    FoodGen:
        FoodPOS[0] = getRandom(minX, maxX);
        FoodPOS[1] = getRandom(minY, maxY);
        if (Collision(FoodPOS[0], FoodPOS[1], playerPOS[0], playerPOS[1]) == true) {
            goto FoodGen;
        }
        if (Collision(FoodPOS[0], FoodPOS[1], EnemyPOS[0], EnemyPOS[1]) == true && diff > 1) {
            goto FoodGen;
        }
        if (Collision(FoodPOS[0], FoodPOS[1], EnemyPOS[2], EnemyPOS[3]) == true && diff > 2) {
            goto FoodGen;
        }
        if (Collision(FoodPOS[0], FoodPOS[1], EnemyPOS[4], EnemyPOS[5]) == true && diff > 3) {
            goto FoodGen;
        }
        if (Collision(FoodPOS[0], FoodPOS[1], EnemyPOS[6], EnemyPOS[7]) == true && diff > 4) {
            goto FoodGen;
        }
        makeNewFood = 0;
        return;
    }
}


// Basic 2D Square collision detection.
// THIS ASSUMES ALL 'Entities' are 10W and 10H!!!
bool Collision(int x0, int y0, int x1, int y1) {
    if (x0 + 10 >= x1 &&    // r1 right edge past r2 left
        x0 <= x1 + 10 &&    // r1 left edge past r2 right
        y0 + 10 >= y1 &&    // r1 top edge past r2 bottom
        y0 <= y1 + 10) {    // r1 top bottom edge past r2 bottom
        return true;
    }
    return false;
}

// This function will allow entities to randomly move.
// The parameters are NOT the X,Y of the Mugger. The parameters are the EnemyPOS array reference numbers.
void MoveMugger(int Eposx, int Eposy) {
    SCR.fillRect(EnemyPOS[Eposx], EnemyPOS[Eposy], 10, 10, TFT_BLACK);
    int newX = EnemyPOS[Eposx];
    int newY = EnemyPOS[Eposy];


    // Random Move not 
    // Move Certain Muggers VERTICALLY
    if ((Eposx % 4) == 0) {
        if (getRandom(0, 1) == 0) {
            newY = newY - (5 + diff);
        } else {
            newY = newY + (5 + diff);
        }
    }

    // Move Certain Muggers HORIZONTILLY
    if ((Eposx % 4) == 2) {
        if (getRandom(0, 1) == 0) {
            newX = newX - (5 + diff);
        } else {
            newX = newX + (5 + diff);
        }
    }

    // OUT OF Bounds check/Fix
    if (newX > (maxX-20)) {
        newX = (maxX - 20);
    }
    if (newX < (minX+20)) {
        newX = (minX + 20);
    }
    if (newY > (maxY-20)) {
       newY = (maxY - 20);
    }
    if (newY < (minY+20)) {
        newY = (minY + 20);
    }
    //=============================
    // Sets the Mugger POS. 
    // Then sets the appropriate flag to signal a new draw is needed for the mugger.
    EnemyPOS[Eposx] = newX;
    EnemyPOS[Eposy] = newY;
    EnemyPOS[8+(Eposx-(Eposx/2))] = 1;
}

// Collision Detection Function Wrapped here instead of Main-Logic loop.
// HERE WE WILL CHECK COLLISIONS/WIPE SPRITES/CHANGE SCORE/ETC
// 1st   : Check Collisions and do proper updates/set update variables.
// 2nd   : Check if difficulty needs updated.
// 3nd   : Update Food-Logic
// 4th   : Update Enemy-Logic
// 5th   : Player-Logic/Left over stuff
void updateCollision() {
    // First CHECK FOR FOOD COLLISONS
    if (Collision(FoodPOS[0], FoodPOS[1], playerPOS[0], playerPOS[1]) == true && makeNewFood == 0) {
        // Clear the food off the screen
        SCR.fillRect(FoodPOS[0], FoodPOS[1], 10,10, TFT_BLACK);
        if (Collision(FoodPOS[0], FoodPOS[1], playerPOS[0], playerPOS[1])) {
            drawPlayer(); // Incase we black out the player.
        }
        // Update Score accounting for SUPERFOOD!
        playerData[1] = (superFood==true) ? playerData[1]+20*diff : playerData[1]+5*diff;

        
        makeNewFood = 1;    // Sets the Foodmakin flag
        _drawScore = 1;    // Sets the Update Score Flag
        food_ate = millis();// Sets the time delay for the food spawner
        FoodPOS[0] = 300;   // Sets the Location to OUT of the rendered screen.
        FoodPOS[1] = 300;
    }

    // Check/Process Mugger/Player Collisions
    if (diff > 1 ) {
        if (Collision(EnemyPOS[0], EnemyPOS[1], playerPOS[0], playerPOS[1])) {
            mugged = 1;
        }
    }
    if (diff > 2) {
        if (Collision(EnemyPOS[2], EnemyPOS[3], playerPOS[0], playerPOS[1])) {
            mugged = 1;
        }
    }
    if (diff > 3) {
        if (Collision(EnemyPOS[4], EnemyPOS[5], playerPOS[0], playerPOS[1])) {
            mugged = 1;
        }
    }
    if (diff > 4) {
        if (Collision(EnemyPOS[6], EnemyPOS[7], playerPOS[0], playerPOS[1])) {
            mugged = 1;
        }
    }
}


// Checks if the player was MUGGED! This is the LOGICAL CODE ONLY. The Drawing Code is a 2nd Function.
void CheckMuggedStatus() {
    if (mugged==1) {
        playerData[0] = playerData[0] - 1;
        livesinfo[7] = (playerData[0] + 48);
        POPUP_MUGGED();
        // Clear the Scorecard so the new/old Score don't overlap
        for (int i = 0; i < String(playerData[1]).length(); i++) {
            scorecard[7 + i] = ' ';
            if (i > String(playerData[1]).length()) {
                scorecard[i + 1] = '\0';
            }
        }
        if (playerData[1] > 100) {
            playerData[1] = playerData[1] - 100;
        }
        else {
            playerData[1] = 0;
        }
        playerPOS[0] = minX+1;
        playerPOS[1] = minY+1;
        switch (diff) {
            default: break; // Shouldn't be called....
            case 2: EnemyPOS[12] = 1; break;
            case 3: EnemyPOS[12] = 1; EnemyPOS[13] = 1; break;
            case 4: EnemyPOS[12] = 1; EnemyPOS[13] = 1; EnemyPOS[14] = 1; break;
            case 5: EnemyPOS[12] = 1; EnemyPOS[13] = 1; EnemyPOS[14] = 1; EnemyPOS[15] = 1; break;
        }
        SCR.fillRect(0, 0, 128, 160, TFT_BLACK);
        DrawBorders();
        _drawScore = 1;
        makeNewFood = 1;
        _drawPlayer = 1;
        mugged = 0;
    }
}

// Checks if the player has been Mugged Too many times. OR if the Timer has run out yet.
// NEEDS-IMPLEMENTED
void CheckGAMEOVER() {

    // Check for LIVES = 0
    if (playerData[0] == 0) {
        // DEAD!
    }
    // Check for TIMER reaching 0.
    if (playerData[0] == 0) {
        // DEAD!
    }
}

// Run Through all the LOGICAL Code in this SPECIFIC Order.
void RunLogic() {
    
    // Check Collisions
    updateCollision();
    // Difficulty update
    Difficultyupdate();
    // Food Logic
    updateFood();
    // Enemy Logic/Tick
    updateMuggers();
    // Check if the player was mugged this cycle...
    CheckMuggedStatus();
    // Check if it's time to GAME_OVER The player
    CheckGAMEOVER();
    // Left Over Logic HERE!
}
// Misc Functions. Not LOGIC/DRAWING Code.
// 
// Some RGB Functions
void setLED(byte r, byte g, byte b) {
    analogWrite(Rpin, r);
    analogWrite(Gpin, g);
    analogWrite(Bpin, b);
}

// DBG init Function
void ToggleDebugMode() {
    if ((millis() - dbg_delay) < 3000) return;
    
    if (dbg == true) {
        dbg = false;
        setLED(0,255,0); // Set the LED Blue to symbolize the DBG mode being OFF AKA Standard Play
        dbg_delay = millis();
        SCR.setTextColor(TFT_WHITE);
        SCR.drawString("DEBUG_OFF", 20, 80, 2);
        delay(500);
        EnemyPOS[8] = 1;
        EnemyPOS[9] = 1;
        EnemyPOS[10] = 1;
        EnemyPOS[11] = 1;
        _drawPlayer = 1;
        SCR.fillRect(20, 80, 80, 30, TFT_BLACK);
    } else {
        dbg = true;
        dbg_delay = millis();
        SCR.setTextColor(TFT_WHITE);
        SCR.drawString("DEBUG_ON", 20, 80, 2);
        delay(500);
        SCR.fillRect(20, 80, 80, 30, TFT_BLACK);
        drawPlayer();
        setLED(0, 0, 255); // Set the LED Blue to symbolize the DBG mode being ON
    }
    
}

// RNG with given range.
int getRandom(int lower, int upper) {
    return ((rand() % (upper - lower + 1)) + lower);
}

/*
    Writing to the rom should be done RARELY. 
    Reading can happen without 
*/
bool SaveDataToRom(int location, byte data) {
    // TODO: Write the 'Write' code.
    // Should Return true if successful.
}

byte ReadDataFromRom(int location) {
    // Return the value stored in ROM.
    // TODO: Create a super function using this to create 
}

//========================================================================================
//=============== THE LOOP ===============================================================
//========================================================================================

// Default Loop Code. Fill with main loop of program, LAST FUNCTION IN FILE! As it will be the last function run.
void loop() {
    // Game State 1 is standard game-play loop
    if (gameState == 1) {
        // We let the player move FIRST.
        READ_INPUT(); // Check for any INPUT.
        drawPlayer(); // Let the Player Move
        // THEN Process their actions.
        RunLogic();     // Run through all the logic, this should mostly affect the MuggerAI / Update powerup odds/flags.
        gameDrawCalls(); // Run through applicable Draw Calls.
    }
    // Menu Loop
    if (gameState == 0) {
        // TODO: Menu Loop
    }
    // Highscore Input/Display Loop
    if (gameState == 2) {
        // TODO Highscore Loop
    }
    
}
