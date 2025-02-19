#include "configuration.h"
void DelayMs(u32 timems);


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
//static level2clk ;                     // Track level 2 clock count
//static Endgameclk ;                     // Track endgame clock count

/***********************************************************************************************************************
LED Control Functions
***********************************************************************************************************************/
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
Delay Function
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

    /* Indicate locked state */
    LedSetColorYellow(); // LED3 yellow during locked state
    ResetCandidatePassword();

    LedOn(LCD_BL);  // Ensure the LCD backlight is on
    LcdClearScreen();

    const u8 ATCO1[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX] = {								
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
{0x4C, 0xC1, 0x01, 0x00, 0x60, 0x00, 0x00},
{0x54, 0x49, 0x00, 0x00, 0x14, 0x00, 0x00},
{0x8C, 0xC1, 0x4E, 0x8C, 0x21, 0x00, 0x00},
{0x14, 0x49, 0xA2, 0x54, 0x44, 0x00, 0x00},
{0x8C, 0x41, 0x62, 0xD5, 0x35, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00},
{0x00, 0x86, 0x7F, 0x0E, 0x1E, 0xA4, 0x00},
{0x00, 0xC9, 0x3F, 0x3F, 0x33, 0x54, 0x01},
{0x80, 0x19, 0x8E, 0xB1, 0x61, 0x00, 0x00},
{0xC0, 0x10, 0x8E, 0x81, 0x40, 0x00, 0x00},
{0xC0, 0x3F, 0x8E, 0xA1, 0x61, 0x00, 0x00},
{0xE0, 0x70, 0x8E, 0x31, 0x33, 0x00, 0x00},
{0x70, 0xF0, 0x0E, 0x1F, 0x1E, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00},
{0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};								


    PixelBlockType sAtcoLogo = {13,60,50,50};
    LcdLoadBitmap(&ATCO1[0][0], &sAtcoLogo);


    // Display welcome messages on the LCD
    PixelAddressType sTestStringLocation = {U8_LCD_SMALL_FONT_LINE0, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8TestString[] = {"Hello, ATCO Talent"};
    LcdLoadString(au8TestString, LCD_FONT_SMALL, &sTestStringLocation);

    PixelAddressType sTestStringLocation1 = {U8_LCD_SMALL_FONT_LINE1, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8TestString1[] = {"Acquisition Team,"};
    LcdLoadString(au8TestString1, LCD_FONT_SMALL, &sTestStringLocation1);

    PixelAddressType sTestStringLocation2 = {U8_LCD_SMALL_FONT_LINE3, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8TestString2[] = {"Please enjoy! FT ANT."};
    LcdLoadString(au8TestString2, LCD_FONT_SMALL, &sTestStringLocation2);

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
            LedFlashBlue(BLUE0); // Feedback for Button0 press (blue on LED0)
            ButtonAcknowledge(BUTTON0);
        }

        if (WasButtonPressed(BUTTON1)) {
            CandidatePassword[InputIndex++] = 1;
            LedFlashBlue(BLUE1); // Feedback for Button1 press (blue on LED1)
            ButtonAcknowledge(BUTTON1);
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

        DelayMs(500);  // Delay for visual effect
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

    const u8 Atco2[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX] = {
    {0x4C, 0xC1, 0x01, 0x00, 0x60, 0x00, 0x00},
    {0x54, 0x49, 0x00, 0x00, 0x14, 0x00, 0x00},
    {0x8C, 0xC1, 0x4E, 0x8C, 0x21, 0x00, 0x00},
    {0x14, 0x49, 0xA2, 0x54, 0x44, 0x00, 0x00},
    {0x8C, 0x41, 0x62, 0xD5, 0x35, 0x00, 0x00},
    {0x00, 0x86, 0x7F, 0x0E, 0x1E, 0xA4, 0x00},
    {0x00, 0xC9, 0x3F, 0x3F, 0x33, 0x54, 0x01},
    {0x80, 0x19, 0x8E, 0xB1, 0x61, 0x00, 0x00},
    {0xC0, 0x10, 0x8E, 0x81, 0x40, 0x00, 0x00},
    {0xC0, 0x3F, 0x8E, 0xA1, 0x61, 0x00, 0x00},
    {0xE0, 0x70, 0x8E, 0x31, 0x33, 0x00, 0x00},
    {0x70, 0xF0, 0x0E, 0x1F, 0x1E, 0x00, 0x00},
    {0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00},
    {0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00},
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


                        
    PixelBlockType sAtcoLogo1 = {0,80,50,50};
    LcdLoadBitmap(&Atco2[0][0], &sAtcoLogo1);

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

    UserApp1_pfStateMachine = UserAppGamelevel1; // enter level 1.
}

void userApppasswordincorrect(void) {
    // Reset after checking
    ResetCandidatePassword();
    // Display denied message on LCD
    LcdClearScreen();
    PixelAddressType sDeniedLocation = {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8DeniedMessage[] = {"    Ascess denied, "};
    LcdLoadString(au8DeniedMessage, LCD_FONT_SMALL, &sDeniedLocation);

    PixelAddressType sDeniedLocation1 = {U8_LCD_SMALL_FONT_LINE3, U16_LCD_LEFT_MOST_COLUMN};
    u8 au8DeniedMessage1[] = {"    Ip Flagged !!!"};
    LcdLoadString(au8DeniedMessage1, LCD_FONT_SMALL, &sDeniedLocation1);

    LedSetColorRed(); // Flash red on LED3 to indicate password is incorrect
    DelayMs(100);  // Visual feedback delay
    LedSetColorYellow(); // Return to locked state
}

static void UserAppGamelevel1(void) {
    Level1clk++;

    // Display messages based on timing
    if (Level1clk == 3000 && clear == 0) {
        LcdClearScreen();
        PixelAddressType sWelcometothegame = {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN};
        u8 au8Welcometothegame[] = {"Welcome to the game!!"};
        LcdLoadString(au8Welcometothegame, LCD_FONT_SMALL, &sWelcometothegame);
        clear = 1;
    }

    if (Level1clk == 6000 && clear == 1) {
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
    if (Level1clk >= 6000) {
        Level1clk = 0;
    }

    // Handle binary input (0-7)
    if (WasButtonPressed(BUTTON0)) {
        if (InputIndex < 3) {  // Ensure we don't go out of bounds
            Binarycount[InputIndex++] = 0;
            LedFlashBlue(BLUE0); // Feedback for Button0 press
            ButtonAcknowledge(BUTTON0);
        }
    }

    if (WasButtonPressed(BUTTON1)) {
        if (InputIndex < 3) {  // Ensure we don't go out of bounds
            Binarycount[InputIndex++] = 1;
            LedFlashBlue(BLUE1); // Feedback for Button1 press
            ButtonAcknowledge(BUTTON1);
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
        int i = 0;
        if (decimalValue == 0 && i == 0) {
            PixelAddressType sbinarycountnumber0 = {U8_LCD_SMALL_FONT_LINE6, U16_LCD_LEFT_MOST_COLUMN};
            u8 au8sbinarycountnumber0[] = {" 000 = 0... Correct."};
            LcdLoadString(au8sbinarycountnumber0, LCD_FONT_SMALL, &sbinarycountnumber0);
        }

        // Reset input index for the next number
        InputIndex = 0;
    }
}

// static void UserAppGamelevel2(void) {

// }

// static void UserAppEndGame(void) {

// }

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
            LedFlashBlue(BLUE0); // Feedback for Button0 press (blue on LED0)
            ButtonAcknowledge(BUTTON0);
        }

        if (WasButtonPressed(BUTTON1)) {
            CandidatePassword[InputIndex++] = 1;
            LedFlashBlue(BLUE1); // Feedback for Button1 press (blue on LED1)
            ButtonAcknowledge(BUTTON1);
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
            UserApp1_pfStateMachine =  userApppasswordcorrect;
        } else {
            userApppasswordincorrect();
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
