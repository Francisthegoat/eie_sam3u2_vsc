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
static u8 Password[10] = {0, 1, 1, 0};     // Default password
static u8 CandidatePassword[10];          // User-entered password
static u8 PasswordLength = 4;             // Default password length
static u8 InputIndex = 0;                 // Track user input index
static bool SettingPassword = FALSE;      // Flag to track password-setting state

/***********************************************************************************************************************
LED Control Functions
***********************************************************************************************************************/
void LedSetColorYellow(void) {
    LedOn(RED3);
    LedOn(GREEN3);
    LedPWM(RED3, LED_PWM_50);   // 50% brightness
    LedPWM(GREEN3, LED_PWM_50); // 50% brightness
    LedOff(BLUE3);
}

void LedSetColorGreen(void) {
    LedOff(RED3);
    LedOn(GREEN3);
    LedPWM(GREEN3, LED_PWM_50); // 50% brightness
    LedOff(BLUE3);
}

void LedSetColorRed(void) {
    LedOn(RED3);
    LedPWM(RED3, LED_PWM_50); // 50% brightness
    LedOff(GREEN3);
    LedOff(BLUE3);
}

void LedSetColorWhite(void) {
    LedOn(RED3);
    LedOn(GREEN3);
    LedOn(BLUE3);
    LedPWM(RED3, LED_PWM_50);   // 50% brightness
    LedPWM(GREEN3, LED_PWM_50); // 50% brightness
    LedPWM(BLUE3, LED_PWM_50);  // 50% brightness
}

void LedFeedbackButton0(void) {
    LedOn(BLUE0);
    LedPWM(BLUE0, LED_PWM_50); // Feedback for BUTTON0
    DelayMs(100);
    LedOff(BLUE0);
}

void LedFeedbackButton1(void) {
    LedOn(BLUE1);
    LedPWM(BLUE1, LED_PWM_50); // Feedback for BUTTON1
    DelayMs(100);
    LedOff(BLUE1);
}

void DelayMs(u32 ms) {
    volatile u32 count;
    while (ms--) {
        for (count = 0; count < 8000; count++) {
            __asm("nop"); // No-operation instruction
        }
    }
}

/***********************************************************************************************************************
Helper Functions
***********************************************************************************************************************/
static void ResetCandidatePassword(void) {
    for (u8 i = 0; i < 10; i++) {
        CandidatePassword[i] = 0;
    }
    InputIndex = 0;
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
        LedSetColorWhite(); // Indicate password-setting mode
        return;
    }

    /* Handle password input during setting mode */
    if (SettingPassword) {
        if (WasButtonPressed(BUTTON0)) {
            CandidatePassword[InputIndex++] = 0;
            LedFeedbackButton0();
            ButtonAcknowledge(BUTTON0);
        }

        if (WasButtonPressed(BUTTON1)) {
            CandidatePassword[InputIndex++] = 1;
            LedFeedbackButton1();
            ButtonAcknowledge(BUTTON1);
        }

        /* Prevent overflow */
        if (InputIndex >= 10) {
            InputIndex = 10;
        }

        /* Finish password-setting when BUTTON0 is held again for 3 seconds */
        if (IsButtonHeld(BUTTON0, 3000)) {
            for (u8 i = 0; i < InputIndex; i++) {
                Password[i] = CandidatePassword[i];
            }
            PasswordLength = InputIndex;
            SettingPassword = FALSE;
            LedSetColorYellow(); // Return to locked state
        }
        return;
    }

    /* Handle normal password input in locked state */
    if (WasButtonPressed(BUTTON0)) {
        CandidatePassword[InputIndex++] = 0;
        LedFeedbackButton0();
        ButtonAcknowledge(BUTTON0);
    }

    if (WasButtonPressed(BUTTON1)) {
        CandidatePassword[InputIndex++] = 1;
        LedFeedbackButton1();
        ButtonAcknowledge(BUTTON1);
    }

    /* Prevent overflow */
    if (InputIndex >= 10) {
        InputIndex = 10;
    }

    /* Password verification */
    if (IsButtonHeld(BUTTON0, 750) && IsButtonHeld(BUTTON1, 750)) {
        bool Match = TRUE;

        /* Check password length */
        if (InputIndex != PasswordLength) {
            Match = FALSE;
        } else {
            /* Check password values */
            for (u8 i = 0; i < PasswordLength; i++) {
                if (CandidatePassword[i] != Password[i]) {
                    Match = FALSE;
                    break;
                }
            }
        }

        /* Feedback for match or mismatch */
        if (Match) {
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

static void UserApp1SM_Error(void) {
    /* Error handling state */
}
