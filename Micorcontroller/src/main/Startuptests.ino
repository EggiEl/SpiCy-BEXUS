extern volatile uint8_t SD_init;
extern volatile uint8_t TCP_init;
// |default 1 |sd init check | sd struct write check| sd struct read check |      cableTest     | TCP setup Check | HeaterTestAuto  |
//      31        30            29                      28                      27                   26                 25-19
uint32_t startuptest(){
  uint32_t startupcheck = 0;
  startupcheck|= ((uint32_t)0b00000001 << 31);
  //sd
  init_SD();
  startupcheck|= ((uint32_t)SD_init << 30);
  char path[]="startuptest";
  sd_numpackets(path);
  if(SD.exists(path)){SD.remove(path);}

  //TCP connection
  startupcheck|= ((uint32_t)cabletest() << 27);
  setup_TCP_Client();
  startupcheck|= ((uint32_t)TCP_init << 26);

  //Heater
  uint8_t buf = heaterTest_auto();
  for(uint8_t i = 0;i<6;i--){
    uint8_t heater_sucess = (buf >> i) & 0x01;
    startupcheck|= ((uint32_t)heater_sucess << (31-(i+19)));
  }
  
  return startupcheck;
}
