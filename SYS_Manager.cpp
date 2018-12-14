#include "stdafx.h"
#include "EditArea.h"
#include "SYS_Manager.h"
#include "QU_Manager.h"
#include <iostream>

RM_FileHandle trfh;
RM_FileHandle crfh;

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
	
	RC res = RM_CreateFile(tables, 25);
	if(res != SUCCESS)
	{
		AfxMessageBox("failed to create system files");
		return SQL_SYNTAX;
	}

	res = RM_CreateFile(cols, 76);
	if(res != SUCCESS)
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
	RC ret = RM_OpenFile(dbname, &trfh);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to open database systables");
		return SQL_SYNTAX;
	}
	strcpy(path, dbname);
	strcat(path, "\\SYSCOLUMNS");
	ret = RM_OpenFile(dbname, &crfh);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to open database syscolumns");
		return SQL_SYNTAX;
	}
	return SUCCESS;
}

RC CloseDB(){
	RC ret = RM_CloseFile(&trfh);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to close database systables");
		return SQL_SYNTAX;
	}
	ret = RM_CloseFile(&crfh);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to close database syscolumns");
		return SQL_SYNTAX;
	}
	return SUCCESS;
}

RC CreateTable(char *relName,int attrCount,AttrInfo *attributes)
{
	if(trfh.bOpen == false || crfh.bOpen == false)
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
	RC ret = InsertRec(&trfh, tableRec, &rid);
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
		char columnRec[76];
		strcpy(columnRec, relName);
		strcpy(columnRec + 21, attributes[i].attrName);
		sprintf(columnRec + 42, "%d", attributes[i].attrType);
		sprintf(columnRec + 46, "%d", attributes[i].attrLength);
		sprintf(columnRec + 50, "%d", offset);
		columnRec[54] = '0';
		ret = InsertRec(&crfh, columnRec, &rid);
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
	// update SYSTABLES;
	RM_FileScan rfs;

	RC ret = OpenScan(&rfs, &trfh, 0, NULL);

	RM_Record rr;
	bool flag = false;
	while(GetNextRec(&rfs, &rr) != EOF)
	{
		if(strstr(rr.pData, relName) == rr.pData)
		{
			DeleteRec(&trfh, &(rr.rid));
			flag = true;
			break;
		}
	}
	if(!flag)
	{
		AfxMessageBox("Table Not Exist");
		return TABLE_NOT_EXIST;
	}
	// update SYSCOLUMNS;
	OpenScan(&rfs, &crfh, 0, NULL);
	while(GetNextRec(&rfs, &rr) != RM_EOF)
	{
		if(strstr(rr.pData, relName) == rr.pData)
		{
			if(rr.pData[54] == '1')
				DropIndex(&(rr.pData[55]));
			DeleteRec(&crfh, &(rr.rid));
		}
	}	

	//delete table record file;
	char cmd[256];
	strcpy(cmd, "del ");
	strcat(cmd, relName);
	system(cmd);
	return SUCCESS;
}
RC CreateIndex(char *indexName,char *relName,char *attrName)
{
	/*char fileName[256];
	sprintf(fileName, "%s_%s", relName, indexName);
	//get attr info;
	RM_FileScan rfs;
	RM_Record rec;
	RC ret = OpenScan(&rfs, crfh, 0, NULL);
	while(GetNextRec(&rfs, &rec) != RM_EOF)
	{
		if((strstr(rec.pData, relName) == rec.pData) && (strstr(rec.pData + 21, attrName) == (rec.pData + 21)))
		{
			//update syscolumns;
			rec.pData[55] = '1';
			strcpy(rec.pData + 55, attrName);
			UpdateRec(crfh, &rec);
			break;
		}
	}
	AttrType attrType;
	int attrLength;
	sscanf(rec.pData + 42, "%d%d", &attrType, &attrLength);
	CreateIndex(fileName, attrType, attrLength);*/
	return SUCCESS;
}
RC DropIndex(char *indexName)
{
	return SUCCESS;
}
RC Insert(char *relName,int nValues,Value * values)
{
	//scan and check existence;

	//insert record;
	
	//build index;
	return SUCCESS;
}
RC Delete(char *relName,int nConditions,Condition *conditions)
{
	return SUCCESS;
}
RC Update(char *relName,char *attrName,Value *value,int nConditions,Condition *conditions)
{
	return SUCCESS;
}
bool CanButtonClick(){//需要重新实现
	//如果当前有数据库已经打开
	if(trfh.bOpen && crfh.bOpen)
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

void HandleError(RC ret)
{
	
	switch (ret)
	{
		case SUCCESS:
			/* code */
			break;
		case SQL_SYNTAX:
			break;
	/*SUCCESS,
	SQL_SYNTAX,
	PF_EXIST,
	PF_FILEERR,
	PF_INVALIDNAME,
	PF_WINDOWS,
	PF_FHCLOSED,
	PF_FHOPEN,
	PF_PHCLOSED,
	PF_PHOPEN,
	PF_NOBUF,
	PF_EOF,
	PF_INVALIDPAGENUM,
	PF_NOTINBUF,
	PF_PAGEPINNED,
	RM_FHCLOSED,
	RM_FHOPENNED,
	RM_INVALIDRECSIZE,
	RM_INVALIDRID,
	RM_FSCLOSED,
	RM_NOMORERECINMEM,
	RM_FSOPEN,
	IX_IHOPENNED,
	IX_IHCLOSED,
	IX_INVALIDKEY,
	IX_NOMEM,
	RM_NOMOREIDXINMEM,
	IX_EOF,
	IX_SCANCLOSED,
	IX_ISCLOSED,
	IX_NOMOREIDXINMEM,
	IX_SCANOPENNED,
	FAIL,

	DB_EXIST,
	DB_NOT_EXIST,
	NO_DB_OPENED,

	TABLE_NOT_EXIST,
	TABLE_EXIST,
	TABLE_NAME_ILLEGAL,

	FLIED_NOT_EXIST,//在不存在的字段上增加索引
	FIELD_NAME_ILLEGAL,
	FIELD_MISSING,//插入的时候字段不足
	FIELD_REDUNDAN,//插入的时候字段太多
	FIELD_TYPE_MISMATCH,//字段类型有误

	RECORD_NOT_EXIST,//对一条不存在的记录进行删改时

	INDEX_NAME_REPEAT,
	INDEX_EXIST,//在指定字段上，已经存在索引了
	INDEX_NOT_EXIST*/	
		default:
			break;
	}
}