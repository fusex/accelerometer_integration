#include "i2c_bus.h"
#include <cerrno>
#include <cstring>
#include <system_error>
#include <string>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

int i2c_bus::fd = 0;
std::vector<i2c_msg> i2c_bus::requests;

static inline std::system_error posix_error(const std::string & what)
{
    return std::system_error(errno, std::system_category(), what);
}

void i2c_bus::open(const std::string & name)
{
  close();
  fd = ::open(name.c_str(), O_RDWR);
  if (fd == -1)
  {
    throw posix_error(std::string("Failed to open I2C device ") + name);
  }
}

void i2c_bus::close()
{
  if (fd != -1)
  {
    ::close(fd);
    fd = -1;
  }
}

void i2c_bus::add_burst_read(uint8_t address, uint8_t *command, uint8_t *data, size_t size)
{
  i2c_msg init_addr = {address, 0, 1, (typeof(i2c_msg().buf)) command};
  i2c_msg burst = {address, I2C_M_RD, (typeof(i2c_msg().len)) size, (typeof(i2c_msg().buf)) data};
  requests.push_back(init_addr);
  requests.push_back(burst);
}

void i2c_bus::process_burst_read()
{
  // Build request
  size_t requests_size = requests.size();
  if (requests_size%2 != 0) {
    throw std::runtime_error("Requests vector is not a multiple of 2");
  }
  // Send messages by pair
  for (size_t i = 0; i < requests_size/2; i++) {
    i2c_msg messages[2] = {requests[2*i], requests[2*i+1]};
    i2c_rdwr_ioctl_data ioctl_data = {messages, 2};
    // Requests to OS
    int result = ioctl(fd, I2C_RDWR, &ioctl_data);
    if (result != 2) {
        throw posix_error("Failed to read from I2C");
    }
  }
  // Clear vector
  requests.clear();
}

void i2c_bus::write(uint8_t address, const uint8_t * data, size_t size)
{
  i2c_msg messages[1] = {
    { address, 0, (typeof(i2c_msg().len)) size, (typeof(i2c_msg().buf)) data }
  };
  i2c_rdwr_ioctl_data ioctl_data = { messages, 1 };

  int result = ioctl(fd, I2C_RDWR, &ioctl_data);

  if (result != 1)
  {
    throw posix_error("Failed to write to I2C");
  }
}

