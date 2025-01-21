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
static bool SettingPassword = FALSE;                     // Flag to track password-setting state

/**********************************************************************************************************************
LED Color Mixing Functions
***********************************************************************************************************************/
// Your LED color functions...

// Debounce function for buttons
#define DEBOUNCE_DELAY_MS 50 // Adjust for your system
bool WasButtonPressedWithDebounce(ButtonNameType button) {   // Fixed type name
    static u32 lastPressTime[U8_TOTAL_BUTTONS] = {0}; // Store the last press time for each button
    if (WasButtonPressed(button)) {
        u32 currentTime = G_u32SystemTime1ms;
        if ((currentTime - lastPressTime[button]) > DEBOUNCE_DELAY_MS) {
            lastPressTime[button] = currentTime;
            return TRUE;  // Button press is valid
        }
    }
    return FALSE;  // Ignore if the debounce time hasn't passed
}

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

void FlashYellowLED(void) {
    LedSetColorYellow();
    DelayMs(500);  // Flash duration
    LedOff(RED3);
    LedOff(GREEN3);
    LedOff(BLUE3); // LED reset
    DelayMs(500);  // Flash interval
}

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

    /* Flash yellow to indicate initialization */
    FlashYellowLED();  // Flash yellow during initialization

    /* Indicate locked state */
    LedSetColorYellow(); // LED3 yellow during locked state

    /* Initialize state machine */
    UserApp1_pfStateMachine = UserApp1SM_Idle;
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
static void UserApp1SM_Error(void) {
    // Error handling state
    u8 au8ErrorString[] = {"ERROR: Invalid Input!"};
    PixelAddressType sErrorStringLocation = {U8_LCD_SMALL_FONT_LINE0, U16_LCD_LEFT_MOST_COLUMN};
    LcdClearScreen(); // Clear the LCD before displaying new message
    LCD_BACKLIGHT_ON(); // Ensure backlight is on
    LcdLoadString(au8ErrorString, LCD_FONT_SMALL, &sErrorStringLocation);

    // Blink red to indicate an error
    LedSetColorRed(); // Red LED for error
    DelayMs(1000);    // Hold the red LED for a second
    LedSetColorYellow(); // Reset to normal state
}

static void UserApp1SM_Idle(void) {
    /* Handle password-setting initiation */
    if (IsButtonHeld(BUTTON0, 3000) && !SettingPassword) {
        SettingPassword = TRUE;
        InputIndex = 0;
        LedSetColorWhite(); // Indicate password-setting mode
        u8 au8SettingPassword[] = {"Setting Password..."};
        PixelAddressType sSettingPasswordLocation = {U8_LCD_SMALL_FONT_LINE0, U16_LCD_LEFT_MOST_COLUMN};
        LcdClearScreen(); // Clear the LCD before displaying new message
        LCD_BACKLIGHT_ON(); // Ensure backlight is on
        LcdLoadString(au8SettingPassword, LCD_FONT_SMALL, &sSettingPasswordLocation);
        return;
    }

    /* Handle password input during setting mode */
    if (SettingPassword) {
        if (WasButtonPressedWithDebounce(BUTTON0)) {
            CandidatePassword[InputIndex++] = 0; //sets 0 into the candidate password array
            ButtonAcknowledge(BUTTON0);
        }

        if (WasButtonPressedWithDebounce(BUTTON1)) {
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
    if (WasButtonPressedWithDebounce(BUTTON0)) {
        CandidatePassword[InputIndex++] = 0; // Store BUTTON0 press and increment InputIndex
        LedOn(BLUE0);
        LedPWM(BLUE0, LED_PWM_10);
        LedOff(BLUE1);
        ButtonAcknowledge(BUTTON0);         // Clear the button press state
    }

    if (WasButtonPressedWithDebounce(BUTTON1)) {
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
            u8 au8Success[] = {"Success!"};
            PixelAddressType sSuccessLocation = {U8_LCD_SMALL_FONT_LINE1, U16_LCD_LEFT_MOST_COLUMN};
            LcdClearScreen(); // Clear the LCD before displaying new message
            LCD_BACKLIGHT_ON(); // Ensure backlight is on
            LcdLoadString(au8Success, LCD_FONT_SMALL, &sSuccessLocation);
        } else {
            // Blink red at 1Hz for 3 seconds, then return to yellow
            for (u16 i = 0; i < 6000; i += LED_1HZ) {
                LedToggle(RED3);
                DelayMs(LED_1HZ);
            }
            LedSetColorYellow(); // Return to yellow after failure
            u8 au8Failure[] = {"Access Denied!"};
            PixelAddressType sFailureLocation = {U8_LCD_SMALL_FONT_LINE1, U16_LCD_LEFT_MOST_COLUMN};
            LcdClearScreen(); // Clear the LCD before displaying new message
            LCD_BACKLIGHT_ON(); // Ensure backlight is on
            LcdLoadString(au8Failure, LCD_FONT_SMALL, &sFailureLocation);

            // Trigger the error state after 2 seconds
            DelayMs(2000);
            UserApp1_pfStateMachine = UserApp1SM_Error; // Switch to error state
        }

        // Reset input for the next attempt
        InputIndex = 0;
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
