from machine import Pin
import time

print("Conditions are a GO!")

while True : 
    red = Pin(4, Pin.OUT , value  = 1)                   # RED output indicator 
    time.sleep(1)
    red.value(0)
    time.sleep(1)