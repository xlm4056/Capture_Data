/*
 * Analyze.c
 *
 *  Created on: 2017年4月2日
 *      Author: xieliming
 */
#include "Analyze.h"
#include "BDs.h"

Operate Opts;
BroadOperation Bopts;                       //Broad Data object
struct Mrset Mrst;
struct Typeset Tyst;
unsigned int dfcnt=0;
unsigned int hfcnt=0;
unsigned int cf1cnt=0;
unsigned int cf2cnt=0;
bool debugflag=false;
bool ramdebugflag=false;

int pmu_num=-1;
unsigned short ratedfrequency=0;

char srcip[50]={0};
char desip[50]={0};
char srcmac[100]={0};
char desmac[100]={0};
int  srcport=0;
int  desport=0;

extern unsigned long uncompletecnt;
extern unsigned long errorpacketnum;

unsigned short PcrcFunc(unsigned char *pBuf, int length, int init_val)    //init_val default 0
{
	unsigned long Temp;
	unsigned long CRC_Code = init_val;
	int   i,j;
	for( i=0; i<length; i++)
    	{
		Temp  =  pBuf[i] << 8;
		CRC_Code  =  CRC_Code ^ Temp;
		for ( j=0; j<8; j++)
		{
			CRC_Code  =  CRC_Code & 0xFFFF;
			CRC_Code  =  CRC_Code << 1;
			if( CRC_Code > 0xFFFF)
			{
				CRC_Code = CRC_Code ^ 0x1021;
			}
		}
  	}
	CRC_Code = CRC_Code & 0xFFFF;
	return CRC_Code;
}
unsigned int  ObjectTypeFunc(unsigned short _p)       //1-数据帧000    2-头帧001   3-配置帧1 010    4-配置帧2  011          8～15     12-14
{
	unsigned char v='\0';
	int i;
	for(i=14;i>=12;--i)
	{
		if(_p&(1<<i))
		{
			v|=(1<<(i-12));
		}
	}
	return v;
}
bool CompareData(unsigned char *_a,unsigned char *_b,int n)
{
	for(int i=0;i<n;i++)
	{
		if(*_a++!=*_b++)
		{
			return false;
		}
	}
	return true;
}

void DataFrameDealFunc(unsigned char * _p,int _length)
{
	unsigned char * p=_p+sizeof(DataFrameHead);                         //1 fre  2  ac  3  reac
	for(int i=0;i<pmu_num;++i)
	{
		unsigned short _stat=*(unsigned short*)p;
		Opts.InsertStat((unsigned char*)&_stat,sizeof(_stat));
		p+=2;
		p+=4*Mrst.mcn[i].phnmr;
		p+=4;                                                          //FREQ AND DFREQ
		for(unsigned int k=0;k<Mrst.mcn[i].annmr;++k)
		{
			unsigned short _t=*(unsigned short*)p;
			switch(Tyst.Tq[i].typelist[k]._type)                        //1 fre  2  ac  3  reac
			{
				case -1:
					break;
				case 1:
				{
					if(Tyst.Tq[i].typelist[k].number==1)
					{
						Opts.InsertFrequency1((unsigned char*)&_t,sizeof(_t));
					}
					else if(Tyst.Tq[i].typelist[k].number==2)
					{
						Opts.InsertFrequency2((unsigned char*)&_t,sizeof(_t));
					}
					break;
				}
				case 2:
				{
					if(Tyst.Tq[i].typelist[k].number==1)
					{
						Opts.InsertActive1((unsigned char*)&_t,sizeof(_t));
					}
					else if(Tyst.Tq[i].typelist[k].number==2)
					{
						Opts.InsertActive2((unsigned char*)&_t,sizeof(_t));
					}
					break;
				}
				case 3:
				{
					if(Tyst.Tq[i].typelist[k].number==1)
					{
						Opts.InsertReActive1((unsigned char*)&_t,sizeof(_t));
					}
					else if(Tyst.Tq[i].typelist[k].number==2)
					{
						Opts.InsertReActive2((unsigned char*)&_t,sizeof(_t));
					}
					break;
				}
				default:
				{
					break;
				}
			}
			p+=2;
		}
		p+=2*Mrst.mcn[i].dgnmr;
	}
}

void ConfFrameDealFunc(unsigned char * _p,int _length)
{
	char * filename="./vxcu.ini";
	ConfFrameHead* cf=(ConfFrameHead* )_p;
	pmu_num=cf->num_pmu;
	if(!WritePrivateProfileInt("NUMMSG","NUM_PMU",cf->num_pmu,filename))          //
	{
		fprintf(stderr,"WritePrivateProfileint was failed:%s\n",strerror(errno));
	}

	if(debugflag)
	{
		printf("cf->num_pmu=:%d\n",cf->num_pmu);
	}

	unsigned char * p=_p+sizeof(ConfFrameHead);
	memset(&Mrst,0,sizeof(Mrst));                                                  //save phnmr annmr
	memset(&Tyst,0,sizeof(Tyst));                                                  //save types
	for(int i=0;i<pmu_num;++i)
	{
		ConfFrameSubData* cfs=(ConfFrameSubData*)p;
		Mrst.mcn[Mrst.index].phnmr=cfs->phnmr;
		Mrst.mcn[Mrst.index].annmr=cfs->annmr;
		Mrst.mcn[Mrst.index].dgnmr=cfs->dgnmr;
		unsigned char * sc=p+sizeof(ConfFrameSubData);

		for(unsigned int pi=0;pi<Mrst.mcn[Mrst.index].phnmr;++pi)
		{
//			struct APname* pn=(struct APname*)sc;


			sc+=sizeof(struct APname);
		}
		for(unsigned int ai=0;ai<Mrst.mcn[Mrst.index].annmr;++ai)
		{
			struct APname* an=(struct APname*)sc;
			if(debugflag)
			{

			}
			if(CompareData((unsigned char *)an->dtype,(unsigned char *)"OOP",sizeof(an->dtype)))           //ActivePower  2
			{
				Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex]._type=2;                 //current pmu's type
				if(strcmp((const char*)an->devname,"1#机")==0)
				{
					Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex].number=1;
				}
				else if(strcmp((const char*)an->devname,"2#机")==0)
				{
					Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex].number=2;
				}
				++Tyst.Tq[Tyst._pmuindex]._dindex;
			}
			if(CompareData((unsigned char *)an->dtype,(unsigned char *)"OOQ",sizeof(an->dtype)))           //ReActivePower   3
			{
				Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex]._type=3;
				if(strcmp((const char*)an->devname,"1#机")==0)
				{
					Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex].number=1;
				}
				else if(strcmp((const char*)an->devname,"2#机")==0)
				{
					Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex].number=2;
				}
				++Tyst.Tq[Tyst._pmuindex]._dindex;
			}
			if(CompareData((unsigned char *)an->dtype,(unsigned char *)"ODF",sizeof(an->dtype)))           //Frequency   1
			{
				Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex]._type=1;
				if(strcmp((const char*)an->devname,"1#机")==0)
				{
					Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex].number=1;
				}
				else if(strcmp((const char*)an->devname,"2#机")==0)
				{
					Tyst.Tq[Tyst._pmuindex].typelist[Tyst.Tq[Tyst._pmuindex]._dindex].number=2;
				}
				++Tyst.Tq[Tyst._pmuindex]._dindex;
			}
			sc+=sizeof(struct APname);
		}
		for(unsigned int di=0;di<(Mrst.mcn[Mrst.index].dgnmr*16);++di)
		{
//			struct Dname* dn=(struct Dname*)sc;


			sc+=sizeof(struct Dname);
		}

		for(unsigned int k=0;k<Mrst.mcn[Mrst.index].phnmr;++k)                             //################
		{
			unsigned int _ps=*(unsigned int *)sc;
			Opts.InsertPhunit((unsigned char*)&_ps,sizeof(_ps));
			sc+=4;
		}
		for(unsigned int k=0;k<Mrst.mcn[Mrst.index].annmr;++k)
		{
			unsigned int _ps=*(unsigned int *)sc;
			Opts.InsertAnuit((unsigned char*)&_ps,sizeof(_ps));
			sc+=4;
		}
		for(unsigned int k=0;k<Mrst.mcn[Mrst.index].dgnmr;++k)
		{
			unsigned int _ps=*(unsigned int *)sc;


			sc+=4;
		}
		p=sc+2;                                                                 //fnom
		++Tyst._pmuindex;                                                       //pmu ++
		++Mrst.index;
	}
}
void HeadFrameDealFunc(unsigned char * _p,int _length)
{

}
void FrameDealFunc(unsigned char * _p,int _length)                                                                   //数据区处理
{
	if(_length<sizeof(FrameHead))
	{
		//读到数据报不完整
		++uncompletecnt;
		return ;
	}
	FrameHead* fh=(FrameHead*)(_p);
	int _type= ObjectTypeFunc(fh->sync);
	int _size=(int)fh->framesize;
//	if(debugflag)
//	{
		printf("\n_length=:%d,_type=:%d _size=:%d\n",_length,_type,_size);
//	}
	if(_length<_size)
	{
		//读到数据报不完整
		++uncompletecnt;
		return ;
	}
	//进行CRC判断
	unsigned short crc=PcrcFunc(_p,(_size-2),0);
	unsigned short tcrc=*((unsigned short*)(_p+_size-2));
	if(debugflag)
	{
		printf("crc=:%d  tcrc=%d\n",crc,tcrc);
	}
	if(crc!=tcrc)
	{
		//数据包不正确
		++errorpacketnum;
		return ;
	}
	//开始处理数据帧
	switch(_type)
	{
		case 0:                                                  //数据帧
		{
			++dfcnt;
			printf("++++++Data Data Data Frame\n");
			if(pmu_num != -1)
			{
				DataFrameDealFunc((unsigned char*)_p,_size);
			}
			break;
		}
		case 1:                                                  //头帧
		{
			++hfcnt;
			printf("++++++Head Data Frame\n");
			HeadFrameDealFunc((unsigned char*)_p,_size);
			break;
		}
		case 2:                                                  //配置1
		{
			++cf1cnt;
			printf("+++++++Config 1 Frame\n");
			ConfFrameDealFunc((unsigned char*)_p,_size);
			break;
		}
		case 3:                                                  //配置2
		{
			++cf2cnt;
			printf("+++++++Config 2 Frame\n");
			ConfFrameDealFunc((unsigned char*)_p,_size);
			break;
		}
		default:                                                //不处理
		{
			break;
		}
	}
	if(_length>_size)
	{
		FrameDealFunc((unsigned char *)(_p+_size),(_length-_size));
	}
}
void TcpAnalyzeFunc(void * _msg,int _len)                                                               //len 读到的字节个数
{
	char * p=(char * )_msg;
	unsigned char* iphead=NULL,*ethhead=NULL;
	ethhead=(unsigned char *)p;
	iphead=(unsigned char *)(p+14);
	sprintf(srcip,"%d.%d.%d.%d",iphead[12],iphead[13],iphead[14],iphead[15]);
	sprintf(desip,"%d.%d.%d.%d",iphead[16],iphead[17],iphead[18],iphead[19]);
	sprintf(srcmac,"%02x:%02x:%02x:%02x:%02x:%02x",ethhead[0],ethhead[1],ethhead[2],ethhead[3],ethhead[4],ethhead[5]);
	sprintf(desmac,"%02x:%02x:%02x:%02x:%02x:%02x",ethhead[6],ethhead[7],ethhead[8],ethhead[9],ethhead[10],ethhead[11]);
	srcport=(iphead[20]<<8)+iphead[21];
	desport=(iphead[22]<<8)+iphead[23];
	//真正拿到数据帧进行处理
	int cnt=_len-66;
	if(cnt<sizeof(FrameHead))
	{
		//读到数据报不完整
		if(debugflag)
		{
			printf("uncomplete error len=:%d  cnt=:%d, sizeof(FrameHead)=:%d\n",_len,cnt,sizeof(FrameHead));
		}
		++uncompletecnt;
		return ;
	}
	FrameDealFunc((unsigned char *)(p+66),cnt);                                              //数据处理
///////////////////////////////////////////////////////////////////////////////////////////////原生模式数据处理
	if(ramdebugflag)
	{
		printf("cnt=:%d _len=:%d\n",cnt,_len);
		int i;
		for(i=0;i<cnt;++i)
		{
			if(i!=0&&i%9==0)
			{
				printf("\n");
			}
			printf("%c ",*(p+i+66));
		}
		printf("\n");
		fflush(stdout);
	}
////////////////////////////////////////////////////////////////////
}
void UdpAnalyzeFunc(void * _msg,int _len)
{

}
