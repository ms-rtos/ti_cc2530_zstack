/**************************************************************************************************
  Filename:       acoinfo_generic_profile.h
  Revised:        $Date: 2021-05-07


  Description:    This file contains the acoinfo generic profile

**************************************************************************************************/

#ifndef ACOINFO_GENERIC_PROFILE_H
#define ACOINFO_GENERIC_PROFILE_H

/*
 * Endpoint number
 */
#define ACOINFO_ZB_ENDPOINT                 1

/*
 * Cluster ID
 */
#define ACOINFO_ZB_BASIC_CLUSTER            0x0000
#define ACOINFO_ZB_DIO_CLUSTER              0x0006
#define ACOINFO_ZB_AIO_CLUSTER              0x000C
#define ACOINFO_ZB_MEM_CLUSTER              0x0500
#define ACOINFO_ZB_COMMON_CLUSTER           0x0300

/*
 * Attribute ID
 */
#define ACOINFO_ZB_MODEL_ID_ATTR            0x0005

#define ACOINFO_ZB_MANUFACTURER_NAME_ATTR   0x0004

#define ACOINFO_ZB_DIO_ATTR_BASE            0x2000
#define ACOINFO_ZB_DIO_ATTR_NR              32
#define ACOINFO_ZB_DIO_ATTR_END             (ACOINFO_ZB_DIO_ATTR_BASE + ACOINFO_ZB_DIO_ATTR_NR)

#define ACOINFO_ZB_AIO_ATTR_BASE            0x2000
#define ACOINFO_ZB_AIO_ATTR_NR              32
#define ACOINFO_ZB_AIO_ATTR_END             (ACOINFO_ZB_AIO_ATTR_BASE + ACOINFO_ZB_AIO_ATTR_NR)

#define ACOINFO_ZB_MEM_ATTR_BASE            0x2000
#define ACOINFO_ZB_MEM_ATTR_NR              32
#define ACOINFO_ZB_MEM_ATTR_END             (ACOINFO_ZB_MEM_ATTR_BASE + ACOINFO_ZB_MEM_ATTR_NR)

#define ACOINFO_ZB_COMMON_ATTR_BASE         0x2000
#define ACOINFO_ZB_DIO_ACTIVE_ATTR          (ACOINFO_ZB_COMMON_ATTR_BASE + 0)
#define ACOINFO_ZB_DIO_WRITE_ATTR           (ACOINFO_ZB_COMMON_ATTR_BASE + 1)
#define ACOINFO_ZB_AIO_ACTIVE_ATTR          (ACOINFO_ZB_COMMON_ATTR_BASE + 2)
#define ACOINFO_ZB_AIO_WRITE_ATTR           (ACOINFO_ZB_COMMON_ATTR_BASE + 3)
#define ACOINFO_ZB_MEM_ACTIVE_ATTR          (ACOINFO_ZB_COMMON_ATTR_BASE + 4)
#define ACOINFO_ZB_MEM_WRITE_ATTR           (ACOINFO_ZB_COMMON_ATTR_BASE + 5)

#endif /* ACOINFO_GENERIC_PROFILE_H */
