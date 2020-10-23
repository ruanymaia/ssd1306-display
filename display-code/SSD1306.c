#include "SSD1306.h"

// Auxiliary data 
uint8_t __GLCD_Buffer[__GLCD_Screen_Width * __GLCD_Screen_Lines];
GLCD_t __GLCD;

// Functions 
void GLCD_SendCommand(uint8_t Command)
{
	GLCD_Send(0<<__GLCD_DC, &Command, 1);
}

void GLCD_Setup(void)
{
	// Setup I2C hardware
	TWI_Setup();

	// Commands needed for initialization
	GLCD_SendCommand(__GLCD_Command_Display_Off);					// 0xAE
	
	GLCD_SendCommand(__GLCD_Command_Display_Clock_Div_Ratio_Set);	// 0xD5
	GLCD_SendCommand(0xF0);											// Suggest ratio
	
	GLCD_SendCommand(__GLCD_Command_Multiplex_Radio_Set);			// 0xA8
	GLCD_SendCommand(__GLCD_Screen_Height - 1);
	
	GLCD_SendCommand(__GLCD_Command_Display_Offset_Set);			// 0xD3
	GLCD_SendCommand(0x00);											// No offset

	GLCD_SendCommand(__GLCD_Command_Charge_Pump_Set);				// 0x8D
	GLCD_SendCommand(0x14);											// Enable charge pump

	GLCD_SendCommand(__GLCD_Command_Display_Start_Line_Set | 0x00);	// 0x40 | Start line
	
	GLCD_SendCommand(__GLCD_Command_Memory_Addressing_Set);			// 0x20
	GLCD_SendCommand(0x00);							// Horizontal Addressing - Operate like KS0108
	
	GLCD_SendCommand(__GLCD_Command_Segment_Remap_Set | 0x01);		// 0xA0 - Left towards Right

	GLCD_SendCommand(__GLCD_Command_Com_Output_Scan_Dec);			// 0xC8 - Up towards Down

	GLCD_SendCommand(__GLCD_Command_Com_Pins_Set);				// 0xDA
	
	GLCD_SendCommand(0x12);						// Sequential COM pin configuration
	
	GLCD_SendCommand(__GLCD_Command_Constrast_Set);				// 0x81
	GLCD_SendCommand(0xFF);

	GLCD_SendCommand(__GLCD_Command_Precharge_Period_Set);			// 0xD9
	GLCD_SendCommand(0xF1);

	GLCD_SendCommand(__GLCD_Command_VCOMH_Deselect_Level_Set);		// 0xDB
	GLCD_SendCommand(0x20);

	GLCD_SendCommand(__GLCD_Command_Display_All_On_Resume);			// 0xA4
	GLCD_SendCommand(__GLCD_Command_Display_Normal);			// 0xA6
	GLCD_SendCommand(__GLCD_Command_Display_On);				// 0xAF
	
	// Go to 0,0
	GLCD_GotoXY(0, 0);
	
	// Reset GLCD structure
	__GLCD.X = __GLCD.Y = __GLCD.Font.Width = __GLCD.Font.Height = __GLCD.Font.Lines = 0;
}

void GLCD_Render(void)
{
	// We have to send buffer as 16-byte packets
	// Buffer Size:				  Width * Height / Line_Height
	// Packet Size:				  16
	// Loop Counter:				  Buffer size / Packet Size		=
	// 							= ((Width * Height) / 8) / 16	=
	// 							= (Width / 16) * (Height / 8)	=
	// 							= (Width >> 4) * (Height >> 3)
	uint8_t i, loop;
	loop = (__GLCD_Screen_Width>>4) * (__GLCD_Screen_Height>>3);

	// Set columns
	GLCD_SendCommand(__GLCD_Command_Column_Address_Set);			// 0x21
	GLCD_SendCommand(0x00);									// Start
	GLCD_SendCommand(__GLCD_Screen_Width - 1);				// End

	// Set rows
	GLCD_SendCommand(__GLCD_Command_Page_Address_Set);			// 0x22
	GLCD_SendCommand(0x00);									// Start
	GLCD_SendCommand(__GLCD_Screen_Lines - 1);				// End

	// Send buffer
	for (i = 0 ; i < loop ; i++)
		GLCD_Send(1<<__GLCD_DC, &__GLCD_Buffer[i<<4], 16);
}

void GLCD_GotoX(const uint8_t X)
{
	if (X < __GLCD_Screen_Width)
		__GLCD.X = X;
}

void GLCD_GotoY(const uint8_t Y)
{
	if (Y < __GLCD_Screen_Height)
		__GLCD.Y = Y;
}

void GLCD_GotoXY(const uint8_t X, const uint8_t Y)
{
	GLCD_GotoX(X);
	GLCD_GotoY(Y);
}

uint8_t GLCD_GetLine(void)
{
	return (__GLCD_GetLine(__GLCD.Y));
}

void GLCD_SetFont(const uint8_t *Name, const uint8_t Width, const uint8_t Height)
{
	if ((Width < __GLCD_Screen_Width) && (Height < __GLCD_Screen_Height))
	{
		// Change font pointer to new font
		__GLCD.Font.Name = (uint8_t *)(Name);
		
		// Update font's size
		__GLCD.Font.Width = Width;
		__GLCD.Font.Height = Height;
		
		// Update lines required for a character to be fully displayed
		__GLCD.Font.Lines = (Height - 1) / __GLCD_Screen_Line_Height + 1;
	}
}

void GLCD_PrintChar(char Character)
{
	// If it doesn't work, replace pgm_read_byte with pgm_read_word
	uint16_t fontStart, fontRead, fontReadPrev;
	uint8_t x, y, y2, i, j, width, overflow, data, dataPrev;
	fontStart = fontRead = fontReadPrev = x = y = y2 = i = j = width = overflow = data = dataPrev = 0;
	
	// #1 - Save current position
	x = __GLCD.X;
	y = y2 = __GLCD.Y;
	
	// #2 - Remove leading empty characters
	Character -= 32;														// 32 is the ASCII of the first printable character
	
	// #3 - Find the start of the character in the font array
	fontStart = Character * (__GLCD.Font.Width * __GLCD.Font.Lines + 1);		// +1 due to first byte of each array line being the width
	
	// #4 - Update width - First byte of each line is the width of the character
	width = pgm_read_byte(&(__GLCD.Font.Name[fontStart++]));
	
	
	// #5 - Calculate overflowing bits
	overflow = __GLCD.Y % __GLCD_Screen_Line_Height;
	
	// #6 - Print the character
	// Scan the lines needed
	for (j = 0 ; j < __GLCD.Font.Lines ; j++)
	{
		// Go to the start of the line
		GLCD_GotoXY(x, y);
		
		// Update the indices for reading the line
		fontRead = fontStart + j;
		fontReadPrev = fontRead - 1;

		// Scan bytes of selected line
		for (i = 0 ; i < width ; i++)
		{
			// Read byte
			data = pgm_read_byte(&(__GLCD.Font.Name[fontRead]));
			
			// Shift byte
			data <<= overflow;
			
			// Merge byte with previous one
			if (j > 0)
			{
				dataPrev = pgm_read_byte(&(__GLCD.Font.Name[fontReadPrev]));
				dataPrev >>= __GLCD_Screen_Line_Height - overflow;
				data |= dataPrev;
				fontReadPrev += __GLCD.Font.Lines;
			}

			// Send byte
			GLCD_BufferWrite(__GLCD.X++, __GLCD.Y, data);
			
			// Increase index
			fontRead += __GLCD.Font.Lines;
		}

		// Send an empty column of 1px in the end
		GLCD_BufferWrite(__GLCD.X, __GLCD.Y, __GLCD_White);
		
		// Increase line counter
		y += __GLCD_Screen_Line_Height;
	}

	// #7 - Update last line, if needed
	// If (LINE_STARTING != LINE_ENDING)
	if (__GLCD_GetLine(y2) != __GLCD_GetLine((y2 + __GLCD.Font.Height - 1)) && y < __GLCD_Screen_Height)
	{
		// Go to the start of the line
		GLCD_GotoXY(x, y);
		
		// Update the index for reading the last printed line
		fontReadPrev = fontStart + j - 1;

		// Scan bytes of selected line
		for (i = 0 ; i < width ; i++)
		{
			// Read byte
			data = GLCD_BufferRead(__GLCD.X, __GLCD.Y);
			
			// Merge byte with previous one
			dataPrev = pgm_read_byte(&(__GLCD.Font.Name[fontReadPrev]));
			dataPrev >>= __GLCD_Screen_Line_Height - overflow;
			data |= dataPrev;
			
			// Send byte
			GLCD_BufferWrite(__GLCD.X++, __GLCD.Y, data);

			// Increase index
			fontReadPrev += __GLCD.Font.Lines;
		}

		// Send an empty column of 1px in the end
		GLCD_BufferWrite(__GLCD.X, __GLCD.Y, __GLCD_White);
	}
	
	// Move cursor to the end of the printed character
	GLCD_GotoXY(x + width + 1, y2);
}

void GLCD_PrintString(const char *Text)
{
	while(*Text)
	{
		if ((__GLCD.X + __GLCD.Font.Width) >= __GLCD_Screen_Width)
			break;

		GLCD_PrintChar(*Text++);
	}
}

void GLCD_PrintString_P(const char *Text)
{
	char r = pgm_read_byte(Text++);
	while(r)
	{
		if ((__GLCD.X + __GLCD.Font.Width) >= __GLCD_Screen_Width) 
			break;

		GLCD_PrintChar(r);
		r = pgm_read_byte(Text++);
	}
}

void GLCD_PrintInteger(const int32_t Value)
{
	if (Value == 0)
	{
		GLCD_PrintChar('0');
	}
	else if ((Value > INT32_MIN) && (Value <= INT32_MAX))
	{
		// int32_max_bytes + sign + null = 12 bytes
		char bcd[12] = { '\0' };
		
		// Convert integer to array
		Int2bcd(Value, bcd);
		
		// Print from first non-zero digit
		GLCD_PrintString(bcd);
	}
}

void GLCD_PrintDouble(double Value, const uint8_t Precision)
{
	if (Value == 0)
	{
		// Print characters individually so no string is stored in RAM
		GLCD_PrintChar('0');
		GLCD_PrintChar('.');
		GLCD_PrintChar('0');
	}
	else if ((Value >= (-2147483647)) && (Value < 2147483648))
	{
		// Print sign
		if (Value < 0)
		{
			Value = -Value;
			GLCD_PrintChar('-');
		}
		
		// Print integer part
		GLCD_PrintInteger(Value);
		
		// Print dot
		GLCD_PrintChar('.');
		
		// Print decimal part
		GLCD_PrintInteger((Value - (uint32_t)(Value)) * pow(10, Precision));
	}
}

static void GLCD_Send(const uint8_t Control, uint8_t *Data, const uint8_t Length)
{
	uint8_t i;

	do
	{
		// Transmit START signal
		TWI_BeginTransmission();

		// Transmit SLA+W
		TWI_Transmit(0x3C<<1);	// 0x3C - SSD1306 I2C address
		
		// Transmit control byte
		TWI_Transmit(Control);

		for (i = 0 ; i < Length ; i++)
		{
			// Transmit data
			TWI_Transmit(Data[i]);
		}
	}
	while (0);
	
	// Transmit STOP signal
	TWI_EndTransmission();
}

static void GLCD_BufferWrite(const uint8_t X, const uint8_t Y, const uint8_t Data)
{
	__GLCD_Buffer[__GLCD_Pointer(X, Y)] = Data;
}

static uint8_t GLCD_BufferRead(const uint8_t X, const uint8_t Y)
{
	// y>>3 = y / 8
	return (__GLCD_Buffer[__GLCD_Pointer(X, Y)]);
}

static void Int2bcd(int32_t Value, char BCD[])
{
	uint8_t isNegative = 0;
	
	BCD[0] = BCD[1] = BCD[2] =
	BCD[3] = BCD[4] = BCD[5] =
	BCD[6] = BCD[7] = BCD[8] =
	BCD[9] = BCD[10] = '0';
	
	if (Value < 0)
	{
		isNegative = 1;
		Value = -Value;
	}
	
	while (Value >= 1000000000)
	{
		Value -= 1000000000;
		BCD[1]++;
	}
	
	while (Value >= 100000000)
	{
		Value -= 100000000;
		BCD[2]++;
	}
		
	while (Value >= 10000000)
	{
		Value -= 10000000;
		BCD[3]++;
	}
	
	while (Value >= 1000000)
	{
		Value -= 1000000;
		BCD[4]++;
	}
	
	while (Value >= 100000)
	{
		Value -= 100000;
		BCD[5]++;
	}

	while (Value >= 10000)
	{
		Value -= 10000;
		BCD[6]++;
	}

	while (Value >= 1000)
	{
		Value -= 1000;
		BCD[7]++;
	}
	
	while (Value >= 100)
	{
		Value -= 100;
		BCD[8]++;
	}
	
	while (Value >= 10)
	{
		Value -= 10;
		BCD[9]++;
	}

	while (Value >= 1)
	{
		Value -= 1;
		BCD[10]++;
	}

	uint8_t i = 0;
	// Find first non zero digit
	while (BCD[i] == '0')
		i++;

	// Add sign 
	if (isNegative)
	{
		i--;
		BCD[i] = '-';
	}

	// Shift array
	uint8_t end = 10 - i;
	uint8_t offset = i;
	i = 0;
	while (i <= end)
	{
		BCD[i] = BCD[i + offset];
		i++;
	}
	BCD[i] = '\0';
}