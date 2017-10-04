/*
 * DataS.c
 *
 *  Created on: 2017年4月4日
 *      Author: xieliming
 */
#include "DataS.h"
DataSet::DataSet()
{
	memset(this->DataQueue,0,sizeof(this->DataQueue));
	this->insertIndex=DATA_CNT-1;
	this->getIndex=DATA_CNT-1;
	this->getIndex_p=DATA_CNT-1;
	this->r=false;
	this->r_p=false;
}
DataSet::~DataSet()
{

}
int DataSet::getInsertIndex()const
{
	return this->insertIndex;
}
int DataSet::getGetIndex()const
{
	return this->getIndex;
}
void DataSet::insertDataNode(void * arg,unsigned int n )
{
	struct DataNode _dn;
	memcpy(_dn.ram,(unsigned char*)arg,n);
	if(this->insertIndex<0)
	{
		this->insertIndex=DATA_CNT-1;
		this->r=true;
		this->r_p=true;
	}
	this->getIndex=this->insertIndex;                     //real-time
	this->DataQueue[this->insertIndex--]=_dn;
}

void * DataSet::getDataNode()
{
	if(this->r==false&&(this->getIndex<=this->insertIndex))
	{
		return (void *)NULL;
	}
	if(this->getIndex<0)
	{
		this->getIndex=DATA_CNT-1;
		this->r=false;
	}
//	if(this->getIndex==DATA_CNT-1&&this->insertIndex!=DATA_CNT)
//	{
//		return (void *)NULL;
//	}
	return this->DataQueue[this->getIndex--].ram;
}
void* DataSet::getDataNode_p()
{
	if(this->r_p==false&&(this->getIndex_p<=this->insertIndex))
	{
		return (void *)NULL;       //NULL
	}
	if(this->getIndex_p<0)
	{
		this->getIndex_p=DATA_CNT-1;
		this->r_p=false;
	}
	return this->DataQueue[this->getIndex_p--].ram;
}
Operate::Operate()
{

}
Operate::~Operate()
{

}
void Operate::InsertFrequency1(void * arg,unsigned int n)
{
	this->FrequencyList1.insertDataNode(arg,n);
}
void * Operate::GetFrequency1()
{
	return this->FrequencyList1.getDataNode();
}
void Operate::InsertFrequency2(void * arg,unsigned int n)
{
	this->FrequencyList2.insertDataNode(arg,n);
}
void * Operate::GetFrequency2()
{
	return this->FrequencyList2.getDataNode();
}

void Operate::InsertActive1(void * arg,unsigned int n)
{
	this->ActivePowerList1.insertDataNode(arg,n);
}
void * Operate::GetActive1()
{
	return this->ActivePowerList1.getDataNode();
}
void Operate::InsertActive2(void * arg,unsigned int n)
{
	this->ActivePowerList2.insertDataNode(arg,n);
}
void * Operate::GetActive2()
{
	return this->ActivePowerList2.getDataNode();
}

void Operate::InsertReActive1(void * arg,unsigned int n)
{
	this->ReActivePowerList1.insertDataNode(arg,n);
}
void * Operate::GetReActive1()
{
	return this->ReActivePowerList1.getDataNode();
}
void Operate::InsertReActive2(void * arg,unsigned int n)
{
	this->ReActivePowerList2.insertDataNode(arg,n);
}
void * Operate::GetReActive2()
{
	return this->ReActivePowerList2.getDataNode();
}






//////////////////////////////////////////////////////////////
void Operate::InsertStat(void * arg,unsigned int n)
{
	this->StatList.insertDataNode(arg,n);
}
void * Operate::GetStat()
{
	return this->StatList.getDataNode();
}

void Operate::InsertPhunit(void * arg,unsigned int n)
{
	this->PhunitList.insertDataNode(arg,n);
}
void * Operate::GetPhunit()
{
	return this->PhunitList.getDataNode();
}
void Operate::InsertAnuit(void * arg,unsigned int n)
{
	this->AnuitList.insertDataNode(arg,n);
}
void * Operate::GetAnuit()
{
	return this->AnuitList.getDataNode();
}
//////////////////////////////////////////////////////
void* Operate::GetFrequency_p1()
{
	return this->FrequencyList1.getDataNode();
}
void* Operate::GetFrequency_p2()
{
	return this->FrequencyList2.getDataNode();
}
void* Operate::GetActive_p1()
{
	return this->ActivePowerList1.getDataNode_p();
}
void* Operate::GetActive_p2()
{
	return this->ActivePowerList2.getDataNode_p();
}
void* Operate::GetReActive_p1()
{
	return this->ReActivePowerList1.getDataNode_p();
}
void* Operate::GetReActive_p2()
{
	return this->ReActivePowerList2.getDataNode_p();
}




void* Operate::GetStat_p()
{
	return this->StatList.getDataNode_p();
}
void* Operate::GetPhunit_p()
{
	return this->PhunitList.getDataNode_p();
}
void* Operate::GetAnuit_p()
{
	return this->AnuitList.getDataNode_p();
}
//////////////////////////////////////////////////////////
int Operate::PhuintCalch(unsigned int t)                 //calc phuint's hight byte
{
	unsigned char * ch=(unsigned char*)&t;
	ch=ch+3;
	return *(unsigned char *)ch;
}
int Operate::PhuintAndAnuintCalcl(unsigned int t)        //calc phuint's or anuint's hight byte
{
	int sum=0;
	for(int i=23;i>=0;--i)
	{
		if(t&(1<<i))
		{
			sum+=pow(2,i);
		}
	}
	return sum;
}
void Operate::showStat(unsigned short &a)
{
	for(int i=15;i>=0;--i)
	{
		if(i==23||i==15||i==7)
		{
			printf(" ");
		}
		if(a&(1<<i))
		{
			printf("1");
		}
		else
		{
			printf("0");
		}
	}
	printf("\n");
}
