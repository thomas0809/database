/*
 * testfilesystem.cpp
 *
 *  Created on: 2015年10月6日
 *      Author: lql
 */
#include "bufmanager/BufPageManager.h"
#include "fileio/FileManager.h"
#include "utils/pagedef.h"
#include "recordmanager/RecordManager.h"
#include "recordmanager/rm_filescan.h"
#include <iostream>

using namespace std;

void test_RM_Manager()
{
    FileManager* fm = new FileManager();
    BufPageManager* bpm = new BufPageManager(fm);
    RM_Manager* rm_m = new RM_Manager(fm, bpm);
    rm_m->CreateFile("testfile.txt", 32);
    int fileID;
    fm->openFile("testfile.txt", fileID); //打开文件，fileID是返回的文件id
    for (int pageID = 0; pageID < 10; ++ pageID) {
        int index;
        //为pageID获取一个缓存页
        BufType b = bpm->getPage(fileID, pageID, index);
        cout << b[0] << endl; 		//读取缓存页中第一个整数
        bpm->access(index); //标记访问
    }
}

void test_RM_FileScan()
{
    FileManager* fm = new FileManager();
    BufPageManager* bpm = new BufPageManager(fm);
    RM_Manager* rm_m = new RM_Manager(fm, bpm);
    //	fm->createFile("testfile.txt"); //新建文件
    rm_m->CreateFile("testfile.txt", 32);
    int fileID;
    fm->openFile("testfile.txt", fileID); //打开文件，fileID是返回的文件id
    
    RM_FileScan *scan = new RM_FileScan(fm, bpm);
    //char a[25];
    //for(int i = 0; i < 24; i++) a[i] = 97 + i;
    float a = 1.2;
    void* value = (void*)&a;
    AttrType attrType = FLOAT;
    Compop compOp = EQ_OP;
    int flag = scan->OpenScan(fileID, attrType, 4, 4,compOp, value);
    RM_Record rec;
    cout << rec.recordSize << endl;
    
    int loc = scan->GetNextRec(rec);
    cout << rec.recordSize << ' ' << rec.rid.Page() << ' ' << rec.rid.Slot() << endl;
    char* data = rec.data;
    int* dd = (int*)data;
    cout << dd[0] << endl;
    cout << loc << endl;
    loc = scan->GetNextRec(rec);
    cout << rec.recordSize << ' ' << rec.rid.Page() << ' ' << rec.rid.Slot() << endl;
    data = rec.data;
    dd = (int*)data;
    cout << dd[0] << endl;
    cout << loc << endl;
}

void test_RM_FileHandle()
{
	cout << "test_rm_filehandle" << endl;
	FileManager* fm = new FileManager();
	BufPageManager* bpm = new BufPageManager(fm);
	RM_Manager* rm_m = new RM_Manager(fm, bpm);
	rm_m->CreateFile("testfile.txt", 32);
	RM_FileHandle file;
	rm_m->OpenFile("testfile.txt", file);
	char data[33];
	data[32] = 0;
	for (int i = 0; i < 32; i++)
		data[i] = '0' + i;
	RID rid;
	for (int i = 0; i < 300; i++)
	{
		file.InsertRec(data, rid);
		cout << rid << endl;
	}
	RM_Record rec;
	file.GetRec(rid, rec);
	char *pdata;
	rec.GetData(pdata);
	memcpy(data, pdata, 32);
	printf("%s\n", data);
	rid = RID(5, 50);
	int x = file.GetRec(rid, rec);
	printf("%d\n", x);
	rm_m->CloseFile(file);
	rm_m->OpenFile("testfile.txt", file);
	rid = RID(1, 100);
	file.GetRec(rid, rec);
	rec.GetData(pdata);
	memcpy(data, pdata, 32);
	printf("%s\n", data);
	file.DeleteRec(rid);
	x = file.GetRec(rid, rec);
	printf("%d\n", x);
	for (int i = 0; i < 32; i++)
		data[i] = 'A' + i;
	rid = RID(1, 10);
	rec.Set(data, 32, rid);
	file.UpdateRec(rec);
	x = file.GetRec(rid, rec);
	printf("%d\n", x);
	file.GetRec(rid, rec);
	rec.GetData(pdata);
	memcpy(data, pdata, 32);
	printf("%s\n", data);
}

int main() {
    //test_RM_Manager();
    //test_RM_FileScan();
    test_RM_FileHandle();
	return 0;
}
