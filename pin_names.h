/* Pin Names for mbed-mazda project */

#ifdef TARGET_LPC1114
#define PIN_SPI_MOSI    dp2
#define PIN_SPI_MISO    dp1
#define PIN_SPI_SCK     dp6
#define PIN_POT_CS      dp4

#define PIN_WHEEL_AIN   dp9

#define PIN_SERIAL_TX   dp16
#define PIN_SERIAL_RX   dp15

#else
#error "Unsupported Target for this project"
#endif
