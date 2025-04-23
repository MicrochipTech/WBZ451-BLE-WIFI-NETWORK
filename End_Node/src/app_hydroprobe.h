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
 * \brief This file contains API for Hydro Probe Click driver.
 *
 * \addtogroup hydroprobe Hydro Probe Click Driver
 * @{
 */
// ----------------------------------------------------------------------------

#ifndef HYDROPROBE_H
#define HYDROPROBE_H



#include "definitions.h"

// -------------------------------------------------------------- PUBLIC MACROS 

#define HYDROPROBE_SLAVE_ADR    0x4D
#define HYDROPROBE_DRY_VAL      0x0D50
#define HYDROPROBE_WET_VAL      0x07B0



#ifdef __cplusplus
extern "C"{
#endif


 void APP_HydroProbe_Read(uint8_t *data_buf, uint8_t len );

/**
 * @brief Read raw data function
 *
 * @param ctx          Click object.
 * 
 * @returns result 16-bit value that represents ADC value from output register
 *
 * Function is used to read raw data from MCP3221.
**/
uint16_t hydroprobe_read_raw ();

void APP_HydroProbe_GetValues();

#ifdef __cplusplus
}
#endif
#endif  // _HYDROPROBE_H_

/** \} */ // End public_function group
/// \}    // End click Driver group  
/*! @} */
// ------------------------------------------------------------------------- END
