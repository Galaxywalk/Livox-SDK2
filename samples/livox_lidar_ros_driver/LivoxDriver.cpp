#include "LivoxDriver.h"

#include <arpa/inet.h>   // inet_ntoa
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

/* ========== Public API ============================================ */

bool LivoxDriver::init(const std::string& cfg) {
  if (running_) return true;           // already started

  if (!LivoxLidarSdkInit(cfg.c_str())) {
    std::cerr << "[LivoxDriver] SDK init failed\n";
    return false;
  }

  /* Attach SDK callbacks – pass `this` so we can hop back into the instance */
  SetLivoxLidarPointCloudCallBack(sPointcloudCb, this);
  SetLivoxLidarImuDataCallback    (sImuCb,       this);
  SetLivoxLidarInfoChangeCallback (sInfoChangeCb, this);
  SetLivoxLidarInfoCallback       (sPushMsgCb,    this);

  running_ = true;
  return true;
}

void LivoxDriver::shutdown() {
  if (!running_) return;
  LivoxLidarSdkUninit();
  running_ = false;
}

/* ========== Static → member thunks ================================= */

void LivoxDriver::sPointcloudCb(uint32_t h, uint8_t t,
                                LivoxLidarEthernetPacket* p, void* self) {
  if (!self) return;
  auto* drv = static_cast<LivoxDriver*>(self);
  if (drv->pc_cb_) drv->pc_cb_(h, t, p);
}

void LivoxDriver::sImuCb(uint32_t h, uint8_t t,
                         LivoxLidarEthernetPacket* p, void* self) {
  if (!self) return;
  auto* drv = static_cast<LivoxDriver*>(self);
  if (drv->imu_cb_) drv->imu_cb_(h, t, p);
}

void LivoxDriver::sInfoChangeCb(uint32_t h,
                                const LivoxLidarInfo* info, void* self) {
  if (!info) return;
  std::cout << "[LivoxDriver] New LiDAR online: handle=" << h
            << " SN=" << info->sn << '\n';

  /* Put LiDAR into “NORMAL” working mode so it starts streaming. */
  SetLivoxLidarWorkMode(h, kLivoxLidarNormal, sWorkModeCb, nullptr);
}

void LivoxDriver::sPushMsgCb(uint32_t h, uint8_t t,
                             const char* info, void* /*self*/) {
  struct in_addr a; a.s_addr = h;
  std::cout << "[LivoxDriver] PushMsg from " << inet_ntoa(a)
            << " : " << info << '\n';
}

void LivoxDriver::sWorkModeCb(livox_status st, uint32_t h,
                              LivoxLidarAsyncControlResponse* r, void*) {
  std::cout << "[LivoxDriver] Work-mode set: status=" << st
            << " handle=" << h
            << " ret=" << (r ? int(r->ret_code) : -1)
            << " err=" << (r ? int(r->error_key) : -1)
            << '\n';
}
