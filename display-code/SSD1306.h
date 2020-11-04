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

#define	__GLCD_Screen_Line_Height	8
#define __GLCD_Screen_Lines			8	// Screen Height / Screen Line Height = 64/8

#define __GLCD_GetLine(Y)		(Y / __GLCD_Screen_Line_Height)
#define __GLCD_Pointer(X, Y)	(X + ((Y / __GLCD_Screen_Line_Height) *__GLCD_Screen_Width))

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
void GLCD_Setup(void);
void GLCD_Render(void);

void GLCD_GotoXY(const uint8_t X, const uint8_t Y);

void GLCD_SetFont(const uint8_t *name, const uint8_t width, const uint8_t height);
void GLCD_PrintChar(char Character);
void GLCD_PrintString(const char *Text);
void GLCD_PrintString_P(const char *Text);
void GLCD_PrintInteger(const int32_t Value);
void GLCD_PrintDouble(double Value, const uint8_t Precision);

static void GLCD_SendData(uint8_t *data);
static void GLCD_SendCommand(int command);
static void GLCD_BufferWrite(const uint8_t X, const uint8_t Y, const uint8_t data);
static uint8_t GLCD_BufferRead(const uint8_t X, const uint8_t Y);
static void Int2bcd(int32_t Value, char BCD[]);

#endif
