#include "lsm6.h"
#include <stdexcept>

void lsm6::handle::write_reg(reg_addr addr, uint8_t value)
{
  i2c_bus::write_two_bytes(config.i2c_address, addr, value);
}

void lsm6::handle::enable(const comm_config & config)
{
  if (config.device == LSM6DS33)
  {
    //// LSM6DS33 gyro
    this->config = config;
    
    // [7:4] ODR = 0110 (416 Hz (high performance))
    // [3:2] FS_G = 11 (2000 dps)
    write_reg(CTRL2_G, 0b01101100);

    // defaults
    write_reg(CTRL7_G, 0b00000000);

    //// LSM6DS33 accelerometer

    // [7:4] ODR = 0110 (416 Hz (high performance))
    // [3:2] FS_XL = 11 (8 g full scale)
    // BW_XL = 00 (200 Hz since XL_BW_SCAL_ODR == 0)
    write_reg(CTRL1_XL, 0b01101100);

    //// common

    // IF_INC = 1 (automatically increment address register)
    write_reg(CTRL3_C, 0b00000100);
  }
  else
  {
    throw std::runtime_error("Cannot enable unknown LSM6 device.");
  }
}

void lsm6::handle::request_read() {
  burst_addr = STATUS_REG;
  i2c_bus::add_burst_read(config.i2c_address, &burst_addr, block, sizeof(block));
}

void lsm6::handle::read() {
  // Status
  const uint8_t status = block[0];
  a_valid = (status & 0b1) == 0b1;
  g_valid = (status & 0b10) == 0b10;

  // Gyro
  g[0] = (int16_t)(block[4] | block[5] << 8);
  g[1] = (int16_t)(block[6] | block[7] << 8);
  g[2] = (int16_t)(block[8] | block[9] << 8);
    
  // Accelerometer
  a[0] = (int16_t)(block[10] | block[11] << 8);
  a[1] = (int16_t)(block[12] | block[13] << 8);
  a[2] = (int16_t)(block[14] | block[15] << 8);
}
