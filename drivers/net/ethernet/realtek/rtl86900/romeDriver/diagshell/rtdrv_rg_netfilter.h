#ifndef __RTDRV_RG_NETFILTER_H__
#define __RTDRV_RG_NETFILTER_H__


#if defined(CONFIG_RTK_L34_ENABLE) && CONFIG_RTK_L34_ENABLE > 0 || defined(CONFIG_XDSL_RG_DIAGSHELL)
#include <rtk_rg_liteRomeDriver.h>
#if defined(CONFIG_RG_CALLBACK) && CONFIG_RG_CALLBACK > 0
#include <rtk_rg_callback.h>
#endif

#endif

#include <rtdrv/rtdrv_cmdoffset.h>

int32 do_rtdrv_rg_set_ctl(struct sock *sk, int cmd, void *user, unsigned int len);
int32 do_rtdrv_rg_get_ctl(struct sock *sk, int cmd, void *user, int *len);

#define RTDRV_RG_OFFSET             			RTDRV_END_OFFSET + 100
#define RTDRV_RGEND_OFFSET            			(RTDRV_RG_OFFSET  + 200)

#if defined(CONFIG_RTK_L34_ENABLE) && CONFIG_RTK_L34_ENABLE > 0 || defined(CONFIG_XDSL_RG_DIAGSHELL)

/* RG SET ENUM */
enum rtdrv_rg_set_e{
    RTDRV_RG_DHCPSERVERSTATICALLOC_DEL=(RTDRV_BASE_CTL +RTDRV_RG_OFFSET),
	RTDRV_RG_DHCPREQUEST_SET,
#if defined(CONFIG_RG_FLOW_BASED_PLATFORM)	
#if defined(CONFIG_RG_RTL9607C_SERIES)
    RTDRV_RG_ASIC_FB_INIT,
    RTDRV_RG_ASIC_NETIFTABLE_ADD,
    RTDRV_RG_ASIC_NETIFTABLE_SET,
    RTDRV_RG_ASIC_NETIFTABLE_DEL,
    RTDRV_RG_ASIC_FLOWPATH_DEL,
    RTDRV_RG_ASIC_FLOWPATH1_ADD,
    RTDRV_RG_ASIC_FLOWPATH1_SET,
    RTDRV_RG_ASIC_FLOWPATH2_ADD,
    RTDRV_RG_ASIC_FLOWPATH2_SET,
    RTDRV_RG_ASIC_FLOWPATH3_ADD,
    RTDRV_RG_ASIC_FLOWPATH3DAHASH_ADD,
    RTDRV_RG_ASIC_FLOWPATH3_SET,
    RTDRV_RG_ASIC_FLOWPATH4_ADD,
    RTDRV_RG_ASIC_FLOWPATH4DAHASH_ADD,
    RTDRV_RG_ASIC_FLOWPATH4_SET,
    RTDRV_RG_ASIC_FLOWPATH5_ADD,
    RTDRV_RG_ASIC_FLOWPATH5_SET,
    RTDRV_RG_ASIC_FLOWPATH6_ADD,
    RTDRV_RG_ASIC_FLOWPATH6_SET,
#endif	// end CONFIG_RG_RTL9607C_SERIES    
    RTDRV_RG_ASIC_GLOBALSTATE_SET,
#endif	// end CONFIG_RG_FLOW_BASED_PLATFORM
    RTDRV_RG_INTERFACE_DEL,
    RTDRV_RG_SVLANTPID_SET,
    RTDRV_RG_SVLANSERVICEPORT_SET,
    RTDRV_RG_CVLAN_DEL,
    RTDRV_RG_VLANBINDING_DEL,
    RTDRV_RG_ALGSERVERINLANAPPSIPADDR_DEL,
    RTDRV_RG_ALGAPPS_SET,
    RTDRV_RG_VIRTUALSERVER_DEL,
    RTDRV_RG_ACLFILTERANDQOS_DEL,
    RTDRV_RG_NAPTFILTERANDQOS_DEL,
    RTDRV_RG_MACFILTER_DEL,
    RTDRV_RG_URLFILTERSTRING_DEL,
    RTDRV_RG_URLFILTERMAC_SET,
    RTDRV_RG_UPNPCONNECTION_DEL,
    RTDRV_RG_NAPTCONNECTION_DEL,
    RTDRV_RG_MULTICASTFLOW_DEL,
    RTDRV_RG_MACENTRY_DEL,
    RTDRV_RG_ARPENTRY_DEL,
    RTDRV_RG_NEIGHBORENTRY_DEL,
    RTDRV_RG_ACCESSWANLIMIT_SET,
    RTDRV_RG_ACCESSWANLIMITCATEGORY_SET,
    RTDRV_RG_SOFTWARESOURCEADDRLEARNINGLIMIT_SET,
    RTDRV_RG_WLANSOFTWARESOURCEADDRLEARNINGLIMIT_SET,
    RTDRV_RG_DOSPORTMASKENABLE_SET,
    RTDRV_RG_DOSTYPE_SET,
    RTDRV_RG_DOSFLOODTYPE_SET,
    RTDRV_RG_PORTMIRROR_SET,
    RTDRV_RG_PORTMIRROR_CLEAR,
    RTDRV_RG_PORTEGRBANDWIDTHCTRLRATE_SET,
    RTDRV_RG_PORTIGRBANDWIDTHCTRLRATE_SET,
    RTDRV_RG_PHYPORTFORCEABILITY_SET,
    RTDRV_RG_CPUPORTFORCETRAFFICCTRL_SET,
    RTDRV_RG_PORTMIBINFO_CLEAR,
    RTDRV_RG_PORTISOLATION_SET,
    RTDRV_RG_STORMCONTROL_DEL,
    RTDRV_RG_SHAREMETER_SET,
    RTDRV_RG_SHAREMETERMODE_SET,
	RTDRV_RG_QOSSTRICTPRIORITYORWEIGHTFAIRQUEUE_SET,
    RTDRV_RG_QOSINTERNALPRIMAPTOQUEUEID_SET,
    RTDRV_RG_QOSINTERNALPRIDECISIONBYWEIGHT_SET,
    RTDRV_RG_QOSDSCPREMAPTOINTERNALPRI_SET,
	RTDRV_RG_QOSPORTBASEDPRIORITY_SET,
	RTDRV_RG_QOSDOT1PPRIREMAPTOINTERNALPRI_SET,
	RTDRV_RG_QOSDSCPREMARKEGRESSPORTENABLEANDSRCSELECT_SET,
	RTDRV_RG_QOSDSCPREMARKBYINTERNALPRI_SET,
	RTDRV_RG_QOSDSCPREMARKBYDSCP_SET,
	RTDRV_RG_QOSDOT1PPRIREMARKBYINTERNALPRIEGRESSPORTENABLE_SET,
	RTDRV_RG_QOSDOT1PPRIREMARKBYINTERNALPRI_SET,
	RTDRV_RG_PORTBASEDCVLANID_SET,
	RTDRV_RG_WLANDEVBASEDCVLANID_SET,
	RTDRV_RG_NAPTEXTPORTFREE,
    RTDRV_RG_CLASSIFYENTRY_DEL,
    RTDRV_RG_GATEWAYSERVICEPORTREGISTER_DEL,
    RTDRV_RG_GPONDSBCFILTERANDREMARKING_ENABLE,
    RTDRV_RG_GPONDSBCFILTERANDREMARKING_DEL,
    RTDRV_RG_GPONDSBCFILTERANDREMARKING_DEL_ALL,
    RTDRV_RG_STPBLOCKINGPORTMASK_SET,
    RTDRV_RG_INTERFACEMIBCOUNTER_DEL,
    RTDRV_RG_SVLANTPID2_ENABLE_SET,
    RTDRV_RG_SVLANTPID2_SET,
    RTDRV_RG_HOSTPOLICELOGGING_DEL,
    RTDRV_RG_STATICROUTE_DEL,
    RTDRV_RG_ACLLOGCOUNTERCONTROL_SET,
    RTDRV_RG_ACLLOGCOUNTER_RESET,
    RTDRV_RG_GROUPMACLIMIT_SET,
    RTDRV_RG_IGMPMLD_CONTROL_SET,
    RTDRV_RG_FLOWMIBCOUNTER_RESET,
    RTDRV_RG_SOFTWAREIDLETIME_SET,
    RTDRV_RG_FUNCBASEDMETER_SET,
    RTDRV_RG_IGMPMLD_PORT_CONTROL_ADD,
	RTDRV_RG_IGMPMLD_PORT_CONTROL_DEL,

};


/* RG GET ENUM */
enum rtdrv_rg_get_e{
    RTDRV_RG_DRIVERVERSION_GET =  (RTDRV_BASE_CTL +RTDRV_RG_OFFSET),
#if defined(CONFIG_RG_FLOW_BASED_PLATFORM)
#if defined(CONFIG_RG_RTL9607C_SERIES)
    RTDRV_RG_ASIC_DUMP_FLOW_TABLE_IDX,
    RTDRV_RG_ASIC_DUMP_FLOW_TABLE_ALL,
    RTDRV_RG_ASIC_DUMP_NETIF_TABLE_IDX,
    RTDRV_RG_ASIC_DUMP_NETIF_TABLE_ALL,
    RTDRV_RG_ASIC_NETIFMIB_GET,
    RTDRV_RG_ASIC_NETIFTABLE_GET,  
    RTDRV_RG_ASIC_FLOWPATH1_GET,
    RTDRV_RG_ASIC_FLOWPATH2_GET,
    RTDRV_RG_ASIC_FLOWPATH3_GET,
    RTDRV_RG_ASIC_FLOWPATH4_GET,
    RTDRV_RG_ASIC_FLOWPATH5_GET,
    RTDRV_RG_ASIC_FLOWPATH6_GET,
#endif	// end CONFIG_RG_RTL9607C_SERIES
    RTDRV_RG_ASIC_GLOBALSTATE_GET,
#endif	// end CONFIG_RG_FLOW_BASED_PLATFORM
    RTDRV_RG_INITPARAM_GET,
    RTDRV_RG_INITPARAM_SET,
    RTDRV_RG_LANINTERFACE_ADD,
    RTDRV_RG_DHCPSERVERSTATICALLOC_ADD,
    RTDRV_RG_DHCPSERVERSTATICALLOC_FIND,
    RTDRV_RG_WANINTERFACE_ADD,
    RTDRV_RG_STATICINFO_SET,
    RTDRV_RG_DSLITEINFO_SET,
    RTDRV_RG_DHCPCLIENTINFO_SET,
    RTDRV_RG_PPPOECLIENTINFOBEFOREDIAL_SET,
    RTDRV_RG_PPPOECLIENTINFOAFTERDIAL_SET,
    RTDRV_RG_PPTPCLIENTINFOBEFOREDIAL_SET,
    RTDRV_RG_PPTPCLIENTINFOAFTERDIAL_SET,
    RTDRV_RG_L2TPCLIENTINFOBEFOREDIAL_SET,
    RTDRV_RG_L2TPCLIENTINFOAFTERDIAL_SET,
    RTDRV_RG_PPPOEDSLITEINFOBEFOREDIAL_SET,
    RTDRV_RG_PPPOEDSLITEINFOAFTERDIAL_SET,
    RTDRV_RG_INTFINFO_FIND,
    RTDRV_RG_SVLANTPID_GET,
    RTDRV_RG_SVLANSERVICEPORT_GET,
    RTDRV_RG_CVLAN_ADD,
    RTDRV_RG_CVLAN_GET,
    RTDRV_RG_VLANBINDING_ADD,
    RTDRV_RG_VLANBINDING_FIND,
    RTDRV_RG_ALGSERVERINLANAPPSIPADDR_ADD,
    RTDRV_RG_ALGAPPS_GET,
    RTDRV_RG_DMZHOST_SET,
    RTDRV_RG_DMZHOST_GET,
    RTDRV_RG_VIRTUALSERVER_ADD,
    RTDRV_RG_VIRTUALSERVER_FIND,
    RTDRV_RG_ACLFILTERANDQOS_ADD,
    RTDRV_RG_ACLFILTERANDQOS_FIND,
    RTDRV_RG_NAPTFILTERANDQOS_ADD,
    RTDRV_RG_NAPTFILTERANDQOS_FIND,
     RTDRV_RG_MACFILTER_ADD,
    RTDRV_RG_MACFILTER_FIND,
    RTDRV_RG_URLFILTERSTRING_ADD,
    RTDRV_RG_URLFILTERSTRING_FIND,
    RTDRV_RG_URLFILTERMAC_GET,
    RTDRV_RG_URLFILTERMAC_ADD,
    RTDRV_RG_URLFILTERMAC_DEL,
    RTDRV_RG_UPNPCONNECTION_ADD,
    RTDRV_RG_UPNPCONNECTION_FIND,
    RTDRV_RG_NAPTCONNECTION_ADD,
    RTDRV_RG_NAPTCONNECTION_FIND,
    RTDRV_RG_MULTICASTFLOW_ADD,
    RTDRV_RG_L2MULTICASTFLOW_ADD,
    RTDRV_RG_MULTICASTFLOW_FIND,
    RTDRV_RG_MULTICASTDATAINGRESSALLLOWEDPORTMASK_SET,
    RTDRV_RG_MULTICASTDATAINGRESSALLLOWEDPORTMASK_GET,
    RTDRV_RG_MACENTRY_ADD,
    RTDRV_RG_MACENTRY_FIND,
    RTDRV_RG_ARPENTRY_ADD,
    RTDRV_RG_ARPENTRY_FIND,
    RTDRV_RG_NEIGHBORENTRY_ADD,
    RTDRV_RG_NEIGHBORENTRY_FIND,
    RTDRV_RG_ACCESSWANLIMIT_GET,
    RTDRV_RG_ACCESSWANLIMITCATEGORY_GET,
    RTDRV_RG_SOFTWARESOURCEADDRLEARNINGLIMIT_GET,
    RTDRV_RG_WLANSOFTWARESOURCEADDRLEARNINGLIMIT_GET,
    RTDRV_RG_DOSPORTMASKENABLE_GET,
    RTDRV_RG_DOSTYPE_GET,
    RTDRV_RG_DOSFLOODTYPE_GET,
    RTDRV_RG_PORTMIRROR_GET,
    RTDRV_RG_PORTEGRBANDWIDTHCTRLRATE_GET,
    RTDRV_RG_PORTIGRBANDWIDTHCTRLRATE_GET,
    RTDRV_RG_PHYPORTFORCEABILITY_GET,
    RTDRV_RG_CPUPORTFORCETRAFFICCTRL_GET,
    RTDRV_RG_PORTMIBINFO_GET,
    RTDRV_RG_PORTISOLATION_GET,
    RTDRV_RG_STORMCONTROL_ADD,
    RTDRV_RG_STORMCONTROL_FIND,
    RTDRV_RG_SHAREMETER_GET,
    RTDRV_RG_SHAREMETERMODE_GET,
	RTDRV_RG_QOSSTRICTPRIORITYORWEIGHTFAIRQUEUE_GET,
    RTDRV_RG_QOSINTERNALPRIMAPTOQUEUEID_GET,
    RTDRV_RG_QOSINTERNALPRIDECISIONBYWEIGHT_GET,
    RTDRV_RG_QOSDSCPREMAPTOINTERNALPRI_GET,
	RTDRV_RG_QOSPORTBASEDPRIORITY_GET,
	RTDRV_RG_QOSDOT1PPRIREMAPTOINTERNALPRI_GET,
	RTDRV_RG_QOSDSCPREMARKEGRESSPORTENABLEANDSRCSELECT_GET,
	RTDRV_RG_QOSDSCPREMARKBYINTERNALPRI_GET,
	RTDRV_RG_QOSDSCPREMARKBYDSCP_GET,
	RTDRV_RG_QOSDOT1PPRIREMARKBYINTERNALPRIEGRESSPORTENABLE_GET,
	RTDRV_RG_QOSDOT1PPRIREMARKBYINTERNALPRI_GET,
	RTDRV_RG_PORTBASEDCVLANID_GET,
	RTDRV_RG_WLANDEVBASEDCVLANID_GET,
	RTDRV_RG_PORTSTATUS_GET,
	RTDRV_RG_NAPTEXTPORTGET,
    RTDRV_RG_CLASSIFYENTRY_ADD,
    RTDRV_RG_CLASSIFYENTRY_FIND,
    RTDRV_RG_PPPOEINTERFACEIDLETIME_GET,
    RTDRV_RG_GATEWAYSERVICEPORTREGISTER_ADD,
    RTDRV_RG_GATEWAYSERVICEPORTREGISTER_FIND,
    RTDRV_RG_GPONDSBCFILTERANDREMARKING_ADD,
    RTDRV_RG_GPONDSBCFILTERANDREMARKING_FIND,
	RTDRV_RG_STPBLOCKINGPORTMASK_GET,
    RTDRV_RG_INTERFACEMIBCOUNTER_GET,
    RTDRV_RG_REDIRECTHTTPALL_SET,
    RTDRV_RG_REDIRECTHTTPALL_GET,
    RTDRV_RG_REDIRECTHTTPURL_ADD,
    RTDRV_RG_REDIRECTHTTPURL_DEL,
    RTDRV_RG_REDIRECTHTTPWHITELIST_ADD,
    RTDRV_RG_REDIRECTHTTPWHITELIST_DEL,
    RTDRV_RG_REDIRECTHTTPRSP_SET,
    RTDRV_RG_REDIRECTHTTPRSP_GET,
    RTDRV_RG_SVLANTPID2_ENABLE_GET,
    RTDRV_RG_SVLANTPID2_GET,
	RTDRV_RG_HOSTPOLICECONTROL_SET,
	RTDRV_RG_HOSTPOLICECONTROL_GET,
	RTDRV_RG_HOSTPOLICELOGGING_GET,
	RTDRV_RG_REDIRECTHTTPCOUNT_SET,
    RTDRV_RG_REDIRECTHTTPCOUNT_GET,
    RTDRV_RG_STATICROUTE_ADD,
    RTDRV_RG_STATICROUTE_FIND,
    RTDRV_RG_ACLLOGCOUNTERCONTROL_GET, 
    RTDRV_RG_ACLLOGCOUNTER_GET,
    RTDRV_RG_GROUPMACLIMIT_GET,    
    RTDRV_RG_CALLBACK_FUNCTION_PTR_GET,
    RTDRV_RG_MACFILTER_WHITE_LIST_ADD,
    RTDRV_RG_MACFILTER_WHITE_LIST_DEL,
    RTDRV_RG_IGMPMLD_CONTROL_GET,
    RTDRV_RG_FLOWMIBCOUNTER_GET,
    RTDRV_RG_SOFTWAREIDLETIME_GET,
    RTDRV_RG_FUNCBASEDMETER_GET,
    RTDRV_RG_FLOWHIGHTPRIENTRY_ADD,        
    RTDRV_RG_FLOWHIGHTPRIENTRY_DEL,  
    RTDRV_RG_IGMPMLD_PORT_CONTROL_FIND,
};

#if defined(CONFIG_RG_FLOW_BASED_PLATFORM)
#if defined(CONFIG_RG_RTL9607C_SERIES)
typedef struct 	rtdrv_rg_asic_fb_init_s
{
	rtk_rg_asic_fbMode_t fbMode;
}rtdrv_rg_asic_fb_init_t;

typedef struct rtdrv_rg_asic_dump_flow_table_idx_s
{
    uint32                      	idx;
} rtdrv_rg_asic_dump_flow_table_idx_t;

typedef struct rtdrv_rg_asic_dump_netif_table_idx_s
{
    uint32                      	idx;
} rtdrv_rg_asic_dump_netif_table_idx_t;


typedef struct rtdrv_rg_asic_netifMib_get_s
{
    uint32                      	idx;
    rtk_rg_asic_netifMib_entry_t   	pNetifMibEntry;
} rtdrv_rg_asic_netifMib_get_t;

typedef struct rtdrv_rg_asic_netifTable_add_s
{
    uint32                      	idx;
    rtk_rg_asic_netif_entry_t   	pNetifEntry;
} rtdrv_rg_asic_netifTable_add_t;

typedef struct rtdrv_rg_asic_netifTable_get_s
{
    uint32                      	idx;
    rtk_rg_asic_netif_entry_t   	pNetifEntry;
} rtdrv_rg_asic_netifTable_get_t;

typedef struct rtdrv_rg_asic_netifTable_del_s
{
    uint32                      	idx;
} rtdrv_rg_asic_netifTable_del_t;

typedef struct rtdrv_rg_asic_flowPath_del_s
{
    uint32                      	idx;
} rtdrv_rg_asic_flowPath_del_t;

typedef struct rtdrv_rg_asic_flowPath1_add_s
{
    uint32                      	idx;
    rtk_rg_asic_path1_entry_t   	pP1Data;
    uint16                      	igrSVID;
    uint16                      	igrCVID;
} rtdrv_rg_asic_flowPath1_add_t;

typedef struct rtdrv_rg_asic_flowPath1_set_s
{
    uint32                      	idx;
    rtk_rg_asic_path1_entry_t   	pP1Data;
} rtdrv_rg_asic_flowPath1_set_t;

typedef struct rtdrv_rg_asic_flowPath1_get_s
{
    uint32                      	idx;
    rtk_rg_asic_path1_entry_t   	pP1Data;
} rtdrv_rg_asic_flowPath1_get_t;

typedef struct rtdrv_rg_asic_flowPath2_add_s
{
    uint32                      	idx;
    rtk_rg_asic_path2_entry_t   	pP2Data;
    uint16                      	igrSVID;
    uint16                      	igrCVID;
} rtdrv_rg_asic_flowPath2_add_t;

typedef struct rtdrv_rg_asic_flowPath2_set_s
{
    uint32                      	idx;
    rtk_rg_asic_path2_entry_t   	pP2Data;
} rtdrv_rg_asic_flowPath2_set_t;

typedef struct rtdrv_rg_asic_flowPath2_get_s
{
    uint32                      	idx;
    rtk_rg_asic_path2_entry_t   	pP2Data;
} rtdrv_rg_asic_flowPath2_get_t;

typedef struct rtdrv_rg_asic_flowPath3_add_s
{
    uint32                      	idx;
    rtk_rg_asic_path3_entry_t   	pP3Data;
    uint16                      	igrSVID;
    uint16                      	igrCVID;
} rtdrv_rg_asic_flowPath3_add_t;

typedef struct rtdrv_rg_asic_flowPath3DAHash_add_s
{
    uint32                      	idx;
    rtk_rg_asic_path3_entry_t   	pP3Data;
    uint16                      	igrSVID;
    uint16                      	igrCVID;
    uint16                      	lutDaIdx;
} rtdrv_rg_asic_flowPath3DAHash_add_t;

typedef struct rtdrv_rg_asic_flowPath3_set_s
{
    uint32                      	idx;
    rtk_rg_asic_path3_entry_t   	pP3Data;
} rtdrv_rg_asic_flowPath3_set_t;

typedef struct rtdrv_rg_asic_flowPath3_get_s
{
    uint32                      	idx;
    rtk_rg_asic_path3_entry_t   	pP3Data;
} rtdrv_rg_asic_flowPath3_get_t;

typedef struct rtdrv_rg_asic_flowPath4_add_s
{
    uint32                      	idx;
    rtk_rg_asic_path4_entry_t   	pP4Data;
    uint16                      	igrSVID;
    uint16                      	igrCVID;
} rtdrv_rg_asic_flowPath4_add_t;

typedef struct rtdrv_rg_asic_flowPath4DAHash_add_s
{
    uint32                      	idx;
    rtk_rg_asic_path4_entry_t   	pP4Data;
    uint16                      	igrSVID;
    uint16                      	igrCVID;
    uint16                      	lutDaIdx;
} rtdrv_rg_asic_flowPath4DAHash_add_t;

typedef struct rtdrv_rg_asic_flowPath4_set_s
{
    uint32                      	idx;
    rtk_rg_asic_path4_entry_t   	pP4Data;
} rtdrv_rg_asic_flowPath4_set_t;

typedef struct rtdrv_rg_asic_flowPath4_get_s
{
    uint32                      	idx;
    rtk_rg_asic_path4_entry_t   	pP4Data;
} rtdrv_rg_asic_flowPath4_get_t;

typedef struct rtdrv_rg_asic_flowPath5_add_s
{
    uint32                      	idx;
    rtk_rg_asic_path5_entry_t   	pP5Data;
    uint16                      	igrSVID;
    uint16                      	igrCVID;
} rtdrv_rg_asic_flowPath5_add_t;

typedef struct rtdrv_rg_asic_flowPath5_set_s
{
    uint32                      	idx;
    rtk_rg_asic_path5_entry_t   	pP5Data;
} rtdrv_rg_asic_flowPath5_set_t;

typedef struct rtdrv_rg_asic_flowPath5_get_s
{
    uint32                      	idx;
    rtk_rg_asic_path5_entry_t   	pP5Data;
} rtdrv_rg_asic_flowPath5_get_t;

typedef struct rtdrv_rg_asic_flowPath6_add_s
{
    uint32                      	idx;
    rtk_rg_asic_path6_entry_t   	pP6Data;
    uint16                      	igrSVID;
    uint16                      	igrCVID;
} rtdrv_rg_asic_flowPath6_add_t;

typedef struct rtdrv_rg_asic_flowPath6_set_s
{
    uint32                      	idx;
    rtk_rg_asic_path6_entry_t   	pP6Data;
} rtdrv_rg_asic_flowPath6_set_t;

typedef struct rtdrv_rg_asic_flowPath6_get_s
{
    uint32                      	idx;
    rtk_rg_asic_path6_entry_t   	pP6Data;
} rtdrv_rg_asic_flowPath6_get_t;
#endif	// end CONFIG_RG_RTL9607C_SERIES

typedef struct rtdrv_rg_asic_globalState_set_s
{
    rtk_rg_asic_globalStateType_t       stateType;
    rtk_enable_t                        state;
} rtdrv_rg_asic_globalState_set_t;

typedef struct rtdrv_rg_asic_globalState_get_s
{
    rtk_rg_asic_globalStateType_t       stateType;
    rtk_enable_t                        pState;
} rtdrv_rg_asic_globalState_get_t;
#endif	// end CONFIG_RG_FLOW_BASED_PLATFORM

typedef struct rtdrv_rg_driverVersion_get_s
{
    rtk_rg_VersionString_t      	version_string;
} rtdrv_rg_driverVersion_get_t;

typedef struct rtdrv_rg_initParam_get_s
{
    rtk_rg_initParams_t         	init_param;
} rtdrv_rg_initParam_get_t;

typedef struct rtdrv_rg_initParam_set_s
{
    rtk_rg_initParams_t         	init_param;
} rtdrv_rg_initParam_set_t;

typedef struct rtdrv_rg_lanInterface_add_s
{
    rtk_rg_lanIntfConf_t        	lan_info;
    int                         	intf_idx;
} rtdrv_rg_lanInterface_add_t;

typedef struct rtdrv_rg_wanInterface_add_s
{
    rtk_rg_wanIntfConf_t        	wanintf;
    int                         	wan_intf_idx;
} rtdrv_rg_wanInterface_add_t;


typedef struct rtdrv_rg_dhcpRequest_set_s
{
    int                                 wan_intf_idx;
} rtdrv_rg_dhcpRequest_set_t;

typedef struct rtdrv_rg_staticInfo_set_s
{
    int                         	wan_intf_idx;
    rtk_rg_ipStaticInfo_t     	static_info;
} rtdrv_rg_staticInfo_set_t;

typedef struct rtdrv_rg_dsliteInfo_set_s
{
    int                                 wan_intf_idx;
    rtk_rg_ipDslitStaticInfo_t          dslite_info;
} rtdrv_rg_dsliteInfo_set_t;

typedef struct rtdrv_rg_dhcpClientInfo_set_s
{
    int                         	wan_intf_idx;
    rtk_rg_ipDhcpClientInfo_t 	dhcpClient_info;
} rtdrv_rg_dhcpClientInfo_set_t;

typedef struct rtdrv_rg_pppoeClientInfoBeforeDial_set_s
{
    int                         	wan_intf_idx;
    rtk_rg_pppoeClientInfoBeforeDial_t	app_info;
} rtdrv_rg_pppoeClientInfoBeforeDial_set_t;

typedef struct rtdrv_rg_pppoeClientInfoAfterDial_set_s
{
    int                         	wan_intf_idx;
    rtk_rg_pppoeClientInfoAfterDial_t	clientPppoe_info;
} rtdrv_rg_pppoeClientInfoAfterDial_set_t;

typedef struct rtdrv_rg_pptpClientInfoBeforeDial_set_s
{
    int                                 wan_intf_idx;
    rtk_rg_pptpClientInfoBeforeDial_t   app_info;
} rtdrv_rg_pptpClientInfoBeforeDial_set_t;

typedef struct rtdrv_rg_pptpClientInfoAfterDial_set_s
{
    int                                 wan_intf_idx;
    rtk_rg_pptpClientInfoAfterDial_t    clientPptp_info;
} rtdrv_rg_pptpClientInfoAfterDial_set_t;

typedef struct rtdrv_rg_l2tpClientInfoBeforeDial_set_s
{
    int                                 wan_intf_idx;
    rtk_rg_l2tpClientInfoBeforeDial_t   app_info;
} rtdrv_rg_l2tpClientInfoBeforeDial_set_t;

typedef struct rtdrv_rg_l2tpClientInfoAfterDial_set_s
{
    int                                 wan_intf_idx;
    rtk_rg_l2tpClientInfoAfterDial_t    clientL2tp_info;
} rtdrv_rg_l2tpClientInfoAfterDial_set_t;

typedef struct rtdrv_rg_pppoeDsliteInfoBeforeDial_set_s
{
    int                                 wan_intf_idx;
    rtk_rg_pppoeClientInfoBeforeDial_t  app_info;
} rtdrv_rg_pppoeDsliteInfoBeforeDial_set_t;

typedef struct rtdrv_rg_pppoeDsliteInfoAfterDial_set_s
{
    int                                 wan_intf_idx;
    rtk_rg_pppoeDsliteInfoAfterDial_t   pppoeDslite_info;
} rtdrv_rg_pppoeDsliteInfoAfterDial_set_t;

typedef struct rtdrv_rg_interface_del_s
{
    int                         	lan_or_wan_intf_idx;
} rtdrv_rg_interface_del_t;

typedef struct rtdrv_rg_intfInfo_find_s
{
    rtk_rg_intfInfo_t           	intf_info;
    int                         	valid_lan_or_wan_intf_idx;
} rtdrv_rg_intfInfo_find_t;

typedef struct rtdrv_rg_svlanTpid_set_s
{
    uint32                          svlan_tag_id;
} rtdrv_rg_svlanTpid_set_t;

typedef struct rtdrv_rg_svlanTpid_get_s
{
    uint32                          pSvlanTagId;
} rtdrv_rg_svlanTpid_get_t;

typedef struct rtdrv_rg_svlanServicePort_set_s
{
    rtk_port_t                      port;
    rtk_enable_t                    enable;
} rtdrv_rg_svlanServicePort_set_t;

typedef struct rtdrv_rg_svlanServicePort_get_s
{
    rtk_port_t                      port;
    rtk_enable_t                    pEnable;
} rtdrv_rg_svlanServicePort_get_t;


typedef struct rtdrv_rg_vlanBinding_add_s
{
    rtk_rg_vlanBinding_t        	vlan_binding_info;
    int                         	vlan_binding_idx;
} rtdrv_rg_vlanBinding_add_t;

typedef struct rtdrv_rg_vlanBinding_del_s
{
    int                         	vlan_binding_idx;
} rtdrv_rg_vlanBinding_del_t;

typedef struct rtdrv_rg_vlanBinding_find_s
{
    rtk_rg_vlanBinding_t        	vlan_binding_info;
    int                         	valid_idx;
} rtdrv_rg_vlanBinding_find_t;

typedef struct rtdrv_rg_algServerInLanAppsIpAddr_add_s
{
    rtk_rg_alg_serverIpMapping_t        srvIpMapping;
} rtdrv_rg_algServerInLanAppsIpAddr_add_t;

typedef struct rtdrv_rg_algServerInLanAppsIpAddr_del_s
{
    rtk_rg_alg_type_t                   delServerMapping;
} rtdrv_rg_algServerInLanAppsIpAddr_del_t;

typedef struct rtdrv_rg_algApps_set_s
{
    rtk_rg_alg_type_t                   alg_app;
} rtdrv_rg_algApps_set_t;

typedef struct rtdrv_rg_algApps_get_s
{
    rtk_rg_alg_type_t                   alg_app;
} rtdrv_rg_algApps_get_t;


typedef struct rtdrv_rg_virtualServer_add_s
{
    rtk_rg_virtualServer_t      	virtual_server;
    int                         	virtual_server_idx;
} rtdrv_rg_virtualServer_add_t;


typedef struct rtdrv_rg_dmzHost_set_s
{
    int                                 wan_intf_idx;
    rtk_rg_dmzInfo_t                    dmz_info;
} rtdrv_rg_dmzHost_set_t;

typedef struct rtdrv_rg_dmzHost_get_s
{
    int                                 wan_intf_idx;
    rtk_rg_dmzInfo_t                    dmz_info;
} rtdrv_rg_dmzHost_get_t;


typedef struct rtdrv_rg_virtualServer_del_s
{
    int                         	virtual_server_idx;
} rtdrv_rg_virtualServer_del_t;

typedef struct rtdrv_rg_virtualServer_find_s
{
    rtk_rg_virtualServer_t      	virtual_server;
    int                         	valid_idx;
} rtdrv_rg_virtualServer_find_t;

typedef struct rtdrv_rg_aclFilterAndQos_add_s
{
    rtk_rg_aclFilterAndQos_t    	acl_filter;
    int                         	acl_filter_idx;
} rtdrv_rg_aclFilterAndQos_add_t;

typedef struct rtdrv_rg_aclFilterAndQos_del_s
{
    int                         	acl_filter_idx;
} rtdrv_rg_aclFilterAndQos_del_t;

typedef struct rtdrv_rg_aclFilterAndQos_find_s
{
    rtk_rg_aclFilterAndQos_t    	acl_filter;
    int                         	valid_idx;
} rtdrv_rg_aclFilterAndQos_find_t;

typedef struct rtdrv_rg_naptFilterAndQos_add_s
{
    int                                 index;
    rtk_rg_naptFilterAndQos_t           napt_filter;
} rtdrv_rg_naptFilterAndQos_add_t;

typedef struct rtdrv_rg_naptFilterAndQos_del_s
{
    int                                 index;
} rtdrv_rg_naptFilterAndQos_del_t;

typedef struct rtdrv_rg_naptFilterAndQos_find_s
{
    int                                 index;
    rtk_rg_naptFilterAndQos_t           napt_filter;
} rtdrv_rg_naptFilterAndQos_find_t;

typedef struct rtdrv_rg_macFilter_add_s
{
    rtk_rg_macFilterEntry_t             macFilterEntry;
    int                                 mac_filter_idx;
} rtdrv_rg_macFilter_add_t;

typedef struct rtdrv_rg_macFilter_del_s
{
    int                                 mac_filter_idx;
} rtdrv_rg_macFilter_del_t;

typedef struct rtdrv_rg_macFilter_find_s
{
    rtk_rg_macFilterEntry_t             macFilterEntry;
    int                                 valid_idx;
} rtdrv_rg_macFilter_find_t;

typedef struct rtdrv_rg_urlFilterString_add_s
{
    rtk_rg_urlFilterString_t    	filter;
    int                         	url_idx;
} rtdrv_rg_urlFilterString_add_t;

typedef struct rtdrv_rg_urlFilterString_del_s
{
    int                         	url_idx;
} rtdrv_rg_urlFilterString_del_t;

typedef struct rtdrv_rg_urlFilterString_find_s
{
    rtk_rg_urlFilterString_t    	filter;
    int                         	valid_idx;
} rtdrv_rg_urlFilterString_find_t;

typedef struct rtdrv_rg_urlFilterMAC_set_s
{
    rtk_mac_t                           filter_mac;
    rtk_rg_filterControlType_t          filter_mode;
} rtdrv_rg_urlFilterMAC_set_t;

typedef struct rtdrv_rg_urlFilterMAC_get_s
{
    rtk_mac_t                           filter_mac;
    rtk_rg_filterControlType_t          filter_mode;
} rtdrv_rg_urlFilterMAC_get_t;

typedef struct rtdrv_rg_urlFilterMAC_add_s
{
    rtk_mac_t                           filter_mac;
    rtk_rg_urlFilterString_t            filter;
} rtdrv_rg_urlFilterMAC_add_t;

typedef struct rtdrv_rg_urlFilterMAC_del_s
{
    rtk_mac_t                           filter_mac;
    rtk_rg_urlFilterString_t            filter;
} rtdrv_rg_urlFilterMAC_del_t;

typedef struct rtdrv_rg_upnpConnection_add_s
{
    rtk_rg_upnpConnection_t     	upnp;
    int                         	upnp_idx;
} rtdrv_rg_upnpConnection_add_t;

typedef struct rtdrv_rg_upnpConnection_del_s
{
    int                         	upnp_idx;
} rtdrv_rg_upnpConnection_del_t;

typedef struct rtdrv_rg_upnpConnection_find_s
{
    rtk_rg_upnpConnection_t     	upnp;
    int                         	valid_idx;
} rtdrv_rg_upnpConnection_find_t;

typedef struct rtdrv_rg_naptConnection_add_s
{
    rtk_rg_naptEntry_t          	naptFlow;
    int                         	flow_idx;
} rtdrv_rg_naptConnection_add_t;

typedef struct rtdrv_rg_naptConnection_del_s
{
    int                         	flow_idx;
} rtdrv_rg_naptConnection_del_t;

typedef struct rtdrv_rg_naptConnection_find_s
{
    rtk_rg_naptInfo_t           	naptInfo;
    int                         	valid_idx;
} rtdrv_rg_naptConnection_find_t;

typedef struct rtdrv_rg_multicastFlow_add_s
{
    rtk_rg_multicastFlow_t      	mcFlow;
    int                         	flow_idx;
} rtdrv_rg_multicastFlow_add_t;

typedef struct rtdrv_rg_multicastFlow_del_s
{
    int                         	flow_idx;
} rtdrv_rg_multicastFlow_del_t;

typedef struct rtdrv_rg_l2MulticastFlow_add_s
{
	rtk_rg_l2MulticastFlow_t		l2McFlow;
	int								flow_idx;
} rtdrv_rg_l2MulticastFlow_add_t;

typedef struct rtdrv_rg_multicastFlow_find_s
{
    rtk_rg_multicastFlow_t      	mcFlow;
    int                         	valid_idx;
} rtdrv_rg_multicastFlow_find_t;

typedef struct rtdrv_rg_multicastDataIngressAlllowedPortMask_set_s
{
    rtk_rg_mac_portmask_t       	macPort_mask;
} rtdrv_rg_multicastDataIngressAlllowedPortMask_set_t;

typedef struct rtdrv_rg_multicastDataIngressAlllowedPortMask_get_s
{
    rtk_rg_mac_portmask_t       	macPort_mask;
} rtdrv_rg_multicastDataIngressAlllowedPortMask_get_t;

typedef struct rtdrv_rg_macEntry_add_s
{
    rtk_rg_macEntry_t           	macEntry;
    int                         	entry_idx;
} rtdrv_rg_macEntry_add_t;

typedef struct rtdrv_rg_macEntry_del_s
{
    int                         	entry_idx;
} rtdrv_rg_macEntry_del_t;

typedef struct rtdrv_rg_macEntry_find_s
{
    rtk_rg_macEntry_t           	macEntry;
    int                         	valid_idx;
} rtdrv_rg_macEntry_find_t;

typedef struct rtdrv_rg_arpEntry_add_s
{
    rtk_rg_arpEntry_t           	arpEntry;
    int                         	arp_entry_idx;
} rtdrv_rg_arpEntry_add_t;

typedef struct rtdrv_rg_arpEntry_del_s
{
    int                         	arp_entry_idx;
} rtdrv_rg_arpEntry_del_t;

typedef struct rtdrv_rg_arpEntry_find_s
{
    rtk_rg_arpInfo_t            	arpInfo;
    int                         	arp_valid_idx;
} rtdrv_rg_arpEntry_find_t;

typedef struct rtdrv_rg_neighborEntry_add_s
{
    rtk_rg_neighborEntry_t              neighborEntry;
    int                                 neighbor_idx;
} rtdrv_rg_neighborEntry_add_t;

typedef struct rtdrv_rg_neighborEntry_del_s
{
    int                                 neighbor_idx;
} rtdrv_rg_neighborEntry_del_t;

typedef struct rtdrv_rg_neighborEntry_find_s
{
    rtk_rg_neighborInfo_t               neighborInfo;
    int                                 neighbor_valid_idx;
} rtdrv_rg_neighborEntry_find_t;

typedef struct rtdrv_rg_cvlan_add_s
{
    rtk_rg_cvlan_info_t                 cvlan_info;
} rtdrv_rg_cvlan_add_t;

typedef struct rtdrv_rg_cvlan_del_s
{
    int                                 cvlan_id;
} rtdrv_rg_cvlan_del_t;

typedef struct rtdrv_rg_cvlan_get_s
{
    rtk_rg_cvlan_info_t         	cvlan_info;
} rtdrv_rg_cvlan_get_t;

typedef struct rtdrv_rg_accessWanLimit_set_s
{
    rtk_rg_accessWanLimitData_t         access_wan_info;
} rtdrv_rg_accessWanLimit_set_t;

typedef struct rtdrv_rg_accessWanLimit_get_s
{
    rtk_rg_accessWanLimitData_t         access_wan_info;
} rtdrv_rg_accessWanLimit_get_t;

typedef struct rtdrv_rg_accessWanLimitCategory_set_s
{
    rtk_rg_accessWanLimitCategory_t     macCategory_info;
} rtdrv_rg_accessWanLimitCategory_set_t;

typedef struct rtdrv_rg_accessWanLimitCategory_get_s
{
    rtk_rg_accessWanLimitCategory_t     macCategory_info;
} rtdrv_rg_accessWanLimitCategory_get_t;

typedef struct rtdrv_rg_softwareSourceAddrLearningLimit_set_s
{
    rtk_rg_saLearningLimitInfo_t        sa_learnLimit_info;
    rtk_rg_port_idx_t               port_idx;
} rtdrv_rg_softwareSourceAddrLearningLimit_set_t;

typedef struct rtdrv_rg_softwareSourceAddrLearningLimit_get_s
{
    rtk_rg_saLearningLimitInfo_t        sa_learnLimit_info;
    rtk_rg_port_idx_t               port_idx;
} rtdrv_rg_softwareSourceAddrLearningLimit_get_t;

typedef struct rtdrv_rg_wlanSoftwareSourceAddrLearningLimit_set_s
{
    rtk_rg_saLearningLimitInfo_t        sa_learnLimit_info;
    int                                 wlan_idx;
    int                                 dev_idx;
} rtdrv_rg_wlanSoftwareSourceAddrLearningLimit_set_t;

typedef struct rtdrv_rg_wlanSoftwareSourceAddrLearningLimit_get_s
{
    rtk_rg_saLearningLimitInfo_t        sa_learnLimit_info;
    int                                 wlan_idx;
    int                                 dev_idx;
} rtdrv_rg_wlanSoftwareSourceAddrLearningLimit_get_t;

typedef struct rtdrv_rg_dosPortMaskEnable_set_s
{
    rtk_rg_mac_portmask_t               dos_port_mask;
} rtdrv_rg_dosPortMaskEnable_set_t;

typedef struct rtdrv_rg_dosPortMaskEnable_get_s
{
    rtk_rg_mac_portmask_t               dos_port_mask;
} rtdrv_rg_dosPortMaskEnable_get_t;

typedef struct rtdrv_rg_dosType_set_s
{
    rtk_rg_dos_type_t                   dos_type;
    int                                 dos_enabled;
    rtk_rg_dos_action_t                 dos_action;
} rtdrv_rg_dosType_set_t;

typedef struct rtdrv_rg_dosType_get_s
{
    rtk_rg_dos_type_t                   dos_type;
    int                                 dos_enabled;
    rtk_rg_dos_action_t                 dos_action;
} rtdrv_rg_dosType_get_t;
typedef struct rtdrv_rg_dosFloodType_set_s
{
    rtk_rg_dos_type_t                   dos_type;
    int                                 dos_enabled;
    rtk_rg_dos_action_t                 dos_action;
    int                                 dos_threshold;
} rtdrv_rg_dosFloodType_set_t;

typedef struct rtdrv_rg_dosFloodType_get_s
{
    rtk_rg_dos_type_t                   dos_type;
    int                                 dos_enabled;
    rtk_rg_dos_action_t                 dos_action;
    int                                 dos_threshold;
} rtdrv_rg_dosFloodType_get_t;


typedef struct rtdrv_rg_portMirror_set_s
{
    rtk_rg_portMirrorInfo_t             portMirrorInfo;
} rtdrv_rg_portMirror_set_t;

typedef struct rtdrv_rg_portMirror_get_s
{
    rtk_rg_portMirrorInfo_t             portMirrorInfo;
} rtdrv_rg_portMirror_get_t;

typedef struct rtdrv_rg_portEgrBandwidthCtrlRate_set_s
{
    rtk_rg_mac_port_idx_t               port;
    uint32                              rate;
} rtdrv_rg_portEgrBandwidthCtrlRate_set_t;

typedef struct rtdrv_rg_portIgrBandwidthCtrlRate_set_s
{
    rtk_rg_mac_port_idx_t               port;
    uint32                              rate;
} rtdrv_rg_portIgrBandwidthCtrlRate_set_t;

typedef struct rtdrv_rg_portEgrBandwidthCtrlRate_get_s
{
    rtk_rg_mac_port_idx_t               port;
    uint32                              rate;
} rtdrv_rg_portEgrBandwidthCtrlRate_get_t;

typedef struct rtdrv_rg_portIgrBandwidthCtrlRate_get_s
{
    rtk_rg_mac_port_idx_t               port;
    uint32                              rate;
} rtdrv_rg_portIgrBandwidthCtrlRate_get_t;

typedef struct rtdrv_rg_phyPortForceAbility_set_s
{
    rtk_rg_mac_port_idx_t               port;
    rtk_rg_phyPortAbilityInfo_t         ability;
} rtdrv_rg_phyPortForceAbility_set_t;

typedef struct rtdrv_rg_phyPortForceAbility_get_s
{
    rtk_rg_mac_port_idx_t               port;
    rtk_rg_phyPortAbilityInfo_t         ability;
} rtdrv_rg_phyPortForceAbility_get_t;

typedef struct rtdrv_rg_cpuPortForceTrafficCtrl_set_s
{
    rtk_rg_enable_t                     tx_fc_state;
    rtk_rg_enable_t                     rx_fc_state;
} rtdrv_rg_cpuPortForceTrafficCtrl_set_t;

typedef struct rtdrv_rg_cpuPortForceTrafficCtrl_get_s
{
    rtk_rg_enable_t                     pTx_fc_state;
    rtk_rg_enable_t                     pRx_fc_state;
} rtdrv_rg_cpuPortForceTrafficCtrl_get_t;

typedef struct rtdrv_rg_portMibInfo_get_s
{
    rtk_rg_mac_port_idx_t               port;
    rtk_rg_port_mib_info_t              mibInfo;
} rtdrv_rg_portMibInfo_get_t;

typedef struct rtdrv_rg_portMibInfo_clear_s
{
    rtk_rg_mac_port_idx_t               port;
} rtdrv_rg_portMibInfo_clear_t;

typedef struct rtdrv_rg_portIsolation_set_s
{
    rtk_rg_port_isolation_t             isolationSetting;
} rtdrv_rg_portIsolation_set_t;

typedef struct rtdrv_rg_portIsolation_get_s
{
    rtk_rg_port_isolation_t             isolationSetting;
} rtdrv_rg_portIsolation_get_t;

typedef struct rtdrv_rg_stormControl_add_s
{
    rtk_rg_stormControlInfo_t           stormInfo;
    int                                 stormInfo_idx;
} rtdrv_rg_stormControl_add_t;

typedef struct rtdrv_rg_stormControl_del_s
{
    int                                 stormInfo_idx;
} rtdrv_rg_stormControl_del_t;

typedef struct rtdrv_rg_stormControl_find_s
{
    rtk_rg_stormControlInfo_t           stormInfo;
    int                                 stormInfo_idx;
} rtdrv_rg_stormControl_find_t;

typedef struct rtdrv_rg_shareMeter_set_s
{
    uint32                              index;
    uint32                              rate;
    rtk_rg_enable_t                     ifgInclude;
} rtdrv_rg_shareMeter_set_t;

typedef struct rtdrv_rg_shareMeter_get_s
{
    uint32                              index;
    uint32                              pRate;
    rtk_rg_enable_t                     pIfgInclude;
} rtdrv_rg_shareMeter_get_t;

typedef struct rtdrv_rg_shareMeterMode_set_s
{
    uint32                              index;
    rtk_rate_metet_mode_t               meterMode;
} rtdrv_rg_shareMeterMode_set_t;

typedef struct rtdrv_rg_shareMeterMode_get_s
{
	uint32								index;
	rtk_rate_metet_mode_t				pMeterMode;
} rtdrv_rg_shareMeterMode_get_t;

typedef struct rtdrv_rg_qosStrictPriorityOrWeightFairQueue_set_s
{
    rtk_rg_mac_port_idx_t               port_idx;
    rtk_rg_qos_queue_weights_t          q_weight;
} rtdrv_rg_qosStrictPriorityOrWeightFairQueue_set_t;

typedef struct rtdrv_rg_qosStrictPriorityOrWeightFairQueue_get_s
{
    rtk_rg_mac_port_idx_t               port_idx;
    rtk_rg_qos_queue_weights_t         pQ_weight;
} rtdrv_rg_qosStrictPriorityOrWeightFairQueue_get_t;

typedef struct rtdrv_rg_qosInternalPriMapToQueueId_set_s
{
	uint32								intPri;
	uint32								queueId;
} rtdrv_rg_qosInternalPriMapToQueueId_set_t;

typedef struct rtdrv_rg_qosInternalPriMapToQueueId_get_s
{
	uint32								intPri;
	uint32								pQueueId;
} rtdrv_rg_qosInternalPriMapToQueueId_get_t;

typedef struct rtdrv_rg_qosInternalPriDecisionByWeight_set_s
{
    rtk_rg_qos_priSelWeight_t           weightOfPriSel;
} rtdrv_rg_qosInternalPriDecisionByWeight_set_t;

typedef struct rtdrv_rg_qosInternalPriDecisionByWeight_get_s
{
    rtk_rg_qos_priSelWeight_t           pWeightOfPriSel;
} rtdrv_rg_qosInternalPriDecisionByWeight_get_t;

typedef struct rtdrv_rg_qosDscpRemapToInternalPri_set_s
{
    uint32                              dscp;
    uint32                              intPri;
} rtdrv_rg_qosDscpRemapToInternalPri_set_t;

typedef struct rtdrv_rg_qosDscpRemapToInternalPri_get_s
{
    uint32                              dscp;
    uint32                              pIntPri;
} rtdrv_rg_qosDscpRemapToInternalPri_get_t;

typedef struct rtdrv_rg_qosDot1pPriRemapToInternalPri_set_s
{
    uint32                              dot1p;
    uint32                              intPri;
} rtdrv_rg_qosDot1pPriRemapToInternalPri_set_t;

typedef struct rtdrv_rg_qosDot1pPriRemapToInternalPri_get_s
{
    uint32                              dot1p;
    uint32                              pIntPri;
} rtdrv_rg_qosDot1pPriRemapToInternalPri_get_t;

typedef struct rtdrv_rg_qosPortBasedPriority_set_s
{
    rtk_rg_mac_port_idx_t               port;
    uint32                              intPri;
} rtdrv_rg_qosPortBasedPriority_set_t;

typedef struct rtdrv_rg_qosPortBasedPriority_get_s
{
    rtk_rg_mac_port_idx_t               port;
    uint32                              pIntPri;
} rtdrv_rg_qosPortBasedPriority_get_t;

typedef struct rtdrv_rg_qosDscpRemarkEgressPortEnableAndSrcSelect_set_s
{
    rtk_rg_mac_port_idx_t               port;
	rtk_rg_enable_t						is_enabled;
    rtk_rg_qos_dscpRmkSrc_t             src_sel;
} rtdrv_rg_qosDscpRemarkEgressPortEnableAndSrcSelect_set_t;

typedef struct rtdrv_rg_qosDscpRemarkEgressPortEnableAndSrcSelect_get_s
{
	rtk_rg_mac_port_idx_t				 port;
 	rtk_rg_enable_t						pIs_enabled;
    rtk_rg_qos_dscpRmkSrc_t             pSrc_sel;
} rtdrv_rg_qosDscpRemarkEgressPortEnableAndSrcSelect_get_t;

typedef struct rtdrv_rg_qosDscpRemarkByInternalPri_set_s
{
    uint32                              int_pri;
    uint32                              rmk_dscp;
} rtdrv_rg_qosDscpRemarkByInternalPri_set_t;

typedef struct rtdrv_rg_qosDscpRemarkByInternalPri_get_s
{
    uint32                              int_pri;
    uint32                              pRmk_dscp;
} rtdrv_rg_qosDscpRemarkByInternalPri_get_t;

typedef struct rtdrv_rg_qosDscpRemarkByDscp_set_s
{
    uint32                              dscp;
    uint32                              rmk_dscp;
} rtdrv_rg_qosDscpRemarkByDscp_set_t;

typedef struct rtdrv_rg_qosDscpRemarkByDscp_get_s
{
    uint32                              dscp;
    uint32                              pRmk_dscp;
} rtdrv_rg_qosDscpRemarkByDscp_get_t;

typedef struct rtdrv_rg_qosDot1pPriRemarkByInternalPriEgressPortEnable_set_s
{
    rtk_rg_mac_port_idx_t               port;
	rtk_rg_enable_t						enable;
} rtdrv_rg_qosDot1pPriRemarkByInternalPriEgressPortEnable_set_t;

typedef struct rtdrv_rg_qosDot1pPriRemarkByInternalPriEgressPortEnable_get_s
{
    rtk_rg_mac_port_idx_t               port;
	rtk_rg_enable_t						pEnable;
} rtdrv_rg_qosDot1pPriRemarkByInternalPriEgressPortEnable_get_t;

typedef struct rtdrv_rg_qosDot1pPriRemarkByInternalPri_set_s
{
    uint32                              int_pri;
    uint32                              rmk_dot1p;
} rtdrv_rg_qosDot1pPriRemarkByInternalPri_set_t;

typedef struct rtdrv_rg_qosDot1pPriRemarkByInternalPri_get_s
{
    uint32                              int_pri;
    uint32                              pRmk_dot1p;
} rtdrv_rg_qosDot1pPriRemarkByInternalPri_get_t;

typedef struct rtdrv_rg_portBasedCVlanId_set_s
{
    rtk_rg_port_idx_t				port_idx;
    int                         	pvid;
} rtdrv_rg_portBasedCVlanId_set_t;

typedef struct rtdrv_rg_portBasedCVlanId_get_s
{
    rtk_rg_port_idx_t				port_idx;
    int                         	pPvid;
} rtdrv_rg_portBasedCVlanId_get_t;

typedef struct rtdrv_rg_wlanDevBasedCVlanId_set_s
{
    int                             wlan_idx;
    int                             dev_idx;
    int                             dvid;
} rtdrv_rg_wlanDevBasedCVlanId_set_t;

typedef struct rtdrv_rg_wlanDevBasedCVlanId_get_s
{
    int                             wlan_idx;
    int                             dev_idx;
    int                             pDvid;
} rtdrv_rg_wlanDevBasedCVlanId_get_t;

typedef struct rtdrv_rg_portStatus_get_s
{
    rtk_rg_mac_port_idx_t               port;
    rtk_rg_portStatusInfo_t             portInfo;
} rtdrv_rg_portStatus_get_t;

typedef struct rtdrv_rg_classifyEntry_add_s
{
    rtk_rg_classifyEntry_t              classifyFilter;
} rtdrv_rg_classifyEntry_add_t;

typedef struct rtdrv_rg_classifyEntry_find_s
{
    int                                 index;
    rtk_rg_classifyEntry_t              classifyFilter;
} rtdrv_rg_classifyEntry_find_t;

typedef struct rtdrv_rg_classifyEntry_del_s
{
    int                                 index;
} rtdrv_rg_classifyEntry_del_t;

typedef struct rtdrv_rg_pppoeInterfaceIdleTime_get_s
{
    int                                 intfIdx;
    uint32                              idleSec;
} rtdrv_rg_pppoeInterfaceIdleTime_get_t;

typedef struct rtdrv_rg_gatewayServicePortRegister_add_s
{
    rtk_rg_gatewayServicePortEntry_t    serviceEntry;
    int                                 index;
} rtdrv_rg_gatewayServicePortRegister_add_t;

typedef struct rtdrv_rg_gatewayServicePortRegister_del_s
{
    int                                 index;
} rtdrv_rg_gatewayServicePortRegister_del_t;

typedef struct rtdrv_rg_gatewayServicePortRegister_find_s
{
    rtk_rg_gatewayServicePortEntry_t    serviceEntry;
    int                                 index;
} rtdrv_rg_gatewayServicePortRegister_find_t;

typedef struct rtdrv_rg_gponDsBcFilterAndRemarking_add_s
{
    rtk_rg_gpon_ds_bc_vlanfilterAndRemarking_t  filterRule;
    int                                 index;
} rtdrv_rg_gponDsBcFilterAndRemarking_add_t;

typedef struct rtdrv_rg_gponDsBcFilterAndRemarking_Enable_s
{
    rtk_rg_enable_t                     enable;
} rtdrv_rg_gponDsBcFilterAndRemarking_Enable_t;

typedef struct rtdrv_rg_gponDsBcFilterAndRemarking_del_s
{
    int                                 index;
} rtdrv_rg_gponDsBcFilterAndRemarking_del_t;

typedef struct rtdrv_rg_gponDsBcFilterAndRemarking_del_all_s
{
    int                                 parameter;
} rtdrv_rg_gponDsBcFilterAndRemarking_del_all_t;

typedef struct rtdrv_rg_gponDsBcFilterAndRemarking_find_s
{
    int                                 index;
    rtk_rg_gpon_ds_bc_vlanfilterAndRemarking_t  filterRule;
} rtdrv_rg_gponDsBcFilterAndRemarking_find_t;


typedef struct rtdrv_rg_stpBlockingPortmask_set_s
{
    rtk_rg_portmask_t                	Mask;
} rtdrv_rg_stpBlockingPortmask_set_t;

typedef struct rtdrv_rg_stpBlockingPortmask_get_s
{
    rtk_rg_portmask_t                	Mask;
} rtdrv_rg_stpBlockingPortmask_get_t;

typedef struct rtdrv_rg_dsliteMcTable_set_s
{
    rtk_l34_dsliteMc_entry_t    	pDsliteMcEntry;
} rtdrv_rg_dsliteMcTable_set_t;

typedef struct rtdrv_rg_dsliteMcTable_get_s
{
    rtk_l34_dsliteMc_entry_t    	pDsliteMcEntry;
} rtdrv_rg_dsliteMcTable_get_t;

typedef struct rtdrv_rg_dsliteControl_set_s
{
    rtk_l34_dsliteCtrlType_t    	ctrlType;
    uint32                      	act;
} rtdrv_rg_dsliteControl_set_t;

typedef struct rtdrv_rg_dsliteControl_get_s
{
    rtk_l34_dsliteCtrlType_t    	ctrlType;
    uint32                      	pAct;
} rtdrv_rg_dsliteControl_get_t;

typedef struct rtdrv_rg_interfaceMibCounter_del_s
{
    int                                 intf_idx;
} rtdrv_rg_interfaceMibCounter_del_t;

typedef struct rtdrv_rg_interfaceMibCounter_get_s
{
    rtk_rg_netifMib_entry_t             pNetifMib;
    int                                 intf_idx;
} rtdrv_rg_interfaceMibCounter_get_t;

typedef struct rtdrv_rg_redirectHttpAll_set_s
{
    rtk_rg_redirectHttpAll_t       	pRedirectHttpAll;
} rtdrv_rg_redirectHttpAll_set_t;

typedef struct rtdrv_rg_redirectHttpAll_get_s
{
    rtk_rg_redirectHttpAll_t       	pRedirectHttpAll;
} rtdrv_rg_redirectHttpAll_get_t;

typedef struct rtdrv_rg_redirectHttpURL_add_s
{
    rtk_rg_redirectHttpURL_t    	pRedirectHttpURL;
} rtdrv_rg_redirectHttpURL_add_t;

typedef struct rtdrv_rg_redirectHttpURL_del_s
{
    rtk_rg_redirectHttpURL_t    	pRedirectHttpURL;
} rtdrv_rg_redirectHttpURL_del_t;

typedef struct rtdrv_rg_redirectHttpWhiteList_add_s
{
    rtk_rg_redirectHttpWhiteList_t	pRedirectHttpWhiteList;
} rtdrv_rg_redirectHttpWhiteList_add_t;

typedef struct rtdrv_rg_redirectHttpWhiteList_del_s
{
    rtk_rg_redirectHttpWhiteList_t	pRedirectHttpWhiteList;
} rtdrv_rg_redirectHttpWhiteList_del_t;

typedef struct rtdrv_rg_redirectHttpRsp_set_s
{
    rtk_rg_redirectHttpRsp_t            pRedirectHttpRsp;
} rtdrv_rg_redirectHttpRsp_set_t;

typedef struct rtdrv_rg_redirectHttpRsp_get_s
{
    rtk_rg_redirectHttpRsp_t            pRedirectHttpRsp;
} rtdrv_rg_redirectHttpRsp_get_t;


typedef struct rtdrv_rg_svlanTpid2_enable_set_s
{
    rtk_rg_enable_t                     enable;
} rtdrv_rg_svlanTpid2_enable_set_t;

typedef struct rtdrv_rg_svlanTpid2_enable_get_s
{
    rtk_rg_enable_t                     pEnable;
} rtdrv_rg_svlanTpid2_enable_get_t;

typedef struct rtdrv_rg_svlanTpid2_set_s
{
    uint32                              svlan_tag_id;
} rtdrv_rg_svlanTpid2_set_t;

typedef struct rtdrv_rg_svlanTpid2_get_s
{
    uint32                              pSvlanTagId;
} rtdrv_rg_svlanTpid2_get_t;

typedef struct rtdrv_rg_hostPoliceControl_set_s
{
    rtk_rg_hostPoliceControl_t            pHostPoliceControl;
    int                                 host_idx;
} rtdrv_rg_hostPoliceControl_set_t;

typedef struct rtdrv_rg_hostPoliceControl_get_s
{
    rtk_rg_hostPoliceControl_t            pHostPoliceControl;
    int                                 host_idx;
} rtdrv_rg_hostPoliceControl_get_t;

typedef struct rtdrv_rg_hostPoliceLogging_get_s
{
    rtk_rg_hostPoliceLogging_t          pHostMibCnt;
    int                                 host_idx;
} rtdrv_rg_hostPoliceLogging_get_t;

typedef struct rtdrv_rg_hostPoliceLogging_del_s
{
    int                                 host_idx;
} rtdrv_rg_hostPoliceLogging_del_t;

typedef struct rtdrv_rg_redirectHttpCount_set_s
{
    rtk_rg_redirectHttpCount_t          pRedirectHttpCount;
} rtdrv_rg_redirectHttpCount_set_t;

typedef struct rtdrv_rg_redirectHttpCount_get_s
{
    rtk_rg_redirectHttpCount_t          pRedirectHttpCount;
} rtdrv_rg_redirectHttpCount_get_t;

typedef struct rtdrv_rg_staticRoute_add_s
{
    rtk_rg_staticRoute_t                pStaticRoute;
    int                                 index;
} rtdrv_rg_staticRoute_add_t;

typedef struct rtdrv_rg_staticRoute_del_s
{
    int                                 index;
} rtdrv_rg_staticRoute_del_t;

typedef struct rtdrv_rg_staticRoute_find_s
{
    rtk_rg_staticRoute_t                pStaticRoute;
    int                                 index;
} rtdrv_rg_staticRoute_find_t;

typedef struct rtdrv_rg_aclLogCounterControl_get_s
{
    int                         	index;
    int								type;
	int								mode;
} rtdrv_rg_aclLogCounterControl_get_t;

typedef struct rtdrv_rg_aclLogCounterControl_set_s
{
    int                         	index;
    int		         				type;
	int								mode;
} rtdrv_rg_aclLogCounterControl_set_t;

typedef struct rtdrv_rg_aclLogCounter_get_s
{
    int                         	index;
    uint64                      	count;
} rtdrv_rg_aclLogCounter_get_t;

typedef struct rtdrv_rg_aclLogCounter_reset_s
{
    int                         	index;
} rtdrv_rg_aclLogCounter_reset_t;

typedef struct rtdrv_rg_groupMacLimit_set_s
{
    rtk_rg_groupMacLimit_t      	group_mac_info;
} rtdrv_rg_groupMacLimit_set_t;

typedef struct rtdrv_rg_groupMacLimit_get_s
{
    rtk_rg_groupMacLimit_t      	pGroup_mac_info;
} rtdrv_rg_groupMacLimit_get_t;

typedef struct rtdrv_rg_callback_function_ptr_get_s
{
    rtk_rg_callbackFunctionPtrGet_t      callback_func_ptr_info;
} rtdrv_rg_callback_function_ptr_get_t;

typedef struct rtdrv_rg_mac_filter_whitelist_s
{
    rtk_rg_macFilterWhiteList_t      mac_filter_info;
} rtdrv_rg_mac_filter_whitelist_t;

typedef struct rtdrv_rg_igmp_mld_control_s
{
    rtk_rg_igmpMldSnoopingControl_t     igmpMld_info;
} rtdrv_rg_igmp_mld_control_t;

typedef struct rtdrv_rg_igmp_mld_port_control_add_s
{
	int port_idx;
    rtk_rg_igmpMldSnoopingPortControl_t     igmpMld_port_info;
} rtdrv_rg_igmp_mld_port_control_add_t;

typedef struct rtdrv_rg_igmp_mld_port_control_del_s
{
	int port_idx;
} rtdrv_rg_igmp_mld_port_control_del_t;


typedef struct rtdrv_rg_flowMibCounter_get_s
{
    int                                 index;
    rtk_rg_table_flowmib_t              counter;
} rtdrv_rg_flowMibCounter_get_t;

typedef struct rtdrv_rg_flowMibCounter_reset_s
{
    int                                 index;
} rtdrv_rg_flowMibCounter_reset_t;

typedef struct rtdrv_rg_softwareIdleTime_set_s
{
    rtk_rg_idle_time_type_t 			idleTimeType;
	int 								idleTime;
} rtdrv_rg_softwareIdleTime_set_t;

typedef struct rtdrv_rg_softwareIdleTime_get_s
{
    rtk_rg_idle_time_type_t 			idleTimeType;
	int 								pIdleTime;
} rtdrv_rg_softwareIdleTime_get_t;

typedef struct rtdrv_rg_funcbasedMeter_set_s
{
	rtk_rg_funcbasedMeterConf_t meterConf;
}rtdrv_rg_funcbasedMeter_set_t;

typedef struct rtdrv_rg_funcbasedMeter_get_s
{
	rtk_rg_funcbasedMeterConf_t meterConf;
}rtdrv_rg_funcbasedMeter_get_t;

typedef struct rtdrv_rg_hiPriEntry_add_s
{
	rtk_rg_table_highPriPatten_t hiPriEntry;
	int							 index;
}rtdrv_rg_hiPriEntry_add_t;

typedef struct rtdrv_rg_hiPriEntry_del_s
{
	int							index;
}rtdrv_rg_hiPriEntry_del_t;


typedef union rtdrv_rg_union_u
{
#if defined(CONFIG_RG_FLOW_BASED_PLATFORM)
#if defined(CONFIG_RG_RTL9607C_SERIES)
	rtdrv_rg_asic_fb_init_t rtdrv_rg_asic_fb_init_cfg;
	rtdrv_rg_asic_dump_flow_table_idx_t rtdrv_rg_asic_dump_flow_table_idx_cfg;
	rtdrv_rg_asic_dump_netif_table_idx_t rtdrv_rg_asic_dump_netif_table_idx_cfg;
	rtdrv_rg_asic_netifMib_get_t rtdrv_rg_asic_netifMib_get_cfg;
	rtdrv_rg_asic_netifTable_add_t rtdrv_rg_asic_netifTable_add_cfg;
	rtdrv_rg_asic_netifTable_get_t rtdrv_rg_asic_netifTable_get_cfg;
	rtdrv_rg_asic_netifTable_del_t rtdrv_rg_asic_netifTable_del_cfg;
	rtdrv_rg_asic_flowPath_del_t rtdrv_rg_asic_flowPath_del_cfg;
	rtdrv_rg_asic_flowPath1_add_t rtdrv_rg_asic_flowPath1_add_cfg;
	rtdrv_rg_asic_flowPath1_set_t rtdrv_rg_asic_flowPath1_set_cfg;
	rtdrv_rg_asic_flowPath1_get_t rtdrv_rg_asic_flowPath1_get_cfg;
	rtdrv_rg_asic_flowPath2_add_t rtdrv_rg_asic_flowPath2_add_cfg;
	rtdrv_rg_asic_flowPath2_set_t rtdrv_rg_asic_flowPath2_set_cfg;
	rtdrv_rg_asic_flowPath2_get_t rtdrv_rg_asic_flowPath2_get_cfg;
	rtdrv_rg_asic_flowPath3_add_t rtdrv_rg_asic_flowPath3_add_cfg;
	rtdrv_rg_asic_flowPath3DAHash_add_t rtdrv_rg_asic_flowPath3DAHash_add_cfg;
	rtdrv_rg_asic_flowPath3_set_t rtdrv_rg_asic_flowPath3_set_cfg;
	rtdrv_rg_asic_flowPath3_get_t rtdrv_rg_asic_flowPath3_get_cfg;
	rtdrv_rg_asic_flowPath4_add_t rtdrv_rg_asic_flowPath4_add_cfg;
	rtdrv_rg_asic_flowPath4DAHash_add_t rtdrv_rg_asic_flowPath4DAHash_add_cfg;
	rtdrv_rg_asic_flowPath4_set_t rtdrv_rg_asic_flowPath4_set_cfg;
	rtdrv_rg_asic_flowPath4_get_t rtdrv_rg_asic_flowPath4_get_cfg;
	rtdrv_rg_asic_flowPath5_add_t rtdrv_rg_asic_flowPath5_add_cfg;
	rtdrv_rg_asic_flowPath5_set_t rtdrv_rg_asic_flowPath5_set_cfg;
	rtdrv_rg_asic_flowPath5_get_t rtdrv_rg_asic_flowPath5_get_cfg;
	rtdrv_rg_asic_flowPath6_add_t rtdrv_rg_asic_flowPath6_add_cfg;
	rtdrv_rg_asic_flowPath6_set_t rtdrv_rg_asic_flowPath6_set_cfg;
	rtdrv_rg_asic_flowPath6_get_t rtdrv_rg_asic_flowPath6_get_cfg;
#endif	// end CONFIG_RG_RTL9607C_SERIES	
	rtdrv_rg_asic_globalState_set_t rtdrv_rg_asic_globalState_set_cfg;
	rtdrv_rg_asic_globalState_get_t rtdrv_rg_asic_globalState_get_cfg;
#endif	// end CONFIG_RG_FLOW_BASED_PLATFORM
	rtdrv_rg_driverVersion_get_t rtdrv_rg_driverVersion_get_cfg;
	rtdrv_rg_initParam_get_t rtdrv_rg_initParam_get_cfg;
	rtdrv_rg_initParam_set_t rtdrv_rg_initParam_set_cfg;
	rtdrv_rg_lanInterface_add_t rtdrv_rg_lanInterface_add_cfg;
	rtdrv_rg_wanInterface_add_t rtdrv_rg_wanInterface_add_cfg;
	rtdrv_rg_staticInfo_set_t rtdrv_rg_staticInfo_set_cfg;
	rtdrv_rg_dsliteInfo_set_t rtdrv_rg_dsliteInfo_set_cfg;
	rtdrv_rg_dhcpClientInfo_set_t rtdrv_rg_dhcpClientInfo_set_cfg;
	rtdrv_rg_pppoeClientInfoBeforeDial_set_t rtdrv_rg_pppoeClientInfoBeforeDial_set_cfg;
	rtdrv_rg_pppoeClientInfoAfterDial_set_t rtdrv_rg_pppoeClientInfoAfterDial_set_cfg;
	rtdrv_rg_pptpClientInfoBeforeDial_set_t rtdrv_rg_pptpClientInfoBeforeDial_set_cfg;
	rtdrv_rg_pptpClientInfoAfterDial_set_t rtdrv_rg_pptpClientInfoAfterDial_set_cfg;
	rtdrv_rg_l2tpClientInfoBeforeDial_set_t rtdrv_rg_l2tpClientInfoBeforeDial_set_cfg;
	rtdrv_rg_l2tpClientInfoAfterDial_set_t rtdrv_rg_l2tpClientInfoAfterDial_set_cfg;
	rtdrv_rg_pppoeDsliteInfoBeforeDial_set_t rtdrv_rg_pppoeDsliteInfoBeforeDial_set_cfg;
	rtdrv_rg_pppoeDsliteInfoAfterDial_set_t rtdrv_rg_pppoeDsliteInfoAfterDial_set_cfg;
	rtdrv_rg_intfInfo_find_t rtdrv_rg_intfInfo_find_cfg;
	rtdrv_rg_vlanBinding_add_t rtdrv_rg_vlanBinding_add_cfg;
	rtdrv_rg_vlanBinding_find_t rtdrv_rg_vlanBinding_find_cfg;
	rtdrv_rg_algServerInLanAppsIpAddr_add_t rtdrv_rg_algServerInLanAppsIpAddr_add_cfg;
	rtdrv_rg_algServerInLanAppsIpAddr_del_t rtdrv_rg_algServerInLanAppsIpAddr_del_cfg;
	rtdrv_rg_virtualServer_add_t rtdrv_rg_virtualServer_add_cfg;
	rtdrv_rg_virtualServer_find_t rtdrv_rg_virtualServer_find_cfg;
	rtdrv_rg_aclFilterAndQos_add_t rtdrv_rg_aclFilterAndQos_add_cfg;
	rtdrv_rg_aclFilterAndQos_find_t rtdrv_rg_aclFilterAndQos_find_cfg;
	rtdrv_rg_macFilter_add_t rtdrv_rg_macFilter_add_cfg;
	rtdrv_rg_macFilter_find_t rtdrv_rg_macFilter_find_cfg;
	rtdrv_rg_urlFilterString_add_t rtdrv_rg_urlFilterString_add_cfg;
	rtdrv_rg_urlFilterString_find_t rtdrv_rg_urlFilterString_find_cfg;
	rtdrv_rg_urlFilterMAC_set_t rtdrv_rg_urlFilterMAC_set_cfg;
	rtdrv_rg_urlFilterMAC_get_t rtdrv_rg_urlFilterMAC_get_cfg;
	rtdrv_rg_urlFilterMAC_add_t rtdrv_rg_urlFilterMAC_add_cfg;
	rtdrv_rg_urlFilterMAC_del_t rtdrv_rg_urlFilterMAC_del_cfg;
	rtdrv_rg_upnpConnection_add_t rtdrv_rg_upnpConnection_add_cfg;
	rtdrv_rg_upnpConnection_find_t rtdrv_rg_upnpConnection_find_cfg;
	rtdrv_rg_naptConnection_add_t rtdrv_rg_naptConnection_add_cfg;
	rtdrv_rg_naptConnection_find_t rtdrv_rg_naptConnection_find_cfg;
	rtdrv_rg_multicastFlow_add_t rtdrv_rg_multicastFlow_add_cfg;
	rtdrv_rg_l2MulticastFlow_add_t rtdrv_rg_l2MulticastFlow_add_cfg;
	rtdrv_rg_multicastFlow_find_t rtdrv_rg_multicastFlow_find_cfg;
	rtdrv_rg_multicastDataIngressAlllowedPortMask_set_t rtdrv_rg_multicastDataIngressAlllowedPortMask_set_cfg;
	rtdrv_rg_multicastDataIngressAlllowedPortMask_get_t rtdrv_rg_multicastDataIngressAlllowedPortMask_get_cfg;
	rtdrv_rg_macEntry_add_t rtdrv_rg_macEntry_add_cfg;
	rtdrv_rg_macEntry_find_t rtdrv_rg_macEntry_find_cfg;
	rtdrv_rg_arpEntry_add_t rtdrv_rg_arpEntry_add_cfg;
	rtdrv_rg_arpEntry_find_t rtdrv_rg_arpEntry_find_cfg;

	rtdrv_rg_interface_del_t rtdrv_rg_interface_del_cfg;
	rtdrv_rg_vlanBinding_del_t rtdrv_rg_vlanBinding_del_cfg;
	rtdrv_rg_virtualServer_del_t rtdrv_rg_virtualServer_del_cfg;
	rtdrv_rg_aclFilterAndQos_del_t rtdrv_rg_aclFilterAndQos_del_cfg;
	rtdrv_rg_macFilter_del_t rtdrv_rg_macFilter_del_cfg;
	rtdrv_rg_urlFilterString_del_t rtdrv_rg_urlFilterString_del_cfg;
	rtdrv_rg_upnpConnection_del_t rtdrv_rg_upnpConnection_del_cfg;
	rtdrv_rg_naptConnection_del_t rtdrv_rg_naptConnection_del_cfg;
	rtdrv_rg_multicastFlow_del_t rtdrv_rg_multicastFlow_del_cfg;
	rtdrv_rg_macEntry_del_t rtdrv_rg_macEntry_del_cfg;
	rtdrv_rg_arpEntry_del_t rtdrv_rg_arpEntry_del_cfg;
	rtdrv_rg_algApps_set_t rtdrv_rg_algApps_set_cfg;
	rtdrv_rg_algApps_get_t rtdrv_rg_algApps_get_cfg;
	rtdrv_rg_dmzHost_set_t rtdrv_rg_dmzHost_set_cfg;
	rtdrv_rg_dmzHost_get_t rtdrv_rg_dmzHost_get_cfg;
	rtdrv_rg_neighborEntry_add_t rtdrv_rg_neighborEntry_add_cfg;
	rtdrv_rg_neighborEntry_del_t rtdrv_rg_neighborEntry_del_cfg;
	rtdrv_rg_neighborEntry_find_t rtdrv_rg_neighborEntry_find_cfg;
	rtdrv_rg_cvlan_add_t rtdrv_rg_cvlan_add_cfg;
	rtdrv_rg_cvlan_del_t rtdrv_rg_cvlan_del_cfg;
	rtdrv_rg_cvlan_get_t rtdrv_rg_cvlan_get_cfg;
	rtdrv_rg_accessWanLimit_set_t rtdrv_rg_accessWanLimit_set_cfg;
	rtdrv_rg_accessWanLimit_get_t rtdrv_rg_accessWanLimit_get_cfg;
	rtdrv_rg_accessWanLimitCategory_set_t rtdrv_rg_accessWanLimitCategory_set_cfg;
	rtdrv_rg_accessWanLimitCategory_get_t rtdrv_rg_accessWanLimitCategory_get_cfg;
	rtdrv_rg_softwareSourceAddrLearningLimit_set_t rtdrv_rg_softwareSourceAddrLearningLimit_set_cfg;
	rtdrv_rg_softwareSourceAddrLearningLimit_get_t rtdrv_rg_softwareSourceAddrLearningLimit_get_cfg;
	rtdrv_rg_wlanSoftwareSourceAddrLearningLimit_set_t rtdrv_rg_wlanSoftwareSourceAddrLearningLimit_set_cfg;
	rtdrv_rg_wlanSoftwareSourceAddrLearningLimit_get_t rtdrv_rg_wlanSoftwareSourceAddrLearningLimit_get_cfg;
	rtdrv_rg_dhcpRequest_set_t rtdrv_rg_dhcpRequest_set_cfg;
	rtdrv_rg_dosPortMaskEnable_set_t rtdrv_rg_dosPortMaskEnable_set_cfg;
	rtdrv_rg_dosPortMaskEnable_get_t rtdrv_rg_dosPortMaskEnable_get_cfg;
	rtdrv_rg_dosType_set_t rtdrv_rg_dosType_set_cfg;
	rtdrv_rg_dosType_get_t rtdrv_rg_dosType_get_cfg;
	rtdrv_rg_dosFloodType_set_t rtdrv_rg_dosFloodType_set_cfg;
	rtdrv_rg_dosFloodType_get_t rtdrv_rg_dosFloodType_get_cfg;
	rtdrv_rg_portMirror_set_t rtdrv_rg_portMirror_set_cfg;
	rtdrv_rg_portMirror_get_t rtdrv_rg_portMirror_get_cfg;
	rtdrv_rg_portEgrBandwidthCtrlRate_set_t rtdrv_rg_portEgrBandwidthCtrlRate_set_cfg;
	rtdrv_rg_portIgrBandwidthCtrlRate_set_t rtdrv_rg_portIgrBandwidthCtrlRate_set_cfg;
	rtdrv_rg_portEgrBandwidthCtrlRate_get_t rtdrv_rg_portEgrBandwidthCtrlRate_get_cfg;
	rtdrv_rg_portIgrBandwidthCtrlRate_get_t rtdrv_rg_portIgrBandwidthCtrlRate_get_cfg;
	rtdrv_rg_phyPortForceAbility_set_t rtdrv_rg_phyPortForceAbility_set_cfg;
	rtdrv_rg_phyPortForceAbility_get_t rtdrv_rg_phyPortForceAbility_get_cfg;
	rtdrv_rg_cpuPortForceTrafficCtrl_set_t rtdrv_rg_cpuPortForceTrafficCtrl_set_cfg;
	rtdrv_rg_cpuPortForceTrafficCtrl_get_t rtdrv_rg_cpuPortForceTrafficCtrl_get_cfg;
	rtdrv_rg_portMibInfo_get_t rtdrv_rg_portMibInfo_get_cfg;
	rtdrv_rg_portMibInfo_clear_t rtdrv_rg_portMibInfo_clear_cfg;
	rtdrv_rg_portIsolation_set_t rtdrv_rg_portIsolation_set_cfg;
	rtdrv_rg_portIsolation_get_t rtdrv_rg_portIsolation_get_cfg;
	rtdrv_rg_stormControl_add_t rtdrv_rg_stormControl_add_cfg;
	rtdrv_rg_stormControl_del_t rtdrv_rg_stormControl_del_cfg;
	rtdrv_rg_stormControl_find_t rtdrv_rg_stormControl_find_cfg;
	rtdrv_rg_shareMeter_set_t rtdrv_rg_shareMeter_set_cfg;
	rtdrv_rg_shareMeter_get_t rtdrv_rg_shareMeter_get_cfg;
	rtdrv_rg_shareMeterMode_set_t rtdrv_rg_shareMeterMode_set_cfg;
	rtdrv_rg_shareMeterMode_get_t rtdrv_rg_shareMeterMode_get_cfg;
	rtdrv_rg_qosStrictPriorityOrWeightFairQueue_set_t rtdrv_rg_qosStrictPriorityOrWeightFairQueue_set_cfg;
	rtdrv_rg_qosStrictPriorityOrWeightFairQueue_get_t rtdrv_rg_qosStrictPriorityOrWeightFairQueue_get_cfg;
	rtdrv_rg_qosInternalPriMapToQueueId_set_t rtdrv_rg_qosInternalPriMapToQueueId_set_cfg;
	rtdrv_rg_qosInternalPriMapToQueueId_get_t rtdrv_rg_qosInternalPriMapToQueueId_get_cfg;
	rtdrv_rg_qosInternalPriDecisionByWeight_set_t rtdrv_rg_qosInternalPriDecisionByWeight_set_cfg;
	rtdrv_rg_qosInternalPriDecisionByWeight_get_t rtdrv_rg_qosInternalPriDecisionByWeight_get_cfg;
	rtdrv_rg_qosDscpRemapToInternalPri_set_t rtdrv_rg_qosDscpRemapToInternalPri_set_cfg;
	rtdrv_rg_qosDscpRemapToInternalPri_get_t rtdrv_rg_qosDscpRemapToInternalPri_get_cfg;
	rtdrv_rg_qosDot1pPriRemapToInternalPri_set_t rtdrv_rg_qosDot1pPriRemapToInternalPri_set_cfg;
	rtdrv_rg_qosDot1pPriRemapToInternalPri_get_t rtdrv_rg_qosDot1pPriRemapToInternalPri_get_cfg;
	rtdrv_rg_qosPortBasedPriority_set_t rtdrv_rg_qosPortBasedPriority_set_cfg;
	rtdrv_rg_qosPortBasedPriority_get_t rtdrv_rg_qosPortBasedPriority_get_cfg;
	rtdrv_rg_qosDscpRemarkEgressPortEnableAndSrcSelect_set_t rtdrv_rg_qosDscpRemarkEgressPortEnableAndSrcSelect_set_cfg;
	rtdrv_rg_qosDscpRemarkEgressPortEnableAndSrcSelect_get_t rtdrv_rg_qosDscpRemarkEgressPortEnableAndSrcSelect_get_cfg;
	rtdrv_rg_qosDscpRemarkByInternalPri_set_t rtdrv_rg_qosDscpRemarkByInternalPri_set_cfg;
	rtdrv_rg_qosDscpRemarkByInternalPri_get_t rtdrv_rg_qosDscpRemarkByInternalPri_get_cfg;
	rtdrv_rg_qosDscpRemarkByDscp_set_t rtdrv_rg_qosDscpRemarkByDscp_set_cfg;
	rtdrv_rg_qosDscpRemarkByDscp_get_t rtdrv_rg_qosDscpRemarkByDscp_get_cfg;
	rtdrv_rg_qosDot1pPriRemarkByInternalPriEgressPortEnable_set_t rtdrv_rg_qosDot1pPriRemarkByInternalPriEgressPortEnable_set_cfg;
	rtdrv_rg_qosDot1pPriRemarkByInternalPriEgressPortEnable_get_t rtdrv_rg_qosDot1pPriRemarkByInternalPriEgressPortEnable_get_cfg;
	rtdrv_rg_qosDot1pPriRemarkByInternalPri_set_t rtdrv_rg_qosDot1pPriRemarkByInternalPri_set_cfg;
	rtdrv_rg_qosDot1pPriRemarkByInternalPri_get_t rtdrv_rg_qosDot1pPriRemarkByInternalPri_get_cfg;
	rtdrv_rg_portBasedCVlanId_set_t rtdrv_rg_portBasedCVlanId_set_cfg;
	rtdrv_rg_portBasedCVlanId_get_t rtdrv_rg_portBasedCVlanId_get_cfg;
	rtdrv_rg_wlanDevBasedCVlanId_set_t rtdrv_rg_wlanDevBasedCVlanId_set_cfg;
	rtdrv_rg_wlanDevBasedCVlanId_get_t rtdrv_rg_wlanDevBasedCVlanId_get_cfg;
	rtdrv_rg_portStatus_get_t rtdrv_rg_portStatus_get_cfg;
	rtdrv_rg_classifyEntry_add_t	rtdrv_rg_classifyEntry_add_cfg;
	rtdrv_rg_classifyEntry_find_t	rtdrv_rg_classifyEntry_find_cfg;
	rtdrv_rg_classifyEntry_del_t	rtdrv_rg_classifyEntry_del_cfg;
	rtdrv_rg_svlanTpid_set_t rtdrv_rg_svlanTpid_set_cfg;
	rtdrv_rg_svlanTpid_get_t rtdrv_rg_svlanTpid_get_cfg;
 	rtdrv_rg_svlanServicePort_set_t rtdrv_rg_svlanServicePort_set_cfg;
	rtdrv_rg_svlanServicePort_get_t rtdrv_rg_svlanServicePort_get_cfg;
	rtdrv_rg_pppoeInterfaceIdleTime_get_t  rtdrv_rg_pppoeInterfaceIdleTime_get_cfg;
	rtdrv_rg_gatewayServicePortRegister_add_t rtdrv_rg_gatewayServicePortRegister_add_cfg;
	rtdrv_rg_gatewayServicePortRegister_del_t rtdrv_rg_gatewayServicePortRegister_del_cfg;
	rtdrv_rg_gatewayServicePortRegister_find_t rtdrv_rg_gatewayServicePortRegister_find_cfg;
	rtdrv_rg_gponDsBcFilterAndRemarking_add_t rtdrv_rg_gponDsBcFilterAndRemarking_add_cfg;
	rtdrv_rg_gponDsBcFilterAndRemarking_Enable_t rtdrv_rg_gponDsBcFilterAndRemarking_Enable_cfg;
	rtdrv_rg_gponDsBcFilterAndRemarking_del_t rtdrv_rg_gponDsBcFilterAndRemarking_del_cfg;
	rtdrv_rg_gponDsBcFilterAndRemarking_del_all_t rtdrv_rg_gponDsBcFilterAndRemarking_del_all_cfg;
	rtdrv_rg_gponDsBcFilterAndRemarking_find_t rtdrv_rg_gponDsBcFilterAndRemarking_find_cfg;

	rtdrv_rg_naptFilterAndQos_add_t rtdrv_rg_naptFilterAndQos_add_cfg;
	rtdrv_rg_naptFilterAndQos_del_t rtdrv_rg_naptFilterAndQos_del_cfg;
	rtdrv_rg_naptFilterAndQos_find_t rtdrv_rg_naptFilterAndQos_find_cfg;
	rtdrv_rg_stpBlockingPortmask_set_t rtdrv_rg_stpBlockingPortmask_set_cfg;
	rtdrv_rg_stpBlockingPortmask_get_t rtdrv_rg_stpBlockingPortmask_get_cfg;
	rtdrv_rg_interfaceMibCounter_del_t rtdrv_rg_interfaceMibCounter_del_cfg;
	rtdrv_rg_interfaceMibCounter_get_t rtdrv_rg_interfaceMibCounter_get_cfg;
	rtdrv_rg_redirectHttpAll_set_t rtdrv_rg_redirectHttpAll_set_cfg;
	rtdrv_rg_redirectHttpAll_get_t rtdrv_rg_redirectHttpAll_get_cfg;
	rtdrv_rg_redirectHttpURL_add_t rtdrv_rg_redirectHttpURL_add_cfg;
	rtdrv_rg_redirectHttpURL_del_t rtdrv_rg_redirectHttpURL_del_cfg;
	rtdrv_rg_redirectHttpWhiteList_add_t rtdrv_rg_redirectHttpWhiteList_add_cfg;
	rtdrv_rg_redirectHttpWhiteList_del_t rtdrv_rg_redirectHttpWhiteList_del_cfg;
	rtdrv_rg_redirectHttpRsp_set_t rtdrv_rg_redirectHttpRsp_set_cfg;
	rtdrv_rg_redirectHttpRsp_get_t rtdrv_rg_redirectHttpRsp_get_cfg;
	rtdrv_rg_svlanTpid2_enable_set_t rtdrv_rg_svlanTpid2_enable_set_cfg;
	rtdrv_rg_svlanTpid2_enable_get_t rtdrv_rg_svlanTpid2_enable_get_cfg;
	rtdrv_rg_svlanTpid2_set_t rtdrv_rg_svlanTpid2_set_cfg;
	rtdrv_rg_svlanTpid2_get_t rtdrv_rg_svlanTpid2_get_cfg;
	rtdrv_rg_hostPoliceControl_set_t rtdrv_rg_hostPoliceControl_set_cfg;
	rtdrv_rg_hostPoliceControl_get_t rtdrv_rg_hostPoliceControl_get_cfg;
	rtdrv_rg_hostPoliceLogging_get_t rtdrv_rg_hostPoliceLogging_get_cfg;
	rtdrv_rg_hostPoliceLogging_del_t rtdrv_rg_hostPoliceLogging_del_cfg;
	rtdrv_rg_redirectHttpCount_set_t rtdrv_rg_redirectHttpCount_set_cfg;
	rtdrv_rg_redirectHttpCount_get_t rtdrv_rg_redirectHttpCount_get_cfg;
	rtdrv_rg_staticRoute_add_t rtdrv_rg_staticRoute_add_cfg;
	rtdrv_rg_staticRoute_del_t rtdrv_rg_staticRoute_del_cfg;
	rtdrv_rg_staticRoute_find_t rtdrv_rg_staticRoute_find_cfg;
	rtdrv_rg_aclLogCounterControl_get_t rtdrv_rg_aclLogCounterControl_get_cfg;
	//rtdrv_rg_aclLogCounter_get_t rtdrv_rg_aclLogCounter_get_cfgl;
	rtdrv_rg_aclLogCounter_reset_t rtdrv_rg_aclLogCounter_reset_cfg;
	rtdrv_rg_aclLogCounter_get_t rtdrv_rg_aclLogCounter_get_cfg;
	rtdrv_rg_aclLogCounterControl_set_t rtdrv_rg_aclLogCounterControl_set_cfg;
	rtdrv_rg_groupMacLimit_set_t rtdrv_rg_groupMacLimit_set_cfg;
	rtdrv_rg_groupMacLimit_get_t rtdrv_rg_groupMacLimit_get_cfg;
	rtdrv_rg_callback_function_ptr_get_t rtdrv_rg_callback_function_ptr_get_cfg;
	rtdrv_rg_mac_filter_whitelist_t rtdrv_mac_filter_whitelist_cfg;
	rtdrv_rg_igmp_mld_control_t rtdrv_rg_igmp_mld_cfg;
	rtdrv_rg_flowMibCounter_get_t rtdrv_rg_flowMibCounter_get_cfg;
	rtdrv_rg_flowMibCounter_reset_t rtdrv_rg_flowMibCounter_reset_cfg;
	rtdrv_rg_softwareIdleTime_set_t rtdrv_rg_softwareIdleTime_set_cfg;
	rtdrv_rg_softwareIdleTime_get_t rtdrv_rg_softwareIdleTime_get_cfg;
	rtdrv_rg_funcbasedMeter_set_t rtdrv_rg_funcbasedMeter_set_cfg;
	rtdrv_rg_funcbasedMeter_get_t	rtdrv_rg_funcbasedMeter_get_cfg;
	rtdrv_rg_hiPriEntry_add_t 	rtdrv_rg_hiPriEntry_add_cfg;
	rtdrv_rg_hiPriEntry_del_t	rtdrv_rg_hiPriEntry_del_cfg;
	rtdrv_rg_igmp_mld_port_control_add_t rtdrv_rg_igmp_mld_port_cfg;
	rtdrv_rg_igmp_mld_port_control_del_t rtdrv_rg_igmp_mld_port;

	
} rtdrv_rg_union_t;

#endif


#endif // __RTDRV_RG_NETFILTER_H__
