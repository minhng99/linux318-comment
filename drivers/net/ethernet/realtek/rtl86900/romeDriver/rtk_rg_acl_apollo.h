#ifndef RTK_RG_ACL_APOLLO_H
#define RTK_RG_ACL_APOLLO_H

#include <common/type.h>
#include <common/rt_type.h>

//Apollo Platform
#define MIN_ACL_ENTRY_INDEX 	RESERVED_ACL_BEFORE
#define MAX_ACL_ENTRY_INDEX 	RESERVED_ACL_AFTER

#define MAX_ACL_SW_ENTRY_SIZE 180

#define MAX_ACL_ENTRY_SIZE 	64
#define MAX_ACL_IPRANGETABLE_SIZE 	8
#define MAX_ACL_PORTRANGETABLE_SIZE 16
#define MAX_ACL_TEMPLATE_SIZE 4 //this size also limit the acl multiple hit size(one aclSWEntry at most mapping to 4 acl ASIC entry)
#define MIN_CF_64TO511_ENTRY_SIZE 	64
#define MAX_CF_64TO511_ENTRY_SIZE 	511
#define MAX_CF0To63_ENTRY_SIZE 	64
#define MAX_CF_IPRANGETABLE_SIZE 	8
#define MAX_CF_PORTRANGETABLE_SIZE  8
#define MAX_CF_DSCPTABLE_SIZE  8
#define CF_FOR_L2_FLOW_INDEX_START 64
#define CF_FOR_L2_FLOW_INDEX_END 511
#define TOTAL_CF_ENTRY_SIZE 	512
#define CF_UNI_DEFAULT 0x7	//if flood or multicast, uni default is 0b111(uni 3 bit)

#define RESERVED_ACL_BEFORE rg_db.systemGlobal.aclAndCfReservedRule.aclLowerBoundary
#define RESERVED_ACL_AFTER rg_db.systemGlobal.aclAndCfReservedRule.aclUpperBoundary

#define RESERVED_CF_BEFORE 	rg_db.systemGlobal.aclAndCfReservedRule.cfLowerBoundary
#define RESERVED_CF_AFTER 	rg_db.systemGlobal.aclAndCfReservedRule.cfUpperBoundary


#define PURE_ACL_PATTERN_BITS \
(INGRESS_PORT_BIT|INGRESS_INTF_BIT|INGRESS_ETHERTYPE_BIT|INGRESS_CTAG_PRI_BIT|INGRESS_CTAG_VID_BIT|INGRESS_SMAC_BIT \
|INGRESS_DMAC_BIT|INGRESS_DSCP_BIT|INGRESS_L4_TCP_BIT|INGRESS_L4_UDP_BIT|INGRESS_IPV6_SIP_RANGE_BIT|INGRESS_IPV6_DIP_RANGE_BIT \
|INGRESS_IPV4_SIP_RANGE_BIT|INGRESS_IPV4_DIP_RANGE_BIT|INGRESS_L4_SPORT_RANGE_BIT|INGRESS_L4_DPORT_RANGE_BIT \
|INGRESS_L4_ICMP_BIT|INGRESS_IPV6_DSCP_BIT|INGRESS_STREAM_ID_BIT|INGRESS_STAG_PRI_BIT|INGRESS_STAG_VID_BIT|INGRESS_STAGIF_BIT|INGRESS_CTAGIF_BIT \
|INGRESS_L4_POROTCAL_VALUE_BIT|INGRESS_TOS_BIT|INGRESS_IPV6_TC_BIT|INGRESS_IPV6_SIP_BIT|INGRESS_IPV6_DIP_BIT|INGRESS_WLANDEV_BIT|INGRESS_IPV4_TAGIF_BIT|INGRESS_IPV6_TAGIF_BIT \
|INGRESS_CTAG_CFI_BIT|INGRESS_STAG_DEI_BIT)


#define PURE_CF_PATTERN_BITS \
(EGRESS_INTF_BIT|EGRESS_IPV4_SIP_RANGE_BIT|EGRESS_IPV4_DIP_RANGE_BIT \
|EGRESS_L4_SPORT_RANGE_BIT|EGRESS_L4_DPORT_RANGE_BIT|EGRESS_CTAG_PRI_BIT|EGRESS_CTAG_VID_BIT|INGRESS_EGRESS_PORTIDX_BIT|INTERNAL_PRI_BIT)


//internal 
typedef enum rtk_rg_aclField_Teamplate_s
{
	//related to Template[0]
	TEMPLATE_DMAC0 			=0,
	TEMPLATE_DMAC1 			=1,
	TEMPLATE_DMAC2 			=2,
	TEMPLATE_EXTPORTMASK	=3, 
	TEMPLATE_SMAC0 			=4,
	TEMPLATE_SMAC1 			=5,
	TEMPLATE_SMAC2 			=6,
	TEMPLATE_ETHERTYPE 		=7,
	//related to Template[1]
	TEMPLATE_FIELDSELECT00	=8,//l4_dst_port
	TEMPLATE_IPv4SIP0 		=9,
	TEMPLATE_IPv4SIP1		=10,
	TEMPLATE_FIELDSELECT01	=11, //l4_src_port
	TEMPLATE_FIELDSELECT14	=12,//IP protocal
	TEMPLATE_PORTRANGE 		=13,
	TEMPLATE_IPv4DIP0 		=14,
	TEMPLATE_IPv4DIP1 		=15,	
	//related to Template[2]
	TEMPLATE_CTAG	 		=16, 
	TEMPLATE_GEMPORT 		=17, //genport means ingress_stream_id
	TEMPLATE_STAG			=18, 
	TEMPLATE_FIELDSELECT03 	=19, //SIPv6[128:112] or DIPv6[128:112] or two byte after ETHERTYPE(for EPON)  => could be change to pktLen by reserved ACL
	TEMPLATE_FIELDSELECT04 	=20, //DSCP
#ifdef CONFIG_RG_ACL_V6SIP_FILTER_BUT_DISABLE_V6ROUTING
	TEMPLATE_IPv6DIP1		=21, //DIPv6[31:16]
	TEMPLATE_IPv6DIP0		=22, //DIPv6[15:0]
#else
	TEMPLATE_IPv6SIP1 		=21, //SIPv6[31:16]
	TEMPLATE_IPv6SIP0 		=22, //SIPv6[15:0]
#endif
	TEMPLATE_FIELDSELECT15 	=23, //SessionID or DSCPv6
	//related to Template[3]
	TEMPLATE_FIELDSELECT08 	=24, //DIPv6[47:32]
	TEMPLATE_FIELDSELECT09 	=25, //DIPv6[63:48]
	TEMPLATE_FIELDSELECT10	=26, //DIPv6[79:64]
	TEMPLATE_FIELDSELECT11 	=27, //DIPv6[95:80]
	TEMPLATE_FIELDSELECT12 	=28, //DIPv6[111:96]
	TEMPLATE_FIELDSELECT13 	=29, //DIPv6[127:112]
#ifdef CONFIG_RG_ACL_V6SIP_FILTER_BUT_DISABLE_V6ROUTING
	TEMPLATE_IPv6SIP1		=30, //SIPv6[31:16]
	TEMPLATE_IPv6SIP0		=31, //SIPv6[15:0]
#else
	TEMPLATE_IPv6DIP1		=30, //DIPv6[31:16]
	TEMPLATE_IPv6DIP0 		=31, //DIPv6[15:0]
#endif
	TEMPLATE_END
}rtk_rg_aclField_Teamplate_t;

typedef enum rtk_rg_aclSWEntry_used_tables_field_s
{
	ACL_USED_IPTABLE_IPV4SIP = 0x1,
	ACL_USED_IPTABLE_IPV4DIP = 0x2,
	ACL_USED_IPTABLE_IPV6SIP = 0x4,
	ACL_USED_IPTABLE_IPV6DIP = 0x8,
	ACL_USED_PORTTABLE_SPORT = 0x10,
	ACL_USED_PORTTABLE_DPORT = 0x20,
	CF_USED_IPTABLE_IPV4SIP = 0x40,
	CF_USED_IPTABLE_IPV4DIP = 0x80,
	CF_USED_PORTTABLE_SPORT = 0x100,
	CF_USED_PORTTABLE_DPORT = 0x200,
	CF_USED_DSCPTABLE = 0x400,
} rtk_rg_aclSWEntry_used_tables_field_t;

typedef enum rtk_rg_aclSWEntry_used_tables_index_s
{
	ACL_USED_IPTABLE_IPV4SIP_INDEX = 0,
	ACL_USED_IPTABLE_IPV4DIP_INDEX ,
	ACL_USED_IPTABLE_IPV6SIP_INDEX ,
	ACL_USED_IPTABLE_IPV6DIP_INDEX ,
	ACL_USED_PORTTABLE_SPORT_INDEX ,
	ACL_USED_PORTTABLE_DPORT_INDEX ,
	CF_USED_IPTABLE_IPV4SIP_INDEX ,
	CF_USED_IPTABLE_IPV4DIP_INDEX ,
	CF_USED_PORTTABLE_SPORT_INDEX ,
	CF_USED_PORTTABLE_DPORT_INDEX ,
	CF_USED_DSCPTABLE_INDEX,
	USED_TABLE_END
} rtk_rg_aclSWEntry_used_tables_index_t;

typedef enum rtk_rg_aclSWEntry_type_s
{
	UNUSE = 0,
	ACL_USE,
	QOS_USE,
	OTHERS_USE,
} rtk_rg_aclSWEntry_type_t;



//ACL Reserved 
typedef enum rtk_rg_aclAndCf_reserved_type_e
{

	/*HEAD PATCH: The lower index number, the higher acl&cf priority*/
	RTK_RG_ACLANDCF_RESERVED_GPON_SMALL_BANDWIDTH_CONTROL=0,
	RTK_RG_ACLANDCF_RESERVED_EPON_DROP_AND_INTERRUPT,
	RTK_RG_ACLANDCF_RESERVED_EPON_ASSIGN_PRIORITY,
	RTK_RG_ACLANDCF_RESERVED_ACL_SKIP_HW_REARRANGE_PERMIT_AND_TRAP_RESERV,
	RTK_RG_ACLANDCF_RESERVED_INTF0_DHCP_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF1_DHCP_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF2_DHCP_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF3_DHCP_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF4_DHCP_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF5_DHCP_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF6_DHCP_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF7_DHCP_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_WIFI_DATAPATH_MASTER2SLAVE_PATCH, //Dual wifi reserved especial vid, pri for datapath(Master=>slave), we translate vid to 1 for avoiding vlan filter & pri to special slave queue.
	RTK_RG_ACLANDCF_RESERVED_STPBLOCKING,
	RTK_RG_ACLANDCF_RESERVED_MULTICAST_RMA_TRAP,
	RTK_RG_ACLANDCF_RESERVED_MULTICAST_SSDP_TRAP,
	RTK_RG_ACLANDCF_RESERVED_WIFI2_IPC_ROUTING_TRAP,
	RTK_RG_ACLANDCF_RESERVED_BROADCAST_TRAP,
	RTK_RG_ACLANDCF_RESERVED_ALL_TRAP,
	RTK_RG_ACLANDCF_RESERVED_UNICAST_TRAP,
	RTK_RG_ACLANDCF_RESERVED_EXT1_SLAVE_WIFI_TRAP,
	RTK_RG_ACLANDCF_RESERVED_SYN_PACKET_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_ACK_PACKET_ASSIGN_PRIORITY,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_LCP_PACKET_ASSIGN_PRIORITY,
	RTK_RG_ACLANDCF_RESERVED_L2TP_CONTROL_LCP_PACKET_TRAP_AND_ASSIGN_PRIORITY,
	RTK_RG_ACLANDCF_RESERVED_SLAVE_WIFI_BC_MC_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF0_DSLITE_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF1_DSLITE_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF2_DSLITE_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF3_DSLITE_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF4_DSLITE_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF5_DSLITE_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF6_DSLITE_TRAP,
	RTK_RG_ACLANDCF_RESERVED_INTF7_DSLITE_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_MULTICAST_PPPOE_RULE0_TRAP,
	RTK_RG_ACLANDCF_RESERVED_MULTICAST_PPPOE_RULE1_TRAP,
	RTK_RG_ACLANDCF_RESERVED_MULTICAST_PPPOE_RULE2_TRAP,
	RTK_RG_ACLANDCF_RESERVED_MULTICAST_PPPOE_RULE3_TRAP,
	RTK_RG_ACLANDCF_RESERVED_MULTICAST_PPPOE_RULE4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_MULTICAST_PPPOE_RULE5_TRAP,
	RTK_RG_ACLANDCF_RESERVED_MULTICAST_PPPOE_RULE6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_MULTICAST_PPPOE_RULE7_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PPPOE_INTF0_MC_ROUTING_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PPPOE_INTF1_MC_ROUTING_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PPPOE_INTF2_MC_ROUTING_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PPPOE_INTF3_MC_ROUTING_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PPPOE_INTF4_MC_ROUTING_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PPPOE_INTF5_MC_ROUTING_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PPPOE_INTF6_MC_ROUTING_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PPPOE_INTF7_MC_ROUTING_TRAP,
	RTK_RG_ACLANDCF_RESERVED_IPV6_INTF0_LINK_LOCAL_TRAP,
	RTK_RG_ACLANDCF_RESERVED_IPV6_INTF1_LINK_LOCAL_TRAP,
	RTK_RG_ACLANDCF_RESERVED_IPV6_INTF2_LINK_LOCAL_TRAP,
	RTK_RG_ACLANDCF_RESERVED_IPV6_INTF3_LINK_LOCAL_TRAP,
	RTK_RG_ACLANDCF_RESERVED_IPV6_INTF4_LINK_LOCAL_TRAP,
	RTK_RG_ACLANDCF_RESERVED_IPV6_INTF5_LINK_LOCAL_TRAP,
	RTK_RG_ACLANDCF_RESERVED_IPV6_INTF6_LINK_LOCAL_TRAP,
	RTK_RG_ACLANDCF_RESERVED_IPV6_INTF7_LINK_LOCAL_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE0_DIP_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE1_DIP_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE2_DIP_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE3_DIP_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE4_DIP_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE5_DIP_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE6_DIP_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE7_DIP_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE8_DIP_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE9_DIP_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE10_DIP_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE11_DIP_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE12_DIP_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE13_DIP_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE14_DIP_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE15_DIP_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE0_DIPv6_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE1_DIPv6_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE2_DIPv6_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE3_DIPv6_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE4_DIPv6_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE5_DIPv6_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE6_DIPv6_MASK_TRAP,
	RTK_RG_ACLANDCF_RESERVED_RULE7_DIPv6_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE8_DIPv6_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE9_DIPv6_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE10_DIPv6_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE11_DIPv6_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE12_DIPv6_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE13_DIPv6_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE14_DIPv6_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_RULE15_DIPv6_MASK_TRAP,	
	RTK_RG_ACLANDCF_RESERVED_PORT0_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PORT1_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PORT2_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PORT3_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PORT4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_PORT5_TRAP,
	
	RTK_RG_ACLANDCF_RESERVED_VLANBIND0_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND1_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND2_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND3_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND5_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND7_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND8_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND9_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND10_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND11_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND12_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND13_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND14_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND15_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND16_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND17_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND18_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND19_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND20_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND21_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND22_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND23_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND24_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND25_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND26_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND27_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND28_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND29_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND30_TRAP,
	RTK_RG_ACLANDCF_RESERVED_VLANBIND31_TRAP,

	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF0_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF1_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF2_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF3_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF4_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF5_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF6_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF7_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF8_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF9_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF10_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF11_PERMIT,	
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF12_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF13_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF14_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_INTF15_PERMIT,
	RTK_RG_ACLANDCF_RESERVED_PPPoE_MULTICAST_DEFAULT_TRAP,
	RTK_RG_ACLANDCF_RESERVED_BRIDGE_UPSTREAM_HTTP_TRAP,

	RTK_RG_ACLANDCF_RESERVED_NETIF0_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF1_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF2_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF3_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF4_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF5_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF6_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF7_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF8_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF9_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF10_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF11_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF12_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF13_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF14_GATEWAY_IPv4_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF15_GATEWAY_IPv4_TRAP,
		
	RTK_RG_ACLANDCF_RESERVED_NETIF0_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF1_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF2_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF3_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF4_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF5_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF6_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF7_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF8_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF9_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF10_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF11_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF12_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF13_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF14_GATEWAY_IPv6_TRAP,
	RTK_RG_ACLANDCF_RESERVED_NETIF15_GATEWAY_IPv6_TRAP,

	RTK_RG_ACLANDCF_RESERVED_IGMP_MLD_DROP, //pactched MLD drop behavior by IPv6 address.
	
	RTK_RG_ACLANDCF_RESERVED_HEAD_END,

	/*TAIL PATCH: The lower index number, the lower acl&cf priority*/

	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF0_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF1_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF2_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF3_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF4_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF5_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF6_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF7_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF8_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF9_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF10_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF11_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF12_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF13_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF14_REMOTE_MAC,
	RTK_RG_ACLANDCF_RESERVED_GPON_PPPoE_SMALL_BANDWIDTH_BRIDGE_DMAC_SAME_AS_INTF15_REMOTE_MAC,


#if 0
    RTK_RG_ACLANDCF_RESERVED_URLFILTER_TRAP,
#endif
    RTK_RG_ACLANDCF_RESERVED_IGMP_TO_SLAVE_WIFI_BLOCK,
    RTK_RG_ACLANDCF_RESERVED_MULTICAST_TRAP_AND_GLOBAL_SCOPE_PERMIT,
    RTK_RG_ACLANDCF_RESERVED_MULTICAST_VID_TRANSLATE_FOR_IPV6_PASSTHROUGHT,
#if 0
    RTK_RG_ACLANDCF_RESERVED_MULTICAST_VID_TRANSLATE_FOR_IPV6,
    RTK_RG_ACLANDCF_RESERVED_MULTICAST_VID_TRANSLATE_FOR_IPV4,
#endif
	RTK_RG_ACLANDCF_RESERVED_PPPoEPASSTHROUGHT_TRAP_ALL8863_US8864_DMAC2CVID_DS8864,
	RTK_RG_ACLANDCF_RESERVED_PPPoEPASSTHROUGHT_INTF7ISPPPOEWAN, //pppoe packet to intf7 avoid hit RTK_RG_ACL_RESERVED_PPPoEPASSTHROUGHT_REMARKIN_BY_DMAC2CVID_DOWNSTREAM_ETHTER_8864
	RTK_RG_ACLANDCF_RESERVED_PPPoEPASSTHROUGHT_INTF6ISPPPOEWAN, //pppoe packet to intf6 avoid hit RTK_RG_ACL_RESERVED_PPPoEPASSTHROUGHT_REMARKIN_BY_DMAC2CVID_DOWNSTREAM_ETHTER_8864
	RTK_RG_ACLANDCF_RESERVED_PPPoEPASSTHROUGHT_INTF5ISPPPOEWAN, //pppoe packet to intf5 avoid hit RTK_RG_ACL_RESERVED_PPPoEPASSTHROUGHT_REMARKIN_BY_DMAC2CVID_DOWNSTREAM_ETHTER_8864
	RTK_RG_ACLANDCF_RESERVED_PPPoEPASSTHROUGHT_INTF4ISPPPOEWAN, //pppoe packet to intf4 avoid hit RTK_RG_ACL_RESERVED_PPPoEPASSTHROUGHT_REMARKIN_BY_DMAC2CVID_DOWNSTREAM_ETHTER_8864
	RTK_RG_ACLANDCF_RESERVED_PPPoEPASSTHROUGHT_INTF3ISPPPOEWAN, //pppoe packet to intf3 avoid hit RTK_RG_ACL_RESERVED_PPPoEPASSTHROUGHT_REMARKIN_BY_DMAC2CVID_DOWNSTREAM_ETHTER_8864
	RTK_RG_ACLANDCF_RESERVED_PPPoEPASSTHROUGHT_INTF2ISPPPOEWAN, //pppoe packet to intf2 avoid hit RTK_RG_ACL_RESERVED_PPPoEPASSTHROUGHT_REMARKIN_BY_DMAC2CVID_DOWNSTREAM_ETHTER_8864
	RTK_RG_ACLANDCF_RESERVED_PPPoEPASSTHROUGHT_INTF1ISPPPOEWAN, //pppoe packet to intf1 avoid hit RTK_RG_ACL_RESERVED_PPPoEPASSTHROUGHT_REMARKIN_BY_DMAC2CVID_DOWNSTREAM_ETHTER_8864
	RTK_RG_ACLANDCF_RESERVED_PPPoEPASSTHROUGHT_INTF0ISPPPOEWAN, //pppoe packet to intf0 avoid hit RTK_RG_ACL_RESERVED_PPPoEPASSTHROUGHT_REMARKIN_BY_DMAC2CVID_DOWNSTREAM_ETHTER_8864

    RTK_RG_ACLANDCF_RESERVED_WIFI_MASTER_EXTPORT_PATCH,//replace src extPort[0](from master) information to egress extPortMask
    RTK_RG_ACLANDCF_RESERVED_WIFI_SLAVE_EXTPORT_PATCH,//replace src extPort[1](from slave) information to egress extPortMask
//#ifdef CONFIG_RG_PPPOE_AND_VALN_ISSUE_PATCH
    RTK_RG_ACLANDCF_RESERVED_PPPoECVIDISSUE_SVIDCOPY2CVID_PATCH, //Wan:PPPoE+Vlan Lan:link-up 100Mbps  have parsing bug=> look 0x8100 as stag in wan port, and copy stag to ctag
//#endif
//#ifdef CONFIG_RG_PPPOE_AND_VALN_ISSUE_PATCH
    RTK_RG_ACLANDCF_RESERVED_PPPoECVIDISSUE_QOSSPRI7REMAPTOINTERNALPRI_PATCH, //Qos remarking pacth for using Stag (SPri[7]=based on Qos API=>internal-priority)
    RTK_RG_ACLANDCF_RESERVED_PPPoECVIDISSUE_QOSSPRI6REMAPTOINTERNALPRI_PATCH, //Qos remarking pacth for using Stag (SPri[6]=based on Qos API=>internal-priority)
    RTK_RG_ACLANDCF_RESERVED_PPPoECVIDISSUE_QOSSPRI5REMAPTOINTERNALPRI_PATCH, //Qos remarking pacth for using Stag (SPri[5]=based on Qos API=>internal-priority)
    RTK_RG_ACLANDCF_RESERVED_PPPoECVIDISSUE_QOSSPRI4REMAPTOINTERNALPRI_PATCH, //Qos remarking pacth for using Stag (SPri[4]=based on Qos API=>internal-priority)
    RTK_RG_ACLANDCF_RESERVED_PPPoECVIDISSUE_QOSSPRI3REMAPTOINTERNALPRI_PATCH, //Qos remarking pacth for using Stag (SPri[3]=based on Qos API=>internal-priority)
    RTK_RG_ACLANDCF_RESERVED_PPPoECVIDISSUE_QOSSPRI2REMAPTOINTERNALPRI_PATCH, //Qos remarking pacth for using Stag (SPri[2]=based on Qos API=>internal-priority)
    RTK_RG_ACLANDCF_RESERVED_PPPoECVIDISSUE_QOSSPRI1REMAPTOINTERNALPRI_PATCH, //Qos remarking pacth for using Stag (SPri[1]=based on Qos API=>internal-priority)
    RTK_RG_ACLANDCF_RESERVED_PPPoECVIDISSUE_QOSSPRI0REMAPTOINTERNALPRI_PATCH, //Qos remarking pacth for using Stag (SPri[0]=based on Qos API=>internal-priority)
//#endif
#if 0
	RTK_RG_ACLANDCF_RESERVED_PON_INTFDEFAULTSSIDREMAP_PATCH, 
	RTK_RG_ACLANDCF_RESERVED_PON_INTF7SSIDREMAP_PATCH, 
	RTK_RG_ACLANDCF_RESERVED_PON_INTF6SSIDREMAP_PATCH, 
	RTK_RG_ACLANDCF_RESERVED_PON_INTF5SSIDREMAP_PATCH, 
	RTK_RG_ACLANDCF_RESERVED_PON_INTF4SSIDREMAP_PATCH, 
	RTK_RG_ACLANDCF_RESERVED_PON_INTF3SSIDREMAP_PATCH, 
	RTK_RG_ACLANDCF_RESERVED_PON_INTF2SSIDREMAP_PATCH, 
	RTK_RG_ACLANDCF_RESERVED_PON_INTF1SSIDREMAP_PATCH, 
	RTK_RG_ACLANDCF_RESERVED_PON_INTF0SSIDREMAP_PATCH, //FIXME: bits not enough
#endif
	RTK_RG_ACLANDCF_RESERVED_ASSIGN_INGRESS_CVLAN_FOR_SERVICE_PORT0_WITHOUT_FILTER,
	RTK_RG_ACLANDCF_RESERVED_ASSIGN_INGRESS_CVLAN_FOR_SERVICE_PORT1_WITHOUT_FILTER,
	RTK_RG_ACLANDCF_RESERVED_ASSIGN_INGRESS_CVLAN_FOR_SERVICE_PORT2_WITHOUT_FILTER,
	RTK_RG_ACLANDCF_RESERVED_ASSIGN_INGRESS_CVLAN_FOR_SERVICE_PORT3_WITHOUT_FILTER,
	RTK_RG_ACLANDCF_RESERVED_ASSIGN_INGRESS_CVLAN_FOR_SERVICE_PORT4_WITHOUT_FILTER,
	RTK_RG_ACLANDCF_RESERVED_ASSIGN_INGRESS_CVLAN_FOR_SERVICE_PORT5_WITHOUT_FILTER,
	RTK_RG_ACLANDCF_RESERVED_ASSIGN_INGRESS_CVLAN_FOR_SERVICE_PORT6_WITHOUT_FILTER,
    RTK_RG_ACLANDCF_RESERVED_TAIL_END,
}rtk_rg_aclAndCf_reserved_type_t;

typedef struct rtk_rg_aclAndCf_reserved_l2tp_control_lcp_trap_and_assign_priority_s{
	int priority;
}rtk_rg_aclAndCf_reserved_l2tp_control_lcp_trap_and_assign_priority_t;


typedef struct rtk_rg_aclAndCf_reserved_WifiMasterExtportPatch_s
{
	int igrPmsk;
	int egrPmsk;
}rtk_rg_aclAndCf_reserved_WifiMasterExtportPatch_t;

typedef struct rtk_rg_aclAndCf_reserved_WifiSlaveExtportPatch_s
{
	int igrPmsk;
	int egrPmsk;
}rtk_rg_aclAndCf_reserved_WifiSlaveExtportPatch_t;

typedef struct rtk_rg_aclAndCf_reserved_WifiDatapathMaster2SlavePatch_s
{
	uint32 igrCvid;
	uint32 igrCpri;
	uint32 tranCvid;
	uint32 tranCpri;
}rtk_rg_aclAndCf_reserved_WifiDatapathMaster2SlavePatch_t;


//#ifdef CONFIG_RG_PPPOE_AND_VALN_ISSUE_PATCH
typedef struct rtk_rg_aclAndCf_reserved_pppoeCvidIssueSvidCopy2CvidPatch_s
{
	uint32 wanPort;
}rtk_rg_aclAndCf_reserved_pppoeCvidIssueSvidCopy2CvidPatch_t;

typedef struct rtk_rg_aclAndCf_reserved_pppoeCvidIssueSpriRemap2InternalPriPatch_s
{
	int wanPort;
	int spri;
	int intpri;
}rtk_rg_aclAndCf_reserved_pppoeCvidIssueSpriRemap2InternalPriPatch_t;
//#endif

typedef struct rtk_rg_aclAndCf_reserved_pppoe_lcp_assign_priority_s{
	int priority;
}rtk_rg_aclAndCf_reserved_pppoe_lcp_assign_priority_t;

typedef struct rtk_rg_aclAndCf_reserved_pppoepassthroughtIntfIsPppoewanPatch_s
{
	rtk_mac_t gmac;
}rtk_rg_aclAndCf_reserved_pppoepassthroughtIntfIsPppoewanPatch_t;


typedef struct rtk_rg_aclAndCf_reserved_pppoepassthroughtDefaulRuletPatch_s
{
	uint32 lanPmsk;
	uint32 wanPmsk;
	uint32 remarkVid;
}rtk_rg_aclAndCf_reserved_pppoepassthroughtDefaulTrapRuletPatch_t;


typedef struct rtk_rg_aclAndCf_reserved_ponIntfSsidRemapPatch_s
{
	int intfIdx;
	int ssid;
	int vid;
}rtk_rg_aclAndCf_reserved_ponIntfSsidRemapPatch_t;

typedef struct rtk_rg_aclAndCf_reserved_multicast_vid_translate_for_ipv4_s{
	int vid;
}rtk_rg_aclAndCf_reserved_multicastVidTranslateForIpv4_t;

typedef struct rtk_rg_aclAndCf_reserved_multicast_vid_translate_for_ipv6_s{
	int vid;
}rtk_rg_aclAndCf_reserved_multicastVidTranslateForIpv6_t;

typedef struct rtk_rg_aclAndCf_reserved_AssignCvlanForServicePortWithoutFilter_s{
	int service_port; //physical port which enabled service port ability.
	int assigned_vid;
}rtk_rg_aclAndCf_reserved_AssignCvlanForServicePortWithoutFilter_t;

typedef struct rtk_rg_aclAndCf_reserved_intf_dhcp_trap_s{
	rtk_mac_t gmac;
}rtk_rg_aclAndCf_reserved_intf_dhcp_trap_t;

typedef struct rtk_rg_aclAndCf_reserved_intf_dslite_trap_s{
	rtk_mac_t smac;
	rtk_ipv6_addr_t ipv6_dip;
}rtk_rg_aclAndCf_reserved_intf_dslite_trap_t;

typedef struct rtk_rg_aclAndCf_reserved_pppoe_intf_multicast_routing_trap_s{
	rtk_mac_t gmac;
}rtk_rg_aclAndCf_reserved_pppoe_intf_multicast_routing_trap_t;

typedef struct rtk_rg_aclAndCf_reserved_intf_linkLocal_trap_s{
	rtk_mac_t gmac;
}rtk_rg_aclAndCf_reserved_intf_linkLocal_trap_t;

typedef struct rtk_rg_aclAndCf_reserved_ack_packet_assign_priority_s{
	int priority;
}rtk_rg_aclAndCf_reserved_ack_packet_assign_priority_t;

typedef struct rtk_rg_aclAndCf_reserved_dip_mask_trap_s{
	uint32 dip;
	uint32 mask;
}rtk_rg_aclAndCf_reserved_dip_mask_trap_t;

typedef struct rtk_rg_aclAndCf_reserved_dipv6_mask_trap_s{
	rtk_ipv6_addr_t dipv6;
	rtk_ipv6_addr_t dipv6_mask;
}rtk_rg_aclAndCf_reserved_dipv6_mask_trap_t;

typedef struct rtk_rg_aclAndCf_reserved_vlan_bind_trap_s{
	uint32 portmask;
	uint32 vid;
}rtk_rg_aclAndCf_reserved_vlan_bind_trap_t;

typedef struct rtk_rg_aclAndCf_reserved_pppoe_multicast_intf_permit_s{
	rtk_mac_t gmac;
}rtk_rg_aclAndCf_reserved_pppoe_multicast_intf_permit_t;

typedef struct rtk_rg_aclAndCf_reserved_gpon_smallbw_bridge_port_redirect_s{
	rtk_mac_t remote_mac;
}rtk_rg_aclAndCf_reserved_gpon_smallbw_bridge_port_redirect_t;

typedef struct rtk_rg_aclAndCf_reserved_igmp_mld_drop_s{
	uint32 portmask;
}rtk_rg_aclAndCf_reserved_igmp_mld_drop_t;


typedef struct rtk_rg_aclAndCf_multicast_pppoe_trap_s{
	rtk_mac_t dmac;
	uint32 dip;
}rtk_rg_aclAndCf_multicast_pppoe_trap_t;


typedef struct rtk_rg_aclAndCf_reserved_global_s
{
	uint32 aclLowerBoundary; //The End index of acl reserve rules in the head
	uint32 aclUpperBoundary; //The start index of acl reserve rules in the tail
	uint32 cfLowerBoundary;  //The start index of cf reserve rules in the head
	uint32 cfUpperBoundary;  //The start index of cf reserve rules in the tail
	uint8 reservedMask[RTK_RG_ACLANDCF_RESERVED_TAIL_END];
	//rtk_rg_aclAndCf_reserved_type_t reservedMask;

	uint32 acl_MC_temp_permit_idx; //this index is reserved for user ACL rearrange, and reserved by type RTK_RG_ACLANDCF_RESERVED_BROADCAST_TRAP or RTK_RG_ACLANDCF_RESERVED_ALL_TRAP
	
	int aclSkipRearrangeIPv4MCPermitIdx;//rsv HW ACL index for multicast permit, used when aclSkipRearrangeHWAclCf==1
	int aclSkipRearrangeIPv6MCPermitIdx;//rsv HW ACL index for multicast permit, used when aclSkipRearrangeHWAclCf==1
	int aclSkipRearrangeAlltTrapIdx;//rsv HW ACL index for rest all trap, used when aclSkipRearrangeHWAclCf==1


	rtk_rg_aclAndCf_reserved_WifiMasterExtportPatch_t 	wifiMasterExtportPatchPara;
	rtk_rg_aclAndCf_reserved_WifiSlaveExtportPatch_t 	wifiSlaveExtportPatchPara;
	rtk_rg_aclAndCf_reserved_WifiDatapathMaster2SlavePatch_t	wifiDadapathM2SPatchPara;
//#ifdef CONFIG_RG_PPPOE_AND_VALN_ISSUE_PATCH
	rtk_rg_aclAndCf_reserved_pppoeCvidIssueSvidCopy2CvidPatch_t 	pppoeCvidIssueSvid2CvidPatchPara;
	rtk_rg_aclAndCf_reserved_pppoeCvidIssueSpriRemap2InternalPriPatch_t pppoeCvidIssueSpriRemap2IntpriPara[8]; //each array stand for one spri.
//#endif
	rtk_rg_aclAndCf_reserved_pppoe_lcp_assign_priority_t pppoe_lcp_assign_prioity;
	rtk_rg_aclAndCf_reserved_l2tp_control_lcp_trap_and_assign_priority_t l2tp_ctrl_lcp_assign_prioity;

	rtk_rg_aclAndCf_reserved_pppoepassthroughtIntfIsPppoewanPatch_t pppoepassthroughtIntfIsPppoewanpara[8];//each array stand for one intf.Idx.
	rtk_rg_aclAndCf_reserved_pppoepassthroughtDefaulTrapRuletPatch_t	pppoepassthroughtDefaulTrapRulePatchPara;
	rtk_rg_aclAndCf_reserved_ponIntfSsidRemapPatch_t	ponIntfSsidRemapPatchPara[9];//each array stand for one intf, ponIntfSsidRemapPatchPara[8] stand for default rule.
	rtk_rg_aclAndCf_reserved_multicastVidTranslateForIpv4_t multicastVidTranslateForIpv4;
	rtk_rg_aclAndCf_reserved_multicastVidTranslateForIpv6_t multicastVidTranslateForIpv6;
	rtk_rg_aclAndCf_reserved_AssignCvlanForServicePortWithoutFilter_t 	assignCvlanForServicePortWithoutFilterPara[7];//each array stand for one port 
	rtk_rg_aclAndCf_reserved_intf_dhcp_trap_t intf_dhcp_trap_para[8]; //each array stand for one intf.Idx.
	rtk_rg_aclAndCf_reserved_intf_dslite_trap_t intf_dslite_trap_para[8];//each array stand for one intf.Idx.
	rtk_rg_aclAndCf_reserved_pppoe_intf_multicast_routing_trap_t pppoe_intf_multicast_routing_trap_para[8];//each array stand for one intf.Idx.
	rtk_rg_aclAndCf_reserved_intf_linkLocal_trap_t intf_link_local_trap_para[8]; //each array stand for one intf.Idx.
	rtk_rg_aclAndCf_reserved_ack_packet_assign_priority_t ack_packet_assign_priority;
	rtk_rg_aclAndCf_reserved_dip_mask_trap_t dip_mask_trap[16];
	rtk_rg_aclAndCf_reserved_dipv6_mask_trap_t dipv6_mask_trap[16];
	rtk_rg_aclAndCf_reserved_vlan_bind_trap_t vlan_bind_trap[32];
	rtk_rg_aclAndCf_reserved_pppoe_multicast_intf_permit_t pppoe_multicast_permit[16]; //each array stand for each valid H/W netif entry.
	rtk_rg_aclAndCf_reserved_gpon_smallbw_bridge_port_redirect_t gpon_smallbw_bridge_port_redirect[16];
	rtk_rg_aclAndCf_reserved_igmp_mld_drop_t igmp_mld_drop_portmask;
	rtk_rg_aclAndCf_multicast_pppoe_trap_t	mc_pppoe_trap[8];
}rtk_rg_acl_reserved_global_t;


#endif //end of #ifndef RTK_RG_ACL_APOLLO_H
