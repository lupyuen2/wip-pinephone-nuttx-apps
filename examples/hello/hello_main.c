/****************************************************************************
 * apps/examples/hello/hello_main.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <arpa/inet.h>  // For ntohs()

// MPU-6050 Accelerometer Data Format
// (14 bytes, big-endian)
struct sensor_data_s
{
  int16_t x_accel;  // Accelerometer X
  int16_t y_accel;  // Accelerometer Y
  int16_t z_accel;  // Accelerometer Z
  int16_t temp;     // Temperature
  int16_t x_gyro;   // Gyroscope X
  int16_t y_gyro;   // Gyroscope Y
  int16_t z_gyro;   // Gyroscope Z
};

// Read the MPU-6050 Accelerometer Data from /dev/imu0 and interpret the data
int main(int argc, FAR char *argv[])
{
  printf("Hello, World!!\n");

  // Open the MPU-6050 Accelerometer at /dev/imu0 for reading
  int fd = open("/dev/imu0", O_RDONLY);
  assert(fd > 0);  // Check that /dev/imu0 exists

  // Accelerometer Data will have 14 bytes
  struct sensor_data_s data;
  assert(sizeof(data) == 14);  // We expect to read 14 bytes

  // Read the Accelerometer Data (14 bytes)
  int bytes_read = read(fd, &data, sizeof(data));
  assert(bytes_read == sizeof(data));  // We expect 14 bytes read

  // Flip the bytes from Big-Endian to Little-Endian for PinePhone.
  // ntohs() is explained here: https://developer.ibm.com/articles/au-endianc/
  int16_t x_accel = ntohs(data.x_accel);
  int16_t y_accel = ntohs(data.y_accel);
  int16_t z_accel = ntohs(data.z_accel);
  int16_t temp    = ntohs(data.temp);
  int16_t x_gyro  = ntohs(data.x_gyro);
  int16_t y_gyro  = ntohs(data.y_gyro);
  int16_t z_gyro  = ntohs(data.z_gyro);

  // Print the Accelerometer Data
  printf("Accelerometer X: %d\n", x_accel);
  printf("Accelerometer Y: %d\n", y_accel);
  printf("Accelerometer Z: %d\n", z_accel);
  printf("Temperature:     %d\n", temp);
  printf("Gyroscope X:     %d\n", x_gyro);
  printf("Gyroscope Y:     %d\n", y_gyro);
  printf("Gyroscope Z:     %d\n", z_gyro);

  // Close the Accelerometer
  close(fd);
  return 0;
}

/* Output Log

Portrait Orientation:
nsh> hello
Hello, World!!
Accelerometer X: 4137
Accelerometer Y: -97
Accelerometer Z: 208
Temperature:     -1500
Gyroscope X:     -29
Gyroscope Y:     66
Gyroscope Z:     12

Landscape Orientation:
nsh> hello
Hello, World!!
Accelerometer X: -143
Accelerometer Y: -4007
Accelerometer Z: -443
Temperature:     -1418
Gyroscope X:     -5
Gyroscope Y:     118
Gyroscope Z:     -56

*/
