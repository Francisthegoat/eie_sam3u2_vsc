#include "configuration.h"
void DelayMs(u32 timems);
//Made by chat gpt so level 2 can work without breaking board.
void _kill(int pid, int sig) {
    // Do nothing or add your custom implementation if needed
    while (1);  // Infinite loop to "kill" the program
}

int _getpid(void) {
    return 1;  // Return a dummy process ID
}

int _fstat(int fd, struct stat *st) {
    // Return 0 to indicate that the file status is always successful
    return 0;
}

int _isatty(int fd) {
    // Always return true (1), assuming no file descriptor handling
    return 1;
}
void _exit(int status) {
    while (1);  // Do nothing, hang the system
}

int _read(int fd, char *buf, size_t count) {
    return 0; // Simulate a no-op read
}

int _write(int fd, const char *buf, size_t count) {
    // Simulate writing to the console or terminal
    return count;
}

int _close(int fd) {
    return -1; // Simulate closing a file descriptor
}

int _lseek(int fd, int offset, int whence) {
    return 0; // Simulate seeking in a file
}

/***********************************************************************************************************************
Global variable definitions with scope across the entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
volatile u32 G_u32UserApp1Flags; /*!< @brief Global state flags */

/* Existing variables */
extern volatile u32 G_u32SystemTime1ms; /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;  /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;  /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags; /*!< @brief From main.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine; /*!< @brief The state machine function pointer */
static u8 Password[4] = {0, 1, 1, 0};     // Default password with 4 inputs
static u8 CandidatePassword[4];          // User-entered password
static u8 InputIndex = 0;
static u8 NewPassword[4];                 // Track user input index
static bool SettingPassword = FALSE;      // Flag to track password-setting state
static Level1clk ;
static clear;                     // Track level 1 clock count
static u8 Binarycount[3];
static u8 Binarycountcheck[3];
static level2clk ;                     // Track level 2 clock count
static Endgameclk ;                     // Track endgame clock count
static winclk ;
static u8 InputIndex1 = 0;       // Tracks user input index
static u8 GamePhase = 0;        // 0 = Show numbers, 1 = Ready, 2 = Input, 3 = Validate, 4 = Success Confirm
static u16 Level2Timer = 0;     // Timer for transitions
static u8 DisplayIndex = 0;     // Tracks which number is being displayed
static u8 Round = 1;
static u8 step = 0;
static u8 startTime = 0;
static Buzz = 0;

static Lvl1SoundClk = 0;
static int noteIndex = 0;
static int state = 0;
static int timer = 0;
static int silenceDuration = 50; // Short silence between notes
static int ledState = 0;  // Tracks LED flashing state

static Lvl2SoundClk = 0;
static int noteIndex2 = 0;
static int state2 = 0;
static int timer2 = 0;
static int silenceDuration2 = 50; // Short silence between notes
static int ledState2 = 0;  // Tracks LED flashing state

static WinSoundClk = 0;
static int noteIndex3 = 0;
static int state3 = 0;
static int timer3 = 0;
static int silenceDuration3 = 50; // Short silence between notes
static int ledState3 = 0;  // Tracks LED flashing state

/***********************************************************************************************************************
LED Control Functions
***********************************************************************************************************************/
static void ResetGameVariables(void) {
    memset(Binarycount, 0, sizeof(Binarycount));
    memset(Binarycountcheck, 0, sizeof(Binarycountcheck));
    Level1clk = 0;
    level2clk = 0;
    Endgameclk = 0;
    winclk = 0;
    clear = 0;
    InputIndex = 0;       // Tracks user input index
    InputIndex1 = 0;       // Tracks user input index
    GamePhase = 0;        // 0 = Show numbers, 1 = Ready, 2 = Input, 3 = Validate, 4 = Success Confirm
    Level2Timer = 0;     // Timer for transitions
    DisplayIndex = 0;     // Tracks which number is being displayed
    Round = 1;
    Buzz = 0;

    Lvl1SoundClk = 0;
    noteIndex = 0;
    state = 0;
    timer = 0;
    silenceDuration = 50; // Short silence between notes
    ledState = 0;

    Lvl2SoundClk = 0;
    noteIndex2 = 0;
    state2 = 0;
    timer2 = 0;
    silenceDuration2 = 50; // Short silence between notes
    ledState2 = 0;

    WinSoundClk = 0;
    noteIndex3 = 0;
    state3 = 0;
    timer3 = 0;
    silenceDuration3 = 50; // Short silence between notes
    ledState3 = 0;

}

void LedSetColorWhite(void) {
    LedOn(RED3);
    LedOn(GREEN3);
    LedOn(BLUE3);
    LedPWM(RED3, LED_PWM_10);  // 50% brightness for RED
    LedPWM(GREEN3, LED_PWM_10); // 50% brightness for GREEN
    LedPWM(BLUE3, LED_PWM_10);  // 50% brightness for BLUE
}

void Led2SetColorYellow(void) {
    LedOn(RED2);  
    LedOn(GREEN2);
    LedPWM(RED2, LED_PWM_10);  // 50% brightness for RED
    LedPWM(GREEN2, LED_PWM_10); // 50% brightness for GREEN
    LedOff(BLUE2); // Ensure BLUE is off
}

void LedSetColorYellow(void) {
    LedOn(RED3);  
    LedOn(GREEN3);
    LedPWM(RED3, LED_PWM_10);  // 50% brightness for RED
    LedPWM(GREEN3, LED_PWM_10); // 50% brightness for GREEN
    LedOff(BLUE3); // Ensure BLUE is off
}

void LedSetColorGreen(void) {
    LedOff(RED3);  // Ensure RED is off
    LedPWM(GREEN3, LED_PWM_10); // 50% brightness for GREEN
    LedOff(BLUE3);  // Ensure BLUE is off
    
    // Blink GREEN 3 times with 200ms on, 200ms off for 1.5 seconds
    for (int i = 0; i < 3; i++) {
        LedOn(GREEN3);   // Turn on GREEN
        DelayMs(200);   // Wait for 200ms
        LedOff(GREEN3);  // Turn off GREEN
        DelayMs(200);   // Wait for 200ms
    }
}

void LedSetColorRed(void) {
    LedOff(GREEN3);  // Ensure GREEN is off
    LedPWM(RED3, LED_PWM_10); // 50% brightness for RED
    LedOff(BLUE3);  // Ensure BLUE is off
    
    // Blink RED 3 times with 200ms on, 200ms off for 1.5 seconds
    for (int i = 0; i < 3; i++) {
        LedOn(RED3);    // Turn on RED
        DelayMs(200);  // Wait for 200ms
        LedOff(RED3);   // Turn off RED
        DelayMs(200);  // Wait for 200ms
    }
}


void LedFlashBlue(u8 led) {
    LedOn(led);
    LedPWM(led, LED_PWM_10);  // Flash with low brightness (blue light)
    DelayMs(100);             // Flash for 100ms
    LedOff(led);
}

/***********************************************************************************************************************
Helper Functions
***********************************************************************************************************************/
static void ResetCandidatePassword(void) {
    for (u8 i = 0; i < 4; i++) {
        CandidatePassword[i] = 0;
    }
    InputIndex = 0;
}

static void ResetNewPassword(void) {
    for (u8 i = 0; i < 4; i++) {
        NewPassword[i] = 0;
    }
}

/***********************************************************************************************************************
Terible delay made by chat gpt in too deep to fix.
***********************************************************************************************************************/
void DelayMs(u32 timems) {
    volatile u32 count;
    while (timems--) {
        for (count = 0; count < 8000; count++) {
            __asm("nop"); // No-operation instruction for a simple delay
        }
    }
}

/***********************************************************************************************************************
State Machine Functions
***********************************************************************************************************************/
void UserApp1Initialize(void) {
    for (u8 i = 0; i < U8_TOTAL_LEDS; i++) {
        LedOff((LedNameType)i); // Turn off all LEDs
    }
    PWMAudioSetFrequency(BUZZER1, 500);

    /* Indicate locked state */
    LedSetColorYellow(); // LED3 yellow during locked state
    ResetCandidatePassword();

    LedOn(LCD_BL);  // Ensure the LCD backlight is on
    LcdClearScreen();

    // Display welcome messages on the LCD
    PixelAddressType sTestStringLocation = {U8_LCD_SMALL_FONT_LINE0, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8TestString[] = {"------Greetings------"};
    LcdLoadString(au8TestString, LCD_FONT_SMALL, &sTestStringLocation);

    PixelAddressType sTestStringLocation2 = {U8_LCD_SMALL_FONT_LINE3, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8TestString2[] = {"Please Enter Password"};
    LcdLoadString(au8TestString2, LCD_FONT_SMALL, &sTestStringLocation2);

    PixelAddressType sTestStringLocation3 = {U8_LCD_SMALL_FONT_LINE5, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8TestString3[] = {"[button0][button1]"};
    LcdLoadString(au8TestString3, LCD_FONT_SMALL, &sTestStringLocation3);

    /* Initialize state machine */
    UserApp1_pfStateMachine = UserApp1SM_Idle;
}

void userApppasswordset(void) {
    LedSetColorWhite(); // Flash white on LED3 to enter password setting mode
    LcdClearScreen();
    PixelAddressType sSetpasswordstring = {U8_LCD_SMALL_FONT_LINE3, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8Setpasswordstring[] = {"Create a new password"};
    LcdLoadString(au8Setpasswordstring, LCD_FONT_SMALL, &sSetpasswordstring);

    if(InputIndex < 4){
        if (WasButtonPressed(BUTTON0)) {
            CandidatePassword[InputIndex++] = 0;
            PWMAudioOn(BUZZER1);
            LedFlashBlue(BLUE0); // Feedback for Button0 press (blue on LED0)
            ButtonAcknowledge(BUTTON0);
            PWMAudioOff(BUZZER1);
        }

        if (WasButtonPressed(BUTTON1)) {
            CandidatePassword[InputIndex++] = 1;
            PWMAudioOn(BUZZER1);
            LedFlashBlue(BLUE1); // Feedback for Button1 press (blue on LED1)
            ButtonAcknowledge(BUTTON1);
            PWMAudioOff(BUZZER1);
        }
    }
    /* Prevent overflow */
    if (InputIndex >= 4) {
        InputIndex = 4;

        /* Automatically store the new password when 4 inputs are entered */
        for (u8 i = 0; i < InputIndex; i++) {
            Password[i] = CandidatePassword[i];
        }
        SettingPassword = FALSE; // Exit setting mode
        Led2SetColorYellow(); // Flash purple on LED3 to indicate new password has been set

        // Display success message on LCD
        LcdClearScreen();
        PixelAddressType sSetpasswordcreated = {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN};
        u8 au8Setpasswordcreation[] = {"Creation successful,"};
        LcdLoadString(au8Setpasswordcreation, LCD_FONT_SMALL, &sSetpasswordcreated);

        PixelAddressType sSetpasswordcreated1 = {U8_LCD_SMALL_FONT_LINE3, U16_LCD_LEFT_MOST_COLUMN};
        u8 au8Setpasswordcreation1[] = {"Please proceed!"};
        LcdLoadString(au8Setpasswordcreation1, LCD_FONT_SMALL, &sSetpasswordcreated1);

        DelayMs(500);
        LedSetColorYellow(); // Return to locked state (yellow)
        ResetCandidatePassword(); // Reset for next input
        UserApp1_pfStateMachine = UserApp1SM_Idle;
    }
    return;  // Remain in password setting mode until completed
}

void userApppasswordcorrect(void) {
    // Reset after checking
    ResetCandidatePassword();
    // Display success message on LCD

    LcdClearScreen();

    const u8 wifi[U8_LCD_IMAGE_ROW_SIZE_25PX][U8_LCD_IMAGE_COL_BYTES_25PX] = {							
    {0x00, 0x00, 0x00, 0x00},							
    {0x00, 0x00, 0x00, 0x00},							
    {0x00, 0x00, 0x00, 0x00},							
    {0x00, 0x00, 0x00, 0x00},							
    {0x00, 0x00, 0x00, 0x00},							
    {0x00, 0x00, 0x00, 0x00},							
    {0x00, 0x00, 0x00, 0x00},							
    {0x00, 0x00, 0x00, 0x00},							
    {0x00, 0x00, 0x00, 0x00},							
    {0x00, 0x0E, 0x00, 0x00},							
    {0x00, 0x3E, 0x00, 0x00},							
    {0x00, 0xE0, 0x00, 0x00},							
    {0x00, 0x80, 0x01, 0x00},							
    {0x00, 0x0E, 0x03, 0x00},							
    {0x00, 0x18, 0x06, 0x00},							
    {0x00, 0x70, 0x0C, 0x00},							
    {0x00, 0xC0, 0x18, 0x00},							
    {0x00, 0x8E, 0x11, 0x00},							
    {0x00, 0x18, 0x33, 0x00},							
    {0x00, 0x30, 0x22, 0x00},							
    {0x00, 0x66, 0x26, 0x00},							
    {0x00, 0x4C, 0x64, 0x00},							
    {0x00, 0xD8, 0x4C, 0x00},							
    {0x00, 0x92, 0xC8, 0x00},							
    {0x00, 0x96, 0xC8, 0x00}							
    };

    PixelBlockType sWifilogo = {35,100,25,25};
    LcdLoadBitmap(&wifi[0][0], &sWifilogo);							


    PixelAddressType sSuccessLocation = {U8_LCD_SMALL_FONT_LINE3, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8SuccessMessage[] = {"Success,"};
    LcdLoadString(au8SuccessMessage, LCD_FONT_SMALL, &sSuccessLocation);

    PixelAddressType sSuccessLocation1 = {U8_LCD_SMALL_FONT_LINE4, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8SuccessMessage1[] = {"Broadcasting Signal.."};
    LcdLoadString(au8SuccessMessage1, LCD_FONT_SMALL, &sSuccessLocation1);

    LedSetColorGreen(); // Flash green on LED3 to indicate password is correct
    DelayMs(100);  // Visual feedback delay
    LedSetColorYellow(); // Return to locked state

    UserApp1_pfStateMachine = UserAppLevel1sound;
    
}

void userApppasswordincorrect(void) {
    // Reset after checking
    ResetCandidatePassword();
    // Display denied message on LCD
    LcdClearScreen();
    PixelAddressType sDeniedLocation = {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8DeniedMessage[] = {"    Ascess denied, "};
    LcdLoadString(au8DeniedMessage, LCD_FONT_SMALL, &sDeniedLocation);

    LedSetColorRed(); // Flash red on LED3 to indicate password is incorrect
    DelayMs(100);  // Visual feedback delay
    LedSetColorYellow(); // Return to locked state

    ResetGameVariables();
    UserApp1_pfStateMachine = UserApp1SM_Idle;
}
static void UserAppLevel1sound(void) {
    static int noteDurations[] = {
        150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
        150, 150, 150, 200, 200, 200, 1000
    };
    
    static int notes[] = {
        E4, D4S, E4, D4S, E4, B3, D4, C4, A3, C4,
        E4, A3, B3, E4, G4S, B3, C4
    };

    // E4, D4S, E4, D4S, E4, B3, D4, C4, A3, C4, E4, A3, B3, E4, G4S, B3, C4, E4, 
    // E4, D4S, E4, D4S, E4, B3, D4, C4, A3, C4, E4, A3, B3, E4, C4, B3, A3, B3, C4, 
    // D4, E4, G4, F4, E4, D4, F4, E4, D4, C4, E4, D4, C4, B3, E4, D4S, E4, D4S, E4, 
    // B3, D4, C4, A3, C4, E4, A3, B3, E4, G4S, B3, C4, E4, E4, D4S, E4, D4S, E4, B3, 
    // D4, C4, A3, C4, E4, A3, B3, E4, C4, B3, A3

    Lvl1SoundClk++;

    if (state == 0 && Lvl1SoundClk >= timer + noteDurations[noteIndex]) {
        PWMAudioOff(BUZZER1);  // Turn off the buzzer for silence
        state = 1;
        timer = Lvl1SoundClk;

        if (ledState % 3 == 0) {
            LedOn(RED0);
            LedPWM(RED0, LED_PWM_30);  // soft brightness for RED
            LedOn(RED1);
            LedPWM(RED1, LED_PWM_30); 
            LedOn(RED2);
            LedPWM(RED2, LED_PWM_30);

            LedOff(BLUE0);
            LedOff(BLUE1);
            LedOff(BLUE2);

            LedOff(GREEN0);
            LedOff(GREEN1);
            LedOff(GREEN2);

        } else if (ledState % 3 == 1) {
            LedOff(RED0);
            LedOff(RED1);
            LedOff(RED2);

            LedOn(BLUE0);
            LedPWM(BLUE0, LED_PWM_30);
            LedOn(BLUE1);
            LedPWM(BLUE1, LED_PWM_30);
            LedOn(BLUE2);
            LedPWM(BLUE2, LED_PWM_30);

            LedOff(GREEN0);
            LedOff(GREEN1);
            LedOff(GREEN2);
        } else {
            LedOff(RED0);
            LedOff(RED1);
            LedOff(RED2);

            LedOff(BLUE0);
            LedOff(BLUE1);
            LedOff(BLUE2);

            LedOn(GREEN0);
            LedPWM(GREEN0, LED_PWM_30);
            LedOn(GREEN1);
            LedPWM(GREEN1, LED_PWM_30);
            LedOn(GREEN2);
            LedPWM(GREEN2, LED_PWM_30);
        }
        ledState++;
    } 
    else if (state == 1 && Lvl1SoundClk >= timer + silenceDuration) {
        noteIndex++;

        if (noteIndex >= sizeof(notes) / sizeof(notes[0])) {
            // Melody has finished
            LedOff(RED0);
            LedOff(RED1);
            LedOff(RED2);

            LedOff(BLUE0);
            LedOff(BLUE1);
            LedOff(BLUE2);

            LedOff(GREEN0);
            LedOff(GREEN1);
            LedOff(GREEN2);

            PWMAudioOff(BUZZER1);  // Ensure sound is off before transitioning
            ResetGameVariables();  
            UserApp1_pfStateMachine = UserAppGamelevel1;  // Move to level 1
            return;
        }

        PWMAudioSetFrequency(BUZZER1, notes[noteIndex]);
        PWMAudioOn(BUZZER1);

        state = 0;
        timer = Lvl1SoundClk;
    }
}

static void UserAppLevel2sound(void){
    static int noteDurations2[] = {
        150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
        150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
        150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 
        200, 200, 200, 200, 1000
    };
    
    static int notes2[] = {
        E4, D4S, E4, D4S, E4, B3, D4, C4, A3, C4,
        E4, A3, B3, E4, G4S, B3, C4, E4, E4, D4S,
        E4, D4S, E4, B3, D4, C4, A3, C4, E4, A3,
        B3, E4, C4, B3, A3
    };

    Lvl2SoundClk++;

    if (state2 == 0 && Lvl2SoundClk >= timer2 + noteDurations2[noteIndex2]) {
        PWMAudioOff(BUZZER1);  // Turn off the buzzer for silence
        state2 = 1;
        timer2 = Lvl2SoundClk;

        if (ledState2 % 3 == 0) {
            LedOn(RED0);
            LedPWM(RED0, LED_PWM_30);  // soft brightness for RED
            LedOn(RED1);
            LedPWM(RED1, LED_PWM_30); 
            LedOn(RED2);
            LedPWM(RED2, LED_PWM_30);

            LedOff(BLUE0);
            LedOff(BLUE1);
            LedOff(BLUE2);

            LedOff(GREEN0);
            LedOff(GREEN1);
            LedOff(GREEN2);

        } else if (ledState2 % 3 == 1) {
            LedOff(RED0);
            LedOff(RED1);
            LedOff(RED2);

            LedOn(BLUE0);
            LedPWM(BLUE0, LED_PWM_30);
            LedOn(BLUE1);
            LedPWM(BLUE1, LED_PWM_30);
            LedOn(BLUE2);
            LedPWM(BLUE2, LED_PWM_30);

            LedOff(GREEN0);
            LedOff(GREEN1);
            LedOff(GREEN2);
        } else {
            LedOff(RED0);
            LedOff(RED1);
            LedOff(RED2);

            LedOff(BLUE0);
            LedOff(BLUE1);
            LedOff(BLUE2);

            LedOn(GREEN0);
            LedPWM(GREEN0, LED_PWM_30);
            LedOn(GREEN1);
            LedPWM(GREEN1, LED_PWM_30);
            LedOn(GREEN2);
            LedPWM(GREEN2, LED_PWM_30);
        }
        ledState2++;
    } 
    else if (state2 == 1 && Lvl2SoundClk >= timer2 + silenceDuration2) {
        noteIndex2++;

        if (noteIndex2 >= sizeof(notes2) / sizeof(notes2[0])) {
            // Melody has finished
            LedOff(RED0);
            LedOff(RED1);
            LedOff(RED2);

            LedOff(BLUE0);
            LedOff(BLUE1);
            LedOff(BLUE2);

            LedOff(GREEN0);
            LedOff(GREEN1);
            LedOff(GREEN2);

            PWMAudioOff(BUZZER1);  // Ensure sound is off before transitioning
            ResetGameVariables();  
            UserApp1_pfStateMachine = UserAppGamelevel2;  // Move to level 1
            return;
        }

        PWMAudioSetFrequency(BUZZER1, notes2[noteIndex2]);
        PWMAudioOn(BUZZER1);

        state2 = 0;
        timer2 = Lvl2SoundClk;
    }
}


static void UserAppWinnerSound(void){
    static int noteDurations3[] = {
        150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
        150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
        150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
        150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
        150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
        150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
        150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
        150, 150
    };
    
    static int notes3[] = {
        E4, D4S, E4, D4S, E4, B3, D4, C4, A3, C4,
        E4, A3, B3, E4, G4S, B3, C4, E4, E4, D4S,
        E4, D4S, E4, B3, D4, C4, A3, C4, E4, A3,
        B3, E4, C4, B3, A3, B3, C4, D4, E4, G4,
        F4, E4, D4, F4, E4, D4, C4, E4, D4, C4,
        B3, E4, D4S, E4, D4S, E4, B3, D4, C4, A3,
        C4, E4, A3, B3, E4, G4S, B3, C4, E4, E4,
        D4S, E4, D4S, E4, B3, D4, C4, A3, C4, E4,
        A3, B3, E4, C4, B3, A3
    };

    WinSoundClk++;

    if (state3 == 0 && WinSoundClk >= timer3 + noteDurations3[noteIndex3]) {
        PWMAudioOff(BUZZER1);  // Turn off the buzzer for silence
        state3 = 1;
        timer3 = WinSoundClk;

        if (ledState3 % 3 == 0) {
            LedOn(RED0);
            LedPWM(RED0, LED_PWM_30);  // soft brightness for RED
            LedOn(RED1);
            LedPWM(RED1, LED_PWM_30); 
            LedOn(RED2);
            LedPWM(RED2, LED_PWM_30);

            LedOff(BLUE0);
            LedOff(BLUE1);
            LedOff(BLUE2);

            LedOff(GREEN0);
            LedOff(GREEN1);
            LedOff(GREEN2);

        } else if (ledState3 % 3 == 1) {
            LedOff(RED0);
            LedOff(RED1);
            LedOff(RED2);

            LedOn(BLUE0);
            LedPWM(BLUE0, LED_PWM_30);
            LedOn(BLUE1);
            LedPWM(BLUE1, LED_PWM_30);
            LedOn(BLUE2);
            LedPWM(BLUE2, LED_PWM_30);

            LedOff(GREEN0);
            LedOff(GREEN1);
            LedOff(GREEN2);
        } else {
            LedOff(RED0);
            LedOff(RED1);
            LedOff(RED2);

            LedOff(BLUE0);
            LedOff(BLUE1);
            LedOff(BLUE2);

            LedOn(GREEN0);
            LedPWM(GREEN0, LED_PWM_30);
            LedOn(GREEN1);
            LedPWM(GREEN1, LED_PWM_30);
            LedOn(GREEN2);
            LedPWM(GREEN2, LED_PWM_30);
        }
        ledState3++;
    } 
    else if (state3 == 1 && WinSoundClk >= timer3 + silenceDuration3) {
        noteIndex3++;

        if (noteIndex3 >= sizeof(notes3) / sizeof(notes3[0])) {
            // Melody has finished
            LedOff(RED0);
            LedOff(RED1);
            LedOff(RED2);

            LedOff(BLUE0);
            LedOff(BLUE1);
            LedOff(BLUE2);

            LedOff(GREEN0);
            LedOff(GREEN1);
            LedOff(GREEN2);

            PWMAudioOff(BUZZER1);  // Ensure sound is off before transitioning
            ResetGameVariables();  
            UserApp1_pfStateMachine = UserApp1Initialize;  // Move to level 1
            return;
        }

        PWMAudioSetFrequency(BUZZER1, notes3[noteIndex3]);
        PWMAudioOn(BUZZER1);

        state3 = 0;
        timer3 = WinSoundClk;
    }
}

static void UserAppGamelevel1(void) {
    Level1clk++;

    // Display messages based on timing
    if (Level1clk == 500 && clear == 0) {
        LcdClearScreen();
        PixelAddressType sWelcometothegame = {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN};
        u8 au8Welcometothegame[] = {"Welcome to the game!!"};
        LcdLoadString(au8Welcometothegame, LCD_FONT_SMALL, &sWelcometothegame);
        clear = 1;
    }

    if (Level1clk == 1000 && clear == 1) {
        LcdClearScreen();
        PixelAddressType sLevel1 = {U8_LCD_SMALL_FONT_LINE0, U16_LCD_LEFT_MOST_COLUMN};
        u8 au8Level1[] = {"     ---Level 1---"};
        LcdLoadString(au8Level1, LCD_FONT_SMALL, &sLevel1);

        PixelAddressType sLevel1_2 = {U8_LCD_SMALL_FONT_LINE3, U16_LCD_LEFT_MOST_COLUMN};
        u8 au8Level1_2[] = {"Count in uint 0 to 7"};
        LcdLoadString(au8Level1_2, LCD_FONT_SMALL, &sLevel1_2);

        PixelAddressType sLevel1_3 = {U8_LCD_SMALL_FONT_LINE4, U16_LCD_LEFT_MOST_COLUMN};
        u8 au8Level1_3[] = {" Button(0,1) = 0,1"};
        LcdLoadString(au8Level1_3, LCD_FONT_SMALL, &sLevel1_3);
    }

    // Reset clock after displaying messages
    if (Level1clk >= 1000) {
        Level1clk = 0;
    }

    // Handle binary input (0-7)
    if (WasButtonPressed(BUTTON0)) {
        if (InputIndex < 3) {  // Ensure we don't go out of bounds
            Binarycount[InputIndex++] = 0;
            PWMAudioOn(BUZZER1);
            LedFlashBlue(BLUE0); // Feedback for Button0 press
            ButtonAcknowledge(BUTTON0);
            PWMAudioOff(BUZZER1);
        }
    }

    if (WasButtonPressed(BUTTON1)) {
        if (InputIndex < 3) {  // Ensure we don't go out of bounds
            Binarycount[InputIndex++] = 1;
            PWMAudioOn(BUZZER1);
            LedFlashBlue(BLUE1); // Feedback for Button1 press
            ButtonAcknowledge(BUTTON1);
            PWMAudioOff(BUZZER1);
        }
    }

    // When 3 bits are entered, process input
    if (InputIndex >= 3) {
        // Copy user input for verification
        for (int i = 0; i < 3; i++) {
            Binarycountcheck[i] = Binarycount[i];
        }

        // Convert binary array to decimal
        int decimalValue = (Binarycountcheck[0] << 2) | 
                           (Binarycountcheck[1] << 1) | 
                           (Binarycountcheck[2] << 0);

        // Print decimal value on LCD
        // Example action: Clear screen if user inputs "000" (0 in decimal)
        static int i = 0;  // Initialize i within this scope

        if (decimalValue == 0 && i == 0) {
            PixelAddressType sbinarycountnumber0 = {U8_LCD_SMALL_FONT_LINE6, U16_LCD_LEFT_MOST_COLUMN};
            u8 au8sbinarycountnumber0[] = {" 000 = 0... Correct."};
            LcdLoadString(au8sbinarycountnumber0, LCD_FONT_SMALL, &sbinarycountnumber0);
            i++;
        }

        else if (decimalValue == 1 && i == 1) {
            PixelAddressType sbinarycountnumber1 = {U8_LCD_SMALL_FONT_LINE6, U16_LCD_LEFT_MOST_COLUMN};
            u8 au8sbinarycountnumber1[] = {" 001 = 1... Correct."};
            LcdLoadString(au8sbinarycountnumber1, LCD_FONT_SMALL, &sbinarycountnumber1);
            i++;
        }

        else if (decimalValue == 2 && i == 2) {
            PixelAddressType sbinarycountnumber2 = {U8_LCD_SMALL_FONT_LINE6, U16_LCD_LEFT_MOST_COLUMN};
            u8 au8sbinarycountnumber2[] = {" 010 = 2... Correct."};
            LcdLoadString(au8sbinarycountnumber2, LCD_FONT_SMALL, &sbinarycountnumber2);
            i++;
        }

        else if (decimalValue == 3 && i == 3) {
            PixelAddressType sbinarycountnumber3 = {U8_LCD_SMALL_FONT_LINE6, U16_LCD_LEFT_MOST_COLUMN};
            u8 au8sbinarycountnumber3[] = {" 011 = 3... Correct."};
            LcdLoadString(au8sbinarycountnumber3, LCD_FONT_SMALL, &sbinarycountnumber3);
            i++;
        }

        else if (decimalValue == 4 && i == 4) {
            PixelAddressType sbinarycountnumber4 = {U8_LCD_SMALL_FONT_LINE6, U16_LCD_LEFT_MOST_COLUMN};
            u8 au8sbinarycountnumber4[] = {" 100 = 4... Correct."};
            LcdLoadString(au8sbinarycountnumber4, LCD_FONT_SMALL, &sbinarycountnumber4);
            i++;
        }

        else if (decimalValue == 5 && i == 5) {
            PixelAddressType sbinarycountnumber5 = {U8_LCD_SMALL_FONT_LINE6, U16_LCD_LEFT_MOST_COLUMN};
            u8 au8sbinarycountnumber5[] = {" 101 = 5... Correct."};
            LcdLoadString(au8sbinarycountnumber5, LCD_FONT_SMALL, &sbinarycountnumber5);
            i++;
        }

        else if (decimalValue == 6 && i == 6) {
            PixelAddressType sbinarycountnumber6 = {U8_LCD_SMALL_FONT_LINE6, U16_LCD_LEFT_MOST_COLUMN};
            u8 au8sbinarycountnumber6[] = {" 110 = 6... Correct."};
            LcdLoadString(au8sbinarycountnumber6, LCD_FONT_SMALL, &sbinarycountnumber6);
            i++;
        }

        else if (decimalValue == 7 && i == 7) {
            PixelAddressType sbinarycountnumber7 = {U8_LCD_SMALL_FONT_LINE6, U16_LCD_LEFT_MOST_COLUMN};
            u8 au8sbinarycountnumber7[] = {" 111 = 7... Correct."};
            LcdLoadString(au8sbinarycountnumber7, LCD_FONT_SMALL, &sbinarycountnumber7);
            i = 0;
            ResetGameVariables();
            UserApp1_pfStateMachine = UserAppLevel2sound;
        }
        else {
            PixelAddressType sbinarycountfail = {U8_LCD_SMALL_FONT_LINE6, U16_LCD_LEFT_MOST_COLUMN};
            u8 au8sbinarycountfail[] = {" Incorrect!!!."};
            LcdLoadString(au8sbinarycountfail, LCD_FONT_SMALL, &sbinarycountfail);
            i = 0;
            ResetGameVariables();
            UserApp1_pfStateMachine = UserAppEndGame;
        }

        // Reset input index for the next number
        InputIndex = 0;
    }
}


#define RandomNumber() (rand() % 2) // Generates 0 or 1

static void UserAppGamelevel2(void) {
    static u8 GeneratedNumbers[7];  // Stores the random sequence (max 6 digits + null terminator)
    static u8 UserInput[7];         // Stores user input
    // static u8 InputIndex = 0;       // Tracks user input index
    // static u8 GamePhase = 0;        // 0 = Show numbers, 1 = Ready, 2 = Input, 3 = Validate, 4 = Success Confirm
    // static u16 Level2Timer = 0;     // Timer for transitions
    // static u8 DisplayIndex = 0;     // Tracks which number is being displayed
    // static u8 Round = 1;            // Tracks the current round (starts at 1, goes to 6)
    static u8 DisplayBuffer[2];     // Buffer to hold a single digit for `LcdLoadString()`
    static u8 RoundMessage[16];     // Buffer to store round messages

    static const PixelAddressType Positions[6] = {
        {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN + 5},    
        {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN + 20},   
        {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN + 35},   
        {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN + 50},   
        {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN + 65},   
        {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN + 80}    
    };

    Level2Timer++;

    switch (GamePhase) {
        case 0:  // **Show Numbers One by One**
            if (Level2Timer == 1) {
                for (int i = 0; i < Round; i++) {
                    GeneratedNumbers[i] = RandomNumber() + '0';  // Convert to char ('0' or '1')
                }
                GeneratedNumbers[Round] = '\0'; // Null-terminate
                LcdClearScreen();
                DisplayIndex = 0;
            }

            if (Level2Timer % 500 == 0 && DisplayIndex < Round) { 
                DisplayBuffer[0] = GeneratedNumbers[DisplayIndex];
                DisplayBuffer[1] = '\0'; // Null terminate
                LcdLoadString(DisplayBuffer, LCD_FONT_SMALL, &Positions[DisplayIndex]);
                DisplayIndex++;
            }

            if (DisplayIndex >= Round && Level2Timer >= 1000 + (Round * 500)) {  
                LcdClearScreen();
                Level2Timer = 0;
                GamePhase = 1;
            }
            break;

        case 1:  // **Display "READY"**
            if (Level2Timer == 100) {
                PixelAddressType Position = {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN + 20};
                u8 ReadyText[] = "READY, Go!";
                LcdLoadString(ReadyText, LCD_FONT_SMALL, &Position);
            }
            if (Level2Timer == 600) {  
                LcdClearScreen();
                GamePhase = 2;
                Level2Timer = 0;
                InputIndex1 = 0;
            }
            break;

        case 2:  // **User Input Phase**
            if (WasButtonPressed(BUTTON0) && InputIndex1 < Round) {
                UserInput[InputIndex1++] = '0';
                PWMAudioOn(BUZZER1);
                LedFlashBlue(BLUE0);
                ButtonAcknowledge(BUTTON0);
                PWMAudioOff(BUZZER1);
            }

            if (WasButtonPressed(BUTTON1) && InputIndex1 < Round) {
                UserInput[InputIndex1++] = '1';
                PWMAudioOn(BUZZER1);
                LedFlashBlue(BLUE1);
                ButtonAcknowledge(BUTTON1);
                PWMAudioOff(BUZZER1);
            }

            if (InputIndex1 >= Round) {  
                UserInput[Round] = '\0'; // Null terminate
                GamePhase = 3;
                Level2Timer = 0;
            }
            break;

        case 3:  // **Validate Input**
            if (Level2Timer == 100) {
                LcdClearScreen();
                PixelAddressType Position = {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN + 10};

                if (strcmp((char*)UserInput, (char*)GeneratedNumbers) == 0) {
                    if (Round < 6) {
                        sprintf((char*)RoundMessage, "Round %d Cleared!", Round);
                    } else {
                        sprintf((char*)RoundMessage, "Final Round Cleared!");
                    }
                    LcdLoadString(RoundMessage, LCD_FONT_SMALL, &Position);
                    GamePhase = 4; // Move to success confirmation
                } else {
                    u8 IncorrectText[] = "Game Over!";
                    LcdLoadString(IncorrectText, LCD_FONT_SMALL, &Position);
                    GamePhase = 5; // Move to end state
                }
                Level2Timer = 0;
            }
            break;

        case 4:  // **Round Success Confirmation**
            if (Level2Timer == 600) {
                LcdClearScreen();
                if (Round < 6) {  // If not the final round, move to next round
                    Round++;
                    GamePhase = 0;
                } else {
                    GamePhase = 6;  // Move to success state
                }
                Level2Timer = 0;
            }
            break;

        case 5:  // **Game Over - Move to End Game**
            if (Level2Timer == 800) {
            InputIndex1 = 0;       // Tracks user input index
            GamePhase = 0;        // 0 = Show numbers, 1 = Ready, 2 = Input, 3 = Validate, 4 = Success Confirm
            Level2Timer = 0;     // Timer for transitions
            DisplayIndex = 0;     // Tracks which number is being displayed
            Round = 1;

                UserApp1_pfStateMachine = UserAppEndGame;
            }
            break;

        case 6:  // **Victory - Move to Success State**
            if (Level2Timer == 800) {
            InputIndex1 = 0;       // Tracks user input index
            GamePhase = 0;        // 0 = Show numbers, 1 = Ready, 2 = Input, 3 = Validate, 4 = Success Confirm
            Level2Timer = 0;     // Timer for transitions
            DisplayIndex = 0;     // Tracks which number is being displayed
            Round = 1;
                UserApp1_pfStateMachine = userAppWinner;
            }
            break;
    }
}


static void UserAppEndGame(void) {
    Endgameclk++;
    
    if(Endgameclk == 1000){
    LcdClearScreen();
    PixelAddressType sEndgame = {U8_LCD_SMALL_FONT_LINE0, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8Endgame[] = {"  ---GAME OVER---"};

    LcdLoadString(au8Endgame, LCD_FONT_SMALL, &sEndgame);

    const u8 aau8car[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX] = {					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xF7, 0x03},					
    {0xFF, 0xFF, 0x56, 0x7D, 0xFF, 0xE1, 0x03},					
    {0xFF, 0xFF, 0x03, 0x18, 0xDE, 0xF7, 0x03},					
    {0xFF, 0x9F, 0x7B, 0x7F, 0x87, 0xFF, 0x03},					
    {0xFF, 0x9F, 0x78, 0xFF, 0xDF, 0xFF, 0x03},					
    {0x3F, 0x7E, 0xFE, 0xFF, 0xFF, 0xFF, 0x03},					
    {0x8F, 0x7E, 0xFE, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xE7, 0x06, 0x80, 0xFF, 0xFF, 0xFF, 0x03},					
    {0x77, 0xF8, 0x6F, 0xFE, 0xFF, 0xFF, 0x03},					
    {0xA7, 0xFF, 0xEF, 0xFD, 0xFF, 0xFF, 0x03},					
    {0xCF, 0xFF, 0xEF, 0xE1, 0xFF, 0xFF, 0x03},					
    {0xEF, 0xFF, 0x0F, 0xDE, 0xFF, 0xFF, 0x03},					
    {0xEF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0x03},					
    {0xEF, 0xF0, 0x87, 0xBF, 0xFF, 0xFF, 0x03},					
    {0x6F, 0xE0, 0x03, 0xDE, 0xFF, 0xFF, 0x03},					
    {0x1F, 0x06, 0x30, 0xE0, 0xFF, 0xFF, 0x03},					
    {0x7F, 0xEF, 0x7B, 0xFF, 0xFF, 0xFF, 0x03},					
    {0x7F, 0xE6, 0x33, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xF0, 0x87, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03}					
    };

    PixelBlockType car = {14, 15, 50, 50}; // 32, 7
    LcdLoadBitmap(&aau8car[0][0], &car);


    const u8 aau8chopper[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX] = {				
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},				
    {0xFF, 0x1F, 0x38, 0xF0, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xD7, 0xFF, 0xBF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xEF, 0xFF, 0x7F, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xC7, 0xFF, 0x7F, 0xFE, 0x03},				
    {0xFF, 0x1F, 0x28, 0xF0, 0xBF, 0xFD, 0x03},				
    {0xFF, 0xFF, 0xEF, 0xFF, 0xC0, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xEF, 0x7F, 0xDF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xEF, 0xBF, 0xC1, 0xFF, 0x03},				
    {0xFF, 0xFF, 0x01, 0xDE, 0xBE, 0xFD, 0x03},				
    {0xFF, 0xFF, 0xEE, 0x6D, 0x7F, 0xFE, 0x03},				
    {0xFF, 0x7F, 0x17, 0xB2, 0x7F, 0xFF, 0x03},				
    {0xFF, 0x7F, 0xDB, 0xDE, 0xBF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xDC, 0xEE, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0x01, 0xF0, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xDB, 0xFA, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xDF, 0xFE, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0x1F, 0xFE, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xDF, 0xFE, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xBF, 0xFD, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xBF, 0xFD, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0x3F, 0xFC, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xBF, 0xFD, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0x7F, 0xFB, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0x7F, 0xFB, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0x7F, 0xF8, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0x7F, 0xFB, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0x7F, 0xFB, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xFF, 0xF6, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xFF, 0xF6, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xFF, 0xF6, 0xFF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xFF, 0xED, 0xFF, 0xFF, 0x03},				
    {0xBF, 0xFF, 0xFD, 0xE1, 0xFF, 0xC7, 0x03},				
    {0x0F, 0x7F, 0xF8, 0xED, 0xFF, 0xBB, 0x03},				
    {0xBF, 0xFF, 0xFD, 0xED, 0xFF, 0x45, 0x03},				
    {0xFF, 0xDF, 0xFF, 0xFF, 0xAB, 0xD6, 0x02},				
    {0x7F, 0x87, 0xEF, 0xFF, 0x47, 0xC7, 0x01},				
    {0x1F, 0xDE, 0xC3, 0xFF, 0xAF, 0xFF, 0x03},				
    {0x7F, 0xFF, 0xEF, 0xFD, 0xAF, 0x00, 0x02},				
    {0xFF, 0xFF, 0xFF, 0xCB, 0xAF, 0xAA, 0x02},				
    {0xFF, 0xFF, 0x7F, 0xB7, 0x8F, 0x7D, 0x03},				
    {0xFF, 0xFF, 0xFF, 0x4A, 0xBF, 0x55, 0x03},				
    {0xFF, 0xFF, 0xFF, 0xB1, 0xBF, 0x01, 0x03},				
    {0xFF, 0xFF, 0xFF, 0x87, 0xBF, 0xFF, 0x03},				
    {0xFF, 0xFF, 0xFF, 0xDF, 0x3F, 0x00, 0x00},				
    {0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xE7, 0x03},				
    {0xFF, 0xFF, 0xFF, 0xAF, 0xFF, 0xDB, 0x03},				
    {0xFF, 0xFF, 0xFF, 0x6F, 0x7F, 0x3C, 0x02}				
    };									

    PixelBlockType chopper = {14, 60, 50, 50};
    LcdLoadBitmap(&aau8chopper[0][0], &chopper);

    Buzz = 0;
    }

    // Play defeat sound sequence
    if (Endgameclk == 1100) {  
        PWMAudioSetFrequency(BUZZER1, D4S);
        PWMAudioOn(BUZZER1);
        Buzz = 1;
    } 
    else if (Endgameclk == 1600 && Buzz == 1) {  
        PWMAudioOff(BUZZER1); // Stop previous note
        PWMAudioSetFrequency(BUZZER1, C4);
        PWMAudioOn(BUZZER1);
        Buzz = 2;
    } 
    else if (Endgameclk == 2100 && Buzz == 2) {  
        PWMAudioOff(BUZZER1);
        PWMAudioSetFrequency(BUZZER1, F3S);
        PWMAudioOn(BUZZER1);
        Buzz = 3;
    }
    else if (Endgameclk == 4400 && Buzz == 3) {  
        PWMAudioOff(BUZZER1); // Stop final sound
    }

    if(Endgameclk == 4600){
        UserApp1_pfStateMachine = UserApp1Initialize;
    }
}

static void userAppWinner(void) {
    winclk++;
    PixelAddressType sWinner = {U8_LCD_SMALL_FONT_LINE0, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8Winner[] = {"   WINNER!!!"};
    LcdLoadString(au8Winner, LCD_FONT_SMALL , &sWinner);


    const u8 aau8victoryroyale[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX] = {						
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0xB8, 0x07, 0x00, 0x00},
        {0x00, 0x00, 0xFE, 0xBF, 0xFF, 0x01, 0x00},
        {0x00, 0x80, 0xE3, 0xB8, 0x03, 0x00, 0x00},
        {0x00, 0x80, 0xF1, 0xB0, 0xCB, 0x01, 0x00},
        {0x00, 0xC0, 0xF0, 0xB0, 0x29, 0x01, 0x00},
        {0x00, 0xE0, 0xCA, 0xE0, 0x29, 0x00, 0x00},
        {0x00, 0x30, 0xCA, 0xE0, 0xE8, 0x01, 0x00},
        {0x00, 0x38, 0xDF, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x1C, 0xCA, 0x00, 0xC0, 0x01, 0x00},
        {0x00, 0x1E, 0xDF, 0x00, 0xC0, 0x01, 0x00},
        {0x00, 0x0F, 0xCA, 0x00, 0x40, 0x00, 0x00},
        {0x80, 0x07, 0xCA, 0x00, 0xC0, 0x01, 0x00},
        {0xC0, 0x01, 0xC0, 0x00, 0xC0, 0x00, 0x00},
        {0x60, 0x0C, 0x00, 0x00, 0xC0, 0x00, 0x00},
        {0x70, 0x07, 0x00, 0x00, 0xC0, 0x00, 0x00},
        {0xF8, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00},
        {0xCC, 0x01, 0xC0, 0x01, 0x00, 0x00, 0x00},
        {0xEE, 0xFF, 0x7F, 0xFF, 0xFF, 0x01, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
        };
        

    // Now display it on the LCD
    PixelBlockType victoryRoyale = {14, 0, 50, 50};  // Define position and size
    LcdLoadBitmap(&aau8victoryroyale[0][0], &victoryRoyale);

    const u8 aau8victoryroyale1[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX] = {						
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0xFF, 0xFF, 0x9F, 0xFF, 0xFF, 0x07, 0x00},
        {0x00, 0x00, 0x60, 0x00, 0x00, 0x06, 0x00},
        {0x6E, 0x4E, 0x13, 0x00, 0x80, 0x03, 0x00},
        {0x94, 0x52, 0x01, 0x00, 0x80, 0x01, 0x00},
        {0x94, 0x8E, 0x00, 0x00, 0xC0, 0x00, 0x00},
        {0x64, 0x9A, 0x00, 0x00, 0x60, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00},
        {0x01, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00},
        {0x03, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00},
        {0x33, 0xD1, 0x89, 0x03, 0x06, 0x00, 0x00},
        {0x4B, 0x5B, 0x89, 0x00, 0x03, 0x00, 0x00},
        {0x49, 0x4E, 0x89, 0x81, 0x01, 0x00, 0x00},
        {0x4B, 0xC4, 0x89, 0xC0, 0x00, 0x00, 0x00},
        {0x33, 0x64, 0xBB, 0x63, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00},
        {0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
        };

    
        // Now display it on the LCD
        PixelBlockType victoryRoyale1 = {14, 41, 50, 50};  // Define position and size
        LcdLoadBitmap(&aau8victoryroyale1[0][0], &victoryRoyale1);
    
    if(winclk == 100){
        UserApp1_pfStateMachine = UserAppWinnerSound;
    }
}

void UserApp1RunActiveState(void) {
    UserApp1_pfStateMachine();
}

static void UserApp1SM_Idle(void) {
    /* Handle password-setting initiation */
    if (IsButtonHeld(BUTTON0, 3000) && !SettingPassword) {
        ResetCandidatePassword();
        UserApp1_pfStateMachine = userApppasswordset;
        return;
    }
    
    /* Handle password input in locked state */
    if (InputIndex < 4) {
        if (WasButtonPressed(BUTTON0)) {
            CandidatePassword[InputIndex++] = 0;
            PWMAudioOn(BUZZER1);
            LedFlashBlue(BLUE0); // Feedback for Button0 press (blue on LED0)
            ButtonAcknowledge(BUTTON0);
            PWMAudioOff(BUZZER1);
        }

        if (WasButtonPressed(BUTTON1)) {
            CandidatePassword[InputIndex++] = 1;
            PWMAudioOn(BUZZER1);
            LedFlashBlue(BLUE1); // Feedback for Button1 press (blue on LED1)
            ButtonAcknowledge(BUTTON1);
            PWMAudioOff(BUZZER1);
        }
    }

    /* If 4 digits entered, check password */
    if (InputIndex == 4) {
        bool Match = TRUE;

        /* Check password values */
        for (u8 i = 0; i < 4; i++) {
            if (CandidatePassword[i] != Password[i]) {
                Match = FALSE;
                break;
            }
        }

        /* Feedback for match or mismatch */
        if (Match) {
            ResetGameVariables();
            UserApp1_pfStateMachine =  userApppasswordcorrect;
        } else {
            ResetGameVariables();
            userApppasswordincorrect();
            //userApppasswordincorrect;
        }
    }
}

/*************  ✨ Codeium Command ⭐  *************/
/**
 * @brief Handles error state in UserApp1 state machine.
 *
 * This function is called when the state machine encounters an error.
 * It should implement measures to handle errors, such as logging or
 * entering a safe default state.
 */

/******  a00add2d-8258-4973-accd-cb745a6c0e40  *******/static void UserApp1SM_Error(void) {
    /* Error handling state */
}
