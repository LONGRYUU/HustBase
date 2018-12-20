#include "StdAfx.h"
#include "QU_Manager.h"

extern RM_FileHandle trfh;
extern RM_FileHandle crfh;

void Init_Result(SelResult * res){
	res->next_res = NULL;
}

void Destory_Result(SelResult * res){
	for(int i = 0;i<res->row_num;i++){
		for(int j = 0;j<res->col_num;j++){
			delete[] res->res[i][j];
		}
		delete[] res->res[i];
	}
	if(res->next_res != NULL){
		Destory_Result(res->next_res);
	}
}

RC Query(char * sql,SelResult * res){
	sqlstr *sql_str = NULL;
	RC rc;
	sql_str = get_sqlstr();
	rc = parse(sql, sql_str);
	if(rc == SUCCESS)
	{
		if(sql_str->flag)
		{
			Select(sql_str->sstr.sel.nSelAttrs, sql_str->sstr.sel.selAttrs, sql_str->sstr.sel.nRelations, sql_str->sstr.sel.relations, sql_str->sstr.sel.nConditions,sql_str->sstr.sel.conditions, res);
		}
	}
	return SUCCESS;
}

int isIndex(char *relName, char *attrName, char *indexName)
{
	RM_FileHandle crfh;
	RC ret = RM_OpenFile("SYSCOLUMNS", &crfh);
	RM_FileScan rfs;
	Con *cons = (Con *)malloc(sizeof(Con) * 2);
	cons[0].bLhsIsAttr = 1;
	cons[0].bRhsIsAttr = 0;
	cons[0].attrType = chars;
	cons[0].LattrLength = 21;
	cons[0].LattrOffset = 0;
	cons[0].compOp = EQual;
	cons[0].Rvalue = relName;
	cons[1].bLhsIsAttr = 1;
	cons[1].bRhsIsAttr = 0;
	cons[1].attrType = chars;
	cons[1].LattrLength = 21;
	cons[1].LattrOffset = 21;
	cons[1].compOp = EQual;
	cons[1].Rvalue = attrName;
	ret = OpenScan(&rfs, &crfh, 2, cons);
	RM_Record rec;
	while(GetNextRec(&rfs, &rec) != RM_EOF)
	{
		free(cons);
		if(rec.pData[54] == '1')
		{
			strcpy(indexName, rec.pData + 55);
			return 1;
		}
		else return 0;
	}
	free(cons);
	return 0;
}



RC Select(int nSelAttrs,RelAttr **selAttrs,int nRelations,char **relations,int nConditions,Condition *conditions,SelResult * res)
{
	RidList *head = NULL;
	RidList *tail = head;
	int num = 0;
	if(nRelations == 1)
	{
		RM_FileHandle rfh;
		RM_OpenFile(relations[0], &rfh);
		for(int i = 0; i < nConditions; i++)
		{
			char indexName[21];
			if(conditions[i].bLhsIsAttr && !conditions[i].bRhsIsAttr)
			{
				if(isIndex(conditions[i].lhsAttr.relName, conditions[i].lhsAttr.attrName, indexName))
				{
					IX_IndexHandle ih;
					IX_IndexScan is;
					OpenIndex(conditions[i].lhsAttr.relName, &ih);
					OpenIndexScan(&is, &ih, conditions[i].op, (char*)conditions[i].rhsValue.data);
					RID rid;
					RM_Record rec;
					while(IX_GetNextEntry(&is, &rid))
					{
						GetRec(&rfh, &rid, &rec);
						
					}
				}
			}
			else
			{

			}
		}
	}
	else
	{

	}
	// select from more than one tables;
	return SUCCESS;
}

