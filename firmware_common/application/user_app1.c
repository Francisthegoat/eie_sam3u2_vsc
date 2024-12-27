/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
LcdClearScreen();
const u8 aau8AntPongLogo[U8_LCD_IMAGE_ROW_SIZE_50PX][U8_LCD_IMAGE_COL_BYTES_50PX] = {					
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0x03},					
{0xFF, 0xFF, 0xFF, 0x7F, 0xE0, 0xFF, 0x03},					
{0xFF, 0x0F, 0x00, 0x00, 0xC6, 0xFF, 0x03},					
{0xFF, 0x0F, 0x00, 0x00, 0xCF, 0xFF, 0x03},					
{0xFF, 0x0F, 0x00, 0x00, 0xCF, 0xFF, 0x03},					
{0xFF, 0x0F, 0x00, 0x00, 0xC6, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0x7F, 0xE0, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0xFF, 0xF0, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0xFF, 0xF0, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0x7F, 0xE0, 0xFF, 0x03},					
{0xFF, 0x0F, 0x00, 0x00, 0xC6, 0xFF, 0x03},					
{0xFF, 0x0F, 0x00, 0x00, 0xCF, 0xFF, 0x03},					
{0xFF, 0x0F, 0x00, 0x00, 0xCF, 0xFF, 0x03},					
{0xFF, 0x0F, 0x00, 0x00, 0xC6, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0x7F, 0xE0, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0xFF, 0xF0, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
{0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x03},					
};					



PixelBlockType pixsize = {0, 0, 50, 50}; // 32, 7
LcdLoadBitmap(&aau8AntPongLogo[0][0], &pixsize);

// PixelAddressType sTestStringLocation = {U8_LCD_SMALL_FONT_LINE0, U16_LCD_LEFT_MOST_COLUMN};
// u8 au8TestString[]  = {"FRANCISISTHEGOATABCDA"};
// LcdLoadString(au8TestString, LCD_FONT_SMALL, &sTestStringLocation);

// PixelAddressType sTestStringLocation2 = {U8_LCD_SMALL_FONT_LINE1, U16_LCD_LEFT_MOST_COLUMN};
// u8 au8TestString2[] = {"FRANCISISTHEGOATABCDA"};
// LcdLoadString(au8TestString2, LCD_FONT_SMALL, &sTestStringLocation2);

// PixelAddressType sTestStringLocation3 = {U8_LCD_SMALL_FONT_LINE2, U16_LCD_LEFT_MOST_COLUMN};
// u8 au8TestString3[] = {"FRANCISISTHEGOATABCDA"};
// LcdLoadString(au8TestString3, LCD_FONT_SMALL, &sTestStringLocation3);

// PixelAddressType sTestStringLocation4 = {U8_LCD_SMALL_FONT_LINE3, U16_LCD_LEFT_MOST_COLUMN};
// u8 au8TestString4[] = {"FRANCISISTHEGOATABCDA"};
// LcdLoadString(au8TestString4, LCD_FONT_SMALL, &sTestStringLocation4);

// PixelAddressType sTestStringLocation5 = {U8_LCD_SMALL_FONT_LINE4, U16_LCD_LEFT_MOST_COLUMN};
// u8 au8TestString5[] = {"FRANCISISTHEGOATABCDA"};
// LcdLoadString(au8TestString5, LCD_FONT_SMALL, &sTestStringLocation5);

// PixelAddressType sTestStringLocation6 = {U8_LCD_SMALL_FONT_LINE5, U16_LCD_LEFT_MOST_COLUMN};
// u8 au8TestString6[] = {"FRANCISISTHEGOATABCDA"};
// LcdLoadString(au8TestString6, LCD_FONT_SMALL, &sTestStringLocation6);

// PixelAddressType sTestStringLocation7 = {U8_LCD_SMALL_FONT_LINE6, U16_LCD_LEFT_MOST_COLUMN};
// u8 au8TestString7[] = {"FRANCISISTHEGOATABCDA"};
// LcdLoadString(au8TestString7, LCD_FONT_SMALL, &sTestStringLocation7);

// PixelAddressType sTestStringLocation8 = {U8_LCD_SMALL_FONT_LINE7, U16_LCD_LEFT_MOST_COLUMN};
// u8 au8TestString8[] = {"FRANCISISTHEGOATABCDA"};
// LcdLoadString(au8TestString8, LCD_FONT_SMALL, &sTestStringLocation8);

// PixelBlockType sPixelsToClear = {0, 4, 25, 100};
// // ROW TO START CLEARING = 0;
// // COLOUNM TO START CLEARING  = 0;
// // VERTICAL = 25;
// // HORIZONTAL = 30;
// LcdClearPixels(&sPixelsToClear);



  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }
} /* end UserApp1Initialize() */

  
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
/* What does this state do? */
static void UserApp1SM_Idle(void)
{
  

} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
