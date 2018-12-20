#include "StdAfx.h"
#include "QU_Manager.h"

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

RC Select(int nSelAttrs,RelAttr **selAttrs,int nRelations,char **relations,int nConditions,Condition *conditions,SelResult * res)
{
	
	return SUCCESS;
}