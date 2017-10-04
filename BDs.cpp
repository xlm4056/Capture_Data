/*
 * BDs.cpp
 *
 *  Created on: Apr 11, 2017
 *      Author: xieliming
 */
#include "BDs.h"
#define RSBUF 153600
unsigned short Pstrace=0;
BroadOperation::BroadOperation()
{
	this->node=240;
	this->port=3300;

	this->fre_id1=-1;
	this->fre_id2=-1;
	this->ac_id1=-1;
	this->ac_id2=-1;
	this->reac_id1=-1;
	this->reac_id2=-1;

	this->sta_id=-1;

	memset(&this->Aaddr,0,sizeof(struct sockaddr_in));
	memset(&this->Baddr,0,sizeof(struct sockaddr_in));
	memset(&this->b_Aaddr,0,sizeof(struct sockaddr_in));
	memset(&this->b_Baddr,0,sizeof(struct sockaddr_in));
	char * filename="./vxcu.ini";

	int s;
	s=GetPrivateProfileInt("IDSET","FREID1",-1,filename);
	if(s!=-1)
	{
		this->fre_id1=s;
	}
	s=GetPrivateProfileInt("IDSET","FREID2",-1,filename);
	if(s!=-1)
	{
		this->fre_id2=s;
	}
	s=GetPrivateProfileInt("IDSET","ACID1",-1,filename);
	if(s!=-1)
	{
		this->ac_id1=s;
	}
	s=GetPrivateProfileInt("IDSET","ACID2",-1,filename);
	if(s!=-1)
	{
		this->ac_id2=s;
	}
	s=GetPrivateProfileInt("IDSET","REACID1",-1,filename);
	if(s!=-1)
	{
		this->reac_id1=s;
	}
	s=GetPrivateProfileInt("IDSET","REACID2",-1,filename);
	if(s!=-1)
	{
		this->reac_id2=s;
	}

	s=GetPrivateProfileInt("IDSET","STAID",-1,filename);
	if(s!=-1)
	{
		this->sta_id=s;
	}

	s=GetPrivateProfileInt("NETSET","NODE",-1,filename);
	if(s!=-1)
	{
		this->node=s;
	}
	s=GetPrivateProfileInt("NETSET","PORT",-1,filename);
	if(s!=-1)
	{
		this->port=s;
	}
	char r_aip[32]={0};
	char r_bip[32]={0};
	if(GetPrivateProfileString("NETSET","NetAip",NULL,r_aip,sizeof(r_aip),filename )==0)
	{
		fprintf(stderr,"GetPrivateProfileString was failed:%s\n",strerror(errno));
	}
	if(GetPrivateProfileString("NETSET","NetBip",NULL,r_bip,sizeof(r_bip),filename )==0)
	{
		fprintf(stderr,"GetPrivateProfileString was failed:%s\n",strerror(errno));
	}
	char b_aip[32]={0};
	char b_bip[32]={0};
	int _a=255;
	sprintf(b_aip,"%s.%d",r_aip,_a);
	sprintf(b_bip,"%s.%d",r_bip,_a);
	this->b_Aaddr.sin_family=AF_INET;
	this->b_Aaddr.sin_addr.s_addr=inet_addr(b_aip);
	this->b_Aaddr.sin_port=htons(this->port);

	this->b_Baddr.sin_family=AF_INET;
	this->b_Baddr.sin_addr.s_addr=inet_addr(b_bip);
	this->b_Baddr.sin_port=htons(this->port+1);

	sprintf(r_aip,"%s.%d",r_aip,this->node);
	sprintf(r_bip,"%s.%d",r_bip,this->node);
	if(init(r_aip,r_bip))
	{
		//successful
	}
	printf("\nr_aip=:%s r_bip=:%s b_aip=:%s b_bip=:%s\n",r_aip,r_bip,b_aip,b_bip);
}
bool BroadOperation::init(char * taip,char * tbip)
{
	this->afd=socket(AF_INET,SOCK_DGRAM,0);
	if(this->afd<0)
	{
		fprintf(stderr,"broad socket was failed:%s\n",strerror(errno));
		return false;
	}
	this->bfd=socket(AF_INET,SOCK_DGRAM,0);
	if(this->bfd<0)
	{
		fprintf(stderr,"broad socket was failed:%s\n",strerror(errno));
		return false;
	}
	//set addr reuse
	const int opt=1;
	if(setsockopt(this->afd,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt))!=-1)
	{
		if(setsockopt(this->afd,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt))!=-1)
		{
			int rsbuf=RSBUF;
			if(setsockopt(this->afd,SOL_SOCKET, SO_SNDBUF, (const char*)&rsbuf,sizeof(int))!=-1)
			{
				if(setsockopt(this->afd,SOL_SOCKET, SO_RCVBUF, (const char*)&rsbuf,sizeof(int))!=-1)
				{
					unsigned long nb=1;
					if(ioctl(this->afd,FIONBIO,&nb)!=-1)
					{
//						fprintf(stderr,"setsockopt this->afd socket was successful\n");
					}
				}
			}
		}
	}

	if(setsockopt(this->bfd,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt))!=-1)
	{
		if(setsockopt(this->bfd,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt))!=-1)
		{
			int rsbuf=RSBUF;
			if(setsockopt(this->bfd,SOL_SOCKET, SO_SNDBUF, (const char*)&rsbuf,sizeof(int))!=-1)
			{
				if(setsockopt(this->bfd,SOL_SOCKET, SO_RCVBUF, (const char*)&rsbuf,sizeof(int))!=-1)
				{
					unsigned long nb=1;
					if(ioctl(this->bfd,FIONBIO,&nb)!=-1)
					{
//						fprintf(stderr,"setsockopt this->bfd socket was successful\n");
					}
				}
			}
		}
	}
	this->Aaddr.sin_family=AF_INET;
	this->Aaddr.sin_addr.s_addr=inet_addr((const char*)taip);
	this->Aaddr.sin_port=htons(this->port);

	this->Baddr.sin_family=AF_INET;
	this->Baddr.sin_addr.s_addr=inet_addr((const char*)tbip);
	this->Baddr.sin_port=htons(this->port+1);
	if(bind(this->afd,(struct sockaddr*)&this->Aaddr,sizeof(struct sockaddr_in))!=-1)
	{
		if(bind(this->bfd,(struct sockaddr*)&this->Baddr,sizeof(struct sockaddr_in))!=-1)
		{
			fprintf(stderr,"bind this->abfd socket was successful\n");
		}
	}
	return true;
}
BroadOperation::~BroadOperation()
{

}

bool BroadOperation::SendBroadData(short pval,int pid,unsigned short pstatue,unsigned short ptype)
{
	struct XPacket Packet;
	memset(&Packet,0,sizeof(Packet));
	Packet.wReserve=0x628;
	Packet.wDstNode=0xffff+200;
	Packet.wSrcNode=0+200;
	Packet.wType=ptype;                                            //Ax:0x0001    Dx:0x0002
	Packet.wLen=0;
	struct SOneAx *Ax = (struct SOneAx*)&Packet.byData[Packet.wLen];
	Ax->wStatue=pstatue;
	Packet.wLen+=sizeof(SOneAx);
	Ax->fValue=pval;
	Ax->wAxID=pid;
	Packet.wSendCnt=Pstrace;
	++Pstrace;

	int res=sendto(this->afd,(char*)&Packet.wSendCnt,Packet.wLen+16,0,(struct sockaddr*)&this->b_Aaddr,sizeof(this->b_Aaddr));
	usleep(300000);
	int s=sendto(this->bfd,(char*)&Packet.wSendCnt,Packet.wLen+16,0,(struct sockaddr*)&this->b_Baddr,sizeof(this->b_Baddr));
	if(res<0||s<0)
	{
		return false;
	}
	return true;
}
