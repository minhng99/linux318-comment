/*
 * ## Please DO NOT edit this file!! ##
 * This file is auto-generated from the register source files.
 * Any modifications to this file will be LOST when it is re-generated.
 *
 * ----------------------------------------------------------------
 * Copyright(c) Realtek Semiconductor Corporation, 2009
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
#include <hal/chipdef/rtl9602bvb/rtk_rtl9602bvb_table_struct.h>
#include <hal/chipdef/rtl9602bvb/rtk_rtl9602bvb_tableField_list.h>

rtk_table_t rtk_rtl9602bvb_table_list[] =
{
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_HSB_DATA_TABLE */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  28,
        /* total field numbers */   RTL9602BVB_HSB_DATA_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_HSB_DATA_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_HSB_PAR_TABLE */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  2,
        /* total field numbers */   RTL9602BVB_HSB_PAR_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_HSB_PAR_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_HSA_DATA_NATMC */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  13,
        /* total field numbers */   RTL9602BVB_HSA_DATA_NATMCFIELD_LIST_END,
        /* table fields */          RTL9602BVB_HSA_DATA_NATMC_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_HSA_DATA_OMCI */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  13,
        /* total field numbers */   RTL9602BVB_HSA_DATA_OMCIFIELD_LIST_END,
        /* table fields */          RTL9602BVB_HSA_DATA_OMCI_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_HSA_DATA_PTP */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  13,
        /* total field numbers */   RTL9602BVB_HSA_DATA_PTPFIELD_LIST_END,
        /* table fields */          RTL9602BVB_HSA_DATA_PTP_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_HSA_DATA_TABLE */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  13,
        /* total field numbers */   RTL9602BVB_HSA_DATA_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_HSA_DATA_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_HSA_DEBUG_DATA */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  15,
        /* total field numbers */   RTL9602BVB_HSA_DEBUG_DATAFIELD_LIST_END,
        /* table fields */          RTL9602BVB_HSA_DEBUG_DATA_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_ACL_ACTION_TABLE */
        /* access table type */     3,
        /* table size */            96,
        /* total data registers */  3,
        /* total field numbers */   RTL9602BVB_ACL_ACTION_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_ACL_ACTION_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_ACL_DATA */
        /* access table type */     2,
        /* table size */            96,
        /* total data registers */  5,
        /* total field numbers */   RTL9602BVB_ACL_DATAFIELD_LIST_END,
        /* table fields */          RTL9602BVB_ACL_DATA_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_ACL_MASK */
        /* access table type */     2,
        /* table size */            96,
        /* total data registers */  5,
        /* total field numbers */   RTL9602BVB_ACL_MASKFIELD_LIST_END,
        /* table fields */          RTL9602BVB_ACL_MASK_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_CF_ACTION_DS */
        /* access table type */     5,
        /* table size */            256,
        /* total data registers */  3,
        /* total field numbers */   RTL9602BVB_CF_ACTION_DSFIELD_LIST_END,
        /* table fields */          RTL9602BVB_CF_ACTION_DS_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_CF_ACTION_US */
        /* access table type */     5,
        /* table size */            256,
        /* total data registers */  3,
        /* total field numbers */   RTL9602BVB_CF_ACTION_USFIELD_LIST_END,
        /* table fields */          RTL9602BVB_CF_ACTION_US_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_CF_MASK_48_P0 */
        /* access table type */     4,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9602BVB_CF_MASK_48_P0FIELD_LIST_END,
        /* table fields */          RTL9602BVB_CF_MASK_48_P0_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_CF_MASK_48_P1_T0 */
        /* access table type */     4,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9602BVB_CF_MASK_48_P1_T0FIELD_LIST_END,
        /* table fields */          RTL9602BVB_CF_MASK_48_P1_T0_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_CF_MASK_48_P1_T1 */
        /* access table type */     4,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9602BVB_CF_MASK_48_P1_T1FIELD_LIST_END,
        /* table fields */          RTL9602BVB_CF_MASK_48_P1_T1_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_CF_RULE_48_P0 */
        /* access table type */     4,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9602BVB_CF_RULE_48_P0FIELD_LIST_END,
        /* table fields */          RTL9602BVB_CF_RULE_48_P0_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_CF_RULE_48_P1_T0 */
        /* access table type */     4,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9602BVB_CF_RULE_48_P1_T0FIELD_LIST_END,
        /* table fields */          RTL9602BVB_CF_RULE_48_P1_T0_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_CF_RULE_48_P1_T1 */
        /* access table type */     4,
        /* table size */            256,
        /* total data registers */  2,
        /* total field numbers */   RTL9602BVB_CF_RULE_48_P1_T1FIELD_LIST_END,
        /* table fields */          RTL9602BVB_CF_RULE_48_P1_T1_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_L2_MC_DSL */
        /* access table type */     0,
        /* table size */            1088,
        /* total data registers */  3,
        /* total field numbers */   RTL9602BVB_L2_MC_DSLFIELD_LIST_END,
        /* table fields */          RTL9602BVB_L2_MC_DSL_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_L2_UC */
        /* access table type */     0,
        /* table size */            1088,
        /* total data registers */  3,
        /* total field numbers */   RTL9602BVB_L2_UCFIELD_LIST_END,
        /* table fields */          RTL9602BVB_L2_UC_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_L3_IP6MC */
        /* access table type */     0,
        /* table size */            1088,
        /* total data registers */  3,
        /* total field numbers */   RTL9602BVB_L3_IP6MCFIELD_LIST_END,
        /* table fields */          RTL9602BVB_L3_IP6MC_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_L3_MC_ROUTE */
        /* access table type */     0,
        /* table size */            1088,
        /* total data registers */  3,
        /* total field numbers */   RTL9602BVB_L3_MC_ROUTEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_L3_MC_ROUTE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_VLAN */
        /* access table type */     1,
        /* table size */            4096,
        /* total data registers */  1,
        /* total field numbers */   RTL9602BVB_VLANFIELD_LIST_END,
        /* table fields */          RTL9602BVB_VLAN_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_EPON_GRANT */
        /* access table type */     0,
        /* table size */            511,
        /* total data registers */  2,
        /* total field numbers */   RTL9602BVB_EPON_GRANTFIELD_LIST_END,
        /* table fields */          RTL9602BVB_EPON_GRANT_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_ARP_CAM_TABLE */
        /* access table type */     13,
        /* table size */            128,
        /* total data registers */  2,
        /* total field numbers */   RTL9602BVB_ARP_CAM_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_ARP_CAM_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_BINDING_TABLE */
        /* access table type */     6,
        /* table size */            32,
        /* total data registers */  1,
        /* total field numbers */   RTL9602BVB_BINDING_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_BINDING_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_EXTERNAL_IP_TABLE */
        /* access table type */     4,
        /* table size */            8,
        /* total data registers */  3,
        /* total field numbers */   RTL9602BVB_EXTERNAL_IP_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_EXTERNAL_IP_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_FLOW_ROUTING_TABLE_IPV4 */
        /* access table type */     12,
        /* table size */            256,
        /* total data registers */  4,
        /* total field numbers */   RTL9602BVB_FLOW_ROUTING_TABLE_IPV4FIELD_LIST_END,
        /* table fields */          RTL9602BVB_FLOW_ROUTING_TABLE_IPV4_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_FLOW_ROUTING_TABLE_IPV6 */
        /* access table type */     12,
        /* table size */            256,
        /* total data registers */  5,
        /* total field numbers */   RTL9602BVB_FLOW_ROUTING_TABLE_IPV6FIELD_LIST_END,
        /* table fields */          RTL9602BVB_FLOW_ROUTING_TABLE_IPV6_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_FLOW_ROUTING_TABLE_IPV6_EXT */
        /* access table type */     12,
        /* table size */            256,
        /* total data registers */  5,
        /* total field numbers */   RTL9602BVB_FLOW_ROUTING_TABLE_IPV6_EXTFIELD_LIST_END,
        /* table fields */          RTL9602BVB_FLOW_ROUTING_TABLE_IPV6_EXT_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_IPV6_ROUTING_TABLE */
        /* access table type */     5,
        /* table size */            8,
        /* total data registers */  5,
        /* total field numbers */   RTL9602BVB_IPV6_ROUTING_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_IPV6_ROUTING_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_L3_ROUTING_DROP_TRAP */
        /* access table type */     0,
        /* table size */            16,
        /* total data registers */  2,
        /* total field numbers */   RTL9602BVB_L3_ROUTING_DROP_TRAPFIELD_LIST_END,
        /* table fields */          RTL9602BVB_L3_ROUTING_DROP_TRAP_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_L3_ROUTING_GLOBAL_ROUTE */
        /* access table type */     0,
        /* table size */            16,
        /* total data registers */  2,
        /* total field numbers */   RTL9602BVB_L3_ROUTING_GLOBAL_ROUTEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_L3_ROUTING_GLOBAL_ROUTE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_L3_ROUTING_LOCAL_ROUTE */
        /* access table type */     0,
        /* table size */            16,
        /* total data registers */  2,
        /* total field numbers */   RTL9602BVB_L3_ROUTING_LOCAL_ROUTEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_L3_ROUTING_LOCAL_ROUTE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_NAPT_TABLE */
        /* access table type */     10,
        /* table size */            4096,
        /* total data registers */  1,
        /* total field numbers */   RTL9602BVB_NAPT_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_NAPT_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_NAPTR_TABLE */
        /* access table type */     9,
        /* table size */            4096,
        /* total data registers */  3,
        /* total field numbers */   RTL9602BVB_NAPTR_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_NAPTR_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_NEIGHBOR_TABLE */
        /* access table type */     11,
        /* table size */            128,
        /* total data registers */  3,
        /* total field numbers */   RTL9602BVB_NEIGHBOR_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_NEIGHBOR_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_NETIF */
        /* access table type */     3,
        /* table size */            16,
        /* total data registers */  4,
        /* total field numbers */   RTL9602BVB_NETIFFIELD_LIST_END,
        /* table fields */          RTL9602BVB_NETIF_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_NEXT_HOP_TABLE */
        /* access table type */     2,
        /* table size */            16,
        /* total data registers */  1,
        /* total field numbers */   RTL9602BVB_NEXT_HOP_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_NEXT_HOP_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_PPPOE_TABLE */
        /* access table type */     1,
        /* table size */            8,
        /* total data registers */  1,
        /* total field numbers */   RTL9602BVB_PPPOE_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_PPPOE_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_WAN_TYPE_TABLE */
        /* access table type */     7,
        /* table size */            16,
        /* total data registers */  1,
        /* total field numbers */   RTL9602BVB_WAN_TYPE_TABLEFIELD_LIST_END,
        /* table fields */          RTL9602BVB_WAN_TYPE_TABLE_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_L34_HSA */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  5,
        /* total field numbers */   RTL9602BVB_L34_HSAFIELD_LIST_END,
        /* table fields */          RTL9602BVB_L34_HSA_FIELDS
    },
#endif
#if defined(CONFIG_SDK_RTL9602BVB)
    {   /* table name               INT_RTL9602BVB_RTL9602BVB_L34_HSB */
        /* access table type */     0,
        /* table size */            1,
        /* total data registers */  14,
        /* total field numbers */   RTL9602BVB_L34_HSBFIELD_LIST_END,
        /* table fields */          RTL9602BVB_L34_HSB_FIELDS
    },
#endif
};

#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
rtk_tableName_t rtk_rtl9602bvb_tableName_list[] =
{
    {"HSB_DATA_TABLE"},
    {"HSB_PAR_TABLE"},
    {"HSA_DATA_NATMC"},
    {"HSA_DATA_OMCI"},
    {"HSA_DATA_PTP"},
    {"HSA_DATA_TABLE"},
    {"HSA_DEBUG_DATA"},
    {"ACL_ACTION_TABLE"},
    {"ACL_DATA"},
    {"ACL_MASK"},
    {"CF_ACTION_DS"},
    {"CF_ACTION_US"},
    {"CF_MASK_48_P0"},
    {"CF_MASK_48_P1_T0"},
    {"CF_MASK_48_P1_T1"},
    {"CF_RULE_48_P0"},
    {"CF_RULE_48_P1_T0"},
    {"CF_RULE_48_P1_T1"},
    {"L2_MC_DSL"},
    {"L2_UC"},
    {"L3_IP6MC"},
    {"L3_MC_ROUTE"},
    {"VLAN"},
    {"EPON_GRANT"},
    {"ARP_CAM_TABLE"},
    {"BINDING_TABLE"},
    {"EXTERNAL_IP_TABLE"},
    {"FLOW_ROUTING_TABLE_IPV4"},
    {"FLOW_ROUTING_TABLE_IPV6"},
    {"FLOW_ROUTING_TABLE_IPV6_EXT"},
    {"IPV6_ROUTING_TABLE"},
    {"L3_ROUTING_DROP_TRAP"},
    {"L3_ROUTING_GLOBAL_ROUTE"},
    {"L3_ROUTING_LOCAL_ROUTE"},
    {"NAPT_TABLE"},
    {"NAPTR_TABLE"},
    {"NEIGHBOR_TABLE"},
    {"NETIF"},
    {"NEXT_HOP_TABLE"},
    {"PPPOE_TABLE"},
    {"WAN_TYPE_TABLE"},
    {"L34_HSA"},
    {"L34_HSB"},
};
#endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
