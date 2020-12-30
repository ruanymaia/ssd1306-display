#include "SSD1306.h"
#include "Font5x8.h"

int main(void)
{	
	GLCD_Setup();
	GLCD_SetFont(Font5x8, 5, 8);

	while (1)
	{
		GLCD_GotoXY(25, 2);
		GLCD_PrintString("This is CS50x");
		_delay_ms(1000);
		GLCD_Render();
		
		GLCD_GotoXY(35, 16);
		GLCD_PrintString("C Language");
		_delay_ms(1000);
		GLCD_Render();

		GLCD_GotoXY(20, 28);
		GLCD_PrintString("ATMega328P AVR");
		_delay_ms(1000);
		GLCD_Render();

		GLCD_GotoXY(5, 40);
		GLCD_PrintString("SSD1306 OLED Display");
		_delay_ms(1000);
		GLCD_Render();

		_delay_ms(1000);
		GLCD_Clear();
	}

    return 0;
}
