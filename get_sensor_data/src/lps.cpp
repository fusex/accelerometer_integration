#include "lps.h"
#include <stdexcept>

void lps::handle::write_reg(reg_addr addr, uint8_t value)
{
  i2c_bus::write_two_bytes(config.i2c_address, addr, value);
}

void lps::handle::enable(const comm_config & config)
{
  if (config.device == LPS25H)
  {
    this->config = config;
    // [7:7] PD = 1 (active mode)
    // [6:4] ODR = 100 (25 Hz pressure & temperature output data rate)
    write_reg(CTRL_REG1, 0b11000000);
  }
  else
  {
    std::runtime_error("Cannot enable unknown LPS device.");
  }
}

void lps::handle::request_read() {
  // assert MSB to enable register address auto-increment
  burst_addr = 0x80 | STATUS_REG;
  i2c_bus::add_burst_read(config.i2c_address, &burst_addr, block, sizeof(block));
}

void lps::handle::read()
{
  // Status
  const uint8_t status = block[0];
  t_valid = (status & 0b1) == 0b1;
  p_valid = (status & 0b10) == 0b10;
  // Pressure
  p = (int32_t)(int8_t)block[3] << 16 | (uint16_t)block[2] << 8 | block[1];
  // Temperature
  t = (int16_t)(block[5] << 8 | block[4]);
}
