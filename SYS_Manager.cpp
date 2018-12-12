#include "stdafx.h"
#include "EditArea.h"
#include "SYS_Manager.h"
#include "QU_Manager.h"
#include <iostream>

table_node *tablesHead;
col_node *columnsHead;
table_node *tableTail;
col_node *columsTail;

RM_FileHandle *rf;

void ExecuteAndMessage(char * sql,CEditArea* editArea){//����ִ�е���������ڽ�������ʾִ�н�����˺������޸�
	std::string s_sql = sql;
	if(s_sql.find("select") == 0){
		SelResult res;
		Init_Result(&res);
		//rc = Query(sql,&res);
		//����ѯ�������һ�£����������������ʽ
		//����editArea->ShowSelResult(col_num,row_num,fields,rows);
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
		messages[0] = "�����ɹ�";
		editArea->ShowMessage(row_num,messages);
		delete[] messages;
		break;
	case SQL_SYNTAX:
		row_num = 1;
		messages = new char*[row_num];
		messages[0] = "���﷨����";
		editArea->ShowMessage(row_num,messages);
		delete[] messages;
		break;
	default:
		row_num = 1;
		messages = new char*[row_num];
		messages[0] = "����δʵ��";
		editArea->ShowMessage(row_num,messages);
	delete[] messages;
		break;
	}
}

RC execute(char * sql){
	sqlstr *sql_str = NULL;
	RC rc;
	sql_str = get_sqlstr();
  	rc = parse(sql, sql_str);//ֻ�����ַ��ؽ��SUCCESS��SQL_SYNTAX
	
	if (rc == SUCCESS)
	{
		int i = 0;
		switch (sql_str->flag)
		{
			//case 1:
			////�ж�SQL���Ϊselect���

			//break;

			case 2:
			//�ж�SQL���Ϊinsert���

			case 3:	
			//�ж�SQL���Ϊupdate���
			break;

			case 4:					
			//�ж�SQL���Ϊdelete���
			break;

			case 5:
			//�ж�SQL���ΪcreateTable���

			break;

			case 6:	
			//�ж�SQL���ΪdropTable���
			break;

			case 7:
			//�ж�SQL���ΪcreateIndex���
			break;
	
			case 8:	
			//�ж�SQL���ΪdropIndex���
			break;
			
			case 9:
			//�ж�Ϊhelp��䣬���Ը���������ʾ
			break;
		
			case 10: 
			//�ж�Ϊexit��䣬�����ɴ˽����˳�����
			break;		
		}
	}else{
		AfxMessageBox(sql_str->sstr.errors);//���������sql���ʷ�����������Ϣ
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
	RC ret = RM_CreateFile(tables, 25);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to create system files");
		return SQL_SYNTAX;
	}
	/*FILE *fp = fopen(tables, "w");
	if(fp == NULL)
	{
		AfxMessageBox("failed to create system files");
		return SQL_SYNTAX;
	}
	fclose(fp);*/
	strcpy(cols, path);
	strcat(cols, "\\SYSCOLUMNS");
	ret = RM_CreateFile(cols, 72);
	if(ret != SUCCESS)
	{
		AfxMessageBox("failed to create system files");
		return SQL_SYNTAX;
	}
	/*fp = fopen(cols, "w");
	if(fp == NULL)
	{
		AfxMessageBox("failed to create system files");
		return SQL_SYNTAX;
	}
	fclose(fp);*/
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
	RM_OpenFile(dbname, rf);
	return SUCCESS;
}

RC CloseDB(){
	RM_CloseFile(rf);
	return SUCCESS;
}

RC CreateTable(char *relName,int attrCount,AttrInfo *attributes)
{
	/*table_node newTable;
	strcpy(newTable.tablename, relName);
	newTable.attrcount = attrCount;
	tables.push_back(newTable);*/
	return SUCCESS;
}
RC DropTable(char *relName);
RC CreateIndex(char *indexName,char *relName,char *attrName);
RC DropIndex(char *indexName);
RC Insert(char *relName,int nValues,Value * values);
RC Delete(char *relName,int nConditions,Condition *conditions);
RC Update(char *relName,char *attrName,Value *value,int nConditions,Condition *conditions);
bool CanButtonClick(){//��Ҫ����ʵ��
	//�����ǰ�����ݿ��Ѿ���
	return true;
	//�����ǰû�����ݿ��
	//return false;
}
