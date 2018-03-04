#include "lis3mdl.h"
#include <stdexcept>

void lis3mdl::handle::write_reg(reg_addr addr, uint8_t value)
{
  i2c_bus::write_two_bytes(config.i2c_address, addr, value);
}

void lis3mdl::handle::enable(const comm_config & config)
{
  if (config.device == LIS3MDL)
  {
    this->config = config;
    // OM = 11 (ultra-high-performance mode for X and Y); 155Hz ODR
    // Temperature sensor on
    write_reg(CTRL_REG1, 0b01100010);

    // FS = 00 (+/- 4 gauss full scale)
    write_reg(CTRL_REG2, 0b00000000);

    // MD = 00 (continuous-conversion mode)
    write_reg(CTRL_REG3, 0b00000000);

    // OMZ = 11 (ultra-high-performance mode for Z)
    write_reg(CTRL_REG4, 0b00001100);
  }
  else
  {
    std::runtime_error("Cannot enable unknown LIS3MDL device.");
  }
}

void lis3mdl::handle::request_read()
{
  burst_addr = 0x80 | STATUS_REG;
  i2c_bus::add_burst_read(config.i2c_address, &burst_addr, block, sizeof(block)); 
}

void lis3mdl::handle::read()
{
  // Status
  const uint8_t status = block[0];
  mx_valid = (status & 0b1) == 0b1;
  my_valid = (status & 0b10) == 0b10;
  mz_valid = (status & 0b100) == 0b100;
  // Magnetometer data
  m[0] = (int16_t)(block[1] | block[2] << 8);
  m[1] = (int16_t)(block[3] | block[4] << 8);
  m[2] = (int16_t)(block[5] | block[6] << 8);
}
