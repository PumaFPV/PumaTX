 const char* ssid = "HP-Nico";
 const char* password = "nico1809";



void OTASetup(){
  
  WiFi.mode(WIFI_STA);
  //tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, "PumaTX");
  WiFi.begin(ssid, password);
  /*
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    ESP.restart();
  }
  */
  WiFi.setHostname("PumaTX");
  
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
    })
    .onEnd([]() {
    })
    .onProgress([](unsigned int progress, unsigned int total) {})
    .onError([](ota_error_t error) {
      if (error == OTA_AUTH_ERROR);
      else if (error == OTA_BEGIN_ERROR);
      else if (error == OTA_CONNECT_ERROR);
      else if (error == OTA_RECEIVE_ERROR);
      else if (error == OTA_END_ERROR);
    });

  ArduinoOTA.begin();
  
}
