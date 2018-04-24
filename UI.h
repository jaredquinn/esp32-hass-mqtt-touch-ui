
#ifndef UI_H
#define UI_H


#define UI_MSG_INIT    "Booting CatsLair IOT"
#define UI_MSG_WELCOME "Welcome to CatsLair"

#define UI_WIDGET_NONE   0
#define UI_WIDGET_FLOAT  1
#define UI_WIDGET_BULB   2
#define UI_WIDGET_BUTTON 3
#define UI_WIDGET_HOUSE  4
#define UI_WIDGET_CLOCK  99

#define UI_CLOCK_TIME_HH    0
#define UI_CLOCK_TIME_MM    1
#define UI_CLOCK_TIME_SS    2
#define UI_CLOCK_TIME_DOTS  3
#define UI_CLOCK_DATE_DOW   4
#define UI_CLOCK_DATE_YEAR  5
#define UI_CLOCK_DATE_MON   6
#define UI_CLOCK_DATE_DAY   7

#define UI_SLOTS_MAX_CLOCK  7


#define UI_POS_1_1  UI_SLOTS_MAX_CLOCK + 1
#define UI_POS_1_2  UI_SLOTS_MAX_CLOCK + 2
#define UI_POS_1_3  UI_SLOTS_MAX_CLOCK + 3
#define UI_POS_1_4  UI_SLOTS_MAX_CLOCK + 4

#define UI_POS_2_1  UI_SLOTS_MAX_CLOCK + 5
#define UI_POS_2_2  UI_SLOTS_MAX_CLOCK + 6
#define UI_POS_2_3  UI_SLOTS_MAX_CLOCK + 7
#define UI_POS_2_4  UI_SLOTS_MAX_CLOCK + 8

#define UI_POS_3_1  UI_SLOTS_MAX_CLOCK + 9
#define UI_POS_3_2  UI_SLOTS_MAX_CLOCK + 10 
#define UI_POS_3_3  UI_SLOTS_MAX_CLOCK + 11
#define UI_POS_3_4  UI_SLOTS_MAX_CLOCK + 12

#define UI_POS_4_1  UI_SLOTS_MAX_CLOCK + 13
#define UI_POS_4_2  UI_SLOTS_MAX_CLOCK + 14
#define UI_POS_4_3  UI_SLOTS_MAX_CLOCK + 15
#define UI_POS_4_4  UI_SLOTS_MAX_CLOCK + 16

#define UI_SLOTS_MAX_WIDGETS    UI_SLOTS_MAX_CLOCK + 16

/* Slots continue;  Slots 16-22 are for the button row with 8 columns! */

#define UI_BUTTON_1   UI_SLOTS_MAX_WIDGETS + 1
#define UI_BUTTON_2   UI_SLOTS_MAX_WIDGETS + 2
#define UI_BUTTON_3   UI_SLOTS_MAX_WIDGETS + 3
#define UI_BUTTON_4   UI_SLOTS_MAX_WIDGETS + 4
#define UI_BUTTON_5   UI_SLOTS_MAX_WIDGETS + 5
#define UI_BUTTON_6   UI_SLOTS_MAX_WIDGETS + 6
#define UI_BUTTON_7   UI_SLOTS_MAX_WIDGETS + 7
#define UI_BUTTON_8   UI_SLOTS_MAX_WIDGETS + 8

/* Slots for the clock come next! */



#include "Adafruit_ILI9341.h"
#include "datastore.h"

#define UI_SLOTS_TOTAL      UI_SLOTS_MAX_WIDGETS + 8


#define UI_CLOCK_COLOUR  ILI9341_YELLOW


class UI {
  public:
    UI(Adafruit_ILI9341& scrn);

    typedef struct {
      int x, y, h, w, fs, slot = -1;
    } uiPosition_t;

    void clearScreen(bool clearClock, bool clearStatus);
    void updateStatus(char *str, uint16_t colour, bool temporary);
    void finishSetup();
    void loop();
    void render();
    void renderWidget(int c);
    void renderFrame(int c);
    void renderGrid();
    void renderActiveFrames();
    void activityLight(int light, bool activity);
    void updateClock(int hour, int minute, int second, char *dow, int year, char *month, int day);

    uiPosition_t getPositionForSlot(int slot);

    void addClock(int slot, DataStore &ds);
    void addWidget(int slot, int widgetType, char *title, char *unit, DataStore &ds);
    void addButton(int slot, int widgetType, char *title);

    Adafruit_ILI9341 *screen;

  private:
   
   struct uiClock_t {
      DataStore *hour;
      DataStore *minute;
      DataStore *second;
      DataStore *dayofweek;
      DataStore *year;
      DataStore *month;
      DataStore *day;
    } clock;
    
    struct uiWidget_t {
      char title[32];
      char unit[3];
      int  widgetType;
      DataStore * ds;
      
      long _lastDrawn;
      long _lastTitle;
      bool _active;
      bool _updates;
      
    } _widgets[UI_SLOTS_TOTAL];

    void _initializeBacklight();
    void _initializeScreen();
    void _initializeWidgets();

    void _drawDivider(int x);

    void _drawClockComponent(uiPosition_t pos, char *value);    
    void _drawWidget_Float(int column, int row, float value);
    void _drawWidget_Bulb(int column, int row, bool state);
    void _drawButton(int column, int row, int width, int height, char *title);
    void _drawWidget_House(int column, int row, char *state);
    
    void _drawTitle(int column, int row, char *title);
    void _drawUnit(int column, int row, char *unit);
    void _drawFrame_Float(int column, int row, char *title, char *unit);
    void _drawFrame_Bulb(int column, int row, char *title, char *unit);

    bool _drawnGrid;  /* has the grid been drawn */
    /* Backlight LED Control */
    int pwm_freq;
    int pwm_ledChannel;
    int pwm_resolution;

    /* When UI is ready */
    void _ready();
    
    /* Status Bar */
    char _lastStatusPermanent[255];
    long _lastStatusUpdate;

    
};

#endif

