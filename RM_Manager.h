#ifndef RM_MANAGER_H_H
#define RM_MANAGER_H_H

#include "PF_Manager.h"
#include "str.h"

typedef struct {
	int nRecords;			//当前文件中包含的记录数
	int recordSize;			//每个记录的大小
	int recordsPerPage;		//每个页面可以装载的记录数量
    int firstRecordOffset;	//每页第一个记录在数据区中的开始位置
} RM_FileSubHeader;

typedef int SlotNum;

typedef struct {
	PageNum pageNum;	//记录所在页的页号
	SlotNum slotNum;		//记录的插槽号
	bool bValid; 			//true表示为一个有效记录的标识符
}RID;

typedef struct{
	bool bValid;		 // False表示还未被读入记录
	RID  rid; 		 // 记录的标识符
	char *pData; 		 //记录所存储的数据
}RM_Record;


typedef struct
{
	int bLhsIsAttr,bRhsIsAttr;//左、右是属性（1）还是值（0）
	AttrType attrType;
	int LattrLength,RattrLength;
	int LattrOffset,RattrOffset;
	CompOp compOp;
	void *Lvalue,*Rvalue;
}Con;

typedef struct{//记录文件句柄
	bool bOpen;//句柄是否打开（是否正在被使用）
	PF_FileHandle pfFileHandle;	//文件句柄（文件名/文件是否被使用）
	RM_FileSubHeader *fileSubHeader;	//记录文件的RM_FileSubHeader的指针
	char *pBitmap;  //指向记录控制页中位图的指针
	PF_PageHandle pfpagehandle; //页面句柄指针
}RM_FileHandle;

typedef struct{
	bool  bOpen;		//扫描是否打开
	RM_FileHandle  *pRMFileHandle;		//扫描的记录文件句柄
	int  conNum;		//扫描涉及的条件数量
	Con  *conditions;	//扫描涉及的条件数组指针
    PF_PageHandle  PageHandle; //处理中的页面句柄
	PageNum  pn; 	//扫描即将处理的页面号
	SlotNum  sn;		//扫描即将处理的插槽号
}RM_FileScan;



RC GetNextRec(RM_FileScan *rmFileScan,RM_Record *rec);

RC OpenScan(RM_FileScan *rmFileScan,RM_FileHandle *fileHandle,int conNum,Con *conditions);

RC CloseScan(RM_FileScan *rmFileScan);

RC UpdateRec (RM_FileHandle *fileHandle,const RM_Record *rec);

RC DeleteRec (RM_FileHandle *fileHandle,const RID *rid);

RC InsertRec (RM_FileHandle *fileHandle, char *pData, RID *rid);

RC GetRec (RM_FileHandle *fileHandle, RID *rid, RM_Record *rec);

RC RM_CloseFile (RM_FileHandle *fileHandle);

RC RM_OpenFile (char *fileName, RM_FileHandle *fileHandle);

RC RM_CreateFile (char *fileName, int recordSize);

#endif
