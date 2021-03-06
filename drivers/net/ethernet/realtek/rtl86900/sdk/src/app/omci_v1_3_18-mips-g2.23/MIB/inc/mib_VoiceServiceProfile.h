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


#ifndef __MIB_VOICESERVICEPROFILE_TABLE_H__
#define __MIB_VOICESERVICEPROFILE_TABLE_H__

/* Table VoiceServiceProfile attribute for STRING type define each entry length */
#define MIB_TABLE_TONEPATTERNTABLE_LEN (20)
#define MIB_TABLE_TONEEVENTTABLE_LEN (7)
#define MIB_TABLE_RINGINGPATTERNTABLE_LEN (5)
#define MIB_TABLE_RINGINGEVENTTABLE_LEN (7)

/* Table VoiceServiceProfile attribute index */
#define MIB_TABLE_VOICESERVICEPROFILE_ATTR_NUM (15)
#define MIB_TABLE_VOICESERVICEPROFILE_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_VOICESERVICEPROFILE_ANNOUNCEMENTTYPE_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_VOICESERVICEPROFILE_JITTERTARGET_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_VOICESERVICEPROFILE_JITTERBUFFERMAX_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_VOICESERVICEPROFILE_ECHOCANCELIND_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_VOICESERVICEPROFILE_PSTNPROTOCOLVARIANT_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_VOICESERVICEPROFILE_DTMFDIGITLEVELS_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_VOICESERVICEPROFILE_DTMFDIGITDURATION_INDEX ((MIB_ATTR_INDEX)8)
#define MIB_TABLE_VOICESERVICEPROFILE_HOOKFLASHMINIMUMTIME_INDEX ((MIB_ATTR_INDEX)9)
#define MIB_TABLE_VOICESERVICEPROFILE_HOOKFLASHMAXIMUMTIME_INDEX ((MIB_ATTR_INDEX)10)
#define MIB_TABLE_VOICESERVICEPROFILE_TONEPATTERNTABLE_INDEX ((MIB_ATTR_INDEX)11)
#define MIB_TABLE_VOICESERVICEPROFILE_TONEEVENTTABLE_INDEX ((MIB_ATTR_INDEX)12)
#define MIB_TABLE_VOICESERVICEPROFILE_RINGINGPATTERNTABLE_INDEX ((MIB_ATTR_INDEX)13)
#define MIB_TABLE_VOICESERVICEPROFILE_RINGINGEVENTTABLE_INDEX ((MIB_ATTR_INDEX)14)
#define MIB_TABLE_VOICESERVICEPROFILE_NETWORKSPECIFICEXTENSIONSPOINTER_INDEX ((MIB_ATTR_INDEX)15)

/* Table VoiceServiceProfile attribute len, only string attrubutes have length definition */
typedef struct {
	UINT16   EntityId;
	UINT8    AnnouncementType;
	UINT16   JitterTarget;
	UINT16   JitterBufferMax;
	UINT8    EchoCancelInd;
	UINT16   PSTNProtocolVariant;
	UINT16   DTMFDigitLevels;
	UINT16   DTMFDigitDuration;
	UINT16   HookFlashMinimumTime;
	UINT16   HookFlashMaximumTime;
	UINT8    TonePatternTable[MIB_TABLE_TONEPATTERNTABLE_LEN];
	UINT8    ToneEventTable[MIB_TABLE_TONEEVENTTABLE_LEN];
	UINT8    RingingPatternTable[MIB_TABLE_RINGINGPATTERNTABLE_LEN];
	UINT8    RingingEventTable[MIB_TABLE_RINGINGEVENTTABLE_LEN];
	UINT16   NetworkSpecificExtensionsPointer;
} __attribute__((aligned)) MIB_TABLE_VOICESERVICEPROFILE_T;

#endif /* __MIB_VOICESERVICEPROFILE_TABLE_H__ */
