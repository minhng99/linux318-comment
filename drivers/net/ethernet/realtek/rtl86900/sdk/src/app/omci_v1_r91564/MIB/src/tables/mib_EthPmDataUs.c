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
 * Purpose : Definition of ME handler: Ethernet frame PMHD US (322)
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) ME handler: Ethernet frame PMHD US (322)
 */

#include "app_basic.h"
#ifndef OMCI_X86
#include "rtk/stat.h"
#endif

MIB_TABLE_INFO_T gMibEthPmDataUsTableInfo;
MIB_ATTR_INFO_T  gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_ATTR_NUM];
MIB_TABLE_ETHPMDATAUS_T gMibEthPmDataUsDefRow;
MIB_TABLE_OPER_T gMibEthPmDataUsOper;

static UINT8    aTcaAlmNumber[MIB_TABLE_ETHPMDATAUS_ATTR_NUM];
static UINT8    aTcaAttrMap[MIB_TABLE_ETHPMDATAUS_ATTR_NUM];


GOS_ERROR_CODE EthPmDataUs_CfgMe(void* pOldRow, void* pNewRow, MIB_OPERA_TYPE operationType, MIB_ATTRS_SET attrSet, UINT32 pri)
{
    GOS_ERROR_CODE                  ret;
    UINT32                          entrySize;
    MIB_TABLE_INDEX                 tableIndex;
    MIB_TABLE_MACBRIPORTCFGDATA_T   mibMBPCD;
    MIB_TABLE_ETHUNI_T              mibPptpEthUNI;
    MIB_TABLE_ETHPMDATAUS_T         *pMibEthPmhdUS;
    MIB_TABLE_ETHPMDATAUS_T         mibCurrentBin;

    pMibEthPmhdUS = (MIB_TABLE_ETHPMDATAUS_T *)pNewRow;

    // check if corresponding PPTP exists
    mibMBPCD.EntityID = pMibEthPmhdUS->EntityId;
    ret = MIB_Get(MIB_TABLE_MACBRIPORTCFGDATA_INDEX, &mibMBPCD, sizeof(MIB_TABLE_MACBRIPORTCFGDATA_T));
    if (GOS_OK != ret)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Instance not found in %s: %s, 0x%x",
            __FUNCTION__, MIB_GetTableName(MIB_TABLE_MACBRIPORTCFGDATA_INDEX), mibMBPCD.EntityID);

        return GOS_FAIL;
    }

    if (MBPCD_TP_TYPE_PPTP_ETH_UNI != mibMBPCD.TPType)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_ERR, "TP type is not correct in %s: %s, 0x%x",
            __FUNCTION__, MIB_GetTableName(MIB_TABLE_MACBRIPORTCFGDATA_INDEX), mibMBPCD.EntityID);

        return GOS_FAIL;
    }

    mibPptpEthUNI.EntityID = mibMBPCD.TPPointer;
    ret = MIB_Get(MIB_TABLE_ETHUNI_INDEX, &mibPptpEthUNI, sizeof(MIB_TABLE_ETHUNI_T));
    if (GOS_OK != ret)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Instance not found in %s: %s, 0x%x",
            __FUNCTION__, MIB_GetTableName(MIB_TABLE_ETHUNI_INDEX), mibPptpEthUNI.EntityID);

        return GOS_FAIL;
    }

    tableIndex = MIB_TABLE_ETHPMDATAUS_INDEX;
    entrySize = MIB_GetTableEntrySize(tableIndex);

    switch (operationType)
    {
        case MIB_ADD:
            // sync control block data from history bin to current bin
            if (GOS_OK != MIB_SetPmCurrentBin(tableIndex, pMibEthPmhdUS, entrySize))
            {
                OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Set current bin error in %s: %s, 0x%x",
                    __FUNCTION__, MIB_GetTableName(tableIndex), pMibEthPmhdUS->EntityId);
            }

            ret = mib_alarm_table_add(tableIndex, pNewRow);
            break;

        case MIB_DEL:
            ret = mib_alarm_table_del(tableIndex, pOldRow);
            break;

        case MIB_SET:
            mibCurrentBin.EntityId = pMibEthPmhdUS->EntityId;
            if (GOS_OK != MIB_GetPmCurrentBin(tableIndex, &mibCurrentBin, entrySize))
            {
                OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Current bin not found in %s: %s, 0x%x",
                    __FUNCTION__, MIB_GetTableName(tableIndex), mibCurrentBin.EntityId);

                return GOS_FAIL;
            }

            // sync control block data from history bin to current bin
            mibCurrentBin.ThresholdID = pMibEthPmhdUS->ThresholdID;
            if (GOS_OK != MIB_SetPmCurrentBin(tableIndex, &mibCurrentBin, entrySize))
            {
                OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Set current bin error in %s: %s, 0x%x",
                    __FUNCTION__, MIB_GetTableName(tableIndex), mibCurrentBin.EntityId);
            }
            break;

        default:
            break;
    }

    return ret;
}

GOS_ERROR_CODE ethernet_frame_pmhd_us_pm_handler(MIB_TABLE_INDEX        tableIndex,
                                                omci_me_instance_t      instanceID,
                                                omci_pm_oper_type_t     operType,
                                                BOOL                    *pIsTcaRaised)
{
    GOS_ERROR_CODE                  ret;
    UINT32                          entrySize;
    MIB_TABLE_MACBRIPORTCFGDATA_T   mibMBPCD;
    MIB_TABLE_ETHUNI_T              mibPptpEthUNI;
    MIB_TABLE_ETHPMDATAUS_T         mibEthPmhdUS;
    MIB_TABLE_ETHPMDATAUS_T         mibCurrentBin;
    omci_port_stat_t                portCntrs;
    omci_port_stat_t                *pPortCntrs = (omci_port_stat_t*)&portCntrs;
    unsigned long long              tempCntr;

    if (!pIsTcaRaised)
        return GOS_ERR_PARAM;

    // get table entry size
    entrySize = MIB_GetTableEntrySize(tableIndex);

    mibMBPCD.EntityID = instanceID;
    if (GOS_OK != MIB_Get(MIB_TABLE_MACBRIPORTCFGDATA_INDEX, &mibMBPCD, sizeof(MIB_TABLE_MACBRIPORTCFGDATA_T)))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Instance not found in %s: %s, 0x%x",
            __FUNCTION__, MIB_GetTableName(MIB_TABLE_MACBRIPORTCFGDATA_INDEX), mibMBPCD.EntityID);

        return GOS_FAIL;
    }

    if (MBPCD_TP_TYPE_PPTP_ETH_UNI != mibMBPCD.TPType)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_ERR, "TP type is not correct in %s: %s, 0x%x",
            __FUNCTION__, MIB_GetTableName(MIB_TABLE_MACBRIPORTCFGDATA_INDEX), mibMBPCD.EntityID);

        return GOS_FAIL;
    }

    mibPptpEthUNI.EntityID = mibMBPCD.TPPointer;
    if (GOS_OK != MIB_Get(MIB_TABLE_ETHUNI_INDEX, &mibPptpEthUNI, sizeof(MIB_TABLE_ETHUNI_T)))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Instance not found in %s: %s, 0x%x",
            __FUNCTION__, MIB_GetTableName(MIB_TABLE_ETHUNI_INDEX), mibPptpEthUNI.EntityID);

        return GOS_FAIL;
    }

    mibEthPmhdUS.EntityId = instanceID;
    if (GOS_OK != MIB_Get(tableIndex, &mibEthPmhdUS, entrySize))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Instance not found in %s: %s, 0x%x",
            __FUNCTION__, MIB_GetTableName(tableIndex), instanceID);

        return GOS_FAIL;
    }

    mibCurrentBin.EntityId = instanceID;
    if (GOS_OK != MIB_GetPmCurrentBin(tableIndex, &mibCurrentBin, entrySize))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Current bin not found in %s: %s, 0x%x",
            __FUNCTION__, MIB_GetTableName(tableIndex), instanceID);

        return GOS_FAIL;
    }

    if (OMCI_PM_OPER_RESET == operType)
    {
        memset(&mibCurrentBin, 0, entrySize);
        mibCurrentBin.EntityId = instanceID;
        mibCurrentBin.ThresholdID = mibEthPmhdUS.ThresholdID;

        if (GOS_OK != MIB_SetPmCurrentBin(tableIndex, &mibCurrentBin, entrySize))
        {
            OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Set current bin error in %s: %s, 0x%x",
                __FUNCTION__, MIB_GetTableName(tableIndex), instanceID);
        }

        if (GOS_OK != MIB_Set(tableIndex, &mibCurrentBin, entrySize))
        {
            OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Set instance error in %s: %s, 0x%x",
                __FUNCTION__, MIB_GetTableName(tableIndex), instanceID);
        }

        return GOS_OK;
    }

	if(OMCI_PM_OPER_GET_CURRENT_DATA == operType)
	{
		omci_pm_update_pptp_eth_uni(OMCI_PM_OPER_GET_CURRENT_DATA);
	}

    if (OMCI_PM_OPER_UPDATE == operType ||
		OMCI_PM_OPER_UPDATE_AND_SWAP == operType ||
		OMCI_PM_OPER_GET_CURRENT_DATA == operType)

    {
        // update PM values
        if (GOS_OK == omci_pm_getcurrent_pptp_eth_uni(mibPptpEthUNI.EntityID,pPortCntrs))
        {
            m_omci_pm_update_accum_attr(pPortCntrs->etherStatsDropEvents, mibCurrentBin.DropEvents);
            m_omci_pm_update_accum_attr(pPortCntrs->ifInOctets, mibCurrentBin.Octets);
            tempCntr = pPortCntrs->etherStatsFragments +
                        pPortCntrs->etherStatsRxUndersizePkts +
                        pPortCntrs->etherStatsRxPkts64Octets +
                        pPortCntrs->etherStatsRxPkts65to127Octets +
                        pPortCntrs->etherStatsRxPkts128to255Octets +
                        pPortCntrs->etherStatsRxPkts256to511Octets +
                        pPortCntrs->etherStatsRxPkts512to1023Octets +
                        pPortCntrs->etherStatsRxPkts1024to1518Octets +
                        pPortCntrs->etherStatsRxOversizePkts +
                        pPortCntrs->etherStatsJabbers;
            m_omci_pm_update_accum_attr(tempCntr, mibCurrentBin.Packets);
            m_omci_pm_update_accum_attr(pPortCntrs->ifInBroadcastPkts, mibCurrentBin.BroadcastPackets);
            m_omci_pm_update_accum_attr(pPortCntrs->ifInMulticastPkts, mibCurrentBin.MulticastPackets);
            m_omci_pm_update_accum_attr(pPortCntrs->etherStatsCRCAlignErrors, mibCurrentBin.CrcErrPackets);
            m_omci_pm_update_accum_attr(pPortCntrs->etherStatsRxUndersizePkts, mibCurrentBin.UndersizePackets);
            m_omci_pm_update_accum_attr(pPortCntrs->etherStatsRxOversizePkts, mibCurrentBin.OversizePackets);
            m_omci_pm_update_accum_attr(pPortCntrs->etherStatsRxPkts64Octets, mibCurrentBin.Octets64);
            m_omci_pm_update_accum_attr(pPortCntrs->etherStatsRxPkts65to127Octets, mibCurrentBin.Octets65to127);
            m_omci_pm_update_accum_attr(pPortCntrs->etherStatsRxPkts128to255Octets, mibCurrentBin.Octets128to255);
            m_omci_pm_update_accum_attr(pPortCntrs->etherStatsRxPkts256to511Octets, mibCurrentBin.Octets256to511);
            m_omci_pm_update_accum_attr(pPortCntrs->etherStatsRxPkts512to1023Octets, mibCurrentBin.Octets512to1023);
            m_omci_pm_update_accum_attr(pPortCntrs->etherStatsRxPkts1024to1518Octets, mibCurrentBin.Octets1024to1518);
        }

        if (GOS_OK != MIB_SetPmCurrentBin(tableIndex, &mibCurrentBin, entrySize))
        {
            OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Set current bin error in %s: %s, 0x%x",
                __FUNCTION__, MIB_GetTableName(tableIndex), instanceID);
        }

        ret = omci_pm_is_threshold_crossed(tableIndex, instanceID,
                &mibCurrentBin, aTcaAlmNumber, aTcaAttrMap, NULL, pIsTcaRaised);
        if (GOS_OK != ret && GOS_ERR_NOT_FOUND != ret)
        {
            OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Check threshold error in %s: %s, 0x%x",
                __FUNCTION__, MIB_GetTableName(tableIndex), instanceID);
        }
    }

    if (OMCI_PM_OPER_SWAP == operType || OMCI_PM_OPER_UPDATE_AND_SWAP == operType)
    {
        // swap PM values
        mibCurrentBin.IntEndTime++;
        mibEthPmhdUS.IntEndTime = mibCurrentBin.IntEndTime;

        if (GOS_OK != MIB_Set(tableIndex, &mibCurrentBin, entrySize))
        {
            OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Set instance error in %s: %s, 0x%x",
                __FUNCTION__, MIB_GetTableName(tableIndex), instanceID);
        }

        memset(&mibCurrentBin, 0, entrySize);
        mibCurrentBin.EntityId = instanceID;
        mibCurrentBin.ThresholdID = mibEthPmhdUS.ThresholdID;
        mibCurrentBin.IntEndTime = mibEthPmhdUS.IntEndTime;

        if (GOS_OK != MIB_SetPmCurrentBin(tableIndex, &mibCurrentBin, entrySize))
        {
            OMCI_LOG(OMCI_LOG_LEVEL_ERR, "Set current bin error in %s: %s, 0x%x",
                __FUNCTION__, MIB_GetTableName(tableIndex), instanceID);
        }
    }

    return GOS_OK;
}

GOS_ERROR_CODE mibTable_init(MIB_TABLE_INDEX tableId)
{
    gMibEthPmDataUsTableInfo.Name = "EthPmDataUs";
    gMibEthPmDataUsTableInfo.ShortName = "ETHFRAMEPMHDUS";
    gMibEthPmDataUsTableInfo.Desc = "Ethernet frame performance monitoring history data upstream";
    gMibEthPmDataUsTableInfo.ClassId = (UINT32)(OMCI_ME_CLASS_ETHERNET_FRAME_PMHD_UPSTREAM);
    gMibEthPmDataUsTableInfo.InitType = (UINT32)(OMCI_ME_INIT_TYPE_OLT);
    gMibEthPmDataUsTableInfo.StdType = (UINT32)(OMCI_ME_TYPE_STANDARD_PM);
    gMibEthPmDataUsTableInfo.ActionType = (UINT32)(OMCI_ME_ACTION_CREATE | OMCI_ME_ACTION_DELETE | OMCI_ME_ACTION_SET | OMCI_ME_ACTION_GET | OMCI_ME_ACTION_GET_CURRENT_DATA);
    gMibEthPmDataUsTableInfo.pAttributes = &(gMibEthPmDataUsAttrInfo[0]);

	gMibEthPmDataUsTableInfo.attrNum = MIB_TABLE_ETHPMDATAUS_ATTR_NUM;
	gMibEthPmDataUsTableInfo.entrySize = sizeof(MIB_TABLE_ETHPMDATAUS_T);
	gMibEthPmDataUsTableInfo.pDefaultRow = &gMibEthPmDataUsDefRow;

    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "EntityId";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_INTENDTIME_INDEX - MIB_TABLE_FIRST_INDEX].Name = "IntEndTime";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_THRESHOLDID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "ThresholdID";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_DROPEVENTS_INDEX - MIB_TABLE_FIRST_INDEX].Name = "DropEvents";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS_INDEX - MIB_TABLE_FIRST_INDEX].Name = "Octets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_PACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Name = "Packets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_BROADCASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Name = "BroadcastPackets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_MULTICASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Name = "MulticastPackets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_CRCERRPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Name = "CrcErrPackets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_UNDERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Name = "UndersizePackets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OVERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Name = "OversizePackets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS64_INDEX - MIB_TABLE_FIRST_INDEX].Name = "Octets64";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS65TO127_INDEX - MIB_TABLE_FIRST_INDEX].Name = "Octets65to127";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS128TO255_INDEX - MIB_TABLE_FIRST_INDEX].Name = "Octets128to255";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS256TO511_INDEX - MIB_TABLE_FIRST_INDEX].Name = "Octets256to511";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS512TO1023_INDEX - MIB_TABLE_FIRST_INDEX].Name = "Octets512to1023";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS1024TO1518_INDEX - MIB_TABLE_FIRST_INDEX].Name = "Octets1024to1518";

    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Entity ID";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_INTENDTIME_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Interval End Time";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_THRESHOLDID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Threshold Data ID";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_DROPEVENTS_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Drop Events";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "The total number of Octets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_PACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "The total number of packets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_BROADCASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "The total number of broadcast packets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_MULTICASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "The total number of multicast packets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_CRCERRPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "CRC errored packets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_UNDERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "The total number of undersize packets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OVERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "The total number of oversize packets";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS64_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "The number of packets that were 64 octets long";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS65TO127_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "The number of packets that were 65..127 octets long";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS128TO255_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "The number of packets that were 128..255 octets long";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS256TO511_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "The number of packets that were 256..511 octets long";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS512TO1023_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "The number of packets that were 512..1023 octets long";
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS1024TO1518_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "The number of packets that were 1024..1518 octets long";

    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_INTENDTIME_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_THRESHOLDID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_DROPEVENTS_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_PACKETS_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_BROADCASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_MULTICASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_CRCERRPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_UNDERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OVERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS64_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS65TO127_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS128TO255_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS256TO511_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS512TO1023_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS1024TO1518_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;

    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_INTENDTIME_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_THRESHOLDID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_DROPEVENTS_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_PACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_BROADCASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_MULTICASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_CRCERRPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_UNDERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OVERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS64_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS65TO127_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS128TO255_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS256TO511_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS512TO1023_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS1024TO1518_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;

    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = TRUE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_INTENDTIME_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_THRESHOLDID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_DROPEVENTS_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_PACKETS_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_BROADCASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_MULTICASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_CRCERRPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_UNDERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OVERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS64_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS65TO127_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS128TO255_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS256TO511_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS512TO1023_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS1024TO1518_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;

    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_INTENDTIME_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_THRESHOLDID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_DROPEVENTS_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_PACKETS_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_BROADCASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_MULTICASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_CRCERRPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_UNDERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OVERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS64_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS65TO127_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS128TO255_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS256TO511_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS512TO1023_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS1024TO1518_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = FALSE;

    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_INTENDTIME_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_THRESHOLDID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_DROPEVENTS_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_PACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_BROADCASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_MULTICASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_CRCERRPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_UNDERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OVERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS64_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS65TO127_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS128TO255_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS256TO511_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS512TO1023_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS1024TO1518_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;

    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ | OMCI_ME_ATTR_ACCESS_SBC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_INTENDTIME_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_THRESHOLDID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ | OMCI_ME_ATTR_ACCESS_WRITE | OMCI_ME_ATTR_ACCESS_SBC;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_DROPEVENTS_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_PACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_BROADCASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_MULTICASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_CRCERRPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_UNDERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OVERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS64_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS65TO127_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS128TO255_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS256TO511_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS512TO1023_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS1024TO1518_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = OMCI_ME_ATTR_ACCESS_READ;

    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_INTENDTIME_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_THRESHOLDID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_DROPEVENTS_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_PACKETS_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_BROADCASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_MULTICASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_CRCERRPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_UNDERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OVERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS64_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS65TO127_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS128TO255_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS256TO511_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS512TO1023_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS1024TO1518_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;

    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = OMCI_ME_ATTR_TYPE_MANDATORY;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_INTENDTIME_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_THRESHOLDID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = OMCI_ME_ATTR_TYPE_MANDATORY;
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_DROPEVENTS_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_PACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_BROADCASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_MULTICASTPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_CRCERRPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_UNDERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OVERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS64_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS65TO127_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS128TO255_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS256TO511_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS512TO1023_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);
    gMibEthPmDataUsAttrInfo[MIB_TABLE_ETHPMDATAUS_OCTETS1024TO1518_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = (OMCI_ME_ATTR_TYPE_MANDATORY | OMCI_ME_ATTR_TYPE_PM);

    memset(&(gMibEthPmDataUsDefRow.EntityId), 0x00, sizeof(gMibEthPmDataUsDefRow.EntityId));
    memset(&(gMibEthPmDataUsDefRow.IntEndTime), 0x00, sizeof(gMibEthPmDataUsDefRow.IntEndTime));
    memset(&(gMibEthPmDataUsDefRow.ThresholdID), 0x00, sizeof(gMibEthPmDataUsDefRow.ThresholdID));
    memset(&(gMibEthPmDataUsDefRow.DropEvents), 0x00, sizeof(gMibEthPmDataUsDefRow.DropEvents));
    memset(&(gMibEthPmDataUsDefRow.Octets), 0x00, sizeof(gMibEthPmDataUsDefRow.Octets));
    memset(&(gMibEthPmDataUsDefRow.Packets), 0x00, sizeof(gMibEthPmDataUsDefRow.Packets));
    memset(&(gMibEthPmDataUsDefRow.BroadcastPackets), 0x00, sizeof(gMibEthPmDataUsDefRow.BroadcastPackets));
    memset(&(gMibEthPmDataUsDefRow.MulticastPackets), 0x00, sizeof(gMibEthPmDataUsDefRow.MulticastPackets));
    memset(&(gMibEthPmDataUsDefRow.CrcErrPackets), 0x00, sizeof(gMibEthPmDataUsDefRow.CrcErrPackets));
    memset(&(gMibEthPmDataUsDefRow.UndersizePackets), 0x00, sizeof(gMibEthPmDataUsDefRow.UndersizePackets));
    memset(&(gMibEthPmDataUsDefRow.OversizePackets), 0x00, sizeof(gMibEthPmDataUsDefRow.OversizePackets));
    memset(&(gMibEthPmDataUsDefRow.Octets64), 0x00, sizeof(gMibEthPmDataUsDefRow.Octets64));
    memset(&(gMibEthPmDataUsDefRow.Octets65to127), 0x00, sizeof(gMibEthPmDataUsDefRow.Octets65to127));
    memset(&(gMibEthPmDataUsDefRow.Octets128to255), 0x00, sizeof(gMibEthPmDataUsDefRow.Octets128to255));
    memset(&(gMibEthPmDataUsDefRow.Octets256to511), 0x00, sizeof(gMibEthPmDataUsDefRow.Octets256to511));
    memset(&(gMibEthPmDataUsDefRow.Octets512to1023), 0x00, sizeof(gMibEthPmDataUsDefRow.Octets512to1023));
    memset(&(gMibEthPmDataUsDefRow.Octets1024to1518), 0x00, sizeof(gMibEthPmDataUsDefRow.Octets1024to1518));


    memset(&gMibEthPmDataUsOper, 0x0, sizeof(MIB_TABLE_OPER_T));
	gMibEthPmDataUsOper.meOperDrvCfg = EthPmDataUs_CfgMe;
    gMibEthPmDataUsOper.meOperConnCfg = NULL;
	gMibEthPmDataUsOper.meOperConnCheck = NULL;
	gMibEthPmDataUsOper.meOperAvlTreeAdd = NULL;
    gMibEthPmDataUsOper.meOperDump = omci_mib_oper_dump_default_handler;
    gMibEthPmDataUsOper.meOperPmHandler = ethernet_frame_pmhd_us_pm_handler;

    // for PM ME only, indicates the Threshold value attribute mapping and alarm number
    memset(aTcaAlmNumber, 0, sizeof(aTcaAlmNumber));
    aTcaAlmNumber[MIB_TABLE_ETHPMDATAUS_DROPEVENTS_INDEX - MIB_TABLE_FIRST_INDEX] = 0;
    aTcaAlmNumber[MIB_TABLE_ETHPMDATAUS_CRCERRPACKETS_INDEX - MIB_TABLE_FIRST_INDEX] = 1;
    aTcaAlmNumber[MIB_TABLE_ETHPMDATAUS_UNDERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX] = 2;
    aTcaAlmNumber[MIB_TABLE_ETHPMDATAUS_OVERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX] = 3;
    memset(aTcaAttrMap, 0, sizeof(aTcaAttrMap));
    aTcaAttrMap[MIB_TABLE_ETHPMDATAUS_DROPEVENTS_INDEX - MIB_TABLE_FIRST_INDEX] = 1;
    aTcaAttrMap[MIB_TABLE_ETHPMDATAUS_CRCERRPACKETS_INDEX - MIB_TABLE_FIRST_INDEX] = 2;
    aTcaAttrMap[MIB_TABLE_ETHPMDATAUS_UNDERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX] = 3;
    aTcaAttrMap[MIB_TABLE_ETHPMDATAUS_OVERSIZEPACKETS_INDEX - MIB_TABLE_FIRST_INDEX] = 4;

    MIB_TABLE_ETHPMDATAUS_INDEX = tableId;
    MIB_InfoRegister(tableId, &gMibEthPmDataUsTableInfo, &gMibEthPmDataUsOper);

    return GOS_OK;
}
