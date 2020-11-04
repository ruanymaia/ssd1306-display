#include "SSD1306.h"
#include "Font5x8.h"

int main(void)
{	
	GLCD_Setup();
	GLCD_SetFont(Font5x8, 5, 8);

	GLCD_GotoXY(11, 2);
	GLCD_PrintString("13 out 2020 12:00");
	GLCD_GotoXY(5, 16);
	GLCD_PrintString("T: 30.00 C");
	GLCD_GotoXY(5, 28);
	GLCD_PrintString("RH: 50.00 %");
	GLCD_GotoXY(5, 40);
	GLCD_PrintString("P: 920.40 hPa");
	GLCD_GotoXY(5, 52);
	GLCD_PrintString("H: 1538.00 m");
	GLCD_Render();

    return 0;
}
