/*
Copyright (c) 2011 Anthony Buckton (abuckton [at] blackink [dot} net {dot} au)

 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
 
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

   Parts written by Jim Lindblom of Sparkfun
   Ported to mbed by A.Buckton, Feb 2011
*/

#ifndef MPR121_H
#define MPR121_H

//using namespace std;

class Mpr121 
{

public:
    // i2c Addresses, bit-shifted
    enum Address { ADD_VSS = 0xb4,// ADD->VSS = 0x5a <-wiring on Sparkfun board
                   ADD_VDD = 0xb6,// ADD->VDD = 0x5b
                   ADD_SCL = 0xb8,// ADD->SDA = 0x5c
                   ADD_SDA = 0xba // ADD->SCL = 0x5d
                 };

    // Real initialiser, takes the i2c address of the device.
    Mpr121(I2C *i2c, Address i2cAddress);
    
    bool getProximityMode();
    
    void setProximityMode(bool mode);
    
    int readTouchData();
               
    unsigned char read(int key);
    
    int write(int address, unsigned char value);
    int writeMany(int start, unsigned char* dataSet, int length);

    void setElectrodeThreshold(int electrodeId, unsigned char touchThreshold, unsigned char releaseThreshold);
        
protected:
    // Configures the MPR with standard settings. This is permitted to be overwritten by sub-classes.
    void configureSettings();
    
private:
    // The I2C bus instance.
    I2C *i2c;

    // i2c address of this mpr121
    Address address;
};


// MPR121 Register Defines
#define    MHD_R        0x2B
#define    NHD_R        0x2C
#define    NCL_R        0x2D
#define    FDL_R        0x2E
#define    MHD_F        0x2F
#define    NHD_F        0x30
#define    NCL_F        0x31
#define    FDL_F        0x32
#define    NHDT         0x33
#define    NCLT         0x34
#define    FDLT         0x35
// Proximity sensing controls
#define    MHDPROXR     0x36
#define    NHDPROXR     0x37
#define    NCLPROXR     0x38
#define    FDLPROXR     0x39
#define    MHDPROXF     0x3A
#define    NHDPROXF     0x3B
#define    NCLPROXF     0x3C
#define    FDLPROXF     0x3D
#define    NHDPROXT     0x3E
#define    NCLPROXT     0x3F
#define    FDLPROXT     0x40
// Electrode Touch/Release thresholds
#define    ELE0_T       0x41
#define    ELE0_R       0x42
#define    ELE1_T       0x43
#define    ELE1_R       0x44
#define    ELE2_T       0x45
#define    ELE2_R       0x46
#define    ELE3_T       0x47
#define    ELE3_R       0x48
#define    ELE4_T       0x49
#define    ELE4_R       0x4A
#define    ELE5_T       0x4B
#define    ELE5_R       0x4C
#define    ELE6_T       0x4D
#define    ELE6_R       0x4E
#define    ELE7_T       0x4F
#define    ELE7_R       0x50
#define    ELE8_T       0x51
#define    ELE8_R       0x52
#define    ELE9_T       0x53
#define    ELE9_R       0x54
#define    ELE10_T      0x55
#define    ELE10_R      0x56
#define    ELE11_T      0x57
#define    ELE11_R      0x58
// Proximity Touch/Release thresholds
#define    EPROXTTH     0x59
#define    EPROXRTH     0x5A
// Debounce configuration
#define    DEB_CFG      0x5B
// AFE- Analogue Front End configuration
#define    AFE_CFG      0x5C 
// Filter configuration
#define    FIL_CFG      0x5D
// Electrode configuration - transistions to "active mode"
#define    ELE_CFG      0x5E

#define GPIO_CTRL0      0x73
#define GPIO_CTRL1      0x74
#define GPIO_DATA       0x75
#define    GPIO_DIR     0x76
#define    GPIO_EN      0x77
#define    GPIO_SET     0x78
#define GPIO_CLEAR      0x79
#define GPIO_TOGGLE     0x7A
// Auto configration registers
#define    AUTO_CFG_0   0x7B
#define    AUTO_CFG_U   0x7D
#define    AUTO_CFG_L   0x7E
#define    AUTO_CFG_T   0x7F

// Threshold defaults
// Electrode touch threshold
#define    E_THR_T      0x0F   
// Electrode release threshold 
#define    E_THR_R      0x0A    
// Prox touch threshold
#define    PROX_THR_T   0x02
// Prox release threshold
#define    PROX_THR_R   0x02

#endif
