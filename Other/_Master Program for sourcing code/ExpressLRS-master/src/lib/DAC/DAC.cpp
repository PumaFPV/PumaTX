
#ifdef TARGET_R9M_TX

#include "DAC.h"
#include "SX127xDriver.h"

int R9DAC::LUT[8][4] = {
    // mw, dB, gain, APC2volts*1000, figures assume 2dBm input
    {10, 10, 8, 720},
    {25, 14, 12, 875},
    {50, 17, 15, 1000},
    {100, 20, 18, 1140},
    {250, 24, 22, 1390},
    {500, 27, 25, 1730},
    {1000, 30, 28, 2100},
    {2000, 33, 31, 2600}, // Danger untested at high power
};

uint32_t R9DAC::CurrVoltageMV = 0;
uint8_t R9DAC::CurrVoltageRegVal = 0;
uint8_t R9DAC::SDA = 0;
uint8_t R9DAC::SCL = 0;
uint8_t R9DAC::ADDR = 0;

DAC_STATE_ R9DAC::DAC_STATE = UNKNOWN;

void R9DAC::init()
{
    Serial.println("Wire.h begin()");

    R9DAC::SDA = GPIO_PIN_SDA;
    R9DAC::SCL = GPIO_PIN_SCL;
    R9DAC::ADDR = 0b0001100;

    Wire.setSDA(GPIO_PIN_SDA); // set is needed or it wont work :/
    Wire.setSCL(SCL);
    Wire.begin();
    R9DAC::DAC_STATE = UNKNOWN;
}

void R9DAC::standby()
{
    if (R9DAC::DAC_STATE != STANDBY)
    {
        Wire.beginTransmission(R9DAC::ADDR);
        Wire.write(0x00);
        Wire.write(0x00);
        Wire.endTransmission();
        R9DAC::DAC_STATE = STANDBY;
    }
}

void R9DAC::resume()
{
    if (R9DAC::DAC_STATE != RUNNING)
    {
        Radio.SetOutputPower(0b0000);
        R9DAC::setVoltageRegDirect(CurrVoltageRegVal);
    }
}

void R9DAC::setVoltageMV(uint32_t voltsMV)
{
    uint8_t ScaledVolts = map(voltsMV, 0, VCC, 0, 255);
    setVoltageRegDirect(ScaledVolts);
    CurrVoltageMV = voltsMV;
    Serial.println(CurrVoltageMV);
}

void R9DAC::setVoltageRegDirect(uint8_t voltReg)
{
    CurrVoltageRegVal = voltReg;
    uint8_t RegH = ((voltReg & 0b11110000) >> 4) + (0b0000 << 4);
    uint8_t RegL = (voltReg & 0b00001111) << 4;

    Radio.SetOutputPower(0b0000);
    Wire.beginTransmission(R9DAC::ADDR);
    Wire.write(RegH);
    Wire.write(RegL);
    Wire.endTransmission();
}

void R9DAC::setPower(DAC_PWR_ power)
{
    Radio.SetOutputPower(0b0000);
    uint32_t reqVolt = LUT[(uint8_t)power][3];
    R9DAC::setVoltageMV(reqVolt);
}

#endif