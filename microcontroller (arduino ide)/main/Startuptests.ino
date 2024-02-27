extern volatile uint8_t SD_init;

// |default 1 |sd init check | sd struct write check| sd struct read check |


uint32_t startuptest(){
  uint32_t startupcheck = 0;
  startupcheck|= ((uint32_t)0b00000001 << 31);
  //sd
  init_SD();
  startupcheck|= ((uint32_t)SD_init << 30);
  char path[]="startuptest";
  sd_numpackets(path);
  if(SD.exists(path)){SD.remove(path);}

  
  return startupcheck;
}