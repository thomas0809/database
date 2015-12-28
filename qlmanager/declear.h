#ifndef DECLEAR_H
#define DECLEAR_H

#define MAXNAME 50
#define MAXATTRNUMBER 10
#define MAXATTRLENGTH 400
#define MAXRID 500000
#define MAXRELATION 5

#include <iostream>
#include "../recordmanager/rm_filescan.h"
#include "../systemmanager/sm_manager.h"
#include "../recordmanager/RecordManager.h"
#include "../common/common.h"
#include "../indexmanager/IX_Manager.h"

using namespace std;

// enum AttrType {
    // MyINT,
    // FLOAT,
    // STRING
// }; 

// enum ClientHint {
    // NO_HINT
// };

// struct AttrInfo {
   // char     attrName[MAXNAME + 1];           // Attribute name
   // AttrType attrType;            // Type of attribute
   // int      attrLength;          // Length of attribute
   // int     notNull;
   // bool     primaryKey;
   // AttrInfo(){
       // memset(attrName, 0, sizeof(attrName));
       // attrType = MyINT;
       // attrLength = 0;
       // notNull = 0;
       // primaryKey = false;
   // }
// };

struct RelAttr {
    AggeType type;
    char *relName;     // relation name (may be NULL) 
    char *attrName;    // attribute name  
    char *outkeyrel;
    char *outkeyattr;            
    //friend ostream &operator<<(ostream &s, const RelAttr &ra);
};

struct Value {
    AttrType type;     // type of value               
    void     *data;    // value                       
    friend ostream &operator<<(ostream &s, const Value &v){
        if(v.type == MyINT) {
            int *ddd = (int*)v.data;
            s << "INT : " << *ddd << endl;
        }
        else if(v.type == FLOAT) {
            float *ddd = (float*)v.data;
            s << "FLOAT : " << *ddd << endl;
        }
        else if(v.type == STRING) {
            char *ddd = (char*)v.data;
            s << "STRING : " << ddd << endl;
        }
        return s;
        }
	int getlength() const {
		if (type == MyINT){
			return sizeof(int);
		}
		else if (type == FLOAT){
			return sizeof(float);
		}
		else if (type == STRING){
			char *ddd = (char*)data;
			return strlen(ddd);
		}
	}
};

struct Condition {
    RelAttr lhsAttr;      // left-hand side attribute                     
    CompOp  op;           // comparison operator                          
    int     bRhsIsAttr;   // TRUE if right-hand side is an attribute
                          //   and not a value
    RelAttr rhsAttr;      // right-hand side attribute if bRhsIsAttr = TRUE
    Value   rhsValue;     // right-hand side value if bRhsIsAttr = FALSE
    //friend ostream &operator<<(ostream &s, const Condition &c);
};

// class RID {
// public:
  // static const int NULL_PAGE = -1;
  // static const int NULL_SLOT = -1;
  // RID() : page(NULL_PAGE), slot(NULL_SLOT) {}     // Default constructor
  // RID(int pageNum, int slotNum) : page(pageNum), slot(slotNum) {}
  // ~RID(){}                                        // Destructor

  // int GetPageNum(int &pageNum) const          // Return page number
  // { pageNum = page; return 0; }
  // int GetSlotNum(int &slotNum) const         // Return slot number
  // { slotNum = slot; return 0; }

  // int Page() const          // Return page number
  // { return page; }
  // int Slot() const          // Return slot number
  // { return slot; }

  // bool operator==(const RID & rhs) const
  // {
    // int p;
    // int s;
    // rhs.GetPageNum(p);
    // rhs.GetSlotNum(s);
    // return (p == page && s == slot);
  // }

// private:
  // int page;
  // int slot;
// };

class QL_Manager {
    public:
                                              // Constructor
		QL_Manager (SM_Manager *_smm, RM_Manager *_rmm, IX_Manager *_ixm):smm(_smm), rmm(_rmm), ixm(_ixm){}
        //QL_Manager (SM_Manager &smm, IX_Manager &ixm, RM_Manager &rmm);
        ~QL_Manager ();                         // Destructor
        
        void Print(int nSelAttrs, const RelAttr selAttrs[], const char* relName, int nrid, RID* rid) {
            cout << "QL PRINT Function" << endl;
            cout << relName << endl;
            cout << "NSelAttr : " << nSelAttrs << endl;
            for(int i = 0; i < nSelAttrs; i++) {
                cout << selAttrs[i].type << ' ' << (selAttrs[i].relName == NULL) << ' ' << selAttrs[i].attrName << endl;
            } 
            cout << "nrid" << nrid << endl;
	    for (int i = 0; i < nrid; i++)
		cout << rid[i] << endl;
            //select *
            int SelectPoint = 0;
            int newSelAttrs = 0;
            if (nSelAttrs == 1 && strcmp(selAttrs[0].attrName,"*") == 0){
		cout << "select *" << endl;
		SelectPoint = 1;
            }
            RM_FileScan rfs = RM_FileScan(rmm->getFileManager(), rmm->getBufPageManager());
	    RM_Record rec;
            RM_FileHandle attrfh;
            int returnCode = 0; 
            rmm->OpenFile("attrcat",attrfh);
            returnCode = rfs.OpenScan(attrfh, STRING, strlen(relName), 16, EQ_OP, (void*)relName);
	    int x = 0;
            DataAttrInfo * getAttr;
            if (SelectPoint == 0)
            	getAttr = new DataAttrInfo[nSelAttrs];
            else
		getAttr = new DataAttrInfo[MAXATTRNUMBER];

	    while (returnCode == 1){
//		cout << "ret : 1" << endl;
		x = rfs.GetNextRec(rec);
//		cout << "ret : 2" << endl;
		if (x == -1)
			break;
//		cout << x << endl;
		DataAttrInfo * mydata;
		rec.GetData((char*&) mydata);
//	        cout << mydata->attrName << endl;
//		cout << cond->lhsAttr.attrName << endl;
		if (SelectPoint == 0){
			for (int i = 0; i < nSelAttrs; i++){
				if (strcmp(mydata->attrName, selAttrs[i].attrName) == 0 && strcmp(mydata->relName, relName) == 0){
//					cout << "offset: " << mydata->offset << "i: " << i << endl;
//					cout << sizeof(DataAttrInfo) << endl;
					memcpy(&(getAttr[i]), mydata, sizeof(DataAttrInfo));
//					cout << getAttr[i].offset << endl;
					break;
				}
			}
		}
		else{
			if (strcmp(mydata->relName, relName) == 0){
				memcpy(&(getAttr[newSelAttrs]), mydata, sizeof(DataAttrInfo));
				newSelAttrs++;
			}
		}
	    }
	    rfs.CloseScan();
            rmm->CloseFile(attrfh);
	    
            if (SelectPoint != 0){
		cout << "new SelAttrs" << newSelAttrs << endl;
		nSelAttrs = newSelAttrs;
	    }
	    cout << "Start print attrName: " << endl;
	    for (int i = 0; i < nSelAttrs; i++)
		cout << getAttr[i].attrName << ' ' <<getAttr[i].offset << endl;
	    
	    //get record
	    cout << "ALL RECORD========= (0_0)=========" << endl;
            for (int i = 0; i < nrid; i++){
                rmm->OpenFile(relName, attrfh);
		RM_Record tempRec;
		cout << rid[i] << endl;
                attrfh.GetRec(rid[i], tempRec);
	    char *temp;
		temp = tempRec.data;
//		for (int i = 0; i < strlen(temp); i++)
//			cout << temp[i];
                for (int j = 0; j < nSelAttrs; j++){
//			cout << selAttrs[j].attrName << ' ' << getAttr[j].offset << ' ';
			cout << getAttr[j].attrName << ": ";
			char* buf = new char[MAXATTRLENGTH];
			memset(buf, 0, MAXATTRLENGTH);
			int length = 0;
			if (getAttr[j].attrType == MyINT)
				length = sizeof(int);
			else if (getAttr[j].attrType == FLOAT)
				length = sizeof(float);
			else
				length = getAttr[j].attrLength;
			memcpy(buf, temp + getAttr[j].offset, length);
//			cout << "buf: ";
//			for (int k = 0; k < 10; k++)
//				cout << buf[k];
//			cout << endl;
                        if (getAttr[j].attrType == MyINT){
                             int* t = (int*)buf;
                             cout << *t;
			}
                        else if (getAttr[j].attrType == FLOAT){
			     float* t = (float*)buf;
                             cout << *t;
			}
                        if (getAttr[j].attrType == STRING){
                             char* t = buf;
			     cout << t;
			}
                        cout << ", ";
		}
		cout << endl;
		rmm->CloseFile(attrfh);
            }
	    delete []getAttr;
            cout << "-----------END PRINT-----------" << endl;
        }


        void Select (int           nSelAttrs,        // # attrs in Select clause
                   const RelAttr selAttrs[],       // attrs in Select clause
                   int           nRelations,       // # relations in From clause
                   const char * const relations[], // relations in From clause
                   int           isGroup,
                   int           nConditions,      // # conditions in Where clause
                   const Condition conditions[],  // conditions in Where clause
                   const RelAttr& groupAttr) 
        {
		cout << "shabi" << endl;
            cout << "QL Select Function" << endl;
            cout << "NSelAttr : " << nSelAttrs << endl;
            for(int i = 0; i < nSelAttrs; i++) {
                cout << selAttrs[i].type << ' ' << (selAttrs[i].relName == NULL) << ' ' << selAttrs[i].attrName << endl;
            } 
            cout << "NRELATIONS : " << nRelations << endl;
            for(int i = 0; i < nRelations; i++) {
                cout << relations[i] << endl;
            }

            cout << "ISGROUP : " << isGroup << endl;
            if(isGroup) {
                cout << (groupAttr.relName == NULL) <<  ' ' << groupAttr.attrName << endl; 
            }
            else {
                cout << "NCONDITIONS : " << nConditions << endl;
                for(int i = 0; i < nConditions; i++) {
                    cout << conditions[i].lhsAttr.attrName << ' ' << conditions[i].op << endl;
                }
            }

  	    RM_FileScan rfs = RM_FileScan(rmm->getFileManager(), rmm->getBufPageManager());
	    RM_FileHandle attrfh;
            for (int i = 0; i < nSelAttrs; i++){
		if (selAttrs[i].relName == NULL){
		    cout << "Select Error: need relation" << endl;
		}
		int judge = 0;
		for (int j = 0; j < nRelations; j++){
		    if (strcmp(relations[i], selAttrs[i].relName) == 0){
			judge = 1;
			break;
		    }		
		}
		if (judge == 0){
		    cout << "Select Error: can't get relation" << endl;
		    return;
		}
	    }
            
	    //get All Rid
            RID rid[MAXRELATION][MAXRID];
	    int number[MAXRELATION];
            for (int i = 0; i < nRelations; i++){
	    	int returnCode = 0;
		int nrid = 0;
		int x = 0;
	    	RM_Record rec_1;
		RID tempRid;
 	        rmm->OpenFile(relations[i], attrfh);
	    	returnCode = rfs.OpenScan(attrfh, MyINT, 0, 0, EQ_OP, NULL);
	        while (returnCode == 1){
		    x = rfs.GetNextRec(rec_1);
		    if (x == -1)
			break;
//		cout << x << endl;
//		char* t = new char[100];
//		char* temp = NULL;
//		rec_1.GetData((char*&) temp);

//		for (int i = 0; i < 100; i++)
//			cout << temp[i];
//		cout << t << endl;		
//		strncpy(t, temp, 100);
//		for (int i = 0; i < 100; i++)
//			cout << t[i];
//		cout << t << endl;
//		delete []t;
		    rec_1.GetRid(tempRid);
		    rid[i][nrid].Copy(tempRid);
		    cout << rid[i][nrid] << endl;
		    nrid++;
//		    cout << "get Rid: " << tempRid << endl;
	    	}
		number[i] = nrid;
	    	rfs.CloseScan();
	    	rmm->CloseFile(attrfh);		
	    }
	
	    
            cout << "END SELECT" << endl << endl; 
        }

        void Insert (const char  *relName,           // relation to insert into
                   int         nValues,            // # values to insert
                   const Value values[],            // values to insert
                   int         nattr,               //
                   const AttrInfo attr[]){
	    RM_FileHandle attrfh;
	    RID rid;
	    int countAttr = 0;
	    DataAttrInfo* dataInfo = NULL;
	    int size = 0;
	    int offset = 0;
	    int inSet[countAttr];
            int label[nValues];
            cout << "=============QL Insert Function==============" << endl;
            cout << "RELNAME : " << relName << endl;
            cout << "nValues : " << nValues << endl;
            for(int i = 0; i < nValues; i++) {
                cout << values[i];

            }
            cout << "nAttr : " << nattr << endl;
			
            for(int i = 0; i < nattr; i++) {
                cout << attr[i].attrName << endl;
            }
	    
//	    cout << "debug: 1" << endl;
	    smm->GetTable(relName, countAttr, dataInfo);
//	    cout << "countAttr: " << countAttr << endl;
	    cout << dataInfo[0].attrLength << endl;
            //insert into Table, check value type

	    for (int i = 0; i < countAttr; i++)
		inSet[i] = -1;
	    for (int i = 0; i < nValues; i++)
		label[i] = -1;

	    if (nattr == 0){
	    	if (countAttr != nValues){
		    cout << "Insert Error: value number is incorrect" << endl;		
		    return;
	    	}
		for (int i = 0; i < countAttr; i++){
		    if (values[i].type == 3){
			if (dataInfo[i].notNull == 1){
			    cout << "Insert Error: not Null: " << dataInfo[i].attrName <<  endl; 
			    return;
			}
			else{
			    label[i] = -2;
			}
		    }
		    else{
			if (dataInfo[i].attrType != values[i].type){
				cout << "Insert Error: value type is incorrect: " << dataInfo[i].attrName << ' ' << dataInfo[i].attrType << endl;
				return;
			}
			label[i] = i;
		    }		
		}
	    }
	    else{
		if (nattr != nValues){
		    cout << "Insert Error: value number is incorrect" << endl;	
		    return;	
		}
	    }
	    //get all dataInfo
/*	    for (int i = 0; i < countAttr; i++)
		cout << dataInfo[i].notNull<< ' ';
	    cout << endl;
	    for (int i = 0; i < countAttr; i++)
		dataInfo[i].print();*/
	    for (int i = 0; i < countAttr; i++){
		size += dataInfo[i].attrLength; 
	    }

	    if (nattr != 0){
	    //check attr type
            	for (int i = 0; i < nValues; i++){
		    for (int j = 0; j < countAttr; j++){
		    	if (strcmp(attr[i].attrName, dataInfo[j].attrName) == 0){
		    	    if (inSet[j] != -1){
			        cout << "Insert Error: same attribute: " << attr[i].attrName <<  endl;
				return;
			    }
			    // if value = null
			    if (values[i].type == 3){
				inSet[j] = -2;
				label[i] = -2;
			    }
                            else{
				inSet[j] = i; 
                            	label[i] = j;
			    }
		            break;
		    	}
		    }
                }
                for (int i = 0; i < countAttr; i++){
		    if (inSet[i] <= -1 && dataInfo[i].notNull == 1){
			cout << "Insert Error: not Null: " << dataInfo[i].attrName <<  endl; 
			return;
		    }		
		}
	    }
	    //在relname中查找，重新获取长度
//	    cout << "debug: 2   size" << size << endl;
            cout << "label" << ' ';
	    for (int i = 0; i < nValues; i++)
		cout << label[i] << ' ';
	    cout << endl;
	    rmm->OpenFile(relName, attrfh);
	    char *buf = new char[size + countAttr];
//	    cout << "debug: 3.1" << endl;
	    memset(buf, 0, size + countAttr);
	    for(int i = 0; i < nValues; i++) {
		if (label[i] <= -1)
		    continue;
//	    	cout << "debug: 3.2" << endl;
	    	int length = 0;
		cout << label[i] <<"label[i]" << dataInfo[label[i]].attrName << ' ' << values[i] << endl;
	    	memcpy(buf + dataInfo[label[i]].offset, values[i].data, values[i].getlength());
//	    	cout << "length: " << values[i].getlength() << endl;
//	    	cout << "debug: 3" << endl;
		buf[size + label[i]] = 1;
	    }
//			cout << "debug: 4" << endl;
			attrfh.InsertRec(buf, rid);

            for(int i = 0; i < nValues; i++) {
                char indexname[100];
                memset(indexname, 0, sizeof indexname);
                sprintf(indexname, "%s.%s.index", relName, dataInfo[i].attrName);
                if (access(indexname, 0) != -1) {
                    IX_IndexHandle ixh;
                    ixm->OpenIndex(relName, dataInfo[i].attrName, ixh);
                    ixh.InsertEntry(values[i].data, rid);
                    ixm->CloseIndex(ixh);
                }
            }
//			cout << "Insert Correctly, Rid: " << rid.Page() << ' ' << rid.Slot() << endl;
			rmm->CloseFile(attrfh);
			delete []buf;
			delete []dataInfo;
            cout << "END INSERT" << endl;
        }

        void Search(const char* relName, const Condition* cond, int& nrid, RID*& rid) {//need two RM_Record, rid = newRID[500000];

            cout << "QL Search Function" << endl;
            cout << relName << endl;
            cout << (cond->lhsAttr.relName == NULL) << ' ' << cond->lhsAttr.attrName << endl;
            cout << "OP : " << cond->op << endl;
            if (cond->op != 7){
            if(cond->bRhsIsAttr) {
                cout << (cond->rhsAttr.relName == NULL) << ' ' << cond->rhsAttr.attrName << endl;
            }
            else {
                cout << cond->rhsValue;
            }}
	    cout << "debug: qby 1" << endl;
            nrid = 0;
            rid = new RID[MAXRID];
            //check bRhsIsAttr, if bRhsIsAttr == true ,exit
            if (cond->bRhsIsAttr) {
		        cout << "Error:  WHERE ? op value" << endl;
		        return;
            }
            RM_FileScan rfs = RM_FileScan(rmm->getFileManager(), rmm->getBufPageManager());
	        RM_FileHandle attrfh;
	        RM_Record rec;
	        RID tempRid;
	        int returnCode;
	        AttrType attrType;
            int attrLength;
            int attrOffset;
	    int attrIndexNo;
            int inRel = -1;

//	    cout << "debug: qby 2" << endl;
            //get attrType
            rmm->OpenFile("attrcat",attrfh);
            returnCode = rfs.OpenScan(attrfh, STRING, strlen(relName), 16, EQ_OP, (void*)relName);
	    int x = 0;
	    while (returnCode == 1){
//		cout << "ret : 1" << endl;
		x = rfs.GetNextRec(rec);
//		cout << "ret : 2" << endl;
		if (x == -1)
			break;
//		cout << x << endl;
		DataAttrInfo * mydata;
		rec.GetData((char*&) mydata);
//	        cout << mydata->attrName << endl;
//		cout << cond->lhsAttr.attrName << endl;
		if (strcmp(mydata->attrName, cond->lhsAttr.attrName) == 0){
			attrType = mydata->attrType;
			attrLength = mydata->attrLength;
			attrOffset = mydata->offset;
			attrIndexNo = mydata->indexNo;
			inRel = 0;
			break;
		}
	    }
	    rfs.CloseScan();
            rmm->CloseFile(attrfh);
            
//	    cout << "debug: qby 3" << endl;
            if (inRel == -1){
	    	cout << "Attribute not in Relation!" << endl;
		return;
	    }

	    //cout << "attrType: " << attrType << endl;
	    //cout << "attrLength: " << attrLength << endl;
	    //cout << "attrOffset: " << attrOffset << endl;

        char buf[100];
        memset(buf, 0, sizeof buf);
        sprintf(buf, "%s.%s.index", relName, cond->lhsAttr.attrName);
        if (access(buf, 0) != -1) {
            IX_IndexHandle ixh;
            ixm->OpenIndex(relName, cond->lhsAttr.attrName, ixh);
            IX_IndexScan ixscan;
            //printf("%s\n", cond->rhsValue.data);
            ixscan.OpenScan(ixh, cond->op, cond->rhsValue.data);
            RID tmprid;
            nrid = 0;
            while (ixscan.GetNextEntry(tmprid) != -1) {
                rid[nrid] = tmprid;
                nrid += 1;
            }
            //ixh.PrintEntries();
            ixm->CloseIndex(ixh);
            printf("search from index, %d\n", nrid);
            return;
        }


            returnCode = 0;
        rmm->OpenFile(relName, attrfh);
	    for (int i = 0; i < 10; i++)
		    cout << *((char*)(cond->rhsValue.data)+ i) << ' ';
	    cout << endl;
	    returnCode = rfs.OpenScan(attrfh, attrType, attrLength, attrOffset, cond->op, cond->rhsValue.data);
	    cout << returnCode << endl;
	    RM_Record rec_1;
	    while (returnCode == 1) {
//		cout << "ret : 1" << endl;
		    x = rfs.GetNextRec(rec_1);
//		cout << "ret : 2" << endl;
		    if (x == -1)
			    break;
//		cout << x << endl;
//		char* t = new char[100];
//		char* temp = NULL;
//		rec_1.GetData((char*&) temp);

//		for (int i = 0; i < 100; i++)
//			cout << temp[i];
//		cout << t << endl;		
//		strncpy(t, temp, 100);
//		for (int i = 0; i < 100; i++)
//			cout << t[i];
//		cout << t << endl;
//		delete []t;
		    rec_1.GetRid(tempRid);
		    rid[nrid].Copy(tempRid);
		    cout << rid[nrid] << endl;
		    nrid++;
//		cout << "get Rid: " << tempRid << endl;
	    }
	    rfs.CloseScan();
	    rmm->CloseFile(attrfh);
            cout << "END SEARCH" << endl;
        }

        void Delete (const char *relName,            // relation to delete from
                   int        nrid,         // # conditions in Where clause
                   const RID rid[])  // conditions in Where clause
        {
            cout << "QL Delete Function" << endl;
            cout << relName << endl;
            cout << nrid << endl;
 	    RM_FileScan rfs = RM_FileScan(rmm->getFileManager(), rmm->getBufPageManager());
	    RM_Record rec;
            RM_FileHandle attrfh;
            int countAttr = 0;
            DataAttrInfo* dataInfo = NULL;
            smm->GetTable(relName, countAttr, dataInfo);

            rmm->OpenFile(relName,attrfh);
        for (int j = 0; j < countAttr; j++) {
            char indexname[100];
            memset(indexname, 0, sizeof indexname);
            sprintf(indexname, "%s.%s.index", relName, dataInfo[j].attrName);
            if (access(indexname, 0) != -1) {
                IX_IndexHandle ixh;
                ixm->OpenIndex(relName, dataInfo[j].attrName, ixh);
                for (int i = 0; i < nrid; i++) {
                    RM_Record rec;
                    attrfh.GetRec(rid[i], rec);
                    char *data = rec.data + dataInfo[j].offset;
                    ixh.DeleteEntry(data, rid[i]);
                }
                ixm->CloseIndex(ixh);
            }
        }
	    for (int i = 0; i < nrid; i++){
            attrfh.DeleteRec(rid[i]);
	    }
	    rmm->CloseFile(attrfh);
            cout << "END DELETE" << endl << endl;

        }
        void Update (const char *relName,            // relation to update
                   const RelAttr &updAttr,         // attribute to update
                   const int bIsValue,             // 0/1 if RHS of = is attribute/value
                   const RelAttr &rhsRelAttr,      // attr on RHS of =
                   const Value &rhsValue,          // value on RHS of =
                   int   nrid,              // # conditions in Where clause
                   const RID rid[])  // conditions in Where clause
        {
            cout << "QL Update Dunction" << endl;
            cout << (updAttr.relName == NULL) << ' ' << updAttr.attrName << endl;
            cout << "BISVALUE : " << bIsValue << endl;
            if(!bIsValue){
                cout << (rhsRelAttr.relName == NULL) << ' ' << rhsRelAttr.attrName << endl;
            } 
            else{
                cout << rhsValue;
            }
            cout << nrid << endl;
            //get position of updata attr

            RM_FileScan rfs = RM_FileScan(rmm->getFileManager(), rmm->getBufPageManager());
	    RM_Record rec;
            RM_FileHandle attrfh;
            int returnCode = 0; 
            rmm->OpenFile("attrcat",attrfh);
            returnCode = rfs.OpenScan(attrfh, STRING, strlen(relName), 16, EQ_OP, (void*)relName);
	    int x = 0;

            DataAttrInfo lAttrInfo;
            DataAttrInfo rAttrInfo;

	    while (returnCode == 1){
//		cout << "ret : 1" << endl;
		x = rfs.GetNextRec(rec);
//		cout << "ret : 2" << endl;
		if (x == -1)
			break;
//		cout << x << endl;
		DataAttrInfo * mydata;
		rec.GetData((char*&) mydata);
//	        cout << mydata->attrName << endl;
//		cout << cond->lhsAttr.attrName << endl;
		if (strcmp(mydata->attrName, updAttr.attrName) == 0){
			memcpy(&lAttrInfo, mydata, sizeof(DataAttrInfo));			
		}
		if (!bIsValue){
			if (strcmp(mydata->attrName, rhsRelAttr.attrName) == 0){
				memcpy(&rAttrInfo, mydata, sizeof(DataAttrInfo));
			}
		}
	    }
	    rfs.CloseScan();
            rmm->CloseFile(attrfh);

	    if (!bIsValue && ((lAttrInfo.attrType != rAttrInfo.attrType)
		|| (lAttrInfo.attrLength != rAttrInfo.attrLength))){
		cout << "Updata Error: type is not equal " << endl;
		return;
	    }
            for (int i = 0; i < nrid; i++){
                rmm->OpenFile(relName, attrfh);
		RM_Record tempRec;
		cout << rid[i] << endl;
                attrfh.GetRec(rid[i], tempRec);
		char *temp;
		temp = (char*)tempRec.data;
		
//		for (int i = 0; i < strlen(temp); i++)
//			cout << temp[i];
		char* buf = new char[MAXATTRLENGTH];
		memset(buf, 0, MAXATTRLENGTH);
		int length = 0;
		if (lAttrInfo.attrType == MyINT)
			length = sizeof(int);
		else if (lAttrInfo.attrType == FLOAT)
			length = sizeof(float);
		else
			length = lAttrInfo.attrLength;
		cout << "lAttrInfo.offset" << lAttrInfo.offset << endl;
		cout << "attrLength => " << length << endl;
		memcpy(buf, temp + lAttrInfo.offset, length);
		char* rbuf = new char[MAXATTRLENGTH];
		memset(rbuf, 0, MAXATTRLENGTH);
		if (!bIsValue)
			memcpy(rbuf, temp + rAttrInfo.offset, length);
		else
			memcpy(rbuf, rhsValue.data, length);
//		for (int j = 0; j < length; j++){
//			cout << buf[j] << rbuf[j] << ' ';		
//		}
		if (lAttrInfo.attrType == STRING){
		    for (int j = 0; j < length; j++)
			if (rbuf[j] == 0){
			    length = j;
			    break;
			}
		}
		memcpy(temp + lAttrInfo.offset, rbuf, length);
		rbuf[0] = 1;
		//delete null
		memcpy(temp + lAttrInfo.indexNo, rbuf, 1);
		attrfh.UpdateRec(tempRec);
		cout << endl;
/*                for (int j = 0; j < nSelAttrs; j++){
//			cout << selAttrs[j].attrName << ' ' << getAttr[j].offset << ' ';
			cout << selAttrs[j].attrName << ' ';
			char* buf = new char[400];
			memset(buf, 0, 400);
			int length = 0;
			if (getAttr[j].attrType == MyINT)
				length = sizeof(int);
			else if (getAttr[j].attrType == FLOAT)
				length = sizeof(float);
			else
				length = getAttr[j].attrLength;
			memcpy(buf, temp + getAttr[j].offset, length);
//			cout << "buf: ";
//			for (int k = 0; k < 10; k++)
//				cout << buf[k];
//			cout << endl;
                        if (getAttr[j].attrType == MyINT){
                             int* t = (int*)buf;
                             cout << *t;
			}
                        else if (getAttr[j].attrType == FLOAT){
			     float* t = (float*)buf;
                             cout << *t;
			}
                        if (getAttr[j].attrType == STRING){
                             char* t = buf;
			     cout << t;
			}
                        cout << ' ';
		}*/
		rmm->CloseFile(attrfh);
            }         
            cout << "END UPDATE" << endl << endl;
        }
		
private:
	SM_Manager *smm;
	RM_Manager *rmm;
    IX_Manager *ixm;
};

#endif
