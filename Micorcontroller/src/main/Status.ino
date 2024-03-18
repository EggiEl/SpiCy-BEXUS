extern volatile uint8_t SD_init;
extern volatile uint8_t TCP_init;

/**
 * returns a status value wich can be decoded to get infos about the parts of the module
 *  @return uint32_t status bitwise encoded as:
 *  |default 1 |sd init check | sd struct write check| sd struct read check |      cableTest     | TCP setup Check | HeaterTestAuto  |
 *      31        30            29                      28                      27                   26                 25-19
 */
uint32_t get_Status()
{
  uint32_t status = 0;
  status |= ((uint32_t)0b00000001 << 31);
  // sd
  init_SD();
  status |= ((uint32_t)SD_init << 30);
  char path[] = "startuptest";
  sd_numpackets(path);
  if (SD.exists(path))
  {
    SD.remove(path);
  }

  // TCP connection
  status |= ((uint32_t)cabletest() << 27);
  setup_TCP_Client();
  status |= ((uint32_t)TCP_init << 26);

  // Heater
  uint8_t buf = heat_testauto();
  for (uint8_t i = 0; i < 6; i--)
  {
    uint8_t heater_sucess = (buf >> i) & 0x01;
    status |= ((uint32_t)heater_sucess << (31 - (i + 19)));
  }

  //memory
  return status;
}
