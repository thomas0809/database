/*
���ݿ��ѯģ�飬��һ����ͬʱ���д����жϺʹ���
��ѯָ��Ľ���ʹ��Yacc�����ڻ�����������ɽ׶Ρ�
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
	bool bRhsIsAttr;			//�Ҳ������Ƿ�ΪRelAttr
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
		~QL_Manager();                             //��������

	int Select  (int nSelAttrs,                    //select�����Ը���
				  const AggRelAttr selAttrs[],     //select�����Լ���        
				  int   nRelations,                //from�й�ϵ����
				  const char * const relations[],  //from�й�ϵ����
				  int   nConditions,               //where����������
				  const Condition conditions[]);   //where����������

	int Insert  (const char *relName,              //���뵽�Ǹ���ϵ��
				  int   nValues,                   //value���и���
				  const Value values[]);           //value����value����

	int Delete  (const char *relName,              //���Ǹ���ϵ��ɾ��
				  int   nConditions,               //where��ɾ������������
				  const Condition conditions[]);   //where��ɾ������������

	int Update  (const char *relName,              //updata�Ĺ�ϵ
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
	
	//���relations�Ƿ���smm��
	for (i = 0; i < nRelations; i++) {
		int rc = smm.SemCheck(relations[i]);							
		if (rc != 0) return rc;
	}
	
	//�ж�relation֮���Ƿ���ȣ���������ô����
	if (!hasEqual(nRelations, _relation)){
		return -1;
	}
	
	if (nSelAttrs == 1 && strcmp(selAttrs[0].attrName, "*") == 0) {		//���select *
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
		//���relationnameΪ�գ������selAttrs ������ĳһ��relation�У�����ڣ���selAttrs��relName��ֵΪѰ�ҵ���ֵ
		if(selAttrs[i].relName == NULL) {
			int rc = smm.FindRelForAttr(selAttrs[i], nRelations, relations);
			if (rc != 0) return rc;
		} 
		else {
			selAttrs[i].relName = strdup(selAttrs[i].relName);
		}
		selAggAttrs[i].relName = strdup(selAttrs[i].relName);
	}
	
	//�����Ƚ�condition�е�attrnameѰ�ҵ�����Ӧ��relation name
	for (i = 0; i < nConditions; i++) {
		//�����������ߵ�ĳһ��relationnameΪ�գ��������һ������
		if(conditions[i].lhsAttr.relName == NULL) {
			int rc = smm.FindRelForAttr(conditions[i].lhsAttr, nRelations, relations);
			if (rc != 0) return rc;
		} 
		else {
			conditions[i].lhsAttr.relName = strdup(conditions[i].lhsAttr.relName);
		}
		
		//����ж�����condition��������attr,��ô�ұߵ�������relname��Attr
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
	
	//���ж�condition��attrname��relation�Ƿ���from��relation��
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
		it = GetLeafIterator(relations[0], nConditions, conditions, 0, NULL, order, &orderAttr);	//ȡ�����е�Attribute������condition����Ŀ
		int rc = MakeRootIterator(it, nSelAttrs, selAggAttrs, nRelations, relation);				//����ɸѡ
		if(rc != 0) return rc;
		rc = PrintIterator(it);
		if(rc != 0) return rc;
	}

	delete it;

	//�ͷſռ�
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
	
	//��������smm�е�attibute
	int attrCount = -1;												//����
	DataAttrInfo * attributes;										//����
	int rc = smm.GetFromTable(relName, attrCount, attributes);		//��������relName�е�attributes
	if (rc != 0) return NULL;
	
	Condition * filters = NULL;
	int nFilters = -1;
	if (nJoinConditions == 0){										//���ֻ��һ��relation
		int nIndexes = 0;
		int number = 0;
		char* chosenIndex = NULL;
		map<string, const Condition*> keys;							//��¼�����е����õ������Ϣ
		map<string, const Condition*> new_keys;
		for(int j = 0; j < nConditions; j++) {						//�����ǰ��relationname��condition��relname��ͬ
			if(strcmp(conditions[j].lhsAttr.relName, relName) == 0) {
				keys[string(conditions[j].lhsAttr.attrName)] = &conditions[j];
			}

			if(conditions[j].bRhsIsAttr == TRUE &&
				strcmp(conditions[j].rhsAttr.relName, relName) == 0) {	
				keys[string(conditions[j].rhsAttr.attrName)] = &conditions[j];
			}
		}
		
		//��ȡ���һ��condition
		for(map<string, const Condition*>::iterator it = keys.begin(); it != keys.end(); it++) {
			for (int i = 0; i < attrCount; i++) {
				if(attributes[i].indexNo != -1 && strcmp(it->first.c_str(), attributes[i].attrName) == 0) {		//���condition�д���attrName��ͬ
					nIndexes++;
					if(chosenIndex == NULL || attributes[i].attrType == INT || attributes[i].attrType == FLOAT) {
						chosenIndex = attributes[i].attrName;
						chosenCond = it->second;
					}
				}
			}
		}
		
		for(int j = 0; j < nConditions; j++){
			if(strcmp(conditions[j].lhsAttr.relName, relName) != 0) {											//���condition���ֵ����ȷ
				continue;
			}
			if(conditions[j].bRhsIsAttr == TRUE && strcmp(conditions[j].rhsAttr.relName, relName) != 0){		//���condition�Ҳ�ֵ����ȷ
				continue;
			}
			number++;
		}
		
		filter = new Condition[number];
		int index = -1;
		for (int j = 0; j < number; j++){
			if(strcmp(conditions[j].lhsAttr.relName, relName) != 0) {											//���condition���ֵ����ȷ
				continue;
			}
			if(conditions[j].bRhsIsAttr == TRUE && strcmp(conditions[j].rhsAttr.relName, relName) != 0){		//���condition�Ҳ�ֵ����ȷ
				continue;
			}
			index++;
			filter[index] = conditions[j];
		}
		
		if(chosenCond == NULL && (nConditions == 0 || nIndexes == 0)) {	//���û��Ҫ����߲�����relation�е�Ҫ��
			Condition cond = NULLCONDITION;
			RC status = -1;
			Iterator* it = NULL;
			it = smm.getList(relation);										//��ȡrelation�����е�attribute
			delete [] filters;
			delete [] attributes;
			return it;
		}
		
		//�������Ҫ������ɸѡ���Ҫ��
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
                                bool group, RelAttr groupAttr) const{					//�ڵ�ǰ������ѡ�������ֶ�
	
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
	//���relName�����Ը����Ƿ���value�и�����ͬ
	if (nValue != attrCount){
		cout << "Value number is incorrect" << endl;
		return 0;
	}
	
	//���value��ÿ�������Ƿ��relName��������ͬ
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
	//ɸѡ������Ҫ��condition����
	
	for(int j = 0; j < nConditions; j++){
			if(strcmp(conditions[j].lhsAttr.relName, relName) != 0) {											//���condition���ֵ����ȷ
				continue;
			}
			if(conditions[j].bRhsIsAttr == TRUE && strcmp(conditions[j].rhsAttr.relName, relName) != 0){		//���condition�Ҳ�ֵ����ȷ
				continue;
			}
			number++;
	}
		
	filter = new Condition[number];
	int index = -1;
	for (int j = 0; j < number; j++){
		if(strcmp(conditions[j].lhsAttr.relName, relName) != 0) {											//���condition���ֵ����ȷ
			continue;
		}
		if(conditions[j].bRhsIsAttr == TRUE && strcmp(conditions[j].rhsAttr.relName, relName) != 0){		//���condition�Ҳ�ֵ����ȷ
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
	//ɸѡ������Ҫ��condition����
	
	for(int j = 0; j < nConditions; j++){
			if(strcmp(conditions[j].lhsAttr.relName, relName) != 0) {											//���condition���ֵ����ȷ
				continue;
			}
			if(conditions[j].bRhsIsAttr == TRUE && strcmp(conditions[j].rhsAttr.relName, relName) != 0){		//���condition�Ҳ�ֵ����ȷ
				continue;
			}
			number++;
	}
		
	filter = new Condition[number];
	int index = -1;
	for (int j = 0; j < number; j++){
		if(strcmp(conditions[j].lhsAttr.relName, relName) != 0) {											//���condition���ֵ����ȷ
			continue;
		}
		if(conditions[j].bRhsIsAttr == TRUE && strcmp(conditions[j].rhsAttr.relName, relName) != 0){		//���condition�Ҳ�ֵ����ȷ
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
	//���ν�value������data��
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