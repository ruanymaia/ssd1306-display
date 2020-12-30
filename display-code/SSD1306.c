#include "SSD1306.h"

// Auxiliary data 
uint8_t __GLCD_Buffer[1024]; // Screen Width * Screen Lines
GLCD_t __GLCD;

void GLCD_Setup(void)
{
	// Setup I2C hardware
	TWI_Setup();

	// Software Initialization
	GLCD_SendCommand(0x20);	// Set Memory Addressing Mode
	GLCD_SendCommand(0x00);	// Horizontal Addressing Mode

	GLCD_SendCommand(0xA8);	// Set Multiplex Ratio		
	GLCD_SendCommand(0x3F);

	GLCD_SendCommand(0xD3);	// Set Display Offset		
	GLCD_SendCommand(0x00);	
	
	GLCD_SendCommand(0x40);	// Set Display Start Line	

	GLCD_SendCommand(0xA1);	// Set Segment Remap 

	GLCD_SendCommand(0xC8);	// Set COM Output Scan Direction - up to down

	GLCD_SendCommand(0xDA);	// Set COM Pins hardware configuration
	GLCD_SendCommand(0x12);	

	GLCD_SendCommand(0x81);	// Set Contrast Control
	GLCD_SendCommand(0xFF);	// 0x00 to 0xFF

	GLCD_SendCommand(0xA4);	// Disable Entire Display On

	GLCD_SendCommand(0xA6);	// Set Normal/Inverse Display (0xA6/0xA7)

	GLCD_SendCommand(0xD5);	// Set Oscillator Frequency
	GLCD_SendCommand(0xF0);	

	GLCD_SendCommand(0xD9);	// Set pre-charge Period
	GLCD_SendCommand(0xF1);	

	GLCD_SendCommand(0x8D);	// Enable charge pump regulator
	GLCD_SendCommand(0x14);

	GLCD_SendCommand(0xDB);	// Set VCOMH Deselect Level
	GLCD_SendCommand(0x20);	

	GLCD_SendCommand(0xAF); // Display ON

	// Go to 0,0
	GLCD_GotoXY(0, 0);
	
	// Reset GLCD structure
	__GLCD.X = __GLCD.Y = __GLCD.Font.Width = __GLCD.Font.Height = __GLCD.Font.Lines = 0;
}

void GLCD_Render(void)
{
	// Buffer Size:		Width*Height / Line_Height = 1024 bytes
	// Packet Size:		16 bytes
	// Loop Counter:	Buffer size / Packet Size	= 64
	
	uint8_t i, count;
	count = 64; 

	// Set columns
	GLCD_SendCommand(0x21);		// Set Column Address
	GLCD_SendCommand(0x00);		// Start
	GLCD_SendCommand(127);		// End

	// Set rows
	GLCD_SendCommand(0x22);		// Set Page Address
	GLCD_SendCommand(0x00);		// Start
	GLCD_SendCommand(7);		// End: __GLCD_Screen_Lines - 1

	// Send buffer
	for (i = 0 ; i < count ; i++)
		GLCD_SendData(&__GLCD_Buffer[i<<4]);
		
}

void GLCD_GotoXY(const uint8_t X, const uint8_t Y)
{
	// X: 0-127; Y: 0-63;
	__GLCD.X = X;
	__GLCD.Y = Y;
}

void GLCD_SetFont(const uint8_t *name, const uint8_t width, const uint8_t height)
{
	// Change font pointer to new font
	__GLCD.Font.Name = (uint8_t *)(name);
	
	// Update font's size
	__GLCD.Font.Width = width;
	__GLCD.Font.Height = height;

	// Update lines required for a character to be fully displayed
	__GLCD.Font.Lines = (height - 1) / __GLCD_Screen_Line_Height + 1;

}

void GLCD_PrintChar(char character)
{
	// If it doesn't work, replace pgm_read_byte with pgm_read_word
	uint16_t fontStart, fontRead, fontReadPrev;
	uint8_t x, y, y2, i, j, width, overflow, data, dataPrev;
	fontStart = fontRead = fontReadPrev = x = y = y2 = i = j = width = overflow = data = dataPrev = 0;
	
	// #1 - Save current position
	x = __GLCD.X;
	y = y2 = __GLCD.Y;
	
	// #2 - Remove leading empty characters
	character -= 32;	// 32 is the ASCII of the first printable character
	
	// #3 - Find the start of the character in the font array
	fontStart = character * (__GLCD.Font.Width * __GLCD.Font.Lines + 1);	// +1 due to first byte of each array line being the width
	
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
		GLCD_BufferWrite(__GLCD.X, __GLCD.Y, 0x00);
		
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
		GLCD_BufferWrite(__GLCD.X, __GLCD.Y, 0x00);
	}
	
	// Move cursor to the end of the printed character
	GLCD_GotoXY(x + width + 1, y2);
}

void GLCD_Clear(void)
{
	uint8_t i, j;

	for (j = 0 ; j < __GLCD_Screen_Height ; j++)
		for (i = 0 ; i < __GLCD_Screen_Width ; i++)
			GLCD_BufferWrite(i, j, 0x00);
}

void GLCD_PrintString(const char *text)
{
	while(*text)
	{
		if ((__GLCD.X + __GLCD.Font.Width) >= __GLCD_Screen_Width)
			break;

		GLCD_PrintChar(*text++);
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

static void GLCD_SendCommand(int command)
{
	// Transmit START signal
	TWI_BeginTransmission();

	// Transmit SLA+W
	TWI_Transmit(0x3C<<1);	// 0x3C - SSD1306 I2C address
	
	// Transmit D/C# selection bit - 0<<DC
	TWI_Transmit(0); // 0<<6 - D/C# pin low: command mode

	// Transmit command
	TWI_Transmit(command); 
	
	// Transmit STOP signal
	TWI_EndTransmission();
}

static void GLCD_SendData(uint8_t *data)
{
	uint8_t i, length;
	length = 16;

	// Transmit START signal
	TWI_BeginTransmission();

	// Transmit SLA+W
	TWI_Transmit(0x3C<<1);	// 0x3C - SSD1306 I2C address
	
	// Transmit D/C# selection bit - 1<<DC
	TWI_Transmit(64); // 1<<6 - D/C# pin high: data mode

	for (i = 0 ; i < length ; i++)
	{
		// Transmit data
		TWI_Transmit(data[i]);
	}

	// Transmit STOP signal
	TWI_EndTransmission();
}

static void GLCD_BufferWrite(const uint8_t X, const uint8_t Y, const uint8_t data)
{
	__GLCD_Buffer[__GLCD_Pointer(X, Y)] = data;
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