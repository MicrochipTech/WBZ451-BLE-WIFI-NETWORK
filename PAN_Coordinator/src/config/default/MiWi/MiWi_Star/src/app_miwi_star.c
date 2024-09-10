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

uint8_t panLEDMsg[6][3] = {"LR1", "LR0", "LB1", "LB0", "LG1", "LG0"};
uint8_t msgCounter = 0;
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
        SYS_CONSOLE_PRINT("Msg Sent\n\r",handle);
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

/*******************************************************************************
  Function:
    void InitializeApp()

******************************************************************************/
void MiApp_Init(void)
{
    //bool freezer_enable = true;
    bool freezer_enable = false;
    // Demo Start Message
    APP_Output_Greeting();

    // User Selection to commission a network or use Freezer
    //freezer_enable = freezer_feature();

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

    RGB_LED_GREEN_Toggle();

    /* Display the Source End Device Info on reception msg, Do not display if it is
       a PAN CO or if the message received was a broadcast packet */
    if ((role == END_DEVICE) && !rxMessage.flags.bits.broadcast)
    {
        Source_END_DEVICE_INFO(rxMessage.Payload);
    }
    /* Display the Instructions message */
//    STAR_DEMO_OPTIONS_MESSAGE (role);
//#if defined(ENABLE_SLEEP_FEATURE)
//    if(role == END_DEVICE)
//    {
//        deviceCanSleep = true;
//    APP_Msg_T    appMsg;
//    APP_Msg_T *appState;
//    appState = &appMsg; 
//    appState->msgId = APP_STATE_PREPARE_TO_SLEEP;
//    OSAL_QUEUE_Send(&appData.appQueue, appState, 0U);
//    }
//#endif
}

void MiAPP_TaskHandler(APP_Msg_T *appState)
{
    //uint8_t long_addr[8] = {0x9E, 0x95, 0x6E, 0xFF, 0xFE, 0x40, 0x95, 0x08};
    uint8_t long_addr[8] = {0x08, 0x95, 0x40, 0xFE, 0xFF, 0x6E, 0x95, 0x9E};    
    switch( appState->msgId )
    {
        case APP_STATE_BROADCAST_UNICASTTOPC:
        {
                if (role == PAN_COORD)
                {
                    /*******************************************************************/
                    // Button 1 pressed. We need to send out the bitmap of word "MiWi".
                    /*******************************************************************/
                    uint16_t broadcastAddress = 0xFFFF;
                    bool mac_ack_status = false;
#ifdef ENABLE_SECURITY
                    /* Function MiApp_SendData is used to broadcast a message with address as 0xFFFF */
                    mac_ack_status = MiApp_SendData(SHORT_ADDR_LEN, (uint8_t *)&broadcastAddress,
                        MIWI_TEXT_LEN, (uint8_t *)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, true, dataConfcb);
#else
					/* Function MiApp_SendData is used to broadcast a message with address as 0xFFFF */
//                    mac_ack_status = MiApp_SendData(SHORT_ADDR_LEN, (uint8_t *)&broadcastAddress,
//                        MIWI_TEXT_LEN, (uint8_t *)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, false, dataConfcb);
                    mac_ack_status = MiApp_SendData(SHORT_ADDR_LEN, (uint8_t *)&broadcastAddress,
                        LED_MSG_LEN , (uint8_t *)&panLEDMsg[msgCounter++], msghandledemo++, true, false, dataConfcb);
                    
//                    mac_ack_status = MiApp_SendData(LONG_ADDR_LEN, (uint8_t *)&long_addr,
//                        LED_MSG_LEN , (uint8_t *)&panLEDMsg[msgCounter++], msghandledemo++, true, false, dataConfcb);                    
#endif

                    if (mac_ack_status)
                    {
                        if (msgCounter == 6)
                        {
                            msgCounter = 0;
                        }    
                        /* Update the bitmap count */
                        TxSynCount++;
                    }
                }
                else
                {
                    bool mac_ack_status = false;
                    if (myConnectionIndex_in_PanCo == select_ed)
                    {
                       /* IF on the demo , a END_Device displays its own Connection Detail
                        unicast data packet to just PAN COR , No forwarding */
#ifdef ENABLE_SECURITY
                        mac_ack_status = MiApp_SendData(LONG_ADDR_LEN, connectionTable[0].Address,
                        MIWI_TEXT_LEN, (uint8_t*)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, true, dataConfcb);
#else
					    mac_ack_status = MiApp_SendData(LONG_ADDR_LEN, connectionTable[0].Address,
						MIWI_TEXT_LEN, (uint8_t*)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, false, dataConfcb);
#endif										
                        if (mac_ack_status)
                        {
                            TxSynCount++;
                        }
                    }                
                }
        }
        break;
        
        case APP_STATE_BROADCAST_FORWARDPACKET:
        {
                if (role == PAN_COORD)
                {
                    /*******************************************************************/
                    // Button 1 pressed. We need to send out the bitmap of word "MiWi".
                    /*******************************************************************/
                    uint16_t broadcastAddress = 0xFFFF;
                    bool mac_ack_status = false;
#ifdef ENABLE_SECURITY
                    /* Function MiApp_SendData is used to broadcast a message with address as 0xFFFF */
                    mac_ack_status = MiApp_SendData(SHORT_ADDR_LEN, (uint8_t *)&broadcastAddress,
                        MIWI_TEXT_LEN, (uint8_t *)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, true, dataConfcb);
#else
					/* Function MiApp_SendData is used to broadcast a message with address as 0xFFFF */
                    mac_ack_status = MiApp_SendData(SHORT_ADDR_LEN, (uint8_t *)&broadcastAddress,
                        MIWI_TEXT_LEN, (uint8_t *)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, false, dataConfcb);
#endif

                    if (mac_ack_status)
                    {
                        /* Update the bitmap count */
                        TxSynCount++;
                    }
                }
            else
            {
                bool mac_ack_status = false;
                /* Data can be sent at a time from one END_DEVICE_TO_ANOTHER
                Edx --> Pan CO --> EDy
                To forward a Packet from one ED to another ED , address should be specified with length as 3
                and address as end dest device short address (3 bytes)*/
#ifdef ENABLE_SECURITY
                mac_ack_status = MiApp_SendData(3, END_DEVICES_Short_Address[select_ed].Address,
                MIWI_TEXT_LEN, (uint8_t*)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, true, dataConfcb);
#else
	    		mac_ack_status = MiApp_SendData(3, END_DEVICES_Short_Address[select_ed].Address,
		    	MIWI_TEXT_LEN, (uint8_t*)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, false, dataConfcb);
#endif
                if (mac_ack_status)
                {
                    TxSynCount++;
                }
            }
        }
        break;
        
        case APP_STATE_NAVIGATE_INDEX_CONNTABLE:
        {
                if (role == PAN_COORD)
                {
                    /*******************************************************************/
                    // Button 1 pressed. We need to send out the bitmap of word "MiWi".
                    /*******************************************************************/
                    uint16_t broadcastAddress = 0xFFFF;
                    bool mac_ack_status = false;
#ifdef ENABLE_SECURITY
                    /* Function MiApp_SendData is used to broadcast a message with address as 0xFFFF */
                    mac_ack_status = MiApp_SendData(SHORT_ADDR_LEN, (uint8_t *)&broadcastAddress,
                        MIWI_TEXT_LEN, (uint8_t *)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, true, dataConfcb);
#else
					/* Function MiApp_SendData is used to broadcast a message with address as 0xFFFF */
                    mac_ack_status = MiApp_SendData(SHORT_ADDR_LEN, (uint8_t *)&broadcastAddress,
                        MIWI_TEXT_LEN, (uint8_t *)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, false, dataConfcb);
#endif

                    if (mac_ack_status)
                    {
                        /* Update the bitmap count */
                        TxSynCount++;
                    }
                }
                else
                {
                    if (select_ed > end_nodes-1U)  /* Reset Peer Device Info */
                    {
                        /* If end of Peer Device Info reset the count */
                        select_ed = 0U;
                    }
                    else
                    {
                    /* New device Information */
                    select_ed = select_ed+1U;
                    }
                }
        }
        break;
        
        case APP_STATE_NAVIGATE_INDEX_CONNTABLE_SEND:
        {
            if (select_ed > end_nodes-1U)  /* Reset Peer Device Info */
            {
                /* If end of Peer Device Info reset the count */
                select_ed = 0U;
            }
            else
            {
            /* New device Information */
                select_ed = select_ed+1U;
            }
            
            if (role == PAN_COORD)
            {
                    /*******************************************************************/
                    // Button 1 pressed. We need to send out the bitmap of word "MiWi".
                    /*******************************************************************/
                    uint16_t broadcastAddress = 0xFFFFU;
                    bool mac_ack_status = false;
#ifdef ENABLE_SECURITY
                    /* Function MiApp_SendData is used to broadcast a message with address as 0xFFFF */
                    mac_ack_status = MiApp_SendData(SHORT_ADDR_LEN, (uint8_t *)&broadcastAddress,
                        MIWI_TEXT_LEN, (uint8_t *)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, true, dataConfcb);
#else
					/* Function MiApp_SendData is used to broadcast a message with address as 0xFFFF */
                    mac_ack_status = MiApp_SendData(SHORT_ADDR_LEN, (uint8_t *)&broadcastAddress,
                        MIWI_TEXT_LEN, (uint8_t *)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, false, dataConfcb);
#endif

                    if (mac_ack_status)
                    {
                        /* Update the bitmap count */
                        TxSynCount++;
                    }
            }
            else
            {
                bool mac_ack_status = false;
                /* Data can be sent at a time from one END_DEVICE_TO_ANOTHER
                Edx --> Pan CO --> EDy
                To forward a Packet from one ED to another ED , address should be specified with length as 3
                and address as end dest device short address (3 bytes)*/
#ifdef ENABLE_SECURITY
                mac_ack_status = MiApp_SendData(3, END_DEVICES_Short_Address[select_ed].Address,
                MIWI_TEXT_LEN, (uint8_t*)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, true, dataConfcb);
#else
	    		mac_ack_status = MiApp_SendData(3, END_DEVICES_Short_Address[select_ed].Address,
		    	MIWI_TEXT_LEN, (uint8_t*)&MiWi[(TxSynCount%6)][0], msghandledemo++, true, false, dataConfcb);
#endif
                if (mac_ack_status)
                {
                    TxSynCount++;
                }
            }            
        }
        break;
        
#if defined(ENABLE_SLEEP_FEATURE)
	    case (uint8_t)APP_STATE_PREPARE_TO_SLEEP:
	    {
//             SYS_CONSOLE_PRINT("\r\n sleep0\r\n");
            uint32_t sleepDuration;
        if ((role == END_DEVICE) && (Total_Connections() > 0U))
        {
            if(MiApp_ReadyToSleep(&sleepDuration))
//            if (APP_ReadyToSleep(&sleepDuration))
            {    
                
                /* Enter system sleep mode */
                DEVICE_EnterDeepSleep(true, sleepDuration);
            }
            else
            {
                APP_Msg_T sleepReq;
                appStates = APP_STATE_PREPARE_TO_SLEEP;
                sleepReq.msgId = (uint8_t)APP_STATE_PREPARE_TO_SLEEP;      
                OSAL_QUEUE_Send(&appData.appQueue, &sleepReq, 0); 
            } 
        }
        }
        	break;
#endif
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