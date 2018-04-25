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

UI_Clock_NTP::UI_Clock_NTP(UI& ui, NTPClient& ntp) {
  _lastUpdate = 0;
  _lastSync = 0;
  
  _ui = &ui;
  _ntp = &ntp;

};

void UI_Clock_NTP::_updateClock() {
  
  _ui->clocks[(int) enumSlot::UI_CLOCK_TIME_SS].update( (int) _ntp->getSeconds() );
  _ui->clocks[(int) enumSlot::UI_CLOCK_TIME_MM].update( (int) _ntp->getMinutes() );
  _ui->clocks[(int) enumSlot::UI_CLOCK_TIME_HH].update( (int) _ntp->getHours() );
  
  _ui->clocks[enumSlot::UI_CLOCK_DATE_YEAR].update( (int) _ntp->getYear() );
  _ui->clocks[enumSlot::UI_CLOCK_DATE_DAY].update( (int) _ntp->getDate() );

  _ui->clocks[enumSlot::UI_CLOCK_DATE_DOW].update( CLOCK_DAYS[ _ntp->getDay() ] );
//  _ui->clocks[enumSlot::UI_CLOCK_DATE_MON].update( CLOCK_MONTHS[ _ntp->getMonth() ] );
  _ui->clocks[enumSlot::UI_CLOCK_TIME_DOTS].update( (bool) ((_ntp->getSeconds() % 2 != 0) ? true : false ) );
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

  if(_lastSync == 0 || millis() - _lastSync > (_syncSeconds * 1000)) {
    _ui->updateStatus("Requesting Time", ILI9341_GREEN);
    _ntp->forceUpdate();
    _lastSync = millis();
    return;
  }
    
  if(millis() - _lastUpdate > 250) {    
    _updateClock();    
    _lastUpdate = millis();
  }
};



