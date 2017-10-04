/*
 * Analyze.h
 *
 *  Created on: 2017年4月2日
 *      Author: xieliming
 */
#ifndef ANALYZE_H_
#define ANALYZE_H_
#include "DataS.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define PACKED __attribute__( ( packed, aligned(1) ) )
#pragma pack(1)

unsigned short PcrcFunc(unsigned char *pBuf, int length, int init_val);             //CRC16 计算
unsigned int  ObjectTypeFunc(unsigned short _p);                                    //帧类型判断
void DataFrameDealFunc(unsigned char * _p,int _length);
void ConfFrameDealFunc(unsigned char * _p,int _length);
void HeadFrameDealFunc(unsigned char * _p,int _length);
void FrameDealFunc(unsigned char * _p,int _length);                                 //数据区处理
void TcpAnalyzeFunc(void * _msg,int _len);
void UdpAnalyzeFunc(void * _msg,int _len);
bool CompareData(unsigned char *_a,unsigned char *_b,int n);

#endif /* ANALYZE_H_ */
