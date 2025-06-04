#pragma once
/*********************************************************************
 * LivoxDriver – thin C++ wrapper around the Livox SDK2 point-cloud /
 * IMU streaming interface.  Designed for ROS / pybind11 bridging.
 *********************************************************************/

#include <functional>
#include <string>
#include <vector>
#include "livox_lidar_def.h"   // SDK headers
#include "livox_lidar_api.h"

class LivoxDriver {
public:
  using PcCallback =
      std::function<void(uint32_t handle,
                         uint8_t  dev_type,
                         const LivoxLidarEthernetPacket* pkt)>;
  using ImuCallback =
      std::function<void(uint32_t handle,
                         uint8_t  dev_type,
                         const LivoxLidarEthernetPacket* pkt)>;

  LivoxDriver()  = default;
  ~LivoxDriver() { shutdown(); }

  /** Initialise SDK, attach internal callbacks, and start the LiDAR. */
  bool init(const std::string& json_cfg_path);

  /** Stop the SDK if running.  Safe to call more than once. */
  void shutdown();

  /** User hooks – call these *before* `init()` or any time afterwards. */
  void setPointcloudCallback(PcCallback cb) { pc_cb_  = std::move(cb); }
  void setImuCallback       (ImuCallback cb) { imu_cb_ = std::move(cb); }

  /** Convenience: true once `init()` succeeded and not yet shut down. */
  bool isRunning() const { return running_; }

private:
  /* SDK → static thunk → instance helpers --------------------------- */
  static void sPointcloudCb(uint32_t h, uint8_t t,
                            LivoxLidarEthernetPacket* p, void* self);
  static void sImuCb(uint32_t h, uint8_t t,
                     LivoxLidarEthernetPacket* p, void* self);
  static void sInfoChangeCb(uint32_t h,
                            const LivoxLidarInfo* info, void* self);
  static void sPushMsgCb(uint32_t h, uint8_t t,
                         const char* info, void* self);
  static void sWorkModeCb(livox_status st, uint32_t h,
                          LivoxLidarAsyncControlResponse* r, void*);

  /* User-supplied callbacks */
  PcCallback  pc_cb_;
  ImuCallback imu_cb_;

  bool running_{false};
};
