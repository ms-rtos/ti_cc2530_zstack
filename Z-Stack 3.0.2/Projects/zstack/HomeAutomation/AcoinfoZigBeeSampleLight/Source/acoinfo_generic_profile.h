/**************************************************************************************************
  Filename:       acoinfo_generic_profile.h
  Revised:        $Date: 2021-05-07


  Description:    This file contains the acoinfo generic profile

**************************************************************************************************/

#ifndef ACOINFO_GENERIC_PROFILE_H
#define ACOINFO_GENERIC_PROFILE_H

/*
 * 端点号
 */
#define ACOINFO_ZB_ENDPOINT                 1	  

/*
 * Cluster ID
 */
#define ACOINFO_ZB_BASIC_CLUSTER            0x0000 // ZHA 相关基础信息
#define ACOINFO_ZB_DIO_CLUSTER              0x0006 // DIO 开关量
#define ACOINFO_ZB_AIO_CLUSTER              0x000C // AIO 模拟量
#define ACOINFO_ZB_MEM_CLUSTER              0x0500 // MEM 内存
#define ACOINFO_ZB_COMMON_CLUSTER           0x0300 // 通用属性

/*
 * Attribute ID
 */
#define ACOINFO_ZB_MODEL_ID_ATTR            0x0005 // model ID 

#define ACOINFO_ZB_MANUFACTURER_NAME_ATTR   0x0004 // manufacturer 名

#define ACOINFO_ZB_DIO_ATTR_BASE            0x2000 // DIO Attribute ID base
#define ACOINFO_ZB_DIO_ATTR_NR              32     // DIO 通道个数
#define ACOINFO_ZB_DIO_ATTR_END             (ACOINFO_ZB_DIO_ATTR_BASE + ACOINFO_ZB_DIO_ATTR_NR)

#define ACOINFO_ZB_AIO_ATTR_BASE            0x2000 // AIO Attribute ID base
#define ACOINFO_ZB_AIO_ATTR_NR              32     // AIO 通道个数
#define ACOINFO_ZB_AIO_ATTR_END             (ACOINFO_ZB_AIO_ATTR_BASE + ACOINFO_ZB_AIO_ATTR_NR)

#define ACOINFO_ZB_MEM_ATTR_BASE            0x2000 // MEM Attribute ID base
#define ACOINFO_ZB_MEM_ATTR_NR              32     // MEM 通道个数
#define ACOINFO_ZB_MEM_ATTR_END             (ACOINFO_ZB_MEM_ATTR_BASE + ACOINFO_ZB_MEM_ATTR_NR)

#define ACOINFO_ZB_COMMON_ATTR_BASE         0x2000 // Common Attribute ID base

/*
 * 通用属性 ID 
 */
#define ACOINFO_ZB_DIO_ACTIVE_ATTR          (ACOINFO_ZB_COMMON_ATTR_BASE + 0)//DIO 有效通道
#define ACOINFO_ZB_DIO_WRITE_ATTR           (ACOINFO_ZB_COMMON_ATTR_BASE + 1)//DIO 可写通道
#define ACOINFO_ZB_AIO_ACTIVE_ATTR          (ACOINFO_ZB_COMMON_ATTR_BASE + 2)//AIO 有效通道
#define ACOINFO_ZB_AIO_WRITE_ATTR           (ACOINFO_ZB_COMMON_ATTR_BASE + 3)//AIO 可写通道
#define ACOINFO_ZB_MEM_ACTIVE_ATTR          (ACOINFO_ZB_COMMON_ATTR_BASE + 4)//MEM 有效通道
#define ACOINFO_ZB_MEM_WRITE_ATTR           (ACOINFO_ZB_COMMON_ATTR_BASE + 5)//MEM 可写通道

#endif /* ACOINFO_GENERIC_PROFILE_H */
