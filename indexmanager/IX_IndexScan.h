/*
 * IX_Index.h
 *
 *  Created on: Dec 23, 2015
 *      Author: thomas
 */

#ifndef INDEXMANAGER_IX_INDEXSCAN_H_
#define INDEXMANAGER_IX_INDEXSCAN_H_

#include "../common/common.h"
#include "IX_IndexHandle.h"
#include <cstdio>

class IX_IndexScan
{
  public:
       IX_IndexScan  () : data(NULL) {}                  // Constructor
       ~IX_IndexScan () {}                               // Destructor
    int OpenScan      (IX_IndexHandle &indexHandle, // Initialize index scan
                      CompOp      compOp,
                      void        *value,
                      ClientHint  pinHint = NO_HINT);
    int GetNextEntry  (RID &rid);                         // Get next matching entry
    int CloseScan     ();                                 // Terminate index scan
  private:
    CompOp    myCompOp;
    int       attrType;
    int       attrLength;
    char      *data;
    IX_IndexHandle *ixhandle;
    RID       indexID;
    void copyData(void* pData);
    int compareData(char* b);
};

void IX_IndexScan::copyData(void *pData)
{
	if (data != NULL)
		delete data;
	data = new char[attrLength + 5];
	memcpy(data, pData, attrLength);
	data[attrLength] = 0;
}

int IX_IndexScan::OpenScan(IX_IndexHandle &indexHandle,
                      	   CompOp      compOp,
						   void        *value,
						   ClientHint  pinHint)
{
	myCompOp = compOp;
	attrType = indexHandle.getAttrType();
	attrLength = indexHandle.getAttrLength();
	copyData(value);
	ixhandle = &indexHandle;
	switch (compOp)
	{
		case EQ_OP:
		case GE_OP:
		{
			ixhandle->GetLowerBound(data, indexID);
			break;
		}
		case GT_OP:
		{
			ixhandle->GetUpperBound(data, indexID);
			break;
		}
		default:
		{
			ixhandle->GetFirst(indexID);
			break;
		}
	}
	return 0;
}

int IX_IndexScan::GetNextEntry(RID &rid)
{
	while (true)
	{
		if (ixhandle->GetRID(indexID, rid) == -1)
			return -1;
		char *k = ixhandle->GetKey(indexID);
		int res = compareData(k);
		if (myCompOp == EQ_OP && res != 0)
			return -1;
		if (myCompOp == LT_OP && res != -1)
			return -1;
		if (myCompOp == LE_OP && res == 1)
			return -1;
		if (myCompOp == NE_OP && res == 0)
		{
			RID newid;
			if (ixhandle->GetNextIndexID(indexID, newid) == -1)
				return -1;
			indexID = newid;
			continue;
		}
		RID newid;
		ixhandle->GetNextIndexID(indexID, newid);
		indexID = newid;
		return 0;
	}
}

int IX_IndexScan::compareData(char* b)
{
	if (attrType == MyINT)
	{
		int x = *((int*)b);
		int dataInt = *((int*)data);
		if (x < dataInt)
			return -1;
		if (x == dataInt)
			return 0;
		return 1;
	}
	if (attrType == FLOAT)
	{
		float x = *((float*)b);
		float dataFloat = *((float*)data);
		if (x < dataFloat)
			return -1;
		if (x == dataFloat)
			return 0;
		return 1;
	}
	char *buf = new char[attrLength + 5];
	memcpy(buf, b, attrLength);
	buf[attrLength] = 0;
	int ans = strcmp(buf, data);
	if (ans < 0) ans = -1;
	if (ans > 0) ans = 1;
	delete buf;
	return ans;
}


#endif /* INDEXMANAGER_IX_INDEXSCAN_H_ */
