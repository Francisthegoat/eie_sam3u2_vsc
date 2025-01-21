#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */

/* Existing variables */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
static u8 Password[] = {0, 1, 1, 0};                      // Default password
static u8 CandidatePassword[10];                         // User-entered password
static u8 PasswordLength = 4;                            // Default password length
static u8 InputIndex = 0;                                // Track user input index
static bool InSettingMode = FALSE;                       // Track if setting mode is active
static bool SettingPassword = FALSE;                     // Flag to track password-setting state

/**********************************************************************************************************************
LED Color Mixing Functions
***********************************************************************************************************************/
void LedSetColorYellow(void) {
    LedOn(RED3);  
    LedOn(GREEN3);
    LedPWM(RED3, LED_PWM_10);  // 50% brightness
    LedPWM(GREEN3, LED_PWM_10); // 50% brightness
    LedOff(BLUE3);
}

void LedSetColorGreen(void) {
    LedOff(RED3);
    LedOn(GREEN3);
    LedPWM(GREEN3, LED_PWM_10); // 50% brightness
    LedOff(BLUE3);
}

void LedSetColorRed(void) {
    LedOn(RED3);
    LedPWM(RED3, LED_PWM_10); // 50% brightness
    LedOff(GREEN3);
    LedOff(BLUE3);
}

void LedSetColorWhite(void) {
    LedOn(RED3);
    LedOn(GREEN3);
    LedOn(BLUE3);
    LedPWM(RED3, LED_PWM_10);  // 50% brightness
    LedPWM(GREEN3, LED_PWM_10); // 50% brightness
    LedPWM(BLUE3, LED_PWM_10);  // 50% brightness
}

// void LedSetColorBlue(LedNameType led) {
//     LedOn(led);
//     LedPWM(led, LED_PWM_10); // 50% brightness
// }

void DelayMs(u32 ms) {
    volatile u32 count;
    while (ms--) {
        for (count = 0; count < 8000; count++) {
            __asm("nop"); // No-operation instruction
        }
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                             
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                             
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)
/hello world

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void) {
    for (u8 i = 0; i < U8_TOTAL_LEDS; i++) {
        LedOff((LedNameType)i); // Turn off all LEDs
    }

    /* Indicate locked state */
    LedSetColorYellow(); // LED3 yellow during locked state

    /* Initialize state machine */
    UserApp1_pfStateMachine = UserApp1SM_Idle;
    LedOn(LCD_BL);
    LcdClearScreen();
}
  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                             
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* Button module exercise */
static void UserApp1SM_Idle(void) {
    /* Handle password-setting initiation */
    if (IsButtonHeld(BUTTON0, 3000) && !SettingPassword) {
        SettingPassword = TRUE;
        InputIndex = 0;
        LedSetColorWhite(); // Indicate password-setting mode
        return;
    }

    /* Handle password input during setting mode */
    if (SettingPassword) {
        if (WasButtonPressed(BUTTON0)) {
            CandidatePassword[InputIndex++] = 0; //sets 0 into the candidate password array
            ButtonAcknowledge(BUTTON0);
        }

        if (WasButtonPressed(BUTTON1)) {
            CandidatePassword[InputIndex++] = 1;  //sets 1 into the candidate password array
            ButtonAcknowledge(BUTTON1);
        }

        if (InputIndex >= 10) {
            InputIndex = 10; // Prevent overflow
        }

        /* Finish password-setting when BUTTON0 is held again for 3 seconds */
        if (IsButtonHeld(BUTTON0, 3000)) {
            for (u8 i = 0; i < InputIndex; i++) {
                Password[i] = CandidatePassword[i];
            }
            PasswordLength = InputIndex;
            SettingPassword = FALSE; // Exit setting mode
            LedSetColorYellow(); // Return to locked state
        }
        return;
    }

    /* Handle normal password input in locked state */
    if (WasButtonPressed(BUTTON0)) {
        CandidatePassword[InputIndex++] = 0; // Store BUTTON0 press and increment InputIndex
        LedOn(BLUE0);
        LedPWM(BLUE0, LED_PWM_10);
        LedOff(BLUE1);
        ButtonAcknowledge(BUTTON0);         // Clear the button press state
    }

    if (WasButtonPressed(BUTTON1)) {
        CandidatePassword[InputIndex++] = 1; // Store BUTTON1 press and increment InputIndex
        LedOn(BLUE1);
        LedPWM(BLUE1, LED_PWM_10);
        LedOff(BLUE0);
        ButtonAcknowledge(BUTTON1);         // Clear the button press state
    }

    /* Prevent overflow (limit inputs to a maximum of 10) */
    if (InputIndex >= 10) {
        InputIndex = 10; // Cap InputIndex at 10 to avoid out-of-bounds errors
    }

    /* Password Verification */
    if (IsButtonHeld(BUTTON0, 500) && IsButtonHeld(BUTTON1, 500)) {
        bool Match = TRUE;

        // Check password length and values
        if (InputIndex == PasswordLength) {
            for (u8 i = 0; i < PasswordLength; i++) {
                if (CandidatePassword[i] != Password[i]) {
                    Match = FALSE;
                    break;
                }
            }
        }

        if (Match) {
            // Blink green at 1Hz for 3 seconds
            for (u16 i = 0; i < 6000; i += LED_1HZ) {
                LedToggle(GREEN3);
                DelayMs(LED_1HZ);
            }
            LedSetColorGreen(); // Keep LED green after success

            // Display success text on the LCD screen
            LcdClearScreen();
            LcdLoadString(Lcd_au8MessageWelcome, LCD_FONT_SMALL, &sStringLocation);
        } else {
            // Blink red at 1Hz for 3 seconds, then return to yellow
            for (u16 i = 0; i < 6000; i += LED_1HZ) {
                LedToggle(RED3);
                DelayMs(LED_1HZ);
            }
            LedSetColorYellow(); // Return to yellow after failure

            // Display failure text on the LCD screen
            LcdClearScreen();
            LcdPrintString("Access Denied", 0, 0);
        }

        // Reset input for the next attempt
        InputIndex = 0;
    }
}

static void UserApp1SM_Error(void) {
    // Error handling state
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
