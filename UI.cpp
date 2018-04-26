/**
 * UI.cpp 
 * UI Interface Rendering Class
 * 
 * The MIT License (MIT)
 * Copyright (c) 2018 by Jared Quinn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Adafruit_ILI9341.h"

#include "UI.h"
#include "datastore.h"

UI::UI(Adafruit_ILI9341& scrn, int ledPin=0) {
  screen = &scrn;

  if(ledPin > 0) _pinLED = ledPin;
  _initializeBacklight();
  _initializeWidgets();

};

void UI::finishSetup() {

  for(int v = 0; v < UI_SLOTS_MAX_CLOCK; v++) {  
    addClock(v); 
  }

  clearScreen(true, false);
  _screenInit = true;
  _ready();
};


void UI::loop() {
  if(!_screenInit) return;
  
  if(_lastStatusUpdate > 0 && millis() - _lastStatusUpdate > (_statusTimeout*1000)) {
    updateStatus(_lastStatusPermanent, _lastStatusPermanentColour, true);
  }

  if(loopRan == 0 || millis() - loopRan > loopHold) {
    renderActiveFrames();
    render();
  }  

}

void UI::initializeScreen() {
  
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
  if(_pinLED > 0) {
    ledcSetup(pwm_ledChannel, pwm_freq, pwm_resolution);
    ledcAttachPin(_pinLED, pwm_ledChannel);
    ledcWrite(pwm_ledChannel, 255);
  }
};


void UI::_initializeWidgets() {
  if(!_screenInit) return;
  
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


void UI::_drawButton(uiPosition_t pos, char *title) {
  if(!_screenInit) return;
  
  screen->fillRect(pos.x + 6, pos.y + 10, 34, 24, ILI9341_PURPLE);
  screen->setCursor(pos.x + 8, pos.y + 18);
  screen->setTextColor(ILI9341_WHITE, ILI9341_PURPLE);
  screen->setTextSize(1);
  screen->print(title);
};

void UI::_drawTitle(uiPosition_t pos, char *title) {
  if(!_screenInit) return;

  screen->setTextSize(1);
  screen->setCursor(pos.x, pos.y);
  screen->setTextColor(ILI9341_BLUE, ILI9341_BLACK);
  screen->print(title);
};

void UI::_drawUnit(uiPosition_t pos, char *symbol) {
  if(!_screenInit) return;
  
  screen->setTextSize(2);

  if (symbol[0] == 'C') {
    screen->drawCircle(pos.x + 52, pos.y + 13, 3, ILI9341_DARKGREY);
    screen->setCursor(pos.x + 62, pos.y + 8);
  } else {
    screen->setCursor(pos.x + 50, pos.y + 8);
  }
  screen->setTextColor(ILI9341_DARKGREY, ILI9341_BLACK);
  screen->print(symbol);

};

void UI::_drawClockComponent(uiWidget_t* w) {

  uint16_t colour;
  char cmpa[255], cmpb[255];

  if((*w).pos.slot == enumSlot::UI_CLOCK_TIME_DOTS) {

    if((*w).ds->getBoolValue() == true) colour = UI_CLOCK_COLOUR;
    else colour = ILI9341_BLACK;
    
    screen->fillRect((*w).pos.x, (*w).pos.y + 8, 4, 4, UI_CLOCK_COLOUR);  
    screen->fillRect((*w).pos.x, (*w).pos.y + 18, 4, 4, UI_CLOCK_COLOUR);
    
  } else {

    if((*w).pos.fs > 0) screen->setTextSize((*w).pos.fs);
    screen->setTextColor(UI_CLOCK_COLOUR, ILI9341_BLACK);
    screen->setCursor((*w).pos.x, (*w).pos.y);
    screen->printf((*w).ds->renderValue);
  }
  
};


void UI::_drawWidget_Float(uiWidget_t* w) {
  
  if(!_screenInit) return;

  float f = (*w).ds->getFloatValue();
  
  int IntegerPart = (int)(f);
  int DecimalPart = 100 * ((f*1000 - IntegerPart*1000)/1000);

  screen->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  screen->setCursor((*w).pos.x, (*w).pos.y + 12);
  screen->setTextSize(4);
  screen->printf("%02d", IntegerPart);
  screen->setCursor((*w).pos.x + 50, (*w).pos.y + 26);
  screen->setTextSize(2);
  screen->printf("%02d", DecimalPart);
};


void UI::_drawWidget_Bulb(uiPosition_t pos, bool state) {
  if(!_screenInit) return;

  uint16_t colour;
  
  if (state == true) colour = screen->color565(255, 99, 71);
  else colour = screen->color565(90, 90, 90);

  screen->fillRect(pos.x + 14, pos.y + 28, 11, 15, colour);
  screen->fillRoundRect(pos.x + 5, pos.y + 10, 28, 28, 14, colour);
};

void UI::_drawWidget_House(uiWidget_t* w) {
  
  if(!_screenInit) return;

  uint16_t colour;
  
  char str[6];
  char state[16];
  strcpy(state, (*w).ds->_charValue);
  
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

  screen->fillTriangle((*w).pos.x + 4, (*w).pos.y + 30, (*w).pos.x + 20, (*w).pos.y + 20, (*w).pos.x + 36, (*w).pos.y + 30, colour);
  screen->fillRect((*w).pos.x + 10, (*w).pos.y + 30, 20, 10, colour);
  screen->fillRect((*w).pos.x + 15, (*w).pos.y + 32, 10, 13, ILI9341_BLACK);

  screen->setTextSize(1);
  screen->setTextColor(colour, ILI9341_BLACK);
  screen->setCursor((*w).pos.x + 6, (*w).pos.y + 10);
  screen->print(str);

};

void UI::activityLight(int light, bool activity, uint16_t myColour) {
  uint16_t colour;

  if (activity == true) colour = ILI9341_RED;
  else colour = ILI9341_BLACK;

  screen->fillRect(234, (light * 6) + 2, 6, 4, colour);
};

void UI::activityLight(int light, bool activity) {
  activityLight(light, activity, ILI9341_RED);
};


void UI::renderWidget(int c) {

  if(!_screenInit) return;
  activityLight(2, true, ILI9341_ORANGE);


  if (_widgets[c].ds != NULL) {
    char str[10];
        
    switch (_widgets[c].widgetType) {
      case UI_WIDGET_BULB:
        _drawWidget_Bulb(_widgets[c].pos, _widgets[c].ds->getBoolValue());
        break;

      case UI_WIDGET_HOUSE:         
        _drawWidget_House(&_widgets[c]);
        break;
        
      case UI_WIDGET_FLOAT:
        _drawWidget_Float(&_widgets[c]);
        break;
        
      case UI_WIDGET_CLOCK:
        _drawClockComponent(&_widgets[c]);
        break;
    }
  }
  activityLight(2, false);

}

void UI::renderFrame(int c) {
  
  if(!_screenInit) return;

  activityLight(3, true, ILI9341_CYAN);

  switch (_widgets[c].widgetType) {
    case UI_WIDGET_BULB:
      _drawTitle(_widgets[c].pos, _widgets[c].title);
      break;
    case UI_WIDGET_HOUSE:
      _drawTitle(_widgets[c].pos, _widgets[c].title);
      break;
    case UI_WIDGET_BUTTON:
      _drawButton(_widgets[c].pos, _widgets[c].title);
      break;
    case UI_WIDGET_FLOAT:
      _drawTitle(_widgets[c].pos, _widgets[c].title);
      _drawUnit(_widgets[c].pos, _widgets[c].unit);
      break;
  }
  activityLight(3, false);

};

void UI::renderGrid() {
  _drawDivider(50);
  _drawDivider(100);
  _drawDivider(150);
  _drawDivider(200);
  _drawnGrid = true;
};

void UI::renderActiveFrames() {
  if(!_screenInit) return;
  
  if (!_drawnGrid) {
    renderGrid();
    _drawnGrid = true;
  }
  
  for (int c = 0; c < UI_SLOTS_TOTAL; c++) {
    if (_widgets[c]._active == true) {

      if( (_widgets[c].widgetType == UI_WIDGET_BUTTON && _widgets[c]._lastTitle == 0) ||
          (_widgets[c].widgetType != UI_WIDGET_BUTTON && millis() - _widgets[c]._lastTitle > 10000) ) {
            
          renderFrame(c);
          _widgets[c]._lastTitle = millis() - random(1000);        
      }
    }
  }

};

void UI::render() {
  if(!_screenInit) return;

  for (int c = 0; c < UI_SLOTS_TOTAL ; c++) {
      if (_widgets[c]._active == true && _widgets[c].ds != NULL) {
 
        if (_widgets[c].ds->hasChanged) {
          renderWidget(c);
          _widgets[c].ds->resetChange();
          _widgets[c]._lastDrawn = millis();
        }

      }
  }

};


void UI::_drawDivider(int x) {
  screen->drawLine(0, x - 5, screen->width(), x - 5, screen->color565(32, 32, 32));
};

void UI::updateStatus(char *str, uint16_t colour, bool persist) {

  if(colour != _lastStatusColour) screen->fillRect(0, 300, 240, 20,colour);
  _lastStatusColour = colour;
  
  screen->setTextSize(1);;
  screen->setTextColor(ILI9341_WHITE, colour);
  screen->setCursor(0, 302);
  screen->print(str);

  if (persist == true) {
    strcpy(_lastStatusPermanent, str);
    _lastStatusPermanentColour = colour;
    _lastStatusUpdate = -1;
  } else {
    _lastStatusUpdate = millis();
  };
};

void UI::updateStatus(char *str, uint16_t colour) {
  updateStatus(str, colour, false);
};


void UI::addButton(int slot, int widgetType, char *title) {
  strcpy(_widgets[slot].title, title);
  _widgets[slot].widgetType = widgetType;
  _widgets[slot].ds = NULL;
  _widgets[slot]._active = true;
  _widgets[slot]._lastDrawn = 0;
  _widgets[slot]._lastTitle = 0;
  _widgets[slot]._updates = false;

  updateSlotPosition(slot);
}


void UI::addWidget(int slot, int widgetType, char *title, char *unit, DataStore& ds) {
  addButton(slot, widgetType, title);
  
  strcpy(_widgets[slot].unit, unit);
  _widgets[slot].ds = &ds;
  _widgets[slot]._updates = true;

  updateSlotPosition(slot);
};

void UI::addClock(int slot) {
  _widgets[slot].ds = (&clocks[slot]);
  _widgets[slot].widgetType = UI_WIDGET_CLOCK;
  _widgets[slot]._active = true;
  _widgets[slot]._lastDrawn = -1;
  _widgets[slot]._lastTitle = 0;
  _widgets[slot]._updates = true;
  updateSlotPosition(slot);
};


void UI::_ready() {
  updateStatus(UI_MSG_WELCOME, ILI9341_BLUE, true);
};


void UI::updateSlotPosition(int slot) {

  uiPosition_t *p;
  p = &_widgets[slot].pos;
  
  (*p).slot = slot;
  
  if(slot >= 0 && slot < enumSlot::UI_POS_1_1) {  
    switch(slot) {
      case enumSlot::UI_CLOCK_TIME_SS:   (*p).x = 116; (*p).y = 4;  (*p).h = 20; (*p).w = 20; (*p).fs = 2;  break;
      case enumSlot::UI_CLOCK_TIME_MM:   (*p).x = 64;  (*p).y = 4;  (*p).h = 46; (*p).w = 50; (*p).fs = 4; break; 
      case enumSlot::UI_CLOCK_TIME_HH:   (*p).x = 6;   (*p).y = 4;  (*p).h = 46; (*p).w = 50; (*p).fs = 4; break;
      case enumSlot::UI_CLOCK_TIME_DOTS: (*p).x = 55;  (*p).y = 4;  (*p).h = 46; (*p).w = 4; (*p).fs = -1; break;
      case enumSlot::UI_CLOCK_DATE_MON:  (*p).x = 146; (*p).y = 18; (*p).h = 22; (*p).w = 50; (*p).fs = 2; break;
      case enumSlot::UI_CLOCK_DATE_DOW:  (*p).x = 118; (*p).y = 24; (*p).h = 16; (*p).w = 20; (*p).fs = 1; break;
      case enumSlot::UI_CLOCK_DATE_DAY:  (*p).x = 186; (*p).y = 4;  (*p).h = 46; (*p).w = 20; (*p).fs = 4;  break;
      case enumSlot::UI_CLOCK_DATE_YEAR: (*p).x = 150; (*p).y = 4;  (*p).h = 20; (*p).w = 20; (*p).fs = 1; break;
    }   
  } 

  if(slot >= enumSlot::UI_POS_1_1 && slot < enumSlot::UI_BUTTON_1) {  
    int wSlot = (slot - 8);
    
    (*p).x = (wSlot % 4) * 80;
    if(wSlot % 4 == 3) (*p).x = (*p).x - 40;

    (*p).y = (int(wSlot / 4) + 1) * 50;
    (*p).h = 50;
    if(wSlot % 4 < 2) (*p).w = 40;
    else (*p).w = 80; 
  }

  if(slot >= enumSlot::UI_BUTTON_1 && slot <= UI_SLOTS_TOTAL) {
    
    int bSlot = (slot - enumSlot::UI_BUTTON_1);
    (*p).x = (bSlot % 8) * 40;
    (*p).y = 250;
    (*p).h = 50;
    (*p).w = 40;
  }

};

