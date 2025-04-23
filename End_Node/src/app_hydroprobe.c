/*
 * MikroSDK - MikroE Software Development Kit
 * Copyright© 2020 MikroElektronika d.o.o.
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE 
 * OR OTHER DEALINGS IN THE SOFTWARE. 
 */

/*!
 * \file
 *
 */

#include "app_hydroprobe.h"

void APP_HydroProbe_Read(uint8_t *data_buf, uint8_t len )
{
    SERCOM2_I2C_Read(HYDROPROBE_SLAVE_ADR, data_buf, len );
}
void APP_HydroProbe_GetValues()
{
    uint16_t dry_val = HYDROPROBE_DRY_VAL;
    uint16_t wet_val = HYDROPROBE_WET_VAL;
    float result = 0.0;
    appData.hydroProbeVal = appData.i2cReadVal[0];
    appData.hydroProbeVal <<= 8;
    appData.hydroProbeVal |= appData.i2cReadVal[1];
    appData.hydroProbeVal &= 0x0FFF;
    //SYS_CONSOLE_PRINT("HP Val: %x\n\r",appData.hydroProbeVal);
    if ( appData.hydroProbeVal > dry_val )
    {
        appData.hydroProbeVal = dry_val;
    }
    if ( appData.hydroProbeVal < wet_val )
    {
        appData.hydroProbeVal = wet_val;
    }

    result = 100.0 - ( ( ( ( float ) appData.hydroProbeVal - ( float ) wet_val ) * 100.0 ) / ( ( float ) dry_val - ( float ) wet_val ) );
    
    appData.relHumPercent = ( uint8_t ) result; 
    
    //SYS_CONSOLE_PRINT("RH Val: %d\n\r",appData.relHumPercent); 
    
}
