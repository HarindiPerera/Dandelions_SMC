from machine import Pin, SoftSPI


# construct a SoftSPI bus on the given pins
# polarity is the idle state of SCK
# phase=0 means sample on the first edge of SCK, phase=1 means the second

spi = SoftSPI(baudrate = 100000, polarity=1, phase=0, sck=Pin(14), mosi=Pin(13), miso=Pin(12))

   
#SPI bus 2
# WRITE Commmand to CAN FD controller
#write = 0b0010
#address = 0xE00
#write_buf = ((write << 12) + address).to_bytes(2, 'big')
#spi.write(write_buf)


# verify with read
read = 0b0011
address = 0xE00
read_buf = ((read << 12) + address).to_bytes(2, 'big')

#write and read into must be seperated 
spi.write(read_buf)
buf = bytearray(4)
spi.readinto(buf)

print("Expected: 0b00000000_00000000_00000000_00000000")
print("Buffer:", buf)
