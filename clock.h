
String lastTimeStr;

static char months[12][4] = {
  "JAN",
  "FEB",
  "MAR",
  "APR",
  "MAY",
  "JUN",
  "JUL",
  "AUG",
  "SEP",
  "OCT",
  "NOV",
  "DEC"
};

static char days[7][4] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

void update_clock() {
  uint16_t colour;

  String currentNtp;
  int OFFSET = 6;
  
  uptime = uptime + 1;

  timeClient.update();
  currentNtp = timeClient.getFormattedTime();

  if (currentNtp != lastTimeStr) {

    myscreen.setTextSize(4);

    myscreen.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);

    myscreen.setCursor(OFFSET, ROW_CLOCK);
    myscreen.printf("%02d", timeClient.getHours());

    myscreen.setCursor(OFFSET + 58, ROW_CLOCK);
    myscreen.printf("%02d", timeClient.getMinutes());

    myscreen.setCursor(OFFSET + 180, ROW_CLOCK);
    myscreen.printf("%2d", timeClient.getDate());

    myscreen.setTextSize(2);

    myscreen.setCursor(OFFSET + 110, ROW_CLOCK);
    myscreen.printf("%02d", timeClient.getSeconds());

  
    if(timeClient.getSeconds() % 2 == 0) colour = ILI9341_YELLOW;
    else colour = ILI9341_BLACK;
    myscreen.fillRect(52, 12, 4, 4, colour);  
    myscreen.fillRect(52, 24, 4, 4, colour);  
  
    myscreen.setCursor(OFFSET + 140, ROW_CLOCK + 14);
    myscreen.printf("%3s", months[ timeClient.getMonth() - 1]);

    myscreen.setTextSize(1);

    myscreen.setCursor(OFFSET + 114, ROW_CLOCK + 20);
    myscreen.printf("%3s", days[ timeClient.getDay() ]);

    myscreen.setCursor(OFFSET + 144, ROW_CLOCK);
    myscreen.printf("%02d", timeClient.getYear());

 //   drawDecimal(COL_CPU_TEMP, ROW_CPU, (temprature_sens_read() - 32) / 1.8);

    lastTimeStr = currentNtp;
  }
}
