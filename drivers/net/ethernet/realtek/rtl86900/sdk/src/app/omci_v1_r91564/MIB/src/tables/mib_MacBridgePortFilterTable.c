/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * AUTOMATIC GENERATED BY PYTHON SCRIPT. PLEASE DONOT MODIFY DIRECTLY.
 *
 */


#include "app_basic.h"
#ifndef OMCI_X86
#include <hal/common/halctrl.h>
#endif

MIB_TABLE_INFO_T gMibMacBridgePortFilterTableTableInfo;
MIB_ATTR_INFO_T  gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_ATTR_NUM];
MIB_TABLE_MACBRIDGEPORTFILTERTABLE_T gMibMacBridgePortFilterTableDefRow;
MIB_TABLE_OPER_T gMibMacBridgePortFilterTableOper;

static void MacFilterTableDump(MIB_TABLE_MACBRIDGEPORTFILTERTABLE_T* pMacBridgePortFilterTable)
{
	macFilterTableEntry_t *pEntry;
	omci_macFilter_raw_entry_t *pRawEntry;

	OMCI_PRINT("Mac Filter Table:");
	LIST_FOREACH(pEntry,&pMacBridgePortFilterTable->head,entries){
		pRawEntry = &pEntry->tableEntry;
		OMCI_PRINT("INDEX %d",pRawEntry->firstWord.bit.entryIndex);
		OMCI_PRINT("%7s  %s : %04X%08X", pRawEntry->firstWord.bit.isFilter ? "Discard" : "Forward",  pRawEntry->firstWord.bit.isSa ? "SA" : "DA",
			pRawEntry->firstWord.bit.mac, pRawEntry->secondWord.bit.mac);

	}
}


GOS_ERROR_CODE MacBridgePortFilterTableDumpMib(void *pData, MIB_TABLE_INFO_T *pTblInfo)
{
	MIB_TABLE_MACBRIDGEPORTFILTERTABLE_T *pMacBridgePortFilterTable = (MIB_TABLE_MACBRIDGEPORTFILTERTABLE_T*)pData;

	OMCI_PRINT("EntityId: 0x%02x", pMacBridgePortFilterTable->EntityId);
	MacFilterTableDump(pMacBridgePortFilterTable);

	return GOS_OK;
}

GOS_ERROR_CODE BridgePortDirGet(UINT16 entityId, UINT32 *pDir)
{
	MIB_TABLE_MACBRIPORTCFGDATA_T tmpMacBriPortCfgData, *pMibMacBriPortCfgData;

	tmpMacBriPortCfgData.EntityID = entityId;

	if(!mib_FindEntry(MIB_TABLE_MACBRIPORTCFGDATA_INDEX,&tmpMacBriPortCfgData,&pMibMacBriPortCfgData))
	{
		OMCI_LOG(OMCI_LOG_LEVEL_DBG, "can't find MacBriPortCfgData 0x%x\n", entityId);
		return GOS_OK;
	}

	/*UNI side*/
	if(MBPCD_TP_TYPE_PPTP_ETH_UNI == pMibMacBriPortCfgData->TPType)
	{
		*pDir = MAC_FILTER_DIR_DS;
	}
	/*ANI side*/
	else if((MBPCD_TP_TYPE_GEM_IWTP == pMibMacBriPortCfgData->TPType) ||
		(MBPCD_TP_TYPE_IEEE_8021P_MAPPER == pMibMacBriPortCfgData->TPType))
	{
		*pDir = MAC_FILTER_DIR_US;
	}
	return GOS_OK;
}

GOS_ERROR_CODE MacFilterTableClearAll(MIB_TABLE_MACBRIDGEPORTFILTERTABLE_T *pMibMacBridgePortFilter,
	mbpcdStateInfo_t mbpcdState)
{
	macFilterTableEntry_t *pTmpEntry,*pRemoveEntry;
	OMCI_MACFILTER_ts macFilterSetting;

    for(pTmpEntry = LIST_FIRST(&pMibMacBridgePortFilter->head); pTmpEntry!=NULL;)
	{
		if(mbpcdState.isConnect)
		{
			/*Set to ASIC*/
			macFilterSetting.id = pMibMacBridgePortFilter->EntityId;
			macFilterSetting.entryAct = MAC_FILTER_ENTRY_ACT_REMOVE;
			BridgePortDirGet(pMibMacBridgePortFilter->EntityId, &(macFilterSetting.dir));
			macFilterSetting.portMask = mbpcdState.uniPortMask;
			macFilterSetting.macType = pTmpEntry->tableEntry.firstWord.bit.isSa;
			macFilterSetting.filter = pTmpEntry->tableEntry.firstWord.bit.isFilter;
			macFilterSetting.mac[0] = (pTmpEntry->tableEntry.firstWord.bit.mac >> 8) & 0xFF;
			macFilterSetting.mac[1] = pTmpEntry->tableEntry.firstWord.bit.mac & 0xFF;
			memcpy(&macFilterSetting.mac[2], &pTmpEntry->tableEntry.secondWord.val, 4);
			omci_wrapper_setMacFilter(&macFilterSetting);
		}
        pRemoveEntry = pTmpEntry;
        pTmpEntry = LIST_NEXT(pTmpEntry,entries);
        LIST_REMOVE(pRemoveEntry,entries);
        free(pRemoveEntry);
	}
	pMibMacBridgePortFilter->curMacTableEntryCnt = 0;
	return GOS_OK;
}



GOS_ERROR_CODE MacFilterTableOper(CHAR *pMACFilterEntry, MIB_TABLE_MACBRIDGEPORTFILTERTABLE_T *pMibMacBridgePortFilter,
	mbpcdStateInfo_t mbpcdState)
{
	macFilterTableEntry_t *pEntry,*pTmpEntry;
	omci_macFilter_raw_entry_t *macFilter;
	OMCI_MACFILTER_ts macFilterSetting;

	memset(&macFilterSetting, 0, sizeof(OMCI_MACFILTER_ts));

	pEntry = (macFilterTableEntry_t*)malloc(sizeof(macFilterTableEntry_t));
	if(!pEntry)
	{
		OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Malloc MacFilter Table Entry Fail");
		return GOS_FAIL;
	}

	macFilter = &pEntry->tableEntry;
	memcpy(macFilter,(omci_macFilter_raw_entry_t*)pMACFilterEntry, sizeof(omci_macFilter_raw_entry_t));
	macFilter->firstWord.val = GOS_Htonl(macFilter->firstWord.val);
	macFilter->secondWord.val = GOS_Htonl(macFilter->secondWord.val);

	switch(macFilter->firstWord.bit.oper)
	{
		case MAC_FILTER_TABLE_OPER_ADD:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_DBG, "MAC_FILTER_TABLE_OPER_ADD");
			LIST_INSERT_HEAD(&pMibMacBridgePortFilter->head, pEntry, entries);
			pMibMacBridgePortFilter->curMacTableEntryCnt++;
			if(mbpcdState.isConnect)
			{
				/*Set to ASIC*/
				macFilterSetting.id = pMibMacBridgePortFilter->EntityId;
				macFilterSetting.entryAct = MAC_FILTER_ENTRY_ACT_ADD;
				BridgePortDirGet(pMibMacBridgePortFilter->EntityId, &(macFilterSetting.dir));
				macFilterSetting.portMask = mbpcdState.uniPortMask;
				macFilterSetting.mac[0] = (macFilter->firstWord.bit.mac >> 8) & 0xFF;
				macFilterSetting.mac[1] = macFilter->firstWord.bit.mac & 0xFF;
				memcpy(&macFilterSetting.mac[2], &macFilter->secondWord.val, 4);
				macFilterSetting.macType = macFilter->firstWord.bit.isSa;
				macFilterSetting.filter = macFilter->firstWord.bit.isFilter;
				omci_wrapper_setMacFilter(&macFilterSetting);
			}
			break;
		}
		case MAC_FILTER_TABLE_OPER_REMOVE:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_DBG, "MAC_FILTER_TABLE_OPER_REMOVE");
			LIST_FOREACH(pTmpEntry,&pMibMacBridgePortFilter->head,entries){
				if((pTmpEntry->tableEntry.firstWord.bit.mac == macFilter->firstWord.bit.mac)
					&& (pTmpEntry->tableEntry.secondWord.val == macFilter->secondWord.val))
				{
					pMibMacBridgePortFilter->curMacTableEntryCnt--;
					if(mbpcdState.isConnect)
					{
						/*Set to ASIC*/
						macFilterSetting.id = pMibMacBridgePortFilter->EntityId;
						macFilterSetting.entryAct = MAC_FILTER_ENTRY_ACT_REMOVE;
						if(0 == pMibMacBridgePortFilter->curMacTableEntryCnt)
						{
							BridgePortDirGet(pMibMacBridgePortFilter->EntityId, &(macFilterSetting.dir));
							macFilterSetting.portMask = mbpcdState.uniPortMask;
							macFilterSetting.macType = pTmpEntry->tableEntry.firstWord.bit.isSa;
							macFilterSetting.filter = pTmpEntry->tableEntry.firstWord.bit.isFilter;
						}
						macFilterSetting.mac[0] = (macFilter->firstWord.bit.mac >> 8) & 0xFF;
						macFilterSetting.mac[1] = macFilter->firstWord.bit.mac & 0xFF;
						memcpy(&macFilterSetting.mac[2], &macFilter->secondWord.val, 4);
						omci_wrapper_setMacFilter(&macFilterSetting);
					}
					LIST_REMOVE(pTmpEntry,entries);
					free(pTmpEntry);
                    break;
				}
			}
			free(pEntry);
			break;
		}
		case MAC_FILTER_TABLE_OPER_CLEAR_ALL:
		{
			OMCI_LOG(OMCI_LOG_LEVEL_DBG, "MAC_FILTER_TABLE_OPER_CLEAR_ALL");
			MacFilterTableClearAll(pMibMacBridgePortFilter, mbpcdState);
			free(pEntry);
			break;
		}
		default:
                        free(pEntry);
			break;
	}
	return GOS_OK;

}

GOS_ERROR_CODE ANI_UNI_MBPCD_StateGet(MIB_TABLE_MACBRIDGEPORTFILTERTABLE_T *pMacBridgePortFilterTable,
	mbpcdStateInfo_t *pMbpcdState)
{
	MIB_TREE_T *pTree = NULL;
	MIB_AVL_KEY_T key = 0;
	UINT16 portId;
	MIB_TABLE_MACBRIPORTCFGDATA_T *pMBPCD = NULL, bridgePort;
	MIB_TREE_NODE_ENTRY_T *pNodeEntry = NULL;
	MIB_ENTRY_T *pEntry;
	MIB_NODE_T	*pNode;

	pMbpcdState->isConnect = FALSE;
	pMbpcdState->node = NULL;
	pMbpcdState->uniPortMask = 0;
	pMbpcdState->other_key = 0xffff;
	//check current is belong to UNI MBPCD or ANI MBPCD
	bridgePort.EntityID = pMacBridgePortFilterTable->EntityId;
	OMCI_ERR_CHK(OMCI_LOG_LEVEL_WARN, (!mib_FindEntry(MIB_TABLE_MACBRIPORTCFGDATA_INDEX, &bridgePort, &pMBPCD)), GOS_FAIL);

	switch (pMBPCD->TPType)
	{
		case MBPCD_TP_TYPE_PPTP_ETH_UNI: //UNI MBPCD
			key =  AVL_KEY_MACBRIPORT_UNI;
			pMbpcdState->other_key = AVL_KEY_MACBRIPORT_ANI;

			OMCI_ERR_CHK(OMCI_LOG_LEVEL_WARN, (GOS_OK != pptp_eth_uni_me_id_to_switch_port(pMBPCD->TPPointer, &portId)), GOS_FAIL);
			pMbpcdState->uniPortMask = (1 << portId);

			break;
		case MBPCD_TP_TYPE_IEEE_8021P_MAPPER: //ANI MBPCD
		case MBPCD_TP_TYPE_GEM_IWTP:
			key =  AVL_KEY_MACBRIPORT_ANI;
			pMbpcdState->other_key = AVL_KEY_MACBRIPORT_UNI;

            OMCI_ERR_CHK(OMCI_LOG_LEVEL_WARN, (GOS_OK != omci_get_pptp_eth_uni_port_mask_in_bridge(pMBPCD->BridgeIdPtr, &(pMbpcdState->uniPortMask))), GOS_FAIL);
            break;
		default:
			OMCI_LOG(OMCI_LOG_LEVEL_WARN, "Not support key=%u \n", key);
			return GOS_FAIL;
	}

	// check the other MBPCD exist or not in the same tree
	OMCI_ERR_CHK(OMCI_LOG_LEVEL_WARN, (NULL == (pTree = MIB_AvlTreeSearchByKey(NULL, pMacBridgePortFilterTable->EntityId, key))), GOS_FAIL);

    if (NULL != (pNode = MIB_AvlTreeSearch(pTree->root, pMbpcdState->other_key)))
	{
		OMCI_LOG(OMCI_LOG_LEVEL_DBG, " %s %d  exit MBPCD in the same tree. ", __FUNCTION__, __LINE__);
		pMbpcdState->node = (void *)pNode;

		LIST_FOREACH(pNodeEntry, &pNode->data.treeNodeEntryHead, treeNodeEntry)
		{
			pEntry = pNodeEntry->mibEntry;
			pMBPCD = (MIB_TABLE_MACBRIPORTCFGDATA_T*)pEntry->pData;
			switch (pMBPCD->TPType)
			{
				case MBPCD_TP_TYPE_PPTP_ETH_UNI: //UNI MBPCD
					pMbpcdState->isConnect = TRUE;
					break;
				case MBPCD_TP_TYPE_IEEE_8021P_MAPPER: //ANI MBPCD
				case MBPCD_TP_TYPE_GEM_IWTP:
					pMbpcdState->isConnect = TRUE;
					break;
				default:
					continue;
			}
		}
	}

	return GOS_OK;
}

GOS_ERROR_CODE MacBridgePortFilterTableDrvCfg(void* pOldRow, void* pNewRow, MIB_OPERA_TYPE operationType,  MIB_ATTRS_SET attrSet, UINT32 pri)
{
	mbpcdStateInfo_t mbpcdState;
	MIB_TABLE_MACBRIDGEPORTFILTERTABLE_T *pMacBridgePortFilterTable, *pMibMacBridgePortFilterTable, tmpMacBridgePortFilterTable;

	OMCI_LOG(OMCI_LOG_LEVEL_DBG, "Start %s...", __FUNCTION__);

	memset(&mbpcdState, 0, sizeof(mbpcdStateInfo_t));

	pMacBridgePortFilterTable = (MIB_TABLE_MACBRIDGEPORTFILTERTABLE_T*)pNewRow;

	tmpMacBridgePortFilterTable.EntityId = pMacBridgePortFilterTable->EntityId;

	if(!mib_FindEntry(MIB_TABLE_MACBRIDGEPORTFILTERTABLE_INDEX,&tmpMacBridgePortFilterTable,&pMibMacBridgePortFilterTable))
	{
		OMCI_LOG(OMCI_LOG_LEVEL_DBG, "can't find MacBridgePortFilterTable 0x%x\n", pMacBridgePortFilterTable->EntityId);
		return GOS_OK;
	}

	if (GOS_OK != ANI_UNI_MBPCD_StateGet(pMacBridgePortFilterTable, &mbpcdState))
		mbpcdState.isConnect = FALSE;

	/* In order to get uni port mask,
		omci should be check mac bridge port config data of ANI and UNI should be connected with mac service profile */
	switch (operationType)
	{
    case MIB_ADD:
    	OMCI_LOG(OMCI_LOG_LEVEL_DBG, "MacBridgePortFilterTable --> ADD\n");
		LIST_INIT(&pMibMacBridgePortFilterTable->head);

    	break;
    case MIB_SET:
		/* Not handle forward and filter action are mixed status due to spec is unspecified its behavior */
    	OMCI_LOG(OMCI_LOG_LEVEL_DBG, "MacBridgePortFilterTable --> SET\n");
		if (MIB_IsInAttrSet(&attrSet, MIB_TABLE_MACBRIDGEPORTFILTERTABLE_MACFILTERTABLE_INDEX))
		{
			MacFilterTableOper(pMacBridgePortFilterTable->MACFilterTable, pMibMacBridgePortFilterTable, mbpcdState);
		}
		break;
    case MIB_DEL:
    	OMCI_LOG(OMCI_LOG_LEVEL_DBG, "MacBridgePortFilterTable --> DEL\n");
		MacFilterTableClearAll(pMibMacBridgePortFilterTable, mbpcdState);
    	break;
    default:
    	return GOS_FAIL;
    	break;
    }

    return GOS_OK;
}

GOS_ERROR_CODE mibTable_init(MIB_TABLE_INDEX tableId)
{
    gMibMacBridgePortFilterTableTableInfo.Name = "MacBridgePortFilterTable";
    gMibMacBridgePortFilterTableTableInfo.ShortName = "MBPFTBL";
    gMibMacBridgePortFilterTableTableInfo.Desc = "MAC bridge port filter table data";
    gMibMacBridgePortFilterTableTableInfo.ClassId = (UINT32)(OMCI_ME_CLASS_MAC_BRG_PORT_FILTER_TBL_DATA);
    gMibMacBridgePortFilterTableTableInfo.InitType = (UINT32)(OMCI_ME_INIT_TYPE_ONU);
    gMibMacBridgePortFilterTableTableInfo.StdType = (UINT32)(OMCI_ME_TYPE_STANDARD);
    gMibMacBridgePortFilterTableTableInfo.ActionType = (UINT32)(OMCI_ME_ACTION_SET | OMCI_ME_ACTION_GET | OMCI_ME_ACTION_GET_NEXT);
    gMibMacBridgePortFilterTableTableInfo.pAttributes = &(gMibMacBridgePortFilterTableAttrInfo[0]);

    gMibMacBridgePortFilterTableTableInfo.attrNum = MIB_TABLE_MACBRIDGEPORTFILTERTABLE_ATTR_NUM;
    gMibMacBridgePortFilterTableTableInfo.entrySize = sizeof(MIB_TABLE_MACBRIDGEPORTFILTERTABLE_T);
    gMibMacBridgePortFilterTableTableInfo.pDefaultRow = &gMibMacBridgePortFilterTableDefRow;

    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "EntityId";
    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_MACFILTERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].Name = "MACFilterTable";

    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Entity ID";
    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_MACFILTERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "MAC filter table";

    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_MACFILTERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_TABLE;

    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_MACFILTERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].Len = 8;

    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = TRUE;
    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_MACFILTERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;

    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_MACFILTERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;

    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_MACFILTERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;

    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_MACFILTERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ | OMCI_ME_ATTR_ACCESS_WRITE;

    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_MACFILTERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;

    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = OMCI_ME_ATTR_TYPE_MANDATORY;
    gMibMacBridgePortFilterTableAttrInfo[MIB_TABLE_MACBRIDGEPORTFILTERTABLE_MACFILTERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_TABLE);

    gMibMacBridgePortFilterTableDefRow.EntityId = 0;
    memset(gMibMacBridgePortFilterTableDefRow.MACFilterTable, 0, sizeof(gMibMacBridgePortFilterTableDefRow.MACFilterTable));

	/*add for table type attribute*/
	LIST_INIT(&gMibMacBridgePortFilterTableDefRow.head);
	gMibMacBridgePortFilterTableDefRow.curMacTableEntryCnt = 0;

    memset(&gMibMacBridgePortFilterTableOper, 0x0, sizeof(MIB_TABLE_OPER_T));
    gMibMacBridgePortFilterTableOper.meOperDrvCfg = MacBridgePortFilterTableDrvCfg;
    gMibMacBridgePortFilterTableOper.meOperConnCheck = NULL;
    gMibMacBridgePortFilterTableOper.meOperDump = MacBridgePortFilterTableDumpMib;
    gMibMacBridgePortFilterTableOper.meOperConnCfg = NULL;
    gMibMacBridgePortFilterTableOper.meOperAvlTreeAdd = NULL;

    MIB_TABLE_MACBRIDGEPORTFILTERTABLE_INDEX = tableId;
    MIB_InfoRegister(tableId, &gMibMacBridgePortFilterTableTableInfo, &gMibMacBridgePortFilterTableOper);

    return GOS_OK;
}

