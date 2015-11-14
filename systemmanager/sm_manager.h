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
#define MAXNAME 100

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
   char     relName[MAXNAME+1];  						// Relation name
   char     attrName[MAXNAME+1]; 						// Attribute name
   int      offset;              						// Offset of attribute 
   AttrType attrType;            						// Type of attribute 
   int      attrLength;         						 // Length of attribute
   int      indexNo;             						 // Attribute index number
   void 	setAttributes(const AttrInfo &newAttrInfo);		 //设置
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
	cout << this->attrName << ' ' << newAttrInfo.attrName << endl;
	strcpy(this->attrName, newAttrInfo.attrName);
	this->attrType = newAttrInfo.attrType;
	this->attrLength = newAttrInfo.attrLength;
}

void DataAttrInfo::print(){
	cout << relName << ' ' << attrLength <<endl;
}

class Log{
	public:
		void LogDebug(const char* a, const char* b);
		void LogDebug(const char* a);
};

class SM_Manager {
  public:
       SM_Manager  (RM_Manager &);  					// 构造函数
       ~SM_Manager ();                                  // 析构函数
    int OpenDb      (const char *);                // 打开目录
    int CloseDb     ();                                  // 关闭目录
    int CreateTable (const char *relName,                // 创建表
                    int        attrCount,
                    AttrInfo   *attributes);
    int DropTable   (const char *deleteRelName);               // 删除表
	int ShowTable	(const char *relName);				 // 打印表的信息
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
//	rmm.CloseFile(relfh);
	IsOpenDB = false;
	return 0;
}

int SM_Manager::CreateTable (const char *relName,                // 创建表, 这里需要检查attribute里面没有重复的名字
                    int        attrCount,
					AttrInfo   *attributes){
	if (strcmp(relName, "relcat") == 0 || strcmp(relName, "attrcat") == 0) {
		myLog->LogDebug(dbName_not_right);
		return FAIL;
	}	
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
	//打开rel文件，查询所有relName属性 = deleteRelName的记录，删除该记录
/*	rfs.OpenScan(relfh, STRING, strlen(deleteRelName), 4, EQ_OP, (void*)deleteRelName);
	while (rfs.GetNextRec(rec) != -1){
		relfh.DeleteRec(rec.rid);
	}
	rfs.CloseScan();*/
	//打开attr文件，查询所有relName属性 = deleteRelName的记录，删除该记录
	rfs.OpenScan(attrfh, STRING, strlen(deleteRelName), 4, EQ_OP, (void*)deleteRelName);
	while (rfs.GetNextRec(rec) != -1){
		attrfh.DeleteRec(rec.rid);
	}
	rfs.CloseScan();
	return 0;
}

int SM_Manager::ShowTable (const char *readRelName){
	//打开attr文件，查询所有relName属性 = readRelName的记录， 打印该记录
	RM_FileScan rfs = RM_FileScan(rmm.getFileManager(), rmm.getBufPageManager());	
	RM_Record rec;
	rfs.OpenScan(attrfh, STRING, strlen(readRelName), 4, EQ_OP, (void*)readRelName);
	int x = 0;
	cout << "TableName : "<< readRelName << endl;
	while (1){
		x = rfs.GetNextRec(rec);
		if (x == -1)
			break;
		DataAttrInfo * mydata;
		rec.GetData((char*&) mydata);
		mydata->print();
	}
	cout << "Table End" << endl;
	rfs.CloseScan();
	return 0;
}

#endif

