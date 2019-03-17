

void initOTA(){
    ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    clearDisplay();
    Serial.println("Start updating " + type);
   display.println("Start updating " + type);
      display.display();
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    display.println(F("end"));
       display.display();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
   Serial.printf("%u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      display.println(F("Auth Failed"));
      Serial.println(F("Auth Failed"));
    } else if (error == OTA_BEGIN_ERROR) {
      display.println(F("Begin Failed"));
      Serial.println(F("Auth Failed"));
    } else if (error == OTA_CONNECT_ERROR) {
      display.println(F("Connect Failed"));
      Serial.println(F("Connect Failed"));
    } else if (error == OTA_RECEIVE_ERROR) {
      display.println(F("Receive Failed"));
      Serial.println(F("Receive Failed"));
    } else if (error == OTA_END_ERROR) {
      display.println(F("End Failed"));
      Serial.println(F("End Failed"));
    }
     display.display();
  });
  ArduinoOTA.begin();
}

void executeOTA(){
   clearDisplay();
   display.println("OTA ENABLED");
   display.display();
  unsigned long timePassed = millis();
  while(millis() < timePassed + 110000l){
      ArduinoOTA.handle();
      delay(200);
  }
}

