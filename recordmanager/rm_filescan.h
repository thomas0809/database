#ifndef RM_FILESCAN_H
#define RM_FILESCAN_H

#include "../fileio/FileManager.h"
#include "../utils/pagedef.h"
#include "../common/common.h"
#include "RecordManager.h"
#include "PageHead.h"
#include <cstring>
#include <iostream>


enum ClientHint {
    NO_HINT
};

class RM_FileScan {
	FileManager* myFileManager;
	BufPageManager* bpm;
	int recordSize;
	int pageNumber;
	
	int fileID;
	AttrType attrType;
	int attrLength;
	int attrOffset;
	CompOp compOp;
	void* value;

	int currentPage;
	int currentRecord;

	bool condINT(int v1, int v2){
		switch(compOp) {
			case EQ_OP: return v1 == v2;
			case LT_OP: return v1 < v2;
			case GT_OP: return v1 > v2;
			case LE_OP: return v1 <= v2;
			case GE_OP: return v1 >= v2;
			case NE_OP: return v1 != v2;
			default: break;
		}
		return false;
	}

	bool condFLOAT(float v1, float v2){
		switch (compOp){
			case EQ_OP: return v1 == v2;
			case LT_OP: return v1 < v2;
			case GT_OP: return v1 > v2;
			case LE_OP: return v1 <= v2;
			case GE_OP: return v1 >= v2;
			case NE_OP: return v1 != v2;
			default: break;
		}
		return false;
	}

	bool CondSTRING(char* v1, char* v2){
		switch(compOp) {
			case EQ_OP: return strcmp(v1, v2) == 0;
			case LT_OP: return strcmp(v1, v2) < 0;
			case GT_OP: return strcmp(v1, v2) > 0;
			case LE_OP: return strcmp(v1, v2) <= 0;;
			case GE_OP: return strcmp(v1, v2) >= 0;
			case NE_OP: return strcmp(v1, v2) != 0;
			case NO_OP: return true;
			default: break;
		}
		return false;
	}
	bool findRecord(BufType b){
		int totalRecord = 8088 >> 5;
		PageHead* pagehead = (PageHead*)(b);

		for(; currentRecord < totalRecord; currentRecord++){
			bool isRecord = pagehead->getRecordHead(currentRecord);
			if(isRecord){
				if(value == NULL)
					return true;

				if(attrType == MyINT){
					int offset = (96 + currentRecord * recordSize + attrOffset) / 4;
					int v1 = b[offset];
					int v2 = *((int*)value);
					if(condINT(v1, v2)) return true;
				}
				if(attrType == FLOAT){
					int offset = (96 + currentRecord * recordSize + attrOffset) / 4;
					float *v = (float*)b;
					float v1 = v[offset];
					float v2 = *((float*)value);
					if(condFLOAT(v1, v2)) return true;
				}
				if(attrType == STRING){
					char* v1 = new char[attrLength + 1];
					char* v2 = new char[attrLength + 1];
					int offset = (96 + currentRecord * recordSize + attrOffset) / 4;
					memcpy(v1, b + offset, attrLength);
					memcpy(v2, value, attrLength);
					v1[attrLength] = '\0';
//					char* v2 = (char*)value;
//					cout << "he" << endl;
					v2[attrLength] = '\0';
//					cout << "he" << endl;
//					cout << v1 << ' ' << v2 << endl;
					if(CondSTRING(v1, v2)) return true;
				}
			}
		}
		return false;
	}
public:
	RM_FileScan(FileManager *pfm, BufPageManager* bpm){
		myFileManager = pfm;
		this->bpm = bpm;
	}                           // Constructor
	~RM_FileScan(){
		myFileManager = NULL;
		bpm = NULL;
	}											// Destructor
    int OpenScan(const RM_FileHandle &fileHandle,  // Initialize file scan
                AttrType attrType,
                int attrLength,
                int attrOffset,
                CompOp compOp,
                void *value,
                ClientHint pinHint = NO_HINT){
    	int index;
		this->fileID = fileHandle.getFileID();
//		cout << "in OpenScan " << fileID << endl;
		if (bpm == NULL)
			cout << "in RM_FileScan buffermannager is NULL" <<endl;
    	BufType b = bpm->getPage(fileID, 0, index);
    	recordSize = b[0];
    	pageNumber = b[1];
    	this->fileID = fileHandle.getFileID();
    	this->attrType = attrType;
    	this->attrLength = attrLength;
    	this->attrOffset = attrOffset;
    	this->compOp = compOp;
    	this->value = value;
    	currentPage = 1;
    	currentRecord = 0;
    	if(attrType < MyINT || attrType > STRING || compOp < EQ_OP || compOp > NO_OP) 
    		return 0;
    	if((attrType == MyINT && attrLength != 4) || (attrType == FLOAT && attrLength != 4) || (attrType == STRING && attrLength < 0))
    		return 0;
    	if((attrOffset + attrLength) > recordSize)
    		return 0;
    	return 1;
    }

    int GetNextRec(RM_Record &rec){	 // Get next matching record
    	for(; currentPage <= pageNumber; currentPage++){
    		int index;
    		BufType b = bpm->getPage(fileID, currentPage, index);
//			cout << "hi" << endl;
    		if(findRecord(b)){
//				cout << "ha" << endl;
    			RID rid(currentPage, currentRecord);
    			//char* pdata = (char*)(b + 24 + (currentRecord * recordSize) / 4);
    			char* pdata = (char*)b + 96 + currentRecord * recordSize;
    			rec.Set(pdata, recordSize, rid);
    			return currentRecord++;
    		}
//			cout << "ho" << endl;
    		currentRecord = 0;
    	}
    	return -1;
    }
    int CloseScan() {return 0;}                               // Terminate file scan
};
#endif
