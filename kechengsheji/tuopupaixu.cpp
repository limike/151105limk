#include "stdlib.h"
#include "stdio.h"
#include<fstream>
#include<iostream>
#include "string"
using namespace std;

/* ������ʽ�������� */
typedef int ElemType;
typedef struct QNode
{	
	ElemType  data;
	struct QNode *next;
}QNode,*QueuePtr;
typedef struct
{	QueuePtr front;
	QueuePtr rear;
}LinkQueue;

/* 1.��ʼ����ʽ���� */
void InitQueue(LinkQueue *Q)
{	
	Q->front=Q->rear=(QueuePtr)malloc(sizeof(QNode));
	if (!(Q->front)) exit(0);
	Q->front->next=NULL; 
}

/* 2.�жϿն��� */
int QueueEmpty(LinkQueue Q)
{	
	if(Q.front==Q.rear)
		return 1;
	else 
		return 0; 
}
/* */
int QueueLength(LinkQueue Q)
{
	return (Q.rear-Q.front);
}

/* 3.����� */
void EnQueue(LinkQueue *Q, ElemType e)
{	
	QueuePtr p;
	p=(QueuePtr)malloc(sizeof(QNode));
	if (!p) exit(0);
	p->data=e; p->next=NULL;
	Q->rear->next=p;
	Q->rear=p; 
}

/* 4.������ */
void DeQueue(LinkQueue *Q, ElemType *e)
{	
	QueuePtr p;
	if(Q->front!=Q->rear)
	{	
		p=Q->front->next;
		*e=p->data;
		Q->front->next=p->next;
		if (Q->rear==p) Q->rear=Q->front;
		free(p); 
	}
}

#define MAX_VERTEX_NUM 100 //��󶥵����
#define MAX_ARC_NUM 4950
//typedef char VertexType[20]; //������Ϣ�����ƣ�
int termNum,gradeM,lessonNum=100,count=0;//ѧ������,ÿѧ�ڵ�ѧ������,רҵ������,������γ̼�������
string lessonNumber;
string lessonManage[12];
/* ͼ�����Ͷ��壨�ڽӱ�洢�ṹ�� */
typedef struct ArcNode //������
{	int vexpos; //�û���ָ��Ķ����������е�λ��
	struct ArcNode *next; //ָ��ǰ������һ������ָ��
}ArcNode;
typedef struct VNode //ͷ���
{	string name; //������Ϣ�����ƣ�
	string mingcheng;
	int grade;
	int indegree; //�������
	ArcNode *firstarc; //ָ��ǰ����ĵ�һ������ָ��
}VNode,AdjList[MAX_VERTEX_NUM];
typedef struct
{	AdjList vexhead; //�ڽӱ�ͷ�������
	int vexnum,arcnum; //ͼ�Ķ������ͻ���
}ALGraph;

/* 5.������ͷ��������еĶ�λ */
int LocateVex(ALGraph G,string v)
{	
	int i;
	for(i=0;i<G.vexnum;i++)
		if(v.compare(G.vexhead[i].name)==0) break;
	return i; 
}

/* 8.����Ԥ�ÿγ�ͼ���ڽӱ� */
int CreateGraph2(ALGraph *G) //�ɹ���������1�����ɹ��򷵻�0
{
	ifstream inf;
	inf.open("data.in");
	string s,tempS;
	int tag=1,i=0,j=0,k=0,m=0;

	string	tempRelationV1[MAX_ARC_NUM],tempRelationV2[MAX_ARC_NUM];

	while(inf>>s){
		switch(tag){
			case 2:
				termNum=atoi(s.c_str());
				break;
			case 4:
				gradeM=atoi(s.c_str());
				break;
			case 6:
				lessonNum=atoi(s.c_str());
				//printf("%d\n",lessonNum);
				break;
			case 8:
				lessonNumber=s.substr(0,1);
				break;
		}
		if(tag>=10&&tag<=lessonNum+9){
			(*G).vexhead[tag-10].grade=atoi(s.c_str());
		}
		
		if((tag-lessonNum-14)>=0&&(tag-lessonNum-14)%3==0){
			tempS=s;
			(*G).vexhead[i++].name=s;
		}
		
		if((tag-lessonNum-14)>=0&&(tag-lessonNum-14)%3==1){
			(*G).vexhead[j++].mingcheng=s;
		}
		if((tag-lessonNum-14)>=0&&(tag-lessonNum-14)%3==2){
			if(s.compare("��")!=0){
				if(s.find("��")!=3){
					tempRelationV1[k++]=s;
					tempRelationV2[m++]=tempS;
				}else{
					while(s.find("��")==3){
						tempRelationV1[k++]=s.substr(0,s.find("��"));
						tempRelationV2[m++]=tempS;
						s=s.substr(s.find("��")+2);
					}
					tempRelationV1[k++]=s;
					tempRelationV2[m++] = tempS;
				}
			}
		}
		tag++;
	}

	(*G).vexnum=lessonNum;	(*G).arcnum=k; 
	string v1,v2; ArcNode *newarc;
	if((*G).vexnum<0||(*G).arcnum<0||(*G).arcnum>(*G).vexnum*((*G).vexnum-1))
	{	
		printf("\n�γ������Ⱥ��ϵ����ȷ������ͼ����ʧ�ܣ�\n");
		return 0;
	} //�жϿγ����ͻ����Ƿ���ȷ

	for(i=0;i<(*G).vexnum;i++) //�ڽӱ��ʼ��
	{	
		(*G).vexhead[i].firstarc=NULL;
		(*G).vexhead[i].indegree=0; 
	}
	
	for(k=0;k<(*G).arcnum;k++)
	{	
		v1.assign(tempRelationV1[k]); v2.assign(tempRelationV2[k]);
		//strcpy(v1,tempRelationV1[k]);	����ʹ��
		//strcpy(v2,tempRelationV2[k]);
		i=LocateVex(*G,v1);j=LocateVex(*G,v2); //��λ�γ̲��жϿγ��Ƿ����
		if(i>=(*G).vexnum)
		{	
			cout<<"�γ�"+v1+"�����ڣ�����ͼ����ʧ�ܣ�"<<endl;
			return 0;
		}
		if(j>=(*G).vexnum)
		{
			cout<<"�γ�"+v2+"�����ڣ�����ͼ����ʧ�ܣ�"<<endl;
			return 0;
		}

		newarc=(ArcNode*)malloc(sizeof(ArcNode)); //ǰ�巨���γ�����
		newarc->vexpos=j;
		if((*G).vexhead[i].firstarc==NULL)
		{	newarc->next=NULL;
			(*G).vexhead[i].firstarc=newarc; 
		}else{	
			newarc->next=(*G).vexhead[i].firstarc->next;
			(*G).vexhead[i].firstarc->next=newarc; 
		}
		(*G).vexhead[j].indegree++; //��Ӧ�γ���ȼ�����1
	}

	printf("\n����ͼ�����ɹ���\n");

	return 1;
}

void PrintGraph2(ALGraph G)
{
	int i;ArcNode *p;
	printf("\n�������ͼ:\n");
	for(i=0; i<G.vexnum; i++)
	{
		cout<<"\n�γ�:   "+G.vexhead[i].name<<endl;
		printf("���:%3d\n",G.vexhead[i].indegree);
		p=G.vexhead[i].firstarc;
		printf("�Ա��γ�Ϊ�����Ŀγ�:");
		while(p!=NULL)
		{
			cout<<"   "+G.vexhead[p->vexpos].name<<endl;
			p=p->next;
		}
		printf("\n");
	}
}


/* 10.�������� */
void TopologicalSort(ALGraph G)
{	
	int i,k,count;ElemType e;ArcNode *p;
	LinkQueue Q; /*�������*/
	InitQueue(&Q);
	for(i=0; i<G.vexnum; i++) //����ȿγ������
		if(!G.vexhead[i].indegree) EnQueue(&Q,i); 
	count=0; //������γ̼���������ʼ��
	printf("\n\n\n���Ͽγ̵�һ��������������Ϊ:\n");
	while(!QueueEmpty(Q))
	{	
		DeQueue(&Q,&e); //�Ƚ����Ϊ��Ŀγ����
		cout<<G.vexhead[e].name+"   ";
		count++; //������Ķ������
		for(p=G.vexhead[e].firstarc;p;p=p->next) //������ǰ�γ̵��ڽӵ�
		{	
			k=p->vexpos; //�ڽӵ�λ��
			if(!(--G.vexhead[k].indegree)) //ÿ���ڽӵ���ȼ�1����Ϊ���������
				EnQueue(&Q,k);
		}
	}
	printf("\n");
	if(count<G.vexnum)
		{	printf("\n������ͼ�л�·���޷������������\n"); }
}

int sumG(ALGraph G,int v[],int v1[],int num,LinkQueue *Q){//ʹ�γ̾����ܵؼ�����ǰ����ѧ����
	int i,j,temp,su=0;
	if(v[0]!=(-1)&&v1[0]==(-1)){
		for(i=0;i<num-1;i++){
			for(j=0;j<num-1-i;j++){
				if(G.vexhead[v[j]].grade>G.vexhead[v[j+1]].grade){
					temp=v[j];
					v[j]=v[j+1];
					v[j+1]=temp;
				}
			}
		}
		for(i=0;i<num;i++){
			su+=G.vexhead[v[i]].grade;
			if(su>10) break;
		}
		for(j=0;j<i;j++){
			EnQueue(Q,v[j]);
		}
		for(j=0;i<num;i++,j++){
			v1[j]=v[i];
		}
	}else{
		for(i=0;i<num-1;i++){
			for(j=0;j<num-1-i;j++){
				if(G.vexhead[v1[j]].grade>G.vexhead[v1[j+1]].grade){
					temp=v1[j];
					v1[j]=v1[j+1];
					v1[j+1]=temp;
				}
			}
		}
		for(i=0;i<num;i++){
			su+=G.vexhead[v1[i]].grade;
			if(su>10) break;
		}
		for(j=0;j<i;j++){
			EnQueue(Q,v1[j]);
		}
		for(j=0;i<num;i++,j++){
			v[j]=v1[i];
		}
	}
	
	return j;
}
int sumG1(ALGraph G,int v[],int v1[],int num,LinkQueue *Q){
	int i,j,temp,su=0;
	for(i=0;i<num-1;i++){
		for(j=0;j<num-1-i;j++){
			if(G.vexhead[v[j]].grade>G.vexhead[v[j+1]].grade){
				temp=v[j];
				v[j]=v[j+1];
				v[j+1]=temp;
			}
		}
	}
	for(i=0;i<num;i++){
		su+=G.vexhead[v[i]].grade;
		if(su>10) break;
	}
	for(j=0;j<i;j++){
		EnQueue(Q,v[j]);
	}
	for(j=0;i<num;i++,j++){
		v1[j]=v[i];
	}

	return j;
}

int learnManage(ALGraph G,LinkQueue *Q,ofstream *outf,int v1[],int i){
	ElemType e;ArcNode *p;
	int linetag=1,tempTerm=1,k;
	while(!QueueEmpty(*Q))
	{	
		DeQueue(Q,&e); //�Ƚ����Ϊ��Ŀγ����
		if(linetag==1){
			(*outf)<<"			"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
		}else{
			(*outf)<<"				"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
		}
		cout<<G.vexhead[e].name+"   ";
		count++; //������Ķ������
		for(p=G.vexhead[e].firstarc;p;p=p->next) //������ǰ�γ̵��ڽӵ�
		{	
			k=p->vexpos; //�ڽӵ�λ��
			if(!(--G.vexhead[k].indegree)) //ÿ���ڽӵ���ȼ�1����Ϊ���������
				//EnQueue(&Q,k);
				v1[i++]=k;
		}
		linetag++;
		//printf("------ %d",i);
	}

	return i;
}

int learnManage1(ALGraph G,LinkQueue *Q,ofstream *outf,int v[],int i){
	ElemType e;ArcNode *p;
	int linetag=1,k;
	while(!QueueEmpty(*Q))
	{	
		DeQueue(Q,&e); //�Ƚ����Ϊ��Ŀγ����
		if(linetag==1){
			(*outf)<<"			"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
		}else{
			(*outf)<<"				"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
		}
		cout<<G.vexhead[e].name+"   ";
		count++; //������Ķ������
		for(p=G.vexhead[e].firstarc;p;p=p->next) //������ǰ�γ̵��ڽӵ�
		{	
			k=p->vexpos; //�ڽӵ�λ��
			if(!(--G.vexhead[k].indegree)) //ÿ���ڽӵ���ȼ�1����Ϊ���������
				//EnQueue(&Q,k);
				v[i++]=k;
		}
		linetag++;
	}

	return i;
}

void testTopologicalSort(ALGraph G)
{
	ofstream outf;
	outf.open("data.out");

	int i,j=1,k,linetag=1,num=0,termtag=2;
	int v[MAX_VERTEX_NUM],v1[MAX_VERTEX_NUM];
	printf("\n\n\n���Ͽγ̵�һ��������������Ϊ:\n");
	memset(v,-1,sizeof(v));
	memset(v1,-1,sizeof(v1));
	//ElemType e;ArcNode *p;
	LinkQueue Q; /*�������*/
	InitQueue(&Q);
	outf<<"				�γ̱��		�γ�����		�γ�ѧ��"<<"\n";
	

	for(i=0; i<G.vexnum; i++) //����ȿγ������
		if(!G.vexhead[i].indegree){ v[num++]=i;}


	while(j<=termNum){
		outf<<"---------------------------------------------------------------"<<"\n";
		outf<<"��"<<j<<"ѧ��";
		i=sumG(G,v,v1,num,&Q);
		num=learnManage(G,&Q,&outf,v1,i);
		memset(v,-1,sizeof(v));

		outf<<"---------------------------------------------------------------"<<endl;
		outf<<"��"<<j+1<<"ѧ��";
		i=sumG(G,v,v1,num,&Q);
		num=learnManage1(G,&Q,&outf,v,i);
		memset(v1,-1,sizeof(v1));

		j+=2;
	}
	

/*
	outf<<"��1ѧ��";
	while(!QueueEmpty(Q))
	{	
		DeQueue(&Q,&e); //�Ƚ����Ϊ��Ŀγ����
		if(linetag==1){
			outf<<"			"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
		}else{
			outf<<"				"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
		}
		cout<<G.vexhead[e].name+"   ";
		count++; //������Ķ������
		for(p=G.vexhead[e].firstarc;p;p=p->next) //������ǰ�γ̵��ڽӵ�
		{	
			k=p->vexpos; //�ڽӵ�λ��
			if(!(--G.vexhead[k].indegree)) //ÿ���ڽӵ���ȼ�1����Ϊ���������
				//EnQueue(&Q,k);
				v1[i++]=k;
		}
		linetag++;
	}

	while(count<G.vexnum){
		linetag=1;
		num=i;
		j=sumG(G,v1,num,&Q);
		for(i=0;j<num;j++,i++){
			v[i]=v1[j];
		}
		memset(v1,-1,sizeof(v1));
		outf<<"---------------------------------------------------------------"<<endl;
		outf<<"��"<<termtag<<"ѧ��";
		while(!QueueEmpty(Q))
		{	
			DeQueue(&Q,&e); //�Ƚ����Ϊ��Ŀγ����
			if(linetag==1){
				outf<<"			"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
			}else{
				outf<<"				"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
			}
			cout<<G.vexhead[e].name+"   ";
			count++; //������Ķ������
			for(p=G.vexhead[e].firstarc;p;p=p->next) //������ǰ�γ̵��ڽӵ�
			{	
				k=p->vexpos; //�ڽӵ�λ��
				if(!(--G.vexhead[k].indegree)) //ÿ���ڽӵ���ȼ�1����Ϊ���������
					//EnQueue(&Q,k);
					v[i++]=k;
			}
			linetag++;
		}
		
		if(termtag<termNum){
			linetag=1;
			num=i;
			j=sumG(G,v,num,&Q);
			for(i=0;j<num;j++,i++){
				v1[i]=v[j];
			}
			memset(v,-1,sizeof(v));
			outf<<"---------------------------------------------------------------"<<endl;
			outf<<"��"<<termtag+1<<"ѧ��";
			while(!QueueEmpty(Q))
			{	
				DeQueue(&Q,&e); //�Ƚ����Ϊ��Ŀγ����
				if(linetag==1){
					outf<<"			"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
				}else{
					outf<<"				"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
				}
				cout<<G.vexhead[e].name+"   ";
				count++; //������Ķ������
				for(p=G.vexhead[e].firstarc;p;p=p->next) //������ǰ�γ̵��ڽӵ�
				{	
					k=p->vexpos; //�ڽӵ�λ��
					if(!(--G.vexhead[k].indegree)) //ÿ���ڽӵ���ȼ�1����Ϊ���������
						//EnQueue(&Q,k);
						v1[i++]=k;
				}
				linetag++;
			}
		}

		termtag+=2;;
	}
	*/
	
	printf("\n");
	if(count<G.vexnum)
		{	printf("\n������ͼ�л�·���޷������������\n"); }
}

int main()
{
	ALGraph G;
	if(CreateGraph2(&G)){
		PrintGraph2(G);
		testTopologicalSort(G);
	} 
	return 0;
}
