
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

  bool isTouched = (*ts).touched();

  if (isTouched && !isLong) 
  {       
    if(!wasTouched) wasTouchedAt = millis();
    else if(millis() - wasTouchedAt > 750) isLong = true;
    wasTouched = true;
  } 
    else if(wasTouched && (!isTouched || isLong)) 
  {   
  
    TS_Point p = (*ts).getPoint();         
    ts_x = p.x; ts_y = p.y; ts_z = p.z;      
    ts_d = (millis() - wasTouchedAt);

    if(lastDuration == 0 || ts_d < lastDuration) {
      handleTouch(ps, ts_x, ts_y, ts_z, ts_d);
      lastDuration = ts_d;
    }       
  }

  if(!isTouched) { isLong = false; wasTouched = false; wasTouchedAt = 0; lastDuration = 0; }
  
};


void UI_Touch::handleTouch(PubSubClient * ps, int x, int y, int z, int d) {
  char event[255], xyz[32], msg[255];
  bool hasEvent = false;
  int convertedX = 0, convertedY = 0;

  convertedX = map(x, ts_minx, ts_maxx, 0, (*ui).screen->width());
  convertedY = map(y, ts_miny, ts_maxy, 0, (*ui).screen->height());

  sprintf(xyz, "[%d, %d, %d, %d]", convertedX, convertedY, z, d);
  (*ps).publish("display/kitchen/touch/position_xyzd", xyz);

  if(convertedX > 0 && convertedX < (*ui).screen->width() && 
     convertedY > 0 && convertedY < 50) {
      strcpy(event, "clock");
      hasEvent = true;
  }
  
  for(unsigned c = 0; c < UI_SLOTS_TOTAL; c++) {
    if((*ui)._widgets[c]._active && (*ui)._widgets[c].ds != NULL) {

      UI::uiPosition_t * ep = &((*ui)._widgets[c].pos);

      if(convertedX > ep->x && convertedX < ep->x + ep->w &&
         convertedY > ep->y && convertedY < ep->y + ep->h ) {
          
          sprintf(event, "%s", (*ui)._widgets[c].ds->_name);  
          (*ps).publish("display/kitchen/touch/target", event);
          
          sprintf(msg, "Sending %s event", (*ui)._widgets[c].ds->_name);          
          (*ui).updateStatus(msg, ILI9341_PURPLE);
          
          hasEvent = true;
      }
    }
  }


}

