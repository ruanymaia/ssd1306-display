#include "SSD1306.h"
#include "Font5x8.h"

int main(void)
{	
	GLCD_Setup();
	GLCD_SetFont(Font5x8, 5, 8);

	GLCD_GotoXY(11, 0);
	GLCD_PrintString("13 out 2020 12:00");
	GLCD_GotoXY(0, 20);
	GLCD_PrintString("T: 30.0 C");
	GLCD_GotoXY(0, 32);
	GLCD_PrintString("RH: 50.0 %");
	GLCD_GotoXY(0, 44);
	GLCD_PrintString("P: 920.4 hPa");
	GLCD_GotoXY(0, 56);
	GLCD_PrintString("H: 1538.0 m");
	GLCD_Render();

    return 0;
}
