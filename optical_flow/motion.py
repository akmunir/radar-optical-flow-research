#!/usr/bin/env python
import time

from pmw3901 import BG_CS_BACK_BCM, BG_CS_FRONT_BCM, PAA5100, PMW3901

print("""motion.py - Detect flow/motion in front of the PMW3901 sensor.

Press Ctrl+C to exit!
""")


# Pick the right class for the specified breakout
SensorClass = PMW3901
flo = SensorClass(spi_port=0, spi_cs=BG_CS_FRONT_BCM)
flo.set_rotation(0)

tx = 0
ty = 0

try:
    while True:
        x, y = flo.get_motion()
        tx += x
        ty += y
        print(f"Relative: x {x:03d} y {y:03d} | Absolute: x {tx:03d} y {ty:03d}")
        time.sleep(0.01)
except KeyboardInterrupt:
    pass