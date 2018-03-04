#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

class i2c_bus
{
public:

  virtual void abstract_class() = 0;

  static void open(const std::string & name);
  static void close();

  static void add_burst_read(uint8_t address, uint8_t* command, uint8_t* data, size_t size);
  static void process_burst_read();

  static void write(uint8_t device_address, const uint8_t * data, size_t size);

  static void write_two_bytes(uint8_t address, uint8_t byte1, uint8_t byte2)
  {
    uint8_t buffer[] = { byte1, byte2 };
    write(address, buffer, 2);
  }

private:
  static int fd;
  static std::vector<i2c_msg> requests;
};
