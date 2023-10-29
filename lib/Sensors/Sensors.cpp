//
// Created by mike on 11/10/23.
//

#include <Arduino.h>

#include "Sensors.h"

#define TEMP_COEFF1_REG_LO 0xE9
#define TEMP_COEFF1_REG_HI 0xEA
#define TEMP_COEFF2_REG_LO 0x8A
#define TEMP_COEFF2_REG_HI 0x8B
#define TEMP_COEFF3_REG    0x8C

#define PRESS_COEFF1_REG_LO 0x8E
#define PRESS_COEFF1_REG_HI 0x8F
#define PRESS_COEFF2_REG_LO 0x90
#define PRESS_COEFF2_REG_HI 0x91
#define PRESS_COEFF3_REG    0x92
#define PRESS_COEFF4_REG_LO 0x94
#define PRESS_COEFF4_REG_HI 0x95
#define PRESS_COEFF5_REG_LO 0x96
#define PRESS_COEFF5_REG_HI 0x97
#define PRESS_COEFF6_REG    0x99
#define PRESS_COEFF7_REG    0x98
#define PRESS_COEFF8_REG_LO 0x9C
#define PRESS_COEFF8_REG_HI 0x9D
#define PRESS_COEFF9_REG_LO 0x9E
#define PRESS_COEFF9_REG_HI 0x9F
#define PRESS_COEFF10_REG 0xA0

#define HUMI_COEFF1_LO      0xE3
#define HUMI_COEFF1_HI_0x0F 0xE2
#define HUMI_COEFF2_LO      0xE1
#define HUMI_COEFF2_HI_0xF0 0xE2
#define HUMI_COEFF3         0xE4
#define HUMI_COEFF4         0xE5
#define HUMI_COEFF5         0xE6
#define HUMI_COEFF6         0xE7
#define HUMI_COEFF7         0xE8

namespace
{
    using Sensors::TemperatureCalibaration;
    using Sensors::PressureCalibration;
    using Sensors::HumidityCalibration;

    void ReadClibrationData(I2C_Functions& i2Functions,
                            TemperatureCalibaration& tempClibration,
                            PressureCalibration&     pressCalibration,
                            HumidityCalibration&     humiCalibration
                            )
    {
        uint8_t data[22];
        i2Functions.Read_Multiple_Register(TEMP_COEFF2_REG_LO, data, 22, true);

        tempClibration.Coeff2 = (data[1] << 8) | data[0];
        tempClibration.Coeff1 = data[2];

        pressCalibration.Coeff1 = (data[5] << 8) | data[4];
        pressCalibration.Coeff2 = (data[7] << 8) | data[6];
        pressCalibration.Coeff3 = data[8];
        pressCalibration.Coeff4 = (data[11] << 8) | data[10];
        pressCalibration.Coeff5 = (data[13] << 8) | data[12];
        pressCalibration.Coeff6 = data[15];
        pressCalibration.Coeff7 = data[14];
        pressCalibration.Coeff8 = (data[19] << 8) | data[18];
        pressCalibration.Coeff9 = (data[21] << 8) | data[20];
        pressCalibration.Coeff10 = data[22];

        i2Functions.Read_Multiple_Register(HUMI_COEFF2_LO, data, 10, true);
        tempClibration.Coeff1 = (data[9] << 8) | data[8];

        humiCalibration.Coeff2 = ((data[1] && 0xF0) << 4) | data[0];
        humiCalibration.Coeff1 = ((data[1] && 0x0F) << 8) | data[2];
        humiCalibration.Coeff3 = data[3];
        humiCalibration.Coeff4 = data[4];
        humiCalibration.Coeff5 = data[5];
        humiCalibration.Coeff6 = data[6];
        humiCalibration.Coeff7 = data[7];
    }
} // namespace

namespace Sensors
{
    Sensors::Sensors(bool const multiplexerEnable, uint8_t const multiplexerChannel) :
            I2C_Functions(__I2C_Addr_BME688__, multiplexerEnable, multiplexerChannel)
    {}

    bool Sensors::Begin()
    {
        if (I2C_Functions::Control_Device())
        {
            ReadClibrationData(*this, m_tempCalibration, m_pressClibration, m_humiCalibration);
            return true;
        }

        return false;
    }
} // Sensors