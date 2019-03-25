


/************** Table Size **************/
#if defined(CONFIG_RG_RTL9607C_SERIES) || defined(CONFIG_RG_G3_SERIES)
#define FLOWBASED_TABLESIZE_FLOWSRAM 	RTL9607C_TABLESIZE_FLOWSRAM 
#define FLOWBASED_TABLESIZE_FLOWTCAM 	RTL9607C_TABLESIZE_FLOWTCAM 
#define FLOWBASED_HASHWAY_FLOWSRAM 	RTL9607C_HASHWAY_FLOWSRAM 
#define FLOWBASED_TABLESIZE_FLOWCAM 		RTL9607C_TABLESIZE_FLOWCAM
#define FLOWBASED_TABLESIZE_FLOWTAG 		RTL9607C_TABLESIZE_FLOWTAG
#define FLOWBASED_TABLESIZE_CAMTAG 		RTL9607C_TABLESIZE_CAMTAG
#define FLOWBASED_TABLESIZE_INTF 			RTL9607C_TABLESIZE_INTF
#define FLOWBASED_TABLESIZE_MACIND 		RTL9607C_TABLESIZE_MACIND
#define FLOWBASED_TABLESIZE_ETHERTYPE 	RTL9607C_TABLESIZE_ETHERTYPE
#define FLOWBASED_TABLESIZE_EXTPORT 		RTL9607C_TABLESIZE_EXTPORT
#define FLOWBASED_TABLESIZE_SHAREMTR 	RTL9607C_TABLESIZE_SHAREMTR
#define FLOWBASED_TABLESIZE_EXTMBR 		RTL9607C_TABLESIZE_EXTMBR
#define FLOWBASED_TABLESIZE_SP2C 			RTL9607C_TABLESIZE_SP2C
#define FLOWBASED_TABLESIZE_WAL 			RTL9607C_TABLESIZE_WAL
#define FLOWBASED_TABLESIZE_FLOWMIB 		RTL9607C_TABLESIZE_FLOWMIB
#define FLOWBASED_TABLESIZE_ACL			RTL9607C_TABLESIZE_ACL
#define FLOWBASED_EXTRATAG_LISTMIN 		RTL9607C_EXTRATAG_LISTMIN
#define FLOWBASED_EXTRATAG_LISTMAX 		RTL9607C_EXTRATAG_LISTMAX
#define FLOWBASED_EXTRATAG_ACTIONS 		RTL9607C_EXTRATAG_ACTIONS

#elif defined(CONFIG_RG_RTL9603D_SERIES)
#define FLOWBASED_TABLESIZE_FLOWSRAM 	RTL9603D_TABLESIZE_FLOWSRAM 
#define FLOWBASED_TABLESIZE_FLOWTCAM 	RTL9603D_TABLESIZE_FLOWTCAM 
#define FLOWBASED_HASHWAY_FLOWSRAM 	RTL9603D_HASHWAY_FLOWSRAM 
#define FLOWBASED_TABLESIZE_FLOWCAM 		RTL9603D_TABLESIZE_FLOWCAM
#define FLOWBASED_TABLESIZE_FLOWTAG 		RTL9603D_TABLESIZE_FLOWTAG
#define FLOWBASED_TABLESIZE_CAMTAG 		RTL9603D_TABLESIZE_CAMTAG
#define FLOWBASED_TABLESIZE_INTF 			RTL9603D_TABLESIZE_INTF
#define FLOWBASED_TABLESIZE_MACIND 		RTL9603D_TABLESIZE_MACIND
#define FLOWBASED_TABLESIZE_ETHERTYPE 	RTL9603D_TABLESIZE_ETHERTYPE
#define FLOWBASED_TABLESIZE_EXTPORT 		RTL9603D_TABLESIZE_EXTPORT
#define FLOWBASED_TABLESIZE_SHAREMTR 	RTL9603D_TABLESIZE_SHAREMTR
#define FLOWBASED_TABLESIZE_EXTMBR 		RTL9603D_TABLESIZE_EXTMBR
#define FLOWBASED_TABLESIZE_SP2C 			RTL9603D_TABLESIZE_SP2C
#define FLOWBASED_TABLESIZE_WAL 			(0)
#define FLOWBASED_TABLESIZE_FLOWMIB 		RTL9603D_TABLESIZE_FLOWMIB
#define FLOWBASED_TABLESIZE_ACL			RTL9603D_TABLESIZE_ACL
#define FLOWBASED_EXTRATAG_LISTMIN 		RTL9603D_EXTRATAG_LISTMIN
#define FLOWBASED_EXTRATAG_LISTMAX 		RTL9603D_EXTRATAG_LISTMAX
#define FLOWBASED_EXTRATAG_ACTIONS 		RTL9603D_EXTRATAG_ACTIONS
#endif


