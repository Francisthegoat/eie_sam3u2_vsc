#include "configuration.h"

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
static u8 InputIndex = 0;                 // Track user input index
static bool SettingPassword = FALSE;      // Flag to track password-setting state

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

void LedSetColorPurple(void) {
    LedOn(RED3);  
    LedOn(GREEN3);
    LedPWM(RED3, LED_PWM_10);  // 50% brightness for RED
    LedPWM(GREEN3, LED_PWM_10); // 50% brightness for GREEN
    LedOff(BLUE3); // Ensure BLUE is off
}

void LedSetColorYellow(void) {
    LedOn(RED3);  
    LedOn(GREEN3);
    LedPWM(RED3, LED_PWM_10);  // 50% brightness for RED
    LedPWM(GREEN3, LED_PWM_10); // 50% brightness for GREEN
    LedOff(BLUE3); // Ensure BLUE is off
}

void LedSetColorGreen(void) {
    LedOff(RED3); // Ensure RED is off
    LedOn(GREEN3);
    LedPWM(GREEN3, LED_PWM_10); // 50% brightness for GREEN
    LedOff(BLUE3); // Ensure BLUE is off
}

void LedSetColorRed(void) {
    LedOn(RED3);
    LedPWM(RED3, LED_PWM_10); // 50% brightness for RED
    LedOff(GREEN3); // Ensure GREEN is off
    LedOff(BLUE3); // Ensure BLUE is off
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

/***********************************************************************************************************************
Delay Function
***********************************************************************************************************************/
void DelayMs(u32 ms) {
    volatile u32 count;
    while (ms--) {
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

    const u8 aau8EngenuicsLogoBlackQ1[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX] = {								
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
    LcdLoadBitmap(&aau8EngenuicsLogoBlackQ1[0][0], &sAtcoLogo);


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

void UserApp1RunActiveState(void) {
    UserApp1_pfStateMachine();
}

static void UserApp1SM_Idle(void) {
    /* Handle password-setting initiation */
    if (IsButtonHeld(BUTTON0, 3000) && !SettingPassword) {
        SettingPassword = TRUE;
        ResetCandidatePassword();
        LedSetColorWhite(); // Flash white on LED3 to enter password setting mode
        return;
    }

    /* Handle password input during setting mode */
    if (SettingPassword) {
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

        /* Prevent overflow */
        if (InputIndex >= 4) {
            InputIndex = 4;
        }

        /* Finish password-setting when BUTTON0 is held again for 3 seconds */
        if (IsButtonHeld(BUTTON0, 3000)) {
            for (u8 i = 0; i < InputIndex; i++) {
                Password[i] = CandidatePassword[i];
            }
            SettingPassword = FALSE;
            LedSetColorPurple(); // Flash purple on LED3 to indicate new password has been set
        }
        return;
    }

    /* Handle normal password input in locked state */
    if (WasButtonPressed(BUTTON0)) {
        CandidatePassword[InputIndex++] = 0;
        LedSetColorGreen(); // Feedback for button 0 press
        ButtonAcknowledge(BUTTON0);
    }

    if (WasButtonPressed(BUTTON1)) {
        CandidatePassword[InputIndex++] = 1;
        LedSetColorRed(); // Feedback for button 1 press
        ButtonAcknowledge(BUTTON1);
    }

    /* Prevent overflow */
    if (InputIndex >= 4) {
        InputIndex = 4;
    }

    /* Automatic password verification after 4 inputs */
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
        if (Match && !(CandidatePassword[0] == 0 && CandidatePassword[1] == 0 && CandidatePassword[2] == 0 && CandidatePassword[3] == 0)) {
            // Ensure [0,0,0,0] is never considered valid
            for (u8 i = 0; i < 6; i++) {
                LedToggle(GREEN3);
                DelayMs(500);
            }
            LedSetColorGreen();
        } else {
            for (u8 i = 0; i < 6; i++) {
                LedToggle(RED3);
                DelayMs(500);
            }
            LedSetColorYellow();
        }

        ResetCandidatePassword();
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
