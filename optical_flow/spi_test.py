import spidev
spi = spidev.SpiDev()
spi.open(0, 0)
spi.max_speed_hz = 400000
print(spi.xfer2([0xAA, 0x55, 0x01]))   # should print [170, 85, 1]