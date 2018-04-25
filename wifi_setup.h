
void local_wifi_setup() {

  delay(10);
  
  // We start by connecting to a WiFi network
  #ifdef USE_SERIAL
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
  #endif
  
  WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef USE_SERIAL
    Serial.print(".");
    #endif
  }

  randomSeed(micros());

  myscreen.println("WiFi Connected");
  myscreen.print("IP address: ");

  #ifdef USE_SERIAL
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  #endif

};


