/**
 * @file main.c
 * @author Ajay Varghese (Team C)
 * @date 21 March 2022
 * @brief The program aims to set-up i2c slave communication on the Raspberry
 * Pi Pico so that it can interface with I2C master controllers to send data
 * when requested of it. 
 * 
 * At any point the I2C master can request data from the data array. This is
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

// library to enable multithreading for the two systems
#include "pico/multicore.h"



/* *************** [CONFIGURATION VARIABLES] *************** */ 



// Uncomment this line to enable debugging statements onto USB UART - MUST ENABLE IN MAKEFILE
#define DEBUGGING
#ifndef PICO_DEFAULT_LED_PIN
    #warning blink example requires a board with a regular LED
#else
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
#endif


/* *************** [I2C CONFIGURATION VARIABLES START] *************** */



// The Address of the I2C Peripheral
#define I2C_ADDR 0x4D   // [WARNING] can only take values between 0x08 - 0x77

// The Specific GP Pins on the Pi Pico to enable I2C on
#define SDA_SLAVE 8
#define SCL_SLAVE 9

// Data Array to hold the information from the registers
// 0 -> The i2c address of the device itself
// 1 -> The data
volatile uint16_t data[2] = {I2C_ADDR, 0};

// Register Variable to Read/Write data too
volatile uint8_t register_accessed;



/* *************** [I2C CONFIGURATION VARIABLES END] *************** */




/* *************** [Pulse Detect CONFIGURATION VARIABLES START] *************** */



// The Pin the Pulse from the Geiger Counter is connected to
#define PULSE_PIN 2

// Variable to hold the number of pulses the Pico receives
volatile uint16_t COUNTS = 0;

// A buffer to add the values to when in slow mode
volatile uint16_t SLOW_MODE_BUFFER[60] = {0};

// Variable to index the SLOW_MODE_BUFFER, resets every 60 seconds
volatile uint16_t SLOW_MODE_INDEX = 0;

// Struct holding the repeated timer
struct repeating_timer TIMER;



/* *************** [Pulse Detect CONFIGURATION VARIABLES END] *************** */



/* *************** [EXECTUTION CODE] *************** */ 



/**
 * @brief This interrupt is called whenever the i2c0 bus is accessed by the
 * master. It handles what to do on reads and writes as well as resetting
 * interrupt registers
 * 
 */
void i2c0_irq_handler() {

    // Get interrupt status
    volatile uint32_t status = i2c0->hw->intr_stat;

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

        volatile uint16_t invalid_data = 0;

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



/**
 * @brief This function is called whenever the GPIO interrupt flag is high
 * This is currenty set to trigger on each pulse
 * 
 * @param gpio The Pin the called the function
 * @param event The type of event it is
 */
void gpio_callback(uint gpio, uint32_t event) 
{
    
    // Checking if the Pulse Pin is the one that called this function
    if (gpio == PULSE_PIN)
    {   
        // Incrementing the Pulse Count
        COUNTS++;

        // Toggling the LED from its previous state
        if (gpio_get_out_level(LED_PIN)  == true)
        {
            gpio_put(LED_PIN, 0);
            return;
        }

        gpio_put(LED_PIN, 1);

        // Returning
        return;
    }

    // Returning if any other pin called the interrupt
    return;
    
}



/**
 * @brief The function is called the the repeating timer interrupt is high,
 * this is currently set to trigger once every seccond
 * 
 * @param t The Adddress of the timer that called this function
 */
void repeating_timer_callback(struct repeating_timer *t)
{
    // Local variable to copy the number of counts to
    volatile uint16_t CPS = COUNTS;

    // Local variable to do the correct calculations for CPM depending on Fast/Slow Mode
    volatile uint16_t CPM = 0;

    // Reseting the Number of pulses received
    COUNTS = 0;

    // Adding the current CPS to the SLOW_MODE_BUFFER
    SLOW_MODE_BUFFER[SLOW_MODE_INDEX] = CPS;

    // Incrementing the SLOW_MODE_INDEX index
    SLOW_MODE_INDEX++;

    // Checking if the index is past 60
    if (SLOW_MODE_INDEX == 60)
    {   
        // Reseting the index if it is past the array size
        SLOW_MODE_INDEX = 0;
    }
    
    // Checking if we're getting less 5 pulses per seccond
    if (CPS < 5)
    {
        /**
         * If CPS is less than 5 then we're in slow mode, when in slow mode
         * the cps is added to a revloving buffer of 60 which summed will give
         * the counts per miniute.
        */

        // Summing up the buffer
        for (uint8_t i = 0; i < 60; i++)
        {
            CPM = CPM + SLOW_MODE_BUFFER[i];
        }

        #ifdef DEBUGGING
            printf("SLOW_MODE ");
        #endif
        
    }
    else
    {
        /**
         * If CPS is over than 5 then we're in fast mode, when in fast mode
         * the CPM is 60 '*' the current CPS. So many counts are comming in
         * per seconds that a buffer will be too slow.
        */

        // Getting the CPM
        CPM = CPS*60;

        #ifdef DEBUGGING
            printf("FAST_MODE ");
        #endif
    }

    // Writing the Current CPM to the Data register
    data[1] = CPM;

    #ifdef DEBUGGING
        printf("CPS %i, CPM %i\n", CPS, CPM);
    #endif
}


void core1_entry()
{
    /* *************** [Pulse Interrupt CONFIGURATION START] *************** */

    
    // Initialising the in built LED to toggle when new pulses are received
    gpio_init(LED_PIN);

    // Setting the LED to be an Output Pin
    gpio_set_dir(LED_PIN, GPIO_OUT);


    // Initialising the Pulse Pin
    gpio_init(PULSE_PIN);

    // Setting the Pulse Pin as an Input
    gpio_set_dir(PULSE_PIN, GPIO_IN);

    // Setting the Pulse Pin to pull down by default
    gpio_pull_down(PULSE_PIN);

    // Enabeling the Interrupt on the Pulse Pin to trigger on rising edge
    gpio_set_irq_enabled_with_callback(PULSE_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    // Creating the timer to interrupt every seccond to calculate the CPS
    add_repeating_timer_ms(1000, repeating_timer_callback, NULL, &TIMER);



    /* *************** [Pulse Interrupt CONFIGURATION END] *************** */


    // Loop forever doing nothing on core 1
    while (true) {
        tight_loop_contents();
    }
}




/* *************** [Main Loop] *************** */ 



int main() {

    // Initialize GPIO and Debug Over USB UART - MUST ENABLE IN MAKEFILE
    stdio_init_all();

    // Starting the Pulse Logic on Core 1
    multicore_launch_core1(core1_entry);



    /* *************** [I2C CONFIGURATION START] *************** */



    // Initializing the I2C0 Controller on the Pi Pico
    i2c_init(i2c0, 100000);

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



    // Printing a Debugging statement to the UART0 on GP0 and GP1
    #ifdef DEBUGGING
        printf("I2C and UART Set-Up and Active\n");
    #endif

    // Loop forever doing nothing
    while (true) {
        tight_loop_contents();
    }

    return 0;
}


