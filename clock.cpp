/**
 * clock.cpp 
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


#include "clock.h"
#include "UI.h"
#include "datastore.h"
#include <NTPClient.h>

UI_Clock_NTP::UI_Clock_NTP(UI *ui, NTPClient* ntp) {
  _lastUpdate = 0;
  _ui = ui;
  _ntp = ntp;
};

void UI_Clock_NTP::_updateClock() {

  char *dow;
  char *mon;

  strcpy(dow, CLOCK_DAYS[ _ntp->getDay() ]);
  strcpy(mon, CLOCK_MONTHS[ _ntp->getMonth() ]);
  
  _ui->updateClock(
      _ntp->getHours(),
      _ntp->getMinutes(),
      _ntp->getSeconds(),
      dow,
      _ntp->getYear(),
      mon,
      _ntp->getDate() 
  );  

  _lastUpdate = _ntp->getEpochTime();
};


void UI_Clock_NTP::setSyncFrequency(int seconds) {
  _syncSeconds = seconds;
  _ntp->setUpdateInterval(seconds);
};

void UI_Clock_NTP::setOffset(int offset) {
  _offset = offset;
  _ntp->setTimeOffset(offset);
};

void UI_Clock_NTP::loop() {
  long c = _ntp->getEpochTime();
  if(c - _lastUpdate > 1000) {
    _updateClock();
  }
};


