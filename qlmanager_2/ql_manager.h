/*
数据库查询模块，这一部分同时进行错误判断和处理
查询指令的解析使用Yacc，现在还处于正在完成阶段。
2015 12/14
*/

#ifndef QL_MANAGER_H
#define QL_MANAGER_H

#define NO_F 1
#define CONST_F 0
#define QL_ERRORWHERENOTINFROM 1000

#include "../recordManager/RecordManager.h"
#include "../recordManager/rm_filescan.h"
#include "../systemManager/sm_manager.h"
#include <string.h>
#include <iostream>
#include <map>

using namespace std;

/*class Condition{
	RelAttr lhsAttr;
	RelAttr rhsAttr;
	bool bRhsIsAttr;			//右侧条件是否为RelAttr
}

class AggRelAttr{
public:
	String relName;
	String attrName;
	int type;					// COUNT_F	0			NO_F  1
}


class RelAttr{
public:
	String relName;
	String attrName;
}*/
class Condition{
	
};

class RelAttr{
	
};

class AggRelAttr{
	
};

class Value{
public:
	AttrType attrType;
	int attrLength;
	void* data;
};

class QL_Manager{
	public:
		QL_Manager (SM_Manager &, RM_Manager &);
		~QL_Manager();                             //析构函数

	int Select  (int nSelAttrs,                    //select中属性个数
				  const AggRelAttr selAttrs[],     //select中属性集合        
				  int   nRelations,                //from中关系个数
				  const char * const relations[],  //from中关系集合
				  int   nConditions,               //where中条件个数
				  const Condition conditions[]);   //where中条件集合

	int Insert  (const char *relName,              //插入到那个关系中
				  int   nValues,                   //value域中个数
				  const Value values[]);           //value域中value集合

	int Delete  (const char *relName,              //在那个关系中删除
				  int   nConditions,               //where中删除的条件个数
				  const Condition conditions[]);   //where中删除的条件集合

	int Update  (const char *relName,              //updata的关系
				  const RelAttr &updAttr,          // attribute to update
				  const int bIsValue,              // 1 if RHS is a value, 0 if attr
				  const RelAttr &rhsRelAttr,       // attr on RHS to set LHS eq to
				  const Value &rhsValue,           // or value to set attr eq to
				  int   nConditions,               // # conditions in where clause
				  const Condition conditions[]);   // conditions in where clause
			  
/*	Iterator* GetLeafIterator(const char *relName, 	//relation to select from
								int nConditions, 	//# conditions in where clause
								const Condition conditions[], //conditions in where clause
								int nJoinConditions,
                                const Condition jconditions[], 
								int order, 				//whethere in order
								RelAttr* porderAttr);	//ordered by which RelAttr
	
	int MakeRootIterator(Iterator*& newit,
                            int nSelAttrs, const AggRelAttr selAttrs[],
                            int nRelations, const char * const relations[],
                            int order, RelAttr orderAttr,
                            bool group, RelAttr groupAttr) const;*/
	private:
		SM_Manager &smm;
		RM_Manager &rmm;
};

QL_Manager::QL_Manager(SM_Manager &_smm, RM_Manager &_rmm):smm(_smm), rmm(_rmm){
	
}

/*int QL_Manager::Select(int nSelAttrs, const AggRelAttr _selAttrs[], int nRelations, 
						const char*const _relations[], int nConditions, const Condition _conditions[]){
							
	RelAttr* selAttrs = new RelAttr[nSelAttrs];
	for (i = 0; i < nSelAttrs; i++) {
		selAttrs[i].relName = _selAttrs[i].relName;
		selAttrs[i].attrName = _selAttrs[i].attrName;
	}
  
	AggRelAttr* selAggAttrs = new AggRelAttr[nSelAttrs];
	for (i = 0; i < nSelAttrs; i++) {
		selAggAttrs[i].relName = _selAttrs[i].relName;
		selAggAttrs[i].attrName = _selAttrs[i].attrName;
		selAggAttrs[i].type = _selAttrs[i].type;
	}
	
	char** relations = new char*[nRelations];
	for (i = 0; i < nRelations; i++) {
		relations[i] = strdup(_relations[i]);
	}
	
	Condition* conditions = new Condition[nConditions];
	for (i = 0; i < nConditions; i++) {
		conditions[i] = _conditions[i];
	}
	
	//检查relations是否在smm中
	for (i = 0; i < nRelations; i++) {
		int rc = smm.SemCheck(relations[i]);							
		if (rc != 0) return rc;
	}
	
	//判断relation之间是否相等，如果相等那么报错
	if (!hasEqual(nRelations, _relation)){
		return -1;
	}
	
	if (nSelAttrs == 1 && strcmp(selAttrs[0].attrName, "*") == 0) {		//如果select *
		 for (int i = 0; i < nRelations; i++) {
			int ac;
			DataAttrInfo * aa;
			int rc = smm.GetFromTable(relations[i], ac, aa);
			if (rc != 0) return rc;
			nSelAttrs += ac;
			delete [] aa;
		}
		
		delete [] selAttrs;
		delete [] selAggAttrs;
		selAttrs = new RelAttr[nSelAttrs];
		selAggAttrs = new AggRelAttr[nSelAttrs];
	    int j = 0;
		for (int i = 0; i < nRelations; i++) {
			int ac;
			DataAttrInfo * aa;
			int rc = smm.GetFromTable(relations[i], ac, aa);
			for (int k = 0; k < ac; k++) {
				selAttrs[j].attrName = strdup(aa[k].attrName);
				selAttrs[j].relName = relations[i];
				selAggAttrs[j].attrName = strdup(aa[k].attrName);
				selAggAttrs[j].relName = relations[i];
				selAggAttrs[j].type = NO_F;
				j++;
			}
			delete [] aa;
		}
	}
	
	for (i = 0; i < nSelAttrs; i++) {
		if(strcmp(selAggAttrs[i].attrName, "*") == 0 && 
			selAggAttrs[i].type == COUNT_F) {
			selAggAttrs[i].attrName = strdup(groupAttr.attrName);
			selAggAttrs[i].relName = strdup(groupAttr.relName);
			selAttrs[i].attrName = strdup(groupAttr.attrName);
			selAttrs[i].relName = strdup(groupAttr.relName);
		}
	}
	
	
	for (i = 0; i < nSelAttrs; i++) {
		//如果relationname为空，则这个selAttrs 必须在某一个relation中，如果在，将selAttrs的relName赋值为寻找到的值
		if(selAttrs[i].relName == NULL) {
			int rc = smm.FindRelForAttr(selAttrs[i], nRelations, relations);
			if (rc != 0) return rc;
		} 
		else {
			selAttrs[i].relName = strdup(selAttrs[i].relName);
		}
		selAggAttrs[i].relName = strdup(selAttrs[i].relName);
	}
	
	//首先先将condition中的attrname寻找到所对应的relation name
	for (i = 0; i < nConditions; i++) {
		//如果条件中左边的某一个relationname为空，则和上面一样处理
		if(conditions[i].lhsAttr.relName == NULL) {
			int rc = smm.FindRelForAttr(conditions[i].lhsAttr, nRelations, relations);
			if (rc != 0) return rc;
		} 
		else {
			conditions[i].lhsAttr.relName = strdup(conditions[i].lhsAttr.relName);
		}
		
		//如果判断条件condition的类型是attr,那么右边的类型是relname和Attr
		if(conditions[i].bRhsIsAttr == TRUE) {
			if(conditions[i].rhsAttr.relName == NULL) {
				int rc = smm.FindRelForAttr(conditions[i].rhsAttr, nRelations, relations);
				if (rc != 0) return rc;
			} 
			else {
				conditions[i].rhsAttr.relName = strdup(conditions[i].rhsAttr.relName);
			}
			int rc = smm.SemCheck(conditions[i].rhsAttr);
			if (rc != 0) return rc;
		}
    }
	
	//再判断condition中attrname的relation是否在from的relation中
	for (int i = 0; i < nConditions; i++) {
		bool lfound = false;
		for (int j = 0; j < nRelations; j++) {
			if(strcmp(conditions[i].lhsAttr.relName, relations[j]) == 0) {
				lfound = true;
				break;
			}
		}
		if(!lfound) 
			return QL_ERRORWHERENOTINFROM;

		if(conditions[i].bRhsIsAttr == TRUE) {
			bool rfound = false;
			for (int j = 0; j < nRelations; j++) {
				if(strcmp(conditions[i].rhsAttr.relName, relations[j]) == 0) {
					rfound = true;
					break;
				}
			}
		if(!rfound) 
			return QL_ERRORWHERENOTINFROM;
		}
	}
	
	Iterator* it = NULL;
	
	if(nRelations == 1) {
		it = GetLeafIterator(relations[0], nConditions, conditions, 0, NULL, order, &orderAttr);	//取出所有的Attribute中满足condition的条目
		int rc = MakeRootIterator(it, nSelAttrs, selAggAttrs, nRelations, relation);				//从中筛选
		if(rc != 0) return rc;
		rc = PrintIterator(it);
		if(rc != 0) return rc;
	}

	delete it;

	//释放空间
	for(int i = 0; i < nSelAttrs; i++) {
		if(SELECTSTAR) {
			free(selAttrs[i].attrName);
			free(selAggAttrs[i].attrName);
		}
		free(selAttrs[i].relName);
		free(selAggAttrs[i].relName);
	}
	delete [] selAttrs;
	delete [] selAggAttrs;
	for (i = 0; i < nRelations; i++) {
		free(relations[i]);
	}
	delete [] relations;
	
	for(int i = 0; i < nConditions; i++) {
		free(conditions[i].lhsAttr.relName);
		if(conditions[i].bRhsIsAttr == TRUE)
			free(conditions[i].rhsAttr.relName);
	}
	delete [] conditions;
 	return 0;
}
				  
Iterator* QL_Manager::GetLeafIterator(const char *relName, int nConditions, const Condition conditions[], int nJoinConditions,
                                      const Condition jconditions[], int order, RelAttr* porderAttr){
	if(relName == NULL) {
		cout << "Waring : RelName = NULL" << endl;
		return NULL;
	}
	
	//返回所有smm中的attibute
	int attrCount = -1;												//数量
	DataAttrInfo * attributes;										//集合
	int rc = smm.GetFromTable(relName, attrCount, attributes);		//返回所有relName中的attributes
	if (rc != 0) return NULL;
	
	Condition * filters = NULL;
	int nFilters = -1;
	if (nJoinConditions == 0){										//如果只有一个relation
		int nIndexes = 0;
		int number = 0;
		char* chosenIndex = NULL;
		map<string, const Condition*> keys;							//记录条件中的有用的相关信息
		map<string, const Condition*> new_keys;
		for(int j = 0; j < nConditions; j++) {						//如果当前的relationname和condition中relname相同
			if(strcmp(conditions[j].lhsAttr.relName, relName) == 0) {
				keys[string(conditions[j].lhsAttr.attrName)] = &conditions[j];
			}

			if(conditions[j].bRhsIsAttr == TRUE &&
				strcmp(conditions[j].rhsAttr.relName, relName) == 0) {	
				keys[string(conditions[j].rhsAttr.attrName)] = &conditions[j];
			}
		}
		
		//获取最后一个condition
		for(map<string, const Condition*>::iterator it = keys.begin(); it != keys.end(); it++) {
			for (int i = 0; i < attrCount; i++) {
				if(attributes[i].indexNo != -1 && strcmp(it->first.c_str(), attributes[i].attrName) == 0) {		//如果condition中存在attrName相同
					nIndexes++;
					if(chosenIndex == NULL || attributes[i].attrType == INT || attributes[i].attrType == FLOAT) {
						chosenIndex = attributes[i].attrName;
						chosenCond = it->second;
					}
				}
			}
		}
		
		for(int j = 0; j < nConditions; j++){
			if(strcmp(conditions[j].lhsAttr.relName, relName) != 0) {											//如果condition左侧值不正确
				continue;
			}
			if(conditions[j].bRhsIsAttr == TRUE && strcmp(conditions[j].rhsAttr.relName, relName) != 0){		//如果condition右侧值不正确
				continue;
			}
			number++;
		}
		
		filter = new Condition[number];
		int index = -1;
		for (int j = 0; j < number; j++){
			if(strcmp(conditions[j].lhsAttr.relName, relName) != 0) {											//如果condition左侧值不正确
				continue;
			}
			if(conditions[j].bRhsIsAttr == TRUE && strcmp(conditions[j].rhsAttr.relName, relName) != 0){		//如果condition右侧值不正确
				continue;
			}
			index++;
			filter[index] = conditions[j];
		}
		
		if(chosenCond == NULL && (nConditions == 0 || nIndexes == 0)) {	//如果没有要求或者不存在relation中的要求
			Condition cond = NULLCONDITION;
			RC status = -1;
			Iterator* it = NULL;
			it = smm.getList(relation);										//获取relation中所有的attribute
			delete [] filters;
			delete [] attributes;
			return it;
		}
		
		//如果存在要求，则传入筛选后的要求
		Iterator* it = NULL;
		it = smm.getList(relation, index, filter);
		return NULL;
	}
	return NULL;
}

int QL_Manager::MakeRootIterator(Iterator*& newit,
                                int nSelAttrs, const AggRelAttr selAttrs[],
                                int nRelations, const char * const relations[],
                                int order, RelAttr orderAttr,
                                bool group, RelAttr groupAttr) const{					//在当前集合中选出所需字段
	
	return 0;
}

int QL_Manager::Insert (const char *relName, int nValues, const Value _values[]){

	Value* value = new Value[nValues];
	for (int i = 0; i < nValues; i++){
		value[i].type = _values[i].type;
		value[i].data = _values[i].data;
	}
	
	int attrCount;
	DataAttrInfo* attr;
	int rc = smm.GetFromTable(relName, attrCount, attr);
	//检查relName中属性个数是否与value中个数相同
	if (nValue != attrCount){
		cout << "Value number is incorrect" << endl;
		return 0;
	}
	
	//检查value中每个属性是否和relName中类型相同
	for (int i = 0; i < nValues; i++){
		if (value[i].type != attr[i].type){
			cout << "Value Type is incorrect" << endl;
			return 0;
		}
	}
	
	smm.insert(relName, nValues, value);
	delete [] attr;
	return 0;
}

int QL_Manager::Delete (const char *relName, int nConditions, const Condition _conditions[]){ 
	Condition * filters = NULL;
	int number= -1;
	Condition* conditions = new Condition[nConditions];
	for (int i = 0; i < nConditions; i++){
		conditions[i].lhsAttr = _conditions[i].lhsAttr;
		conditions[i].rhsAttr = _conditions[i].rhsAttr;
		conditions[i].bRhsIsAttr = _conditions[i].bRhsIsAttr;
	}
	//筛选出所需要的condition集合
	
	for(int j = 0; j < nConditions; j++){
			if(strcmp(conditions[j].lhsAttr.relName, relName) != 0) {											//如果condition左侧值不正确
				continue;
			}
			if(conditions[j].bRhsIsAttr == TRUE && strcmp(conditions[j].rhsAttr.relName, relName) != 0){		//如果condition右侧值不正确
				continue;
			}
			number++;
	}
		
	filter = new Condition[number];
	int index = -1;
	for (int j = 0; j < number; j++){
		if(strcmp(conditions[j].lhsAttr.relName, relName) != 0) {											//如果condition左侧值不正确
			continue;
		}
		if(conditions[j].bRhsIsAttr == TRUE && strcmp(conditions[j].rhsAttr.relName, relName) != 0){		//如果condition右侧值不正确
			continue;
		}
		index++;
		filter[index] = conditions[j];
	}
	
	int rc = smm.Delete(relName, index, filter);
	if (rc != 0)
		return rc;
	return 0;
}

int QL_Manager::Update (const char *relName, const RelAttr &updAttr, const int bIsValue, const RelAttr &rhsRelAttr,     
				  const Value &rhsValue, int nConditions, const Condition _conditions[]){					
	Condition * filters = NULL;
	int number= -1;
	Condition* conditions = new Condition[nConditions];
	for (int i = 0; i < nConditions; i++){
		conditions[i].lhsAttr = _conditions[i].lhsAttr;
		conditions[i].rhsAttr = _conditions[i].rhsAttr;
		conditions[i].bRhsIsAttr = _conditions[i].bRhsIsAttr;
	}
	//筛选出所需要的condition集合
	
	for(int j = 0; j < nConditions; j++){
			if(strcmp(conditions[j].lhsAttr.relName, relName) != 0) {											//如果condition左侧值不正确
				continue;
			}
			if(conditions[j].bRhsIsAttr == TRUE && strcmp(conditions[j].rhsAttr.relName, relName) != 0){		//如果condition右侧值不正确
				continue;
			}
			number++;
	}
		
	filter = new Condition[number];
	int index = -1;
	for (int j = 0; j < number; j++){
		if(strcmp(conditions[j].lhsAttr.relName, relName) != 0) {											//如果condition左侧值不正确
			continue;
		}
		if(conditions[j].bRhsIsAttr == TRUE && strcmp(conditions[j].rhsAttr.relName, relName) != 0){		//如果condition右侧值不正确
			continue;
		}
		index++;
		filter[index] = conditions[j];
	}
	
	it = smm.scan();	
	int rc = smm.Delete(it);
	if (rc != 0)
		return -1;
	it.update();
	rc = smm.Load(it);
	if (rc != 0)
		return -1;
	return 0;
}*/

int QL_Manager::Insert(const char *relName, int nValues, const Value _values[]){
	Value* value = new Value[nValues];
	int length = 0;
	int start = 0;
	for (int i = 0; i < nValues; i++){
		value[i].attrType = _values[i].attrType;
		value[i].attrLength = _values[i].attrLength;
		length = length + value[i].attrLength;
		value[i].data = _values[i].data;
	}
	RM_FileHandle file;
	cout << "openFile" << endl;
	rmm.OpenFile(relName, file);
	cout << "checkFile" << endl;
	RID rid;
	cout << "length" << length << endl;
	char* data = new char[length];
	//依次将value拷贝到data中
	for (int i = 0; i < nValues; i++){
		for (int j = 0; j < value[i].attrLength; j++){
			void* d = value[i].data;
			d = d + 1;
			char x[4];
			memcpy(x, d, 1);
			cout << x << endl;
		//	data[start + j] = char(d + j);
		}
		start = start + value[i].attrLength;
	}
	cout << "QL_Manager::Insert" << endl;
	file.InsertRec(data,rid);
	delete data;
	delete []value;
	return 0;
}

int QL_Manager::Delete (const char *relName, int nConditions, const Condition _conditions[]){
	Condition* conditon = new Condition[nConditions];
	for (int i = 0; i < nConditions; i++){
	}
	return 0;
}
#endif 