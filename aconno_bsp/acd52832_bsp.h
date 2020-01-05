/* Copyright (c) 2016 Aconno. All Rights Reserved.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**
 *  General IO pins
 */
    #define PIN_BUTTON                                  (p25)       //S1  gpio number (Active low)
    #define PIN_LED                                     (p26)       //LD1 gpio number (Active low)
    #define PIN_IRLED                                   (p27)       //LD4 gpio number (Active high)
    #define PIN_BUZZER                                  (p31)       //LS1 gpio number (Active high)

    #define PIN_LED_RED                                 (p22)       //Module red LED
    #define PIN_LED_BLUE                                (p23)       //Module blue LED
    #define PIN_LED_GREEN                               (p24)       //Module green LED

/**
 *  Alternate General IO pins
 *  Connect:
 *          (+, c, -) (Orange, Red, Brown)
 */
    #define PIN_SERVO1                                  (p25)        //Servo1 gpio number
    #define PIN_SERVO2                                  (p26)        //Servo2 gpio number

/**
 *  Analog to Digital Conversion (ADC) pins
 */
    #define ADC_BAT                                     (p2)         //Battery
    #define ADC_POT                                     (p28)        //Potentiometer
    #define ADC_LIGHT                                   (p29)        //Light Dependant Resistor
    #define ADC_TEMP                                    (p30)        //Temperature Dependant Resistor

/**
 *  Electronic Paper Display (EPD) pins
 */
    #define PIN_EPD_CS                                  (p5)          //EPD Chip select   pin (Active low)
    #define PIN_EPD_DC                                  (p6)          //EPD Data/Command pin (Data active high)
    #define PIN_EPD_RST                                 (p7)          //EPD Reset pin (Active low)
    #define PIN_EPD_BUSY                                (p8)          //EPD Busy pin (Active low)
    #define PIN_EPD_SCK                                 (p4)          //EPD SCK pin
    #define PIN_EPD_MOSI                                (p3)          //EPD MOSI pin
    #define PIN_EPD_MISO                                (0xFF)        //EPD MISO pin not used

/**
 *  IO Expander pins
 */
    #define PIN_EXP_RST                                 (p19)         //IO Expander reset pin
    #define PIN_EXP_INT_A                               (p11)         //IO Expander interrupt for PORTA
    #define PIN_EXP_INT_B                               (p12)         //IO Expander interrupt for PORTB
    #define PIN_EXP_SDA                                 (p20)         //TWI SCK pin
    #define PIN_EXP_SCL                                 (p21)         //TWI SCL pin

/**
 *  Pins concerning TWI Addresses
 *  Control bits are added by nrf_drv_twi
*/
    #define TWI_EXPANDER_ADDR                           (0x40)  //Port expander address
    #define TWI_AG_ADDR                                 (0x6A << 1)  //Accelerometer/Gyroscope address
    #define TWI_MAG_ADDR                                (0x1C << 1)  //Magnetometer address

    /**
     *  Define structure used by bluetooth library
     *  Define if BLE_LIB is defined as preprocessor symbol
    */
#ifdef BLE_LIB
    // Low frequency clock source to be used by the SoftDevice
    #define NRF_CLOCK_LFCLKSRC {                            \
        .source        = NRF_CLOCK_LF_SRC_XTAL,             \
        .rc_ctiv       = 0,                                 \
        .rc_temp_ctiv  = 0,                                 \
        .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}
#endif
