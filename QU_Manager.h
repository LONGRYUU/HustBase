#ifndef __QUERY_MANAGER_H_
#define __QUERY_MANAGER_H_
#include "str.h"
#include "RM_Manager.h"
#include "IX_Manager.h"

typedef struct SelResult{
	int col_num;
	int row_num;
	AttrType type[20];	//结果集各字段的数据类型
	int length[20];		//结果集各字段值的长度
	char fields[20][20];//最多二十个字段名，而且每个字段的长度不超过20
	char ** res[100];	//最多一百条记录
	SelResult * next_res;
}SelResult;

typedef struct RidList
{
	RID *rid;
	RidList *next;
}RidList;

void Init_Result(SelResult * res);
void Destory_Result(SelResult * res);
extern "C"
{
	void scanCols(char *relName, char *attrName, int* len, AttrType* type, int *offset);
	void Condi2Con(int n, Condition* conditions, Con* cons);
}
RC Query(char * sql,SelResult * res);

RC Select(int nSelAttrs,RelAttr **selAttrs,int nRelations,char **relations,int nConditions,Condition *conditions,SelResult * res);
#endif