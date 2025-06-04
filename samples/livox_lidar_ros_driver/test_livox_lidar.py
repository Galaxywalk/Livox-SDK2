import ctypes, numpy as np, time, sys, signal, _livox_core as lv

class Pt(ctypes.Structure):
    _pack_ = 1                     # no implicit padding
    _fields_ = [("x",  ctypes.c_int32),   # mm, little-endian
                ("y",  ctypes.c_int32),
                ("z",  ctypes.c_int32),
                ("ref",ctypes.c_uint8),
                ("tag",ctypes.c_uint8)]  # 2-byte reserved

def cb(handle, dev_type, ptr, n, dtype):
    if dtype not in (0, 1, 4, 5) or n == 0:
        return

    buf = (Pt * n).from_address(ptr)

    print(f"\nframe from {handle:#x}  ({n} pts)")
    for i in range(n):
        p = buf[i]
        print(f"{i:02d}: ({p.x/1000:.3f}, {p.y/1000:.3f}, {p.z/1000:.3f}) m  "
              f"ref={p.ref} tag={p.tag}")

drv = lv.LivoxDriver()
drv.register_pointcloud_callback(cb)
cfg = sys.argv[1] if len(sys.argv)==2 else sys.exit("cfg.json?")
assert drv.init(cfg), "SDK init failed"

signal.signal(signal.SIGINT, lambda s,f:(drv.shutdown(), sys.exit(0)))
print("Streaming – Ctrl-C to quit");  [time.sleep(1) for _ in iter(int,1)]
