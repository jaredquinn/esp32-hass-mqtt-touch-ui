
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
};

void UI::finishSetup() {
  clearScreen(true, false);
}

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
  for(int c=0; c<=UI_WIDGET_SLOTS; c++) {
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
  
  if(clearStatus && clearClock) {
    screen->fillScreen(ILI9341_BLACK);
  } else {
    if(clearStatus) bottom = 320;
    else bottom = 300;
    if(clearClock) top = 0;
    else top = 49;
    screen->fillRect(0, top, screen->width(), bottom, ILI9341_BLACK);
  }
};

void UI::_drawButton(int column, int row, int width, int height, char *title) {
  screen->fillRect(column+6, row+10, 34, 24, ILI9341_PURPLE);
  screen->setCursor(column+8, row+18);
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
  
  if(state == true) colour = screen->color565(255,99,71);
  else colour = screen->color565(90,90,90);

  screen->fillRect(column+14, row+28, 11, 15, colour);
  screen->fillRoundRect(column+5, row+10, 28, 28, 14, colour);
};

void UI::_drawWidget_House(int column, int row, char *state) {
  
  uint16_t colour;
  colour = ILI9341_DARKGREY;

  if(strcmp(state, "disarmed") == 0) colour = ILI9341_GREEN;
  if(strcmp(state, "pending") == 0) colour = ILI9341_ORANGE;
  if(strcmp(state, "armed_away") == 0) colour = ILI9341_RED;
  if(strcmp(state, "armed_home") == 0) colour = ILI9341_BLUE;
  
  screen->fillTriangle(column+4, row+25, column+20, row+10, column+36, row+25, colour);
  screen->fillRect(column+10, row+25, 20, 20, colour); 
};


int UI::_getSlotX(int slot) {
  if(slot <= UI_WIDGET_SLOTS) {
    if(slot > 15) {
      return (slot % 8) * 40;
    }
    if(slot % 4 < 2) {
      return (slot % 4) * 80;
    }
    if(slot % 4 >= 2) {
      return 160 + (((slot % 4)-2) * 40);
    }
  }
  return 0;
};

int UI::_getSlotY(int slot) {
  if(slot <= UI_WIDGET_SLOTS) {
    if(slot > 15 && slot < 23) {
      return 250;
    }
    return (int(slot / 4) + 1) * 50;
  }
  return 0;
};

int UI::_getSlotW(int slot) {
  if(slot <= UI_WIDGET_SLOTS) {
    if(slot % 4 < 2) return 90;
    if(slot % 4 >= 2) return 40;
  }
  return 0;
};

int UI::_getSlotH(int slot) {
  return 50;
};

void UI::renderWidget(int c) {
  if(_widgets[c].ds != NULL) {
    switch(_widgets[c].widgetType) {
      case UI_WIDGET_BULB:  
          _drawWidget_Bulb(_getSlotX(c), _getSlotY(c), _widgets[c].ds->getBoolValue()); 
          break;
      case UI_WIDGET_HOUSE:
          _drawWidget_House(_getSlotX(c), _getSlotY(c), _widgets[c].ds->getCharValue()); 
          break;
      case UI_WIDGET_FLOAT: 
          _drawWidget_Float(_getSlotX(c), _getSlotY(c), _widgets[c].ds->getFloatValue()); 
          break;
    }
  }
}

void UI::renderFrame(int c) {
  int x = _getSlotX(c);
  int y = _getSlotY(c);
  int w = _getSlotW(c);
  int h = _getSlotH(c);

 switch(_widgets[c].widgetType) {
    case UI_WIDGET_BULB:
        _drawTitle(x,y, _widgets[c].title);
        break; 
    case UI_WIDGET_HOUSE:
        _drawTitle(x,y, _widgets[c].title);
        break;     
    case UI_WIDGET_BUTTON:
        _drawButton(x,y, w, h, _widgets[c].title);
        break;
    case UI_WIDGET_FLOAT: 
        _drawTitle(x,y, _widgets[c].title);
        _drawUnit(x,y, _widgets[c].unit);
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
  if(!_drawnGrid) {
    renderGrid();
    _drawnGrid = true;
  }
  for(int c=0; c<UI_WIDGET_SLOTS; c++) {
    if(_widgets[c]._active == true) {
      if(millis() - _widgets[c]._lastTitle > 10000) {
         _widgets[c]._lastTitle = millis() - random(1000);
         renderFrame(c); 
      }
    }    
  }
}

void UI::render() {
  for(int c=0; c<UI_WIDGET_SLOTS; c++) {

    if(_widgets[c]._active == true && _widgets[c].ds != NULL) {
      if(_widgets[c].ds->lastChange() > _widgets[c]._lastDrawn) {
         _widgets[c]._lastDrawn = millis();         
         renderWidget(c); 
      }
    }    
  }
};


void UI::_drawDivider(int x) {
  screen->drawLine(0, x-5, screen->width(), x-5, screen->color565(32,32,32));
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
    strcpy(_widgets[slot].title, title);
    strcpy(_widgets[slot].unit, unit);
    _widgets[slot].widgetType = widgetType;
    _widgets[slot].ds = &ds;
    _widgets[slot]._active = true;
    _widgets[slot]._lastDrawn = 0;
    _widgets[slot]._lastTitle = 0;
    _widgets[slot]._updates = true;
};

void UI::updateStatus(char *str, uint16_t colour, bool temporary) {
  screen->fillRect(0, 300, 240, 20, colour);
  screen->setTextSize(1);
  screen->setTextColor(ILI9341_WHITE, colour);
  screen->setCursor(0, 302);
  screen->print(str);

  if(temporary) {
    _lastStatusUpdate = millis();
  } else {
    strcpy(_lastStatusPermanent, str);
    _lastStatusUpdate = -1;
  };

};

void UI::_ready() {
  updateStatus(UI_MSG_WELCOME, ILI9341_BLUE, true);
};



