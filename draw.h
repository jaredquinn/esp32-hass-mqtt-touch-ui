/* Deprecated */

#include "rows.h"

void show_mqtt_activity(bool activity) {
  uint16_t colour;

  if(activity == true) colour = myscreen.color565(0,255,0);
  else colour = myscreen.color565(0,0,0);

  myscreen.fillRect(236, 4, 4, 4, colour);  
}



void drawStatus(char *str, uint16_t colour) {
  myscreen.fillRect(0, 300, 240, 20, colour);
  myscreen.setTextSize(1);
  myscreen.setTextColor(ILI9341_WHITE, colour);
  myscreen.setCursor(0, 302);
  myscreen.print(str);
  if(strcmp(str, "Welcome to CatsLair") == 0) lastStatusPush = -1;
  else lastStatusPush = millis();
}

void drawBulb(int x, int y, bool on) {
  uint16_t colour;
  

  if(on == true) {
    colour = myscreen.color565(255,99,71);
  //  myscreen.drawBitmap(x, y+10, BMP_BULB_ON, 36, 36, colour);
  } else {
    colour = myscreen.color565(90,90,90);
  //  myscreen.drawBitmap(x, y+10, BMP_BULB_OFF, 36, 36, colour);
  }

  myscreen.fillRect(x+9, y+28, 11, 15, colour);
  myscreen.fillRoundRect(x, y+10, 28, 28, 14, colour);
}

void drawDecimal(int x, int y, float temp) {

  int IntegerPart = (int)(temp);
  int DecimalPart = 100 * (temp - IntegerPart);

  myscreen.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  myscreen.setCursor(x, y + 12);
  myscreen.setTextSize(4);
  myscreen.printf("%02d", IntegerPart);
  myscreen.setCursor(x + 50, y + 26);
  myscreen.setTextSize(2);
  myscreen.printf("%02d", DecimalPart);

};

/*
void drawTitle(int x, int y, char *title, char *symbol) {
  myscreen.setTextSize(1);
  myscreen.setCursor(x, y);
  myscreen.setTextColor(ILI9341_BLUE, ILI9341_BLACK);
  myscreen.print(title);

  myscreen.setTextSize(2);

  if (symbol[0] == 'C') {
    myscreen.drawCircle(x + 52, y + 13, 3, ILI9341_DARKGREY);
    myscreen.setCursor(x + 62, y + 8);
  } else {
    myscreen.setCursor(x + 50, y + 8);
  }
  myscreen.setTextColor(ILI9341_DARKGREY, ILI9341_BLACK);
  myscreen.print(symbol);
}
*/

