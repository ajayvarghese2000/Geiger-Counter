from machine import UART                     # imports machine library
from time import sleep                       # imports sleep library
uart = UART(0, 9600)                         # init with given baudrate



while True:                                  # start while loop
    buffer=uart.readline(16,17)                   # prints the csv 
    sleep(1)                                 # puts the geiger to sleep 
    num=str(buffer)
    num=int
