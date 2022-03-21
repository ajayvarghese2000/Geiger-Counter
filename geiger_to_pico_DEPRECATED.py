from machine import UART                     # imports machine library
from time import sleep                       # imports sleep library
uart = UART(0, 9600)                         # init with given baudrate

def extract_uvs(a):                                    # define function to extract required data
    i = 0                                              # starts counter at 0 
    for char in a:                                     # for loop
        if char == str('r'):                           # if loop
            data = a[i+3] + a[i+4] + a[i+5] + a[i+6]   # increments characters required
            return data                                # end process
        i = i+1                                        # increment i

while True:                                  # start while loop

    geiger_data=uart.readline()              # put the serial data from the geiger into a variable
    output = extract_uvs(str(geiger_data))  # define output as varibale
    print (output)                           # prints output
    sleep(1)                                 # puts the geiger to sleep 1 second