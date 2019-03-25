/*
 * ## Please DO NOT edit this file!! ##
 * This file is auto-generated from the register source files.
 * Any modifications to this file will be LOST when it is re-generated.
 *
 * ----------------------------------------------------------------
 * Copyright(c) Realtek Semiconductor Corporation, 2009-2016
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : chip table declaration in the SDK.
 *
 * Feature : chip table declaration
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_table_struct.h>
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_tableField_list.h>

rtk_table_t rtk_rtl9607c_table_list[] =
{
#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER_HSB)
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSB_DATA_TABLE */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  27,
        /* total field numbers */   RTL9607C_HSB_DATA_TABLEFIELD_LIST_END,
        /* table fields */          RTL9607C_HSB_DATA_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSB_DATA_TABLE2 */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  27,
        /* total field numbers */   RTL9607C_HSB_DATA_TABLE2FIELD_LIST_END,
        /* table fields */          RTL9607C_HSB_DATA_TABLE2_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSB_PAR_TABLE */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  3,
        /* total field numbers */   RTL9607C_HSB_PAR_TABLEFIELD_LIST_END,
        /* table fields */          RTL9607C_HSB_PAR_TABLE_FIELDS
    },
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER_HSB */
#if defined(CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA)
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSA_DATA_FB */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  23,
        /* total field numbers */   RTL9607C_HSA_DATA_FBFIELD_LIST_END,
        /* table fields */          RTL9607C_HSA_DATA_FB_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSA_DATA_FB2 */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  23,
        /* total field numbers */   RTL9607C_HSA_DATA_FB2FIELD_LIST_END,
        /* table fields */          RTL9607C_HSA_DATA_FB2_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSA_DATA_NAT */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  23,
        /* total field numbers */   RTL9607C_HSA_DATA_NATFIELD_LIST_END,
        /* table fields */          RTL9607C_HSA_DATA_NAT_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSA_DATA_NAT2 */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  23,
        /* total field numbers */   RTL9607C_HSA_DATA_NAT2FIELD_LIST_END,
        /* table fields */          RTL9607C_HSA_DATA_NAT2_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSA_DATA_NORMAL */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  23,
        /* total field numbers */   RTL9607C_HSA_DATA_NORMALFIELD_LIST_END,
        /* table fields */          RTL9607C_HSA_DATA_NORMAL_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSA_DATA_NORMAL2 */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  23,
        /* total field numbers */   RTL9607C_HSA_DATA_NORMAL2FIELD_LIST_END,
        /* table fields */          RTL9607C_HSA_DATA_NORMAL2_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSA_DATA_OMCI */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  23,
        /* total field numbers */   RTL9607C_HSA_DATA_OMCIFIELD_LIST_END,
        /* table fields */          RTL9607C_HSA_DATA_OMCI_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSA_DATA_OMCI2 */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  23,
        /* total field numbers */   RTL9607C_HSA_DATA_OMCI2FIELD_LIST_END,
        /* table fields */          RTL9607C_HSA_DATA_OMCI2_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSA_DATA_PTP */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  23,
        /* total field numbers */   RTL9607C_HSA_DATA_PTPFIELD_LIST_END,
        /* table fields */          RTL9607C_HSA_DATA_PTP_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSA_DATA_PTP2 */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  23,
        /* total field numbers */   RTL9607C_HSA_DATA_PTP2FIELD_LIST_END,
        /* table fields */          RTL9607C_HSA_DATA_PTP2_FIELDS
    },
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA */
#if defined(CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_)
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_HSA_DEBUG_DATA */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  16,
        /* total field numbers */   RTL9607C_HSA_DEBUG_DATAFIELD_LIST_END,
        /* table fields */          RTL9607C_HSA_DEBUG_DATA_FIELDS
    },
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_ACL_ACTION_TABLE */
        /* access table type */     3,
        /* table size */            128,
        /* total data registers */  2,
        /* total field numbers */   RTL9607C_ACL_ACTION_TABLEFIELD_LIST_END,
        /* table fields */          RTL9607C_ACL_ACTION_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_ACL_DATA */
        /* access table type */     2,
        /* table size */            128,
        /* total data registers */  5,
        /* total field numbers */   RTL9607C_ACL_DATAFIELD_LIST_END,
        /* table fields */          RTL9607C_ACL_DATA_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_ACL_MASK */
        /* access table type */     2,
        /* table size */            128,
        /* total data registers */  5,
        /* total field numbers */   RTL9607C_ACL_MASKFIELD_LIST_END,
        /* table fields */          RTL9607C_ACL_MASK_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_CF_ACTION_DS */
        /* access table type */     5,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9607C_CF_ACTION_DSFIELD_LIST_END,
        /* table fields */          RTL9607C_CF_ACTION_DS_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_CF_ACTION_US */
        /* access table type */     5,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9607C_CF_ACTION_USFIELD_LIST_END,
        /* table fields */          RTL9607C_CF_ACTION_US_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_CF_MASK_T0 */
        /* access table type */     4,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9607C_CF_MASK_T0FIELD_LIST_END,
        /* table fields */          RTL9607C_CF_MASK_T0_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_CF_MASK_T1 */
        /* access table type */     4,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9607C_CF_MASK_T1FIELD_LIST_END,
        /* table fields */          RTL9607C_CF_MASK_T1_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_CF_MASK_T2 */
        /* access table type */     4,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9607C_CF_MASK_T2FIELD_LIST_END,
        /* table fields */          RTL9607C_CF_MASK_T2_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_CF_RULE_T0 */
        /* access table type */     4,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9607C_CF_RULE_T0FIELD_LIST_END,
        /* table fields */          RTL9607C_CF_RULE_T0_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_CF_RULE_T1 */
        /* access table type */     4,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9607C_CF_RULE_T1FIELD_LIST_END,
        /* table fields */          RTL9607C_CF_RULE_T1_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_CF_RULE_T2 */
        /* access table type */     4,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9607C_CF_RULE_T2FIELD_LIST_END,
        /* table fields */          RTL9607C_CF_RULE_T2_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L2_MC_DSL */
        /* access table type */     0,
        /* table size */            2112,
        /* total data registers */  3,
        /* total field numbers */   RTL9607C_L2_MC_DSLFIELD_LIST_END,
        /* table fields */          RTL9607C_L2_MC_DSL_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L2_UC */
        /* access table type */     0,
        /* table size */            2112,
        /* total data registers */  3,
        /* total field numbers */   RTL9607C_L2_UCFIELD_LIST_END,
        /* table fields */          RTL9607C_L2_UC_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L3_MC */
        /* access table type */     0,
        /* table size */            2112,
        /* total data registers */  3,
        /* total field numbers */   RTL9607C_L3_MCFIELD_LIST_END,
        /* table fields */          RTL9607C_L3_MC_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L3_MC_FID */
        /* access table type */     0,
        /* table size */            2112,
        /* total data registers */  3,
        /* total field numbers */   RTL9607C_L3_MC_FIDFIELD_LIST_END,
        /* table fields */          RTL9607C_L3_MC_FID_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L3_MC_VID */
        /* access table type */     0,
        /* table size */            2112,
        /* total data registers */  3,
        /* total field numbers */   RTL9607C_L3_MC_VIDFIELD_LIST_END,
        /* table fields */          RTL9607C_L3_MC_VID_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_VLAN */
        /* access table type */     1,
        /* table size */            4096,
        /* total data registers */  1,
        /* total field numbers */   RTL9607C_VLANFIELD_LIST_END,
        /* table fields */          RTL9607C_VLAN_FIELDS
    },
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_EPON_CONFIGURATION)
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_EPON_GRANT */
        /* access table type */     0,
        /* table size */            511,
        /* total data registers */  2,
        /* total field numbers */   RTL9607C_EPON_GRANTFIELD_LIST_END,
        /* table fields */          RTL9607C_EPON_GRANT_FIELDS
    },
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_EPON_CONFIGURATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_CAM */
        /* access table type */     9,
        /* table size */            64,
        /* total data registers */  8,
        /* total field numbers */   RTL9607C_CAMFIELD_LIST_END,
        /* table fields */          RTL9607C_CAM_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_CAM_TAG */
        /* access table type */     2,
        /* table size */            64,
        /* total data registers */  1,
        /* total field numbers */   RTL9607C_CAM_TAGFIELD_LIST_END,
        /* table fields */          RTL9607C_CAM_TAG_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_ETHER_TYPE */
        /* access table type */     1,
        /* table size */            8,
        /* total data registers */  1,
        /* total field numbers */   RTL9607C_ETHER_TYPEFIELD_LIST_END,
        /* table fields */          RTL9607C_ETHER_TYPE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_FB_EXT_PORT */
        /* access table type */     3,
        /* table size */            32,
        /* total data registers */  1,
        /* total field numbers */   RTL9607C_FB_EXT_PORTFIELD_LIST_END,
        /* table fields */          RTL9607C_FB_EXT_PORT_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_FLOW_TABLE_PATH1_2 */
        /* access table type */     8,
        /* table size */            4096,
        /* total data registers */  8,
        /* total field numbers */   RTL9607C_FLOW_TABLE_PATH1_2FIELD_LIST_END,
        /* table fields */          RTL9607C_FLOW_TABLE_PATH1_2_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_FLOW_TABLE_PATH3_4 */
        /* access table type */     8,
        /* table size */            4096,
        /* total data registers */  8,
        /* total field numbers */   RTL9607C_FLOW_TABLE_PATH3_4FIELD_LIST_END,
        /* table fields */          RTL9607C_FLOW_TABLE_PATH3_4_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_FLOW_TABLE_PATH5 */
        /* access table type */     8,
        /* table size */            4096,
        /* total data registers */  8,
        /* total field numbers */   RTL9607C_FLOW_TABLE_PATH5FIELD_LIST_END,
        /* table fields */          RTL9607C_FLOW_TABLE_PATH5_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_FLOW_TABLE_PATH6 */
        /* access table type */     8,
        /* table size */            4096,
        /* total data registers */  8,
        /* total field numbers */   RTL9607C_FLOW_TABLE_PATH6FIELD_LIST_END,
        /* table fields */          RTL9607C_FLOW_TABLE_PATH6_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_FLOW_TABLE_TAG */
        /* access table type */     11,
        /* table size */            4096,
        /* total data registers */  1,
        /* total field numbers */   RTL9607C_FLOW_TABLE_TAGFIELD_LIST_END,
        /* table fields */          RTL9607C_FLOW_TABLE_TAG_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_INTERFACE */
        /* access table type */     0,
        /* table size */            16,
        /* total data registers */  5,
        /* total field numbers */   RTL9607C_INTERFACEFIELD_LIST_END,
        /* table fields */          RTL9607C_INTERFACE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_MAC_IDX */
        /* access table type */     10,
        /* table size */            256,
        /* total data registers */  1,
        /* total field numbers */   RTL9607C_MAC_IDXFIELD_LIST_END,
        /* table fields */          RTL9607C_MAC_IDX_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_TCAM */
        /* access table type */     12,
        /* table size */            64,
        /* total data registers */  8,
        /* total field numbers */   RTL9607C_TCAMFIELD_LIST_END,
        /* table fields */          RTL9607C_TCAM_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_TCAM_RAW_TABLE_PATH1_2 */
        /* access table type */     13,
        /* table size */            64,
        /* total data registers */  5,
        /* total field numbers */   RTL9607C_TCAM_RAW_TABLE_PATH1_2FIELD_LIST_END,
        /* table fields */          RTL9607C_TCAM_RAW_TABLE_PATH1_2_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_TCAM_RAW_TABLE_PATH3_5 */
        /* access table type */     13,
        /* table size */            64,
        /* total data registers */  5,
        /* total field numbers */   RTL9607C_TCAM_RAW_TABLE_PATH3_5FIELD_LIST_END,
        /* table fields */          RTL9607C_TCAM_RAW_TABLE_PATH3_5_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_WAN_ACCESS_LIMIT */
        /* access table type */     4,
        /* table size */            32,
        /* total data registers */  1,
        /* total field numbers */   RTL9607C_WAN_ACCESS_LIMITFIELD_LIST_END,
        /* table fields */          RTL9607C_WAN_ACCESS_LIMIT_FIELDS
    },
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_FB_HSBA)
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L34_HSA_NON_PATH5 */
        /* access table type */     1,
        /* table size */            1,
        /* total data registers */  8,
        /* total field numbers */   RTL9607C_L34_HSA_NON_PATH5FIELD_LIST_END,
        /* table fields */          RTL9607C_L34_HSA_NON_PATH5_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L34_HSA_NON_PATH5_2 */
        /* access table type */     1,
        /* table size */            1,
        /* total data registers */  8,
        /* total field numbers */   RTL9607C_L34_HSA_NON_PATH5_2FIELD_LIST_END,
        /* table fields */          RTL9607C_L34_HSA_NON_PATH5_2_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L34_HSA_PATH5 */
        /* access table type */     1,
        /* table size */            1,
        /* total data registers */  8,
        /* total field numbers */   RTL9607C_L34_HSA_PATH5FIELD_LIST_END,
        /* table fields */          RTL9607C_L34_HSA_PATH5_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L34_HSA_PATH5_2 */
        /* access table type */     1,
        /* table size */            1,
        /* total data registers */  8,
        /* total field numbers */   RTL9607C_L34_HSA_PATH5_2FIELD_LIST_END,
        /* table fields */          RTL9607C_L34_HSA_PATH5_2_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L34_HSA_TRAP_DROP */
        /* access table type */     1,
        /* table size */            1,
        /* total data registers */  8,
        /* total field numbers */   RTL9607C_L34_HSA_TRAP_DROPFIELD_LIST_END,
        /* table fields */          RTL9607C_L34_HSA_TRAP_DROP_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L34_HSA_TRAP_DROP_2 */
        /* access table type */     1,
        /* table size */            1,
        /* total data registers */  8,
        /* total field numbers */   RTL9607C_L34_HSA_TRAP_DROP_2FIELD_LIST_END,
        /* table fields */          RTL9607C_L34_HSA_TRAP_DROP_2_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L34_HSB */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  13,
        /* total field numbers */   RTL9607C_L34_HSBFIELD_LIST_END,
        /* table fields */          RTL9607C_L34_HSB_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {   /* table name               INT_RTL9607C_RTL9607C_L34_HSB_2 */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  13,
        /* total field numbers */   RTL9607C_L34_HSB_2FIELD_LIST_END,
        /* table fields */          RTL9607C_L34_HSB_2_FIELDS
    },
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FB_HSBA */
};

#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
rtk_tableName_t rtk_rtl9607c_tableName_list[] =
{
#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER_HSB)
    {"HSB_DATA_TABLE"},
    {"HSB_DATA_TABLE2"},
    {"HSB_PAR_TABLE"},
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER_HSB */
#if defined(CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA)
    {"HSA_DATA_FB"},
    {"HSA_DATA_FB2"},
    {"HSA_DATA_NAT"},
    {"HSA_DATA_NAT2"},
    {"HSA_DATA_NORMAL"},
    {"HSA_DATA_NORMAL2"},
    {"HSA_DATA_OMCI"},
    {"HSA_DATA_OMCI2"},
    {"HSA_DATA_PTP"},
    {"HSA_DATA_PTP2"},
#endif   /* CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA */
#if defined(CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_)
    {"HSA_DEBUG_DATA"},
#endif   /* CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
    {"ACL_ACTION_TABLE"},
    {"ACL_DATA"},
    {"ACL_MASK"},
    {"CF_ACTION_DS"},
    {"CF_ACTION_US"},
    {"CF_MASK_T0"},
    {"CF_MASK_T1"},
    {"CF_MASK_T2"},
    {"CF_RULE_T0"},
    {"CF_RULE_T1"},
    {"CF_RULE_T2"},
    {"L2_MC_DSL"},
    {"L2_UC"},
    {"L3_MC"},
    {"L3_MC_FID"},
    {"L3_MC_VID"},
    {"VLAN"},
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_EPON_CONFIGURATION)
    {"EPON_GRANT"},
#endif   /* CONFIG_SDK_CHIP_FEATURE_EPON_CONFIGURATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
    {"CAM"},
    {"CAM_TAG"},
    {"ETHER_TYPE"},
    {"FB_EXT_PORT"},
    {"FLOW_TABLE_PATH1_2"},
    {"FLOW_TABLE_PATH3_4"},
    {"FLOW_TABLE_PATH5"},
    {"FLOW_TABLE_PATH6"},
    {"FLOW_TABLE_TAG"},
    {"INTERFACE"},
    {"MAC_IDX"},
    {"TCAM"},
    {"TCAM_RAW_TABLE_PATH1_2"},
    {"TCAM_RAW_TABLE_PATH3_5"},
    {"WAN_ACCESS_LIMIT"},
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_FB_HSBA)
    {"L34_HSA_NON_PATH5"},
    {"L34_HSA_NON_PATH5_2"},
    {"L34_HSA_PATH5"},
    {"L34_HSA_PATH5_2"},
    {"L34_HSA_TRAP_DROP"},
    {"L34_HSA_TRAP_DROP_2"},
    {"L34_HSB"},
    {"L34_HSB_2"},
#endif   /* CONFIG_SDK_CHIP_FEATURE_FB_HSBA */
};
#endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
