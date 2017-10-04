/*
 * DataS.h
 *
 *  Created on: 2017年4月4日
 *      Author: xieliming
 */

#ifndef DATAS_H_
#define DATAS_H_
#define BUF_SIZE 50*1024
#define DATA_CNT 100
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#define PACKED __attribute__( ( packed, aligned(1) ) )
#pragma pack(1)

typedef struct                                        //帧的头部结构
{
	unsigned short sync;
	unsigned short framesize;                         //帧字节数
	unsigned int soc;
}FrameHead;

typedef struct
{
	unsigned short sync;
	unsigned short framesize;
	unsigned int soc;
	unsigned short  d_frame;
	unsigned int meas_rate;
	unsigned short num_pmu;                          //######################
}ConfFrameHead;

typedef struct
{
	unsigned char stn[16];                           //站名
	unsigned char idcode[8];                         //PMU硬件标识
	unsigned short format;                           //数据帧的数据格式
	unsigned short phnmr;                            //相量的数量
	unsigned short annmr;                            //模拟量数量
	unsigned short dgnmr;                            //开关量状态字的数量
}ConfFrameSubData;

typedef struct
{
	unsigned short sync;
	unsigned short framesize;                         //帧字节数
	unsigned int soc;
	unsigned int fracsec;
}DataFrameHead;

struct Mrcnt
{
	unsigned int phnmr;
	unsigned int annmr;
	unsigned int dgnmr;
};
struct Mrset
{
	struct Mrcnt mcn[100];                            //Max Pmu num
	int index;
	Mrset()
	{
		memset(mcn,0,sizeof(mcn));
		index=0;
	}
};

struct typeinfo
{
	int number;                         //unit's serial number values=1 and 2
	int _type;                          //frame type OOP OOQ ODF
};
struct typeso
{
	struct typeinfo typelist[100];         //anmr number
	int _dindex;
	typeso()
	{
		memset(typelist,0,sizeof(typelist));
		_dindex=0;
	}
};
struct Typeset                             //pmu number
{
	struct typeso Tq[100];
	int _pmuindex;
	Typeset()
	{
		memset(&Tq,0,sizeof(Tq));
		this->_pmuindex=0;
	}
};

struct APname
{
	unsigned char cname[4];
	unsigned char fr;
	unsigned char devname[7];
	unsigned char frs;
	unsigned char dtype[3];
};
struct Dname
{
	unsigned char cname[4];
	unsigned char fr;
	unsigned char devname[11];
};

struct DataNode
{
	char ram[50];
};

class DataSet
{
private:
	struct DataNode DataQueue[DATA_CNT];
	int insertIndex;
	bool r;
	bool r_p;
	int getIndex;
	int getIndex_p;
public:
	DataSet();
	~DataSet();
	int getInsertIndex()const;
	int getGetIndex()const;
	void insertDataNode(void *arg,unsigned int n);
	void* getDataNode();
	void* getDataNode_p();
};
///////////////////////////////////////////////////////
class Operate
{
public:
	Operate();
	~Operate();
	void InsertFrequency1(void *arg,unsigned int n);
	void* GetFrequency1();

	void InsertFrequency2(void *arg,unsigned int n);
	void* GetFrequency2();

	void InsertActive1(void *arg,unsigned int n);
	void* GetActive1();
	void InsertActive2(void *arg,unsigned int n);
	void* GetActive2();

	void InsertReActive1(void *arg,unsigned int n);
	void* GetReActive1();
	void InsertReActive2(void *arg,unsigned int n);
	void* GetReActive2();



	void InsertStat(void *arg,unsigned int n);
	void* GetStat();

	void InsertPhunit(void *arg,unsigned int n);
	void* GetPhunit();

	void InsertAnuit(void *arg,unsigned int n);
	void* GetAnuit();
	/////////////////////////////////////////////////////
	void* GetFrequency_p1();
	void* GetActive_p1();
	void* GetReActive_p1();

	void* GetFrequency_p2();
	void* GetActive_p2();
	void* GetReActive_p2();

	void* GetStat_p();
	void showStat(unsigned short &a);
	void* GetPhunit_p();
	void* GetAnuit_p();
	int PhuintCalch(unsigned int t);
	int PhuintAndAnuintCalcl(unsigned int t);
private:
	DataSet FrequencyList1;
	DataSet ActivePowerList1;
	DataSet ReActivePowerList1;
	DataSet FrequencyList2;
	DataSet ActivePowerList2;
	DataSet ReActivePowerList2;

	DataSet StatList;
	DataSet PhunitList;
	DataSet AnuitList;
};
#endif               /* DATAS_H_ */
