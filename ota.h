void ota_setup() {
  ArduinoOTA
  .onStart([]() {
    ui.updateStatus("Rcv. OTA Update", ui.screen->color565(255,0,0), false);
  })
  .onEnd([]() {
    ui.updateStatus("Finishing Update", ui.screen->color565(0,255,0), false);
  })
  .onProgress([](unsigned int progress, unsigned int total) {
//    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
//    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) ui.updateStatus("OTA: Auth Failed", ui.screen->color565(255,0,0), false);
    else if (error == OTA_BEGIN_ERROR) ui.updateStatus("OTA: Begin Failed", ui.screen->color565(255,0,0), false);
    else if (error == OTA_CONNECT_ERROR) ui.updateStatus("OTA: Connect Failed", ui.screen->color565(255,0,0), false);
    else if (error == OTA_RECEIVE_ERROR) ui.updateStatus("OTA: Rcv Error", ui.screen->color565(255,0,0), false);
    else if (error == OTA_END_ERROR) ui.updateStatus("OTA: End Failed", ui.screen->color565(255,0,0), false);
  });

  ArduinoOTA.begin();
}

