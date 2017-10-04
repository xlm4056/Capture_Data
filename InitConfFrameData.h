/*
 * InitConfFrameData.h
 *
 *  Created on: Apr 17, 2017
 *      Author: xieliming
 */

#ifndef INITCONFFRAMEDATA_H_
#define INITCONFFRAMEDATA_H_
#include "conf.h"
#include "DataS.h"
#include "Analyze.h"
#define PACKED __attribute__( ( packed, aligned(1) ) )
#pragma pack(1)
class ConfObject
{
public:
	ConfObject();
	~ConfObject();
	bool ReadLocalConfigFile();
	int  GetPmuNUM()const;
	void showLocalConfigFile();
private:
	char * filename;
	int pmu_num;
};

#endif /* INITCONFFRAMEDATA_H_ */
