//ArpCheat.h

#ifndef MY_ARP_CHEAT_INCLUDE_H
#define MY_ARP_CHEAT_INCLUDE_H

#pragma pack (1)
struct ethernet_head
{
	unsigned char dest_mac[6];		//Ŀ������MAC��ַ
	unsigned char source_mac[6];	//Դ��MAC��ַ
	unsigned short eh_type;			//��̫������
};

struct arp_head
{
	unsigned short hardware_type;	//Ӳ�����ͣ���̫���ӿ�����Ϊ1
	unsigned short protocol_type;	//Э�����ͣ�IPЭ������Ϊ0X0800
	unsigned char add_len;			//Ӳ����ַ���ȣ�MAC��ַ����Ϊ6B
	unsigned char pro_len;			//Э���ַ���ȣ�IP��ַ����Ϊ4B
	unsigned short option;			//������ARP����Ϊ1��ARPӦ��Ϊ2
	unsigned char sour_addr[6];		//ԴMAC��ַ�����ͷ���MAC��ַ
	unsigned long sour_ip;			//ԴIP��ַ�����ͷ���IP��ַ
	unsigned char dest_addr[6];		//Ŀ��MAC��ַ��ARP�����и��ֶ�û�����壻ARP��Ӧ��Ϊ���շ���MAC��ַ
	unsigned long dest_ip;			//Ŀ��IP��ַ��ARP������Ϊ���������IP��ַ��ARP��Ӧ��Ϊ���շ���IP��ַ
	unsigned char padding[18];
};

struct arp_packet					//����arp���ṹ
{
	ethernet_head eth;				//��̫��ͷ��
	arp_head arp;					//arp���ݰ�ͷ��
};
#pragma pack ()
/**
* ���������MAC��ַ
* pDevName �������豸����
*/
unsigned char* GetSelfMac(char* pDevName);
/**
* ��װARP�����
* source_mac ԴMAC��ַ
* srcIP ԴIP
* destIP Ŀ��IP
*/
unsigned char* BuildArpPacket(unsigned char* source_mac, unsigned long srcIP, unsigned long destIP);

#endif