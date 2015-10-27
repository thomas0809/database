/*
 * rm.h
 *
 *  Created on: 2015年10月26日
 *      Author: QianBenY
 */

#ifndef RM_H_
#define RM_H_

#include "../fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
#include "../utils/pagedef.h"
#include <cstring>
#include <iostream>

//
// RID: Record id interface
//
class RID {
public:
	static const int NULL_PAGE = -1;
	static const int NULL_SLOT = -1;
	RID() : page(NULL_PAGE), slot(NULL_SLOT) {}     // Default constructor
	RID(int pageNum, int slotNum) : page(pageNum), slot(slotNum) {}
	~RID(){}                                        // Destructor

	int GetPageNum(int &pageNum) const          // Return page number
	{ pageNum = page; return 0; }
	int GetSlotNum(int &slotNum) const         // Return slot number
	{ slotNum = slot; return 0; }

	int Page() const          // Return page number
	{ return page; }
	int Slot() const          // Return slot number
	{ return slot; }

	bool operator==(const RID & rhs) const
	{
		int p;
		int s;
		rhs.GetPageNum(p);
		rhs.GetSlotNum(s);
		return (p == page && s == slot);
	}

private:
	int page;
	int slot;
};

inline ostream& operator <<(ostream & os, const RID& r)
{
	int p, s;
	r.GetPageNum(p);
	r.GetSlotNum(s);
	os << "[" << p << "," << s << "]";
	return os;
}

//
// RM_Record: RM Record interface
//
class RM_Record {
public:
    RM_Record ();
    ~RM_Record();

    // Return the data corresponding to the record.  Sets *pData to the
    // record contents.
    int GetData(char *&pData) const;

    int Set(char *pData, int size, RID rid_);

    // Return the RID associated with the record
    int GetRid (RID &rid) const;
//private:
    int recordSize;
    char *data;
    RID rid;
};

class FileHead{			//可以继续加
public:
	FileHead(int recordSize){
		int pageHead = 96;
		int emptyHead = 12;
		int byteNumber = pageHead - emptyHead;
		int bitPerByte = 8;
		int bitPerPage = 8196;
		this->recordSize = recordSize;
		this->pageNumber = 0;
		if (recordSize * byteNumber * bitPerByte < (bitPerPage - pageHead))
			this->recordPerPage = byteNumber * bitPerByte;
		else
			this->recordPerPage = (bitPerPage - pageHead) / recordSize;
		this->recordNumber = 0;
	}
	int recordSize;		//记录长度
	int pageNumber;		//页的个数
	int recordPerPage;	//每页记录个数
	int	recordNumber;	//记录的总数
};

//
// RM_FileHandle: RM File interface
//
class RM_FileHandle {
public:
    RM_FileHandle() : bpm(NULL), fileID(-1) {}
    RM_FileHandle(BufPageManager *bpm_, int fileID_)
    {
    	Open(bpm_, fileID_);
    }
    ~RM_FileHandle() {}

    void Open(BufPageManager *bpm_, int fileID_)
    {
    	bpm = bpm_;
    	fileID = fileID_;
    	int index;
    	fileHead = (FileHead*)(bpm->getPage(fileID, 0, index));
    	recordSize = fileHead->recordSize;
    	recordPerPage = fileHead->recordPerPage;
    	bpm->access(index);
    }

    int getFileID() const
    {
    	return fileID;
    }

    // Given a RID, return the record
    int GetRec     (const RID &rid, RM_Record &rec) const;

    int InsertRec  (const char *pData, RID &rid);       // Insert a new record

    int DeleteRec  (const RID &rid);                    // Delete a record
    int UpdateRec  (const RM_Record &rec);              // Update a record

private:
    BufPageManager *bpm;
    int fileID;
    FileHead *fileHead;
    int recordSize;
    int recordPerPage;
};

class PFS{
public:
	PFS(){
		pageNumber = 1;
		for (int i = 0; i < 8088; i++)
			used[i] = 0;
	}
	~PFS();
	int pageNumber;		//使用的页数
	char head[100];		//随意吧
	char used[8088];	//0--unused, 1--used
};

class RM_Manager {
	FileManager* myFileManager;
	BufPageManager* bpm;
public:
    RM_Manager    (FileManager *pfm, BufPageManager* bpm){
    	myFileManager = pfm;
    	this->bpm = bpm;
    }
    ~RM_Manager   ();

    int CreateFile (const char *fileName, int recordSize){
    	int index;
    	int fileID;
    	int pageID = 0;
    	if (myFileManager == NULL)
    		cout << "file not found" << endl;
    	myFileManager->createFile(fileName);
    	myFileManager->openFile("testfile.txt", fileID);
    	BufType b = bpm->allocPage(fileID, pageID, index, false);
    	bpm->markDirty(index);
    	cout << "log: recordSize " << recordSize <<endl;

    	FileHead *newPage = new FileHead(recordSize);
    	memcpy(b, newPage, sizeof(FileHead));
    	cout << "log: after memcpy" << b[0] << endl;

    	//PFS *pfs = new PFS();
    	//BufType b1 = bpm->allocPage(fileID, pageID + 1, index, false);
    	//memcpy(b1, pfs, sizeof(PFS));
    	b = bpm->getPage(fileID, pageID, index);
    	cout << "log: test page head" << b[0] << endl;
    	myFileManager->closeFile(fileID);
    	return 0;
    }
    int DestroyFile(const char *fileName){
    	myFileManager->destroyFile(fileName);
        return 0;
    }
    int OpenFile   (const char *fileName, RM_FileHandle &fileHandle){
    	int fileID;
    	myFileManager->openFile(fileName, fileID);
    	fileHandle.Open(bpm, fileID);
        return 0;
    }

    int CloseFile  (RM_FileHandle &fileHandle){
    	int fileID = fileHandle.getFileID();
    	bpm->close();
    	myFileManager->closeFile(fileID);
        return 0;
    }
};



#endif /* RECORDMANAGER_RECORDMANAGER_H_ */
