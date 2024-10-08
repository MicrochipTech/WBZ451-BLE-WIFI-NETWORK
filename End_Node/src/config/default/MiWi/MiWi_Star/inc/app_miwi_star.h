/**
* \file  star_demo.h
*
* \brief Demo Application for MiWi Star Interface
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

#ifndef STAR_DEMO_H
#define	STAR_DEMO_H

#if defined(ENABLE_FREQUENCY_AGILITY)
extern uint8_t maxRSSI;
extern uint8_t channelCount;
#endif
/*********************************************************************
* Function: void ReceivedDataIndication (RECEIVED_MESSAGE *ind)
*
* Overview: Process a Received Message
*
* PreCondition: MiApp_ProtocolInit
*
* Input:  None
*
* Output: None
*
********************************************************************/
void ReceivedDataIndication (RECEIVED_MESSAGE *ind);


/*********************************************************************
* Function: void run_star_demo(void)
*
* Overview: Starts the star demo
*
* PreCondition: System Initialization and Miwi Protocol Init
*
* Input:  None
*
* Output: None
*
********************************************************************/
void run_star_demo(void);
void eic_custom_cb(uintptr_t context);
void eicCbTimerHandler(uintptr_t context);
void APP_Parse_Packet(void);
#if defined(ENABLE_FREQUENCY_AGILITY)
void performFreqAgility(void);
#endif
#endif	/* STAR_DEMO_H */

