/**************************************************************************************************
  Filename:       acoinfo_samplesw_data.c
  Revised:        $Date: 2014-07-30 12:57:37 -0700 (Wed, 30 Jul 2014) $
  Revision:       $Revision: 39591 $

  Description:    Acoinfo sample device application.

  Copyright 2006-2014 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "zcl_poll_control.h"
#include "zcl_electrical_measurement.h"
#include "zcl_diagnostic.h"
#include "zcl_meter_identification.h"
#include "zcl_appliance_identification.h"
#include "zcl_appliance_events_alerts.h"
#include "zcl_power_profile.h"
#include "zcl_appliance_control.h"
#include "zcl_appliance_statistics.h"
#include "zcl_hvac.h"
#include "hal_uart.h"

#include "acoinfo_sample.h"
#include "acoinfo_generic_profile.h"

/*********************************************************************
 * CONSTANTS
 */
#define SAMPLE_DEVICE_VERSION       1
#define SAMPLE_FLAGS                0

/*********************************************************************
 * MACROS
 */
#define ACOINFO_ZB_GENERIC_MODEL(str) \
       {\
          ACOINFO_ZB_BASIC_CLUSTER,\
          {\
            ACOINFO_ZB_MODEL_ID_ATTR,\
            ZCL_DATATYPE_CHAR_STR,\
            ACCESS_CONTROL_READ,\
            (void *)str\
          }\
        },

#define ACOINFO_ZB_MANUFACTURER_NAME(str) \
      {\
         ACOINFO_ZB_BASIC_CLUSTER,\
         {\
          ACOINFO_ZB_MANUFACTURER_NAME_ATTR,\
          ZCL_DATATYPE_CHAR_STR,\
          ACCESS_CONTROL_READ,\
          (void *)str\
         }\
      },

#define ACOINFO_ZB_DIO(n, flag) \
        {\
          ACOINFO_ZB_DIO_CLUSTER,\
          {  \
            ACOINFO_ZB_DIO_ATTR_BASE + n,\
            ZCL_DATATYPE_BOOLEAN,\
            flag,\
            NULL,\
          }\
        },

#define ACOINFO_ZB_AIO(n, flag) \
        {\
          ACOINFO_ZB_AIO_CLUSTER,\
          { \
            ACOINFO_ZB_AIO_ATTR_BASE + n,\
            ZCL_DATATYPE_SINGLE_PREC,\
            flag,\
            NULL,\
          }\
        },

#define ACOINFO_ZB_MEM(n, flag) \
        {\
          ACOINFO_ZB_MEM_CLUSTER,\
          {  \
            ACOINFO_ZB_MEM_ATTR_BASE + n,\
            ZCL_DATATYPE_UINT32,\
            flag,\
            NULL,\
          }\
        },

#define ACOINFO_ZB_COMMON(attr, var) \
        {\
          ACOINFO_ZB_COMMON_CLUSTER,\
          {\
            attr,\
            ZCL_DATATYPE_UINT32,\
            ACCESS_CONTROL_READ,\
            (void *)&var,\
          }\
        },

/*********************************************************************
 * LOCAL FUNCTIONS
 */
/*********************************************************************
 * LOCAL VARIABLES
 */
/*
 * Basic Cluster
 */
static const uint8 acoinfoSample_GenericModel[] = { 22, 'a','c','o','i','n','f','o','.','z','i','g','b','e','e','.','g','e','n','e','r','i','c'}; // model ID 
static const uint8 acoinfoSample_ManufacturerName[] = { 17, 'a','c','o','i','n','f','o','.','l','i','g','h','t','.','l','e','d'}; // manufacturer 名

/*
 * Acoinfo generic cluster common attribute
 */
static uint32 dioActiveMask   = 0x1; // DIO 有效通道掩码
static uint32 dioWritableMask = 0x1; // DIO 可写通道掩码

static uint32 aioActiveMask   = 0x1; // AIO 有效通道掩码
static uint32 aioWritableMask = 0x0; // AIO 可写通道掩码
/*********************************************************************
 * GLOBAL VARIABLES
 */
/*********************************************************************
 * ATTRIBUTE DEFINITIONS - Uses REAL cluster IDs
 */
/*
 * NOTE: The attributes listed in the AttrRec must be in ascending order
 * per cluster to allow right function of the Foundation discovery commands
 */
CONST zclAttrRec_t acoinfoSample_Attrs[] =
{
  /*
   * 添加 ACOINFO ZigBee 通用设备的 model ID (只读)
   */
  ACOINFO_ZB_GENERIC_MODEL( acoinfoSample_GenericModel )

  /*
   * 添加一个 manufacturer 名（只读）
   */
  ACOINFO_ZB_MANUFACTURER_NAME( acoinfoSample_ManufacturerName )

  /*
   * 添加一个开关量，即 LED 所使用的属性（可读写）
   */
  ACOINFO_ZB_DIO( ACOINFO_SAMPLE_LED_ATTR_CHAN_NUM,  ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE )
    
  /*
   * 添加一个模拟量，即 Vdd 所使用的属性（只读）
   */
  ACOINFO_ZB_AIO( ACOINFO_SAMPLE_VDD_ATTR_CHAN_NUM,  ACCESS_CONTROL_READ )

  /*
   * 添加开关量通用属性（只读）
   */
  ACOINFO_ZB_COMMON( ACOINFO_ZB_DIO_ACTIVE_ATTR, dioActiveMask )
  ACOINFO_ZB_COMMON( ACOINFO_ZB_DIO_WRITE_ATTR,  dioWritableMask )
      
  /*
   * 添加模拟量通用属性（只读）
   */
  ACOINFO_ZB_COMMON( ACOINFO_ZB_AIO_ACTIVE_ATTR, aioActiveMask )
  ACOINFO_ZB_COMMON( ACOINFO_ZB_AIO_WRITE_ATTR,  aioWritableMask )
};

uint8 CONST acoinfoSample_NumAttributes = ( sizeof(acoinfoSample_Attrs) / sizeof(acoinfoSample_Attrs[0]) );

/*********************************************************************
 * SIMPLE DESCRIPTOR
 */
/*
 * This is the Cluster ID List and should be filled with Application
 * specific cluster IDs.
 */
const cId_t acoinfoSample_InClusterList[] =
{
  ZCL_CLUSTER_ID_GEN_BASIC,
  ACOINFO_ZB_DIO_CLUSTER,
  ACOINFO_ZB_AIO_CLUSTER,
  ACOINFO_ZB_MEM_CLUSTER,
  ACOINFO_ZB_COMMON_CLUSTER,
};

#define ACOINFO_SAMPLE_MAX_INCLUSTERS    ( sizeof( acoinfoSample_InClusterList ) / sizeof( acoinfoSample_InClusterList[0] ))

SimpleDescriptionFormat_t zclSampleSw_SimpleDesc =
{
  ACOINFO_ZB_ENDPOINT,                  //  int Endpoint;
  ZCL_HA_PROFILE_ID,                    //  uint16 AppProfId[2];
  ZCL_HA_DEVICEID_ON_OFF_LIGHT_SWITCH,  //  uint16 AppDeviceId[2];
  SAMPLE_DEVICE_VERSION,                //  int   AppDevVer:4;
  SAMPLE_FLAGS,                         //  int   AppFlags:4;
  ACOINFO_SAMPLE_MAX_INCLUSTERS,        //  byte  AppNumInClusters;
  (cId_t *)acoinfoSample_InClusterList, //  byte *pAppInClusterList;
  0,                                    //  byte  AppNumInClusters;
  NULL                                  //  byte *pAppInClusterList;
};
