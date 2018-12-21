#include "stdafx.h"
#include "RM_Manager.h"
#include "str.h"
#include "math.h"
#include "PF_Manager.h"

RC OpenScan(RM_FileScan *rmFileScan,RM_FileHandle *fileHandle,int conNum,Con *conditions)//³õÊ¼»¯É¨Ãè
{
    if(rmFileScan->bOpen) return RM_FSOPEN;
    rmFileScan->bOpen = true;
	rmFileScan->pRMFileHandle = fileHandle;
	rmFileScan->conNum = conNum;
	if(conNum == 0)
		rmFileScan->conditions = NULL;
	else
	{
		rmFileScan->conditions = (Con *)malloc(conNum * sizeof(Con));
		memcpy(rmFileScan->conditions,conditions,conNum * sizeof(Con));
	}
	rmFileScan->PageHandle = fileHandle->pfpagehandle;
	rmFileScan->pn = 2;
	rmFileScan->sn = 0;
	return SUCCESS;
}

//判断查询条件是否满足
bool ConditionJudge(RM_FileScan *rmFileScan,char *pRec)
{
	int i,i1=0,i2=0,i3;
	float f1=0,f2=0;
	char *s1=NULL,*s2=NULL;
	if(rmFileScan->conNum == 0)
		return true;
	for(i=0;i<rmFileScan->conNum;i++)
	{
        i3 = 0;
		switch(rmFileScan->conditions[i].compOp)
		{
			case EQual:
				switch(rmFileScan->conditions[i].attrType)
				{
                    case chars:
                        if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            s1 = pRec + rmFileScan->conditions[i].LattrOffset;
                        else
                            s1 = (char *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            s2 = pRec + rmFileScan->conditions[i].RattrOffset;
                        else
                            s2 = (char *)rmFileScan->conditions[i].Rvalue;
                        if(strcmp(s1,s2) == 0)
                            i3 = 1;
                        break;
					case ints:
						if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            memcpy(&i1,pRec + rmFileScan->conditions[i].LattrOffset,sizeof(int));
                        else
                            i1 = *(int *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            memcpy(&i2,pRec + rmFileScan->conditions[i].RattrOffset,sizeof(int));
                        else
                            i2 = *(int *)rmFileScan->conditions[i].Rvalue;
                        if(i1 == i2)
                            i3 = 1;
						break;
					case floats:
					    if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            memcpy(&f1,pRec + rmFileScan->conditions[i].LattrOffset,sizeof(float));
                        else
                            f1 = *(float *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            memcpy(&f2,pRec+rmFileScan->conditions[i].RattrOffset,sizeof(float));
                        else
                            f2 = *(float *)rmFileScan->conditions[i].Rvalue;
                        if(f1 == f2)
                            i3 = 1;
						break;
                }
				break;
			case LessT:
				switch(rmFileScan->conditions[i].attrType)
				{
					case chars:
                        if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            s1 = pRec + rmFileScan->conditions[i].LattrOffset;
                        else
                            s1 = (char *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            s2 = pRec + rmFileScan->conditions[i].RattrOffset;
                        else
                            s2 = (char *)rmFileScan->conditions[i].Rvalue;
                        if(strcmp(s1,s2) < 0)
                            i3 = 1;
                        break;
					case ints:
						if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            memcpy(&i1,pRec + rmFileScan->conditions[i].LattrOffset,sizeof(int));
                        else
                            i1 = *(int *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            memcpy(&i2,pRec + rmFileScan->conditions[i].RattrOffset,sizeof(int));
                        else
                            i2 = *(int *)rmFileScan->conditions[i].Rvalue;
                        if(i1 < i2)
                            i3 = 1;
						break;
					case floats:
					    if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            memcpy(&f1,pRec + rmFileScan->conditions[i].LattrOffset,sizeof(float));
                        else
                            f1 = *(float *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            memcpy(&f2,pRec+rmFileScan->conditions[i].RattrOffset,sizeof(float));
                        else
                            f2 = *(float *)rmFileScan->conditions[i].Rvalue;
                        if(f1 < f2)
                            i3 = 1;
						break;
                }
				break;
			case GreatT:
				switch(rmFileScan->conditions[i].attrType)
				{
					case chars:
                        if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            s1 = pRec + rmFileScan->conditions[i].LattrOffset;
                        else
                            s1 = (char *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            s2 = pRec + rmFileScan->conditions[i].RattrOffset;
                        else
                            s2 = (char *)rmFileScan->conditions[i].Rvalue;
                        if(strcmp(s1,s2) > 0)
                            i3 = 1;
                        break;
					case ints:
						if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            memcpy(&i1,pRec + rmFileScan->conditions[i].LattrOffset,sizeof(int));
                        else
                            i1 = *(int *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            memcpy(&i2,pRec + rmFileScan->conditions[i].RattrOffset,sizeof(int));
                        else
                            i2 = *(int *)rmFileScan->conditions[i].Rvalue;
                        if(i1 > i2)
                            i3 = 1;
						break;
					case floats:
					    if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            memcpy(&f1,pRec + rmFileScan->conditions[i].LattrOffset,sizeof(float));
                        else
                            f1 = *(float *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            memcpy(&f2,pRec+rmFileScan->conditions[i].RattrOffset,sizeof(float));
                        else
                            f2 = *(float *)rmFileScan->conditions[i].Rvalue;
                        if(f1 > f2)
                            i3 = 1;
						break;
                }
				break;
			case LEqual:
				switch(rmFileScan->conditions[i].attrType)
				{
					case chars:
                        if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            s1 = pRec + rmFileScan->conditions[i].LattrOffset;
                        else
                            s1 = (char *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            s2 = pRec + rmFileScan->conditions[i].RattrOffset;
                        else
                            s2 = (char *)rmFileScan->conditions[i].Rvalue;
                        if(strcmp(s1,s2) <= 0)
                            i3 = 1;
                        break;
					case ints:
						if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            memcpy(&i1,pRec + rmFileScan->conditions[i].LattrOffset,sizeof(int));
                        else
                            i1 = *(int *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            memcpy(&i2,pRec + rmFileScan->conditions[i].RattrOffset,sizeof(int));
                        else
                            i2 = *(int *)rmFileScan->conditions[i].Rvalue;
                        if(i1 <= i2)
                            i3 = 1;
						break;
					case floats:
					    if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            memcpy(&f1,pRec + rmFileScan->conditions[i].LattrOffset,sizeof(float));
                        else
                            f1 = *(float *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            memcpy(&f2,pRec+rmFileScan->conditions[i].RattrOffset,sizeof(float));
                        else
                            f2 = *(float *)rmFileScan->conditions[i].Rvalue;
                        if(f1 <= f2)
                            i3 = 1;
						break;
                }
				break;
			case GEqual:
				switch(rmFileScan->conditions[i].attrType)
				{
					case chars:
                        if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            s1 = pRec + rmFileScan->conditions[i].LattrOffset;
                        else
                            s1 = (char *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            s2 = pRec + rmFileScan->conditions[i].RattrOffset;
                        else
                            s2 = (char *)rmFileScan->conditions[i].Rvalue;
                        if(strcmp(s1,s2) >= 0)
                            i3 = 1;
                        break;
					case ints:
						if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            memcpy(&i1,pRec + rmFileScan->conditions[i].LattrOffset,sizeof(int));
                        else
                            i1 = *(int *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            memcpy(&i2,pRec + rmFileScan->conditions[i].RattrOffset,sizeof(int));
                        else
                            i2 = *(int *)rmFileScan->conditions[i].Rvalue;
                        if(i1 >= i2)
                            i3 = 1;
						break;
					case floats:
					    if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            memcpy(&f1,pRec + rmFileScan->conditions[i].LattrOffset,sizeof(float));
                        else
                            f1 = *(float *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            memcpy(&f2,pRec+rmFileScan->conditions[i].RattrOffset,sizeof(float));
                        else
                            f2 = *(float *)rmFileScan->conditions[i].Rvalue;
                        if(f1 >= f2)
                            i3 = 1;
						break;
                }
				break;
			case NEqual:
				switch(rmFileScan->conditions[i].attrType)
				{
					case chars:
                        if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            s1 = pRec + rmFileScan->conditions[i].LattrOffset;
                        else
                            s1 = (char *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            s2 = pRec + rmFileScan->conditions[i].RattrOffset;
                        else
                            s2 = (char *)rmFileScan->conditions[i].Rvalue;
                        if(strcmp(s1,s2) != 0)
                            i3 = 1;
                        break;
					case ints:
						if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            memcpy(&i1,pRec + rmFileScan->conditions[i].LattrOffset,sizeof(int));
                        else
                            i1 = *(int *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            memcpy(&i2,pRec + rmFileScan->conditions[i].RattrOffset,sizeof(int));
                        else
                            i2 = *(int *)rmFileScan->conditions[i].Rvalue;
                        if(i1 != i2)
                            i3 = 1;
						break;
					case floats:
					    if(rmFileScan->conditions[i].bLhsIsAttr == 1)
                            memcpy(&f1,pRec + rmFileScan->conditions[i].LattrOffset,sizeof(float));
                        else
                            f1 = *(float *)rmFileScan->conditions[i].Lvalue;
                        if(rmFileScan->conditions[i].bRhsIsAttr == 1)
                            memcpy(&f2,pRec+rmFileScan->conditions[i].RattrOffset,sizeof(float));
                        else
                            f2 = *(float *)rmFileScan->conditions[i].Rvalue;
                        if(f1 != f2)
                            i3 = 1;
						break;
                }
				break;
		}
		if(i3 == 0)
			return false;
	}
	return true;
}

RC GetNextRec(RM_FileScan *rmFileScan,RM_Record *rec)
{
	if(rmFileScan->bOpen == false)
		return RM_FSCLOSED;
    //记录文件总记录数
    //rmFileScan->pRMFileHandle->fileSubHeader->nRecords
    //每页可以装载的记录数量
    //rmFileScan->pRMFileHandle->fileSubHeader->recordsPerPage
    int i,recmaxpage;
    PF_PageHandle pfpagehandle;
    RID judgerid;
    RM_Record rmrec;
    RC rc;
    char *prec;
    int lastpagerecnum;
    lastpagerecnum = (rmFileScan->pRMFileHandle->fileSubHeader->nRecords)%(rmFileScan->pRMFileHandle->fileSubHeader->recordsPerPage);
    recmaxpage = ceil((rmFileScan->pRMFileHandle->fileSubHeader->nRecords)/(double)(rmFileScan->pRMFileHandle->fileSubHeader->recordsPerPage));
    rc = GetThisPage(&(rmFileScan->pRMFileHandle->pfFileHandle),rmFileScan->pn,&pfpagehandle);
    if(rc != SUCCESS) return rc;
    while(rmFileScan->pn <= (recmaxpage + 1))
	{
		if(rmFileScan->sn == rmFileScan->pRMFileHandle->fileSubHeader->recordsPerPage)
		{
		    rc = UnpinPage(&pfpagehandle);
		    if(rc != SUCCESS) return rc;
		    rmFileScan->sn = 0;
		    rmFileScan->pn++;
            rc = GetThisPage(&(rmFileScan->pRMFileHandle->pfFileHandle),rmFileScan->pn,&pfpagehandle);
            if(rc != SUCCESS) return rc;
		}
		//判断该pn页的记录是否满足条件
		if(rmFileScan->pn == (recmaxpage + 1))  //如果是最后一页记录
        {
            for( ,rmFileScan->sn < lastpagerecnum,rmFileScan->sn++)
            {
                judgerid->bValid = true;
                judgerid->pageNum = rmFileScan->pn;
                judgerid->slotNum = rmFileScan->sn;
                rc = GetRec(rmFileScan->pRMFileHandle,&judgerid,&rmrec);
                if(rc != SUCCESS) return rc;
                memcpy(prec,rmrec->pData,rmFileScan->pRMFileHandle->fileSubHeader->recordSize);
                if(ConditionJudge(rmFileScan,prec) == true) //满足条件，取出记录
                {
                    rmFileScan->sn++;
                    rec->bValid = true;
                    rec->rid = judgerid;
                    memcpy(rec->pData,prec,rmFileScan->pRMFileHandle->fileSubHeader->recordSize);
                    return SUCCESS;
                }
            }
            return RM_EOF;   //需要添加在rc.h头文件中
        }
		for( ,rmFileScan->sn<rmFileScan->pRMFileHandle->fileSubHeader->recordsPerPage,rmFileScan->sn++)//非最后一页记录
        {
            judgerid->bValid = true;
            judgerid->pageNum = rmFileScan->pn;
            judgerid->slotNum = rmFileScan->sn;
            rc = GetRec(rmFileScan->pRMFileHandle,&judgerid,&rmrec);
            if(rc != SUCCESS) return rc;
            memcpy(prec,rmrec->pData,rmFileScan->pRMFileHandle->fileSubHeader->recordSize);
            if(ConditionJudge(rmFileScan,prec) == true) //满足条件，取出记录
            {
                rmFileScan->sn++;
                rec->bValid = true;
                rec->rid = judgerid;
                memcpy(rec->pData,prec,rmFileScan->pRMFileHandle->fileSubHeader->recordSize);
                return SUCCESS;
            }
        }

    }
}

RC GetRec (RM_FileHandle *fileHandle,RID *rid, RM_Record *rec)
{
	char *pBitmap,*poffset;
	RC rc;
	if(fileHandle->bOpen == false) return RM_FHCLOSED;
	PF_PageHandle pfpagehandle;
	rc = GetThisPage(&(fileHandle->pfFileHandle),rid->pageNum,&pfpagehandle);
	if(rc != SUCCESS) return rc;
	rc = GetData(&pfpagehandle,&pBitmap);
	if(rc != SUCCESS) return rc;
	int byte bit;
	byte = rid->slotNum/8;
	bit = rid->slotNum%8;
	if(pBitmap[byte]&(1<<bit) == 0) return RM_INVALIDRID;
	poffset = pBitmap + fileHandle->fileSubHeader->firstRecordOffset + (fileHandle->fileSubHeader->recordSize)*rid->slotNum;
	rec->pData = (char *)malloc(fileHandle->fileSubHeader->recordSize);
	memcpy(rec->pData,poffset,fileHandle->fileSubHeader->recordSize);
	rec->rid.pageNum = rid->pageNum;
	rec->rid.slotNum = rid->slotNum;
	rec->bValid = true;
	rc = UnpinPage(&pfpagehandle);
	if(rc != SUCCESS) return rc;
	return SUCCESS;
}

RC InsertRec (RM_FileHandle *fileHandle,char *pData, RID *rid)
{
    PageNum pageNum;
	PageNum pageCount = fileHandle->pfFileHandle.pFileSubHeader->pageCount;
	SlotNum slotNum;
	int byte,bit;
	char *poffset,*pBitmap,*pdata;
	if(fileHandle->bOpen == false) return RM_FHCLOSED;
	for(pageNum = 2;pageNum <= pageCount;pageNum++)//找到空闲页
    {
		byte = pageNum/8;
		bit = pageNum%8;
		if(((fileHandle->pfFileHandle.pBitmap[byte]&(1<<bit)) == 1)&&((fileHandle->pBitmap[byte]&(1<<bit)) == 0))
		{
			break;
		}
	}
	PF_PageHandle pfPageHandle;
	RC rc;
	if(pageNum > pageCount)
	{
		rc = AllocatePage(&(fileHandle->pfFileHandle),&pfPageHandle);
		if(rc != SUCCESS) return rc;
		rc = GetPageNum(&pfPageHandle,&pageNum);
		if(rc != SUCCESS) return rc;
	}
	else
	{
		rc = GetThisPage(&(fileHandle->pfFileHandle),pageNum,&pfPageHandle);
		if(rc != SUCCESS) return rc;
	}
	rc = GetData(&pfPageHandle,&pBitmap);
	if(rc != SUCCESS) return rc;
	for(slotNum = 0;slotNum < fileHandle->fileSubHeader->recordsPerPage;slotNum++) //找到空闲插槽
	{
		byte = slotNum/8;
		bit = slotNum%8;
		if((pBitmap[byte]&(1<<bit))==0)
        {
            break;
		}
	}
	rc = GetData(&pfPageHandle,&poffset);
	if(rc != SUCCESS) return rc;
	poffset = poffset + fileHandle->fileSubHeader->firstRecordOffset + slotNum*(fileHandle->fileSubHeader->recordSize);
	memcpy(poffset,pData,fileHandle->fileSubHeader->recordSize);//插入至插槽中
	byte = slotNum/8;
	bit = slotNum%8;
	pBitmap[byte] = pBitmap[byte] | (1<<bit);
	fileHandle->fileSubHeader->nRecords++;
	if(fileHandle->fileSubHeader->recordsPerPage == (slotNum + 1))//判断插入记录后页是否满了，满了控制页位图相应位置置为1
    {
		byte = pageNum/8;
		bit = pageNum%8;
		fileHandle->pBitmap[byte] = fileHandle->pBitmap[byte] | (1<<bit);
	}
	rc = MarkDirty(&pfPageHandle);
	if(rc != SUCCESS) return rc;
	rc = UnpinPage(&pfPageHandle);
	if(rc != SUCCESS) return rc;
	rc = GetThisPage(&(fileHandle->pfFileHandle),1,&pfPageHandle);
	if(rc != SUCCESS) return rc;
	rc = MarkDirty(&pfPageHandle);
	if(rc != SUCCESS) return rc;
	rc = UnpinPage(&pfPageHandle);
	if(rc != SUCCESS) return rc;
	rid->pageNum = pageNum;
	rid->slotNum = slotNum;
	rid->bValid = true;
	return SUCCESS;
}

RC DeleteRec (RM_FileHandle *fileHandle,const RID *rid)
{
    PageNum pageNum = rid->pageNum;
	SlotNum slotNum = rid->slotNum;
	char *pBitmap;
	RC rc;
	if(fileHandle->bOpen == false) return RM_FHCLOSED;
	PF_PageHandle pfPageHandle;
	rc = GetThisPage(&(fileHandle->pfFileHandle),pageNum,&pfPageHandle);
	if(rc != SUCCESS) return rc;
	rc = GetData(&pfPageHandle,&pBitmap);
	if(rc != SUCCESS) return rc;
	int byte,bit;
	byte = slotNum/8;
	bit = slotNum%8;
	if(pBitmap[byte]&(1<<bit) == 0) return RM_INVALIDRID;
	pBitmap[byte] = pBitmap[byte] & (~(1<<bit));
	fileHandle->fileSubHeader->nRecords--;
	byte = pageNum/8;
	bit = pageNum%8;
	fileHandle->pBitmap[byte] = fileHandle->pBitmap[byte] & (~(1<<bit));
	rc = MarkDirty(&pfPageHandle);
	if(rc != SUCCESS) return rc;
	rc = UnpinPage(&pfPageHandle);
	if(rc != SUCCESS) return rc;
	rc = GetThisPage(&(fileHandle->pfFileHandle),1,&pfPageHandle);
	if(rc != SUCCESS) return rc;
	rc = MarkDirty(&pfPageHandle);
	if(rc != SUCCESS) return rc;
	rc = UnpinPage(&pfPageHandle);
	if(rc != SUCCESS) return rc;
	return SUCCESS;
}

RC UpdateRec (RM_FileHandle *fileHandle,const RM_Record *rec)
{
	char *pBitmap,*poffset;
	RC rc;
	PF_PageHandle pfPageHandle;
	PageNum pageNum = rec->rid.pageNum;
	SlotNum slotNum = rec->rid.slotNum;
	if(fileHandle->bOpen == false) return RM_FHCLOSED;
	rc = GetThisPage(&(fileHandle->pfFileHandle),pageNum,&pfPageHandle);
	if(rc != SUCCESS) return rc;
	rc = GetData(&pfPageHandle,&pBitmap);
	int byte,bit;
	byte = slotNum/8;
	bit = slotNum%8;
	if(pBitmap[byte]&(1<<bit) == 0) return RM_INVALIDRID;
	poffset = pBitmap + fileHandle->fileSubHeader->firstRecordOffset + slotNum*(fileHandle->fileSubHeader->recordSize);
	memcpy(poffset,rec->pData,fileHandle->fileSubHeader->recordSize);
	rc = MarkDirty(&pfPageHandle);
	if(rc != SUCCESS) return rc;
	rc = UnpinPage(&pfPageHandle);
	if(rc != SUCCESS) return rc;
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
	rc = openFile(filename,&pffilehandle);
	if(rc != SUCCESS) return rc;
	PF_PageHandle pfpagehandle;
	rc = AllocatePage(pffilehandle,&pfpagehandle);
	if(rc != SUCCESS) return rc;
	char *pdata;
	rc = GetData(pfpagehandle,&pdata);
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
	rc = GetThisPage(&(fileHandle->pfFileHandle),1,&(fileHandle->pfpagehandle);
	if(rc != SUCCESS) return rc;
	char *pdata;
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
	rc = UnpinPage(fileHandle->pfpagehandle);
	if(rc != SUCCESS) return rc;
	rc = CloseFile(&(fileHandle->pfFileHandle));
	if(rc != SUCCESS) return rc;
	fileHandle->bOpen = false;
	return SUCCESS;
}
