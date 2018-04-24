
#ifndef UI_H
#define UI_H


#define UI_MSG_INIT    "Booting CatsLair IOT"
#define UI_MSG_WELCOME "Welcome to CatsLair"

#define UI_WIDGET_NONE   0
#define UI_WIDGET_FLOAT  1
#define UI_WIDGET_BULB   2
#define UI_WIDGET_BUTTON 3
#define UI_WIDGET_HOUSE  4

#define UI_POS_1_1 0
#define UI_POS_1_2 1
#define UI_POS_1_3 2
#define UI_POS_1_4 3

#define UI_POS_2_1 4
#define UI_POS_2_2 5
#define UI_POS_2_3 6
#define UI_POS_2_4 7

#define UI_POS_3_1 8
#define UI_POS_3_2 9
#define UI_POS_3_3 10
#define UI_POS_3_4 11

#define UI_POS_4_1 12
#define UI_POS_4_2 13
#define UI_POS_4_3 14
#define UI_POS_4_4 15

#define UI_BUTTON_1 16
#define UI_BUTTON_2 17
#define UI_BUTTON_3 18
#define UI_BUTTON_4 19
#define UI_BUTTON_5 20
#define UI_BUTTON_6 21
#define UI_BUTTON_7 21
#define UI_BUTTON_8_22

#include "Adafruit_ILI9341.h"
#include "datastore.h"


#define UI_WIDGET_SLOTS 22



class UI {
  public:
    UI(Adafruit_ILI9341& scrn);
    void clearScreen(bool clearClock, bool clearStatus);
    void updateStatus(char *str, uint16_t colour, bool temporary);
    void finishSetup();
    void loop();
    void render();
    void renderWidget(int c);
    void renderFrame(int c);
    void renderGrid();
    void renderActiveFrames();
    

    void addWidget(int slot, int widgetType, char *title, char *unit, DataStore &ds);
    void addButton(int slot, int widgetType, char *title);

    Adafruit_ILI9341 *screen;

  private:

    struct uiWidget_t {
      char title[32];
      char unit[3];
      int  widgetType;
      DataStore * ds;
      
      long _lastDrawn;
      long _lastTitle;
      bool _active;
      bool _updates;
    } _widgets[UI_WIDGET_SLOTS];
    
    void _initializeBacklight();
    void _initializeScreen();
    void _initializeWidgets();

    void _drawDivider(int x);
    
    int _getSlotX(int slot);
    int _getSlotY(int slot);
    int _getSlotH(int slot);
    int _getSlotW(int slot);

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

