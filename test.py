from machine import Pin, SoftSPI
import time

size = 50

spi = SoftSPI(baudrate = 100000, polarity=1, phase=0, sck=Pin(14), mosi=Pin(13), miso=Pin(12))
print("SPI bus created")

#set the baudrate
spi.init(baudrate=200000)
print("SPI bus initialised")

#TEST1
# read 10 bytes on MISO
spi.read(10)
print("read : 10 bytes")

# read 10 bytes while outputting 0xff on MOSI
spi.read(10, 0xff)      


#TEST2
# create a buffer
buf = bytearray(size)
print("buffer size", size)

# read into the given buffer (reads 50 bytes in this case)
spi.readinto(buf)

# read into the given buffer and output 0xff on MOSI
spi.readinto(buf, 0xff) 

#TEST3
# write 5 bytes on MOSI
spi.write(b'12345')     


#TEST4
# create a buffer
buf = bytearray(4)

# write to MOSI and read from MISO into the buffer
spi.write_readinto(b'1234', buf)

# write buf to MOSI and read MISO back into buf
spi.write_readinto(buf, buf) 
