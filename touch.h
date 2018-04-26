
#ifndef H_UI_TOUCH
#define H_UI_TOUCH

#include <PubSubClient.h>
#include <XPT2046_Touchscreen.h>
#include "UI.h"

class UI_Touch {
  
  public:
    UI_Touch(UI * userInterface, XPT2046_Touchscreen * touchScreen);

    void begin();
    void loop(PubSubClient * ps);    

    XPT2046_Touchscreen * ts;
    UI * ui;
    PubSubClient * ps;
    
    TS_Point lastPoint;
    
    int ts_minx = 351;
    int ts_miny = 262;
    int ts_maxx = 3777;
    int ts_maxy = 3771;
    
    int smallX = 0;
    int smallY = 0;
    int largeX = 0;
    int largeY = 0;
    int convertedX = 0;
    int convertedY = 0;

  private:

 //   void _processTouch(const TS_Point * p);
    
    bool wasTouched = true;
    int  wasTouchedAt = 0;
};

#endif

/*  
  if (strcmp(topic, "display/kitchen/setting/ts_minx") == 0) {
    ts_minx = atoi(pl);
  }

  if (strcmp(topic, "display/kitchen/setting/ts_maxx") == 0) {
    ts_maxx = atoi(pl);
  }
  
  if (strcmp(topic, "display/kitchen/setting/ts_miny") == 0) {
    ts_miny = atoi(pl);
  }

  if (strcmp(topic, "display/kitchen/setting/ts_maxy") == 0) {
    ts_maxy = atoi(pl);
  }
*/

