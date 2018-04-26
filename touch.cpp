
#include "touch.h"

UI_Touch::UI_Touch(UI * userInterface, XPT2046_Touchscreen * touchScreen) {
  ui = userInterface;
  ts = touchScreen;
};

  
void UI_Touch::begin() {
  if (!(*ts).begin()) {
   #ifdef USE_SERIAL
   Serial.println("Failed to Start Touch");
   #endif
   (*ui).screen->println("Failed to Start Touch");
  }
  (*ts).setRotation(2);
};

void UI_Touch::loop(PubSubClient * ps) {
  char event[255];
  char xyz[20];

  bool hasEvent = false;

  bool isTouched = (*ts).touched();

  if (isTouched) {
    
    TS_Point p = (*ts).getPoint();   
     
    if(p != lastPoint) {
      
      convertedX = map(p.x, ts_minx, ts_maxx, 0, (*ui).screen->width());
      convertedY = map(p.y, ts_miny, ts_maxy, 0, (*ui).screen->height());

      sprintf(xyz, "[%d, %d, %d]", convertedX, convertedY, p.z);
      (*ps).publish("display/kitchen/touch/position", xyz);

      if(convertedX > 0 && convertedX < (*ui).screen->width() && convertedY > 0 && convertedY < 50) {
          strcpy(event, "clock");
          hasEvent = true;
      }
      
      for(unsigned c = 0; c < UI_SLOTS_TOTAL; c++) {
        if((*ui)._widgets[c]._active && (*ui)._widgets[c].ds != NULL) {

          UI::uiPosition_t * ep = &((*ui)._widgets[c].pos);

          if(convertedX > ep->x && convertedX < ep->x + ep->w &&
             convertedY > ep->y && convertedY < ep->y + ep->h ) {
              sprintf(event, "%s", (*ui)._widgets[c].ds->_name);  
              hasEvent = true;
          }
        }
      }

      if(hasEvent) {
           (*ps).publish("display/kitchen/touch/target", event);
      }
 
    }
  }
};


