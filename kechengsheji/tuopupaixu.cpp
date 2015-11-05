#include "stdlib.h"
#include "stdio.h"
#include<fstream>
#include<iostream>
#include "string"
using namespace std;

/* 定义链式队列类型 */
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

/* 1.初始化链式队列 */
void InitQueue(LinkQueue *Q)
{	
	Q->front=Q->rear=(QueuePtr)malloc(sizeof(QNode));
	if (!(Q->front)) exit(0);
	Q->front->next=NULL; 
}

/* 2.判断空队列 */
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

/* 3.入队列 */
void EnQueue(LinkQueue *Q, ElemType e)
{	
	QueuePtr p;
	p=(QueuePtr)malloc(sizeof(QNode));
	if (!p) exit(0);
	p->data=e; p->next=NULL;
	Q->rear->next=p;
	Q->rear=p; 
}

/* 4.出队列 */
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

#define MAX_VERTEX_NUM 100 //最大顶点个数
#define MAX_ARC_NUM 4950
//typedef char VertexType[20]; //顶点信息（名称）
int termNum,gradeM,lessonNum=100,count=0;//学期总数,每学期的学分上限,专业课门数,对输出课程计数变量
string lessonNumber;
string lessonManage[12];
/* 图的类型定义（邻接表存储结构） */
typedef struct ArcNode //链表结点
{	int vexpos; //该弧所指向的顶点在数组中的位置
	struct ArcNode *next; //指向当前起点的下一条弧的指针
}ArcNode;
typedef struct VNode //头结点
{	string name; //顶点信息（名称）
	string mingcheng;
	int grade;
	int indegree; //顶点入度
	ArcNode *firstarc; //指向当前顶点的第一条弧的指针
}VNode,AdjList[MAX_VERTEX_NUM];
typedef struct
{	AdjList vexhead; //邻接表头结点数组
	int vexnum,arcnum; //图的顶点数和弧数
}ALGraph;

/* 5.顶点在头结点数组中的定位 */
int LocateVex(ALGraph G,string v)
{	
	int i;
	for(i=0;i<G.vexnum;i++)
		if(v.compare(G.vexhead[i].name)==0) break;
	return i; 
}

/* 8.建立预置课程图（邻接表） */
int CreateGraph2(ALGraph *G) //成功建立返回1，不成功则返回0
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
			if(s.compare("无")!=0){
				if(s.find("，")!=3){
					tempRelationV1[k++]=s;
					tempRelationV2[m++]=tempS;
				}else{
					while(s.find("，")==3){
						tempRelationV1[k++]=s.substr(0,s.find("，"));
						tempRelationV2[m++]=tempS;
						s=s.substr(s.find("，")+2);
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
		printf("\n课程数或先后关系不正确，有向图建立失败！\n");
		return 0;
	} //判断课程数和弧数是否正确

	for(i=0;i<(*G).vexnum;i++) //邻接表初始化
	{	
		(*G).vexhead[i].firstarc=NULL;
		(*G).vexhead[i].indegree=0; 
	}
	
	for(k=0;k<(*G).arcnum;k++)
	{	
		v1.assign(tempRelationV1[k]); v2.assign(tempRelationV2[k]);
		//strcpy(v1,tempRelationV1[k]);	不能使用
		//strcpy(v2,tempRelationV2[k]);
		i=LocateVex(*G,v1);j=LocateVex(*G,v2); //定位课程并判断课程是否存在
		if(i>=(*G).vexnum)
		{	
			cout<<"课程"+v1+"不存在，有向图建立失败！"<<endl;
			return 0;
		}
		if(j>=(*G).vexnum)
		{
			cout<<"课程"+v2+"不存在，有向图建立失败！"<<endl;
			return 0;
		}

		newarc=(ArcNode*)malloc(sizeof(ArcNode)); //前插法建课程链表
		newarc->vexpos=j;
		if((*G).vexhead[i].firstarc==NULL)
		{	newarc->next=NULL;
			(*G).vexhead[i].firstarc=newarc; 
		}else{	
			newarc->next=(*G).vexhead[i].firstarc->next;
			(*G).vexhead[i].firstarc->next=newarc; 
		}
		(*G).vexhead[j].indegree++; //对应课程入度计数加1
	}

	printf("\n有向图建立成功！\n");

	return 1;
}

void PrintGraph2(ALGraph G)
{
	int i;ArcNode *p;
	printf("\n输出有向图:\n");
	for(i=0; i<G.vexnum; i++)
	{
		cout<<"\n课程:   "+G.vexhead[i].name<<endl;
		printf("入度:%3d\n",G.vexhead[i].indegree);
		p=G.vexhead[i].firstarc;
		printf("以本课程为基础的课程:");
		while(p!=NULL)
		{
			cout<<"   "+G.vexhead[p->vexpos].name<<endl;
			p=p->next;
		}
		printf("\n");
	}
}


/* 10.拓扑排序 */
void TopologicalSort(ALGraph G)
{	
	int i,k,count;ElemType e;ArcNode *p;
	LinkQueue Q; /*定义队列*/
	InitQueue(&Q);
	for(i=0; i<G.vexnum; i++) //零入度课程入队列
		if(!G.vexhead[i].indegree) EnQueue(&Q,i); 
	count=0; //对输出课程计数变量初始化
	printf("\n\n\n以上课程的一个拓扑排序序列为:\n");
	while(!QueueEmpty(Q))
	{	
		DeQueue(&Q,&e); //先将入度为零的课程输出
		cout<<G.vexhead[e].name+"   ";
		count++; //对输出的顶点计数
		for(p=G.vexhead[e].firstarc;p;p=p->next) //遍历当前课程的邻接点
		{	
			k=p->vexpos; //邻接点位置
			if(!(--G.vexhead[k].indegree)) //每个邻接点入度减1后若为零则入队列
				EnQueue(&Q,k);
		}
	}
	printf("\n");
	if(count<G.vexnum)
		{	printf("\n该有向图有回路，无法完成拓扑排序！\n"); }
}

int sumG(ALGraph G,int v[],int v1[],int num,LinkQueue *Q){//使课程尽可能地集中在前几个学期中
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
		DeQueue(Q,&e); //先将入度为零的课程输出
		if(linetag==1){
			(*outf)<<"			"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
		}else{
			(*outf)<<"				"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
		}
		cout<<G.vexhead[e].name+"   ";
		count++; //对输出的顶点计数
		for(p=G.vexhead[e].firstarc;p;p=p->next) //遍历当前课程的邻接点
		{	
			k=p->vexpos; //邻接点位置
			if(!(--G.vexhead[k].indegree)) //每个邻接点入度减1后若为零则入队列
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
		DeQueue(Q,&e); //先将入度为零的课程输出
		if(linetag==1){
			(*outf)<<"			"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
		}else{
			(*outf)<<"				"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
		}
		cout<<G.vexhead[e].name+"   ";
		count++; //对输出的顶点计数
		for(p=G.vexhead[e].firstarc;p;p=p->next) //遍历当前课程的邻接点
		{	
			k=p->vexpos; //邻接点位置
			if(!(--G.vexhead[k].indegree)) //每个邻接点入度减1后若为零则入队列
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
	printf("\n\n\n以上课程的一个拓扑排序序列为:\n");
	memset(v,-1,sizeof(v));
	memset(v1,-1,sizeof(v1));
	//ElemType e;ArcNode *p;
	LinkQueue Q; /*定义队列*/
	InitQueue(&Q);
	outf<<"				课程编号		课程名称		课程学分"<<"\n";
	

	for(i=0; i<G.vexnum; i++) //零入度课程入队列
		if(!G.vexhead[i].indegree){ v[num++]=i;}


	while(j<=termNum){
		outf<<"---------------------------------------------------------------"<<"\n";
		outf<<"第"<<j<<"学期";
		i=sumG(G,v,v1,num,&Q);
		num=learnManage(G,&Q,&outf,v1,i);
		memset(v,-1,sizeof(v));

		outf<<"---------------------------------------------------------------"<<endl;
		outf<<"第"<<j+1<<"学期";
		i=sumG(G,v,v1,num,&Q);
		num=learnManage1(G,&Q,&outf,v,i);
		memset(v1,-1,sizeof(v1));

		j+=2;
	}
	

/*
	outf<<"第1学期";
	while(!QueueEmpty(Q))
	{	
		DeQueue(&Q,&e); //先将入度为零的课程输出
		if(linetag==1){
			outf<<"			"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
		}else{
			outf<<"				"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
		}
		cout<<G.vexhead[e].name+"   ";
		count++; //对输出的顶点计数
		for(p=G.vexhead[e].firstarc;p;p=p->next) //遍历当前课程的邻接点
		{	
			k=p->vexpos; //邻接点位置
			if(!(--G.vexhead[k].indegree)) //每个邻接点入度减1后若为零则入队列
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
		outf<<"第"<<termtag<<"学期";
		while(!QueueEmpty(Q))
		{	
			DeQueue(&Q,&e); //先将入度为零的课程输出
			if(linetag==1){
				outf<<"			"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
			}else{
				outf<<"				"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
			}
			cout<<G.vexhead[e].name+"   ";
			count++; //对输出的顶点计数
			for(p=G.vexhead[e].firstarc;p;p=p->next) //遍历当前课程的邻接点
			{	
				k=p->vexpos; //邻接点位置
				if(!(--G.vexhead[k].indegree)) //每个邻接点入度减1后若为零则入队列
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
			outf<<"第"<<termtag+1<<"学期";
			while(!QueueEmpty(Q))
			{	
				DeQueue(&Q,&e); //先将入度为零的课程输出
				if(linetag==1){
					outf<<"			"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
				}else{
					outf<<"				"<<G.vexhead[e].name<<"				"<<G.vexhead[e].mingcheng<<"		"<<G.vexhead[e].grade<<"\n";
				}
				cout<<G.vexhead[e].name+"   ";
				count++; //对输出的顶点计数
				for(p=G.vexhead[e].firstarc;p;p=p->next) //遍历当前课程的邻接点
				{	
					k=p->vexpos; //邻接点位置
					if(!(--G.vexhead[k].indegree)) //每个邻接点入度减1后若为零则入队列
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
		{	printf("\n该有向图有回路，无法完成拓扑排序！\n"); }
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
