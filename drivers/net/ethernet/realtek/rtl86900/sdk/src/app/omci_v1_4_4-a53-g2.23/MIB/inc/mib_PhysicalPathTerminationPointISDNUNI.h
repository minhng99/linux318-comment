/* 
 * Copyright (C) 2014 Realtek Semiconductor Corp.
 * All Rights Reserved.
 * 
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 * 
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 * 
 */


#ifndef __MIB_PHYSICALPATHTERMINATIONPOINTISDNUNI_TABLE_H__
#define __MIB_PHYSICALPATHTERMINATIONPOINTISDNUNI_TABLE_H__

/* Table PhysicalPathTerminationPointISDNUNI attribute for STRING type define each entry length */

/* Table PhysicalPathTerminationPointISDNUNI attribute index */
#define MIB_TABLE_PHYSICALPATHTERMINATIONPOINTISDNUNI_ATTR_NUM (9)
#define MIB_TABLE_PHYSICALPATHTERMINATIONPOINTISDNUNI_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_PHYSICALPATHTERMINATIONPOINTISDNUNI_ADMINISTRATIVESTATE_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_PHYSICALPATHTERMINATIONPOINTISDNUNI_INTERWORKINGPOINTER_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_PHYSICALPATHTERMINATIONPOINTISDNUNI_DCHANNELID_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_PHYSICALPATHTERMINATIONPOINTISDNUNI_B1CHANNELID_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_PHYSICALPATHTERMINATIONPOINTISDNUNI_B2CHANNELID_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_PHYSICALPATHTERMINATIONPOINTISDNUNI_ARC_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_PHYSICALPATHTERMINATIONPOINTISDNUNI_ARCINTERVAL_INDEX ((MIB_ATTR_INDEX)8)
#define MIB_TABLE_PHYSICALPATHTERMINATIONPOINTISDNUNI_ISDNLOOPBACKCONFIGURATION_INDEX ((MIB_ATTR_INDEX)9)

/* Table PhysicalPathTerminationPointISDNUNI attribute len, only string attrubutes have length definition */
typedef struct {
	UINT16   EntityId;
	UINT8    AdministrativeState;
	UINT16   InterworkingPointer;
	UINT8    DChannelID;
	UINT8    B1ChannelID;
	UINT8    B2ChannelID;
	UINT8    ARC;
	UINT8    ARCInterval;
	UINT8    ISDNLoopbackConfiguration;
} __attribute__((packed)) MIB_TABLE_PHYSICALPATHTERMINATIONPOINTISDNUNI_T;

#endif /* __MIB_PHYSICALPATHTERMINATIONPOINTISDNUNI_TABLE_H__ */
