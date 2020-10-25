/*

    PURPOSE         : A Game using Arduino(MEGA2560R3) and an 1.8" 128x160 TFT_LCD_DISPLAY
                     
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
#define MAX_GAME_TIME 300000
#define MAX_LIVES 5
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
// EX: Copying Array into here when making a 'Gold' or flashing sprite. ONLY ONE AT A TIME!
unsigned short TEMP[100];
//--





// Game Data
int diff = 1; // Difficulty
unsigned int playerData[] = {5,70};// { Lives, Score}

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
//                 A    B    C    D     DRAW FLAGS[8-11]  UPDATE FLAGS [12-15]   RandomMove Flags[16-20]
int EnemyPOS[] = { 0,0, 0,0, 0,0, 0,0     , 0,0,0,0,            0,0,0,0,              0,0,0,0 };
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
int TimeDelay = 10;
long LastMoved = 0;

// Delay Timers for various things.
const int EnemyMoveDelay = 500; // A MINIMUM Delay for enemies to make a minimum movement.
const int food_delay = 300;     // A FIXED Delay for how soon food should gen after being taken.
const int superDelay = 5000;    // Delay between Super Foods spawns.
const int superScoreReq = 200;  // Minimum Score needed for SuperFoods to spawn
const int superOdds = 50;        // Out a 100

// Analog Storage variable. Nothing more. Made GLOBAL!
int xin = 0;
int yin = 0;

// These CONSTs have NO Reason to be updated/Changed. 
// They are CONST for a reason. Core Gameplay/Settings
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
//                  2     8   12                                   
char dbginfo[] = "L-0  SF-0  \0";
//--

//========================================================================================
//========================================================================================
//========================================================================================
// Setup stuff. ex: Hardware init's, default rgb colors, any calibration needed
// TRY NOT TO USE CUSTOM FUNCTIONS HERE!
void setup() {
    setLED(255, 0, 0); // Red light shows that the game is NOT Ready.

    // Serial Should NOT be left in the final version. It is a debugging tool
    //Serial.begin(9600);

    // Pinmodes for the Analog Joystick
    pinMode(joyHat, INPUT_PULLUP);
    pinMode(joyX, INPUT);
    pinMode(joyY, INPUT);

    // Init LCD to start outputing to it.
    SCR.init();
    SCR.fillScreen(TFT_BLACK);
    DrawBorder();                                   // Done to setup the game.
    SCR.setTextColor(TFT_WHITE);
    SCR.drawString("Welcome to BIM!",   10, 60, 2);
    SCR.drawString("By Bailey",         10, 80, 2);
    SCR.setTextColor(TFT_RED);
    SCR.drawString("V-BETA",           10, 100, 1); // Show the VERSION
    delay(3000);
    SCR.fillRect(10, 60, 100, 80, TFT_BLACK);       // This Blacks out the Title Card.
    drawIcon(Bim, playerPOS[0], playerPOS[1]);
    delay(500);     // Delay Before Handing control over to player.
    gameState = 1; // Jumps Straight into the Main gameloop after Setup completes.
    setLED(0, 255, 0);// We show a GREEN light at the end to represent a GO Status for the program
}

//========================================================================================
//========================================================================================
//========================================================================================
//
//   ALL FUNCTIONS BELOW ARE IN ORDER OF
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
    if ((millis() - LastMoved) < TimeDelay) {
        return; // Dont't spam movement.
    }
    LastMoved = millis();
    // Special INPUTs checked here. ex: Joystick hat button
    if (digitalRead(joyHat) == LOW) {
        ToggleDebugLight();
    }
    xin = analogRead(joyX);
    yin = analogRead(joyY);
    // X-Based Movement
    if ((xin > 700 || xin < 300) && (yin < 600 && yin > 400)) {
        if (xin < 300) {
            // RIGHT
            MOVE_PLAYER(3);
        }
        if (xin > 900) {
            // LEFT
            MOVE_PLAYER(2);
        }
    }
    // Y-Based Movement
    if ((yin > 600 || yin < 400) && (xin < 600 && xin > 400)) {

        if (yin < 300) {
            // UP
            MOVE_PLAYER(0);
        }
        if (yin > 900) {
            // DOWN
            MOVE_PLAYER(1);
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
void drawBads() {
    if (gameState != 1)return;
    // We'll use the difficulty to determine amt of enemies to draw and check for.
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
void DrawBorder() {
    SCR.fillRect(BorderA[0], BorderA[1], BorderA[2], BorderA[3], BORDER_COLOR);
    SCR.fillRect(BorderB[0], BorderB[1], BorderB[2], BorderB[3], BORDER_COLOR);
    SCR.fillRect(BorderC[0], BorderC[1], BorderC[2], BorderC[3], BORDER_COLOR);
    SCR.fillRect(BorderD[0], BorderD[1], BorderD[2], BorderD[3], BORDER_COLOR);
}

// Draws the MUGGED NOTIFICATION. Also Pausing the Game for 5 Seconds. After this Draw Call a Logic Function should take place.
void DrawMUGGED_POPUP() {
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

void DrawGAMEOVER_POPUP() {

}

// Show this when a new Highscore is Achieved. 
void DrawHIGHSCORE_POPUP() {

}

// This is the Highscore MENU draw code. NOT the popup.
void DrawHIGHSCORE_MENU() {

}

// A Debug Overlay. 
void DrawDebugInfo() {
    SCR.setTextColor(0xFFFF);
    dbginfo[2] = (playerData[0]+48);
    dbginfo[8] = String(superFood)[0];
    SCR.drawString(dbginfo, 5, 20, 1);
}

// Draw Bitmap function from the Faster TFT library. (Bodmer-ST7735)
// Maximum icon size is <<255x255>> pixels to avoid integer overflow
void drawIcon(const unsigned short* icon, int16_t x, int16_t y) {
 
    uint16_t  pix_buffer[64];   // 64-Pixel Buffer (16 bits per pixel)

    // Set up a window the right size to stream pixels into
    SCR.setAddrWindow(x, y, x + 10 - 1, y +10 - 1);

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
void RunDrawCalls() {
    drawFood();    // Draw the Food Objects.
    drawBads();     // Draw Muggers
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
    if (playerData[1] > 99 && diff == 1 ) {
        EnemyPOS[12] = 1;
        diff = 2; return;
    }
    if(playerData[1] > 299 && diff == 2  ) {
        EnemyPOS[13] = 1;
        diff = 3; return;
    }
    if (playerData[1] > 599 && diff == 3 ) {
        EnemyPOS[14] = 1;
        diff = 4; return;
    }
    if (playerData[1] > 999 && diff == 4) {
        EnemyPOS[15] = 1;
        diff = 5; return;
    }
}

// Updates/Sets Bad Guys Logic/Positions
void updateMuggers() {
    // Work through the Muggers ONE at a time.
    //=============================================================================================
    // 1st Enemy
    if (EnemyPOS[12] == 1) {
        SCR.fillRect(EnemyPOS[0], EnemyPOS[1], 10, 10, TFT_BLACK);
    POS_A:
            EnemyPOS[0] = getRandom(maxX, minX+50);
            EnemyPOS[1] = getRandom(maxY, minY);
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
    // 2nd Enemy Update
    if (EnemyPOS[13] == 1) {
        SCR.fillRect(EnemyPOS[2], EnemyPOS[3], 10, 10, TFT_BLACK);
        POS_B:
            EnemyPOS[2] = getRandom(maxX, minX);
            EnemyPOS[3] = getRandom(maxY, minY+50);
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
    // 3rd Enemy Update
    if (EnemyPOS[14] == 1) {
        SCR.fillRect(EnemyPOS[4], EnemyPOS[5], 10, 10, TFT_BLACK);
        POS_C:
            EnemyPOS[4] = getRandom(maxX, minX);
            EnemyPOS[5] = getRandom(maxY-40, minY);
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
    // 4th Enemy Update
    if (EnemyPOS[15] == 1) {
        SCR.fillRect(EnemyPOS[6], EnemyPOS[7], 10, 10, TFT_BLACK);
        POS_D:
            EnemyPOS[6] = getRandom(maxX-50, minX);
            EnemyPOS[7] = getRandom(maxY, minY);
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
}

// Checks/Updates Food logic
// 90% of the code is picking a location for the next food to spawn. 
// 10% is for possibly spawning a SuperFood
void updateFood() {
    // Runs through the logic of making new food.
    if (makeNewFood == 1 && ((millis() - food_ate) > food_delay)) {
        if (diff > 1 && (getRandom(1,100)<(superOdds+(diff*2)))) { // Low odds for Super food.
           superFood = true;
        }
        else {
           superFood = false;
        }
    FoodGen:
        FoodPOS[0] = getRandom(maxX, minX);
        FoodPOS[1] = getRandom(maxY, minY);
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

// This will be used to create moving Muggers/Food
void moveSprite(int x, int y) {
    // TODO: Implement
}

// Collision Detection Function Wrapped here instead of Main-Logic loop.
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
// HERE WE WILL CHECK COLLISIONS/WIPE SPRITES/CHANGE SCORE/ETC
// 1st   : Check Collisions and do proper updates/set update variables.
// 2nd   : Check if difficulty needs updated.
// 3nd   : Update Food-Logic
// 4th   : Update Enemy-Logic
// 5th   : Player-Logic/Left over stuff

// Checks if the player was MUGGED! This is the LOGICAL CODE ONLY. The Drawing Code is a 2nd Function.
void CheckMuggedStatus() {
    if (mugged==1) {
        playerData[0] = playerData[0] - 1;
        livesinfo[7] = (playerData[0] + 48);
        DrawMUGGED_POPUP();
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
        DrawBorder();
        _drawScore = 1;
        makeNewFood = 1;
        drawPlayer();
        mugged = 0;
    }
}

// Checks if the player has been Mugged Too many times. OR if the Timer has run out yet.
// NEEDS-IMPLEMENTED
void CheckGAMEOVER() {

    // Check for LIVES = 0

    // Check for TIMER reaching 0.

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
    // None yet.
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
void ToggleDebugLight() {
    if ((millis() - dbg_delay) < 3000) {
        return;
    }
    if (dbg == true) {
        dbg = false;
        setLED(0,255,0); // Set the LED Blue to symbolize the DBG mode being OFF AKA Standard Play
        dbg_delay = millis();
        SCR.setTextColor(TFT_WHITE);
        SCR.drawString("DEBUG_OFF", 20, 80, 2);
        delay(500);
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

// RNG for the Spawns with given random vars.
int getRandom(int lower, int upper) {
    return ((rand() % (upper - lower + 1)) + lower);
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
        RunDrawCalls(); // Run through applicable Draw Calls.
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
