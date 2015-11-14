#ifndef SM_MANAGER_H
#define SM_MANAGER_H

#include "iostream"
#include <cstring>
#include <stdlib.h>
#include "../recordManager/RecordManager.h"
#include "../recordManager/rm_filescan.h"
#include <set>

#define LOG_TAG "SM_Manager"
#define dbName_Null "dbName is Null"
#define dbName_opened "dbName is Opened"
#define dbName_closed "dbName is Closed"
#define dbName_not_exist "dbName not exist"
#define dbName_not_right "dbName is illegal"
#define FAIL -1
#define MAXNAME 11

using namespace std;


// Used by SM_Manager::CreateTable
struct AttrInfo {
   char     *attrName;           // Attribute name
   AttrType attrType;            // Type of attribute
   int      attrLength;          // Length of attribute
   AttrInfo(){
	   attrName = NULL;
	   attrType = MyINT;
	   attrLength = 0;
   }
};

// Used by Printer class
struct DataAttrInfo {
   int      offset;              						// Offset of attribute 
   int      attrLength;         						 // Length of attribute
   int      indexNo;             						 // Attribute index number
   AttrType attrType;            						// Type of attribute 
   char     relName[MAXNAME+1];  						// Relation name
   char     attrName[MAXNAME+1]; 						// Attribute name
   void 	setAttributes(const AttrInfo &newAttrInfo);		 //����
   void 	print();
   DataAttrInfo(){
	   memset(relName, 0, MAXNAME+1);
	   memset(attrName, 0, MAXNAME+1);
	   offset = 0;
	   attrType = MyINT;
	   attrLength = 0;
	   indexNo = 0;
   }
};

struct DataRelInfo {
	int		relLength;
	char	relName[MAXNAME+1];
	DataRelInfo(){
		relLength = 0;
		memset(relName, 0, MAXNAME+1);
   }
};

void DataAttrInfo::setAttributes(const AttrInfo &newAttrInfo){
	memset(relName, 0, MAXNAME + 1);
	memset(attrName, 0, MAXNAME + 1);
	strcpy(this->attrName, newAttrInfo.attrName);
	this->attrType = newAttrInfo.attrType;
	this->attrLength = newAttrInfo.attrLength;
}

void DataAttrInfo::print(){
//	cout << "start print" << endl;
	cout << "TableName: " << relName << ",attrName: " << attrName <<",attrLength: " << attrLength <<endl;
}

class Log{
	public:
		void LogDebug(const char* a, const char* b);
		void LogDebug(const char* a);
};

class SM_Manager {
  public:
       SM_Manager  (RM_Manager &);  					// ���캯��
       ~SM_Manager ();                                  // ��������
    int OpenDb      (const char *);                // ��Ŀ¼
    int CloseDb     ();                                  // �ر�Ŀ¼
    int CreateTable (const char *relName,                // ������
                    int        attrCount,
                    AttrInfo   *attributes);
    int DropTable   (const char *deleteRelName);               // ɾ����
	int ShowTable	(const char *relName);				 // ��ӡ�����Ϣ
 private:
	RM_Manager& rmm;
	RM_FileHandle attrfh;
	RM_FileHandle relfh;
	bool IsOpenDB;
	Log* myLog;
};


void Log::LogDebug(const char* a, const char* b){
	cout << a << ' ' << b << endl;
}

void Log::LogDebug(const char* a){
	cout << LOG_TAG << ' ' << a << endl;
}

SM_Manager::SM_Manager(RM_Manager &_rmm):rmm(_rmm){
	IsOpenDB = false;
	myLog = new Log();
}

SM_Manager::~SM_Manager(){
	
}

int SM_Manager::OpenDb (const char *dbName){
	if (dbName == NULL){
		myLog->LogDebug(LOG_TAG, dbName_Null);
		return FAIL;
	}
	if (IsOpenDB == true){
		myLog->LogDebug(LOG_TAG, dbName_opened);
		return FAIL;
	}
	if (chdir(dbName) < 0){
		myLog->LogDebug(dbName_not_exist);
		return FAIL;
	}
	rmm.OpenFile("attrcat", attrfh);
	cout << attrfh.getFileID() << endl;
//	rmm.OpenFile("relcat", relfh);
//	cout << relfh.getFileID() << endl;
	IsOpenDB = true;
	return 0;
}

int SM_Manager::CloseDb(){
	if (IsOpenDB == false){
		myLog->LogDebug(dbName_closed);
		return FAIL;
	}
	rmm.CloseFile(attrfh);
	chdir("..");
//	rmm.CloseFile(relfh);
	IsOpenDB = false;
	return 0;
}

int SM_Manager::CreateTable (const char *relName,                // ������, ������Ҫ���attribute����û���ظ�������
                    int        attrCount,
					AttrInfo   *attributes){
	if (strcmp(relName, "relcat") == 0 || strcmp(relName, "attrcat") == 0) {
		myLog->LogDebug(dbName_not_right);
		return FAIL;
	}	
	int fileID = attrfh.getFileID();
//	cout << "fileID " << fileID << endl;
	DataAttrInfo* d = new DataAttrInfo[attrCount];
	for (int i = 0; i < attrCount; i++)
		d[i] = DataAttrInfo();
	int size = 0;
	int returnCode = 0;
	RID rid;
	for (int i = 0; i < attrCount; i++) {
		d[i].setAttributes(attributes[i]);
		d[i].offset = size;
		size += attributes[i].attrLength;
		strcpy (d[i].relName, relName);
		returnCode = attrfh.InsertRec((char*) &d[i], rid);
	}
/*	for (int pageID = 0; pageID < 2; ++ pageID) {
        int index;
        //ΪpageID��ȡһ������ҳ
        BufType b = rmm.getBufPageManager()->getPage(fileID, pageID, index);
        cout << b[0] << "---";
		int i = 24;
		for (int j = 0; j < 3; j++){
			for (int k = 0; k < 10; k++){
				cout << b[i + j*10 + k] << ' ';
			}
			cout << "---";
		}
		cout << endl;
		rmm.getBufPageManager()->access(index); //��Ƿ���
    }*/
/*	DataRelInfo rel;
	strcpy(rel.relName, relName);
	rel.relLength = size;
	relfh.InsertRec((char*) &rel, rid);*/
	delete []d;
	return 0;
}

int SM_Manager::DropTable (const char * deleteRelName){
	RM_FileScan rfs = RM_FileScan(rmm.getFileManager(), rmm.getBufPageManager());
	RM_Record rec;
	//RM_FileHandle attrfh;
	//RM_FileHandle relfh;
	//��rel�ļ�����ѯ����relName���� = deleteRelName�ļ�¼��ɾ���ü�¼
/*	rfs.OpenScan(relfh, STRING, strlen(deleteRelName), 4, EQ_OP, (void*)deleteRelName);
	while (rfs.GetNextRec(rec) != -1){
		relfh.DeleteRec(rec.rid);
	}
	rfs.CloseScan();*/
	//��attr�ļ�����ѯ����relName���� = deleteRelName�ļ�¼��ɾ���ü�¼
	rfs.OpenScan(attrfh, STRING, strlen(deleteRelName), 16, EQ_OP, (void*)deleteRelName);
	while (rfs.GetNextRec(rec) != -1){
		attrfh.DeleteRec(rec.rid);
	}
	rfs.CloseScan();
	return 0;
}

int SM_Manager::ShowTable (const char *readRelName){
	//��attr�ļ�����ѯ����relName���� = readRelName�ļ�¼�� ��ӡ�ü�¼
	RM_FileScan rfs = RM_FileScan(rmm.getFileManager(), rmm.getBufPageManager());	
 	RM_Record rec;
	int returnCode;
//	cout << "ShowTable" << endl;
	returnCode = rfs.OpenScan(attrfh, STRING, strlen(readRelName), 16, EQ_OP, (void*)readRelName);
	int x = 0;
//	cout << "TableName : "<< readRelName << endl;
	while (returnCode == 1){
//		cout << "ret : 1" << endl;
		x = rfs.GetNextRec(rec);
//		cout << "ret : 2" << endl;
		if (x == -1)
			break;
//		cout << x << endl;
		DataAttrInfo * mydata;
		rec.GetData((char*&) mydata);
		mydata->print();
	}
//	cout << "Table End" << endl;
	rfs.CloseScan();
	return 0;
}

#endif

