#ifndef IX_MANAGER_H_
#define IX_MANAGER_H_

#include "../fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
#include "IX_IndexHandle.h"
#include "IX_IndexScan.h"
#include "IX.h"

#include <cstdio>
#include <cstring>
#include <string>
using namespace std;

class IX_Manager
{
  public:
    IX_Manager   (FileManager *pfm_, BufPageManager* bpm_)
  		: pfm(pfm_), bpm(bpm_) {}
    ~IX_Manager  ();
    int CreateIndex  (const char *fileName,          // Create new index
                     int        indexNo,
                     AttrType   attrType,
                     int        attrLength);
    int DestroyIndex (const char *fileName,          // Destroy index
                     int        indexNo);
    int OpenIndex    (const char *fileName,          // Open index
                     int        indexNo,
                     IX_IndexHandle &indexHandle);
    int CloseIndex   (IX_IndexHandle &indexHandle);  // Close index
  private:
    FileManager *pfm;
    BufPageManager *bpm;
};

int IX_Manager::CreateIndex(const char *fileName, int indexNo, AttrType attrType, int attrLength)
{
	int fileID;
	char buf[100];
	memset(buf, 0, sizeof buf);
	sprintf(buf, "%s.%d.index", fileName, indexNo);
	pfm->createFile(buf);
	pfm->openFile(buf, fileID);

	IX_FileHead filehead;
	filehead.attrType = attrType;
	filehead.attrLength = attrLength;
	filehead.maxN = 8000 / (attrLength + sizeof(RID)) - 2;
	filehead.pageNum = 1;
	filehead.firstEmptyPage = 1;
	filehead.root = -1;

	int index;
	char* b = (char*)bpm->allocPage(fileID, 0, index);
	memcpy(b, (char*)(&filehead), sizeof(IX_FileHead));
	bpm->markDirty(index);

	bpm->close();
	pfm->closeFile(fileID);

	return 0;
}

int IX_Manager::DestroyIndex(const char *fileName, int indexNo)
{
	char buf[100];
	memset(buf, 0, sizeof buf);
	sprintf(buf, "%s.%d.index", fileName, indexNo);
	pfm->destroyFile(buf);
	return 0;
}

int IX_Manager::OpenIndex(const char *fileName, int indexNo, IX_IndexHandle &indexHandle)
{
	int fileID;
	char buf[100];
	memset(buf, 0, sizeof buf);
	sprintf(buf, "%s.%d.index", fileName, indexNo);
	pfm->openFile(buf, fileID);
	indexHandle = IX_IndexHandle(bpm, fileID);
	return 0;
}

int IX_Manager::CloseIndex(IX_IndexHandle &indexHandle)
{
	return 0;
}

#endif
