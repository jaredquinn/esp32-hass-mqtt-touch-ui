
void local_wifi_setup() {

  delay(10);
  // We start by connecting to a WiFi network
  if (USE_SERIAL) {
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
  }
  WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (USE_SERIAL) {
      Serial.print(".");
    }
  }

  randomSeed(micros());

  myscreen.println("WiFi Connected");
  myscreen.print("IP address: ");

  if (USE_SERIAL) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
};

