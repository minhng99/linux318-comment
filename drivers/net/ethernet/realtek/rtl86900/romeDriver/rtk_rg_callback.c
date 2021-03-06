#include <rtk_rg_struct.h>
#include <rtk_rg_internal.h>
#include <rtk_rg_debug.h>
#include <rtk_rg_callback.h>
#include <rtk_rg_liteRomeDriver.h>

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/inet.h>	//for hton, in_aton
#if defined(CONFIG_RG_G3_SERIES)
#include <linux/uio.h>	//for import_single_range
#endif
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 30)
#else
#include <linux/config.h>
#endif

#ifdef CONFIG_RG_WMUX_SUPPORT
extern void wmux_cleanup_devices(void);
#endif

//static char ifconfig[]="/bin/ifconfig";
static char ipUtility[]="ip";
static char SNAT_INFO_FILE[]="/tmp/SNAT_INFO";
static char IGMPPROXY_PID[]="/var/run/igmp_pid";

mm_segment_t rg_callback_oldfs;

struct file *openFile(char *path,int flag,int mode)
{
    struct file *fp;

    fp=filp_open(path, flag, 0);
 
   	if(IS_ERR(fp)) 
   		return NULL;
	else
		return fp;
}

int readFile(struct file *fp,char *buf,int readlen)
{
    if (fp->f_op && fp->f_op->read)
        return fp->f_op->read(fp,buf,readlen, &fp->f_pos);
    else
        return -1;
}

int closeFile(struct file *fp)
{
    filp_close(fp,NULL);
    return 0;
}

void initKernelEnv(void)
{
    rg_callback_oldfs = get_fs();
    set_fs(KERNEL_DS);
}

int read_pid(char *filename)
{
	struct file *fp;
	int8 pidBuf[32];
	int ret, pid=0;

	initKernelEnv();
	fp = openFile(filename,O_RDWR,0);
	if (fp!=NULL)
	{
		memset(pidBuf,0,32);
		if ((ret=readFile(fp,pidBuf,31))>0){
			//rtlglue_printf("buff:%s\n",snat_file_buff);
		}
		else{ 
			rtlglue_printf("[rg callback]openFile %s error, ret=%d\n",pidBuf,ret);
		}
		closeFile(fp);
		set_fs(rg_callback_oldfs);
		sscanf(pidBuf,"%d",&pid);
	}
	return pid;

}

#ifdef CONFIG_RG_BRIDGE_PPP_STATUS
enum PPP_BRIDGE_STATE {
	STATE_DOWN, 				// 0
	STATE_LCP,					// 1
	STATE_AUTH,					// 2
	STATE_AUTH_FAILED,			// 3
	STATE_AUTH_SUCCESS,			// 4
	STATE_IPCP,					// 5
	STATE_IPCP_ACK,				// 6
	STATE_UP,					// 7
	STATE_UNCHANGE,				// 8
};

struct bridgeSessionState
{
	unsigned int id;
	unsigned int state;
};
#define MAX_BRIDGE_PPPSESSION 8
struct bridgeSessionState sessionStates[MAX_BRIDGE_PPPSESSION]={{0,0}};
unsigned int brg_ppp_state = 0;

void logSessionStateForBridge(unsigned int sessionId, unsigned int sessionState)
{
	int i = 0;
	unsigned int stateChanged = 0;
	unsigned int firstZero = MAX_BRIDGE_PPPSESSION;
	unsigned int firstUnAuthAck = MAX_BRIDGE_PPPSESSION;
	
	for(i=0; i<MAX_BRIDGE_PPPSESSION; i++)
	{
		if(sessionStates[i].id==sessionId)
		{
			if((sessionState==STATE_UP))
			{
				//receive LCP Echo reply, if orig state is not up, ignore it.
				return;
			}
			if((sessionState==STATE_IPCP_ACK)&&(sessionStates[i].state==sessionState))
			{
				sessionState = STATE_UP;
			}
			
			if(((sessionStates[i].state==STATE_UP)&&(sessionState!=STATE_UP))
				||((sessionStates[i].state!=STATE_UP)&&(sessionState==STATE_UP)))
			{
				stateChanged = 1;
			}
			sessionStates[i].state=sessionState;
			break;
		}
		else if((sessionStates[i].id==0)&&(firstZero==MAX_BRIDGE_PPPSESSION))
		{
			firstZero = i;
		}
		else if((sessionStates[i].state < STATE_AUTH_SUCCESS)&&(firstUnAuthAck==MAX_BRIDGE_PPPSESSION))
		{
			firstUnAuthAck = i;
		}
	}
	
	if(i==MAX_BRIDGE_PPPSESSION)
	{	//session id not found, Array maybe be cleared
		if(firstZero!=MAX_BRIDGE_PPPSESSION)
		{
			i = firstZero;
		}
		else if(firstUnAuthAck!=MAX_BRIDGE_PPPSESSION)
		{
			i = firstUnAuthAck;
		}
		else
		{
			//no pos....return.
			return;
		}
		sessionStates[i].id = sessionId;
		sessionStates[i].state = sessionState;
		if(sessionState == STATE_UP)
			stateChanged = 1;
	}

	if(!stateChanged)
	{
		//no need to change bridge state...
		return;
	}

	//log it..
	for(i=0; i<MAX_BRIDGE_PPPSESSION; i++)
	{
		if(sessionStates[i].state==STATE_UP)
		{
			brg_ppp_state = 1;
			break;
		}
	}
	if(i==MAX_BRIDGE_PPPSESSION)
	{
		brg_ppp_state = 0;
	}
	return;
}
#endif


/*IPv6 address to string*/
int8 *_rtk_rg_inet_n6toa(const uint8 *ipv6)
{
#define RG_IPV6_TMP_BUFFER_LENGTH 8

	static int8 buf[8*sizeof "FFFF:"];
    uint32  i;
    uint16  ipv6_ptr[RG_IPV6_TMP_BUFFER_LENGTH] = {0};

    for (i = 0; i < RG_IPV6_TMP_BUFFER_LENGTH ;i++)
    {
        ipv6_ptr[i] = ipv6[i*2+1];
        ipv6_ptr[i] |=  ipv6[i*2] << 8;
    }

    sprintf(buf, "%x:%x:%x:%x:%x:%x:%x:%x", ipv6_ptr[0], ipv6_ptr[1], ipv6_ptr[2], ipv6_ptr[3]
    , ipv6_ptr[4], ipv6_ptr[5], ipv6_ptr[6], ipv6_ptr[7]);
	return (buf);
}


/* convert IPv6 address from string to number. Length of ipv6_addr must more than 16 characters */
int32
_rg_str2ipv6(uint8 *ipv6, const uint8 *str)
{
#define RG_IN6ADDRSZ 16
#define RG_INT16SZ     2
    static const uint8 xdigits_l[] = "0123456789abcdef",
              xdigits_u[] = "0123456789ABCDEF";
    uint8 tmp[RG_IN6ADDRSZ], *tp, *endp, *colonp;
    const uint8 *xdigits, *curtok;
    int ch, saw_xdigit;
    int val;

    if ((NULL == str) || (NULL == ipv6))
    {
        return RT_ERR_FAILED;
    }

    memset((tp = tmp), '\0', RG_IN6ADDRSZ);
    endp = tp + RG_IN6ADDRSZ;
    colonp = NULL;
    /* Leading :: requires some special handling. */
    if (*str == ':')
        if (*++str != ':')
            return (RT_ERR_FAILED);
    curtok = str;
    saw_xdigit = 0;
    val = 0;
    while ((ch = *str++) != '\0') {
        const uint8 *pch;

        if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
            pch = strchr((xdigits = xdigits_u), ch);
        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);
            if (val > 0xffff)
                return (RT_ERR_FAILED);
            saw_xdigit = 1;
            continue;
        }
        if (ch == ':') {
            curtok = str;
            if (!saw_xdigit) {
                if (colonp)
                    return (RT_ERR_FAILED);
                colonp = tp;
                continue;
            }
            if (tp + RG_INT16SZ > endp)
                return (RT_ERR_FAILED);
            *tp++ = (uint8) (val >> 8) & 0xff;
            *tp++ = (uint8) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }

        return (RT_ERR_FAILED);
    }
    if (saw_xdigit) {
        if (tp + RG_INT16SZ > endp)
            return (RT_ERR_FAILED);
        *tp++ = (uint8) (val >> 8) & 0xff;
        *tp++ = (uint8) val & 0xff;
    }
    if (colonp != NULL) {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int n = tp - colonp;
        int i;

        for (i = 1; i <= n; i++) {
            endp[- i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if (tp != endp)
        return (RT_ERR_FAILED);
    memcpy(ipv6, tmp, RG_IN6ADDRSZ);
    return (RT_ERR_OK);
}/* end of diag_util_str2Ipv6 */

/* Check if the MAC address is a broadcast address or not */
#define RG_CALLBACK_DHCPD_MINIMUM_IP_NUM	8
int _rg_cacluate_dhcp_range(ipaddr_t ip, ipaddr_t mask, ipaddr_t *start, ipaddr_t *end)
{
	uint32 notMask=(~mask)&0xffffffff;
	uint32 region_length=notMask>>1;
	//ipaddr_t first_region_start,first_region_end;
	//ipaddr_t second_region_start,second_region_end;

	if(mask==0)
		return (RT_ERR_FAILED);
	if(region_length < RG_CALLBACK_DHCPD_MINIMUM_IP_NUM)
		return (RT_ERR_FAILED);

	//first_region_start = ip&mask;
	//first_region_end = first_region_start + region_length;
	//second_region_start = first_region_end++;
	//second_region_end = second_region_start + region_length;
#if 0
	if(ip-(ip&mask)>=region_length)
	{
		//server ip located in second region, so we choose FIRST region for dhcpd to distribute
		*start=ip&mask;//first_region_start;
		*end=(*start)+region_length;//first_region_end;
	}
	else
	{
		//server ip located in first region, so we choose SECOND region for dhcpd to distribute
		*start=ip&mask+region_length+1;//second_region_start;
		*end=(*start)+region_length;//second_region_end;
	}
#else
	if((ip-(ip&mask))>=region_length)
		*start=ip&mask;	//server ip located in second region, so we choose FIRST region for dhcpd to distribute
	else
		*start=(ip&mask)+region_length+1;	//server ip located in first region, so we choose SECOND region for dhcpd to distribute
	*end=(*start)+region_length;
#endif
	//Check for boundary
	if(((*start)&0xff)==0)(*start)++;
	else if(((*end)&0xff)==0xff)(*end)--;

	return (RT_ERR_OK);
}
void _rg_delete_dhcpc(char *dev_name, int forceAll)
{
	if(forceAll)
	{
		//clear old udhcpd daemon and conf file
		rtk_rg_callback_pipe_cmd("`for FILE in $(ps | grep '[u]dhcpc' | sed 's/^[ \t]*//g;s/[ \t][ \t]*.*//g'); do kill -9 $FILE > /dev/null 2>&1; done`");
		rtk_rg_callback_pipe_cmd("`for FILE in $(ls /var/udhcpc/udhcpc*); do rm -rf $FILE>/dev/null 2>&1; done`");
	}
	else if(dev_name)
	{
		//clear old udhcpd daemon and conf file for specific interface
		rtk_rg_callback_pipe_cmd("`for FILE in $(ps | grep '[u]dhcpc -i %s' | sed 's/^[ \t]*//g;s/[ \t][ \t]*.*//g'); do kill -9 $FILE > /dev/null 2>&1; done`",dev_name);
		rtk_rg_callback_pipe_cmd("`for FILE in $(ls /var/udhcpc/udhcpc.%d); do rm -rf $FILE>/dev/null 2>&1; done`",dev_name);
	}
}
void _rg_delete_dhcpd(int vlan_id, int forceAll)
{
	if(forceAll)
	{
		//clear old udhcpd daemon and conf file
		rtk_rg_callback_pipe_cmd("`for FILE in $(ps | grep '[u]dhcpd' | sed 's/^[ \t]*//g;s/[ \t][ \t]*.*//g'); do kill -9 $FILE > /dev/null 2>&1; done`");
		rtk_rg_callback_pipe_cmd("`for FILE in $(ls /var/udhcpd/udhcpd*); do rm -rf $FILE>/dev/null 2>&1; done`");
	}
	else if(vlan_id>0)
	{
		//clear old udhcpd daemon and conf file
		rtk_rg_callback_pipe_cmd("`for FILE in $(ps | grep '[u]dhcpd_br0.%d.conf' | sed 's/^[ \t]*//g;s/[ \t][ \t]*.*//g'); do kill -9 $FILE > /dev/null 2>&1; done`",vlan_id);
		rtk_rg_callback_pipe_cmd("`for FILE in $(ls /var/udhcpd/udhcpd_br0.%d*); do rm -rf $FILE>/dev/null 2>&1; done`",vlan_id);
	}
	else
	{
		//clear old udhcpd daemon and conf file
		rtk_rg_callback_pipe_cmd("`for FILE in $(ps | grep '[u]dhcpd_br0.conf' | sed 's/^[ \t]*//g;s/[ \t][ \t]*.*//g'); do kill -9 $FILE > /dev/null 2>&1; done`");
		rtk_rg_callback_pipe_cmd("rm -rf /var/udhcpd/udhcpd_br0.conf > /dev/null 2>&1");
	}
}
void _rg_setup_dhcpd(ipaddr_t ip, ipaddr_t mask, int vlan_id)
{
	ipaddr_t dhcpIPstart, dhcpIPend;

	//20170428LUKE: check for auto test mode
	if(rg_kernel.autoTestMode){
		CBACK("autoTestMode, won't setup dhcpd.");
		return;
	}

	_rg_delete_dhcpd(vlan_id,0);
	
	if(_rg_cacluate_dhcp_range(ip,mask,&dhcpIPstart,&dhcpIPend)==RT_ERR_OK)
	{
		unsigned char dhcp_start[16], dhcp_end[16];
		memcpy(dhcp_start,(void*)_inet_ntoa(dhcpIPstart),16);
		memcpy(dhcp_end,(void*)_inet_ntoa(dhcpIPend),16);
		if(vlan_id>0)
		{
			//create conf file
			rtk_rg_callback_pipe_cmd("mkdir -p /var/udhcpd/");
			rtk_rg_callback_pipe_cmd("echo > /var/udhcpd/udhcpd_br0.%d.conf",vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"poolname br0.%d\" >> /var/udhcpd/udhcpd_br0.%d.conf",vlan_id,vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"interface br0.%d\" >> /var/udhcpd/udhcpd_br0.%d.conf",vlan_id,vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"server %s\" >> /var/udhcpd/udhcpd_br0.%d.conf",(char*)_inet_ntoa(ip),vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"start %s\" >> /var/udhcpd/udhcpd_br0.%d.conf",dhcp_start,vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"end %s\" >> /var/udhcpd/udhcpd_br0.%d.conf",dhcp_end,vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"opt subnet %s\" >> /var/udhcpd/udhcpd_br0.%d.conf",(char*)_inet_ntoa(mask),vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"opt router %s\" >> /var/udhcpd/udhcpd_br0.%d.conf",(char*)_inet_ntoa(ip),vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"opt dns %s\" >> /var/udhcpd/udhcpd_br0.%d.conf",(char*)_inet_ntoa(ip),vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"opt dns 8.8.8.8\" >> /var/udhcpd/udhcpd_br0.%d.conf",vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"opt lease 86400\" >> /var/udhcpd/udhcpd_br0.%d.conf",vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"opt domain domain.name\" >> /var/udhcpd/udhcpd_br0.%d.conf",vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"opt venspec 3561 4 00E04C 5 000000000002 6 IGD\" >> /var/udhcpd/udhcpd_br0.%d.conf",vlan_id);
			rtk_rg_callback_pipe_cmd("echo \"poolend end\" >> /var/udhcpd/udhcpd_br0.%d.conf",vlan_id);

			//execute dhcpd daemon
			rtk_rg_callback_pipe_cmd("udhcpd -S /var/udhcpd/udhcpd_br0.%d.conf > /dev/null 2>&1 &",vlan_id);
		}
		else
		{
			//create conf file
			rtk_rg_callback_pipe_cmd("mkdir -p /var/udhcpd/");
			rtk_rg_callback_pipe_cmd("echo > /var/udhcpd/udhcpd_br0.conf");
			rtk_rg_callback_pipe_cmd("echo \"poolname br0\" >> /var/udhcpd/udhcpd_br0.conf");
			rtk_rg_callback_pipe_cmd("echo \"interface br0\" >> /var/udhcpd/udhcpd_br0.conf");
			rtk_rg_callback_pipe_cmd("echo \"server %s\" >> /var/udhcpd/udhcpd_br0.conf",(char*)_inet_ntoa(ip));
			rtk_rg_callback_pipe_cmd("echo \"start %s\" >> /var/udhcpd/udhcpd_br0.conf",dhcp_start);
			rtk_rg_callback_pipe_cmd("echo \"end %s\" >> /var/udhcpd/udhcpd_br0.conf",dhcp_end);
			rtk_rg_callback_pipe_cmd("echo \"opt subnet %s\" >> /var/udhcpd/udhcpd_br0.conf",(char*)_inet_ntoa(mask));
			rtk_rg_callback_pipe_cmd("echo \"opt router %s\" >> /var/udhcpd/udhcpd_br0.conf",(char*)_inet_ntoa(ip));
			rtk_rg_callback_pipe_cmd("echo \"opt dns %s\" >> /var/udhcpd/udhcpd_br0.conf",(char*)_inet_ntoa(ip));
			rtk_rg_callback_pipe_cmd("echo \"opt dns 8.8.8.8\" >> /var/udhcpd/udhcpd_br0.conf");
			rtk_rg_callback_pipe_cmd("echo \"opt lease 86400\" >> /var/udhcpd/udhcpd_br0.conf");
			rtk_rg_callback_pipe_cmd("echo \"opt domain domain.name\" >> /var/udhcpd/udhcpd_br0.conf");
			rtk_rg_callback_pipe_cmd("echo \"opt venspec 3561 4 00E04C 5 000000000002 6 IGD\" >> /var/udhcpd/udhcpd_br0.conf");
			rtk_rg_callback_pipe_cmd("echo \"poolend end\" >> /var/udhcpd/udhcpd_br0.conf");

			//execute dhcpd daemon
			rtk_rg_callback_pipe_cmd("udhcpd -S /var/udhcpd/udhcpd_br0.conf > /dev/null 2>&1 &");
		}
	}
}

void _rg_delete_radvd(int vlan_id, int forceAll)
{
	if(forceAll)
	{
		//clear old udhcpd daemon and conf file
		rtk_rg_callback_pipe_cmd("`for FILE in $(ps | grep '[r]advd' | sed 's/^[ \t]*//g;s/[ \t][ \t]*.*//g'); do kill -9 $FILE > /dev/null 2>&1; done`",vlan_id);
		rtk_rg_callback_pipe_cmd("`for FILE in $(ls /var/radvd*); do rm -rf $FILE>/dev/null 2>&1; done`",vlan_id);
	}
	else if(vlan_id>0)
	{
		//clear old udhcpd daemon and conf file
		rtk_rg_callback_pipe_cmd("`for FILE in $(ps | grep '[r]advd_br0.%d.conf' | sed 's/^[ \t]*//g;s/[ \t][ \t]*.*//g'); do kill -9 $FILE > /dev/null 2>&1; done`",vlan_id);
		rtk_rg_callback_pipe_cmd("`for FILE in $(ls /var/radvd_br0.%d*); do rm -rf $FILE>/dev/null 2>&1; done`",vlan_id);

		rtk_rg_callback_pipe_cmd("ip -6 addr delete fe80::1/64 dev br0.%d",vlan_id);
	}
	else
	{
		//clear old udhcpd daemon and conf file
		rtk_rg_callback_pipe_cmd("`for FILE in $(ps | grep '[r]advd_br0.conf' | sed 's/^[ \t]*//g;s/[ \t][ \t]*.*//g'); do kill -9 $FILE > /dev/null 2>&1; done`");
		rtk_rg_callback_pipe_cmd("rm -rf /var/radvd_br0.conf > /dev/null 2>&1");

		rtk_rg_callback_pipe_cmd("ip -6 addr delete fe80::1/64 dev br0");
	}
	rtk_rg_callback_pipe_cmd("echo 0 > /proc/sys/net/ipv6/conf/all/forwarding");
}
void _rg_setup_radvd(unsigned char *ipv6, int maskLen, int vlan_id)
{
	int prefix;
	unsigned short idx,bitMask;
	rtk_ipv6_addr_t  ipv6MaskedAddr={{0}};

	//20170428LUKE: check for auto test mode
	if(rg_kernel.autoTestMode){
		CBACK("autoTestMode, won't setup radvd.");
		return;
	}
		
	if(maskLen==128)
		memcpy(ipv6MaskedAddr.ipv6_addr,ipv6,IPV6_ADDR_LEN);
	else
	{
		prefix=maskLen;
		idx=(prefix>>3)&0xff;
			
		if((prefix&0x7)==0)
			bitMask=0;
		else
			bitMask=(0xff<<(8-(prefix&0x7)))&0xff;
		//DEBUG("prefix = %d, idx = %d, bitMask =%02x",prefix,idx,bitMask);
	
		memcpy(ipv6MaskedAddr.ipv6_addr,ipv6,idx);
		ipv6MaskedAddr.ipv6_addr[idx]=ipv6[idx]&bitMask;
		DEBUG("v6 mask addr is %s",_rtk_rg_inet_n6toa(ipv6MaskedAddr.ipv6_addr));
	}

	_rg_delete_radvd(vlan_id,0);

	rtk_rg_callback_pipe_cmd("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");

	if(vlan_id>0)
	{
		rtk_rg_callback_pipe_cmd("echo \"interface br0.%d\" > /var/radvd_br0.%d.conf",vlan_id,vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"{\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       AdvSendAdvert on;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       MaxRtrAdvInterval 600;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       MinRtrAdvInterval 198;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       AdvCurHopLimit 64;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       AdvDefaultLifetime 1800;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       AdvReachableTime 0;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       AdvRetransTimer 0;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       AdvLinkMTU 0;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       AdvManagedFlag off;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       AdvOtherConfigFlag off;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       prefix %s/%d\" >> /var/radvd_br0.%d.conf",_rtk_rg_inet_n6toa(ipv6MaskedAddr.ipv6_addr),maskLen,vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       {\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"         AdvOnLink on;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"         AdvAutonomous on;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"         AdvRouterAddr on;\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"       };\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo \"};\" >> /var/radvd_br0.%d.conf",vlan_id);
		rtk_rg_callback_pipe_cmd("echo "" >> /var/radvd_br0.%d.conf",vlan_id);

#ifdef CONFIG_LUNA_G3_SERIES
		rtk_rg_callback_pipe_cmd("/bin/radvd -C /var/radvd_br0.%d.conf",vlan_id);
#else
		rtk_rg_callback_pipe_cmd("/bin/radvd -s -C /var/radvd_br0.%d.conf",vlan_id);
#endif
		rtk_rg_callback_pipe_cmd("ip -6 addr add fe80::1/64 dev br0.%d",vlan_id);
	}
	else
	{
		rtk_rg_callback_pipe_cmd("echo \"interface br0\" > /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"{\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		AdvSendAdvert on;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		MaxRtrAdvInterval 600;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		MinRtrAdvInterval 198;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		AdvCurHopLimit 64;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		AdvDefaultLifetime 1800;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		AdvReachableTime 0;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		AdvRetransTimer 0;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		AdvLinkMTU 0;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		AdvManagedFlag off;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		AdvOtherConfigFlag off;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		prefix %s/%d\" >> /var/radvd_br0.conf",_rtk_rg_inet_n6toa(ipv6MaskedAddr.ipv6_addr),maskLen);
		rtk_rg_callback_pipe_cmd("echo \"		{\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		  AdvOnLink on;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		  AdvAutonomous on;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		  AdvRouterAddr on;\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"		};\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo \"};\" >> /var/radvd_br0.conf");
		rtk_rg_callback_pipe_cmd("echo "" >> /var/radvd_br0.conf");

#ifdef CONFIG_LUNA_G3_SERIES
		rtk_rg_callback_pipe_cmd("/bin/radvd -C /var/radvd_br0.conf");
#else
		rtk_rg_callback_pipe_cmd("/bin/radvd -s -C /var/radvd_br0.conf");
#endif
		rtk_rg_callback_pipe_cmd("ip -6 addr add fe80::1/64 dev br0");
	}
}


void _rg_configure_protocol_stack(int intfIdx, char *dev)
{
	rtk_rg_intfInfo_t *intfInfo,localInfo;
	RGDB_LOCK
	memcpy(&localInfo,&rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo,sizeof(rtk_rg_intfInfo_t));
	RGDB_UNLOCK
	intfInfo=&localInfo;
	
	//ps cmd for bring up nas0 & nas0_x for dhcp
	//do cmd
#if defined(CONFIG_RG_G3_SERIES)
	rtk_rg_callback_pipe_cmd("echo %d %s > /proc/driver/cortina/ni/dev_port_mapping",intfInfo->wan_intf.wan_intf_conf.wan_port_idx,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME);
#elif defined(CONFIG_APOLLO_MP)
    rtk_rg_callback_pipe_cmd("echo %d %s > /proc/rtl8686gmac/dev_port_mapping",intfInfo->wan_intf.wan_intf_conf.wan_port_idx,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME);
#elif defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) //end defined(CONFIG_APOLLO_MP) elif defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) 
	
		FIXME("FIXME XDSL callback\n");
	
#endif //end defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) 

	if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WIRED)
	{
		rtk_rg_callback_pipe_cmd("%s link set %s up",ipUtility,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME);
		rtk_rg_callback_pipe_cmd("%s link set %s down",ipUtility,dev);

#ifdef CONFIG_RG_WMUX_SUPPORT
		rtk_rg_callback_pipe_cmd("echo %s %d > /proc/rg/wmux_add",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,intfIdx);
#else
		rtk_rg_callback_pipe_cmd("ethctl remsmux bridge %s %s",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev);

		//smux: ignor napt cmd, just care vlan
		if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on){
			//have vlan tag
			rtk_rg_callback_pipe_cmd("ethctl addsmux %s %s %s nonapt vlan %d",intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE?"pppoe":"ipoe",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev,intfInfo->wan_intf.wan_intf_conf.egress_vlan_id); 
		}else{
			//no vlan tag
			rtk_rg_callback_pipe_cmd("ethctl addsmux %s %s %s ",intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE?"pppoe":"ipoe",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev);
		}
#endif
	}
	
	rtk_rg_callback_pipe_cmd("%s link set %s address %02x:%02x:%02x:%02x:%02x:%02x",ipUtility,dev,
			   intfInfo->wan_intf.wan_intf_conf.gmac.octet[0],
			   intfInfo->wan_intf.wan_intf_conf.gmac.octet[1],
			   intfInfo->wan_intf.wan_intf_conf.gmac.octet[2],
			   intfInfo->wan_intf.wan_intf_conf.gmac.octet[3],
			   intfInfo->wan_intf.wan_intf_conf.gmac.octet[4],
			   intfInfo->wan_intf.wan_intf_conf.gmac.octet[5]);

	rtk_rg_callback_pipe_cmd("%s link set %s txqueuelen 10",ipUtility,dev);

	rtk_rg_callback_pipe_cmd("%s link set %s up",ipUtility,dev);
}

#ifdef CONFIG_RG_CALLBACK
char snat_file_buff[CB_MAX_FILE_SIZE];
int _delete_iptables_snat(void){

	struct file *fp;
	char* buff_line[CB_MAX_FILE_LINE]={NULL};
	char* token, *ptr_head, *ptr_tail;
	int ret,len,i;
	char intf[IFNAMSIZ];
	char ip[16];
	
	/*cat ipatbles SNAT info into /tmp/SNAT_INFO */
	rtk_rg_callback_pipe_cmd("iptables -t nat -nvL POSTROUTING > /tmp/SNAT_INFO");



	/*read /tmp/SNAT_INFO */
    initKernelEnv();
    fp=openFile(SNAT_INFO_FILE,O_RDWR,0);
    if (fp!=NULL)
    {
        memset(snat_file_buff,0,CB_MAX_FILE_SIZE);
        if ((ret=readFile(fp,snat_file_buff,(CB_MAX_FILE_SIZE-1)))>0){
            //rtlglue_printf("buff:%s\n",snat_file_buff);
        }
        else{ 
			rtlglue_printf("[rg callback]openFile %s error, ret=%d\n",SNAT_INFO_FILE,ret);
        }
		closeFile(fp);

    }
    set_fs(rg_callback_oldfs);
	
	/*parse /tmp/SNAT_INFO */	
	len = strlen(snat_file_buff);
	token = snat_file_buff; //buff is static, cannot use as strsep parameter!
	while (strsep(&token, "\n"));//parse each line to buff_line[i]
	for (i=0, token=snat_file_buff; token<(snat_file_buff+len) && i<CB_MAX_FILE_LINE ;i++) { 
		buff_line[i] = token; 
		token=token+strlen(token)+1; 
	} 


	//start from buff_line[2] to parse intfname & ip, adn delete such iptables rule
	for(i=2; buff_line[i]!=NULL && i<5;i++){ //first two line are not rules in /tmp/SNAT_INFO
		/*parse intf name*/
		ptr_head = strstr(buff_line[i], "nas");
		if(ptr_head==NULL) continue;//string is not for SNAT rule 		
		ptr_tail = strchr(ptr_head, ' ');	
		if(ptr_tail==NULL) continue;//string is not for SNAT rule
		
		len = ptr_tail - ptr_head;
		strncpy(intf,ptr_head,len);
		intf[len]='\0';

		/*parse ip*/
		ptr_head = strstr(ptr_tail+1, "to:");
		if(ptr_head==NULL) continue;//string is not for SNAT rule 	
		ptr_tail = strchr(ptr_head, ' ');
		if(ptr_tail==NULL) continue;//string is not for SNAT rule
		
		len = ptr_tail - (ptr_head+3); //+3 to skip "to:"
		strncpy(ip,ptr_head+3,len);
		ip[len]='\0';
		
		rtk_rg_callback_pipe_cmd("iptables -t nat -D POSTROUTING -o %s -j SNAT --to-source %s",intf,ip);
	}

	/*remove  /tmp/SNAT_INFO */
	rtk_rg_callback_pipe_cmd("rm %s",SNAT_INFO_FILE);

	return SUCCESS;
}

/*Default callback functions*/

#ifdef CONFIG_RG_SIMPLE_PROTOCOL_STACK

//callback for rg init
int _rtk_rg_initParameterSetByHwCallBack(void)
{
	int i;
	char intf_name[20];

	rtk_rg_callback_pipe_cmd("%s br0 down",ifconfig);

	for(i=0;i<8;i++){
		sprintf(intf_name,"eth%d",i);
		rtk_rg_callback_pipe_cmd("%s %s down",ifconfig,intf_name);

		//1 FIXME: PPPoE disconnet issue
		/*
		if(rg_kernel.ppp_diaged[i]==ENABLED){
			rtk_rg_callback_pipe_cmd("/bin/spppctl down ppp%d",i);
			rg_kernel.ppp_diaged[i]=DISABLED;
		}
		*/
		//rtk_rg_callback_pipe_cmd("/bin/spppctl down ppp%d",i);
		//rtk_rg_callback_pipe_cmd("/bin/spppd");
	}

	//patch for ipatbles
	_delete_iptables_snat();
	
    return SUCCESS;

}


//rg_db.systemGlobal.initParam.interfaceAddByHwCallBack;
int _rtk_rg_interfaceAddByHwCallBack(rtk_rg_intfInfo_t* intfInfo, int* intfIdx)
{

	unsigned char ip[16];
	unsigned char gw_ip[16];
    unsigned char mask[16];
	unsigned char brcast_ip[16];
	int i;
    char dev[IFNAMSIZ]; //device name, ex:nas0_0
    char* wan_type; //wan type: ex:PPPoE
    int vid=0;
	rtk_ip_addr_t bc_ip; //broadCast ip for wan intf
	rtk_rg_ipStaticInfo_t static_info;

	
    if(intfInfo->is_wan)
    {
        //add wan interface        
		memset(&static_info,0,sizeof(rtk_rg_ipStaticInfo_t));

		//get static_info by different type
		if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_STATIC){
			 memcpy(&static_info,&intfInfo->wan_intf.static_info,sizeof(rtk_rg_ipStaticInfo_t));
		}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP){
			 memcpy(&static_info,&intfInfo->wan_intf.dhcp_client_info.hw_info,sizeof(rtk_rg_ipStaticInfo_t));
		}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
			 memcpy(&static_info,&intfInfo->wan_intf.pppoe_info.after_dial.hw_info,sizeof(rtk_rg_ipStaticInfo_t));
		}
		
        //sprintf(dev,"nas0_%d",(*intfIdx-1));
        sprintf(dev,"eth%d",(*intfIdx));
        memcpy(ip,(void*)_inet_ntoa(static_info.ip_addr),16);
		memcpy(gw_ip,(void*)_inet_ntoa(static_info.gateway_ipv4_addr),16);
        memcpy(mask,(void*)_inet_ntoa(static_info.ip_network_mask),16);

		//caculate broacast ip
		bc_ip = static_info.gateway_ipv4_addr & static_info.ip_network_mask;

		for(i=0;i<32;i++){
			if(static_info.ip_network_mask & (1<<i)){
				//wan domain (mask)
			}else{
				//wan broacast ip
				bc_ip |= (1<<i);
			}
		}
		memcpy(brcast_ip,(void*)_inet_ntoa(bc_ip),16);

        switch(intfInfo->wan_intf.wan_intf_conf.wan_type)
        {
	        case RTK_RG_STATIC:
	            wan_type="ipoe";
	            break;
	        case RTK_RG_DHCP:
	            wan_type="dhcp";
	            break;
	        case RTK_RG_PPPoE:
	            wan_type="pppoe";
	            break;
	        case RTK_RG_BRIDGE:
	            wan_type="bridge";
	            break;
	        default:
				wan_type=" ";
	            break;
        }

        if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
            vid = intfInfo->wan_intf.wan_intf_conf.egress_vlan_id;

        //do cmd
#if defined(CONFIG_RG_G3_SERIES)
		rtk_rg_callback_pipe_cmd("echo %d wan0 > /proc/driver/cortina/ni/dev_port_mapping",intfInfo->wan_intf.wan_intf_conf.wan_port_idx);
#else
        rtk_rg_callback_pipe_cmd("echo %d wan0 > /proc/rtl8686gmac/dev_port_mapping",intfInfo->wan_intf.wan_intf_conf.wan_port_idx);
#endif
		//create nas0_X
		if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP){
			//DHCP protocal-stack setting is set by _rtk_rg_dhcpRequestByHwCallBack
		}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
			//PPPoE protocal-stack setting is set by _rtk_rg_pppoeBeforeDiagByHwCallBack
		}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE){
			/*rtk_rg_callback_pipe_cmd("%s nas0 up",ifconfig);
			rtk_rg_callback_pipe_cmd("/bin/ethctl remsmux bridge nas0 %s",dev);
			
			if(static_info.napt_enable && intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
			{	
				//NAPT + VALN TAG
				rtk_rg_callback_pipe_cmd("/bin/ethctl addsmux %s nas0 %s napt vlan %d",wan_type,dev,vid); 
			}
			else if(static_info.napt_enable)
			{
				//NAPT
				rtk_rg_callback_pipe_cmd("/bin/ethctl addsmux %s nas0 %s napt",wan_type,dev);
			}
			else if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on){
				//VALN TAG
				rtk_rg_callback_pipe_cmd("/bin/ethctl addsmux %s nas0 %s nonapt vlan %d",wan_type,dev,vid); 
			}
			else{
				rtk_rg_callback_pipe_cmd("/bin/ethctl addsmux %s nas0 %s",wan_type,dev);
			}*/
			
			rtk_rg_callback_pipe_cmd("%s %s hw ether %02x%02x%02x%02x%02x%02x",ifconfig,dev,
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[0],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[1],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[2],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[3],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[4],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[5]);

			rtk_rg_callback_pipe_cmd("%s %s txqueuelen 10",ifconfig,dev);
			rtk_rg_callback_pipe_cmd("%s %s up",ifconfig,dev);
			//rtk_rg_callback_pipe_cmd("/bin/brctl addif br0 %s",dev);

		}
		else{
			//set STATIC & BRIDGE protocal-stack
	        /*rtk_rg_callback_pipe_cmd("%s nas0 up",ifconfig);
	        rtk_rg_callback_pipe_cmd("/bin/ethctl remsmux bridge nas0 %s",dev);

			if(static_info.napt_enable && intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
	        {	
	        	//NAPT + VALN TAG
	            rtk_rg_callback_pipe_cmd("/bin/ethctl addsmux %s nas0 %s napt vlan %d",wan_type,dev,vid); 
	        }
	        else if(static_info.napt_enable)
	        {
	        	//NAPT
	            rtk_rg_callback_pipe_cmd("/bin/ethctl addsmux %s nas0 %s napt",wan_type,dev);
	        }
			else if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on){
				//VALN TAG
				rtk_rg_callback_pipe_cmd("/bin/ethctl addsmux %s nas0 %s nonapt vlan %d",wan_type,dev,vid); 
			}
			else{
				rtk_rg_callback_pipe_cmd("/bin/ethctl addsmux %s nas0 %s",wan_type,dev);
			}*/
			
	        rtk_rg_callback_pipe_cmd("%s %s hw ether %02x%02x%02x%02x%02x%02x",ifconfig,dev,
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[0],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[1],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[2],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[3],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[4],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[5]);
			
	        rtk_rg_callback_pipe_cmd("%s %s up",ifconfig,dev);
			rtk_rg_callback_pipe_cmd("%s %s mtu %d",ifconfig,dev,static_info.mtu);       
    	}


		/***IPv4 Setting***/
		if(static_info.ip_version==IPVER_V4ONLY || static_info.ip_version==IPVER_V4V6){

			/*if(static_info.ipv4_default_gateway_on){
				rtk_rg_callback_pipe_cmd("/bin/route add default gw %s",gw_ip);
			}
			
			//enable forwarding state
			rtk_rg_callback_pipe_cmd("echo 1 > /proc/sys/net/ipv4/ip_forward");*/

			if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP){
				//NAT set by _rtk_rg_dhcpRequestByHwCallBack
				rtk_rg_callback_pipe_cmd("%s %s %s netmask %s broadcast %s",ifconfig,dev,ip,mask,brcast_ip);
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
				//NAT set by _rtk_rg_pppoeBeforeDiagByHwCallBack
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE){
			
			}
			else{		
				rtk_rg_callback_pipe_cmd("%s %s %s netmask %s broadcast %s",ifconfig,dev,ip,mask,brcast_ip);
				/*if(static_info.napt_enable){//add NAPT in iptables
					rtk_rg_callback_pipe_cmd("/bin/iptables -t nat -A POSTROUTING -o %s -j SNAT --to-source %s",dev,ip);
				}else{
					//pure routing
				}*/
			}
			
			//bring message to DDNS
			rtk_rg_callback_pipe_cmd("/bin/updateddctrl %s",dev);

			//do_sys_cmd("/bin/ip route add default via %s table 32",gw_ip);
			//do_sys_cmd("/bin/ip route add 192.168.150.0/24 dev nas0_0 table 32");
		}

		/***IPv6 Setting***/
		if(static_info.ip_version==IPVER_V6ONLY || static_info.ip_version==IPVER_V4V6){
			if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP){
				//NAT set by _rtk_rg_dhcpRequestByHwCallBack
				rtk_rg_callback_pipe_cmd("%s %s %s netmask %s broadcast %s",ifconfig,dev,ip,mask,brcast_ip);
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
				//NAT set by _rtk_rg_pppoeBeforeDiagByHwCallBack
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE){
			
			}
			else{		
				//current support ipv6_static only!!!
				rtk_rg_callback_pipe_cmd("%s %s add %s/%d",ifconfig,dev,_rtk_rg_inet_n6toa(&static_info.ipv6_addr.ipv6_addr[0]),static_info.ipv6_mask_length);
				//rtk_rg_callback_pipe_cmd("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
				/*if(static_info.ipv6_default_gateway_on){
					rtk_rg_callback_pipe_cmd("/bin/route -A inet6 add ::/0 gw %s %s",_rtk_rg_inet_n6toa(&static_info.gateway_ipv6_addr.ipv6_addr[0]),dev);
				}*/			
			}
		}
		
    }
    else
    {
    	//add lan interface
        memcpy(ip,(void*)_inet_ntoa(intfInfo->lan_intf.ip_addr),16);
        memcpy(mask,(void*)_inet_ntoa(intfInfo->lan_intf.ip_network_mask),16);

		rtk_rg_callback_pipe_cmd("brctl addbr br0");
		rtk_rg_callback_pipe_cmd("brctl addif %s", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
#ifdef CONFIG_DUALBAND_CONCURRENT
		rtk_rg_callback_pipe_cmd("/bin/brctl addif br0 wlan1");
		rtk_rg_callback_pipe_cmd("%s wlan1 down",ifconfig);
#endif		
        rtk_rg_callback_pipe_cmd("%s %s down",ifconfig, CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
        rtk_rg_callback_pipe_cmd("%s br0 down",ifconfig);

		rtk_rg_callback_pipe_cmd("%s br0 hw ether %02x%02x%02x%02x%02x%02x",ifconfig,
				   intfInfo->lan_intf.gmac.octet[0],
				   intfInfo->lan_intf.gmac.octet[1],
				   intfInfo->lan_intf.gmac.octet[2],
				   intfInfo->lan_intf.gmac.octet[3],
				   intfInfo->lan_intf.gmac.octet[4],
				   intfInfo->lan_intf.gmac.octet[5]);


		/***IPv4 Setting***/
		if(intfInfo->lan_intf.ip_version==IPVER_V4ONLY || intfInfo->lan_intf.ip_version==IPVER_V4V6){
	        rtk_rg_callback_pipe_cmd("%s br0 %s netmask %s mtu %d",ifconfig,ip,mask,intfInfo->lan_intf.mtu);
		}

		/***IPv6 Setting***/
		if(intfInfo->lan_intf.ip_version==IPVER_V6ONLY || intfInfo->lan_intf.ip_version==IPVER_V4V6){
			rtk_rg_callback_pipe_cmd("%s br0 add %s/%d",ifconfig,_rtk_rg_inet_n6toa(&intfInfo->lan_intf.ipv6_addr.ipv6_addr[0]),intfInfo->lan_intf.ipv6_network_mask_length);
		}

		rtk_rg_callback_pipe_cmd("%s br0 up",ifconfig);
        rtk_rg_callback_pipe_cmd("%s %s up",ifconfig, CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);


#ifdef CONFIG_DUALBAND_CONCURRENT
		rtk_rg_callback_pipe_cmd("%s wlan1 up",ifconfig);
#endif

        //patch for iptables
        rtk_rg_callback_pipe_cmd("/bin/iptables -A INPUT -s %s -d %s -j ACCEPT",ip,ip);
#if 0
		//restart udhcpd: this will make signal hang
		do_sys_cmd("/bin/udhcpd -B /var/udhcpd/udhcpd.conf");
#endif
    }

	//rtlglue_printf("intf[0] valid==%d\n",rg_db.systemGlobal.interfaceInfo[0].valid);
	//rtlglue_printf("intfIdx=%d	wan_type=%d@@@@@@n",*intfIdx,rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo.wan_intf.wan_intf_conf.wan_type);

    return SUCCESS;
}

//rg_db.systemGlobal.initParam.interfaceDelByHwCallBack;
int _rtk_rg_interfaceDelByHwCallBack(rtk_rg_intfInfo_t* intfInfo, int* intfIdx)
{
	int ret,dhcpc_pid;
	struct task_struct *t;
	
	if(intfInfo->is_wan==0){
		rtk_rg_callback_pipe_cmd("%s %s down",ifconfig, CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
		//rtk_rg_callback_pipe_cmd("%s br0 down",ifconfig);
	}else{
		char dev[IFNAMSIZ]; //device name, ex:nas0_0
		//sprintf(dev,"nas0_%d",(*intfIdx-1));
		sprintf(dev,"eth%d",(*intfIdx));

		if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP)
		{
			//1 FIXME:if there is two or more WAN use DHCP, this may not working
			dhcpc_pid = read_pid((char*)DHCPC_PID);
			CBACK("the dhcpc_pid is %d",dhcpc_pid);
			if (dhcpc_pid > 0)
			{
				t = get_pid_task(find_get_pid(dhcpc_pid), PIDTYPE_PID);
				if(t == NULL){
					CBACK("no such pid");
				}
				else
				{
					ret = send_sig(SIGTERM,t,0);//send_sig_info(SIG_TEST, &info, t);    //send the signal
					if (ret < 0) {
						CBACK("error sending signal\n");
					}
				}
				
				//kill(dhcpc_pid, SIGTERM);
				//rtk_rg_callback_pipe_cmd("kill -%d %d",SIGTERM,dhcpc_pid);//kill(dhcpc_pid, SIGTERM);
			}
		}
		else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE)
		{
			if(rg_db.systemGlobal.not_disconnect_ppp==0){
				rtk_rg_callback_pipe_cmd("/bin/spppctl down %d",*intfIdx);
				//let spppd to down the eth wan for us, otherwise PADT won't send out
				return SUCCESS;
			}else{
				//don't call spppctl to disconnet interface pppX, because this case is "Server disconnet problem".
				//so, do nothing!
				rg_db.systemGlobal.not_disconnect_ppp=0;
			}
		}
		
		//wan-intf
		rtk_rg_callback_pipe_cmd("%s %s down",ifconfig,dev);
	}

    return SUCCESS;
}

//rg_db.systemGlobal.initParam.dhcpRequestByHwCallBack;
int _rtk_rg_dhcpRequestByHwCallBack(int* intfIdx){

	char dev[IFNAMSIZ]; //device name, ex:nas0_0
	int dev_idx;
	//int vid;
	rtk_rg_intfInfo_t intfInfo;
	CBACK("%s is called!!!",__func__);

	memcpy(&intfInfo,&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo,sizeof(rtk_rg_intfInfo_t));

	////check wan_intf info & set dev name
	if((*intfIdx>7) || (*intfIdx<0)){
		CBACK("invalid wan_intf_idx\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else{
		//dev_idx = (*intfIdx-1);
		dev_idx = (*intfIdx);
		sprintf(dev,"eth%d",dev_idx);
	}
	
	if(rg_db.systemGlobal.interfaceInfo[*intfIdx].valid==IF_INVALID_ENTRY){
		CBACK("Assigned wan_intf_idx is not valid.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else if(!intfInfo.is_wan){
		CBACK("Assigned wan_intf_idx is not wan.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else if(intfInfo.wan_intf.wan_intf_conf.wan_type!=RTK_RG_DHCP){
		CBACK("Assigned wan_intf_idx is not DHCP.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}
	
	//ps cmd for bring up nas0 & nas0_x for dhcp

	//do cmd
#if defined(CONFIG_RG_G3_SERIES)
	rtk_rg_callback_pipe_cmd("echo %d wan0 > /proc/driver/cortina/ni/dev_port_mapping",intfInfo.wan_intf.wan_intf_conf.wan_port_idx);
#else
    rtk_rg_callback_pipe_cmd("echo %d wan0 > /proc/rtl8686gmac/dev_port_mapping",intfInfo.wan_intf.wan_intf_conf.wan_port_idx);
#endif	
	//rtk_rg_callback_pipe_cmd("%s nas0 up",ifconfig);
	rtk_rg_callback_pipe_cmd("%s %s down",ifconfig,dev);
	//rtk_rg_callback_pipe_cmd("/bin/ethctl remsmux bridge nas0 %s",dev);

	//smux: ignor napt cmd in ppp, just care vlan
	/*if(intfInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on){
		//have vlan tag
		vid = intfInfo.wan_intf.wan_intf_conf.egress_vlan_id;
		rtk_rg_callback_pipe_cmd("/bin/ethctl addsmux ipoe nas0 %s nonapt vlan %d",dev,vid); 
	}else{
		//no vlan tag
		rtk_rg_callback_pipe_cmd("/bin/ethctl addsmux ipoe nas0 %s ",dev);
	}*/
	
	rtk_rg_callback_pipe_cmd("%s %s hw ether %02x%02x%02x%02x%02x%02x",ifconfig,dev,
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[0],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[1],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[2],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[3],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[4],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[5]);

	rtk_rg_callback_pipe_cmd("/bin/ifconfig %s txqueuelen 10",dev);
	rtk_rg_callback_pipe_cmd("/bin/ifconfig %s mtu 1500",dev);
	rtk_rg_callback_pipe_cmd("/bin/iptables -A INPUT -i %s -p UDP --dport 69 -d 255.255.255.255 -m state --state NEW -j ACCEPT",dev);

	rtk_rg_callback_pipe_cmd("%s %s up",ifconfig,dev);

	rtk_rg_callback_pipe_cmd("/bin/udhcpc -i %s -W &",dev);
	//rtk_rg_callback_pipe_cmd("/bin/iptables -t nat -A POSTROUTING -o %s -j MASQUERADE",dev);
	
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.pppoeBeforeDiagByHwCallBack;
int _rtk_rg_pppoeBeforeDiagByHwCallBack(rtk_rg_pppoeClientInfoBeforeDial_t* before_diag, int* intfIdx){


	char dev[IFNAMSIZ]; //device name, ex:nas0_0
	int dev_idx;
	//int vid;
	rtk_rg_intfInfo_t intfInfo;
	memcpy(&intfInfo,&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo,sizeof(rtk_rg_intfInfo_t));

	////check wan_intf info & set dev name
	if((*intfIdx>7) || (*intfIdx<0)){
		CBACK("invalid wan_intf_idx\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else{
		//dev_idx = (*intfIdx-1);
		dev_idx = (*intfIdx);	
		sprintf(dev,"eth%d",dev_idx);
	}
	
	if(rg_db.systemGlobal.interfaceInfo[*intfIdx].valid==IF_INVALID_ENTRY){
		CBACK("Assigned wan_intf_idx is not valid.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else if(!intfInfo.is_wan){
		CBACK("Assigned wan_intf_idx is not wan.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else if(intfInfo.wan_intf.wan_intf_conf.wan_type!=RTK_RG_PPPoE){
		CBACK("Assigned wan_intf_idx is not PPPoE.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}
	
	//ps cmd for bring up nas0 & nas0_x for ppp	

	//do cmd
#if defined(CONFIG_RG_G3_SERIES)
	rtk_rg_callback_pipe_cmd("echo %d wan0 > /proc/driver/cortina/ni/dev_port_mapping",intfInfo.wan_intf.wan_intf_conf.wan_port_idx);
#else
    rtk_rg_callback_pipe_cmd("echo %d wan0 > /proc/rtl8686gmac/dev_port_mapping",intfInfo.wan_intf.wan_intf_conf.wan_port_idx);
#endif
	
	//rtk_rg_callback_pipe_cmd("%s nas0 up",ifconfig);
	//rtk_rg_callback_pipe_cmd("/bin/ethctl remsmux bridge nas0 %s",dev);

	//smux: ignor napt cmd in ppp, just care vlan
	/*if(intfInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on){
		//have vlan tag
		vid = intfInfo.wan_intf.wan_intf_conf.egress_vlan_id;
		rtk_rg_callback_pipe_cmd("/bin/ethctl addsmux pppoe nas0 %s nonapt vlan %d",dev,vid); 
	}else{
		//no vlan tag
		rtk_rg_callback_pipe_cmd("/bin/ethctl addsmux pppoe nas0 %s ",dev);
	}*/
	
	rtk_rg_callback_pipe_cmd("%s %s hw ether %02x%02x%02x%02x%02x%02x",ifconfig,dev,
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[0],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[1],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[2],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[3],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[4],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[5]);
	

	rtk_rg_callback_pipe_cmd("%s %s txqueuelen 10",ifconfig,dev);
	rtk_rg_callback_pipe_cmd("%s %s up",ifconfig,dev);


	rtk_rg_callback_pipe_cmd("/bin/ifconfig ppp%d txqueuelen 0",dev_idx);


	/*set proc for Radvd create global v6 ip*/
	rtk_rg_callback_pipe_cmd("/bin/echo 1 > /proc/sys/net/ipv6/conf/ppp%d/autoconf",dev_idx);
	rtk_rg_callback_pipe_cmd("/bin/echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
	rtk_rg_callback_pipe_cmd("/bin/echo 0 > /proc/sys/net/ipv6/conf/ppp%d/forwarding",dev_idx);
	//enable,disable ipv6 ppp diag
	//rtk_rg_callback_pipe_cmd("/bin/echo 1 > proc/sys/net/ipv6/conf/ppp%d/disable_ipv6",dev_idx);

	
	rtk_rg_callback_pipe_cmd("/bin/spppctl add %d pppoe %s username %s password %s gw 1 mru 1492 nohwnat ippt 0 debug 0 ipt 2 ",dev_idx,dev,before_diag->username,before_diag->password);
	rtk_rg_callback_pipe_cmd("/bin/spppctl katimer 100");

	//1 FIX ME: PPPoE disconnet issue
	/*Need to judge if PPPoE diag failed!!!*/
	//rg_kernel.ppp_diaged[dev_idx]=ENABLED;

	rtk_rg_callback_pipe_cmd("/bin/iptables -t nat -A POSTROUTING -o ppp%d -j MASQUERADE",dev_idx);
	
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.arpAddByHwCallBack;
int _rtk_rg_arpAddByHwCallBack(rtk_rg_arpInfo_t* arpInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.arpDelByHwCallBack;
int _rtk_rg_arpDelByHwCallBack(rtk_rg_arpInfo_t* arpInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.macAddByHwCallBack=NULL;
int _rtk_rg_macAddByHwCallBack(rtk_rg_macEntry_t* macInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.macDelByHwCallBack=NULL;
int _rtk_rg_macDelByHwCallBack(rtk_rg_macEntry_t* macInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.naptAddByHwCallBack=NULL;
int _rtk_rg_naptAddByHwCallBack(rtk_rg_naptInfo_t* naptInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.naptDelByHwCallBack=NULL;
int _rtk_rg_naptDelByHwCallBack(rtk_rg_naptInfo_t* naptInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.routingAddByHwCallBack=NULL;
int _rtk_rg_routingAddByHwCallBack(rtk_rg_ipv4RoutingEntry_t* routingInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.routingDelByHwCallBack=NULL;
int _rtk_rg_routingDelByHwCallBack(rtk_rg_ipv4RoutingEntry_t* routingInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.bindingAddByHwCallBack=NULL;
int _rtk_rg_bindingAddByHwCallBack(rtk_rg_bindingEntry_t* bindingInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.bindingDelByHwCallBack=NULL;
int _rtk_rg_bindingDelByHwCallBack(rtk_rg_bindingEntry_t* bindingInfo){
	return SUCCESS;
}


int _rtk_rg_neighborAddByHwCallBack(rtk_rg_neighborInfo_t* neighborInfo){
	return SUCCESS;
}

int _rtk_rg_neighborDelByHwCallBack(rtk_rg_neighborInfo_t* neighborInfo){
	return SUCCESS;
}

int _rtk_rg_v6RoutingAddByHwCallBack(rtk_rg_ipv6RoutingEntry_t* cb_routv6Et){
	return SUCCESS;
}

int _rtk_rg_v6RoutingDelByHwCallBack(rtk_rg_ipv6RoutingEntry_t* cb_routv6Et){
	return SUCCESS;
}

#else		//normal multi eth wan

rtk_rg_wq_union_t *wq_callbackAssign(void)
{
	if(atomic_dec_and_test(&rg_kernel.rg_cbUnionArray[rg_kernel.rg_cbUnionCurrentIdx].not_used)){		
		rtk_rg_wq_union_t *ret;
		ret=&rg_kernel.rg_cbUnionArray[rg_kernel.rg_cbUnionCurrentIdx++];
		rg_kernel.rg_cbUnionCurrentIdx%=CONFIG_RG_CALLBACK_WQ_TOTAL_SIZE;
		return ret;
	}
	CBACK("[WQ]retrieve failed...Please enlarge CONFIG_RG_CALLBACK_WQ_TOTAL_SIZE by kernel menuconfig!!");
	return NULL;
}

//callback for rg init
int __initByHwCallBack(void)
{
	int i,process_pid,ret;
	struct task_struct *t;
	
	//Leo: The /var/tmp will be created after boad had been executed
	//	   So, we check the path /var/tmp instead of boa pid
	if (0!=rtk_rg_callback_pipe_cmd("cd /var/tmp")) {
		rtk_rg_callback_pipe_cmd("/etc/init.d/rc2");
	}

	//Clear all vconfig settings
#if defined(CONFIG_RG_G3_SERIES)
	//If remove eth0.4.300 then remove br0.3001, G3 will call trace: tried to remove device eth0.4 from br0.3001
	rtk_rg_callback_pipe_cmd("`for FILE in $(ls /sys/class/net/ | grep 'eth0.' | grep -v 'eth0.[0-9].'); do vconfig rem $FILE > /dev/null 2>&1; done`");
	rtk_rg_callback_pipe_cmd("`for FILE in $(ls /sys/class/net/ | grep 'br0.'); do vconfig rem $FILE > /dev/null 2>&1; done`");
	rtk_rg_callback_pipe_cmd("`for FILE in $(ls /sys/class/net/ | grep 'eth0.'); do vconfig rem $FILE > /dev/null 2>&1; done`");
#else
	rtk_rg_callback_pipe_cmd("`for FILE in $(ls /sys/class/net/ | grep 'eth0.'); do vconfig rem $FILE > /dev/null 2>&1; done`");
	rtk_rg_callback_pipe_cmd("`for FILE in $(ls /sys/class/net/ | grep 'br0.'); do vconfig rem $FILE > /dev/null 2>&1; done`");
#endif
	rtk_rg_callback_pipe_cmd("%s link set br0 down",ipUtility);
	rtk_rg_callback_pipe_cmd("brctl delbr br0");
	rtk_rg_callback_pipe_cmd("brctl addbr br0");
	rtk_rg_callback_pipe_cmd("brctl setfd br0 0");	
	rtk_rg_callback_pipe_cmd("%s addr flush dev br0",ipUtility);		//reset br0's address information
	
#ifdef CONFIG_RG_WMUX_SUPPORT
	wmux_cleanup_devices();
#endif

	rtk_rg_callback_pipe_cmd("`for FILE in $(ls /sys/class/net/ | grep '%s'); do %s link set $FILE down > /dev/null 2>&1; done`", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME, ipUtility);
	rtk_rg_callback_pipe_cmd("`for FILE in $(ls /sys/class/net/ | grep '%s'); do brctl addif br0 $FILE > /dev/null 2>&1; done`", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
	rtk_rg_callback_pipe_cmd("`for FILE in $(ls /sys/class/net/ | grep 'eth0.'); do brctl addif br0 $FILE > /dev/null 2>&1; done`");
	
	rtk_rg_callback_pipe_cmd("%s link set %s down",ipUtility,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME);

	for(i=0;i<MAX_NETIF_SW_TABLE_SIZE;i++)
	{
		//rtk_rg_callback_pipe_cmd("brctl delbr br_%s_%d",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,i);
		rtk_rg_callback_pipe_cmd("%s link set br_%s_%d down",ipUtility,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,i);
		rtk_rg_callback_pipe_cmd("ethctl remsmux bridge %s %s_%d",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,i);

#ifdef CONFIG_GPON_FEATURE
		//20150312LUKE: trigger OMCI to remove related CF rules, parameters: netIfIdx, vid, pri, type, service, isBinding, bAdd
		if(rg_db.systemGlobal.initParam.wanPortGponMode)
			rtk_rg_callback_pipe_cmd("echo %d -1 -1 -1 -1 -1 0 > /proc/omci/wanInfo",i);
#endif
		//1 FIX ME: PPPoE disconnet issue
		/*
		if(rg_kernel.ppp_diaged[i]==ENABLED){
			rtk_rg_callback_pipe_cmd("/bin/spppctl down ppp%d",i);
			rg_kernel.ppp_diaged[i]=DISABLED;
		}
		*/
		//rtk_rg_callback_pipe_cmd("/bin/spppctl down ppp%d",i);
		//rtk_rg_callback_pipe_cmd("/bin/spppd");
	}

#if defined(CONFIG_RG_G3_SERIES)

#if defined(CONFIG_RTL_MULTI_LAN_DEV)
	for(i=0;i<RTK_RG_MAC_PORT_PON;i++)
	{
		if(RG_INVALID_MAC_PORT(i)) continue;
		//reset all port-mapping to multi-LAN dev
		rtk_rg_callback_pipe_cmd("echo %d eth0.%d > /proc/driver/cortina/ni/dev_port_mapping",i,i+2);
	}
#endif

#elif defined(CONFIG_APOLLO_MP)
	for(i=0;i<RTK_RG_MAC_PORT_MAX;i++)
	{
		if(RG_INVALID_MAC_PORT(i)) continue;
		//reset all port-mapping to eth0
		rtk_rg_callback_pipe_cmd("echo %d %s > /proc/rtl8686gmac/dev_port_mapping", i, CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
	}
#if defined(CONFIG_RTL_MULTI_LAN_DEV)
	for(i=0;i<RTK_RG_MAC_PORT_PON;i++)
	{
		if(RG_INVALID_MAC_PORT(i)) continue;
		//reset all port-mapping to multi-LAN dev
		rtk_rg_callback_pipe_cmd("echo %d eth0.%d > /proc/rtl8686gmac/dev_port_mapping",i,i+2);
	}
#endif

#elif defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) //end defined(CONFIG_APOLLO_MP) elif defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) 

	CBACK("FIXME XDSL callback\n");

#endif //end defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) 

	//Clear all udhcpc daemon
	_rg_delete_dhcpc(NULL,1);

	//Clear all udhcpd daemon
	_rg_delete_dhcpd(0,1);

	//Clear all radvd daemon
	_rg_delete_radvd(0,1);

	//Clear all un-terminated PPPoE connections
	//20170428LUKE: check for auto test mode
	rtk_rg_callback_pipe_cmd("`for FILE in $(ps | grep 'spppd' | sed 's/^[ \t]*//g;s/[ \t][ \t]*.*//g'); do kill -9 $FILE > /dev/null 2>&1; done`");
	if(!rg_kernel.autoTestMode){
		rtk_rg_callback_pipe_cmd("/bin/spppd > /dev/null 2>&1 &");
		rtk_rg_callback_pipe_cmd("`for FILE in $(ls /sys/class/net/ | grep 'ppp'); do spppctl del $FILE > /dev/null 2>&1; done`");
	}

	//Clear un-terminated IGMP proxy client
	//1 FIXME:if there is two or more WAN use IGMP Proxy, this may not working
	process_pid = read_pid((char*)IGMPPROXY_PID);
	CBACK("the igmp_proxy_pid is %d",process_pid);
	if (process_pid > 0)
	{
		t = get_pid_task(find_get_pid(process_pid), PIDTYPE_PID);
		if(t == NULL){
			CBACK("no such pid");
		}
		else
		{
			ret = send_sig(SIGTERM,t,0);//send_sig_info(SIG_TEST, &info, t);	//send the signal
			if (ret < 0) {
				CBACK("error sending signal\n");
			}
		}
	}

	//Clear all Firewall rules
	rtk_rg_callback_pipe_cmd("iptables -F");
	rtk_rg_callback_pipe_cmd("iptables -X");
	rtk_rg_callback_pipe_cmd("iptables -P INPUT ACCEPT");
	rtk_rg_callback_pipe_cmd("iptables -P FORWARD DROP");	//disable protocol stack forwarding IPv4
	rtk_rg_callback_pipe_cmd("iptables -P OUTPUT ACCEPT");
	//20140627LUKE:fix rg init didn't clear SNAT rules problem
	rtk_rg_callback_pipe_cmd("iptables -t nat -F");
	rtk_rg_callback_pipe_cmd("iptables -t nat -X");
	rtk_rg_callback_pipe_cmd("iptables -t nat -P PREROUTING ACCEPT");
	rtk_rg_callback_pipe_cmd("iptables -t nat -P POSTROUTING ACCEPT");
	rtk_rg_callback_pipe_cmd("iptables -t nat -P OUTPUT ACCEPT");
	
	rtk_rg_callback_pipe_cmd("ip6tables -F");
	rtk_rg_callback_pipe_cmd("ip6tables -X");
	rtk_rg_callback_pipe_cmd("ip6tables -P INPUT ACCEPT");
	rtk_rg_callback_pipe_cmd("ip6tables -P FORWARD DROP");	//disable protocol stack forwarding IPv6
	rtk_rg_callback_pipe_cmd("ip6tables -P OUTPUT ACCEPT");
	rtk_rg_callback_pipe_cmd("ebtables -X");
	rtk_rg_callback_pipe_cmd("ebtables -F");
	rtk_rg_callback_pipe_cmd("ebtables -t broute -X");
	rtk_rg_callback_pipe_cmd("ebtables -t broute -F");
	rtk_rg_callback_pipe_cmd("ebtables -P INPUT ACCEPT");
	rtk_rg_callback_pipe_cmd("ebtables -P FORWARD DROP");
	rtk_rg_callback_pipe_cmd("ebtables -P OUTPUT ACCEPT");

	//patch for ipatbles
	_delete_iptables_snat();

	//20140617LUKE:we do not let protocol stack forward ANY packet, so just change default policy above.
	//fix ptorocol stack send duplicated broadcast and multicast packets problem
	/*rtk_rg_callback_pipe_cmd("ebtables -D FORWARD -j disBCMC");
	rtk_rg_callback_pipe_cmd("ebtables -X disBCMC");
	
	rtk_rg_callback_pipe_cmd("ebtables -N disBCMC");
	rtk_rg_callback_pipe_cmd("ebtables -A disBCMC -d Broadcast -j DROP");
	rtk_rg_callback_pipe_cmd("ebtables -A disBCMC -d Multicast -j DROP");
	rtk_rg_callback_pipe_cmd("ebtables -I FORWARD 1 -j disBCMC");*/

	//enable IGMP query if IGMP snooping is turn on in RG
	RGDB_LOCK
	if(rg_db.systemGlobal.initParam.igmpSnoopingEnable && rg_db.systemGlobal.multicastProtocol != RG_MC_MLD_ONLY)
	{RGDB_UNLOCK
		rtk_rg_callback_pipe_cmd("echo 1 > /proc/br_igmpquery");
	}
	else
	{RGDB_UNLOCK
		rtk_rg_callback_pipe_cmd("echo 0 > /proc/br_igmpquery");
	}
	RGDB_LOCK
	if(rg_db.systemGlobal.initParam.igmpSnoopingEnable && rg_db.systemGlobal.multicastProtocol != RG_MC_IGMP_ONLY)
	{RGDB_UNLOCK
		rtk_rg_callback_pipe_cmd("echo 1 > proc/br_mldquery");
	}
	else
	{RGDB_UNLOCK
		rtk_rg_callback_pipe_cmd("echo 0 > proc/br_mldquery");
	}



	//flush all route table
	rtk_rg_callback_pipe_cmd("%s route flush table all",ipUtility);

#ifdef CONFIG_DUALBAND_CONCURRENT	
	//reset route for IPC
	rtk_rg_callback_pipe_cmd("%s link set vwlan down",ipUtility);
	rtk_rg_callback_pipe_cmd("%s link set vwlan up",ipUtility);
#endif
	//reset route for lo
	rtk_rg_callback_pipe_cmd("%s route add 127.0.0.0/24 dev lo",ipUtility);

	//20141021 Leo Force system to release memory
	//rtk_rg_callback_pipe_cmd("sync");
	rtk_rg_callback_pipe_cmd("echo 3 > /proc/sys/vm/drop_caches");
	rtk_rg_callback_pipe_cmd("echo 0 > /proc/sys/vm/drop_caches");

	//20141205LUKE: turn on eth0 in case packet won't be received by NIC
	rtk_rg_callback_pipe_cmd("%s link set %s up", ipUtility, CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
#if defined(CONFIG_RG_G3_SERIES)
	rtk_rg_callback_pipe_cmd("%s link set %s up", ipUtility, CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME);
	rtk_rg_callback_pipe_cmd("%s link set wlan0 up", ipUtility);
	rtk_rg_callback_pipe_cmd("%s link set wlan0-vap0 up", ipUtility);
#endif

	return SUCCESS;

}
void wq_do_initByHwCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	__initByHwCallBack();
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}
int _rtk_rg_initParameterSetByHwCallBack(void)
{
	rtk_rg_wq_union_t *initWq=wq_callbackAssign();
	if(initWq){
		INIT_WORK(&(initWq->work), wq_do_initByHwCallBack);
		queue_work(rg_kernel.rg_callbackWQ,&(initWq->work));
	}
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.arpAddByHwCallBack;
int _rtk_rg_arpAddByHwCallBack(rtk_rg_arpInfo_t* arpInfo){	
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.arpDelByHwCallBack;
int _rtk_rg_arpDelByHwCallBack(rtk_rg_arpInfo_t* arpInfo){
	return SUCCESS;
}

rtk_mac_t g_whiteList[WHITELISTSIZE] = 
{{.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}},
 {.octet = {0,0,0,0,0,0}}};
unsigned int g_whiteList_idx = 0;
int _rtk_rg_macAddByHwCallBack(rtk_rg_macEntry_t* macInfo){
	int i, macIdx, isblock=1;
	
	if(rg_db.systemGlobal.whiteListState)
	{
		if(macInfo->static_entry==0 && macInfo->sa_block==0 && (rg_db.systemGlobal.lanPortMask.portmask & (0x1<<macInfo->port_idx)))
		{
			for(i=0; i<WHITELISTSIZE; i++)
			{
				if(memcmp(g_whiteList[i].octet, macInfo->mac.octet,ETHER_ADDR_LEN)==0)
				{
					isblock=0;
					break;
				}
			}
			if(isblock)
			{
				macInfo->sa_block=1;
				assert_ok((pf.rtk_rg_macEntry_add)(macInfo, &macIdx));
				rtlglue_printf("[rg callback] block MAC[%02x:%02x:%02x:%02x:%02x:%02x]\n", macInfo->mac.octet[0], macInfo->mac.octet[1], macInfo->mac.octet[2], macInfo->mac.octet[3], macInfo->mac.octet[4], macInfo->mac.octet[5]);
			}
		}	
	}
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.macDelByHwCallBack=NULL;
int _rtk_rg_macDelByHwCallBack(rtk_rg_macEntry_t* macInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.routingAddByHwCallBack=NULL;
int __routingAddByHwCallBack(rtk_rg_ipv4RoutingEntry_t* routingInfo)
{
	unsigned char ip[16],mask[16];
	int taggingCPU,vlanId;
	RGDB_LOCK
	if(rg_db.systemGlobal.interfaceInfo[routingInfo->wan_intf_idx].valid &&
		rg_db.systemGlobal.interfaceInfo[routingInfo->wan_intf_idx].storedInfo.is_wan==0 &&
		rg_db.systemGlobal.interfaceInfo[routingInfo->wan_intf_idx].p_lanIntfConf->ip_addr!=routingInfo->dest_ip)
	{
		//add one more subnet with same interface!! sync to protocol stack
		taggingCPU=(rg_db.systemGlobal.interfaceInfo[routingInfo->wan_intf_idx].p_lanIntfConf->untag_mask.portmask&RTK_RG_ALL_MAC_MASTER_CPU_PORTMASK)?0:1;
		vlanId=rg_db.systemGlobal.interfaceInfo[routingInfo->wan_intf_idx].p_lanIntfConf->intf_vlan_id;
		RGDB_UNLOCK
	
		//add lan interface
		memcpy(ip,(void*)_inet_ntoa(routingInfo->dest_ip),16);
		memcpy(mask,(void*)_inet_ntoa(routingInfo->ip_mask),16);
		if(taggingCPU)
		{		
			//CPU tagged, set br0.vid
			if(routingInfo->ip_mask != 0)
			{
				rtk_rg_callback_pipe_cmd("%s addr add %s/%s broadcast + dev br0.%d",ipUtility,ip,mask,vlanId);
			}
		}
		else
		{		
			//CPU untag, set br0 directly
			if(routingInfo->ip_mask != 0)
			{
				rtk_rg_callback_pipe_cmd("%s addr add %s/%s broadcast + dev br0",ipUtility,ip,mask);
			}
		}
		RGDB_LOCK
	}
	RGDB_UNLOCK
	return SUCCESS;
}
void wq_do_routingAddByHwCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	__routingAddByHwCallBack(&p_callback_work->v4RoutingInfo);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}
int _rtk_rg_routingAddByHwCallBack(rtk_rg_ipv4RoutingEntry_t* routingInfo)
{
	rtk_rg_wq_union_t *routingAddWq=wq_callbackAssign();
	if(routingAddWq){
		INIT_WORK(&(routingAddWq->work), wq_do_routingAddByHwCallBack);
		memcpy(&(routingAddWq->v4RoutingInfo),routingInfo,sizeof(rtk_rg_ipv4RoutingEntry_t));	
		queue_work(rg_kernel.rg_callbackWQ,&(routingAddWq->work));
	}
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.routingDelByHwCallBack=NULL;
int __routingDelByHwCallBack(rtk_rg_ipv4RoutingEntry_t* routingInfo)
{
	unsigned char ip[16];

	//20140616LUKE: when we deleting IPv4 routing entry from NAPT WAN, deleting iptables's SNAT rule
	RGDB_LOCK
	if(rg_db.systemGlobal.interfaceInfo[routingInfo->wan_intf_idx].valid &&
		rg_db.systemGlobal.interfaceInfo[routingInfo->wan_intf_idx].storedInfo.is_wan && 
		(rg_db.systemGlobal.interfaceInfo[routingInfo->wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_STATIC) &&
		rg_db.systemGlobal.interfaceInfo[routingInfo->wan_intf_idx].p_wanStaticInfo->ip_version!=IPVER_V6ONLY &&
		rg_db.systemGlobal.interfaceInfo[routingInfo->wan_intf_idx].p_wanStaticInfo->napt_enable)
	{	
		memcpy(ip,(void*)_inet_ntoa(rg_db.systemGlobal.interfaceInfo[routingInfo->wan_intf_idx].p_wanStaticInfo->ip_addr),16);
		RGDB_UNLOCK
		rtk_rg_callback_pipe_cmd("iptables -t nat -D POSTROUTING -o %s_%d -j SNAT --to-source %s",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,routingInfo->wan_intf_idx,ip);
		RGDB_LOCK
	}
	RGDB_UNLOCK
	return SUCCESS;
}
void wq_do_routingDelByHwCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	__routingDelByHwCallBack(&p_callback_work->v4RoutingInfo);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}
int _rtk_rg_routingDelByHwCallBack(rtk_rg_ipv4RoutingEntry_t* routingInfo)
{
	rtk_rg_wq_union_t *routingDelWq=wq_callbackAssign();
	if(routingDelWq){
		INIT_WORK(&(routingDelWq->work), wq_do_routingDelByHwCallBack);
		memcpy(&(routingDelWq->v4RoutingInfo),routingInfo,sizeof(rtk_rg_ipv4RoutingEntry_t));	
		queue_work(rg_kernel.rg_callbackWQ,&(routingDelWq->work));
	}
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.naptAddByHwCallBack=NULL;
int _rtk_rg_naptAddByHwCallBack(rtk_rg_naptInfo_t* naptInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.naptDelByHwCallBack=NULL;
int _rtk_rg_naptDelByHwCallBack(rtk_rg_naptInfo_t* naptInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.bindingAddByHwCallBack=NULL;
void _rtk_rg_checkVlanBindingAndAddVconfig(rtk_rg_port_idx_t portIdx, int vlanId)
{
	int i,lanTotalNum;
	rtk_mac_t gmac;
	rtk_rg_portmask_t portmask;
	RGDB_LOCK
	lanTotalNum=rg_db.systemGlobal.lanIntfTotalNum;
	RGDB_UNLOCK
	for(i=0;i<lanTotalNum;i++)
	{
		RGDB_LOCK
		portmask.portmask=rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->port_mask.portmask;
		memcpy(&gmac,&rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->gmac,sizeof(rtk_mac_t));
		RGDB_UNLOCK
		if(portmask.portmask&(0x1<<portIdx))
		{
#if defined(CONFIG_RTL_MULTI_LAN_DEV)
			if(portIdx>=RTK_RG_PORT_LASTCPU)
			{
				//add virtual ether device for VLAN
				rtk_rg_callback_pipe_cmd("vconfig add %s %d", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME, vlanId);
				rtk_rg_callback_pipe_cmd("%s link set eth0.%d address %02x:%02x:%02x:%02x:%02x:%02x",ipUtility,vlanId,
					gmac.octet[0],
					gmac.octet[1],
					gmac.octet[2],
					gmac.octet[3],
					gmac.octet[4],
					gmac.octet[5]);
				
				rtk_rg_callback_pipe_cmd("%s link set eth0.%d up",ipUtility,vlanId);
				rtk_rg_callback_pipe_cmd("brctl addif br0 eth0.%d",vlanId);
				rtk_rg_callback_pipe_cmd("ebtables -t broute -A BROUTING -i %s -p 802_1Q -j DROP", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
			}
			else
			{
				//add virtual ether device for VLAN
				rtk_rg_callback_pipe_cmd("vconfig add eth0.%d %d",2+portIdx,vlanId);
				rtk_rg_callback_pipe_cmd("%s link set eth0.%d.%d address %02x:%02x:%02x:%02x:%02x:%02x",ipUtility,2+portIdx,vlanId,
					gmac.octet[0],
					gmac.octet[1],
					gmac.octet[2],
					gmac.octet[3],
					gmac.octet[4],
					gmac.octet[5]);
				
				rtk_rg_callback_pipe_cmd("%s link set eth0.%d.%d up",ipUtility,2+portIdx,vlanId);
				rtk_rg_callback_pipe_cmd("brctl addif br0 eth0.%d.%d",2+portIdx,vlanId);
				rtk_rg_callback_pipe_cmd("ebtables -t broute -A BROUTING -i eth0.%d -p 802_1Q -j DROP",2+portIdx);
			}
#else
			//add virtual ether device for VLAN
			rtk_rg_callback_pipe_cmd("vconfig add %s %d", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME, vlanId);
			rtk_rg_callback_pipe_cmd("%s link set eth0.%d address %02x:%02x:%02x:%02x:%02x:%02x",ipUtility,vlanId,
				gmac.octet[0],
				gmac.octet[1],
				gmac.octet[2],
				gmac.octet[3],
				gmac.octet[4],
				gmac.octet[5]);
			
			rtk_rg_callback_pipe_cmd("%s link set eth0.%d up",ipUtility,vlanId);
			rtk_rg_callback_pipe_cmd("brctl addif br0 eth0.%d",vlanId);
			rtk_rg_callback_pipe_cmd("ebtables -t broute -A BROUTING -i %s -p 802_1Q -j DROP", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
#endif
			break;
		}
	}
}
int __bindingAddByHwCallBack(rtk_rg_bindingEntry_t* bindingInfo)
{	
	if(bindingInfo->type==BIND_TYPE_VLAN)
	{	
		//WARNING("vlan binding: vid is %d, port is %d",bindingInfo->vlan.vlan_bind_vlan_id,bindingInfo->vlan.vlan_bind_port_idx);
		_rtk_rg_checkVlanBindingAndAddVconfig(bindingInfo->vlan.vlan_bind_port_idx,bindingInfo->vlan.vlan_bind_vlan_id);
#ifdef CONFIG_GPON_FEATURE
	//20160314LUKE: trigger OMCI to recreate related CF rules, parameters: netIfIdx, vid, pri, type, service, isBinding, bAdd
	if(rg_db.systemGlobal.initParam.wanPortGponMode)
	{
		RGDB_LOCK
		if(rg_db.systemGlobal.interfaceInfo[bindingInfo->wan_intf_idx].storedInfo.is_wan && rg_db.systemGlobal.interfaceInfo[bindingInfo->wan_intf_idx].storedInfo.wan_intf.wirelessWan==RG_WWAN_WIRED){
			rtk_rg_intfInfo_t localIntfInfo,*intfInfo;
			memcpy(&localIntfInfo,&rg_db.systemGlobal.interfaceInfo[bindingInfo->wan_intf_idx].storedInfo,sizeof(rtk_rg_intfInfo_t));
			RGDB_UNLOCK
			intfInfo=&localIntfInfo;
			//Remove first, then add back
			rtk_rg_callback_pipe_cmd("echo %d -1 -1 -1 -1 -1 0 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx);
			if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
				if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
					rtk_rg_callback_pipe_cmd("echo %d %d %d 0 %d %d 1 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx,intfInfo->wan_intf.wan_intf_conf.egress_vlan_id,
						intfInfo->wan_intf.wan_intf_conf.vlan_based_pri_enable?intfInfo->wan_intf.wan_intf_conf.vlan_based_pri:-1,
						intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
						(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
				else
					rtk_rg_callback_pipe_cmd("echo %d -1 -1 0 %d %d 1 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx,
						intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
						(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
			}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE)			{
				if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
					rtk_rg_callback_pipe_cmd("echo %d %d %d 2 %d %d 1 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx,intfInfo->wan_intf.wan_intf_conf.egress_vlan_id,
						intfInfo->wan_intf.wan_intf_conf.vlan_based_pri_enable?intfInfo->wan_intf.wan_intf_conf.vlan_based_pri:-1,
						intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
						(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
				else
					rtk_rg_callback_pipe_cmd("echo %d -1 -1 2 %d %d 1 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx,
						intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
						(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
			}else if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
				rtk_rg_callback_pipe_cmd("echo %d %d %d 1 %d %d  1 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx,intfInfo->wan_intf.wan_intf_conf.egress_vlan_id,
					intfInfo->wan_intf.wan_intf_conf.vlan_based_pri_enable?intfInfo->wan_intf.wan_intf_conf.vlan_based_pri:-1,
					intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
					(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
			else
				rtk_rg_callback_pipe_cmd("echo %d -1 -1 1 %d %d 1 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx,
					intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
					(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
			RGDB_LOCK
		}
		RGDB_UNLOCK
	}
#endif
	}
	
	return SUCCESS;
}
void wq_do_bindingAddByHwCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	__bindingAddByHwCallBack(&p_callback_work->bindInfo);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}
int _rtk_rg_bindingAddByHwCallBack(rtk_rg_bindingEntry_t* bindingInfo)
{
	rtk_rg_wq_union_t *bindAddWq=wq_callbackAssign();
	if(bindAddWq){
		INIT_WORK(&(bindAddWq->work), wq_do_bindingAddByHwCallBack);
		memcpy(&(bindAddWq->bindInfo),bindingInfo,sizeof(rtk_rg_bindingEntry_t)); 
		queue_work(rg_kernel.rg_callbackWQ,&(bindAddWq->work));
	}
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.bindingDelByHwCallBack=NULL;
int __bindingDelByHwCallBack(rtk_rg_bindingEntry_t* bindingInfo)
{
	int i;
	
	if(bindingInfo->type==BIND_TYPE_VLAN)
	{
		//WARNING("vlan binding: vid is %d, port is %d",bindingInfo->vlan.vlan_bind_vlan_id,bindingInfo->vlan.vlan_bind_port_idx);
		
		//del virtual ether device for VLAN
#if defined(CONFIG_RTL_MULTI_LAN_DEV)
		if(bindingInfo->vlan.vlan_bind_port_idx>=RTK_RG_PORT_LASTCPU)
		{
			rtk_rg_callback_pipe_cmd("vconfig rem eth0.%d",bindingInfo->vlan.vlan_bind_vlan_id);
			rtk_rg_callback_pipe_cmd("ebtables -t broute -D BROUTING -i %s -p 802_1Q -j DROP", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
		}
		else
		{
			rtk_rg_callback_pipe_cmd("vconfig rem eth0.%d.%d",2+bindingInfo->vlan.vlan_bind_port_idx,bindingInfo->vlan.vlan_bind_vlan_id);
			rtk_rg_callback_pipe_cmd("ebtables -t broute -D BROUTING -i eth0.%d -p 802_1Q -j DROP",2+bindingInfo->vlan.vlan_bind_port_idx);
		}
#else
		rtk_rg_callback_pipe_cmd("vconfig rem eth0.%d",bindingInfo->vlan.vlan_bind_vlan_id);
		rtk_rg_callback_pipe_cmd("ebtables -t broute -D BROUTING -i %s -p 802_1Q -j DROP", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
#endif

		//re-check all Vlan-binding rule for re-create vconfig 
		for(i=0;i<MAX_BIND_SW_TABLE_SIZE;i++)
		{
			rtk_rg_table_bind_t localBind;
			RGDB_LOCK
			memcpy(&localBind,&rg_db.bind[i],sizeof(rtk_rg_table_bind_t));
			RGDB_UNLOCK
			if(localBind.valid && localBind.rtk_bind.vidLan!=0)
			{	
				if(localBind.rtk_bind.portMask.bits[0]>0)
				{
					if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT0))
						_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT0,localBind.rtk_bind.vidLan);
					else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT1))
						_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT1,localBind.rtk_bind.vidLan);
#if !defined(CONFIG_RG_RTL9602C_SERIES)					
					else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT2))
						_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT2,localBind.rtk_bind.vidLan);
					else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT3))
						_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT3,localBind.rtk_bind.vidLan);
#if !defined(CONFIG_RG_G3_SERIES)					
					else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT_RGMII))
						_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT_RGMII,localBind.rtk_bind.vidLan);
#endif					
#endif					
					else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT_PON)) 					
						_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT_PON,localBind.rtk_bind.vidLan);
					else
						CBACK("vlan-binding[%d] portMask wrong...%x",i,localBind.rtk_bind.portMask.bits[0]);
				}
				else
				{
					if(localBind.rtk_bind.extPortMask.bits[0]&(0x1<<(RTK_RG_EXT_PORT0-RTK_RG_EXT_PORT0)))
						_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_EXT_PORT0,localBind.rtk_bind.vidLan);
					else if(localBind.rtk_bind.extPortMask.bits[0]&(0x1<<(RTK_RG_EXT_PORT1-RTK_RG_EXT_PORT0)))
						_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_EXT_PORT1,localBind.rtk_bind.vidLan);
					else if(localBind.rtk_bind.extPortMask.bits[0]&(0x1<<(RTK_RG_EXT_PORT2-RTK_RG_EXT_PORT0)))
						_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_EXT_PORT2,localBind.rtk_bind.vidLan);
					else if(localBind.rtk_bind.extPortMask.bits[0]&(0x1<<(RTK_RG_EXT_PORT3-RTK_RG_EXT_PORT0)))
						_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_EXT_PORT3,localBind.rtk_bind.vidLan);
#if !defined(CONFIG_RG_G3_SERIES)					
					else if(localBind.rtk_bind.extPortMask.bits[0]&(0x1<<(RTK_RG_EXT_PORT4-RTK_RG_EXT_PORT0)))
						_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_EXT_PORT4,localBind.rtk_bind.vidLan);
#endif					
					else
						CBACK("vlan-binding[%d] ext-portMask wrong...%x",i,localBind.rtk_bind.extPortMask.bits[0]);
				}
			}
		}
#ifdef CONFIG_GPON_FEATURE
	//20160314LUKE: trigger OMCI to recreate related CF rules, parameters: netIfIdx, vid, pri, type, service, isBinding, bAdd
	if(rg_db.systemGlobal.initParam.wanPortGponMode)
	{
		RGDB_LOCK
		if(rg_db.systemGlobal.interfaceInfo[bindingInfo->wan_intf_idx].storedInfo.is_wan && rg_db.systemGlobal.interfaceInfo[bindingInfo->wan_intf_idx].storedInfo.wan_intf.wirelessWan==RG_WWAN_WIRED){
			rtk_rg_intfInfo_t localIntfInfo,*intfInfo;
			memcpy(&localIntfInfo,&rg_db.systemGlobal.interfaceInfo[bindingInfo->wan_intf_idx].storedInfo,sizeof(rtk_rg_intfInfo_t));
			RGDB_UNLOCK
			intfInfo=&localIntfInfo;
			//Remove first, then add back
			rtk_rg_callback_pipe_cmd("echo %d -1 -1 -1 -1 -1 0 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx);
			if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
				if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
					rtk_rg_callback_pipe_cmd("echo %d %d %d 0 %d %d 1 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx,intfInfo->wan_intf.wan_intf_conf.egress_vlan_id,
						intfInfo->wan_intf.wan_intf_conf.vlan_based_pri_enable?intfInfo->wan_intf.wan_intf_conf.vlan_based_pri:-1,
						intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
						(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
				else
					rtk_rg_callback_pipe_cmd("echo %d -1 -1 0 %d %d 1 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx,
						intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
						(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
			}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE)			{
				if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
					rtk_rg_callback_pipe_cmd("echo %d %d %d 2 %d %d 1 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx,intfInfo->wan_intf.wan_intf_conf.egress_vlan_id,
						intfInfo->wan_intf.wan_intf_conf.vlan_based_pri_enable?intfInfo->wan_intf.wan_intf_conf.vlan_based_pri:-1,
						intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
						(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
				else
					rtk_rg_callback_pipe_cmd("echo %d -1 -1 2 %d %d 1 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx,
						intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
						(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
			}else if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
				rtk_rg_callback_pipe_cmd("echo %d %d %d 1 %d %d  1 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx,intfInfo->wan_intf.wan_intf_conf.egress_vlan_id,
					intfInfo->wan_intf.wan_intf_conf.vlan_based_pri_enable?intfInfo->wan_intf.wan_intf_conf.vlan_based_pri:-1,
					intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
					(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
			else
				rtk_rg_callback_pipe_cmd("echo %d -1 -1 1 %d %d 1 > /proc/omci/wanInfo",bindingInfo->wan_intf_idx,
					intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
					(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
			RGDB_LOCK
		}
		RGDB_UNLOCK
	}
#endif

	}
	return SUCCESS;
}
void wq_do_bindingDelByHwCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	__bindingDelByHwCallBack(&p_callback_work->bindInfo);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}
int _rtk_rg_bindingDelByHwCallBack(rtk_rg_bindingEntry_t* bindingInfo)
{
	rtk_rg_wq_union_t *bindDelWq=wq_callbackAssign();
	if(bindDelWq){
		INIT_WORK(&(bindDelWq->work), wq_do_bindingDelByHwCallBack);
		memcpy(&(bindDelWq->bindInfo),bindingInfo,sizeof(rtk_rg_bindingEntry_t)); 
		queue_work(rg_kernel.rg_callbackWQ,&(bindDelWq->work));
	}
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.interfaceAddByHwCallBack;
int __interfaceAddByHwCallBack(rtk_rg_intfInfo_t* intfInfo, int* intfIdx)
{
	int i,taggingCPU;
	unsigned char ip[16];
	unsigned char gw_ip[16];
	unsigned char mask[16];
	unsigned char brcast_ip[16];
	
	if(intfInfo->is_wan)
	{
		//add wan interface
		char dev[IFNAMSIZ]; //device name, ex:nas0_0
		char* wan_type; //wan type: ex:PPPoE
		int vid=0;
		rtk_ip_addr_t bc_ip; //broadCast ip for wan intf

		switch(intfInfo->wan_intf.wan_intf_conf.wan_type)
		{
			case RTK_RG_STATIC:
				wan_type="ipoe";
				break;
			case RTK_RG_DHCP:
				wan_type="dhcp";
				break;
			case RTK_RG_PPPoE:
				wan_type="pppoe";
				break;
			case RTK_RG_BRIDGE:
				wan_type="bridge";
				break;
			case RTK_RG_PPTP:
				wan_type="pptp";
				break;
			case RTK_RG_L2TP:
				wan_type="l2tp";
				break;
			case RTK_RG_DSLITE:
				wan_type="dslite";
				break;
			case RTK_RG_PPPoE_DSLITE:
				wan_type="pppoe_dslite";
				break;
			default:
				wan_type=" ";
				break;
		}

		if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
			vid = intfInfo->wan_intf.wan_intf_conf.egress_vlan_id;

		if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WLAN0_VXD)
			sprintf(dev,"wlan0-vxd");
		else if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WLAN1_VXD)
			sprintf(dev,"wlan1-vxd");
		else
			sprintf(dev,"%s_%d",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,(*intfIdx));

#if defined(CONFIG_RG_G3_SERIES)
		//do cmd
		rtk_rg_callback_pipe_cmd("echo %d %s > /proc/driver/cortina/ni/dev_port_mapping",intfInfo->wan_intf.wan_intf_conf.wan_port_idx,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME);

#elif defined(CONFIG_APOLLO_MP)
		//do cmd
		rtk_rg_callback_pipe_cmd("echo %d %s > /proc/rtl8686gmac/dev_port_mapping",intfInfo->wan_intf.wan_intf_conf.wan_port_idx,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME);

#elif defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) //end defined(CONFIG_APOLLO_MP) elif defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) 

		CBACK("FIXME XDSL callback\n");

#endif //end defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) 

		//create nas0_X
		if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE)
		{
			if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WIRED)
			{
				rtk_rg_callback_pipe_cmd("%s link set %s up",ipUtility,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME);
#ifdef CONFIG_RG_WMUX_SUPPORT
				rtk_rg_callback_pipe_cmd("echo %s %d > /proc/rg/wmux_add",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,(*intfIdx));
#else
				rtk_rg_callback_pipe_cmd("ethctl remsmux bridge %s %s",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev);
				
				if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on){
					//VALN TAG
					rtk_rg_callback_pipe_cmd("ethctl addsmux %s %s %s napt brpppoe vlan %d",wan_type,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev,vid); 
				}
				else{
					rtk_rg_callback_pipe_cmd("ethctl addsmux %s %s %s napt brpppoe",wan_type,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev);
				}
#endif
			}
			
			rtk_rg_callback_pipe_cmd("%s link set %s address %02x:%02x:%02x:%02x:%02x:%02x",ipUtility,dev,
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[0],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[1],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[2],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[3],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[4],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[5]);

			rtk_rg_callback_pipe_cmd("%s link set %s txqueuelen 10",ipUtility,dev);
			rtk_rg_callback_pipe_cmd("%s link set %s up",ipUtility,dev);
			rtk_rg_callback_pipe_cmd("brctl addif br0 %s",dev);

			//20140620LUKE:if add bridge tagged WAN, we should add virtual ether device for it's VLAN
			if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
			{
				rtk_rg_callback_pipe_cmd("vconfig add br0 %d",intfInfo->wan_intf.wan_intf_conf.egress_vlan_id);
				rtk_rg_callback_pipe_cmd("%s link set br0.%d up",ipUtility,intfInfo->wan_intf.wan_intf_conf.egress_vlan_id);
			}
		}
		else if(strlen(intfInfo->intf_name))	//not called by rtk_rg_apollo_wanInterface_add, but by _rtk_rg_internal_wanSet
		{	
			rtk_rg_ipStaticInfo_t *static_info;
			if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP)
			{
				static_info=&intfInfo->wan_intf.dhcp_client_info.hw_info;
				memcpy(ip,(void*)_inet_ntoa(static_info->ip_addr),16);
				memcpy(gw_ip,(void*)_inet_ntoa(static_info->gateway_ipv4_addr),16);
				memcpy(mask,(void*)_inet_ntoa(static_info->ip_network_mask),16);

				//configure wan interface in protocol stack
				_rg_configure_protocol_stack(*intfIdx,dev);
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE)
			{
				static_info=&intfInfo->wan_intf.pppoe_info.after_dial.hw_info;
				memcpy(ip,(void*)_inet_ntoa(static_info->ip_addr),16);
				memcpy(gw_ip,(void*)_inet_ntoa(static_info->gateway_ipv4_addr),16);
				memcpy(mask,(void*)_inet_ntoa(static_info->ip_network_mask),16);
			}
			else
			{
				if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPTP)
					static_info=&intfInfo->wan_intf.pptp_info.after_dial.hw_info;
				else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_L2TP)
					static_info=&intfInfo->wan_intf.l2tp_info.after_dial.hw_info;
				else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DSLITE)
					static_info=&intfInfo->wan_intf.dslite_info.static_info;
				else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE_DSLITE)
					static_info=&intfInfo->wan_intf.pppoe_dslite_info.after_dial.dslite_hw_info.static_info;
				else	
					static_info=&intfInfo->wan_intf.static_info;
				memcpy(ip,(void*)_inet_ntoa(static_info->ip_addr),16);
				memcpy(gw_ip,(void*)_inet_ntoa(static_info->gateway_ipv4_addr),16);
				memcpy(mask,(void*)_inet_ntoa(static_info->ip_network_mask),16);

				//caculate broacast ip
				bc_ip = static_info->gateway_ipv4_addr & static_info->ip_network_mask;

				for(i=0;i<32;i++){
					if(static_info->ip_network_mask & (1<<i)){
						//wan domain (mask)
					}else{
						//wan broacast ip
						bc_ip |= (1<<i);
					}
				}
				memcpy(brcast_ip,(void*)_inet_ntoa(bc_ip),16);
				
				//set STATIC protocal-stack
				if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WIRED)
				{
					rtk_rg_callback_pipe_cmd("%s link set %s up",ipUtility,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME);
#ifdef CONFIG_RG_WMUX_SUPPORT
					rtk_rg_callback_pipe_cmd("echo %s %d > /proc/rg/wmux_add",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,(*intfIdx));
#else
					rtk_rg_callback_pipe_cmd("ethctl remsmux bridge %s %s",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev);

					if(static_info->napt_enable && intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
					{	
						//NAPT + VALN TAG
						rtk_rg_callback_pipe_cmd("ethctl addsmux %s %s %s napt vlan %d",wan_type,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev,vid); 
					}
					else if(static_info->napt_enable)
					{
						//NAPT
						rtk_rg_callback_pipe_cmd("ethctl addsmux %s %s %s napt",wan_type,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev);
					}
					else if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on){
						//VALN TAG
						rtk_rg_callback_pipe_cmd("ethctl addsmux %s %s %s nonapt vlan %d",wan_type,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev,vid); 
					}
					else{
						rtk_rg_callback_pipe_cmd("ethctl addsmux %s %s %s",wan_type,CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev);
					}
#endif
				}
				rtk_rg_callback_pipe_cmd("%s link set %s address %02x:%02x:%02x:%02x:%02x:%02x",ipUtility,dev,
						   intfInfo->wan_intf.wan_intf_conf.gmac.octet[0],
						   intfInfo->wan_intf.wan_intf_conf.gmac.octet[1],
						   intfInfo->wan_intf.wan_intf_conf.gmac.octet[2],
						   intfInfo->wan_intf.wan_intf_conf.gmac.octet[3],
						   intfInfo->wan_intf.wan_intf_conf.gmac.octet[4],
						   intfInfo->wan_intf.wan_intf_conf.gmac.octet[5]);

				if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WIRED)
					rtk_rg_callback_pipe_cmd("%s link set %s up",ipUtility,dev);

				//setup MTU
				rtk_rg_callback_pipe_cmd("%s link set %s mtu %d",ipUtility,dev,static_info->mtu);
			}

			/***IPv4 Setting***/
			if(static_info->ip_version==IPVER_V4ONLY || static_info->ip_version==IPVER_V4V6)
			{
				//enable forwarding state
				rtk_rg_callback_pipe_cmd("echo 1 > /proc/sys/net/ipv4/ip_forward");

				//20140616LUKE:before add address, forced delete once!(for change IP at same interface)
				rtk_rg_callback_pipe_cmd("%s -4 addr flush dev %s",ipUtility,dev);
				rtk_rg_callback_pipe_cmd("%s -4 route flush dev %s",ipUtility,dev);
				if(static_info->ip_version==IPVER_V4ONLY)
				{
					rtk_rg_callback_pipe_cmd("%s -6 addr flush dev %s",ipUtility,dev);
					rtk_rg_callback_pipe_cmd("%s -6 route flush dev %s",ipUtility,dev);
				}

				//STATIC or DHCP
				switch(intfInfo->wan_intf.wan_intf_conf.wan_type)
				{
					case RTK_RG_STATIC:
					case RTK_RG_DHCP:
					case RTK_RG_DSLITE:
					case RTK_RG_PPPoE_DSLITE:
						rtk_rg_callback_pipe_cmd("%s addr add %s/%s broadcast + dev %s",ipUtility,ip,mask,dev);
						if(static_info->ipv4_default_gateway_on)
						{
							rtk_rg_callback_pipe_cmd("%s route add default via %s dev %s mtu %d",ipUtility,gw_ip,dev,static_info->mtu);
#if defined(CONFIG_RTL_NEW_FLOW_BASE_HWNAT_DRIVER)
							rtk_rg_callback_pipe_cmd("%s neigh add %s dev %s lladdr %pM",ipUtility,gw_ip,dev,&static_info->gateway_mac_addr_for_ipv4.octet[0]);
#endif
						}
						
						if((intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_STATIC)&&(static_info->napt_enable && static_info->ip_addr!=0))//add NAPT in iptables, if routing no need this
						{
							//20140625LUKE:delete our IP once before add, in case IPv4 didn't change in re-set staticInfo
							rtk_rg_callback_pipe_cmd("iptables -t nat -D POSTROUTING -o %s -j SNAT --to-source %s",dev,ip);
							rtk_rg_callback_pipe_cmd("iptables -t nat -A POSTROUTING -o %s -j SNAT --to-source %s",dev,ip);
						}
						break;
					case RTK_RG_PPTP:
					case RTK_RG_L2TP:
						if(static_info->ipv4_default_gateway_on)
						{
							rtk_rg_callback_pipe_cmd("%s route add default via %s dev ppp%d mtu %d",ipUtility,gw_ip,(*intfIdx),static_info->mtu);
						}	
						break;
					default:
						break;
				}
				
				//bring message to DDNS
				//rtk_rg_callback_pipe_cmd("/bin/updateddctrl %s",dev);

				//20150225LUKE: for ARP request, we should check IP for incoming interface				
#if 0				
				rtk_rg_callback_pipe_cmd("brctl addbr br_%s",dev);
				rtk_rg_callback_pipe_cmd("brctl addif br_%s %s",dev,dev);
				rtk_rg_callback_pipe_cmd("brctl setfd br_%s 0",dev);	//set forward delay
				rtk_rg_callback_pipe_cmd("%s link set br_%s up",ipUtility,dev);
				rtk_rg_callback_pipe_cmd("ebtables -A INPUT -i %s -p arp --arp-ip-dst ! %s -j DROP",dev,ip);
#endif				
			}

			/***IPv6 Setting***/
			if(static_info->ip_version==IPVER_V6ONLY || static_info->ip_version==IPVER_V4V6)
			{
				//20160614LUKE: turn on IPv6 at /proc/sys/net
				rtk_rg_callback_pipe_cmd("echo 0 >/proc/sys/net/ipv6/conf/%s/disable_ipv6",dev);
				
				//20140630LUKE:for setup IPv6 in protocol stack, the MTU need to be bigger than 1280!!
				if(static_info->mtu<1280)
					WARNING("for IPv6, the smallest MTU is 1280, otherwise IPv6 address and route could not be added.");
				
				//20140616LUKE:before add address, forced delete once!(for change IP at same interface)
				rtk_rg_callback_pipe_cmd("%s -6 addr flush dev %s",ipUtility,dev);
				rtk_rg_callback_pipe_cmd("%s -6 route flush dev %s",ipUtility,dev);
				if(static_info->ip_version==IPVER_V6ONLY)
				{
					rtk_rg_callback_pipe_cmd("%s -4 addr flush dev %s",ipUtility,dev);
					rtk_rg_callback_pipe_cmd("%s -4 route flush dev %s",ipUtility,dev);
				}
				
				if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP)
				{
					//NAT set by _rtk_rg_dhcpRequestByHwCallBack
					rtk_rg_callback_pipe_cmd("%s addr add %s/%d dev %s",ipUtility,_rtk_rg_inet_n6toa(static_info->ipv6_addr.ipv6_addr),static_info->ipv6_mask_length,dev);
				}
				else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE)
				{
					//NAT set by _rtk_rg_pppoeBeforeDiagByHwCallBack
				}
				else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE)
				{
				}
				else
				{		
					//current support ipv6_static only!!!
					rtk_rg_callback_pipe_cmd("%s addr add %s/%d dev %s",ipUtility,_rtk_rg_inet_n6toa(static_info->ipv6_addr.ipv6_addr),static_info->ipv6_mask_length,dev);
					rtk_rg_callback_pipe_cmd("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
					if(static_info->ipv6_default_gateway_on)
					{
						rtk_rg_callback_pipe_cmd("%s route add default via %s dev %s mtu %d",ipUtility,_rtk_rg_inet_n6toa(static_info->gateway_ipv6_addr.ipv6_addr),dev,static_info->mtu);
#if defined(CONFIG_RTL_NEW_FLOW_BASE_HWNAT_DRIVER)
						rtk_rg_callback_pipe_cmd("%s -6 neigh add %s dev %s lladdr %pM",ipUtility,_rtk_rg_inet_n6toa(static_info->gateway_ipv6_addr.ipv6_addr),dev,&static_info->gateway_mac_addr_for_ipv6.octet[0]);
#endif
					}

					if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DSLITE)
						rtk_rg_callback_pipe_cmd("%s addr add %s/%d dev %s",ipUtility,_rtk_rg_inet_n6toa(intfInfo->wan_intf.dslite_info.rtk_dslite.ipB4.ipv6_addr),static_info->ipv6_mask_length,dev);
					else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE_DSLITE)
						rtk_rg_callback_pipe_cmd("%s addr add %s/%d dev %s",ipUtility,_rtk_rg_inet_n6toa(intfInfo->wan_intf.pppoe_dslite_info.after_dial.dslite_hw_info.rtk_dslite.ipB4.ipv6_addr),static_info->ipv6_mask_length,dev);
				}
			}
		}
#ifdef CONFIG_GPON_FEATURE
	//20150312LUKE: trigger OMCI to create related CF rules, parameters: netIfIdx, vid, pri, type, service, isBinding, bAdd
	if(rg_db.systemGlobal.initParam.wanPortGponMode)	
	{
		if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WIRED){
			if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
				if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
					rtk_rg_callback_pipe_cmd("echo %d %d %d 0 %d %d 1 > /proc/omci/wanInfo",*intfIdx,intfInfo->wan_intf.wan_intf_conf.egress_vlan_id,
						intfInfo->wan_intf.wan_intf_conf.vlan_based_pri_enable?intfInfo->wan_intf.wan_intf_conf.vlan_based_pri:-1,
						intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
						(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
				else
					rtk_rg_callback_pipe_cmd("echo %d -1 -1 0 %d %d 1 > /proc/omci/wanInfo",*intfIdx,
						intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
						(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
			}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE){
				if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
					rtk_rg_callback_pipe_cmd("echo %d %d %d 2 %d %d 1 > /proc/omci/wanInfo",*intfIdx,intfInfo->wan_intf.wan_intf_conf.egress_vlan_id,
						intfInfo->wan_intf.wan_intf_conf.vlan_based_pri_enable?intfInfo->wan_intf.wan_intf_conf.vlan_based_pri:-1,
						intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
						(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
				else
					rtk_rg_callback_pipe_cmd("echo %d -1 -1 2 %d %d 1 > /proc/omci/wanInfo",*intfIdx,
						intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
						(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
			}else if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
				rtk_rg_callback_pipe_cmd("echo %d %d %d 1 %d %d  1 > /proc/omci/wanInfo",*intfIdx,intfInfo->wan_intf.wan_intf_conf.egress_vlan_id,
					intfInfo->wan_intf.wan_intf_conf.vlan_based_pri_enable?intfInfo->wan_intf.wan_intf_conf.vlan_based_pri:-1,
					intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
					(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
			else
				rtk_rg_callback_pipe_cmd("echo %d -1 -1 1 %d %d 1 > /proc/omci/wanInfo",*intfIdx,
					intfInfo->wan_intf.wan_intf_conf.none_internet?0:1,
					(intfInfo->wan_intf.wan_intf_conf.vlan_binding_mask.portmask|intfInfo->wan_intf.wan_intf_conf.port_binding_mask.portmask)>0?1:0);
		}
	}
#endif
	}
	else
	{
		taggingCPU=(intfInfo->lan_intf.untag_mask.portmask&RTK_RG_ALL_MAC_MASTER_CPU_PORTMASK)?0:1;
	
		//add lan interface
		memcpy(ip,(void*)_inet_ntoa(intfInfo->lan_intf.ip_addr),16);
		memcpy(mask,(void*)_inet_ntoa(intfInfo->lan_intf.ip_network_mask),16);
		
#if defined(CONFIG_RTL_MULTI_LAN_DEV)
		for(i=0;i<RTK_RG_PORT_PON;i++)
		{
			if(RG_INVALID_PORT(i)) continue;
			if(intfInfo->lan_intf.port_mask.portmask&(0x1<<i))
				rtk_rg_callback_pipe_cmd("%s link set eth0.%d down",ipUtility,i+2);
		}
//#else
#endif
		rtk_rg_callback_pipe_cmd("%s link set %s down", ipUtility, CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
//#endif
		rtk_rg_callback_pipe_cmd("brctl addif br0 wlan0");
#ifdef CONFIG_DUALBAND_CONCURRENT
		rtk_rg_callback_pipe_cmd("brctl addif br0 wlan1");
		rtk_rg_callback_pipe_cmd("echo 3 > /proc/vwlan");	//echo 3 means down
#elif defined(CONFIG_RG_WLAN_HWNAT_ACCELERATION)
		rtk_rg_callback_pipe_cmd("brctl addif br0 wlan1");
#endif

		//1 FIXME:br0.vid should match br0's ether address, but it's not definitely required
		if(!taggingCPU)
			rtk_rg_callback_pipe_cmd("%s link set br0 address %02x:%02x:%02x:%02x:%02x:%02x",ipUtility,
				   intfInfo->lan_intf.gmac.octet[0],intfInfo->lan_intf.gmac.octet[1],
				   intfInfo->lan_intf.gmac.octet[2],intfInfo->lan_intf.gmac.octet[3],
				   intfInfo->lan_intf.gmac.octet[4],intfInfo->lan_intf.gmac.octet[5]);

#if defined(CONFIG_RTL_MULTI_LAN_DEV)
		if(intfInfo->lan_intf.port_mask.portmask&RTK_RG_ALL_MASTER_EXT_PORTMASK
#ifdef CONFIG_DUALBAND_CONCURRENT
			|| intfInfo->lan_intf.port_mask.portmask&RTK_RG_ALL_SLAVE_EXT_PORTMASK
#endif
			)	//sync eth0 mac with wlan mac when multiple LAN is on.
#endif
			rtk_rg_callback_pipe_cmd("%s link set %s address %02x:%02x:%02x:%02x:%02x:%02x", ipUtility, CONFIG_RG_CALLBACK_LAN_DEVICE_NAME,
					   intfInfo->lan_intf.gmac.octet[0],
					   intfInfo->lan_intf.gmac.octet[1],
					   intfInfo->lan_intf.gmac.octet[2],
					   intfInfo->lan_intf.gmac.octet[3],
					   intfInfo->lan_intf.gmac.octet[4],
					   intfInfo->lan_intf.gmac.octet[5]);
		rtk_rg_callback_pipe_cmd("%s link set %s up", ipUtility, CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
		
		for(i=0;i<RTK_RG_PORT_PON;i++)
		{
			if(RG_INVALID_PORT(i)) continue;
			if(intfInfo->lan_intf.port_mask.portmask&(0x1<<i))
			{
				rtk_rg_callback_pipe_cmd("%s link set eth0.%d address %02x:%02x:%02x:%02x:%02x:%02x",ipUtility,i+2,
				   intfInfo->lan_intf.gmac.octet[0],
				   intfInfo->lan_intf.gmac.octet[1],
				   intfInfo->lan_intf.gmac.octet[2],
				   intfInfo->lan_intf.gmac.octet[3],
				   intfInfo->lan_intf.gmac.octet[4],
				   intfInfo->lan_intf.gmac.octet[5]);
				rtk_rg_callback_pipe_cmd("%s link set eth0.%d up",ipUtility,i+2);
			}
		}

#if defined(CONFIG_RTL_NEW_FLOW_BASE_HWNAT_DRIVER)
		// to remove the bridge data path to unknown port. (tx devportmapping eth0->0x0)
		rtk_rg_callback_pipe_cmd("brctl delif br0 %s", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
#endif

		rtk_rg_callback_pipe_cmd("%s link set br0 up",ipUtility);

		//add virtual ether device for VLAN
		if(taggingCPU)
			rtk_rg_callback_pipe_cmd("vconfig add br0 %d",intfInfo->lan_intf.intf_vlan_id);
		
		/***IPv4 Setting***/
		if(intfInfo->lan_intf.ip_version==IPVER_V4ONLY || intfInfo->lan_intf.ip_version==IPVER_V4V6)
		{	
			if(taggingCPU)
			{
				//20140702LUKE:before add address, forced delete once!(for change IP at same interface)
				if(intfInfo->lan_intf.replace_subnet)
				{
					//when replace, the "more subnet" added before will be flushed also
					rtk_rg_callback_pipe_cmd("%s -4 addr flush dev br0.%d",ipUtility,intfInfo->lan_intf.intf_vlan_id);
					if(intfInfo->lan_intf.ip_version==IPVER_V4ONLY)
					{
						rtk_rg_callback_pipe_cmd("%s -6 addr flush dev br0.%d",ipUtility,intfInfo->lan_intf.intf_vlan_id);
						rtk_rg_callback_pipe_cmd("%s -6 route flush dev br0.%d",ipUtility,intfInfo->lan_intf.intf_vlan_id);
					}
				}
			
				//CPU tagged, set br0.vid
				//remove br0's ipv4 address, may be set by boa or other app.
				rtk_rg_callback_pipe_cmd("%s link set br0.%d mtu %d",ipUtility,intfInfo->lan_intf.intf_vlan_id,intfInfo->lan_intf.mtu);
				if(intfInfo->lan_intf.ip_network_mask != 0)
				{
					rtk_rg_callback_pipe_cmd("%s addr add %s/%s broadcast + dev br0.%d",ipUtility,ip,mask,intfInfo->lan_intf.intf_vlan_id);
				}
				//Setup dhcpd for the adding interface
				_rg_setup_dhcpd(intfInfo->lan_intf.ip_addr,intfInfo->lan_intf.ip_network_mask,intfInfo->lan_intf.intf_vlan_id);
			}
			else
			{
				//20140702LUKE:before add address, forced delete once!(for change IP at same interface)
				if(intfInfo->lan_intf.replace_subnet)
				{
					//when replace, the "more subnet" added before will be flushed also
					rtk_rg_callback_pipe_cmd("%s -4 addr flush dev br0",ipUtility);
					if(intfInfo->lan_intf.ip_version==IPVER_V4ONLY)
					{
						rtk_rg_callback_pipe_cmd("%s -6 addr flush dev br0",ipUtility);
						rtk_rg_callback_pipe_cmd("%s -6 route flush dev br0",ipUtility);
					}
				}
				
				//CPU untag, set br0 directly
				rtk_rg_callback_pipe_cmd("%s link set br0 mtu %d",ipUtility,intfInfo->lan_intf.mtu);
				if(intfInfo->lan_intf.ip_network_mask != 0)
				{
					rtk_rg_callback_pipe_cmd("%s addr add %s/%s broadcast + dev br0",ipUtility,ip,mask);
				}

				//Setup dhcpd for the adding interface
				_rg_setup_dhcpd(intfInfo->lan_intf.ip_addr,intfInfo->lan_intf.ip_network_mask,-1);
			}
		}

		/***IPv6 Setting***/
		if(intfInfo->lan_intf.ip_version==IPVER_V6ONLY || intfInfo->lan_intf.ip_version==IPVER_V4V6)
		{	
			if(intfInfo->lan_intf.ipv6_network_mask_length != 0 && 
				!(*(unsigned int *)intfInfo->lan_intf.ipv6_addr.ipv6_addr == 0 &&
				*(unsigned int *)(intfInfo->lan_intf.ipv6_addr.ipv6_addr+4) == 0 &&
				*(unsigned int *)(intfInfo->lan_intf.ipv6_addr.ipv6_addr+8) == 0 &&
				*(unsigned int *)(intfInfo->lan_intf.ipv6_addr.ipv6_addr+12) == 0))
			{
				if(taggingCPU)
				{
					//20160614LUKE: turn on IPv6 at /proc/sys/net
					rtk_rg_callback_pipe_cmd("echo 0 >/proc/sys/net/ipv6/conf/br0.%d/disable_ipv6",intfInfo->lan_intf.intf_vlan_id);
					
					//20140702LUKE:before add address, forced delete once!(for change IP at same interface)
					if(intfInfo->lan_intf.replace_subnet)
					{
						//FIXME:since we don't support IPv6 more subnet right now, here we just flush all(one) IPv6 address.
						rtk_rg_callback_pipe_cmd("%s -6 addr flush dev br0.%d",ipUtility,intfInfo->lan_intf.intf_vlan_id);
						rtk_rg_callback_pipe_cmd("%s -6 route flush dev br0.%d",ipUtility,intfInfo->lan_intf.intf_vlan_id);
						if(intfInfo->lan_intf.ip_version==IPVER_V6ONLY)
						{
							rtk_rg_callback_pipe_cmd("%s -4 addr flush dev br0.%d",ipUtility,intfInfo->lan_intf.intf_vlan_id);
							rtk_rg_callback_pipe_cmd("%s -4 route flush dev br0.%d",ipUtility,intfInfo->lan_intf.intf_vlan_id);
						}
					}
				
					//CPU tagged, set br0.vid
					rtk_rg_callback_pipe_cmd("%s link set br0.%d mtu %d",ipUtility,intfInfo->lan_intf.intf_vlan_id,intfInfo->lan_intf.mtu);
					//remove br0's ipv6 address, may be set by boa or other app.
					rtk_rg_callback_pipe_cmd("%s addr add %s/%d dev br0.%d",ipUtility,_rtk_rg_inet_n6toa(intfInfo->lan_intf.ipv6_addr.ipv6_addr),intfInfo->lan_intf.ipv6_network_mask_length,intfInfo->lan_intf.intf_vlan_id);

					//Setup radvd for the adding interface
					_rg_setup_radvd(intfInfo->lan_intf.ipv6_addr.ipv6_addr,intfInfo->lan_intf.ipv6_network_mask_length,intfInfo->lan_intf.intf_vlan_id);
				}
				else
				{
					//20160614LUKE: turn on IPv6 at /proc/sys/net
					rtk_rg_callback_pipe_cmd("echo 0 >/proc/sys/net/ipv6/conf/br0/disable_ipv6");
					
					//20140702LUKE:before add address, forced delete once!(for change IP at same interface)
					if(intfInfo->lan_intf.replace_subnet)
					{
						//FIXME:since we don't support IPv6 more subnet right now, here we just flush all(one) IPv6 address.
						rtk_rg_callback_pipe_cmd("%s -6 addr flush dev br0",ipUtility);
						rtk_rg_callback_pipe_cmd("%s -6 route flush dev br0",ipUtility);
						if(intfInfo->lan_intf.ip_version==IPVER_V6ONLY)
						{
							rtk_rg_callback_pipe_cmd("%s -4 addr flush dev br0",ipUtility);
							rtk_rg_callback_pipe_cmd("%s -4 route flush dev br0",ipUtility);
						}
					}
					
					//CPU untag, set br0 directly
					rtk_rg_callback_pipe_cmd("%s link set br0 mtu %d",ipUtility,intfInfo->lan_intf.mtu);
					rtk_rg_callback_pipe_cmd("%s addr add %s/%d dev br0",ipUtility,_rtk_rg_inet_n6toa(intfInfo->lan_intf.ipv6_addr.ipv6_addr),intfInfo->lan_intf.ipv6_network_mask_length);

					//Setup radvd for the adding interface
					_rg_setup_radvd(intfInfo->lan_intf.ipv6_addr.ipv6_addr,intfInfo->lan_intf.ipv6_network_mask_length,-1);
				}	
			}
		}

		if(taggingCPU)
		{
			rtk_rg_callback_pipe_cmd("%s link set br0.%d address %02x:%02x:%02x:%02x:%02x:%02x",ipUtility,intfInfo->lan_intf.intf_vlan_id,
				   intfInfo->lan_intf.gmac.octet[0],intfInfo->lan_intf.gmac.octet[1],
				   intfInfo->lan_intf.gmac.octet[2],intfInfo->lan_intf.gmac.octet[3],
				   intfInfo->lan_intf.gmac.octet[4],intfInfo->lan_intf.gmac.octet[5]);
			rtk_rg_callback_pipe_cmd("%s link set br0.%d up",ipUtility,intfInfo->lan_intf.intf_vlan_id);
		}

		//20140702LUKE:when change LAN's ip setting, the vlan-binding should be remove and readd for new IP address!!
		//re-check all Vlan-binding rule for re-create vconfig 
		for(i=0;i<MAX_BIND_SW_TABLE_SIZE;i++)
		{
			rtk_rg_table_bind_t localBind;
			RGDB_LOCK
			memcpy(&localBind,&rg_db.bind[i],sizeof(rtk_rg_table_bind_t));
			RGDB_UNLOCK
			if(localBind.valid && localBind.rtk_bind.vidLan!=0)
			{	
				if(localBind.rtk_bind.portMask.bits[0]>0)
				{
					if(localBind.rtk_bind.portMask.bits[0]&intfInfo->lan_intf.port_mask.portmask)
					{
						//delete it first, readd later
#if defined(CONFIG_RTL_MULTI_LAN_DEV)
						if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT0))
						{
							rtk_rg_callback_pipe_cmd("vconfig rem eth0.%d.%d",2+RTK_RG_PORT0,localBind.rtk_bind.vidLan);
							rtk_rg_callback_pipe_cmd("ebtables -t broute -D BROUTING -i eth0.%d -p 802_1Q -j DROP",2+RTK_RG_PORT0);
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT0,localBind.rtk_bind.vidLan);
						}
						else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT1))
						{
							rtk_rg_callback_pipe_cmd("vconfig rem eth0.%d.%d",2+RTK_RG_PORT1,localBind.rtk_bind.vidLan);
							rtk_rg_callback_pipe_cmd("ebtables -t broute -D BROUTING -i eth0.%d -p 802_1Q -j DROP",2+RTK_RG_PORT1);
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT1,localBind.rtk_bind.vidLan);
						}
#if !defined(CONFIG_RG_RTL9602C_SERIES)

						else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT2))
						{
							rtk_rg_callback_pipe_cmd("vconfig rem eth0.%d.%d",2+RTK_RG_PORT2,localBind.rtk_bind.vidLan);
							rtk_rg_callback_pipe_cmd("ebtables -t broute -D BROUTING -i eth0.%d -p 802_1Q -j DROP",2+RTK_RG_PORT2);
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT2,localBind.rtk_bind.vidLan);
						}
						else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT3))
						{
							rtk_rg_callback_pipe_cmd("vconfig rem eth0.%d.%d",2+RTK_RG_PORT3,localBind.rtk_bind.vidLan);
							rtk_rg_callback_pipe_cmd("ebtables -t broute -D BROUTING -i eth0.%d -p 802_1Q -j DROP",2+RTK_RG_PORT3);
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT3,localBind.rtk_bind.vidLan);
						}
#if !defined(CONFIG_RG_G3_SERIES)						
						else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT_RGMII))
						{
							rtk_rg_callback_pipe_cmd("vconfig rem eth0.%d.%d",2+RTK_RG_PORT_RGMII,localBind.rtk_bind.vidLan);
							rtk_rg_callback_pipe_cmd("ebtables -t broute -D BROUTING -i eth0.%d -p 802_1Q -j DROP",2+RTK_RG_PORT_RGMII);
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT_RGMII,localBind.rtk_bind.vidLan);
						}
#endif
#endif
#if defined(CONFIG_RG_RTL9607C_SERIES)
						else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT4))
						{
							rtk_rg_callback_pipe_cmd("vconfig rem eth0.%d.%d",2+RTK_RG_PORT4,localBind.rtk_bind.vidLan);
							rtk_rg_callback_pipe_cmd("ebtables -t broute -D BROUTING -i eth0.%d -p 802_1Q -j DROP",2+RTK_RG_PORT4);
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT4,localBind.rtk_bind.vidLan);
						}
#endif
						else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT_PON))
						{
							rtk_rg_callback_pipe_cmd("vconfig rem eth0.%d.%d",2+RTK_RG_PORT_PON,localBind.rtk_bind.vidLan);
							rtk_rg_callback_pipe_cmd("ebtables -t broute -D BROUTING -i eth0.%d -p 802_1Q -j DROP",2+RTK_RG_PORT_PON);
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT_PON,localBind.rtk_bind.vidLan);
						}

						else
							CBACK("vlan-binding[%d] portMask wrong...%x",i,localBind.rtk_bind.portMask.bits[0]);
#else
						rtk_rg_callback_pipe_cmd("vconfig rem eth0.%d",localBind.rtk_bind.vidLan);
						rtk_rg_callback_pipe_cmd("ebtables -t broute -D BROUTING -i %s -p 802_1Q -j DROP", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);

						if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT0))
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT0,localBind.rtk_bind.vidLan);
						else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT1))
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT1,localBind.rtk_bind.vidLan);
#if !defined(CONFIG_RG_RTL9602C_SERIES)					
						else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT2))
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT2,localBind.rtk_bind.vidLan);
						else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT3))
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT3,localBind.rtk_bind.vidLan);
#if !defined(CONFIG_RG_G3_SERIES)						
						else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT_RGMII))
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT_RGMII,localBind.rtk_bind.vidLan);
#endif						
#endif	
#if defined(CONFIG_RG_RTL9607C_SERIES)
						else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT4))
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT4,localBind.rtk_bind.vidLan);
#endif
						else if(localBind.rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT_PON))
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_PORT_PON,localBind.rtk_bind.vidLan);						
						else
							CBACK("vlan-binding[%d] portMask wrong...%x",i,localBind.rtk_bind.portMask.bits[0]);
#endif						
					}
				}
				else
				{
					if(localBind.rtk_bind.extPortMask.bits[0]&(intfInfo->lan_intf.port_mask.portmask>>RTK_RG_EXT_PORT0))
					{
						//delete it first, readd later
						rtk_rg_callback_pipe_cmd("vconfig rem eth0.%d",localBind.rtk_bind.vidLan);
						rtk_rg_callback_pipe_cmd("ebtables -t broute -D BROUTING -i %s -p 802_1Q -j DROP", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
						
						if(localBind.rtk_bind.extPortMask.bits[0]&(0x1<<(RTK_RG_EXT_PORT0-RTK_RG_EXT_PORT0)))
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_EXT_PORT0,localBind.rtk_bind.vidLan);
						else if(localBind.rtk_bind.extPortMask.bits[0]&(0x1<<(RTK_RG_EXT_PORT1-RTK_RG_EXT_PORT0)))
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_EXT_PORT1,localBind.rtk_bind.vidLan);
						else if(localBind.rtk_bind.extPortMask.bits[0]&(0x1<<(RTK_RG_EXT_PORT2-RTK_RG_EXT_PORT0)))
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_EXT_PORT2,localBind.rtk_bind.vidLan);
						else if(localBind.rtk_bind.extPortMask.bits[0]&(0x1<<(RTK_RG_EXT_PORT3-RTK_RG_EXT_PORT0)))
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_EXT_PORT3,localBind.rtk_bind.vidLan);
#if !defined(CONFIG_RG_G3_SERIES)						
						else if(localBind.rtk_bind.extPortMask.bits[0]&(0x1<<(RTK_RG_EXT_PORT4-RTK_RG_EXT_PORT0)))
							_rtk_rg_checkVlanBindingAndAddVconfig(RTK_RG_EXT_PORT4,localBind.rtk_bind.vidLan);
#endif
						else
							CBACK("vlan-binding[%d] ext-portMask wrong...%x",i,localBind.rtk_bind.extPortMask.bits[0]);
					}
				}
			}
		}

#ifdef CONFIG_DUALBAND_CONCURRENT
		rtk_rg_callback_pipe_cmd("echo 2 > /proc/vwlan");	//echo 2 means up
#endif
		
#if defined(CONFIG_RG_G3_SERIES)

#if defined(CONFIG_RTL_MULTI_LAN_DEV)
	for(i=0;i<RTK_RG_MAC_PORT_PON;i++)
	{
		if(RG_INVALID_MAC_PORT(i)) continue;
		//reset all port-mapping to multi-LAN dev
		rtk_rg_callback_pipe_cmd("echo %d eth0.%d > /proc/driver/cortina/ni/dev_port_mapping",i,i+2);
	}
#endif

#elif defined(CONFIG_APOLLO_MP)
		//do cmd
		for(i=0;i<RTK_RG_MAC_PORT_MAX;i++){
			if(RG_INVALID_MAC_PORT(i)) continue;
			if(intfInfo->lan_intf.port_mask.portmask & (1<<i)){
				rtk_rg_callback_pipe_cmd("echo %d %s > /proc/rtl8686gmac/dev_port_mapping", i, CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
			}
		}
#if defined(CONFIG_RTL_MULTI_LAN_DEV)
		for(i=0;i<RTK_RG_MAC_PORT_PON;i++)
		{	
			if(RG_INVALID_MAC_PORT(i)) continue;
			if(intfInfo->lan_intf.port_mask.portmask & (1<<i))
				rtk_rg_callback_pipe_cmd("echo %d eth0.%d > /proc/rtl8686gmac/dev_port_mapping",i,i+2);
		}
#endif
#elif defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) //end defined(CONFIG_APOLLO_MP) elif defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) 

	FIXME("FIXME XDSL callback\n");

#endif //end defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) 


		//patch for iptables
		rtk_rg_callback_pipe_cmd("iptables -A INPUT -s %s -d %s -j ACCEPT",ip,ip);
#if 0
		//restart udhcpd: this will make signal hang
		do_sys_cmd("/bin/udhcpd -B /var/udhcpd/udhcpd.conf");
#endif
	}

	return SUCCESS;
}
void wq_do_interfaceAddByHwCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	__interfaceAddByHwCallBack(&p_callback_work->interfaceCallBack.intfInfo,&p_callback_work->interfaceCallBack.intfIdx);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}
int _rtk_rg_interfaceAddByHwCallBack(rtk_rg_intfInfo_t* intfInfo, int* intfIdx)
{
	rtk_rg_wq_union_t *interfaceAddWq=wq_callbackAssign();
	if(interfaceAddWq){
		INIT_WORK(&(interfaceAddWq->work), wq_do_interfaceAddByHwCallBack);
		memcpy(&(interfaceAddWq->interfaceCallBack.intfInfo),intfInfo,sizeof(rtk_rg_intfInfo_t)); 
		interfaceAddWq->interfaceCallBack.intfIdx = *intfIdx;
		queue_work(rg_kernel.rg_callbackWQ,&(interfaceAddWq->work));
	}
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.interfaceDelByHwCallBack;
int __interfaceDelByHwCallBack(rtk_rg_intfInfo_t* intfInfo, int* intfIdx)
{
	int ret;
	
	if(intfInfo->is_wan==0)
	{
		//clear dhcpd settings
		_rg_delete_dhcpd((intfInfo->lan_intf.untag_mask.portmask&RTK_RG_ALL_MAC_MASTER_CPU_PORTMASK)?-1:intfInfo->lan_intf.intf_vlan_id,0);

		//clear radvd settings
		_rg_delete_radvd((intfInfo->lan_intf.untag_mask.portmask&RTK_RG_ALL_MAC_MASTER_CPU_PORTMASK)?-1:intfInfo->lan_intf.intf_vlan_id,0);

		if(intfInfo->lan_intf.untag_mask.portmask&RTK_RG_ALL_MAC_MASTER_CPU_PORTMASK)
		{
			rtk_rg_callback_pipe_cmd("%s addr flush dev br0",ipUtility);
			rtk_rg_callback_pipe_cmd("%s route flush dev br0",ipUtility);
		}
		else
		{
			rtk_rg_callback_pipe_cmd("%s addr flush dev br0.%d",ipUtility,intfInfo->lan_intf.intf_vlan_id);
			rtk_rg_callback_pipe_cmd("%s route flush dev br0.%d",ipUtility,intfInfo->lan_intf.intf_vlan_id);
			//removing virtual ethernet device for VLAN
			rtk_rg_callback_pipe_cmd("vconfig rem br0.%d",intfInfo->lan_intf.intf_vlan_id);
		}
		RGDB_LOCK
		if(rg_db.systemGlobal.lanIntfTotalNum==0)
		{RGDB_UNLOCK
			rtk_rg_callback_pipe_cmd("`for FILE in $(ls /sys/class/net/ | grep '%s'); do %s link set $FILE down > /dev/null 2>&1; done`", CONFIG_RG_CALLBACK_LAN_DEVICE_NAME, ipUtility);
			rtk_rg_callback_pipe_cmd("%s link set br0 down",ipUtility);
		RGDB_LOCK
		}
		RGDB_UNLOCK
	}
	else
	{
		char dev[IFNAMSIZ]; //device name, ex:nas0_0
		//sprintf(dev,"nas0_%d",(*intfIdx-1));
		if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WLAN0_VXD)
			sprintf(dev,"wlan0-vxd");
		else if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WLAN1_VXD)
			sprintf(dev,"wlan1-vxd");
		else
			sprintf(dev,"%s_%d",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,(*intfIdx));
		if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP)
		{
			_rg_delete_dhcpc(dev, 0);
		}
		else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE)
		{
			int localVal;
			RGDB_LOCK
			localVal=rg_db.systemGlobal.not_disconnect_ppp;
			RGDB_UNLOCK
			if(localVal==0){
				rtk_rg_callback_pipe_cmd("spppctl del %d",*intfIdx);
			}else{
				//don't call spppctl to disconnet interface pppX, because this case is "Server disconnet problem".
				//so, do nothing!
				RGDB_LOCK
				rg_db.systemGlobal.not_disconnect_ppp=0;
				RGDB_UNLOCK
			}
		}
		else if((intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPTP)||
			(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_L2TP))
		{
			rtk_rg_callback_pipe_cmd("spppctl del %d",*intfIdx);
		}
		else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE)
		{
			if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
			//removing virtual ethernet device for VLAN
			rtk_rg_callback_pipe_cmd("vconfig rem br0.%d",intfInfo->wan_intf.wan_intf_conf.egress_vlan_id);
		}

		//20150226LUKE: for ARP request, we should check IP for incoming interface	
#if 0		
		if(intfInfo->wan_intf.wan_intf_conf.wan_type!=RTK_RG_BRIDGE)
		{
			rtk_rg_ipStaticInfo_t *static_info;
			unsigned char ip[16];
			if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP)
				static_info=&intfInfo->wan_intf.dhcp_client_info.hw_info;
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE)
				static_info=&intfInfo->wan_intf.pppoe_info.after_dial.hw_info;
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPTP)
				static_info=&intfInfo->wan_intf.pptp_info.after_dial.hw_info;
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_L2TP)
				static_info=&intfInfo->wan_intf.l2tp_info.after_dial.hw_info;
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DSLITE)
				static_info=&intfInfo->wan_intf.dslite_info.static_info;
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE_DSLITE)
				static_info=&intfInfo->wan_intf.pppoe_dslite_info.after_dial.dslite_hw_info.static_info;
			else	
				static_info=&intfInfo->wan_intf.static_info;
			memcpy(ip,(void*)_inet_ntoa(static_info->ip_addr),16);
			rtk_rg_callback_pipe_cmd("%s link set br_%s down",ipUtility,dev);
			rtk_rg_callback_pipe_cmd("brctl delbr br_%s",dev);
			rtk_rg_callback_pipe_cmd("ebtables -D INPUT -i %s -p arp --arp-ip-dst ! %s -j DROP",dev,ip);
		}
#endif		
		//wan-intf
		rtk_rg_callback_pipe_cmd("%s link set %s down",ipUtility,dev);
#ifdef CONFIG_RG_WMUX_SUPPORT
		rtk_rg_callback_pipe_cmd("echo %s > /proc/rg/wmux_del",dev);
#endif

		//reset to eth0 if no other WAN use the same wan port
		RGDB_LOCK
		for(ret=rg_db.systemGlobal.wanIntfTotalNum;ret>0;ret--)
		{
			if(rg_db.systemGlobal.wanIntfGroup[ret-1].p_wanIntfConf->wan_port_idx==intfInfo->wan_intf.wan_intf_conf.wan_port_idx)
				break;
		}
		RGDB_UNLOCK
#if defined(CONFIG_RG_G3_SERIES)
		if(ret==0)
			rtk_rg_callback_pipe_cmd("echo %d %s > /proc/driver/cortina/ni/dev_port_mapping", intfInfo->wan_intf.wan_intf_conf.wan_port_idx, CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
#elif defined(CONFIG_APOLLO_MP)
		if(ret==0)
			rtk_rg_callback_pipe_cmd("echo %d %s > /proc/rtl8686gmac/dev_port_mapping", intfInfo->wan_intf.wan_intf_conf.wan_port_idx, CONFIG_RG_CALLBACK_LAN_DEVICE_NAME);
#elif defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) //end defined(CONFIG_APOLLO_MP) elif defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) 
		
		FIXME("FIXME XDSL callback\n");
		
#endif //end defined(CONFIG_XDSL_NEW_HWNAT_DRIVER) 

#ifdef CONFIG_GPON_FEATURE
		//20150312LUKE: trigger OMCI to remove related CF rules, parameters: netIfIdx, vid, pri, type, service, isBinding, bAdd
		if(rg_db.systemGlobal.initParam.wanPortGponMode)
		{
			if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WIRED)
				rtk_rg_callback_pipe_cmd("echo %d -1 -1 -1 -1 -1 0 > /proc/omci/wanInfo",*intfIdx);
		}
#endif
	}

	return SUCCESS;
}
void wq_do_interfaceDelByHwCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	__interfaceDelByHwCallBack(&p_callback_work->interfaceCallBack.intfInfo,&p_callback_work->interfaceCallBack.intfIdx);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}
int _rtk_rg_interfaceDelByHwCallBack(rtk_rg_intfInfo_t* intfInfo, int* intfIdx)
{
	rtk_rg_wq_union_t *interfaceDelWq=wq_callbackAssign();
	if(interfaceDelWq){
		INIT_WORK(&(interfaceDelWq->work), wq_do_interfaceDelByHwCallBack);
		memcpy(&(interfaceDelWq->interfaceCallBack.intfInfo),intfInfo,sizeof(rtk_rg_intfInfo_t)); 
		interfaceDelWq->interfaceCallBack.intfIdx = *intfIdx;
		queue_work(rg_kernel.rg_callbackWQ,&(interfaceDelWq->work));
	}
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.neighborAddByHwCallBack;
int _rtk_rg_neighborAddByHwCallBack(rtk_rg_neighborInfo_t* neighborInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.neighborDelByHwCallBack;
int _rtk_rg_neighborDelByHwCallBack(rtk_rg_neighborInfo_t* neighborInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.v6RoutingAddByHwCallBack;
int _rtk_rg_v6RoutingAddByHwCallBack(rtk_rg_ipv6RoutingEntry_t* cb_routv6Et){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.v6RoutingDelByHwCallBack;
int _rtk_rg_v6RoutingDelByHwCallBack(rtk_rg_ipv6RoutingEntry_t* cb_routv6Et){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.pppoeBeforeDiagByHwCallBack;
int __pppoeBeforeDialByHwCallBack(rtk_rg_pppoeClientInfoBeforeDial_t* before_diag, int* intfIdx)
{
	char dev[IFNAMSIZ]; //device name, ex:nas0_0
	int dev_idx;
	rtk_rg_intfInfo_t *intfInfo;
	//memcpy(&intfInfo,&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo,sizeof(rtk_rg_intfInfo_t));
	RGDB_LOCK

	////check wan_intf info & set dev name
	if((*intfIdx>=MAX_NETIF_SW_TABLE_SIZE) || (*intfIdx<0)){
		CBACK("invalid wan_intf_idx\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else{
		//dev_idx = (*intfIdx-1);
		dev_idx = (*intfIdx);
		intfInfo=&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo;
		if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WLAN0_VXD)
			sprintf(dev,"wlan0-vxd");
		else if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WLAN1_VXD)
			sprintf(dev,"wlan1-vxd");
		else
			sprintf(dev,"%s_%d",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev_idx);
	}
	
	if(rg_db.systemGlobal.interfaceInfo[*intfIdx].valid==IF_INVALID_ENTRY){
		CBACK("Assigned wan_intf_idx is not valid.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else if(!intfInfo->is_wan){
		CBACK("Assigned wan_intf_idx is not wan.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else if(intfInfo->wan_intf.wan_intf_conf.wan_type!=RTK_RG_PPPoE){
		CBACK("Assigned wan_intf_idx is not PPPoE.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}
	RGDB_UNLOCK
	//configure wan interface in protocol stack
	_rg_configure_protocol_stack(*intfIdx,dev);
	
	rtk_rg_callback_pipe_cmd("spppctl add %d pppoe %s username %s password %s gw 1 mru 1492 nohwnat ippt 0 debug 0 ipt 2",dev_idx,dev,before_diag->username,before_diag->password);
	rtk_rg_callback_pipe_cmd("spppctl katimer 100");

	rtk_rg_callback_pipe_cmd("%s link set ppp%d txqueuelen 0",ipUtility,dev_idx);

	/*set proc for Radvd create global v6 ip*/
	rtk_rg_callback_pipe_cmd("echo 1 > /proc/sys/net/ipv6/conf/ppp%d/autoconf",dev_idx);
	rtk_rg_callback_pipe_cmd("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
	rtk_rg_callback_pipe_cmd("echo 0 > /proc/sys/net/ipv6/conf/ppp%d/forwarding",dev_idx);
	//enable,disable ipv6 ppp diag
	//rtk_rg_callback_pipe_cmd("/bin/echo 1 > proc/sys/net/ipv6/conf/ppp%d/disable_ipv6",dev_idx);

	//1 FIX ME: PPPoE disconnet issue
	/*Need to judge if PPPoE diag failed!!!*/
	//rg_kernel.ppp_diaged[dev_idx]=ENABLED;

	rtk_rg_callback_pipe_cmd("iptables -t nat -A POSTROUTING -o ppp%d -j MASQUERADE",dev_idx);
	
	return SUCCESS;
}
void wq_do_pppoeBeforeDiagByHwCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	__pppoeBeforeDialByHwCallBack(&p_callback_work->pppoeBeforeDialCallBack.pppoeBeforeInfo,&p_callback_work->pppoeBeforeDialCallBack.wanIdx);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}
int _rtk_rg_pppoeBeforeDialByHwCallBack(rtk_rg_pppoeClientInfoBeforeDial_t* before_diag, int* intfIdx)
{
	rtk_rg_wq_union_t *pppoeBeforeDialWq=wq_callbackAssign();
	if(pppoeBeforeDialWq){
		INIT_WORK(&(pppoeBeforeDialWq->work), wq_do_pppoeBeforeDiagByHwCallBack);
		memcpy(&(pppoeBeforeDialWq->pppoeBeforeDialCallBack.pppoeBeforeInfo),before_diag,sizeof(rtk_rg_pppoeClientInfoBeforeDial_t)); 
		pppoeBeforeDialWq->pppoeBeforeDialCallBack.wanIdx = *intfIdx;
		queue_work(rg_kernel.rg_callbackWQ,&(pppoeBeforeDialWq->work));
	}
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.dhcpRequestByHwCallBack;
int __dhcpRequestByHwCallBack(int* intfIdx)
{
	char dev[IFNAMSIZ]; //device name, ex:nas0_0
	int dev_idx;
	rtk_rg_intfInfo_t *intfInfo;
	CBACK("%s is called!!!",__func__);

	//memcpy(&intfInfo,&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo,sizeof(rtk_rg_intfInfo_t));
	RGDB_LOCK

	////check wan_intf info & set dev name
	if((*intfIdx>=MAX_NETIF_SW_TABLE_SIZE) || (*intfIdx<0)){
		CBACK("invalid wan_intf_idx\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else{
		//dev_idx = (*intfIdx-1);
		dev_idx = (*intfIdx);
		intfInfo=&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo;
		if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WLAN0_VXD)
			sprintf(dev,"wlan0-vxd");
		else if(intfInfo->wan_intf.wirelessWan==RG_WWAN_WLAN1_VXD)
			sprintf(dev,"wlan1-vxd");
		else
			sprintf(dev,"%s_%d",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev_idx);
	}
	
	if(rg_db.systemGlobal.interfaceInfo[*intfIdx].valid==IF_INVALID_ENTRY){
		CBACK("Assigned wan_intf_idx is not valid.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else if(!intfInfo->is_wan){
		CBACK("Assigned wan_intf_idx is not wan.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else if(intfInfo->wan_intf.wan_intf_conf.wan_type!=RTK_RG_DHCP){
		CBACK("Assigned wan_intf_idx is not DHCP.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}
	RGDB_UNLOCK
	//configure wan interface in protocol stack
	_rg_configure_protocol_stack(*intfIdx,dev);
	
	rtk_rg_callback_pipe_cmd("iptables -A INPUT -i %s -p UDP --dport 69 -d 255.255.255.255 -m state --state NEW -j ACCEPT",dev);

	//Delete the file to avoid accessing the last one.
	rtk_rg_callback_pipe_cmd("rm -f /var/udhcpc/udhcpc.info");
	_rg_delete_dhcpc(dev,0);
	//20170428LUKE: check for auto test mode
	if(rg_kernel.autoTestMode)
		CBACK("autoTestMode, won't setup dhcpc.");
	else
		rtk_rg_callback_pipe_cmd("udhcpc -i %s -W -s /etc/scripts/udhcpc_rgcb.sh &",dev);
	rtk_rg_callback_pipe_cmd("iptables -t nat -A POSTROUTING -o %s -j MASQUERADE",dev);

	rtlglue_printf("Please cat /var/udhcpc/udhcpc.info to read DHCP request information.\n");

	return SUCCESS;
}
void wq_do_dhcpRequestByHwCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	__dhcpRequestByHwCallBack(&p_callback_work->wanIdx);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}
int _rtk_rg_dhcpRequestByHwCallBack(int* intfIdx)
{
	rtk_rg_wq_union_t *dhcpRequestWq=wq_callbackAssign();
	if(dhcpRequestWq){
		INIT_WORK(&(dhcpRequestWq->work), wq_do_dhcpRequestByHwCallBack);
		dhcpRequestWq->wanIdx = *intfIdx;
		queue_work(rg_kernel.rg_callbackWQ,&(dhcpRequestWq->work));
	}
	return SUCCESS;
}

int _rtk_rg_pppoeBeforeDiagByHwCallBack(rtk_rg_pppoeClientInfoBeforeDial_t* before_diag, int* intfIdx)
{
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.pptpBeforeDialByHwCallBack;
int __pptpBeforeDialByHwCallBack(rtk_rg_pptpClientInfoBeforeDial_t* before_dial, int* intfIdx)
{
	char dev[IFNAMSIZ]; //device name, ex:nas0_0
	int dev_idx;
	rtk_rg_intfInfo_t *intfInfo;
	RGDB_LOCK

	////check wan_intf info & set dev name
	if((*intfIdx>=MAX_NETIF_SW_TABLE_SIZE) || (*intfIdx<0)){
		CBACK("invalid wan_intf_idx\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else{
		dev_idx = (*intfIdx);	
		sprintf(dev,"%s_%d",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev_idx);
	}
	intfInfo=&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo;
	if(rg_db.systemGlobal.interfaceInfo[*intfIdx].valid==IF_INVALID_ENTRY){
		CBACK("Assigned wan_intf_idx is not valid.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else if(!intfInfo->is_wan){
		CBACK("Assigned wan_intf_idx is not wan.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else if(intfInfo->wan_intf.wan_intf_conf.wan_type!=RTK_RG_PPTP){
		CBACK("Assigned wan_intf_idx is not PPTP.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}
	RGDB_UNLOCK
	rtk_rg_callback_pipe_cmd("spppctl add %d pptp auth auto username %s password %s server %s gw 0 enctype none",dev_idx,before_dial->username,before_dial->password,_inet_ntoa(before_dial->pptp_ipv4_addr));

	return SUCCESS;
}
void wq_do_pptpBeforeDialByHwCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	__pptpBeforeDialByHwCallBack(&p_callback_work->pptpBeforeDialCallBack.pptpBeforeInfo,&p_callback_work->pptpBeforeDialCallBack.wanIdx);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}
int _rtk_rg_pptpBeforeDialByHwCallBack(rtk_rg_pptpClientInfoBeforeDial_t* before_dial, int* intfIdx)
{
	rtk_rg_wq_union_t *pptpBeforeDialWq=wq_callbackAssign();
	if(pptpBeforeDialWq){
		INIT_WORK(&(pptpBeforeDialWq->work), wq_do_pptpBeforeDialByHwCallBack);
		memcpy(&(pptpBeforeDialWq->pptpBeforeDialCallBack.pptpBeforeInfo),before_dial,sizeof(rtk_rg_pptpClientInfoBeforeDial_t));
		pptpBeforeDialWq->pptpBeforeDialCallBack.wanIdx = *intfIdx;
		queue_work(rg_kernel.rg_callbackWQ,&(pptpBeforeDialWq->work));
	}
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.l2tpBeforeDialByHwCallBack;
int __l2tpBeforeDialByHwCallBack(rtk_rg_l2tpClientInfoBeforeDial_t* before_dial, int* intfIdx)
{
	char dev[IFNAMSIZ]; //device name, ex:nas0_0
	int dev_idx;
	rtk_rg_intfInfo_t *intfInfo;
	RGDB_LOCK

	////check wan_intf info & set dev name
	if((*intfIdx>=MAX_NETIF_SW_TABLE_SIZE) || (*intfIdx<0)){
		CBACK("invalid wan_intf_idx\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else{
		dev_idx = (*intfIdx);	
		sprintf(dev,"%s_%d",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev_idx);
	}
	intfInfo=&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo;
	if(rg_db.systemGlobal.interfaceInfo[*intfIdx].valid==IF_INVALID_ENTRY){
		CBACK("Assigned wan_intf_idx is not valid.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else if(!intfInfo->is_wan){
		CBACK("Assigned wan_intf_idx is not wan.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else if(intfInfo->wan_intf.wan_intf_conf.wan_type!=RTK_RG_L2TP){
		CBACK("Assigned wan_intf_idx is not L2TP.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}
	RGDB_UNLOCK
	//FIXME: should we configure the MRU value here?
	rtk_rg_callback_pipe_cmd("spppctl add %d l2tp username %s password %s gw 0 mru 1458 auth auto enctype none server %s tunnel_auth none",dev_idx,before_dial->username,before_dial->password,_inet_ntoa(before_dial->l2tp_ipv4_addr));

	return SUCCESS;
}
void wq_do_l2tpBeforeDialByHwCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	__l2tpBeforeDialByHwCallBack(&p_callback_work->l2tpBeforeDialCallBack.l2tpBeforeInfo,&p_callback_work->l2tpBeforeDialCallBack.wanIdx);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}
int _rtk_rg_l2tpBeforeDialByHwCallBack(rtk_rg_l2tpClientInfoBeforeDial_t* before_dial, int* intfIdx)
{
	rtk_rg_wq_union_t *l2tpBeforeDialWq=wq_callbackAssign();
	if(l2tpBeforeDialWq){
		INIT_WORK(&(l2tpBeforeDialWq->work), wq_do_l2tpBeforeDialByHwCallBack);
		memcpy(&(l2tpBeforeDialWq->l2tpBeforeDialCallBack.l2tpBeforeInfo),before_dial,sizeof(rtk_rg_l2tpClientInfoBeforeDial_t));
		l2tpBeforeDialWq->l2tpBeforeDialCallBack.wanIdx = *intfIdx;
		queue_work(rg_kernel.rg_callbackWQ,&(l2tpBeforeDialWq->work));
	}
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.pppoeDsliteBeforeDialByHwCallBack
int __pppoeDsliteBeforeDialByHwCallBack(rtk_rg_pppoeClientInfoBeforeDial_t *before_dial, int *intfIdx)
{
	char dev[IFNAMSIZ]; //device name, ex:nas0_0
	int dev_idx;
	rtk_rg_intfInfo_t *intfInfo;
	//memcpy(&intfInfo,&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo,sizeof(rtk_rg_intfInfo_t));
	RGDB_LOCK

	////check wan_intf info & set dev name
	if((*intfIdx>=MAX_NETIF_SW_TABLE_SIZE) || (*intfIdx<0)){
		CBACK("invalid wan_intf_idx\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else{
		//dev_idx = (*intfIdx-1);
		dev_idx = (*intfIdx);	
		sprintf(dev,"%s_%d",CONFIG_RG_CALLBACK_MULTI_WAN_DEVICE_NAME,dev_idx);
	}
	intfInfo=&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo;
	if(rg_db.systemGlobal.interfaceInfo[*intfIdx].valid==IF_INVALID_ENTRY){
		CBACK("Assigned wan_intf_idx is not valid.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else if(!intfInfo->is_wan){
		CBACK("Assigned wan_intf_idx is not wan.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}else if(intfInfo->wan_intf.wan_intf_conf.wan_type!=RTK_RG_PPPoE_DSLITE){
		CBACK("Assigned wan_intf_idx is not PPPoE_Dslite.\n");
		RGDB_UNLOCK
		return RT_ERR_RG_INVALID_PARAM;
	}
	RGDB_UNLOCK
	//configure wan interface in protocol stack
	_rg_configure_protocol_stack(*intfIdx,dev);
	
	rtk_rg_callback_pipe_cmd("spppctl add %d pppoe %s username %s password %s gw 1 mru 1492 nohwnat ippt 0 debug 0 ipt 2",dev_idx,dev,before_dial->username,before_dial->password);
	rtk_rg_callback_pipe_cmd("spppctl katimer 100");

	rtk_rg_callback_pipe_cmd("%s link set ppp%d txqueuelen 0",ipUtility,dev_idx);

	/*set proc for Radvd create global v6 ip*/
	rtk_rg_callback_pipe_cmd("echo 1 > /proc/sys/net/ipv6/conf/ppp%d/autoconf",dev_idx);
	rtk_rg_callback_pipe_cmd("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
	rtk_rg_callback_pipe_cmd("echo 0 > /proc/sys/net/ipv6/conf/ppp%d/forwarding",dev_idx);
	//enable,disable ipv6 ppp diag
	//rtk_rg_callback_pipe_cmd("/bin/echo 1 > proc/sys/net/ipv6/conf/ppp%d/disable_ipv6",dev_idx);

	//1 FIX ME: PPPoE disconnet issue
	/*Need to judge if PPPoE diag failed!!!*/
	//rg_kernel.ppp_diaged[dev_idx]=ENABLED;

	rtk_rg_callback_pipe_cmd("iptables -t nat -A POSTROUTING -o ppp%d -j MASQUERADE",dev_idx);
	
	return SUCCESS;
}
void wq_do_pppoeDsliteBeforeDialByHwCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	__pppoeDsliteBeforeDialByHwCallBack(&p_callback_work->pppoeDsliteBeforeDialCallBack.pppoeDsliteBeforeInfo,&p_callback_work->pppoeDsliteBeforeDialCallBack.wanIdx);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}
int _rtk_rg_pppoeDsliteBeforeDialByHwCallBack(rtk_rg_pppoeClientInfoBeforeDial_t *before_dial, int *intfIdx)
{
	rtk_rg_wq_union_t *pppoeDsliteBeforeDialWq=wq_callbackAssign();
	if(pppoeDsliteBeforeDialWq){
		INIT_WORK(&(pppoeDsliteBeforeDialWq->work), wq_do_pppoeDsliteBeforeDialByHwCallBack);
		memcpy(&(pppoeDsliteBeforeDialWq->pppoeDsliteBeforeDialCallBack.pppoeDsliteBeforeInfo),before_dial,sizeof(rtk_rg_pppoeClientInfoBeforeDial_t));
		pppoeDsliteBeforeDialWq->pppoeDsliteBeforeDialCallBack.wanIdx = *intfIdx;
		queue_work(rg_kernel.rg_callbackWQ,&(pppoeDsliteBeforeDialWq->work));
	}
	return SUCCESS;
}

//int gcount=10;
//rg_db.systemGlobal.initParam.softwareNaptInfoAddCallBack=NULL;
int _rtk_rg_softwareNaptInfoAddCallBack(rtk_rg_naptInfo_t* naptInfo){
	CBACK("enter %s!!! ",__FUNCTION__);
	CBACK("naptTuples.is_tcp = %s",naptInfo->naptTuples.is_tcp?"TCP":"UDP");
	CBACK("naptTuples.local_ip = %s",_inet_ntoa(naptInfo->naptTuples.local_ip));
	CBACK("naptTuples.local_port = %d",naptInfo->naptTuples.local_port);
	CBACK("naptTuples.remote_ip = %s",_inet_ntoa(naptInfo->naptTuples.remote_ip));
	CBACK("naptTuples.remote_port = %d",naptInfo->naptTuples.remote_port);
	CBACK("naptTuples.wan_intf_idx = %d",naptInfo->naptTuples.wan_intf_idx);
	CBACK("naptTuples.inbound_pri_valid = %d",naptInfo->naptTuples.inbound_pri_valid);
	CBACK("naptTuples.inbound_priority = %d",naptInfo->naptTuples.inbound_priority);
	CBACK("naptTuples.external_port = %d",naptInfo->naptTuples.external_port);
	CBACK("naptTuples.outbound_pri_valid = %d",naptInfo->naptTuples.outbound_pri_valid);
	CBACK("naptTuples.outbound_priority = %d",naptInfo->naptTuples.outbound_priority);
	CBACK("idleSecs = %d",naptInfo->idleSecs);
	switch(naptInfo->state){
		case INVALID:
			CBACK("state = INVALID");
			break;
		case SYN_RECV:
			CBACK("state = SYN_RECV");
			break;
		case UDP_FIRST:
			CBACK("state = UDP_FIRST");
			break;
		case SYN_ACK_RECV:
			CBACK("state = SYN_ACK_RECV");
			break;
		case UDP_SECOND:
			CBACK("state = UDP_SECOND");
			break;
		case TCP_CONNECTED:
			CBACK("state = TCP_CONNECTED");
			break;
		case UDP_CONNECTED:
			CBACK("state = UDP_CONNECTED");
			break;
		case FIRST_FIN:
			CBACK("state = FIRST_FIN");
			break;
		case RST_RECV:
			CBACK("state = RST_RECV");
			break;
		case FIN_SEND_AND_RECV:
			CBACK("state = FIN_SEND_AND_RECV");
			break;
		case LAST_ACK:
			CBACK("state = LAST_ACK");
			break;
		default:
			break;
	}

	//*naptInfo->pContext=&gcount;
	//CBACK("%s add napt sw flow, pContext is %p %d",__FUNCTION__,*naptInfo->pContext,(*(int *)(*naptInfo->pContext))++);
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.softwareNaptInfoDeleteCallBack=NULL;
int _rtk_rg_softwareNaptInfoDeleteCallBack(rtk_rg_naptInfo_t* naptInfo){
	CBACK("enter %s!!! ",__FUNCTION__);
	CBACK("naptTuples.is_tcp = %s",naptInfo->naptTuples.is_tcp?"TCP":"UDP");
	CBACK("naptTuples.local_ip = %s",_inet_ntoa(naptInfo->naptTuples.local_ip));
	CBACK("naptTuples.local_port = %d",naptInfo->naptTuples.local_port);
	CBACK("naptTuples.remote_ip = %s",_inet_ntoa(naptInfo->naptTuples.remote_ip));
	CBACK("naptTuples.remote_port = %d",naptInfo->naptTuples.remote_port);
	CBACK("naptTuples.wan_intf_idx = %d",naptInfo->naptTuples.wan_intf_idx);
	CBACK("naptTuples.inbound_pri_valid = %d",naptInfo->naptTuples.inbound_pri_valid);
	CBACK("naptTuples.inbound_priority = %d",naptInfo->naptTuples.inbound_priority);
	CBACK("naptTuples.external_port = %d",naptInfo->naptTuples.external_port);
	CBACK("naptTuples.outbound_pri_valid = %d",naptInfo->naptTuples.outbound_pri_valid);
	CBACK("naptTuples.outbound_priority = %d",naptInfo->naptTuples.outbound_priority);
	CBACK("idleSecs = %d",naptInfo->idleSecs);
	switch(naptInfo->state){
		case INVALID:
			CBACK("state = INVALID");
			break;
		case SYN_RECV:
			CBACK("state = SYN_RECV");
			break;
		case UDP_FIRST:
			CBACK("state = UDP_FIRST");
			break;
		case SYN_ACK_RECV:
			CBACK("state = SYN_ACK_RECV");
			break;
		case UDP_SECOND:
			CBACK("state = UDP_SECOND");
			break;
		case TCP_CONNECTED:
			CBACK("state = TCP_CONNECTED");
			break;
		case UDP_CONNECTED:
			CBACK("state = UDP_CONNECTED");
			break;
		case FIRST_FIN:
			CBACK("state = FIRST_FIN");
			break;
		case RST_RECV:
			CBACK("state = RST_RECV");
			break;
		case FIN_SEND_AND_RECV:
			CBACK("state = FIN_SEND_AND_RECV");
			break;
		case LAST_ACK:
			CBACK("state = LAST_ACK");
			break;
		default:
			break;
	}
	//CBACK("%s delete napt sw flow, pContext is %p %d",__FUNCTION__,*naptInfo->pContext,(*(int *)(*naptInfo->pContext)));
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.pppoeLCPStateCallBack=NULL;
int _rtk_rg_pppoeLCPStateCallBack(void **data)
{
	int off=12+2+6;	//ether + ethType + pppoeHdr
#ifdef CONFIG_RG_BRIDGE_PPP_STATUS	
	unsigned int sessionId;
	unsigned short etherType;
	unsigned short protocol;
	unsigned int sessionState = STATE_UNCHANGE;
#endif	
	struct sk_buff *skb = (struct sk_buff *)*data;
	
	//dump_packet(skb->data,skb->len,"pppoe LCP");
	if(skb->len>=off+2 && *(unsigned short *)(skb->data+off)==0xc223){
		off+=2;
		if(*(skb->data+off)==0x3)CBACK("PPP CHAP Success!!\n");
		else if(*(skb->data+off)==0x4)CBACK("PPP CHAP Failed....\n");
	}

#ifdef CONFIG_RG_BRIDGE_PPP_STATUS	
	//parse packet to log the session state.
	off = 12;
	if(*(unsigned short*)(skb->data+off)==0x8100)	//skip vlan
	{
		off += 4;
	}
	etherType = *(unsigned short*)(skb->data+off);
	off += 2;
	sessionId = *(unsigned short *)(skb->data+off+2);
	if(0x8863 == etherType)
	{
		//pppoed
		unsigned char code = skb->data[off+1];
		if((0!=sessionId)&&(0xa7==code))
		{
			//PADT with session id
			sessionState = STATE_DOWN;
		}
	}
	else if(0x8864 == etherType)
	{
		//pppoes
		off += 6;	//skip pppoeheader
		protocol = *(unsigned short *)(skb->data+off);
		off += 2;
		
		if(protocol==0xc021)		//LCP
		{
			if((*(skb->data+off)==0x5)||(*(skb->data+off)==0x6))
			{
				//term request&reply
				sessionState = STATE_DOWN;
				//printk("[%s %d]sessionState=%d\n", __func__, __LINE__, sessionState);
			}
			else if(*(skb->data+off)==0xa)
			{
				//echo reply
				sessionState = STATE_UP;
				//printk("[%s %d]sessionState=%d\n", __func__, __LINE__, sessionState);
			}
		}
		else if(protocol==0xc023)		//PAP
		{
			if(*(skb->data+off)==0x2)	//ack
			{
				sessionState = STATE_AUTH_SUCCESS;
				//printk("[%s %d]sessionState=%d\n", __func__, __LINE__, sessionState);
			}
			else if(*(skb->data+off)==0x3)	//nak
			{
				sessionState = STATE_AUTH_FAILED;
				//printk("[%s %d]sessionState=%d\n", __func__, __LINE__, sessionState);
			}
		}
		else if(protocol==0xc223)		//CHAP
		{
			if(*(skb->data+off)==0x3)	//ack
			{
				sessionState = STATE_AUTH_SUCCESS;
				//printk("[%s %d]sessionState=%d\n", __func__, __LINE__, sessionState);
			}
			else if(*(skb->data+off)==0x4)	//nak
			{
				sessionState = STATE_AUTH_FAILED;
				//printk("[%s %d]sessionState=%d\n", __func__, __LINE__, sessionState);
			}
		}
		else if((protocol==0x8021)||(protocol==0x8057))		//IPCP or IPv6CP
		{
			if(*(skb->data+off)==0x2)
			{
				sessionState = STATE_IPCP_ACK;
				//printk("[%s %d]sessionState=%d\n", __func__, __LINE__, sessionState);
			}
		}
	}
	
	if(sessionState != STATE_UNCHANGE)
	{
		logSessionStateForBridge(sessionId, sessionState);
	}
#endif	

#ifdef __KERNEL__
	//1 NOTICE: we should free the skb here, otherwise it leaks!!
	_rtk_rg_dev_kfree_skb_any(skb);
#endif
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.naptPreRouteDPICallBack=NULL;
int _rtk_rg_naptPreRouteDPICallBack(void *data, rtk_rg_naptDirection_t direct){
	rtk_rg_pktHdr_t *pPktHdr=(rtk_rg_pktHdr_t *)data;
	int8 sip_buf[4*sizeof "123"], dip_buf[4*sizeof "123"];
#if 0
	rtk_rg_naptInfo_t naptEntry;
	int naptIdx=pPktHdr->naptOutboundIndx;

	if(rtk_rg_naptConnection_find(&naptEntry,&naptIdx)==RT_ERR_RG_OK)
		WARNING("%s the flow's pcontext is %p %d",__FUNCTION__,*naptEntry.pContext,(*(int *)(*naptEntry.pContext))++);
#else
	//CBACK("%s the flow's pcontext is %p %d",__FUNCTION__,rg_db.naptOut[pPktHdr->naptOutboundIndx].pContext,(*(int *)(rg_db.naptOut[pPktHdr->naptOutboundIndx].pContext))++);
#endif
	//only for display string
	memcpy(sip_buf,_inet_ntoa(pPktHdr->ipv4Sip),sizeof(sip_buf));
	memcpy(dip_buf,_inet_ntoa(pPktHdr->ipv4Dip),sizeof(dip_buf));

	CBACK("Callback %s enter!! ppkthdr=%p",__FUNCTION__,pPktHdr);

	CBACK("direction = %s",direct==NAPT_DIRECTION_OUTBOUND?"NAPT":"NAPTR");
	CBACK("protocol = 0x%x",pPktHdr->ipProtocol);
	CBACK("ipv4Sip: %s -> %s",sip_buf,_inet_ntoa(*pPktHdr->pIpv4Sip));
	CBACK("ipv4Dip: %s -> %s",dip_buf,_inet_ntoa(*pPktHdr->pIpv4Dip));
	if((pPktHdr->tagif&TCP_TAGIF)||(pPktHdr->tagif&UDP_TAGIF)){
		CBACK("sport: %d -> %d",pPktHdr->sport,ntohs(*pPktHdr->pSport));
		CBACK("dport: %d -> %d",pPktHdr->dport,ntohs(*pPktHdr->pDport));
	}

	//default return value: RG_FWDENGINE_PREROUTECB_CONTINUE
	return rg_db.systemGlobal.demo_dpiPreRouteCallback_retValue;
}

//rg_db.systemGlobal.initParam.naptForwardDPICallBack=NULL;
int _rtk_rg_naptForwardDPICallBack(void *data, rtk_rg_naptDirection_t direct){
	rtk_rg_pktHdr_t *pPktHdr=(rtk_rg_pktHdr_t *)data;
	int8 sip_buf[4*sizeof "123"], dip_buf[4*sizeof "123"];

#if 0
	rtk_rg_naptInfo_t naptEntry;
	int naptIdx=pPktHdr->naptOutboundIndx;
	
	if(rtk_rg_naptConnection_find(&naptEntry,&naptIdx)==RT_ERR_RG_OK)
		WARNING("%s the flow's pcontext is %p %d",__FUNCTION__,*naptEntry.pContext,(*(int *)(*naptEntry.pContext))++);
#else
	//rg_db.naptOut[pPktHdr->naptOutboundIndx].rtk_naptOut.priValid=1;
	//rg_db.naptOut[pPktHdr->naptOutboundIndx].rtk_naptOut.priValue=4;
	//rg_db.naptIn[pPktHdr->naptrInboundIndx].rtk_naptIn.priValid=1;
	//rg_db.naptIn[pPktHdr->naptrInboundIndx].rtk_naptIn.priId=6;
	//CBACK("%s the flow's pcontext is %p %d",__FUNCTION__,rg_db.naptOut[pPktHdr->naptOutboundIndx].pContext,(*(int *)(rg_db.naptOut[pPktHdr->naptOutboundIndx].pContext))++);
#endif
	//only for display string
	memcpy(sip_buf,_inet_ntoa(pPktHdr->ipv4Sip),sizeof(sip_buf));
	memcpy(dip_buf,_inet_ntoa(pPktHdr->ipv4Dip),sizeof(dip_buf));

	CBACK("Callback %s enter!! ppkthdr=%p",__FUNCTION__,pPktHdr);
	
	CBACK("direction = %s",direct==NAPT_DIRECTION_OUTBOUND?"NAPT":"NAPTR");
	CBACK("protocol = 0x%x",pPktHdr->ipProtocol);
	CBACK("ipv4Sip: %s -> %s",sip_buf,_inet_ntoa(*pPktHdr->pIpv4Sip));
	CBACK("ipv4Dip: %s -> %s",dip_buf,_inet_ntoa(*pPktHdr->pIpv4Dip));
	if((pPktHdr->tagif&TCP_TAGIF)||(pPktHdr->tagif&UDP_TAGIF)){
		CBACK("sport: %d -> %d",pPktHdr->sport, ntohs(*pPktHdr->pSport));
		CBACK("dport: %d -> %d",pPktHdr->dport, ntohs(*pPktHdr->pDport));
	}

	//default return value:RG_FWDENGINE_FORWARDCB_FINISH_DPI
	return rg_db.systemGlobal.demo_dpiFwdCallback_retValue;
}

void _rtk_rg_syncPortRangeUsedByPS(void)
{
	struct file *fp_proc = NULL;
	char buf[20]={0};
	mm_segment_t old_fs;
	
	//sync kernel port range into rg
	fp_proc = filp_open("/proc/sys/net/ipv4/ip_local_port_range", O_RDONLY, 0);	
	if(fp_proc!=NULL && !IS_ERR(fp_proc))
	{
		old_fs = get_fs();
		set_fs(KERNEL_DS);
		if(fp_proc->f_op && fp_proc->f_op->read) 
      		fp_proc->f_op->read(fp_proc, buf, sizeof(buf), &fp_proc->f_pos); 
		set_fs(old_fs);	
		
		sscanf(buf, "%hu %hu", &rg_kernel.lowerBoundPortUsedByPS, &rg_kernel.upperBoundPortUsedByPS);
		filp_close(fp_proc, NULL);
		DEBUG("sync port range %d~%d from /proc/sys/net/ipv4/ip_local_port_range\n", rg_kernel.lowerBoundPortUsedByPS, rg_kernel.upperBoundPortUsedByPS);
	}else{
		WARNING("file /proc/sys/net/ipv4/ip_local_port_range open failed\n");		
	}	
	return;
}

void wq_do_syncPortRangeUsedByPS(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	_rtk_rg_syncPortRangeUsedByPS();
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
	return;
}
int _rtk_rg_syncPortRangeUsedByPSCallBack(void)
{
	rtk_rg_wq_union_t *initWq=wq_callbackAssign();
	if(initWq){
		INIT_WORK(&(initWq->work), wq_do_syncPortRangeUsedByPS);
		queue_work(rg_kernel.rg_callbackWQ,&(initWq->work));
	}
	return SUCCESS;
}

void _rtk_rg_syncPortRangeToPS(void)
{
	rtk_rg_callback_pipe_cmd("echo %d %d > /proc/sys/net/ipv4/ip_local_port_range", rg_kernel.lowerBoundPortUsedByPS, rg_kernel.upperBoundPortUsedByPS);	
	return;
}

void wq_do_syncPortRangeToPS(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	_rtk_rg_syncPortRangeToPS();
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
	return;
}
int _rtk_rg_syncPortRangeToPSCallBack(void)
{
	rtk_rg_wq_union_t *initWq=wq_callbackAssign();
	if(initWq){
		INIT_WORK(&(initWq->work), wq_do_syncPortRangeToPS);
		queue_work(rg_kernel.rg_callbackWQ,&(initWq->work));
	}
	return SUCCESS;
}

void _rtk_rg_aclUDPencapMirrorLoopBack(unsigned short port_num, struct sk_buff *skb)
{
	struct socket *clientsocket=NULL;
	struct sockaddr_in to;
	struct msghdr msg;
	struct iovec iov;
	mm_segment_t oldfs;
	int len;
	struct sockaddr_in myaddr;
	int err_bind;
	int udp_payload=42;//MAC12+PROTO2+IP20+UDP8

	/*dump_packet(skb->data,skb->len,"call back function receive");
	dump_packet(skb->data+udp_payload,skb->len-udp_payload,"udp payload");
	printk( KERN_ERR "port=%d, len=%d, udp_payload=%d, pPktHdr->l4Offset=%d", htons(pPktHdr->aclDecision.action_encap_udp.encap_Dport), skb->len, udp_payload, pPktHdr->l4Offset);*/

	/* socket to send data */
	if (sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &clientsocket) < 0) {
		printk( KERN_ERR "server: Error creating udp clientsocket.n" );
		goto freeData;
	}

	memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(18888);

	/* Bind to the address to which the service will be offered */
	err_bind = clientsocket->ops->bind(clientsocket, (struct sockaddr *)&myaddr, sizeof(struct sockaddr));
	if(err_bind < 0){
		printk(KERN_ERR "%s: Could not bind or connect to socket, error = %d\n", __FUNCTION__, -err_bind);
		goto freeData;
	}

	/* generate answer message */
	memset(&to,0, sizeof(to));
	to.sin_family = AF_INET;
	to.sin_addr.s_addr = in_aton("127.0.0.1");  
	to.sin_port = htons(port_num);

	/* send the message */
	memset(&msg,0,sizeof(msg));
	msg.msg_flags = MSG_NOSIGNAL;//0/*MSG_DONTWAIT*/;
	msg.msg_name = &to;
	msg.msg_namelen = sizeof(to);
	msg.msg_controllen = 0;
	msg.msg_control = NULL;

	/* send the message */
#if defined(CONFIG_RG_G3_SERIES)
	if(unlikely(import_single_range(WRITE, skb->data+udp_payload, skb->len-udp_payload, &iov, &msg.msg_iter))) {
		printk(KERN_ERR "%s: Could not prepare send buffer\n", __FUNCTION__);
		goto freeData;
	}
#else
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	iov.iov_base = skb->data+udp_payload;
	iov.iov_len = skb->len-udp_payload;
#endif

	oldfs = get_fs();
	set_fs(KERNEL_DS);
#if defined(CONFIG_RG_G3_SERIES)
	len = sock_sendmsg(clientsocket,&msg);
#else
	len = sock_sendmsg(clientsocket,&msg,iov.iov_len);
#endif
	set_fs(oldfs);
	//printk(KERN_ERR "DONE! Send packet len %d to 127.0.0.1:5000\n", len);

freeData:
	//close socket
	if (clientsocket)
		sock_release(clientsocket);

	_rtk_rg_dev_kfree_skb_any(skb);

	return;
}

void wq_do_aclUDPencapMirrorLoopBackCallBack(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	_rtk_rg_aclUDPencapMirrorLoopBack(p_callback_work->udpMirrorCallBack.port_num, p_callback_work->udpMirrorCallBack.skb);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
}

int _rtk_rg_aclUDPencapMirrorLoopBackCallBack(struct sk_buff *skb, uint16 portNum)
{
	rtk_rg_wq_union_t *initWq=wq_callbackAssign();
	if(initWq){
		INIT_WORK(&(initWq->work), wq_do_aclUDPencapMirrorLoopBackCallBack);
		initWq->udpMirrorCallBack.skb=skb;
		initWq->udpMirrorCallBack.port_num=portNum;
		queue_work(rg_kernel.rg_callbackWQ,&(initWq->work));
	}else
	{
		//allocate fail...release skb.
		_rtk_rg_dev_kfree_skb_any(skb);
	}
	return SUCCESS;
}

#if defined(CONFIG_RG_G3_SERIES)
void wq_do_g3_devmem_set(struct work_struct *p_work)
{
	rtk_rg_wq_union_t *p_callback_work = container_of(p_work, rtk_rg_wq_union_t, work);
	rtk_rg_callback_pipe_cmd("devmem 0x%x 32 0x%x", p_callback_work->devmemCallBack.reg, p_callback_work->devmemCallBack.value);
	atomic_set(&p_callback_work->not_used,1);	//reset to 1
	return;
}
int _rtk_rg_g3_devmem_set(uint32 reg, uint32 value)
{
	rtk_rg_wq_union_t *devmemWq=wq_callbackAssign();
	if(devmemWq){
		INIT_WORK(&(devmemWq->work), wq_do_g3_devmem_set);
		devmemWq->devmemCallBack.reg = reg;
		devmemWq->devmemCallBack.value = value;
		queue_work(rg_kernel.rg_callbackWQ,&(devmemWq->work));
	}
	return SUCCESS;
}
#endif	// end CONFIG_RG_G3_SERIES

#endif

#endif

