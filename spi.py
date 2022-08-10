#Author : Harindi Perera 
#date : 8/08 



from machine import Pin
from machine import SPI 
import time

print("Conditions are a GO!")



#Declare pins for slave 1
mosi = Pin(22 , mode=Pin.OUT)                           #Master out slave in an output
print("PIN 22 declared output")


miso = Pin(21 , mode=Pin.IN)                            #Master out slave in an output
print("PIN 21 declared input")

cs = Pin(2 , mode=Pin.OUT , value = 1)                  #io 2 is pulled high internally
print("chip select at io 2 -  set high")

#clock out is connected to slave device through pin 20

# Initialise the SPI bus
#hspi = SPI(1, 10000000)

#Should it be pin 6 or 18

hspi = SPI(1, 10000000, sck=Pin(18), mosi=Pin(22), miso=Pin(21))


#Set to zero's 

#case1
try:
    cs(0)                                               # Select peripheral.
    hspi.write(b"12345678")                             # Write 8 bytes, and don't care about received data.
finally:
    cs(1)                                               # Deselect peripheral.

#case2
try:
    cs(0)                                               # Select peripheral.
    rxdata = hspi.read(8, 0x42)                         # Read 8 bytes while writing 0x42 for each byte.
finally:
    cs(1)                                               # Deselect peripheral.

#case3
rxdata = bytearray(8)
try:
    cs(0)                                               # Select peripheral.
    hspi.readinto(rxdata, 0x42)                         # Read 8 bytes inplace while writing 0x42 for each byte.
finally:
    cs(1)                                               # Deselect peripheral.

#case4
txdata = b"12345678"
rxdata = bytearray(len(txdata))
try:
    cs(0)                                               # Select peripheral.
    hspi.write_readinto(txdata, rxdata)                 # Simultaneously write and read bytes.
finally:
    cs(1)                                               # Deselect peripheral.
print("Successful spi comms")



#############################################################################################


# Can Configuration

# Reset the MCP2557 

# Configure the oscillator and clk pins

#configure the i/o pins

# configure the bit time registers
# configure the tef , txq , tx and rx fifo

#############################################################################################
