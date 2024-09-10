/**
* \file  demo_output.c
*
* \brief Demo output Implementation.
*
* Copyright (c) 2024 Microchip Technology Inc. and its subsidiaries. 
*
* \asf_license_start
*
* \page License
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products. 
* It is your responsibility to comply with third party license terms applicable 
* to your use of third party software (including open source software) that 
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, 
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, 
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE 
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL 
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE 
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE 
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY 
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/
#include <stdio.h>
#include "config/default/definitions.h"
#include "miwi_config.h"
#include "miwi_config_p2p.h"
#include "miwi_api.h"
#include "app_miwi_output.h"


/*************************************************************************/
// the following two variable arrays are the data to be transmitted
// in this demo. They are bit map of English word "MiWi" and "DE"
// respectively.
/*************************************************************************/
const uint8_t MiWi[6][21] =
{
    {0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0x20,0x20,0xB2,0x20,0xB2,0x0D,0x0A},
    {0xB2,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x0D,0x0A},
    {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0D,0x0A}
    
};   

const uint8_t DE[6][11] =
{
    {0xB2,0xB2,0xB2,0x20,0x20,0xB2,0xB2,0xB2,0xB2,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0xB2,0xB2,0xB2,0xB2,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0x0D,0x0A},
    {0xB2,0xB2,0xB2,0x20,0x20,0xB2,0xB2,0xB2,0xB2,0x0D,0x0A},
    {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0D,0x0A}
}; 

const uint8_t TestMsg[TEST_MSG_SIZE] = {"Test\r\n"};

#define DEBOUNCE_TIME 0x00003FFF

#if defined (CONF_BOARD_JOYSTICK)
#define IS_JOYSTICK_PUSH_ACTION(x) (x > 25 && x < 35)
#define IS_JOYSTICK_UP_ACTION(x) (x > 85 && x < 95)
#define IS_JOYSTICK_LEFT_ACTION(x) (x > 120 && x < 130)
#define IS_JOYSTICK_DOWN_ACTION(x) (x > 220 && x < 230)
#define IS_JOYSTICK_RIGHT_ACTION(x) (x > 200 && x < 215)

static struct adc_module adc_instance;
#endif

bool PUSH_BUTTON_pressed;
MIWI_TICK PUSH_BUTTON_press_time;

void APP_Output_Greeting(void)
{    
    #if defined (ENABLE_CONSOLE)
        #if defined(PROTOCOL_P2P)
            SYS_CONSOLE_PRINT("\r\nStarting Node 1 of Simple Demo for MiWi(TM) P2P Stack ...");  
        #endif
        #if defined(PROTOCOL_STAR)
            //SYS_CONSOLE_PRINT("\r\nStarting Node 1 of Simple Demo for MiWi(TM) STAR Stack ...");
            SYS_CONSOLE_PRINT("\r\nMiWi + BLE Sensor Network Demo - End Node\r\n");
        #endif
        #if defined(PROTOCOL_MIWI)
            SYS_CONSOLE_PRINT("\r\nStarting Node 1 of Simple Demo for MiWi(TM) Stack ...");  
        #endif 
        #if defined(PROTOCOL_MIWI_PRO)
            SYS_CONSOLE_PRINT("\r\nStarting Node 1 of Simple Demo for MiWi(TM) PRO Stack ...");  
        #endif 
    #endif
}        


void APP_Output_Channel(uint8_t channel, uint8_t Step)
{
    if( Step == 0 )
    {
        SYS_CONSOLE_PRINT("\r\nSet Channel ");
        SYS_CONSOLE_PRINT("%d",channel);
        SYS_CONSOLE_PRINT("\r\n");
    }
    else
    { 
        SYS_CONSOLE_PRINT("\r\nConnected Peer on Channel ");
        SYS_CONSOLE_PRINT("%d",channel);
        SYS_CONSOLE_PRINT("\r\n");
    }
}   

void APP_Output_Instruction(void)
{
    SYS_CONSOLE_PRINT("\r\nBUTTON1: Unicast\r\n");
}

bool justFlag = false;
void APP_Output_HandleMessage(void)
{
    uint8_t i;

    if( rxMessage.flags.bits.secEn )
    {
        SYS_CONSOLE_PRINT("Secured ");
    }

    if( rxMessage.flags.bits.broadcast )
    {
        SYS_CONSOLE_PRINT("BP RSSI: ");
    }
    else
    {
		justFlag = true;
        SYS_CONSOLE_PRINT("UP RSSI: ");
    }
    SYS_CONSOLE_PRINT("%02x", rxMessage.PacketRSSI);
    if( rxMessage.flags.bits.srcPrsnt )
    {
        SYS_CONSOLE_PRINT(" from ");
        if( rxMessage.flags.bits.altSrcAddr )
        {
            SYS_CONSOLE_PRINT( "%x", rxMessage.SourceAddress[1]);
            SYS_CONSOLE_PRINT( "%x", rxMessage.SourceAddress[0]);
        }
        else
        {    
            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
            {
                SYS_CONSOLE_PRINT("%02x", rxMessage.SourceAddress[MY_ADDRESS_LENGTH-1-i]);
            }    
        }
    }
    SYS_CONSOLE_PRINT(": ");
    
    for(i = 0; i < rxMessage.PayloadSize; i++)
    {
        SYS_CONSOLE_PRINT("%c",rxMessage.Payload[i]);
    }       
}


void APP_Output_ChannelError(uint8_t channel)
{
    #if defined (ENABLE_CONSOLE)
        SYS_CONSOLE_PRINT("\r\nSelection of channel ");
        SYS_CONSOLE_PRINT("%d", channel);
        SYS_CONSOLE_PRINT(" is not supported in current configuration.\r\n");
    #endif
}

void APP_Output_UnicastFail(void)
{
        SYS_CONSOLE_PRINT("\r\nUnicast Failed\r\n");
}    

#if defined(PROTOCOL_STAR)
    void Source_END_DEVICE_INFO(uint8_t *SrAddr)
    {

    }
    
void STAR_DEMO_OPTIONS_MESSAGE(DeviceRole_t NetworkRole)
{

    if (NetworkRole == PAN_COORD)
    {
        SYS_CONSOLE_PRINT("Role: PAN_COORD\r\n");
    }
    else
    {
        SYS_CONSOLE_PRINT("Role: END_DEVICE\r\n");
    }
   
}
#endif
    





