
#ifndef UI_H
#define UI_H

#include "Adafruit_ILI9341.h"
#include "datastore.h"

#define UI_MSG_INIT    "Booting CatsLair IOT"
#define UI_MSG_WELCOME "Welcome to CatsLair"

#define UI_WIDGET_NONE   0
#define UI_WIDGET_FLOAT  1
#define UI_WIDGET_BULB   2
#define UI_WIDGET_BUTTON 3
#define UI_WIDGET_HOUSE  4
#define UI_WIDGET_CLOCK  99


#define UI_SLOTS_MAX_WIDGETS    UI_SLOTS_MAX_CLOCK + 16

/* Slots continue;  Slots 16-22 are for the button row with 8 columns! */


#define seconds() (millis()/1000)


#define UI_CLOCK_COLOUR     ILI9341_YELLOW

#define UI_SLOTS_MAX_CLOCK  8
#define UI_SLOTS_TOTAL      UI_SLOTS_MAX_WIDGETS + 8

enum enumSlot { 
  UI_CLOCK_TIME_HH,
  UI_CLOCK_TIME_MM,
  UI_CLOCK_TIME_SS,
  UI_CLOCK_DATE_DOW,
  UI_CLOCK_DATE_YEAR,
  UI_CLOCK_DATE_MON,
  UI_CLOCK_DATE_DAY,  
  UI_CLOCK_TIME_DOTS,  
  UI_POS_1_1,
  UI_POS_1_2,
  UI_POS_1_3,
  UI_POS_1_4,  
  UI_POS_2_1,
  UI_POS_2_2,
  UI_POS_2_3,
  UI_POS_2_4,  
  UI_POS_3_1,
  UI_POS_3_2,
  UI_POS_3_3,
  UI_POS_3_4,  
  UI_POS_4_1,
  UI_POS_4_2,
  UI_POS_4_3,
  UI_POS_4_4,  
  UI_BUTTON_1,
  UI_BUTTON_2,
  UI_BUTTON_3,
  UI_BUTTON_4,
  UI_BUTTON_5,
  UI_BUTTON_6,
  UI_BUTTON_7,
  UI_BUTTON_8,              
};



class UI {
  public:
    UI(Adafruit_ILI9341& scrn, int ledPin);

    typedef struct {
      int x = -1, y = -1, h = -1, w = -1, fs = -1, slot = -1;
    } uiPosition_t;

    DataStore clocks[8] = {
      DataStore("clock_time_hh", DATASTORE_TYPE_INT, "%02d"),
      DataStore("clock_time_mm", DATASTORE_TYPE_INT, "%02d"),
      DataStore("clock_time_ss", DATASTORE_TYPE_INT, "%02d"),
      DataStore("clock_date_dow", DATASTORE_TYPE_CHAR, "%3s"),
      DataStore("clock_date_year", DATASTORE_TYPE_INT, "%04d"),
      DataStore("clock_date_mon", DATASTORE_TYPE_CHAR, "%3s"),
      DataStore("clock_date_day", DATASTORE_TYPE_INT, "%02d"),
      DataStore("clock_time_dots", DATASTORE_TYPE_BOOL),
    };
        
    void initializeScreen();
    void clearScreen(bool clearClock, bool clearStatus);
    void finishSetup();
    void loop();

    void updateStatus(char *str, uint16_t colour);
    void updateStatus(char *str, uint16_t colour, bool persist);

    void render();
    void renderWidget(int c);
    void renderFrame(int c);
    void renderGrid();
    void renderActiveFrames();
    void activityLight(int light, bool activity);
    void activityLight(int light, bool activity, uint16_t myColour);
    void updateClock(int hour, int minute, int second, char *dow, int year, char *month, int day);

    void updateSlotPosition(int slot);

    void addClock(int slot);
    void addWidget(int slot, int widgetType, char *title, char *unit, DataStore &ds);
    void addButton(int slot, int widgetType, char *title);

    Adafruit_ILI9341 *screen;

    long loopRan = 0;
    int  loopHold = 250;

    struct uiWidget_t {
      char title[32];
      char unit[3];
      int  widgetType = 0;

      DataStore *ds;
      uiPosition_t pos;
      
      long _lastDrawn = 0;
      long _lastTitle  = 0;
      bool _active = false;
      bool _updates = false;

    } _widgets[UI_SLOTS_TOTAL];

  private:
       
    void _initializeBacklight();
    void _initializeWidgets();

    void _drawDivider(int x);

    void _drawClockComponent(uiWidget_t* widget);    
    void _drawWidget_Float(uiWidget_t* widget);
    void _drawWidget_Bulb(uiPosition_t pos, bool state);
    void _drawButton(uiPosition_t pos, char *title);
    void _drawWidget_House(uiWidget_t* widget);
    
    void _drawTitle(uiPosition_t pos, char *title);
    void _drawUnit(uiPosition_t pos, char *unit);
    void _drawFrame_Float(uiPosition_t pos, char *title, char *unit);
    void _drawFrame_Bulb(uiPosition_t pos, char *title, char *unit);

    void _updatePosition(int slot);
    bool _drawnGrid = false;  /* has the grid been drawn */
    bool _screenInit = false; /* has the screen been init */
    
    /* Backlight LED Control */
    int _pinLED = 0;
    int pwm_freq = 5000;
    int pwm_ledChannel = 8;
    int pwm_resolution = 1024;

    /* When UI is ready */
    void _ready();
    
    /* Status Bar */
    uint16_t  _lastStatusPermanentColour;
    char      _lastStatusPermanent[255];
    long      _lastStatusUpdate = 0;
    uint16_t  _lastStatusColour;
    
    /* Status Timeout */
    int       _statusTimeout = 10;

    
};

#endif

