/*
 * BDs.h
 *
 *  Created on: Apr 11, 2017
 *      Author: xieliming
 */

#ifndef BDS_H_
#define BDS_H_
#include "conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#define XB_DATALEN 496
#define PACKED __attribute__( ( packed, aligned(1) ) )

#pragma pack(1)
struct SOneAx
{
	unsigned int wAxID;          //peizhi wenjian peizhi
	float fValue;
	unsigned short wStatue;      //dingsi  0
};
struct SOneDx
{
	unsigned int wDxID;
	unsigned short wStatue;     //byte0 0  1
};

struct XPacket
{
	unsigned short wSendCnt;            //send after ++
	unsigned short wReserve;            //dingshi 0x628
	unsigned short wDstNode;            //dingshi 0xffff
	unsigned short wSrcNode;            //240
	unsigned short wLen;                //n*sizeof(SOneAx)  The Message Packet DataLength:byData
	unsigned short wType;               //Ax:0x0001    Dx:0x0002
	unsigned short wSrcTask;            //baoliu
	unsigned short wTraceWord;          //buguan
	unsigned char byData[XB_DATALEN];   //n*sizeof(SOneAx)
};



class BroadOperation
{
public:
	BroadOperation();
	bool init(char * taip,char * tbip);
	~BroadOperation();
	bool SendBroadData(short pval,int pid,unsigned short pstatue=0,unsigned short ptype=0x0001);
public:
	int fre_id1;
	int fre_id2;
	int ac_id1;
	int ac_id2;
	int reac_id1;
	int reac_id2;

	int sta_id;
	int node;
	int port;

	int afd;
	struct sockaddr_in Aaddr;
	struct sockaddr_in b_Aaddr;
	int bfd;
	struct sockaddr_in Baddr;
	struct sockaddr_in b_Baddr;
};
#endif /* BDS_H_ */
