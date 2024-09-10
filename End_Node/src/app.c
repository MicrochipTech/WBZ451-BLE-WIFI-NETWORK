// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "app.h"
#include "definitions.h"
#include "app_ble.h"
#include "ble_trsps/ble_trsps.h"
#include "system/console/sys_console.h"
#include "miwi_app.h"
#include "app_hydroprobe.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;
uint16_t conn_hdl;
uint8_t noNetworkMsg[] = {"No Networks Found"};
uint8_t uart_data;
uint8_t myDeviceName[] = {"Board_41"};
uint8_t myDeviceNameSize = sizeof(myDeviceName);

// Global variables
bool SW2_State = 0;             // User switch status
uint8_t bleCount = 0;           // Counter for BLE LED
bool bleConnect = 0;            // Flag for BLE connection status
bool bleLED = 0;                // Flag for BLE LED status
uint8_t oneSec = 0;
uint8_t secondCnt = 0;

// ADC stuff for Temperature sensor
#define ADC_VREF  (2.971)           // VCC on Curiosity Board
#define ADC_MAX    4096             // 2^12 or 12-bits
#define MCP9700_V0C  (0.5)          // Offset of the MCP9700
#define MCP9700_TC   (0.01)         // 10mV per degree C output
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************
void APP_UartCBHandler()
{
    // Send the data from UART to connected device through Transparent service
    BLE_TRSPS_SendData(conn_hdl, 1, &uart_data);      
}
void uart_cb(SERCOM_USART_EVENT event, uintptr_t context)
{
  APP_Msg_T   appMsg;  
  // If RX data from UART reached threshold (previously set to 1)
  if( event == SERCOM_USART_EVENT_READ_THRESHOLD_REACHED )
  {
    // Read 1 byte data from UART
    SERCOM0_USART_Read(&uart_data, 1);
    appMsg.msgId = APP_MSG_UART_CB;
    OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);     
  }
}
void APP_TC3_Handler()
{
    APP_Msg_T    appMsg;
    APP_Msg_T   *p_appMsg;
    p_appMsg = &appMsg;    

    if (SWITCH_Get() == 1 && appData.switchState == 1)
    {
        p_appMsg->msgData[0] = 2;               // Number of bytes to send
        p_appMsg->msgData[1] = 'S';             // ASCII "S"
        p_appMsg->msgData[2] = '0';             // ASCII "0"                                   
        p_appMsg->msgId = APP_MSG_BLE_TX;
        OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);          
        appData.switchState = 0;
        SYS_CONSOLE_PRINT("SW Released\n\r");     
    }
    else if(SWITCH_Get() == 0 && appData.switchState == 1)
    {
        p_appMsg->msgData[0] = 2;               // Number of bytes to send
        p_appMsg->msgData[1] = 'S';             // ASCII "S"
        p_appMsg->msgData[2] = '1';             // ASCII "1"           
        p_appMsg->msgId = APP_MSG_BLE_TX;               // Assign the proper state
        OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);        
        SYS_CONSOLE_PRINT("SW On\n\r");        
    }    
    if (oneSec == 5)
    {
        if (conn_hdl == 0)
        {
            USER_LED_On();
        }    
        //RGB_LED_RED_On(); 
        APP_HydroProbe_Read (appData.i2cReadVal, 2);
        appData.tempCelsius = MCP9700_Temp_Celsius(); 
        oneSec = 0;
        if (secondCnt == 3) // Every 4 seconds send a MiWi Packet
        {
            SYS_CONSOLE_PRINT("M:%d T:%3.1f \n\r",appData.relHumPercent,appData.tempCelsius);
            if (appData.isMiWiConnected == true)
            {    
                APP_Msg_T    appMsgn;
                APP_Msg_T *appState;
                appState = &appMsgn;
                appState->msgId = APP_STATE_UNICASTTOPC;
                OSAL_QUEUE_Send(&appData.appQueue, appState, 0U);         
            }
            secondCnt = 0;
        } 
        else
        {    
            secondCnt++;
        }    
    }
    else
    {
        if (conn_hdl == 0)
        {
            USER_LED_Off();
        }  
        //RGB_LED_RED_Off();
    } 
    if (oneSec == 4)
    {
        if (conn_hdl != 0)
        {
            p_appMsg->msgData[0] = sprintf(&p_appMsg->msgData[1],"M%3d",appData.relHumPercent);                                           
            p_appMsg->msgId = APP_MSG_BLE_TX;
            OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
        }  
    }     
    if (oneSec == 3)
    {
        if (conn_hdl !=0)
        {
            p_appMsg->msgData[0] = sprintf(&p_appMsg->msgData[1],"T%3.1f",appData.tempCelsius);                                            
            p_appMsg->msgId = APP_MSG_BLE_TX;
            OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
        }  
    }     

    oneSec++;

}
void APP_I2C_Handler()
{
    APP_HydroProbe_GetValues();
}


/* TODO:  Add any necessary local functions.
*/
bool appInitialized = false;

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    
    appData.networksFound = 0;
    appData.isMiWiConnected = false;
    appData.validNetworkChoice = false;  
    
    conn_hdl = 0;

    oneSec = 0;
    appData.appQueue = xQueueCreate( 64, sizeof(APP_Msg_T) );
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    APP_Msg_T    appMsg;
    APP_Msg_T    *p_appMsg;
    p_appMsg = &appMsg;

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
           
            SERCOM0_USART_ReadNotificationEnable(true, true);
            // Set UART RX notification threshold to be 1
            SERCOM0_USART_ReadThresholdSet(1);
            // Register the UART RX callback function
            SERCOM0_USART_ReadCallbackRegister(uart_cb, (uintptr_t)NULL);
            TC3_TimerCallbackRegister(APP_TC3_Handler,(uintptr_t)NULL);
            SERCOM2_I2C_CallbackRegister(APP_I2C_Handler,(uintptr_t)NULL);
            TC3_TimerStart();
            APP_BleStackInit();
            
            MiWi_ApplicationInit();
            
            // Start Advertisement
            BLE_GAP_SetAdvEnable(0x01, 0x00);
            
            SYS_CONSOLE_PRINT("N:%s\n\r", myDeviceName);
            
            if (appInitialized)
            {
                appData.state = APP_STATE_SERVICE_TASKS;   
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            if (OSAL_QUEUE_Receive(&appData.appQueue, &appMsg, OSAL_WAIT_FOREVER))
            {
                if(p_appMsg->msgId==APP_MSG_BLE_STACK_EVT)
                {
                    // Pass BLE Stack Event Message to User Application for handling
                    APP_BleStackEvtHandler((STACK_Event_T *)p_appMsg->msgData);
                }
                else if(p_appMsg->msgId==APP_MSG_BLE_STACK_LOG)
                {
                    // Pass BLE LOG Event Message to User Application for handling
                    APP_BleStackLogHandler((BT_SYS_LogEvent_T *)p_appMsg->msgData);
                }
                else if(p_appMsg->msgId==APP_MSG_BLE_DISCONNECT)  
                {
                    SYS_CONSOLE_PRINT("BLE Disconnected\n\r");
                    conn_hdl = 0;
                    USER_LED_Off();
                    BLE_GAP_SetAdvEnable(0x01, 0x00);
                    SYS_CONSOLE_PRINT("BLE Advertising\r\n");
                } 
                else if(p_appMsg->msgId==APP_MSG_BLE_CONNECTED)  
                {
                    SYS_CONSOLE_PRINT("BLE Connected\n\r");
                    USER_LED_On();
                }                 
                else if(p_appMsg->msgId==APP_MSG_TX_SCAN_RESULTS)                    
                {
                    //SYS_CONSOLE_PRINT("\n\rTX Scan");
                    //Only transmit if we a valid handle for the BLE connection
                    if (conn_hdl != 0 )                        
                    {   
                        uint8_t res = 0;
                        uint8_t i;
                        if (appData.networksFound == 0)
                        {
                            SYS_CONSOLE_PRINT("\n\rNo Networks Found");
                            BLE_TRSPS_SendData(conn_hdl,sizeof(noNetworkMsg),noNetworkMsg );
                        }
                        else
                        {    
                            for (i = 0; i < appData.networksFound; i++)
                            {    
                                res = BLE_TRSPS_SendData(conn_hdl, appData.scnMsgSize, appData.scnMsg[i]);
                                if (res != 0)
                                {    
                                    SYS_CONSOLE_PRINT("\n\r BLE Send Data fail: res = %d",res);
                                }
                            }
                        }    
                    }            
                }
                else if(p_appMsg->msgId==APP_MSG_BLE_TX)  
                {                            
                    BLE_TRSPS_SendData(conn_hdl, p_appMsg->msgData[0], &p_appMsg->msgData[1]);
                }                  
                else if(p_appMsg->msgId==APP_MSG_BLE_DATA_RDY)  
                {
                    switch(p_appMsg->msgData[1])
                    {        
                        //SYS_CONSOLE_PRINT("\n\rLen: %d",p_appMsg->msgData[0]);
                        case 'N': 
                        
                            if ((p_appMsg->msgData[2] == '0') && (appData.networksFound > 0))
                            {
                                SYS_CONSOLE_PRINT("\n\rNetwork 0 chosen\n\r");
                                appData.networkChoiceIndex = 0;
                                appData.validNetworkChoice = true;
                            }
                            else if ((p_appMsg->msgData[2]) == '1' && (appData.networksFound > 1))
                            {
                                SYS_CONSOLE_PRINT("\n\rNetwork 1 chosen\n\r");
                                appData.networkChoiceIndex = 1; 
                                appData.validNetworkChoice = true;
                            }
                            else if ((p_appMsg->msgData[2]) == '2' && (appData.networksFound > 2))
                            {
                                SYS_CONSOLE_PRINT("\n\rNetwork 2 chosen\n\r");
                                appData.networkChoiceIndex = 2;
                                appData.validNetworkChoice = true;
                            } 
                            else if ((p_appMsg->msgData[2]) == '3' && (appData.networksFound > 3))
                            {
                                appData.networkChoiceIndex = 3;
                                appData.validNetworkChoice = true;
                            } 
                            else if ((p_appMsg->msgData[2]) == 'F')
                            {
                                    APP_Msg_T    appMsg;
                                    APP_Msg_T *appState;
                                    appState = &appMsg;
                                    appState->msgId = APP_STATE_SEARCH_NETWORK;
                                    OSAL_QUEUE_Send(&appData.appQueue, appState, 0U); 
                            }                            
                            if (appData.validNetworkChoice)
                            {
                                    APP_Msg_T    appMsg;
                                    APP_Msg_T *appState;
                                    appState = &appMsg;
                                    appState->msgId = APP_STATE_CONNECT_NETWORK;
                                    OSAL_QUEUE_Send(&appData.appQueue, appState, 0U);   
                            }
                            break;
                        case 'L':                       // LED Command
                            if(p_appMsg->msgData[2] == 'R')     // RED
                            {
                                if(p_appMsg->msgData[3] == '1') // LED is on
                                    {
                                    SYS_CONSOLE_PRINT("Red LED On\n");	 
                                    RGB_LED_RED_On();
                                    appData.redLED = 1;
                                    }
                                if(p_appMsg->msgData[3] == '0') // LED is off
                                    {
                                    SYS_CONSOLE_PRINT("Red LED Off\n");	 
                                    RGB_LED_RED_Off();
                                    appData.redLED = 0;
                                    }              
                            }
                            if(p_appMsg->msgData[2] == 'G')     // GREEN
                            {
                                if(p_appMsg->msgData[3] == '1') // LED is on
                                    {
                                    SYS_CONSOLE_PRINT("Green LED On\n");	 
                                    RGB_LED_GREEN_On();
                                    appData.greenLED = 1;
                                    }
                                if(p_appMsg->msgData[3] == '0') // LED is off
                                    {
                                    SYS_CONSOLE_PRINT("Green LED Off\n");	 
                                    RGB_LED_GREEN_Off();
                                    appData.greenLED = 0;
                                    }              
                            }
                            if(p_appMsg->msgData[2] == 'B')     // BLUE
                            {
                                if(p_appMsg->msgData[3] == '1') // LED is on
                                    {
                                    SYS_CONSOLE_PRINT("Blue LED On\n");	 
                                    RGB_LED_BLUE_On();
                                    appData.blueLED = 1;
                                    }
                            
                                if(p_appMsg->msgData[3] == '0') // LED is off
                                    {
                                    SYS_CONSOLE_PRINT("Blue LED Off\n");	 
                                    RGB_LED_BLUE_Off();
                                    appData.blueLED = 0;
                                    }              
                            }
                            break;
                        default:
                            break;
                        }
                }

                else if(p_appMsg->msgId==APP_MSG_MIWI_CONNECTED)  
                {
                   appData.scnMsgSize = sprintf((char*)&appData.scnMsg[0],"Connected to ");
                   BLE_TRSPS_SendData(conn_hdl, appData.scnMsgSize, appData.scnMsg[0]);
                   appData.scnMsgSize = sprintf((char*)&appData.scnMsg[0],"P:%XCH:%d\n\r",activeScanResults[appData.networkChoiceIndex].PANID,activeScanResults[appData.networkChoiceIndex].Channel);
                   BLE_TRSPS_SendData(conn_hdl, appData.scnMsgSize, appData.scnMsg[0]);
                }                   
                else if(p_appMsg->msgId==APP_MSG_UART_CB)
                {
                    // Pass BLE UART Data transmission target BLE UART Device handling
                    APP_UartCBHandler();
                }
                else
                {    
                    //SYS_CONSOLE_PRINT("MW TaskH\n\r");
                    MiAPP_TaskHandler(p_appMsg);
                }    
            }
            break;
        
        /* TODO: implement your application state machine.*/

        }
        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}
float MCP9700_Temp_Celsius(void)
{
	uint16_t adc_read = 0;
    volatile float temperature = 0, vout;
    //
    ADCHS_GlobalEdgeConversionStart();              // Start ADC conversion
    while(!ADCHS_ChannelResultIsReady(ADCHS_CH2));  // Wait for result
    adc_read = ADCHS_ChannelResultGet(ADCHS_CH2);   // Get ADC result      
    //
    vout =  ((float)adc_read) / ((float)ADC_MAX);
    vout = vout * ADC_VREF;  // Convert to voltage with 3.25 V is reference
    // TA = (VOUT - V0°C )/TC  TC = 0.01, V0°C = 0.5v as per MCP9700/9700A datasheet
	temperature = (vout - MCP9700_V0C) / MCP9700_TC;
	return (temperature);
}


/*******************************************************************************
 End of File
 */
