#pragma once

#include "i2c_bus.h"
#include <cstdint>

namespace lps
{
  enum device_type
  {
    LPS25H = 0xBD,
  };
    
  enum i2c_addr
  {
    SA0_LOW_ADDR = 0x5C,
    SA0_HIGH_ADDR = 0x5D,
  };

  enum reg_addr
  {
      REF_P_XL                = 0x08,
      REF_P_L                 = 0x09,
      REF_P_H                 = 0x0A,
                              
      WHO_AM_I                = 0x0F,
                              
      RES_CONF                = 0x10,
                              
      CTRL_REG1               = 0x20,
      CTRL_REG2               = 0x21,
      CTRL_REG3               = 0x22,
      CTRL_REG4               = 0x23, // 25H
              
      STATUS_REG              = 0x27,
                            
      PRESS_OUT_XL            = 0x28,
      PRESS_OUT_L             = 0x29,
      PRESS_OUT_H             = 0x2A,

      TEMP_OUT_L              = 0x2B,
      TEMP_OUT_H              = 0x2C,
      
      FIFO_CTRL               = 0x2E, // 25H
      FIFO_STATUS             = 0x2F, // 25H
      
      RPDS_L                  = 0x39, // 25H
      RPDS_H                  = 0x3A, // 25H
      
      // dummy addresses for registers in different locations on different devices;
      // the library translates these based on device type
      // value with sign flipped is used as index into translated_regs array
    
      INTERRUPT_CFG    = -1,
      INT_SOURCE       = -2,
      THS_P_L          = -3,
      THS_P_H          = -4,
      // update dummy_reg_count if registers are added here!
      
      
      // device-specific register addresses      
      LPS25H_INTERRUPT_CFG    = 0x24,
      LPS25H_INT_SOURCE       = 0x25,
      LPS25H_THS_P_L          = 0x30,
      LPS25H_THS_P_H          = 0x31,
    };  

  struct comm_config {
    bool use_sensor = false;
    device_type device;
    i2c_addr i2c_address;
  };

  class handle
  {
  public:
    void enable(const comm_config &);

    void write_reg(reg_addr addr, uint8_t value);

    void request_read();
    void read();

    int32_t p;
    bool p_valid;
    int16_t t;
    bool t_valid;
    
  protected:
    comm_config config;
    uint8_t burst_addr;
    uint8_t block[6];
  };
};
