#include "stdafx.h"
#include "RM_Manager.h"
#include "str.h"
#include "math.h"
#include "PF_Manager.h"

RC OpenScan(RM_FileScan *rmFileScan,RM_FileHandle *fileHandle,int conNum,Con *conditions)//≥ı ºªØ…®√Ë
{
	return SUCCESS;
}

RC GetNextRec(RM_FileScan *rmFileScan,RM_Record *rec)
{
	return SUCCESS;
}

RC GetRec (RM_FileHandle *fileHandle,RID *rid, RM_Record *rec)
{
	return SUCCESS;
}

RC InsertRec (RM_FileHandle *fileHandle,char *pData, RID *rid)
{
	return SUCCESS;
}

RC DeleteRec (RM_FileHandle *fileHandle,const RID *rid)
{
	return SUCCESS;
}

RC UpdateRec (RM_FileHandle *fileHandle,const RM_Record *rec)
{
	return SUCCESS;
}

RC RM_CreateFile (char *fileName, int recordSize)
{
	int recordnumperpage;
	recordnumperpage = 8*(sizeof(Page) - 4)/(8*recordSize + 1);
	if(recordnumperpage < 1) return RM_INVALIDRECSIZE;
	RC rc;
	rc = CreateFile(fileName);
	if(rc != SUCCESS) return rc;
	PF_FileHandle pffilehandle;
	rc = openFile(fileName,&pffilehandle);
	if(rc != SUCCESS) return rc;
	PF_PageHandle pfpagehandle;
	rc = AllocatePage(&pffilehandle,&pfpagehandle);
	if(rc != SUCCESS) return rc;
	char *pdata;
	rc = GetData(&pfpagehandle,&pdata);
	if(rc != SUCCESS) return rc;
	RM_FileSubHeader *rmfilesubheader;
	rmfilesubheader=(RM_FileSubHeader *)pdata;
	rmfilesubheader->nRecords = 0;
	rmfilesubheader->recordSize = recordSize;
	rmfilesubheader->recordsPerPage = recordnumperpage;
	rmfilesubheader->firstRecordOffset = 4 + ceil(recordnumperpage/8.0);
	memset(pdata+sizeof(RM_FileSubHeader),0,sizeof(Page)-4-sizeof(RM_FileSubHeader));
	rc = MarkDirty(&pfpagehandle);
	if(rc != SUCCESS) return rc;
	rc = UnpinPage(&pfpagehandle);
	if(rc != SUCCESS) return rc;
	rc = AllocatePage(&pffilehandle,&pfpagehandle);
	if(rc != SUCCESS) return rc;
	rc = MarkDirty(&pfpagehandle);
	if(rc != SUCCESS) return rc;
	rc = UnpinPage(&pfpagehandle);
	if(rc != SUCCESS) return rc;

	rc = CloseFile(&pffilehandle);
	if(rc != SUCCESS) return rc;
	return SUCCESS;
}
RC RM_OpenFile(char *fileName, RM_FileHandle *fileHandle)
{
	if(fileHandle->bOpen) return RM_FHOPENNED;
	RC rc;
	rc = openFile(fileName,&(fileHandle->pfFileHandle));
	if(rc != SUCCESS) return rc;

	rc = GetThisPage(&(fileHandle->pfFileHandle),1,&(fileHandle->pfpagehandle));
	if(rc != SUCCESS) return rc;
	char* pdata;
	rc = GetData(&(fileHandle->pfpagehandle),&pdata);
	if(rc != SUCCESS) return rc;
	fileHandle->bOpen=true;
	fileHandle->fileSubHeader=(RM_FileSubHeader *)pdata;
	fileHandle->pBitmap=pdata+sizeof(RM_FileSubHeader);
	return SUCCESS;
}

RC RM_CloseFile(RM_FileHandle *fileHandle)
{
    RC rc;
	if(fileHandle->bOpen == false) return RM_FHCLOSED;
	rc = UnpinPage(&(fileHandle->pfpagehandle));
	if(rc != SUCCESS) return rc;
	rc = CloseFile(&(fileHandle->pfFileHandle));
	if(rc != SUCCESS) return rc;
	fileHandle->bOpen = false;
	return SUCCESS;
}
