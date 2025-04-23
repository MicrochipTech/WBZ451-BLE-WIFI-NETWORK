/**
* \file  star_demo.c
*
* \brief Demo Application for MiWi Star Implementation
*
* Copyright (c) 2023 - 2024 Microchip Technology Inc. and its subsidiaries.
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

/************************ HEADERS ****************************************/
#include "config/default/definitions.h"
#if defined(USER_BUTTON_ENABLED)
#include "config/default/peripheral/eic/plib_eic.h"
#endif
#include "miwi_api.h"
#include "miwi_p2p_star.h"
#include "task.h"
#include "app_miwi_star.h"
#include "config/default/driver/IEEE_802154_PHY/pal/inc/pal.h"
#include "app.h"
#if defined(ENABLE_SLEEP_FEATURE)
#include "config/default/device_deep_sleep.h"
#endif
#if defined (ENABLE_CONSOLE)
//#include "sio2host.h"
#endif
#if defined(ENABLE_FREQUENCY_AGILITY)
uint8_t maxRSSI;
uint8_t channelCount = 0U;
static uint8_t minRSSI = 0xFFU;
static uint8_t gOptimalChannel = 0xFFU;
#endif


#if defined(PROTOCOL_STAR)
/************************ LOCAL VARIABLES ****************************************/
uint8_t i;
uint8_t TxSynCount = 0U;    // Maintain the Count on TX's Done
uint8_t TxSynCount2 = 0U; // Maintain the Count on TX's Done
uint8_t TxNum = 0U;         // Maintain the Count on TX's Done
uint8_t RxNum = 0U;         // Maintain the Count on RX's Done
/* check for selections made by USER */
bool chk_sel_status;
uint8_t NumOfActiveScanResponse;
bool update_ed;
uint8_t select_ed;
uint8_t msghandledemo = 0U;
extern uint8_t myChannel;
/* Connection Table Memory */
extern CONNECTION_ENTRY connectionTable[CONNECTION_SIZE];
bool display_connections;
uint16_t bcastAddr = 0xFFFF;
uint8_t retCode = 0;
uint8_t buttonCnt = 0U;
SYS_TIME_HANDLE eicCbTimerHandle;
SYS_Timer_t eicCbTimer;
//#define SCAN_CHANNEL_MAP 0x06108000 //Channels 15, 20, 25, 26
//#define SCAN_CHANNEL_MAP 0x04100000 //Channels 20, 26
#define SCAN_CHANNEL_MAP 0x02100000 //Channels 20, 25
/************************ FUNCTION DEFINITIONS ****************************************/
/*********************************************************************
* Function: static void dataConfcb(uint8_t handle, miwi_status_t status)
*
* Overview: Confirmation Callback for MiApp_SendData
*
* Parameters:  handle - message handle, miwi_status_t status of data send
****************************************************************************/
static void dataConfcb(uint8_t handle, miwi_status_t status, uint8_t* msgPointer)
{
    if (SUCCESS == status)
    {
        SYS_CONSOLE_PRINT("MiWi Packet Sent\n\r");
    }
    else
    {
        SYS_CONSOLE_PRINT("Send Data EC:%x\n\r",status);
    }    

#if defined(ENABLE_SLEEP_FEATURE)
    if(role == END_DEVICE)
    {
    deviceCanSleep = true;
    APP_Msg_T    appMsg;
    APP_Msg_T *appState;
    appState = &appMsg; 
    appState->msgId = APP_STATE_PREPARE_TO_SLEEP;
    OSAL_QUEUE_Send(&appData.appQueue, appState, 0U);
    }
#endif
}
void eic_custom_cb(uintptr_t context)
{
    if(buttonCnt == 0U)
    {
        eicCbTimer.handler = eicCbTimerHandler;
        eicCbTimer.interval = 1000uL;
        eicCbTimer.timeout = 1000uL;
        eicCbTimer.mode = SYS_TIME_SINGLE;
        eicCbTimerHandle = SYS_TIME_CallbackRegisterMS(&eicCbTimerHandler, (uintptr_t)&dummyVal, eicCbTimer.interval, eicCbTimer.mode);
        if(eicCbTimerHandle == SYS_TIME_HANDLE_INVALID)
        {
            return;
        }
    }
    buttonCnt += 1U;
    if (buttonCnt >= 3U)
    {
        buttonCnt = 0U;
    }    
}
 
void eicCbTimerHandler(uintptr_t context)
{
    APP_Msg_T    appMsg;
    APP_Msg_T *appState;
    appState = &appMsg;
 
    appData.switchState = true;
    SYS_CONSOLE_PRINT("SW Press\n\r");
    if(buttonCnt == 2U) //short press
    {
        buttonCnt = 0U;
        if (appData.isMiWiConnected)
        {    
            appState->msgId = APP_STATE_UNICASTTOPC;
            OSAL_QUEUE_Send(&appData.appQueue, appState, 0U);
        }
        else
        {    
            appState->msgId = APP_STATE_SEARCH_NETWORK;
            OSAL_QUEUE_Send(&appData.appQueue, appState, 0U);        
        }    
    }
    else if(buttonCnt == 1U)//long press
    {
        buttonCnt = 3U;
        appState->msgId = APP_STATE_BROADCAST_FORWARDPACKET;
        OSAL_QUEUE_Send(&appData.appQueue, appState, 0U);
    }
    else
    {
        //Do nothing
    }

    (void)context;
}

/*******************************************************************************
  Function:
    void InitializeApp()

******************************************************************************/
void MiApp_Init(void)
{
    bool freezer_enable = false;
    // Demo Start Message
    APP_Output_Greeting();

    // Commission the network
    Initialize_Demo(freezer_enable);
    Rx_On(false);
    appInitialized = true;
#ifdef USER_BUTTON_ENABLED
    dummyVal = 0U;
#if defined(CHIMERA_SOC)
    EIC_CallbackRegister(EIC_PIN_0, eic_custom_cb, dummyVal);
#else
    EIC_CallbackRegister(EIC_PIN_2, eic_custom_cb, dummyVal);
#endif
#endif
}
#endif

/*********************************************************************
* Function: void ReceivedDataIndication (RECEIVED_MESSAGE *ind)
*
* Overview: Process a Received Message
*
* PreCondition: MiApp_ProtocolInit
*
* Input:  RECEIVED_MESSAGE *ind - Indication structure
********************************************************************/
void ReceivedDataIndication (RECEIVED_MESSAGE *ind)
{
    /* Print the received information via Console */
    APP_Output_HandleMessage();
    APP_Parse_Packet();
    //RGB_LED_GREEN_Toggle();

#if defined(ENABLE_SLEEP_FEATURE)
    if(role == END_DEVICE)
    {
        deviceCanSleep = true;
    APP_Msg_T    appMsg;
    APP_Msg_T *appState;
    appState = &appMsg; 
    appState->msgId = APP_STATE_PREPARE_TO_SLEEP;
    OSAL_QUEUE_Send(&appData.appQueue, appState, 0U);
    }
#endif
}
static void ActiveScan_cb (uint8_t foundScanResults, void* ScanResults)
{
    ACTIVE_SCAN_RESULT *curResults;
    APP_Msg_T   appMsg;
    APP_Msg_T   *p_appMsg;
    appMsg.msgId = APP_MSG_TX_SCAN_RESULTS;
    appData.scnMsgSize = 0;
    appData.networksFound = foundScanResults;
    SYS_CONSOLE_PRINT("Found %d PAN\n\r", foundScanResults);
    for(i = 0; i < foundScanResults; i++)
    {
        SYS_CONSOLE_PRINT("Scan Results %d: ", i);
        SYS_CONSOLE_PRINT("PANID: %x ", activeScanResults[i].PANID);
        SYS_CONSOLE_PRINT("Channel %d \n\r",activeScanResults[i].Channel);
        //appData.scnMsgSize = sprintf((char*)&appData.scnMsg[i], "%d> PANID: %x CH %d",i, activeScanResults[i].PANID,activeScanResults[i].Channel );
        appData.scnMsgSize = sprintf((char*)&appData.scnMsg[i], "P%d:%x:%d:",(i+1),activeScanResults[i].PANID,activeScanResults[i].Channel );
    }
    p_appMsg = &appMsg;
    OSAL_QUEUE_Send(&appData.appQueue, p_appMsg, 0);        
}
static void EstablishConn_cb(miwi_status_t status)
{
    APP_Msg_T   appMsg;
    APP_Msg_T   *p_appMsg;

    if ((SUCCESS == status) || (ALREADY_EXISTS == status))
    {
        MiApp_ConnectionMode(DISABLE_ALL_CONN);
        RGB_LED_GREEN_On();
        appData.greenLED = 1;
        appData.isMiWiConnected = true;
        APP_Output_Channel(myChannel, 1);

        APP_DumpConnectionInfo(0xFF);
        appMsg.msgId = APP_MSG_MIWI_CONNECTED;
        p_appMsg = &appMsg;
        OSAL_QUEUE_Send(&appData.appQueue, p_appMsg, 0);

    }
    else
    {
        SYS_CONSOLE_PRINT("MiWi Connection Failed ");
        SYS_CONSOLE_PRINT("EC: %d\n\r",status);
    }
}
void MiAPP_TaskHandler(APP_Msg_T *appState)
{
    switch( appState->msgId )
    {
        case APP_STATE_SEARCH_NETWORK:
        {
            SYS_CONSOLE_PRINT("Searching...\n\r");
            NumOfActiveScanResponse = MiApp_SearchConnection(9, SCAN_CHANNEL_MAP, ActiveScan_cb);
            //SYS_CONSOLE_PRINT("SearchConn ret: %x\n\r",NumOfActiveScanResponse);
            break;
        }
        case APP_STATE_CONNECT_NETWORK:
        {   
            SYS_CONSOLE_PRINT("Connecting to Network %d ", appData.networkChoiceIndex);
            SYS_CONSOLE_PRINT("CH: %d ", activeScanResults[appData.networkChoiceIndex].Channel);
            SYS_CONSOLE_PRINT("PAN: %x\n\r", activeScanResults[appData.networkChoiceIndex].PANID);
            retCode = MiApp_Set(PANID,(uint8_t*) &activeScanResults[appData.networkChoiceIndex].PANID);
            retCode = MiApp_EstablishConnection(activeScanResults[appData.networkChoiceIndex].Channel,2U,(uint8_t*)&bcastAddr,0,EstablishConn_cb);
            if (retCode != SUCCESS)
            {    
                SYS_CONSOLE_PRINT("EstCon Failed: %d\n\r", retCode);
            }    
            break;
        }    
        case APP_STATE_UNICASTTOPC:
        {
#ifdef ENABLE_SECURITY
                        mac_ack_status = MiApp_SendData(LONG_ADDR_LEN, connectionTable[0].Address,
                        MIWI_TEXT_LEN, (uint8_t*)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, true, dataConfcb);
#else
                appData.txPacketSize = sprintf(&appData.txPacketBuf[0], "%s,%3d,%3.1f,%d,%d,%d,%d\n\r", myDeviceName,
                    appData.relHumPercent, appData.tempCelsius, appData.redLED, appData.greenLED, appData.blueLED, appData.switchState);
                        
                        MiApp_SendData(LONG_ADDR_LEN,connectionTable[0].Address,
                                appData.txPacketSize,&appData.txPacketBuf[0],0,true,false,dataConfcb);
#endif										

        }
        break;

#if defined(ENABLE_FREQUENCY_AGILITY)
    case (uint8_t)APP_STATE_FREQUENCY_AGILITY:
    {
//        MiApp_InitChannelHopping(FULL_CHANNEL_MAP);
        if( appState->msgData[0] > maxRSSI )
        {
            maxRSSI = appState->msgData[0];
        }
        // if scan time exceed scan duration, prepare to scan the next channel
            if( maxRSSI < minRSSI )
            {
                minRSSI = maxRSSI;
                gOptimalChannel = channelCount-1;
            }    
        appStates = APP_STATE_NOISE_DETECTION;
            appState->msgId = (uint8_t)APP_STATE_NOISE_DETECTION;
            OSAL_QUEUE_Send(&appData.appQueue, appState, 0);
        break;
    }
    
    case (uint8_t)APP_STATE_NOISE_DETECTION:
    {
        uint8_t ScanDuration = 3;
        uint32_t channelMask = 0x00000001;
	    /* Check the given channel is within the range and callback is not NULL*/
	    if (!(MiMAC_GetPHYChannelInfo() & FULL_CHANNEL_MAP))
        {
            return;
        }
        if(channelCount < 32)
        {
            if(FULL_CHANNEL_MAP & MiMAC_GetPHYChannelInfo() & (channelMask << channelCount))
            {
                        maxRSSI = 0;
                /* choose appropriate channel */
                MiApp_Set(CHANNEL, &channelCount);
                PHY_EdStart(ScanDuration);
            }
            else
            {
                appStates = APP_STATE_NOISE_DETECTION;
                appState->msgId = (uint8_t)APP_STATE_NOISE_DETECTION;
                OSAL_QUEUE_Send(&appData.appQueue, appState, 0);
            }
            channelCount++;
        }
        else
        {
            MiApp_InitChannelHopping(FULL_CHANNEL_MAP, gOptimalChannel);
        }
       break;     
    }
#endif
        default:
        {
            //handle exceptions if any
        }
        break;
    }
}
void APP_Parse_Packet(void)
{
    switch (rxMessage.Payload[0])
    {
    case 'L':                       // LED Command
        if(rxMessage.Payload[1] == 'R')     // RED
        {
            if(rxMessage.Payload[2] == '1') // LED is on
                {
                SYS_CONSOLE_PRINT("Red LED On\n");	 
                RGB_LED_RED_On();
                appData.redLED = 1;
                }
            if(rxMessage.Payload[2] == '0') // LED is off
                {
                SYS_CONSOLE_PRINT("Red LED Off\n");	 
                RGB_LED_RED_Off();
                appData.redLED = 0;
                }              
        }
        if(rxMessage.Payload[1] == 'G')     // GREEN
        {
            if(rxMessage.Payload[2] == '1') // LED is on
                {
                SYS_CONSOLE_PRINT("Green LED On\n");	 
                RGB_LED_GREEN_On();
                appData.greenLED = 1;
                }
            if(rxMessage.Payload[2] == '0') // LED is off
                {
                SYS_CONSOLE_PRINT("Green LED Off\n");	 
                RGB_LED_GREEN_Off();
                appData.greenLED = 0;
                }              
        }
        if(rxMessage.Payload[1] == 'B')     // BLUE
        {
            if(rxMessage.Payload[2] == '1') // LED is on
                {
                SYS_CONSOLE_PRINT("Blue LED On\n");	 
                RGB_LED_BLUE_On();
                appData.blueLED = 1;
                }

            if(rxMessage.Payload[2] == '0') // LED is off
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

#if defined(ENABLE_FREQUENCY_AGILITY)
void performFreqAgility(void)
{
    APP_Msg_T    appMsg;
    APP_Msg_T    *p_appmsg;
    p_appmsg = &appMsg;
    channelCount = 0U;
    maxRSSI = 0U;
    MiApp_Get(CHANNEL, &backupChannel);
    appStates = APP_STATE_NOISE_DETECTION;
    p_appmsg->msgId = APP_STATE_NOISE_DETECTION;
    OSAL_QUEUE_Send(&appData.appQueue, p_appmsg, 0UL);
}
#endif