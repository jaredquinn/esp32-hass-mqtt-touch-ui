
#include "Adafruit_ILI9341.h"

#include "UI.h"
#include "pins.h"

UI::UI(Adafruit_ILI9341& scrn) {
  screen = &scrn;

  pwm_freq = 5000;
  pwm_ledChannel = 0;
  pwm_resolution = 8;

  _drawnGrid = false;
  _initializeWidgets();
  _initializeBacklight();
  _initializeScreen();

  *clock.hour = DataStore("clock_hour", DATASTORE_TYPE_INT);
  *clock.minute = DataStore("clock_minute", DATASTORE_TYPE_INT);
  *clock.second = DataStore("clock_second", DATASTORE_TYPE_INT);
  *clock.dayofweek = DataStore("clock_dayofweek", DATASTORE_TYPE_CHAR);
  *clock.year = DataStore("clock_year", DATASTORE_TYPE_INT);
  *clock.month = DataStore("clock_month", DATASTORE_TYPE_CHAR);
  *clock.day = DataStore("clock_day", DATASTORE_TYPE_INT);

  
};

void UI::finishSetup() {
  clearScreen(true, false);
  _ready();
};

    
void UI::updateClock(int h, int m, int s, char *dw, int y, char *mw, int d) {
  clock.hour->update( h);
  clock.minute->update( m );
  clock.second->update( s );
  clock.dayofweek->update( dw );
  clock.year->update( y );
  clock.month->update( mw );
  clock.day->update( d );
};


void UI::loop() {
  renderActiveFrames();
  render();
}

void UI::_initializeScreen() {
  screen->begin();
  clearScreen(true, true);

  updateStatus(UI_MSG_INIT, ILI9341_BLUE, true);

  screen->setCursor(0, 0);
  screen->setTextColor(ILI9341_WHITE);
  screen->setTextSize(1);
  screen->println("CatsLair IOT Framework Booting 0.1a...");

  screen->print("Running on core ");
  screen->println(xPortGetCoreID());
};


void UI::_initializeBacklight() {
  ledcSetup(pwm_ledChannel, pwm_freq, pwm_resolution);
  ledcAttachPin(PIN_TFT_LED, pwm_ledChannel);
  ledcWrite(pwm_ledChannel, 255);
};


void UI::_initializeWidgets() {
  /* Initialize our Widgets Page */
  for (int c = 0; c <= UI_SLOTS_TOTAL; c++) {
    strcpy(_widgets[c].title, "");
    strcpy(_widgets[c].unit, "");
    _widgets[c].widgetType = UI_WIDGET_NONE;
    _widgets[c].ds = NULL;
    _widgets[c]._active = false;
    _widgets[c]._lastDrawn = 0;
    _widgets[c]._lastTitle = 0;
  }
};

void UI::clearScreen(bool clearClock, bool clearStatus) {
  int top = 0;
  int bottom = 320;

  if (clearStatus && clearClock) {
    screen->fillScreen(ILI9341_BLACK);
  } else {
    if (clearStatus) bottom = 320;
    else bottom = 300;
    if (clearClock) top = 0;
    else top = 49;
    screen->fillRect(0, top, screen->width(), bottom, ILI9341_BLACK);
  }
};

void UI::_drawButton(int column, int row, int width, int height, char *title) {
  screen->fillRect(column + 6, row + 10, 34, 24, ILI9341_PURPLE);
  screen->setCursor(column + 8, row + 18);
  screen->setTextColor(ILI9341_WHITE, ILI9341_PURPLE);
  screen->setTextSize(1);
  screen->print(title);
};

void UI::_drawTitle(int column, int row, char *title) {
  screen->setTextSize(1);
  screen->setCursor(column, row);
  screen->setTextColor(ILI9341_BLUE, ILI9341_BLACK);
  screen->print(title);
};

void UI::_drawUnit(int column, int row, char *symbol) {
  screen->setTextSize(2);

  if (symbol[0] == 'C') {
    screen->drawCircle(column + 52, row + 13, 3, ILI9341_DARKGREY);
    screen->setCursor(column + 62, row + 8);
  } else {
    screen->setCursor(column + 50, row + 8);
  }
  screen->setTextColor(ILI9341_DARKGREY, ILI9341_BLACK);
  screen->print(symbol);

};

void UI::_drawClockComponent(uiPosition_t pos, char *value) {

  if(pos.slot == UI_CLOCK_TIME_DOTS) {
    screen->fillRect(pos.x, pos.y, 4, 4, UI_CLOCK_COLOUR);  
    screen->fillRect(pos.x, pos.y + 10, 4, 4, UI_CLOCK_COLOUR);
  } else {
    if(pos.fs > 0) screen->setTextSize(pos.fs);
    screen->setTextColor(UI_CLOCK_COLOUR, ILI9341_BLACK);
    screen->setCursor(pos.x, pos.y);
    screen->printf(value);
  }
};


void UI::_drawWidget_Float(int column, int row, float value) {

  int IntegerPart = (int)(value);
  int DecimalPart = 100 * (value - IntegerPart);

  screen->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  screen->setCursor(column, row + 12);
  screen->setTextSize(4);
  screen->printf("%02d", IntegerPart);
  screen->setCursor(column + 50, row + 26);
  screen->setTextSize(2);
  screen->printf("%02d", DecimalPart);
};


void UI::_drawWidget_Bulb(int column, int row, bool state) {
  uint16_t colour;

  if (state == true) colour = screen->color565(255, 99, 71);
  else colour = screen->color565(90, 90, 90);

  screen->fillRect(column + 14, row + 28, 11, 15, colour);
  screen->fillRoundRect(column + 5, row + 10, 28, 28, 14, colour);
};

void UI::_drawWidget_House(int column, int row, char *state) {

  uint16_t colour;
  char str[6];

  colour = ILI9341_DARKGREY;

  strcpy(str, "");
  if (strcmp(state, "disarmed") == 0)  {
    strcpy(str, "DSARM");
    colour = ILI9341_GREEN;
  }
  if (strcmp(state, "pending") == 0)   {
    strcpy(str, "PEND ");
    colour = ILI9341_ORANGE;
  }
  if (strcmp(state, "armed_away") == 0) {
    strcpy(str, "AWAY ");
    colour = ILI9341_RED;
  }
  if (strcmp(state, "armed_home") == 0) {
    strcpy(str, "HOME ");
    colour = ILI9341_BLUE;
  }

  screen->fillTriangle(column + 4, row + 30, column + 20, row + 20, column + 36, row + 30, colour);
  screen->fillRect(column + 10, row + 30, 20, 10, colour);
  screen->fillRect(column + 15, row + 32, 10, 13, ILI9341_BLACK);

  screen->setTextSize(1);
  screen->setTextColor(colour, ILI9341_BLACK);
  screen->setCursor(column + 6, row + 10);
  screen->print(str);

};

void UI::activityLight(int light, bool activity) {
  uint16_t colour;

  if (activity == true) colour = ILI9341_RED;
  else colour = ILI9341_BLACK;

  screen->fillRect(234, (light * 6) + 2, 6, 4, colour);
}


void UI::renderWidget(int c) {
  uiPosition_t pos = getPositionForSlot(c);

  if (_widgets[c].ds != NULL) {
    
    switch (_widgets[c].widgetType) {
      case UI_WIDGET_BULB:
        _drawWidget_Bulb(pos.x, pos.y, _widgets[c].ds->getBoolValue());
        break;
      case UI_WIDGET_HOUSE:
        _drawWidget_House(pos.x, pos.y, _widgets[c].ds->getCharValue());
        break;
      case UI_WIDGET_FLOAT:
        _drawWidget_Float(pos.x, pos.y, _widgets[c].ds->getFloatValue());
        break;
      case UI_WIDGET_CLOCK:
        char str[10];
        if(c == UI_CLOCK_TIME_HH || c == UI_CLOCK_TIME_MM || \
           c == UI_CLOCK_TIME_SS || c == UI_CLOCK_DATE_DAY)  sprintf(str, "%02d", _widgets[c].ds->getIntValue());
        if(c == UI_CLOCK_DATE_YEAR)                          sprintf(str, "%04d", _widgets[c].ds->getIntValue());
        if(c == UI_CLOCK_DATE_DOW || c == UI_CLOCK_DATE_MON) strcpy(str, _widgets[c].ds->getCharValue());        
        if(c == UI_CLOCK_TIME_DOTS)                          strcpy(str, "");
        
        _drawClockComponent(pos, str);
        break;
    }
  }
}

void UI::renderFrame(int c) {

  uiPosition_t pos = getPositionForSlot(c);

  switch (_widgets[c].widgetType) {
    case UI_WIDGET_BULB:
      _drawTitle(pos.x, pos.y, _widgets[c].title);
      break;
    case UI_WIDGET_HOUSE:
      _drawTitle(pos.x, pos.y, _widgets[c].title);
      break;
    case UI_WIDGET_BUTTON:
      _drawButton(pos.x, pos.y, pos.w, pos.h, _widgets[c].title);
      break;
    case UI_WIDGET_FLOAT:
      _drawTitle(pos.x, pos.y, _widgets[c].title);
      _drawUnit(pos.x, pos.y, _widgets[c].unit);
      break;
  }
  return;
}

void UI::renderGrid() {
  _drawDivider(50);
  _drawDivider(100);
  _drawDivider(150);
  _drawDivider(200);
}

void UI::renderActiveFrames() {
  if (!_drawnGrid) {
    renderGrid();
    _drawnGrid = true;
  }
  for (int c = 0; c < UI_SLOTS_TOTAL; c++) {
    if (_widgets[c]._active == true) {
      if (millis() - _widgets[c]._lastTitle > 10000) {
        _widgets[c]._lastTitle = millis() - random(1000);
        renderFrame(c);
      }
    }
  }
}

void UI::render() {
  for (int c = 0; c < UI_SLOTS_TOTAL ; c++) {

    if (_widgets[c]._active == true && _widgets[c].ds != NULL) {
      if (_widgets[c].ds->lastChange() > _widgets[c]._lastDrawn) {
        _widgets[c]._lastDrawn = millis();
        renderWidget(c);
      }
    }
  }
};


void UI::_drawDivider(int x) {
  screen->drawLine(0, x - 5, screen->width(), x - 5, screen->color565(32, 32, 32));
}


void UI::addButton(int slot, int widgetType, char *title) {
  strcpy(_widgets[slot].title, title);
  _widgets[slot].widgetType = widgetType;
  _widgets[slot].ds = NULL;
  _widgets[slot]._active = true;
  _widgets[slot]._lastDrawn = 0;
  _widgets[slot]._lastTitle = 0;
  _widgets[slot]._updates = false;
}

void UI::addWidget(int slot, int widgetType, char *title, char *unit, DataStore& ds) {
  addButton(slot, widgetType, title);
  
  strcpy(_widgets[slot].unit, unit);
  _widgets[slot].ds = &ds;
  _widgets[slot]._updates = true;
};

void UI::addClock(int slot, DataStore& ds) {
  _widgets[slot].ds = &ds;
  _widgets[slot]._active = true;
  _widgets[slot]._lastDrawn = 0;
  _widgets[slot]._lastTitle = 0;
  _widgets[slot]._updates = true;
};

void UI::updateStatus(char *str, uint16_t colour, bool temporary) {
  screen->fillRect(0, 300, 240, 20, colour);
  screen->setTextSize(2);
  screen->setTextColor(ILI9341_WHITE, colour);
  screen->setCursor(0, 302);
  screen->print(str);

  if (temporary) {
    _lastStatusUpdate = millis();
  } else {
    strcpy(_lastStatusPermanent, str);
    _lastStatusUpdate = -1;
  };

};

void UI::_ready() {
  updateStatus(UI_MSG_WELCOME, ILI9341_BLUE, true);
};


UI::uiPosition_t UI::getPositionForSlot(int slot) {

  uiPosition_t pos;

  if(slot <= UI_SLOTS_MAX_CLOCK) {
    pos.h = 50;
    switch(slot) {
      case UI_CLOCK_TIME_HH:   pos.x = 6;   pos.y = 4; pos.fs = 4; 
      case UI_CLOCK_TIME_MM:   pos.x = 64;  pos.y = 4; pos.fs = 4; 
      case UI_CLOCK_TIME_SS:   pos.x = 116; pos.y = 4; pos.fs = 2; 
      case UI_CLOCK_TIME_DOTS: pos.x = 52;  pos.y = 12; 
      case UI_CLOCK_DATE_MON:  pos.x = 146; pos.y = 18; pos.fs = 2;
      case UI_CLOCK_DATE_DOW:  pos.x = 118; pos.y = 24; pos.fs = 1; 
      case UI_CLOCK_DATE_DAY:  pos.x = 186; pos.y = 4; pos.fs = 4; 
      case UI_CLOCK_DATE_YEAR: pos.x = 150; pos.y = 4; pos.fs = 1; 
    }   
  }

  if(slot > UI_SLOTS_MAX_CLOCK && slot < UI_BUTTON_1) {
    int wSlot = slot - UI_SLOTS_MAX_CLOCK;
    
    if (wSlot % 4 == 0 || wSlot % 4 == 1) pos.x = wSlot % 4 * 80;
    if (wSlot % 4 == 2 || wSlot % 4 == 3) pos.x = 160 + (((wSlot % 4) - 2) * 40);
    pos.y = (int(wSlot / 4) + 1) * 50;
  }

  if(slot >= UI_BUTTON_1 <= UI_SLOTS_TOTAL) {
    int bSlot = slot - UI_BUTTON_1;
    pos.y = 250;
    pos.x = (bSlot % 8) * 40; 
  }

  pos.slot = slot;
  return pos;  
};

