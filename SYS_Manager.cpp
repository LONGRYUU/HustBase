#include "stdafx.h"
#include "EditArea.h"
#include "SYS_Manager.h"
#include "QU_Manager.h"
#include <iostream>

RM_FileHandle *trfh = NULL;
RM_FileHandle *crfh = NULL;

void ExecuteAndMessage(char * sql,CEditArea* editArea){//根据执行的语句类型在界面上显示执行结果。此函数需修改
	std::string s_sql = sql;
	if(s_sql.find("select") == 0){
		SelResult res;
		Init_Result(&res);
		//rc = Query(sql,&res);
		//将查询结果处理一下，整理成下面这种形式
		//调用editArea->ShowSelResult(col_num,row_num,fields,rows);
		int col_num = 5;
		int row_num = 3;
		char ** fields = new char *[5];
		for(int i = 0;i<col_num;i++){
			fields[i] = new char[20];
			memset(fields[i],0,20);
			fields[i][0] = 'f';
			fields[i][1] = i+'0';
		}
		char *** rows = new char**[row_num];
		for(int i = 0;i<row_num;i++){
			rows[i] = new char*[col_num];
			for(int j = 0;j<col_num;j++){
				rows[i][j] = new char[20];
				memset(rows[i][j],0,20);
				rows[i][j][0] = 'r';
				rows[i][j][1] = i + '0';
				rows[i][j][2] = '+';
				rows[i][j][3] = j + '0';
			}
		}
		editArea->ShowSelResult(col_num,row_num,fields,rows);
		for(int i = 0;i<5;i++){
			delete[] fields[i];
		}
		delete[] fields;
		Destory_Result(&res);
		return;
	}
	RC rc = execute(sql);
	int row_num = 0;
	char**messages;
	switch(rc){
	case SUCCESS:
		row_num = 1;
		messages = new char*[row_num];
		messages[0] = "操作成功";
		editArea->ShowMessage(row_num,messages);
		delete[] messages;
		break;
	case SQL_SYNTAX:
		row_num = 1;
		messages = new char*[row_num];
		messages[0] = "有语法错误";
		editArea->ShowMessage(row_num,messages);
		delete[] messages;
		break;
	default:
		row_num = 1;
		messages = new char*[row_num];
		messages[0] = "功能未实现";
		editArea->ShowMessage(row_num,messages);
	delete[] messages;
		break;
	}
}

RC execute(char * sql){
	sqlstr *sql_str = NULL;
	RC rc;
	sql_str = get_sqlstr();
  	rc = parse(sql, sql_str);//只有两种返回结果SUCCESS和SQL_SYNTAX
	
	if (rc == SUCCESS)
	{
		int i = 0;
		switch (sql_str->flag)
		{
			//case 1:
			////判断SQL语句为select语句

			//break;

			case 2:
			//判断SQL语句为insert语句

			case 3:	
			//判断SQL语句为update语句
			break;

			case 4:					
			//判断SQL语句为delete语句
			break;

			case 5:
			//判断SQL语句为createTable语句

			break;

			case 6:	
			//判断SQL语句为dropTable语句
			break;

			case 7:
			//判断SQL语句为createIndex语句
			break;
	
			case 8:	
			//判断SQL语句为dropIndex语句
			break;
			
			case 9:
			//判断为help语句，可以给出帮助提示
			break;
		
			case 10: 
			//判断为exit语句，可以由此进行退出操作
			break;		
		}
	}else{
		AfxMessageBox(sql_str->sstr.errors);//弹出警告框，sql语句词法解析错误信息
		return rc;
	}
}

RC CreateDB(char *dbpath,char *dbname){
	char path[256];
	char tables[256];
	char cols[256];
	//create dir
	strcpy(path, dbpath);
	strcat(path, "\\");
	strcat(path, dbname); 
	int ret;
	ret = mkdir(path);
	if(ret == -1)
	{
		AfxMessageBox("failed to create new database");
		return SQL_SYNTAX;
	}
	//create system files
	strcpy(tables, path);
	strcat(tables, "\\SYSTABLES");
	strcpy(cols, path);
	strcat(cols, "\\SYSCOLUMNS");
	
	RC ret = RM_CreateFile(tables, 25);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to create system files");
		return SQL_SYNTAX;
	}

	ret = RM_CreateFile(cols, 72);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to create system files");
		return SQL_SYNTAX;
	}
	return SUCCESS;
}

RC DropDB(char *dbname){
	char cmd[256];
	strcpy(cmd, "rd /s /q ");
	strcat(cmd, dbname);
	system(cmd);
	return SUCCESS;
}

RC OpenDB(char *dbname){
	char path[256];
	strcpy(path, dbname);
	strcat(path, "\\SYSTABLES");
	RC ret = RM_OpenFile(dbname, trfh);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to open database systables");
		return SQL_SYNTAX;
	}
	strcpy(path, dbname);
	strcat("\\SYSCOLUMNS");
	ret = RM_OpenFile(dbname, crfh);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to open database syscolumns");
		return SQL_SYNTAX;
	}
	return SUCCESS;
}

RC CloseDB(){
	RC ret = RM_CloseFile(trfh);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to close database systables");
		return SQL_SYNTAX;
	}
	ret = RM_CloseFile(crfh);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to close database syscolumns");
		return SQL_SYNTAX;
	}
	return SUCCESS;
}

RC CreateTable(char *relName,int attrCount,AttrInfo *attributes)
{
	if(trfh == NULL || crfh == NULL || trfh->bOpen == false || crfh->bOpen == false)
	{
		AfxMessageBox("please open a database first");
		return SQL_SYNTAX;
	}
	if(IsCreated(relName))
	{
		AfxMessageBox("table already created");
		return SQL_SYNTAX;
	}

	//update SYSTABLES;
	char tableRec[25];
	RID rid;
	strcpy(tableRec, relName);
	sprintf(tableRec + 21, "%d", attrCount);
	RC ret = InsertRec(trfh, tableRec, &rid);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to update SYSTABLES");
		return SQL_SYNTAX;
	}
	//update SYSCOLUMNS
	int i = 0;
	int offset = 0;
	while(i < attrCount)
	{
		char columnRec[72];
		strcpy(columnRec, relName);
		strcpy(columnRec + 21, attributes[i].attrName);
		sprintf(columnRec + 42, "%d", attributes[i].attrType);
		sprintf(columnRec + 46, "%d", attributes[i].attrLength);
		sprintf(columnRec + 50, "%d", offset);
		columnRec[54] = '0';
		ret = InsertRec(crfh, columnRec, &rid);
		if(ret != SUCCESS)
		{
			AfxMessageBox("failed to update SYSCOLUMNS");
			return SQL_SYNTAX;
		}
		offset += attributes[i].attrLength;
	}
	//create table record file;
	ret = RM_CreateFile(relName, offset);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to create table file");
		return SQL_SYNTAX;
	}
	return SUCCESS;
}
RC DropTable(char *relName)
{
	//get rid;
	RM_FileScan rfs;

	RC ret = OpenScan(&rfs, trfh, 0, NULL);

	RM_Record rr;
	ret = GetNextRec(&rfs, &rr);
	while(true)
	{
		
	}

	// update SYSTABLES;
	ret = DeleteRec(trfh, rr.rid);	

	// update SYSCOLUMNS;
	OpenScan(&rfs, crfh, 0, NULL);
	while(true)
	{
		ret = GetNextRec(&rfs, &rr);
		if(ret != SUCCESS) break;
	}	

	//delete table record file;
	char cmd[256];
	strcpy(cmd, "del ");
	strcat(cmd, relName);
	system(cmd);
	//delete indices;
	return SUCCESS;
}
RC CreateIndex(char *indexName,char *relName,char *attrName)
{

}
RC DropIndex(char *indexName)
{

}
RC Insert(char *relName,int nValues,Value * values)
{
	//scan and check existence;

	//insert record;
	
	//build index;

}
RC Delete(char *relName,int nConditions,Condition *conditions)
{

}
RC Update(char *relName,char *attrName,Value *value,int nConditions,Condition *conditions)
{

}
bool CanButtonClick(){//需要重新实现
	//如果当前有数据库已经打开
	if(trfh != NULL && crfh != NULL)
		return true;
	//如果当前没有数据库打开
	else return false;
}

bool IsCreated(char *path)
{
	int ret = _access(path, 0);
	if(ret == 0) return true;
	return false;
}