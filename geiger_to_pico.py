from machine import UART                     # imports machine library
from time import sleep                       # imports sleep library
uart = UART(0, 9600)                         # init with given baudrate



while True:                                  # start while loop
    prin(uart.readline())                   # prints the csv 
    sleep(1)                                 # puts the geiger to sleep 
