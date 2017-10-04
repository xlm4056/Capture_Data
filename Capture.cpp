//============================================================================
// Name        : Capture.cpp
// Author      : xieliming
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "Analyze.h"
#include "DataS.h"
#include "BDs.h"
#include "conf.h"
#include "InitConfFrameData.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <linux/filter.h>
#include <sys/ioctl.h>
#include <pthread.h>
#define PACKED __attribute__( ( packed, aligned(1) ) )
#pragma pack(1)
#define RSBUF 100*1024
unsigned long recverrorcnt=0;                //数据接收错误计数
unsigned long uncompletecnt=0;
unsigned long errorpacketnum=0;
unsigned long broadnum=0;
extern bool debugflag;
extern bool ramdebugflag;
extern Operate Opts;
extern BroadOperation Bopts;
ConfObject Confopts;
char checknet[20]={0};
void * recvThread(void * arg);                //抓取数据报文的线程
void * broadThread(void * arg);
bool   Initconfigfile();                      //初始化配置文件

int main(int argc,char** argv)
{
	printf("++++++Capture Process Start++++++++\n");
	int sock;
	struct ifreq ethreq;
	memset(&ethreq,0,sizeof(ethreq));
	if((sock=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL)))==-1)         //这里很重要 ETH_P_ALL
	{
		fprintf(stderr,"socket was failed:%s\n",strerror(errno));
		close(sock);
		exit(1);
	}

	int rsbuf=RSBUF;
	if(setsockopt(sock,SOL_SOCKET, SO_RCVBUF, (const char*)&rsbuf,sizeof(int))==-1)
	{
		fprintf(stderr,"setsockopt SO_RCVBUF was failed:%s\n",strerror(errno));
		exit(1);
	}
	int b_on = 1;
	if(ioctl(sock, FIONBIO, &b_on)==-1)
	{
		fprintf(stderr,"sock FIONBIO was failed:%s\n",strerror(errno));
		exit(1);
	}
	//设置网卡混杂模式
	if(argc>1)
	{
		if(argc==3)
		{
			if(strcmp(argv[2],"-d")==0)                      //调试模式运行
			{
				debugflag=true;
			}
			else if(strcmp(argv[2],"-r")==0)                 //原生模式运行
			{
				ramdebugflag=true;
			}
		}
		memcpy(ethreq.ifr_ifrn.ifrn_name,argv[1],IFNAMSIZ);
		memcpy(checknet,argv[1],IFNAMSIZ);
	}
	else
	{
		memcpy(ethreq.ifr_ifrn.ifrn_name,"eth1",IFNAMSIZ);
		memcpy(checknet,"enp7s0",IFNAMSIZ);
	}
	//////////////////////////////////////////////////////////////////
	if(ioctl(sock,SIOCGIFFLAGS,&ethreq)==-1)
	{
		fprintf(stderr,"ioctl sock SIOCGIFFLAGS was failed:%s\n",strerror(errno));
		close(sock);
		exit(1);
	}
	ethreq.ifr_ifru.ifru_flags |=IFF_PROMISC;
	if(ioctl(sock,SIOCGIFFLAGS,&ethreq)==-1)
	{
		fprintf(stderr,"ioctl sock SIOCGIFFLAGS was failed:%s\n",strerror(errno));
		close(sock);
		exit(1);
	}

	//Initconfigfile();                                   // Read Config File
	pthread_t rt,bt;
	if(pthread_create(&rt,NULL,recvThread,&sock)!=0)    //数据接收
	{
		fprintf(stderr,"pthread_create recvThread was failed:%s\n",strerror(errno));
		exit(1);
	}
	if(pthread_create(&bt,NULL,broadThread,NULL)!=0)    //数据广播
	{
		fprintf(stderr,"pthread_create broadThread was failed:%s\n",strerror(errno));
	}
	if(pthread_detach(bt)!=0)
	{
		fprintf(stderr,"pthread_detach broadThread was failed:%s\n",strerror(errno));
	}
	if(pthread_join(rt,NULL)!=0)
	{
		fprintf(stderr,"pthread_join recvThread was failed:%s\n",strerror(errno));
		exit(1);
	}
	return 0;
}

void * broadThread(void * arg)
{
	printf("broadThread function run.\n");
	while(1)
	{
		//Broad Fre data
		printf("fre_id1=:%d  fre_id2=:%d ac_id1=:%d ac_id2=:%d reac_id1=:%d reac_id2=:%d sta_id=:%d\n",Bopts.fre_id1,Bopts.fre_id2,Bopts.ac_id1,Bopts.ac_id2,Bopts.reac_id1,Bopts.reac_id2,Bopts.sta_id);
		++broadnum;
		printf("broad datapacket num=:%ld\n",broadnum);
		void * Gval=NULL;
		if(Bopts.fre_id1!=-1)                                    //广播频率数据
		{
			Gval=Opts.GetFrequency1();
			if(Gval==NULL)
			{
				Bopts.SendBroadData(0,Bopts.fre_id1,0x0028);
			}
			else
			{
				Bopts.SendBroadData(*(short*)Gval,Bopts.fre_id1);
			}
		}
		if(Bopts.fre_id2!=-1)
		{
			Gval=Opts.GetFrequency2();
			if(Gval==NULL)
			{
				Bopts.SendBroadData(0,Bopts.fre_id2,0x0028);
			}
			else
			{
				Bopts.SendBroadData(*(short*)Gval,Bopts.fre_id2);
			}
		}
		//Broad ac data
		if(Bopts.ac_id1!=-1)                           //广播有功功率数据
		{
			Gval=Opts.GetActive1();
			if(Gval==NULL)
			{
				Bopts.SendBroadData(0,Bopts.ac_id1,0x0028);
			}
			else
			{
				//			printf("Broad AC=:%d\n",*(short*)Gval);
				Bopts.SendBroadData(*(short*)Gval,Bopts.ac_id1);
			}
		}
		if(Bopts.ac_id2!=-1)
		{
			Gval=Opts.GetActive2();
			if(Gval==NULL)
			{
				Bopts.SendBroadData(0,Bopts.ac_id2,0x0028);
			}
			else
			{
//				printf("Broad AC=:%d\n",*(short*)Gval);
				Bopts.SendBroadData(*(short*)Gval,Bopts.ac_id2);
			}
		}
		//Broad reac data
		if(Bopts.reac_id1!=-1)                                  //广播无功功率数据
		{
			Gval=Opts.GetReActive1();
			if(Gval==NULL)
			{
				Bopts.SendBroadData(0,Bopts.reac_id1,0x0028);
			}
			else
			{
				Bopts.SendBroadData(*(short*)Gval,Bopts.reac_id1);
			}
		}
		if(Bopts.reac_id2!=-1)
		{
			Gval=Opts.GetReActive2();
			if(Gval==NULL)
			{
				Bopts.SendBroadData(0,Bopts.reac_id2,0x0028);
			}
			else
			{
				Bopts.SendBroadData(*(short*)Gval,Bopts.reac_id2);
			}
		}
		//Broad Sta data
		if(Bopts.sta_id!=-1)
		{
			Gval=Opts.GetStat();
			if(Gval==NULL)
			{
				Bopts.SendBroadData(0,Bopts.sta_id,0x0028);
			}
			else
			{
				Bopts.SendBroadData(*(short*)Gval,Bopts.sta_id);
			}
		}
		usleep(500000);
	}
	return (void *)NULL;
}
bool Initconfigfile()                                                  //InitConfig File
{
	char * filename=(char*)"./vxcu.ini";
	int s=GetPrivateProfileInt("NUMMSG","NUM_PMU",-1,filename);
	printf("Read Config File NUM_PMU s=:%d\n",s);
	if(s==-1)
		Confopts.ReadLocalConfigFile();
	return true;
}

void * recvThread(void * arg)
{
	int sock=*((int *)arg);
	fd_set rset;
	FD_ZERO(&rset);
	struct timeval timeout;
	memset(&timeout,0,sizeof(timeout));
	char buf[BUF_SIZE]={0};
	if(ramdebugflag)
	{
		printf("Start Capture Data...\n");
	}
	printf("recvThread function run.\n");
	while(1)
	{
		FD_SET(sock,&rset);
		timeout.tv_sec=0;
		timeout.tv_usec=10000;
		int res=select(sock+1,&rset,NULL,NULL,&timeout);
		if(res<0)
		{
			//select error
			continue;
		}
		else if(res==0)
		{
			//timeout
			continue;
		}
		else if(FD_ISSET(sock,&rset))
		{
			memset(buf,0,BUF_SIZE);
			int readnum=0;
			readnum=recvfrom(sock,buf,BUF_SIZE,0,NULL,NULL);
			if(readnum<42)                                       //数据包不完整
			{
				fprintf(stderr,"recvfrom incomplete packet %s\n",strerror(errno));
				++recverrorcnt;
				continue;
			}
			unsigned char* iphead=NULL;
			//跳过网卡头部
			iphead=(unsigned char *)(buf+14);
			if(*iphead==0x45)
			{
				if(iphead[9]==6)
				{
					TcpAnalyzeFunc(buf,readnum);        //TCP Packet Analyzes
				}
				else if(iphead[9]==17)                  //UDP Packet Analyzes
				{
				}
				else  if(iphead[9]==1)                  //ICMP Packet Analyzes                                                                                                                //OTH Analyzes
				{
				}
				else
				{
				}
			}
		}
	}
	return (void * )NULL;
}
