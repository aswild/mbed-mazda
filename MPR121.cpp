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
*/

#include <mbed.h>
//#include <sstream>
//#include <string>
//#include <list>

#include <mpr121.h>

MPR121::MPR121(I2C *i2c, Address i2cAddress)
{
    this->i2c = i2c;

    address = i2cAddress;

    // Configure the MPR121 settings to default
    this->configureSettings();
}


void MPR121::configureSettings()
{
    // Put the MPR into setup mode
    this->write(ELE_CFG,0x00);

    // Electrode filters for when data is > baseline
    unsigned char gtBaseline[] = {
         0x01,  //MHD_R
         0x01,  //NHD_R
         0x00,  //NCL_R
         0x00   //FDL_R
         };

    writeMany(MHD_R,gtBaseline,4);

     // Electrode filters for when data is < baseline
     unsigned char ltBaseline[] = {
        0x01,   //MHD_F
        0x01,   //NHD_F
        0xFF,   //NCL_F
        0x02    //FDL_F
        };

    writeMany(MHD_F,ltBaseline,4);

    // Electrode touch and release thresholds
    unsigned char electrodeThresholds[] = {
        E_THR_T, // Touch Threshhold
        E_THR_R  // Release Threshold
        };

    for(int i=0; i<12; i++){
        int result = writeMany((ELE0_T+(i*2)),electrodeThresholds,2);
    }

    // Proximity Settings
    unsigned char proximitySettings[] = {
        0xff,   //MHD_Prox_R
        0xff,   //NHD_Prox_R
        0x00,   //NCL_Prox_R
        0x00,   //FDL_Prox_R
        0x01,   //MHD_Prox_F
        0x01,   //NHD_Prox_F
        0xFF,   //NCL_Prox_F
        0xff,   //FDL_Prox_F
        0x00,   //NHD_Prox_T
        0x00,   //NCL_Prox_T
        0x00    //NFD_Prox_T
        };
    writeMany(MHDPROXR,proximitySettings,11);

    unsigned char proxThresh[] = {
        PROX_THR_T, // Touch Threshold
        PROX_THR_R  // Release Threshold
        };
    writeMany(EPROXTTH,proxThresh,2);

    this->write(FIL_CFG,0x04);

    // Set the electrode config to transition to active mode
    this->write(ELE_CFG,0x0c);
}

void MPR121::setElectrodeThreshold(int electrode, unsigned char touch, unsigned char release){

    if(electrode > 11) return;

    // Get the current mode
    unsigned char mode = this->read(ELE_CFG);

    // Put the MPR into setup mode
    this->write(ELE_CFG,0x00);

    // Write the new threshold
    this->write((ELE0_T+(electrode*2)), touch);
    this->write((ELE0_T+(electrode*2)+1), release);

    //Restore the operating mode
    this->write(ELE_CFG, mode);
}


unsigned char MPR121::read(int key){

    unsigned char data[2];

    //Start the command
    i2c->start();

    // Address the target (Write mode)
    i2c->write(address);

    // Set the register key to read
    i2c->write(key);

    // Re-start for read of data
    i2c->start();

    // Re-send the target address in read mode
    i2c->write(address+1);

    // Read in the result
    data[0] = i2c->read(0);

    // Reset the bus
    i2c->stop();

    return data[0];
}


int MPR121::write(int key, unsigned char value){

    //Start the command
    i2c->start();

    // Address the target (Write mode)
    int ack1= i2c->write(address);

    // Set the register key to write
    int ack2 = i2c->write(key);

    // Read in the result
    int ack3 = i2c->write(value);

    // Reset the bus
    i2c->stop();

    return (ack1+ack2+ack3)-3;
}


int MPR121::writeMany(int start, unsigned char* dataSet, int length){
    //Start the command
    i2c->start();

    // Address the target (Write mode)
    int ack= i2c->write(address);
    if(ack!=1){
        return -1;
    }

    // Set the register key to write
    ack = i2c->write(start);
    if(ack!=1){
        return -1;
    }

    // Write the date set
    int count = 0;
    while(ack==1 && (count < length)){
        ack = i2c->write(dataSet[count]);
        count++;
    }
    // Stop the cmd
    i2c->stop();

    return count;
}


bool MPR121::getProximityMode(){
    if(this->read(ELE_CFG) > 0x0c)
        return true;
    else
        return false;
}

void MPR121::setProximityMode(bool mode){
    this->write(ELE_CFG,0x00);
    if(mode){
        this->write(ELE_CFG,0x30); //Sense proximity from ALL pads
    } else {
        this->write(ELE_CFG,0x0c); //Sense touch, all 12 pads active.
    }
}


int MPR121::readTouchData(){
    return this->read(0x00);
}
