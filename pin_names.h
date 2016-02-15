// Pin Names for mbed-mazda project

#ifdef TARGET_LPC1114
#define PIN_SPI_MOSI    dp2
#define PIN_SPI_MISO    dp1
#define PIN_SPI_SCK     dp6
#define PIN_POT_CS      dp4

#define PIN_WHEEL_AIN   dp9

#define PIN_SERIAL_TX   dp16
#define PIN_SERIAL_RX   dp15

#define PIN_RN52_TX     PIN_SERIAL_TX
#define PIN_RN52_RX     PIN_SERIAL_RX
#define PIN_RN52_IO9    dp17
#define PIN_RN52_IO2    dp18

#elif defined(TARGET_LPC1768)
// not used right now, but there to let things compile
#define PIN_SPI_MOSI    p5
#define PIN_SPI_MISO    p6
#define PIN_SPI_SCK     p7
#define PIN_POT_CS      p8
#define PIN_WHEEL_AIN   p20

// serial for debugging
#define PIN_SERIAL_TX   USBTX
#define PIN_SERIAL_RX   USBRX

#define PIN_RN52_TX     p13
#define PIN_RN52_RX     p14
#define PIN_RN52_IO9    p16
#define PIN_RN52_IO2    p21

#define PIN_KEYPAD_SDA  p9
#define PIN_KEYPAD_SCL  p10

#else
#error "Unsupported Target for this project"
#endif
