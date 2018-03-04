#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <inttypes.h>
#include <system_error>
#include <chrono>
#include "i2c_bus.h"
#include "lsm6.h"
#include "lps.h"
#include "lis3mdl.h"

using namespace std::chrono;
using time_stamp = time_point<system_clock, nanoseconds>;

FILE *open_filename(std::string filename)
{
   FILE *new_handle = NULL;
   char *filename_c = const_cast<char *>(filename.c_str());

   if (filename_c)
   {
      bool bNetwork = false;
      int sfd = -1, socktype;

      if(!strncmp("tcp://", filename_c, 6))
      {
         bNetwork = true;
         socktype = SOCK_STREAM;
      }
      else if(!strncmp("udp://", filename_c, 6))
      {
         bNetwork = true;
         socktype = SOCK_DGRAM;
      }

      if(bNetwork)
      {
         unsigned short port;
         filename_c += 6;
         char *colon;
         if(NULL == (colon = strchr(filename_c, ':')))
         {
            fprintf(stderr, "%s is not a valid IPv4:port, use something like tcp://1.2.3.4:1234 or udp://1.2.3.4:1234\n",
                    filename_c);
            exit(132);
         }
         if(1 != sscanf(colon + 1, "%hu", &port))
         {
            fprintf(stderr,
                    "Port parse failed. %s is not a valid network file name, use something like tcp://1.2.3.4:1234 or udp://1.2.3.4:1234\n",
                    filename_c);
            exit(133);
         }
         char chTmp = *colon;
         *colon = 0;

         struct sockaddr_in saddr;
         saddr.sin_family = AF_INET;
         saddr.sin_port = htons(port);
         if(0 == inet_aton(filename_c, &saddr.sin_addr))
         {
            fprintf(stderr, "inet_aton failed. %s is not a valid IPv4 address\n",
                    filename_c);
            exit(134);
         }
         *colon = chTmp;

         if(0 <= (sfd = socket(AF_INET, socktype | SOCK_NONBLOCK, 0)))
         {
           fprintf(stderr, "Connecting to %s:%hu...", inet_ntoa(saddr.sin_addr), port);

           int iTmp = 1;
           while ((-1 == (iTmp = connect(sfd, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in)))) && (EINTR == errno))
             ;
           if (iTmp < 0)
             fprintf(stderr, "error: %s\n", strerror(errno));
           else
             fprintf(stderr, "connected, sending video...\n");
         }
         else
           fprintf(stderr, "Error creating socket: %s\n", strerror(errno));

         if (sfd >= 0)
            new_handle = fdopen(sfd, "w");
      }
      else
      {
         new_handle = fopen(filename_c, "wb");
      }
   }

   return new_handle;
}

int main() {
  // Log file
  FILE *output_handle = open_filename("test.log");
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

  // Output file header
  {
    fprintf(output_handle, "# Timestamp since EPOCH in nanoseconds\n");
    fprintf(output_handle, "# Accelerometer: ODR = 416Hz, FS = +-8g, a_x(m/s^2) = a_x*9.8*(0.244*10**-3)\n");
    fprintf(output_handle, "# Gyroscope: ODR = 416Hz, FS = +-2000 dps, g_x(dps) = g_x*(70*10**-3)\n");
    fprintf(output_handle, "# Magnetometer: ODR = 155Hz, FS = +-4 gauss, m_x(gauss) = m_x*(146*10**-6)\n");
    fprintf(output_handle, "# Altimeter: ODR = 25Hz, Pout(hPa) = p/4096\n");
    fprintf(output_handle, "# Thermometer: ODR = 25Hz, T(C) = 42.5 + (t/480)\n");
    fprintf(output_handle, "timestamp, a_x, a_y, a_z, g_x, g_y, g_z, m_x, m_y, m_z, p, t, "
            "a_valid, g_valid, mx_valid, my_valid, mz_valid, p_valid, t_valid\n");
  }
  
  // Enable all sensors
  lsm6.enable(lsm6_config);
  lis3mdl.enable(lis3mdl_config);
  lps.enable(lps_config);
  // Collect data loop
  while(1) {
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

    // Print all sensor datax
    fprintf(output_handle, "%" PRId64 ", %" PRId32 ", %" PRId32 ", %" PRId32 ", %" PRId32
            ", %" PRId32 ", %" PRId32 ", %" PRId32 ", %" PRId32 ", %" PRId32 ", %" PRId32
            ", %" PRId16 ", %c, %c, %c, %c, %c, %c, %c\n",
            ts.count(), lsm6.a[0], lsm6.a[1], lsm6.a[2], lsm6.g[0], lsm6.g[1], lsm6.g[2],
            lis3mdl.m[0], lis3mdl.m[1], lis3mdl.m[2], lps.p, lps.t, (lsm6.a_valid ? '1' : '0'),
            (lsm6.g_valid ? '1' : '0'), (lis3mdl.mx_valid ? '1' : '0'),
            (lis3mdl.my_valid ? '1' : '0'), (lis3mdl.mz_valid ? '1' : '0'),
            (lps.p_valid ? '1' : '0'), (lps.t_valid ? '1' : '0'));
  }
  return 0;
}
    
