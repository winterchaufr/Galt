/*
 * imu.hpp
 *
 *  Copyright (c) 2014 Kumar Robotics. All rights reserved.
 *
 *  This file is part of galt.
 *
 *  Created on: 2/6/2014
 *		  Author: gareth
 */

#ifndef IMU_H_
#define IMU_H_

#include <memory>
#include <string>
#include <boost/asio.hpp>

extern "C" {
  #include <stdint.h>
}

namespace imu_3dm_gx4
{

/**
 *  @brief Class for communicating with the Microstrain 3DM-GX4-25
 */
class Imu
{
public:

  struct Packet
  {
    union {
      struct {
        uint8_t syncMSB;
        uint8_t syncLSB;
      };
      uint16_t sync;       /**< Header of packet */
    };

    uint8_t descriptor;    /**< Type of packet */
    uint8_t length;        /**< Length of the packet in bytes */

    uint8_t payload[255];  /**< Payload of packet */

    union {
      struct {
        uint8_t checkMSB;
        uint8_t checkLSB;
      };
      uint16_t checksum;  /**< Packet checksum */
    };

    /**
     *
     */
    void calcChecksum();

    /**
     *
     */
    void copyToBuffer(uint8_t *insert);
  };

  /**
   *  @brief Ctor
   */
  Imu(ros::NodeHandle comm, ros::NodeHandle params, const boost::asio::io_service& io_service);

  /**
   *  @brief Open serial port and configure IMU
   */
  void initialize();

  /**
   *  @brief Run the read loop and publish packets as they arrive
   */
  void run();

  void ping();

private:

  void setIdle();

  void setImuFormat();

  void setFilterFormat();

  void saveMessageFormat();

  void enableDataStream();

  void resume();

  // bool poll();

  bool write(const Packet& packet);

  size_t read(void * buffer, size_t length);

  ros::NodeHandle comm_;
  ros::NodeHandle params_;

  std::shared_ptr<boost::asio::serial_port> port_;
};

}  //  imu_3dm_gx4

#endif // IMU_H_
