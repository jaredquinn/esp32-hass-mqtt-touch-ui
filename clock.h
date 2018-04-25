/**
 * clock.h
 * UI Interface Wrapper for NTPClient for syncing clock with NTP
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


#ifndef CLOCK_H
#define CLOCK_H

#include "UI.h"
#include <NTPClient.h>

const static char CLOCK_MONTHS[12][4] = { "JAN", "FEB", "MAR", "APR","MAY","JUN","JUL","AUG","SEP","OCT", "NOV", "DEC" };
const static char CLOCK_DAYS[7][4] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

class UI_Clock_NTP  {
  public:
    UI_Clock_NTP(UI& ui, NTPClient& ntp);
    int syncTimeout;
    
    void setOffset(int offset);
    void loop();
    void setSyncFrequency(int seconds);

  private:

    void _updateClock();
    
    unsigned long _lastTime;
    
    long _lastUpdate = 0;
    long _lastSync = 0;

    int _offset = 36000;
    int _syncSeconds = 120;
    
    UI* _ui;
    NTPClient* _ntp;
    
    
};

#endif

