/**************************************************************************************************
  Filename:       acoinfo_samplesw.c
  Revised:        $Date: 2015-08-19 17:11:00 -0700 (Wed, 19 Aug 2015) $
  Revision:       $Revision: 44460 $

  Description:    Acoinfo sample switch application.

  Copyright 2006-2013 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED 鎻係 IS锟� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include "MT_SYS.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "acoinfo_sample.h"
#include "zcl_diagnostic.h"

#include "onboard.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "hal_adc.h"

#include "bdb.h"
#include "bdb_interface.h"

#include "acoinfo_generic_profile.h"
#include <stdio.h>
#include <stdarg.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
// Events for the sample app
#define SAMPLEAPP_END_DEVICE_REJOIN_EVT     0x0001

#define SAMPLEAPP_END_DEVICE_REJOIN_DELAY   1000

// Uart Tx buffer length
#define ZCLSAMPLESW_UART_BUF_LEN            128

static char *acoinfoSample_NwkState[] = {
  "Initialized - not started automatically",
  "Initialized - not connected to anything",
  "Discovering PAN's to join",
  "Joining a PAN",
  "ReJoining a PAN in secure mode scanning in current channel, only for end devices",
  "Joined but not yet authenticated by trust center",
  "Started as device after authentication",
  "Device joined, authenticated and is a router",
  "Started as Zigbee Coordinator",
  "Started as Zigbee Coordinator",
  "Device has lost information about its parent..",
  "Device is sending KeepAlive message to its parent",
  "Device is waiting before trying to rejoin",
  "ReJoining a PAN in secure mode scanning in all channels, only for end devices",
  "ReJoining a PAN in Trust center mode scanning in current channel, only for end devices",
  "ReJoining a PAN in Trust center mode scanning in all channels, only for end devices"
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static byte acoinfo_Sample_TaskID;

#if HAL_UART == TRUE
static char acoinfoSample_UartBuf[ZCLSAMPLESW_UART_BUF_LEN];
#endif

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void acoinfoSample_HandleKeys( byte shift, byte keys );
static void acoinfoSample_HandleStateChange( devStates_t  status );
static void acoinfoSample_ProcessCommissioningStatus( bdbCommissioningModeMsg_t *bdbCommissioningModeMsg );
static ZStatus_t acoinfoSample_ReadWriteCallBack( uint16 clusterId, uint16 attrId, uint8 oper,uint8 *pValue, uint16 *pLen );
static ZStatus_t acoinfoSample_HandleMemData( uint16 attrId, uint8 oper, uint8 *pValue, uint16 *pLen );
static ZStatus_t acoinfoSample_HandleDioData( uint16 attrId, uint8 oper, uint8 *pValue, uint16 *pLen );
static ZStatus_t acoinfoSample_HandleAioData( uint16 attrId, uint8 oper, uint8 *pValue, uint16 *pLen );

#if HAL_UART == TRUE
static void acoinfoSample_InitUart( void );
#endif

/*********************************************************************
 * REFERENCED EXTERNALS
 */
extern int16 zdpExternalStateTaskID;

extern void ZDAppCheckForHoldKey( void );

/*********************************************************************
 * @fn          acoinfoSample_Init
 *
 * @brief       Initialization function for the zclGeneral layer.
 *
 * @param       none
 *
 * @return      none
 */
void acoinfoSample_Init( byte task_id )
{
  acoinfo_Sample_TaskID = task_id;

#if HAL_UART == TRUE
  acoinfoSample_InitUart();
#endif

  /*
   *  Register the Simple Descriptor for this application
   */
  bdb_RegisterSimpleDescriptor( &zclSampleSw_SimpleDesc );

  /*
   *  Register the application's attribute list
   */
  zcl_registerAttrList( ACOINFO_ZB_ENDPOINT, acoinfoSample_NumAttributes, acoinfoSample_Attrs );

  /*
   * Register the application's callback function to read/write attribute data
   */
  zcl_registerReadWriteCB( ACOINFO_ZB_ENDPOINT, acoinfoSample_ReadWriteCallBack, NULL );

  /*
   *  Register for all key events - This app will handle all key events
   */
  RegisterForKeys( acoinfo_Sample_TaskID );
  
  bdb_RegisterCommissioningStatusCB( acoinfoSample_ProcessCommissioningStatus );

  zdpExternalStateTaskID = acoinfo_Sample_TaskID;

  /*
   *  Start joining the network
   */
  bdb_StartCommissioning( BDB_COMMISSIONING_MODE_NWK_STEERING |
                          BDB_COMMISSIONING_MODE_FINDING_BINDING );
}

/*********************************************************************
 * @fn          zclSample_event_loop
 *
 * @brief       Event Loop Processor for zclGeneral.
 *
 * @param       none
 *
 * @return      none
 */
uint16 acoinfoSample_event_loop( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;
  
  if ( events & SYS_EVENT_MSG )
  {
    while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( acoinfo_Sample_TaskID )) )
    {
      switch ( MSGpkt->hdr.event )
      {
        case KEY_CHANGE:
          acoinfoSample_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        case ZDO_STATE_CHANGE:
          acoinfoSample_HandleStateChange( (devStates_t)MSGpkt->hdr.status );
          break;

        default:
          break;
      }

      /*
       *  Release the memory
       */
      osal_msg_deallocate( (uint8 *)MSGpkt );
    }

    /*
     *  return unprocessed events
     */
    return ( events ^ SYS_EVENT_MSG );
  }

#if ZG_BUILD_ENDDEVICE_TYPE    
  if ( events & SAMPLEAPP_END_DEVICE_REJOIN_EVT )
  {
    debugMessage( "attempt rejoin\n" );
    bdb_ZedAttemptRecoverNwk();
    return ( events ^ SAMPLEAPP_END_DEVICE_REJOIN_EVT );
  }
#endif

  /*
   *  Discard unknown events
   */
  return 0;
}

/*********************************************************************
 * @fn      debugMessage
 *
 * @brief   Debug message
 *
 * @param   fmt
 *
 * @return  none
 */
void debugMessage( const char *fmt, ... )
{
#if HAL_UART == TRUE
  int     i;
  va_list ap;

  va_start( ap, fmt );
  i = vsprintf( acoinfoSample_UartBuf, fmt, ap );
  va_end( ap );
  HalUARTWrite( HAL_UART_PORT_0,(uint8 *)acoinfoSample_UartBuf, i );
#endif
}

#if HAL_UART == TRUE
/*********************************************************************
 * @fn      zclSampleSw_InitUart
 *
 * @brief   Initialization function for serial port
 *
 * @param   none
 *
 * @return  none
 */
static void acoinfoSample_InitUart( void )
{
  halUARTCfg_t uartConfig;

  /*
   *  UART Configuration
   */
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = HAL_UART_BR_115200;
  uartConfig.flowControl          = FALSE;
  uartConfig.flowControlThreshold = 0;
  uartConfig.rx.maxBufSize        = ZCLSAMPLESW_UART_BUF_LEN;
  uartConfig.tx.maxBufSize        = 0;
  uartConfig.idleTimeout          = 6;
  uartConfig.intEnable            = TRUE;
  uartConfig.callBackFunc         = NULL;

  /*
   *  Start UART
   */
  HalUARTOpen( HAL_UART_PORT_0, &uartConfig );
}
#endif

/*********************************************************************
 * @fn      zclSampleSw_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_5
 *                 HAL_KEY_SW_4
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void acoinfoSample_HandleKeys( byte shift, byte keys )
{
  if ( keys & HAL_KEY_SW_6 )
  {
    bdb_resetLocalAction();
    ZDAppCheckForHoldKey();
    debugMessage( "booting to skip past NV Restore\n" );
    bdb_StartCommissioning( BDB_COMMISSIONING_MODE_NWK_STEERING |
                            BDB_COMMISSIONING_MODE_FINDING_BINDING );
  }
}

/*********************************************************************
 * @fn      zclSampleSw_HandleStateChange
 *
 * @brief   Handles state change events for this device .
 *
 * @param   status - status for device:
 *
 * @return  none
 */
static void acoinfoSample_HandleStateChange( devStates_t status )
{
  debugMessage( "ZDO state: %s\n", acoinfoSample_NwkState[status] );

  switch(status)
  {
    case DEV_HOLD:
    case DEV_INIT:
    case DEV_NWK_ORPHAN:
      HalLedSet( HAL_LED_3, HAL_LED_MODE_OFF );
      break;

    case DEV_NWK_DISC:
      HalLedBlink( HAL_LED_3, 10, 50, 400 );
      break;

    case DEV_END_DEVICE:
      HalLedSet( HAL_LED_3, HAL_LED_MODE_ON );
      break;
  }
}

/*********************************************************************
 * @fn      zclSampleSw_ProcessCommissioningStatus
 *
 * @brief   Callback in which the status of the commissioning process are reported
 *
 * @param   bdbCommissioningModeMsg - Context message of the status of a commissioning process
 *
 * @return  none
 */
static void acoinfoSample_ProcessCommissioningStatus( bdbCommissioningModeMsg_t *bdbCommissioningModeMsg )
{
  switch( bdbCommissioningModeMsg->bdbCommissioningMode )
  {
#if ZG_BUILD_ENDDEVICE_TYPE    
    case BDB_COMMISSIONING_PARENT_LOST:
      if( bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_NETWORK_RESTORED )
      {
        /*
         *  We did recover from losing parent
         */
        debugMessage( "recover from losing parent\n" );
      }
      else
      {
        /*
         *  Parent not found, attempt to rejoin again after a fixed delay
         */
        osal_start_timerEx( acoinfo_Sample_TaskID, SAMPLEAPP_END_DEVICE_REJOIN_EVT, SAMPLEAPP_END_DEVICE_REJOIN_DELAY );
      }
      break;
#endif 
  }
}

/*********************************************************************
 * @fn      acoinfoSample_ReadWriteCallBack
 *
 * @brief   Callback in which the data of the reading/writing attribute
 *
 * @param   clusterId - cluster ID
 * @param   attrId    - attribute ID
 * @param   oper      - read/write/get length
 * @param   pValue    - attribute data
 * @param   pLen      - attribute data length
 *
 * @return  Successful if data was read
 */
static ZStatus_t acoinfoSample_ReadWriteCallBack( uint16 clusterId, uint16 attrId, uint8 oper, uint8 *pValue, uint16 *pLen )
{
  ZStatus_t ret = ZCL_STATUS_FAILURE;

  switch( clusterId )
  {
    case ACOINFO_ZB_DIO_CLUSTER:
      ret = acoinfoSample_HandleDioData( attrId, oper, pValue, pLen );
      break;

    case ACOINFO_ZB_AIO_CLUSTER:
      ret = acoinfoSample_HandleAioData( attrId, oper, pValue, pLen );
      break;

    case ACOINFO_ZB_MEM_CLUSTER:
      ret = acoinfoSample_HandleMemData( attrId, oper, pValue, pLen );
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn      acoinfoSample_HandleDioData
 *
 * @brief   Handle data for DIO attribute
 *
 * @param   attrId - attribute ID
 * @param   oper   - read/write/get length operation
 * @param   pValue - attribute data
 * @param   pLen   - attribute data length
 *
 * @return  Successful if data was read
 */
static ZStatus_t acoinfoSample_HandleDioData( uint16 attrId, uint8 oper, uint8 *pValue, uint16 *pLen )
{
  ZStatus_t ret = ZCL_STATUS_SUCCESS;
  uint8     data = attrId - ACOINFO_ZB_MEM_ATTR_BASE;

  switch( oper )
  {
    case ZCL_OPER_READ:
      *(uint8 *)pValue = data;
      *pLen = sizeof( data );
      debugMessage( "dio[%d] read :%d\n", attrId - ACOINFO_ZB_MEM_ATTR_BASE, data );
      break;

    case ZCL_OPER_WRITE:
      data = *(uint8 *)pValue;
      debugMessage( "dio[%d] write :%d\n", attrId - ACOINFO_ZB_MEM_ATTR_BASE, data );
      break;

    case ZCL_OPER_LEN:
      *pLen = sizeof( data );
      debugMessage( "dio[%d] get length :%d\n", attrId - ACOINFO_ZB_MEM_ATTR_BASE, *pLen );
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn      acoinfoSample_HandleAioData
 *
 * @brief   Handle data for AIO attribute
 *
 * @param   attrId - attribute ID
 * @param   oper   - read/write/get length operation
 * @param   pValue - attribute data
 * @param   pLen   - attribute data length
 *
 * @return  Successful if data was read
 */
static ZStatus_t acoinfoSample_HandleAioData( uint16 attrId, uint8 oper, uint8 *pValue, uint16 *pLen )
{
  ZStatus_t ret = ZCL_STATUS_SUCCESS;
  float     data = attrId - ACOINFO_ZB_MEM_ATTR_BASE;

  switch( oper )
  {
    case ZCL_OPER_READ:
      *(float *)pValue = data;
      *pLen = sizeof( data );
      debugMessage( "aio[%d] read :%d\n", attrId - ACOINFO_ZB_MEM_ATTR_BASE, data );
      break;

    case ZCL_OPER_WRITE:
      data = *(float *)pValue;
      debugMessage( "aio[%d] write :%d\n", attrId - ACOINFO_ZB_MEM_ATTR_BASE, data );
      break;

    case ZCL_OPER_LEN:
      *pLen = sizeof( data );
      debugMessage( "aio[%d] get length :%d\n", attrId - ACOINFO_ZB_MEM_ATTR_BASE, *pLen );
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn      acoinfoSample_HandleMemData
 *
 * @brief   Handle data for MEM attribute
 *
 * @param   attrId - attribute ID
 * @param   oper   - read/write/get length operation
 * @param   pValue - attribute data
 * @param   pLen   - attribute data length
 *
 * @return  Successful if data was read
 */
static ZStatus_t acoinfoSample_HandleMemData( uint16 attrId, uint8 oper, uint8 *pValue, uint16 *pLen )
{
  ZStatus_t ret = ZCL_STATUS_SUCCESS;
  uint32    data = attrId - ACOINFO_ZB_MEM_ATTR_BASE;

  switch( oper )
  {
    case ZCL_OPER_READ:
      *(uint32 *)pValue = data;
      *pLen = sizeof( data );
      debugMessage( "mem[%d] read :%d\n", attrId - ACOINFO_ZB_MEM_ATTR_BASE, data );
      break;

    case ZCL_OPER_WRITE:
      data = *(uint32 *)pValue;
      debugMessage( "mem[%d] write :%d\n", attrId - ACOINFO_ZB_MEM_ATTR_BASE, data );
      break;

    case ZCL_OPER_LEN:
      *pLen = sizeof(data);
      debugMessage( "mem[%d] get length :%d\n", attrId - ACOINFO_ZB_MEM_ATTR_BASE, *pLen );
      break;
  }

  return ( ret );
}
/****************************************************************************
****************************************************************************/
