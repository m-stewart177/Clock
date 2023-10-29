//
// Created by mike on 11/10/23.
//

#pragma once

#include <I2C_Functions.h>

#define __I2C_Addr_BME688__ 0x76

namespace Sensors
{
    struct TemperatureCalibaration
    {
        int32_t Coeff1;
        int32_t Coeff2;
        int32_t Coeff3;
    };

    struct PressureCalibration
    {
        int32_t Coeff1;
        int32_t Coeff2;
        int32_t Coeff3;
        int32_t Coeff4;
        int32_t Coeff5;
        int32_t Coeff6;
        int32_t Coeff7;
        int32_t Coeff8;
        int32_t Coeff9;
        int32_t Coeff10;
    };
    struct HumidityCalibration
    {
        int32_t Coeff1;
        int32_t Coeff2;
        int32_t Coeff3;
        int32_t Coeff4;
        int32_t Coeff5;
        int32_t Coeff6;
        int32_t Coeff7;
    };

    class Sensors: private I2C_Functions
    {

    public:
        Sensors(bool multiplexerEnable = false, uint8_t multiplexerChannel = 0);

    public:
        bool Begin();

    private:
        TemperatureCalibaration m_tempCalibration;
        PressureCalibration     m_pressClibration;
        HumidityCalibration     m_humiCalibration;
    };

} // Sensors
