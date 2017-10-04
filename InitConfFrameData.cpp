/*
 * InitConfFrameData.cpp
 *
 *  Created on: Apr 17, 2017
 *      Author: xieliming
 */
#include "InitConfFrameData.h"
extern Operate Opts;
extern Mrset Mrst;
extern Typeset Tyst;
ConfObject::ConfObject()
{
	this->filename=(char*)"./Conf.ini";
	this->pmu_num=0;
}
ConfObject::~ConfObject()
{

}
bool ConfObject::ReadLocalConfigFile()
{
	int s;
	s=GetPrivateProfileInt("PMUNUM","PMU",-1,filename);
	if(s==-1)
	{
		return false;
	}
	pmu_num=s;
	char KeyBuf[128]={0};
	char FieldBuf[128]={0};
	for(int i=0;i<pmu_num;++i)
	{
		memset(KeyBuf,0,sizeof(KeyBuf));
		sprintf(KeyBuf,"PMU%d",i);
		s=GetPrivateProfileInt(KeyBuf,"PHMR",-1,filename);
		if(s!=-1)
		{
			Mrst.mcn[Mrst.index].phnmr=s;
		}
		s=GetPrivateProfileInt(KeyBuf,"ANMR",-1,filename);
		if(s!=-1)
		{
			Mrst.mcn[Mrst.index].annmr=s;
		}
		s=GetPrivateProfileInt(KeyBuf,"DGMR",-1,filename);
		if(s!=-1)
		{
			Mrst.mcn[Mrst.index].dgnmr=s;
		}
		++Mrst.index;
	}
	///////////////////////////////////////////////////////
	char cres[128]={0};
	int clen=sizeof(cres);
	char cdefault[128]={0};
	for(int i=0;i<pmu_num;++i)
	{
		memset(KeyBuf,0,sizeof(KeyBuf));
		sprintf(KeyBuf,"PMU%d_NAME",i);
		for(unsigned int j=0;j < Mrst.mcn[i].annmr; ++j)
		{
			memset(FieldBuf,0,sizeof(FieldBuf));
			sprintf(FieldBuf,"ANNAME%d",j);
			memset(cres,0,sizeof(cres));
			s=GetPrivateProfileString((const char*)KeyBuf,(const char*)FieldBuf,(char*)cdefault,(char*)cres,clen,filename );
			if(s==0)
			{
				//failed
			}
			else
			{
				Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex].number=atoi((const char*)cres);
				if(CompareData((unsigned char *)(cres+2),(unsigned char *)"OOP",3))
				{
					Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex]._type=2;
				}
				if(CompareData((unsigned char *)(cres+2),(unsigned char *)"OOQ",3))
				{
					Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex]._type=3;
				}
				if(CompareData((unsigned char *)(cres+2),(unsigned char *)"ODF",3))
				{
					Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex]._type=1;
				}
			}
			++Tyst.Tq[Tyst._pmuindex]._dindex;
		}
		++Tyst._pmuindex;
	}
	this->showLocalConfigFile();
	return true;
}
void ConfObject::showLocalConfigFile()
{
	printf("pmu_num=:%d\n",pmu_num);
	for(int i=0;i<pmu_num;++i)
	{
		printf("phmr%d=:%d\n",i,Mrst.mcn[i].phnmr);
		printf("anmr%d=:%d\n",i,Mrst.mcn[i].annmr);
		printf("dgmr%d=:%d\n",i,Mrst.mcn[i].dgnmr);
	}
	for(int i=0;i<pmu_num;++i)
	{
		for(int j=0;j<Mrst.mcn[i].annmr;++j)
		{
			printf("%d number=:%d type=:%d\n",i,Tyst.Tq[i].typelist[j].number,Tyst.Tq[i].typelist[j]._type);
		}
	}
}
int  ConfObject::GetPmuNUM()const
{
	return this->pmu_num;
}
