#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <wiringPi.h>
#include <atomic>
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <thread>
#include "file_handling.h"
#include "i2c_bus.h"
#include "lis3mdl.h"
#include "lps.h"
#include "lsm6.h"
#include "Queue.h"

volatile std::atomic<bool> on_off_n;

class sensor_data {
public:
  int64_t timestamp;
  int32_t a_x;
  int32_t a_y;
  int32_t a_z;
  int32_t g_x;
  int32_t g_y;
  int32_t g_z;
  int32_t m_x;
  int32_t m_y;
  int32_t m_z;
  int32_t p;
  int16_t t;         
  bool a_valid;
  bool g_valid;
  bool mx_valid;
  bool my_valid;
  bool mz_valid;
  bool p_valid;
  bool t_valid; 
};

using namespace std::chrono;
using time_stamp = time_point<system_clock, nanoseconds>;

void write_data(Queue<sensor_data>& q) {
  // Create directory
  char date[25];
  std::time_t t=std::time(NULL);
  std::strftime(date, sizeof(date), "%Y%m%d_%H%M%S", std::localtime(&t));
  std::string directory(date);
  // mkdir
  const int dir_err = mkdir(directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (dir_err) {
      printf("Error creating directory!\n");
      return;
  }

  // Create header file
  const std::string header_filename = directory + "/" + "_header";
  FILE *output_handle = open_filename(header_filename);
  {
    fprintf(output_handle, "# Timestamp since EPOCH in nanoseconds\n");
    fprintf(output_handle, "# Accelerometer: ODR = 416Hz, FS = +-8g, a_x(m/s^2) = "
            "a_x*9.8*(0.244*10**-3)\n");
    fprintf(output_handle, "# Gyroscope: ODR = 416Hz, FS = +-2000 dps, g_x(dps) = "
            "g_x*(70*10**-3)\n");
    fprintf(output_handle, "# Magnetometer: ODR = 155Hz, FS = +-4 gauss, m_x(gauss) = "
            "m_x*(146*10**-6)\n");
    fprintf(output_handle, "# Barometer: ODR = 25Hz, p(hPa) = p/4096\n");
    fprintf(output_handle, "# Thermometer: ODR = 25Hz, t(C) = 42.5 + (t/480)\n");
    fprintf(output_handle,
            "# Altimeter: h(m) = ((1013.25/p(hPa))**(1/5.257)-1)*(t(C)+273.15)/0.0065\n");

    fprintf(output_handle, "timestamp, a_x, a_y, a_z, g_x, g_y, g_z, m_x, m_y, m_z, p, t, "
            "a_valid, g_valid, mx_valid, my_valid, mz_valid, p_valid, t_valid\n");
  }
  fclose(output_handle);

  for (int i = 0; on_off_n ; i++) {
    std::stringstream ssf;
    ssf << directory << "/data_" << i;
    FILE *output_handle = open_filename(ssf.str().c_str());
    // 7000 samples gives about 10s of data per file
    for (int j = 0; j < 7000; j++) {
      sensor_data d;
      try {
        q.pop(d);
      } catch (const std::runtime_error& e) {
        if (on_off_n) {
          std::cerr << "Timeout has been reached on queue pop" << std::endl;
        }
        break;
      }
      // Print sensor data loop
      fprintf(output_handle, "%" PRId64 ", %" PRId32 ", %" PRId32 ", %" PRId32 ", %" PRId32
              ", %" PRId32 ", %" PRId32 ", %" PRId32 ", %" PRId32 ", %" PRId32 ", %" PRId32
              ", %" PRId16 ", %c, %c, %c, %c, %c, %c, %c\n",
              d.timestamp, d.a_x, d.a_y, d.a_z, d.g_x, d.g_y, d.g_z, d.m_x, d.m_y, d.m_z, d.p, d.t,
              (d.a_valid ? '1' : '0'), (d.g_valid ? '1' : '0'), (d.mx_valid ? '1' : '0'),
              (d.my_valid ? '1' : '0'), (d.mz_valid ? '1' : '0'), (d.p_valid ? '1' : '0'),
              (d.t_valid ? '1' : '0'));
    }
    fclose(output_handle);
  }
}

void read_sensors(Queue<sensor_data>& q) {
  // In RPi Zero W, i2c-1 should be read
  const std::string bus_name = "/dev/i2c-1";
  i2c_bus::open(bus_name);
  
  // Enable & config lsm6
  lsm6::comm_config lsm6_config;
  lsm6_config.use_sensor = true;
  lsm6_config.device = lsm6::LSM6DS33;
  lsm6_config.i2c_address = lsm6::SA0_HIGH_ADDR;
  lsm6::handle lsm6;
  
  // Enable and config lis3mdl
  lis3mdl::comm_config lis3mdl_config;
  lis3mdl_config.use_sensor = true;
  lis3mdl_config.device = lis3mdl::LIS3MDL;
  lis3mdl_config.i2c_address = lis3mdl::SA1_LOW_ADDR;
  lis3mdl::handle lis3mdl;

  // Enable and config lps
  lps::comm_config lps_config;
  lps_config.use_sensor = true;
  lps_config.device = lps::LPS25H;
  lps_config.i2c_address = lps::SA0_HIGH_ADDR;
  lps::handle lps;

  // Enable all sensors
  lsm6.enable(lsm6_config);
  lis3mdl.enable(lis3mdl_config);
  lps.enable(lps_config);
  
  // Collect data loop
  while (on_off_n) {
    // Request i2c transactions to i2c_bus class
    lsm6.request_read();
    lis3mdl.request_read();
    lps.request_read();
    // Send i2c transactions to OS
    i2c_bus::process_burst_read();
    // Read sensor data
    lsm6.read();
    lis3mdl.read();
    lps.read();
    // Timestamp since epoch
    auto ts = time_point_cast<nanoseconds>(system_clock::now()).time_since_epoch();
    // Send data to queue
    sensor_data d;
    d.timestamp = ts.count();
    d.a_x = lsm6.a[0];   
    d.a_y = lsm6.a[1];    
    d.a_z = lsm6.a[2];   
    d.g_x = lsm6.g[0];     
    d.g_y = lsm6.g[1];     
    d.g_z = lsm6.g[2];      
    d.m_x = lis3mdl.m[0];      
    d.m_y = lis3mdl.m[1];    
    d.m_z = lis3mdl.m[2];    
    d.p = lps.p;
    d.t = lps.t;         
    d.a_valid = lsm6.a_valid; 
    d.g_valid = lsm6.g_valid;
    d.mx_valid = lis3mdl.mx_valid;
    d.my_valid = lis3mdl.my_valid;
    d.mz_valid = lis3mdl.mz_valid;
    d.p_valid = lps.p_valid;
    d.t_valid = lps.t_valid;
    q.push(d);
  }
}

void check_button() {
  const int gpio_button = 3;
  // WiringPi GPIO numerotation
  wiringPiSetup();
  // Pin 3 as input and with pull-down
  pinMode(gpio_button, INPUT);
  pullUpDnControl(gpio_button, PUD_DOWN);

  // Open log file
  FILE *output_handle = open_filename("./get_sensor_data.log");

  while(1) {
    // Positive edge
    {
      bool pressed_button = false;
      int samples[10] = {0};
      int sum = 0;
      while (!pressed_button) {
        for (int i = 0; i < 10; i++) {
          sum -= samples[i];
          samples[i] = digitalRead(gpio_button);
          sum += samples[i];
          if (sum >= 5) {
            pressed_button = true;
            break;
          }
          delay(10);
        }
      }
    }
    // Negative edge
    {
      bool released_button = false;
      int samples[10] = {0};
      int sum = 0;
      while (!released_button) {
        for (int i = 0; i < 10; i++) {
          sum -= samples[i];
          samples[i] = (digitalRead(gpio_button) ? 0 : 1);
          sum += samples[i];
          if (sum == 10) {
            released_button = true;
            break;
          }
          delay(10);
        }
      }
    }
    on_off_n = !on_off_n;
    if (on_off_n) {
      // Led on
      system("echo 0 > /sys/class/leds/led0/brightness");
    } else {
      // Led off
      system("echo 1 > /sys/class/leds/led0/brightness");
    }
    fprintf(output_handle, "Toggling to: %s\n", (on_off_n ? "on" : "off"));
    fflush(output_handle);
  }
  fclose(output_handle);
}

int main() {
  // Producer sends data, consumer writes it to file
  Queue<sensor_data> q;
  // Sensor capture is off
  on_off_n = false;
  system("echo 1 > /sys/class/leds/led0/brightness");
  // Button management thread
  std::thread t_check_button(check_button);

  while(1) {
    while (!on_off_n) {
      delay(1000);
    }
    
    // Start read and write threads
    std::thread t_read_sensors(std::bind(read_sensors, std::ref(q)));
    std::thread t_write_data(std::bind(write_data, std::ref(q)));

    t_read_sensors.join();
    t_write_data.join();
  }

  t_check_button.join();
  return 0;
}
    
