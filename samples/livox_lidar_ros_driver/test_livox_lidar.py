import ctypes, numpy as np, time, sys, signal, _livox_core as lv

class PointCloud(ctypes.Structure):
    _pack_ = 1                     # no implicit padding
    _fields_ = [("x",  ctypes.c_int32),   # mm, little-endian
                ("y",  ctypes.c_int32),
                ("z",  ctypes.c_int32),
                ("ref",ctypes.c_uint8),
                ("tag",ctypes.c_uint8)]  # 2-byte reserved

class IMU(ctypes.Structure):
    _pack_ = 1
    _fields_ = [("gyro_x", ctypes.c_float),
                ("gyro_y", ctypes.c_float),
                ("gyro_z", ctypes.c_float),
                ("acc_x",  ctypes.c_float),
                ("acc_y",  ctypes.c_float),
                ("acc_z",  ctypes.c_float)]


def pointcloud_cb(handle, dev_type, ptr, n, dtype):
    if dtype != 1 or n == 0:
        return # dev_type = 1: CartesianCoordinateHighData

    buf = (PointCloud * n).from_address(ptr)

    print(f"\nframe from {handle:#x}  ({n} pts)")
    for i in range(n):
        p = buf[i]
        print(f"{i:02d}: ({p.x/1000:.3f}, {p.y/1000:.3f}, {p.z/1000:.3f}) m  "
              f"ref={p.ref} tag={p.tag}")

def imu_cb(handle, dev_type, ptr, n, dtype):
    if dtype != 0:
        return  # dtype==0: imu
    if not ptr or n == 0:
        return

    buf = (IMU * n).from_address(ptr)
    print(f"\nIMU frame from {handle:#x}  ({n} samples)")
    for i in range(n):
        imu = buf[i]
        print(f"{i:02d}: gyro=({imu.gyro_x:.3f}, {imu.gyro_y:.3f}, {imu.gyro_z:.3f}) "
              f"acc=({imu.acc_x:.3f}, {imu.acc_y:.3f}, {imu.acc_z:.3f})")


drv = lv.LivoxDriver()
drv.register_pointcloud_callback(pointcloud_cb)
drv.register_imu_callback(imu_cb)
cfg = sys.argv[1] if len(sys.argv)==2 else sys.exit("do not receive parameter of cfg.json?")
assert drv.init(cfg), "SDK init failed"

signal.signal(signal.SIGINT, lambda s,f:(drv.shutdown(), sys.exit(0)))
print("Streaming – Ctrl-C to quit");  [time.sleep(1) for _ in iter(int,1)]
