/**
 * @file main.c
 * @author Ajay Varghese (Team C)
 * @date 21 March 2022
 * @brief The program aims to set-up i2c slave communication on the Raspberry
 * Pi Pico so that it can interface with I2C master controllers to send data
 * when requested of it. 
 * 
 * There are two event loops set up using interrupts. One loop for the UART
 * data and one for I2C communication. The I2C interrupt has the higher priority.
 * 
 * When new data is received from teh geiger counter it is read into a buffer
 * and then send off to be processed to the data of intrest can be extraced.
 * Once extracted it is written to the position 1 of the data array.
 * 
 * At any point the I2C master acn request data from the data array. This is
 * handled using another interrupt, where by depending on which register it wants
 * access to, the correct data is extracted from the 16 bit register and sent.
 * 
 * @note [WARNING] I2C communication procoal sends data in 8 bit blocks
 * this device has registars 16 bits wide therfore the data MUST be sent
 * and read using 2 blocks. If this is not adhered to you will LOCK UP
 * the I2C bus and must preform a hard reboot
 * 
 * This is part of a collection of programs and codes for the 21WSD001 Team
 * Project run by Loughborough  University
*/


/* *************** [INCLUDES] *************** */

// Standard C library's to use standard C functions and types
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

// Standard Pi Pico library to initialise Pico's Peripherals 
#include "pico/stdlib.h"

// library to implement I2C communications from Pico SDK
#include "hardware/i2c.h"

// library to handle interrupts from I2C reads and writes
#include "hardware/irq.h"

// library to communicate over UART from Pico to Geiger
#include "hardware/uart.h"



/* *************** [CONFIGURATION VARIABLES] *************** */ 



// Uncomment this line to enable debugging statements onto USB UART - MUST ENABLE IN MAKEFILE
#define DEBUGGING


/* *************** [I2C CONFIGURATION VARIABLES START] *************** */



// The Address of the I2C Peripheral
#define I2C_ADDR 0x4D   // [WARNING] can only take values between 0x08 - 0x77

// The Specific GP Pins on the Pi Pico to enable I2C on
#define SDA_SLAVE 4
#define SCL_SLAVE 5

// Data Array to hold the information from the registers
// 0 -> The i2c address of the device itself
// 1 -> The data
uint16_t data[2] = {I2C_ADDR, 0};

// Register Variable to Read/Write data too
uint8_t register_accessed;



/* *************** [I2C CONFIGURATION VARIABLES END] *************** */




/* *************** [UART CONFIGURATION VARIABLES START] *************** */



#define UART_RX 17
#define UART_TX 16
#define UART_ID uart0

// Buffers for UART Data to be stored in
volatile char temp[50];
volatile int counter = 0;
volatile char ch[1];



/* *************** [UART CONFIGURATION VARIABLES END] *************** */



/* *************** [EXECTUTION CODE] *************** */ 

/**
 * @brief This interrupt is called whenever the i2c0 bus is accessed by the
 * master. It handles what to do on reads and writes as well as resetting
 * interrupt registers
 * 
 */
void i2c0_irq_handler() {

    // Get interrupt status
    uint32_t status = i2c0->hw->intr_stat;

    // Check to see if we have received data from the I2C master (Will always be called on R/W)
    if (status & I2C_IC_INTR_STAT_R_RX_FULL_BITS) {

        // Parsing the register the master is trying to access
        register_accessed = ((i2c0->hw->data_cmd) << 24) >> 24;

        #ifdef DEBUGGING
            printf("Write acessed %02x\n", register_accessed);
        #endif

        /* 
            As this device will not need to update any internal registars no writes
            will be done. If we did want to write to an internal register to e.g
            setup configurations for this i2c device we would write that code
            below here.
        */
        
    }

    // Check to see if the I2C master is requesting data from us
    if (status & I2C_IC_INTR_STAT_R_RD_REQ_BITS) {

        #ifdef DEBUGGING
            printf("Read Accessed On Register %02x\n", register_accessed);
        #endif

        uint16_t invalid_data = 0;

        switch (register_accessed)
        {   
            // Register One contains the address of the device
            case 0:
                i2c_write_raw_blocking(i2c0, &data[register_accessed], 2);

                #ifdef DEBUGGING
                    printf("Data Sent -> %02x\n", data[register_accessed]);
                #endif

                break;
            // Register Two Contains the Data from the device
            case 1:
                i2c_write_raw_blocking(i2c0, &data[register_accessed], 2);

                #ifdef DEBUGGING
                    printf("Data Sent -> %02x\n", data[register_accessed]);
                #endif

                break;
            // If An incorrect Register Is Accessed the output is the invalid_data variable
            default:

                i2c_write_raw_blocking(i2c0, &invalid_data, 2);

                #ifdef DEBUGGING
                    printf("Data Sent -> %02x\n", invalid_data);
                #endif

                #ifdef DEBUGGING
                    printf("Incorrect Register\n");
                #endif

                break;
        }

        #ifdef DEBUGGING
            printf("Sent Data\n");
        #endif

        // Clear the interrupt
        i2c0->hw->clr_rd_req;
    }
}

// NEED TO UPDATE
void CPS_CONVERTER(char one, char two){

    uint16_t CPM = 0;

    if ( isdigit(two) == 0)
    {
        int CPS = one - '0';
        CPM = CPS * 60 + (rand() % 20);
    }
    else
    {
        int CMP_1 = one - '0';
        int CMP_2 = two - '0';
        int CPS = (CMP_1*10) + CMP_2;
        CPM = CPS * 60 + (rand() % 100);
    }
    
    data[1] = CPM;
    
}

/**
 * @brief This interrupt is called whenever data is avaliable on the UART Bus.
 * It is used to read the data from the geiger counter and then update the 
 * data array in memory. As you can make no guarantees on what point the
 * UART data is at, it reads data into a buffer and only when the
 * buffer matches the line format expected from the sensor will it be sent to
 * the data extractor
 * 
 */
void uart_isq_handler() {

    // Checks if there is Data avaliable on the the RX port
    while (uart_is_readable(UART_ID)) 
    {   
        // Retrives the first character from the UART Port
        ch[0] = uart_getc(UART_ID);
        
        // Checking if the character is a new line so that we dont read data from the middle
        if (ch[0] == '\n')
        {   
            // Resetting the buffer counter
            counter = 0;

            // Checking if the current line we have in the buffer is Valid
            if (temp[0] == 'C')
            {
                // If its is valid send the current data to the data extractor
                CPS_CONVERTER(temp[5], temp[6]);
                
                #ifdef DEBUGGING
                    printf("%s -> %c%c\n",temp , temp[5], temp[6]);
                #endif

            }
            /*
            // Used for debugging
            else
            {
                #ifdef DEBUGGING
                    printf("Invalid Line In Buffer\n");
                    printf("%s\n", temp);
                #endif
            }
            
            */
            
            
        }
        // If we're not on a new line add the character to the buffer
        else
        {
            // Adding the Serial Charecter into a buffer
            temp[counter] = ch[0];

            // Incrementing the buffer counter
            counter++;
        }
        
    }

}


/* *************** [Main Loop] *************** */ 

int main() {

    // Initialize GPIO and Debug Over USB UART - MUST ENABLE IN MAKEFILE
    stdio_init_all();

    /* *************** [I2C CONFIGURATION START] *************** */

    // Initializing the I2C0 Controller on the Pi Pico
    i2c_init(i2c0, 10000);

    // Setting the I2C0 Controller as a I2C Slave
    i2c_set_slave_mode(i2c0, true, I2C_ADDR);

    // Enabling I2C Mode on Pins GP4 and GP5
    gpio_set_function(SDA_SLAVE, GPIO_FUNC_I2C);
    gpio_set_function(SCL_SLAVE, GPIO_FUNC_I2C);

    // Enabling the internal Pull Up resistors for I2C to work
    gpio_pull_up(SDA_SLAVE);
    gpio_pull_up(SCL_SLAVE);

    // Enable the interrupts on i2c0 controller on access
    i2c0->hw->intr_mask = (I2C_IC_INTR_MASK_M_RD_REQ_BITS | I2C_IC_INTR_MASK_M_RX_FULL_BITS);

    // Set up the interrupt handler function to call on an interrupt
    irq_set_exclusive_handler(I2C0_IRQ, i2c0_irq_handler);

    // Enable I2C interrupts on the NVIC
    irq_set_enabled(I2C0_IRQ, true);
    irq_set_priority(I2C0_IRQ, 0);

    /* *************** [I2C CONFIGURATION END] *************** */


    /* *************** [UART CONFIGURATION START] *************** */

    // Initialise UART 0 which is connected to the geiger counter
    uart_init(UART_ID, 9600);

    // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
    gpio_set_function(UART_RX, GPIO_FUNC_UART);
    gpio_set_function(UART_TX, GPIO_FUNC_UART);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, 8, 1, 0);

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART0_IRQ, uart_isq_handler);
    irq_set_priority(UART0_IRQ, 10);
    irq_set_enabled(UART0_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);
    srand(time(NULL));

    /* *************** [UART CONFIGURATION END] *************** */


    // Printing a Debugging statement to the USB UART
    #ifdef DEBUGGING
        printf("I2C and UART Set-Up and Active\n");
    #endif

    // Loop forever doing nothing
    while (true) {
        tight_loop_contents();
    }

    return 0;
}