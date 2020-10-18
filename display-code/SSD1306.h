#ifndef SSD1306_H_INCLUDED
#define SSD1306_H_INCLUDED

// Headers
#include "TWI.h"

#include <inttypes.h>
#include <util/delay.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

// Auxiliary data
#define __GLCD_Screen_Width		128
#define __GLCD_Screen_Height	64

#define __GLCD_White			0x00
	
#define __GLCD_RW				0
#define __GLCD_SA0				1
#define __GLCD_DC				6
#define __GLCD_CO				7

#define	__GLCD_Screen_Line_Height	8
#define __GLCD_Screen_Lines			(__GLCD_Screen_Height / __GLCD_Screen_Line_Height)

#define __GLCD_GetLine(Y)		(Y / __GLCD_Screen_Line_Height)
#define __GLCD_Pointer(X, Y)	(X + ((Y / __GLCD_Screen_Line_Height) *__GLCD_Screen_Width))

// Fundamental Command Table
#define __GLCD_Command_Display_On				0xAF
#define __GLCD_Command_Display_Off				0xAE
#define __GLCD_Command_Constrast_Set			0x81
#define __GLCD_Command_Display_All_On_Resume	0xA4
#define __GLCD_Command_Display_All_On			0xA5
#define __GLCD_Command_Display_Normal			0xA6

// Addressing Setting Command Table
#define __GLCD_Command_Page_Address_Set						0x22
#define __GLCD_Command_Memory_Addressing_Set				0x20
#define __GLCD_Command_Column_Address_Set					0x21

// Hardware Configuration
#define __GLCD_Command_Display_Start_Line_Set	0x40
#define __GLCD_Command_Display_Offset_Set		0xD3
#define __GLCD_Command_Segment_Remap_Set		0xA0
#define __GLCD_Command_Multiplex_Radio_Set		0xA8
#define __GLCD_Command_Com_Output_Scan_Inc		0xC0
#define __GLCD_Command_Com_Output_Scan_Dec		0xC8
#define __GLCD_Command_Com_Pins_Set				0xDA

// Timing and Driving Scheme Setting Command Table
#define __GLCD_Command_Display_Clock_Div_Ratio_Set		0xD5
#define __GLCD_Command_Precharge_Period_Set				0xD9
#define __GLCD_Command_VCOMH_Deselect_Level_Set			0xDB

// Charge Pump Command Table
#define __GLCD_Command_Charge_Pump_Set	0x8D

enum PrintMode_t
{
	GLCD_Overwrite,
	GLCD_Merge
};

typedef struct
{
	uint8_t *Name;
	uint8_t Width;
	uint8_t Height;
	uint8_t Lines;
}Font_t;

typedef struct
{
	uint8_t X;
	uint8_t Y;
	Font_t Font;
}GLCD_t;

// Prototypes
void GLCD_SendCommand(uint8_t Command);
void GLCD_Setup(void);
void GLCD_Render(void);

void GLCD_GotoX(const uint8_t X);
void GLCD_GotoY(const uint8_t Y);
void GLCD_GotoXY(const uint8_t X, const uint8_t Y);
uint8_t GLCD_GetLine(void);

void GLCD_SetFont(const uint8_t *Name, const uint8_t Width, const uint8_t Height);
void GLCD_PrintChar(char Character);
void GLCD_PrintString(const char *Text);
void GLCD_PrintString_P(const char *Text);
void GLCD_PrintInteger(const int32_t Value);
void GLCD_PrintDouble(double Value, const uint8_t Precision);

static void GLCD_Send(const uint8_t Control, uint8_t *Data, const uint8_t Length);
static void GLCD_BufferWrite(const uint8_t X, const uint8_t Y, const uint8_t Data);
static uint8_t GLCD_BufferRead(const uint8_t X, const uint8_t Y);
static void Int2bcd(int32_t Value, char BCD[]);

#endif
