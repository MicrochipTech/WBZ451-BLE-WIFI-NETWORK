static void APP_BleConfigBasic(void)
{
    int8_t                          connTxPower;
    int8_t                          advTxPower;
    BLE_GAP_AdvParams_T             advParam;
    uint8_t advData[]={0x02, 0x01, 0x06};
    uint8_t myAdvData[30];
    BLE_GAP_AdvDataParams_T         appAdvData;
    uint8_t scanRspData[]={0x11, 0x07, 0x55, 0xE4, 0x05, 0xD2, 0xAF, 0x9F, 0xA9, 0x8F, 0xE5, 0x4A, 0x7D, 0xFE, 0x43, 0x53, 0x53, 0x49};
    BLE_GAP_AdvDataParams_T         appScanRspData;
    
    //Code to add device name string to adv data
    uint8_t nameSize;
    uint8_t advDataSize;
    nameSize = myDeviceNameSize;
    advDataSize = sizeof(advData);
    //Set Length of Device Name Field (string length plus one byte for type)
    myAdvData[advDataSize] = nameSize + 1;
    //Set Type Field for Local Name
    myAdvData[advDataSize + 1] = 0x09;
    memcpy(myAdvData, advData, sizeof(advData));
    for (uint8_t i = 0; i < nameSize; i++)
    {
        myAdvData[(i + advDataSize + 2)] = myDeviceName[i];
    }    

    // Configure advertising parameters
    BLE_GAP_SetAdvTxPowerLevel(9,&advTxPower);      /* Advertising TX Power */
    
    (void)memset(&advParam, 0, sizeof(BLE_GAP_AdvParams_T));
    advParam.intervalMin = 32;     /* Advertising Interval Min */
    advParam.intervalMax = 32;     /* Advertising Interval Max */
    advParam.type = BLE_GAP_ADV_TYPE_ADV_IND;        /* Advertising Type */
    advParam.advChannelMap = BLE_GAP_ADV_CHANNEL_ALL;        /* Advertising Channel Map */
    advParam.filterPolicy = BLE_GAP_ADV_FILTER_DEFAULT;     /* Advertising Filter Policy */
    BLE_GAP_SetAdvParams(&advParam);

    // Configure advertising data
    appAdvData.advLen=sizeof(advData)+nameSize+2;  
    (void)memcpy(appAdvData.advData, myAdvData, appAdvData.advLen);     /* Advertising Data */
    BLE_GAP_SetAdvData(&appAdvData);

    //Configure advertising scan response data
    appScanRspData.advLen=sizeof(scanRspData);
    (void)memcpy(appScanRspData.advData, scanRspData, appScanRspData.advLen);     /* Scan Response Data */
    BLE_GAP_SetScanRspData(&appScanRspData);

    BLE_GAP_SetConnTxPowerLevel(15, &connTxPower);      /* Connection TX Power */
}
