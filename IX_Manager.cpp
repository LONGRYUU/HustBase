#include "stdafx.h"
#include "IX_Manager.h"
#include <math.h>
RC CreateIndex(const char *fileName, AttrType attrType, int attrLength)
{
	CreateFile(fileName);
	PF_FileHandle fileHandle;
	openFile((char *)fileName, &fileHandle);
	PF_PageHandle pageHandle;
	AllocatePage(&fileHandle, &pageHandle);
	if (pageHandle.pFrame->page.pageNum != 1) {
		DisposePage(&fileHandle, pageHandle.pFrame->page.pageNum);
		return PF_FILEERR;
	}
	IX_FileHeader ix_FileHeader;
	ix_FileHeader.attrLength = attrLength;
	ix_FileHeader.keyLength = attrLength + sizeof(RID);
	ix_FileHeader.attrType = attrType;
	// -1 means not set yet;
	ix_FileHeader.first_leaf = -1;
	ix_FileHeader.rootPage = -1;
	ix_FileHeader.order = (PF_PAGE_SIZE - sizeof(IX_FileHeader) - sizeof(IX_Node) / (2 * sizeof(RID) + attrLength));
	char *pData;
	GetData(&pageHandle, &pData);
	memcpy(pData, &ix_FileHeader, sizeof(IX_FileHeader));
	MarkDirty(&pageHandle);
	UnpinPage(&pageHandle);
	CloseFile(&fileHandle);
	return SUCCESS;
}

RC OpenIndex(const char *fileName, IX_IndexHandle *indexHandle) {
	IX_IndexHandle *pindexHandle = indexHandle;
	openFile((char *)fileName, &pindexHandle->fileHandle);
	PF_PageHandle pf_PageHandle;
	GetThisPage(&pindexHandle->fileHandle, 1, &pf_PageHandle);
	memcpy(&pindexHandle->fileHeader, pf_PageHandle.pFrame->page.pData, sizeof(IX_FileHeader));
	return SUCCESS;
}

RC CloseIndex(IX_IndexHandle *indexHandle) {
	CloseFile(&indexHandle->fileHandle);
	return SUCCESS;
}

RC DeleteEntry(IX_IndexHandle *indexHandle, void *pData, const RID * rid) {
	return SUCCESS;
}

RC InsertEntry(IX_IndexHandle *indexHandle, void *pData, const RID * rid) {
	IX_IndexHandle *pindexHandle = indexHandle;
	int control_Off = sizeof(IX_FileHeader);
	int data_Off = sizeof(IX_FileHeader) + sizeof(IX_Node);
	int rid_Off = data_Off + pindexHandle->fileHeader.keyLength;
	int data_Length = pindexHandle->fileHeader.attrLength + 2 * sizeof(RID);
	//init root page;(when we insert first data)
	if (pindexHandle->fileHeader.rootPage == 0) {
		pindexHandle->fileHeader.rootPage = 1;
		pindexHandle->fileHeader.first_leaf = 1;
		PF_PageHandle Page;
		GetThisPage(&pindexHandle->fileHandle, pindexHandle->fileHeader.rootPage, &Page);
		IX_Node ix_Node;
		char *Data;
		GetData(&Page, &Data);
		ix_Node.is_leaf = true;
		ix_Node.brother = 0;
		ix_Node.parent = 0;
		ix_Node.keynum = 0;
		ix_Node.keys = (char *)(Data + data_Off);
		ix_Node.rids = (RID *)(Data + rid_Off);
		memcpy(Data + sizeof(IX_FileHeader), &ix_Node, sizeof(IX_Node));
		MarkDirty(&Page);
		UnpinPage(&Page);
	}
	/*############# search insertPage£¨leaf£© #############*/
	PF_PageHandle insertPage;
	char *Data;
	IX_Node *ix_Node;
	PageNum pageNum = pindexHandle->fileHeader.rootPage;
	while (true) {
		GetThisPage(&pindexHandle->fileHandle, pageNum, &insertPage);
		GetData(&insertPage, &Data);
		ix_Node = (IX_Node *)(Data + control_Off);
		if (ix_Node->is_leaf) {
			break;
		}
		else {
			//get next layer page;
			int i = 0;
			for (; i < ix_Node->keynum; i++) {
				int ret_Data = Compare_Data(pindexHandle->fileHeader.attrType,
					(char *)pData, ix_Node->keys + i * data_Length);
				if (ret_Data < 0 ) {
					break;
				}
				else if(ret_Data > 0){
					continue;
				}
				else {
					int ret_RID = Compare_RID(rid, (RID *)(ix_Node->keys + pindexHandle->fileHeader.attrLength + i * data_Length));
					if (ret_RID < 0) {
						break;
					}
					else if (ret_RID > 0) {
						continue;
					}
					else {
						return IX_INVALIDKEY;
					}
				}
			}
			pageNum = ((RID *)(ix_Node->rids + i * data_Length))->pageNum;
		}
	}
	/*############# insert into page ##############*/
	//TODO;
	char BUF[BUFSIZ];
	RID r_temp;
	strcpy(BUF, (char *)pData);
	*(RID *)(BUF + pindexHandle->fileHeader.attrLength) = *rid;
	r_temp = *rid;
	InsertPage(pindexHandle, insertPage.pFrame->page.pageNum, BUF, &r_temp);
	return SUCCESS;
}

RC OpenIndexScan(IX_IndexScan *indexScan,IX_IndexHandle *indexHandle,CompOp compOp,char *value){
	//indexScan->compOp = compOp;
	//indexScan->value = value; 
	//indexScan->pIXIndexHandle = indexHandle;
	return SUCCESS;
}

RC IX_GetNextEntry(IX_IndexScan *indexScan,RID * rid){
	return SUCCESS;
}

RC CloseIndexScan(IX_IndexScan *indexScan){
		return SUCCESS;
}

RC GetIndexTree(char *fileName, Tree *index){
	return SUCCESS;
}

int Compare_Data(AttrType type, const char *L_Data, const char *R_Data) {
	switch (type)
	{
		case chars:
			return strcmp(L_Data, R_Data);
		case floats:
			return *(float*)L_Data - *(float*)R_Data;
		case ints:
			return *(int*)L_Data - *(int*)R_Data;
		break;
	}
	return 0;
}

int Compare_RID(const RID *L_RID, const RID *R_RID) {
	int ret = L_RID->pageNum - R_RID->pageNum;
	if (ret == 0) {
		ret = L_RID->slotNum - R_RID->slotNum;
	}
	return ret;
}

int Swap_Data_RID(AttrType type, int length, char *L_Data, char *R_Data, RID *L_RID, RID *R_RID) {
	//swap_data;
	switch (type) {
		case chars: {
			char BUF[BUFSIZ];
			strcpy(BUF, L_Data);
			strcpy(L_Data, R_Data);
			strcpy(R_Data, BUF);
			break;
		}
		case ints: {
			int i_temp;
			i_temp = *(int*)L_Data;
			*(int*)L_Data = *(int*)R_Data;
			*(int*)R_Data = i_temp;
			break;
		}
		case floats: {
			float f_temp;
			f_temp = *(float*)L_Data;
			*(float*)L_Data = *(float*)R_Data;
			*(float*)R_Data = f_temp;
			break;
		}
	}
	//swap_Data_RID;
	RID dr_temp;
	dr_temp = *(RID *)(L_Data + length);
	*(RID *)(L_Data + length) = *(RID *)(R_Data + length);
	*(RID *)(R_Data + length) = dr_temp;
	//swap_RID;
	RID r_temp;
	r_temp = *L_RID;
	*L_RID = *R_RID;
	*R_RID = r_temp;
	return 0;
}

RC InsertPage(IX_IndexHandle *indexHandle, PageNum pageNum,char *pData, RID * rid) {
	IX_IndexHandle *pindexHandle = indexHandle;
	PF_PageHandle insertPage;
	IX_Node *ix_Node; 
	int control_Off = sizeof(IX_FileHeader);
	int data_Off = sizeof(IX_FileHeader) + sizeof(IX_Node);
	int rid_Off = data_Off + pindexHandle->fileHeader.keyLength;
	int data_Length = pindexHandle->fileHeader.attrLength + 2 * sizeof(RID);
	char *Data;

	GetThisPage(&pindexHandle->fileHandle, pageNum, &insertPage);
	GetData(&insertPage, &Data);
	ix_Node = (IX_Node *)(Data + control_Off);

	if (ix_Node->keynum >= pindexHandle->fileHeader.order) {
		//split into two pages & make point;
		for (int i = 0; i < ceil(ix_Node->keynum / 2.0); i++) {
			int ret_Data = Compare_Data(pindexHandle->fileHeader.attrType,
				(char *)pData, ix_Node->keys + i * data_Length);
			if (ret_Data < 0) {
				Swap_Data_RID(pindexHandle->fileHeader.attrType, pindexHandle->fileHeader.attrLength, pData,
					ix_Node->keys + i * data_Length, rid, ix_Node->rids + i * data_Length);
				continue;
			}
			else if (ret_Data > 0) {
				continue;
			}
			else {
				int ret_RID = Compare_RID(rid, (RID *)(ix_Node->keys + pindexHandle->fileHeader.attrLength + i * data_Length));
				if (ret_RID < 0) {
					Swap_Data_RID(pindexHandle->fileHeader.attrType, pindexHandle->fileHeader.attrLength, pData,
						ix_Node->keys + i * data_Length, rid, ix_Node->rids + i * data_Length);
					continue;
				}
				else if (ret_RID > 0) {
					continue;
				}
				else {
					return IX_INVALIDKEY;
				}
			}
		}
		ix_Node->keynum = ceil(ix_Node->keynum / 2.0);

		//create brother page and insert half nodes;
		PF_PageHandle brotherPage;
		AllocatePage(&indexHandle->fileHandle, &brotherPage);
		char *brotherData;
		IX_Node *ix_Node_Brother;
		GetData(&brotherPage, &brotherData);
		ix_Node_Brother = (IX_Node *)(brotherData + control_Off);
		ix_Node_Brother->is_leaf = ix_Node->is_leaf;
		ix_Node_Brother->brother = ix_Node->brother;
		ix_Node->brother = brotherPage.pFrame->page.pageNum;
		ix_Node_Brother->keynum = 0;
		ix_Node_Brother->keys = (char *)(brotherData + data_Off);
		ix_Node_Brother->rids = (RID *)(brotherData + rid_Off);
		for (int i = ceil(ix_Node->keynum / 2.0); i < pindexHandle->fileHeader.order; i++) {
			int ret_Data = Compare_Data(pindexHandle->fileHeader.attrType,
				(char *)pData, ix_Node->keys + i * data_Length);
			if (ret_Data < 0) {
				Swap_Data_RID(pindexHandle->fileHeader.attrType, pindexHandle->fileHeader.attrLength, pData,
					ix_Node->keys + i * data_Length, rid, ix_Node->rids + i * data_Length);
			}
			else if (ret_Data > 0) {
				//pass;
			}
			else {
				int ret_RID = Compare_RID(rid, (RID *)(ix_Node->keys + pindexHandle->fileHeader.attrLength + i * data_Length));
				if (ret_RID < 0) {
					Swap_Data_RID(pindexHandle->fileHeader.attrType, pindexHandle->fileHeader.attrLength, pData,
						ix_Node->keys + i * data_Length, rid, ix_Node->rids + i * data_Length);
				}
				else if (ret_RID > 0) {
					//pass;
				}
				else {
					return IX_INVALIDKEY;
				}
			}
			Swap_Data_RID(pindexHandle->fileHeader.attrType, pindexHandle->fileHeader.attrLength,
				ix_Node->keys + i * data_Length, ix_Node_Brother->keys + ix_Node_Brother->keynum * data_Length,
				(RID *)(ix_Node->rids + i * data_Length),
				(RID *)(ix_Node_Brother->rids + ix_Node_Brother->keynum * data_Length));
			ix_Node_Brother->keynum++;
		}
		Swap_Data_RID(pindexHandle->fileHeader.attrType, pindexHandle->fileHeader.attrLength, pData,
			ix_Node_Brother->keys + ix_Node_Brother->keynum * data_Length, rid, ix_Node_Brother->rids + ix_Node_Brother->keynum * data_Length);
		ix_Node_Brother->keynum++;
		if (ix_Node->parent == 0) {//means root page;
			PF_PageHandle parentPage;
			AllocatePage(&indexHandle->fileHandle, &parentPage);
			char *parentData;
			IX_Node *ix_Node_Parent;
			GetData(&parentPage, &parentData);
			ix_Node_Parent = (IX_Node *)(parentData + control_Off);
			ix_Node_Parent->is_leaf = false;
			ix_Node_Parent->brother = 0;
			ix_Node_Parent->parent = 0;
			ix_Node_Parent->keynum = 0;
			ix_Node_Parent->keys = (char *)(parentData + data_Off);
			ix_Node_Parent->rids = (RID *)(parentData + rid_Off);
			MarkDirty(&parentPage);
			UnpinPage(&parentPage);
			rid->bValid = true;
			rid->pageNum = insertPage.pFrame->page.pageNum;
			switch (indexHandle->fileHeader.attrType) {
			case chars: {
				strcpy(pData, ix_Node->keys);
				break;
			}
			case ints: {
				*(int *)pData = *(int *)ix_Node->keys;
				break;
			}
			case floats: {
				*(float *)pData = *(float *)ix_Node->keys;
				break;
			}
			}
			*(RID *)(pData + pindexHandle->fileHeader.attrLength) = *(RID *)(ix_Node->keys + pindexHandle->fileHeader.attrLength);
			InsertPage(pindexHandle, parentPage.pFrame->page.pageNum, pData, rid);
		}
		rid->bValid = true;
		rid->pageNum = brotherPage.pFrame->page.pageNum;
		switch (indexHandle->fileHeader.attrType) {
			case chars: {
				strcpy(pData, ix_Node_Brother->keys);
				break;
			}
			case ints: {
				*(int *)pData = *(int *)ix_Node_Brother->keys;
				break;
			}
			case floats: {
				*(float *)pData = *(float *)ix_Node_Brother->keys;
				break;
			}
		}
		*(RID *)(pData + pindexHandle->fileHeader.attrLength) = *(RID *)(ix_Node_Brother->keys + pindexHandle->fileHeader.attrLength);
		MarkDirty(&brotherPage);
		UnpinPage(&brotherPage);
		InsertPage(pindexHandle, ix_Node->parent, pData, rid);
	}
	//insert into unfull page;
	else {
		/*################ correct childpage's parent point ##################*/
		if (ix_Node->is_leaf == false) {
			PF_PageHandle pageTemp;
			GetThisPage(&pindexHandle->fileHandle, rid->pageNum, &pageTemp);
			char *dataTemp;
			IX_Node *nodeTemp;
			GetData(&pageTemp, &dataTemp);
			nodeTemp = (IX_Node *)(dataTemp + control_Off);
			nodeTemp->parent = pageNum;
			MarkDirty(&pageTemp);
			UnpinPage(&pageTemp);
		}
		//insert into unfull page;
		for (int i = 0; i < ix_Node->keynum; i++) {
			int ret_Data = Compare_Data(pindexHandle->fileHeader.attrType,
				pData, ix_Node->keys + i * data_Length);
			if (ret_Data < 0) {
				Swap_Data_RID(pindexHandle->fileHeader.attrType, pindexHandle->fileHeader.attrLength, pData,
					ix_Node->keys + i * data_Length, rid, ix_Node->rids + i * data_Length);
				continue;
			}
			else if (ret_Data > 0) {
				continue;
			}
			else {
				int ret_RID = Compare_RID(rid, (RID *)(ix_Node->keys + pindexHandle->fileHeader.attrLength + i * data_Length));
				if (ret_RID < 0) {
					Swap_Data_RID(pindexHandle->fileHeader.attrType, pindexHandle->fileHeader.attrLength, pData,
						ix_Node->keys + i * data_Length, rid, ix_Node->rids + i * data_Length);
					continue;
				}
				else if (ret_RID > 0) {
					continue;
				}
				else {
					return IX_INVALIDKEY;
				}
			}
		}
		strcpy(ix_Node->keys + ix_Node->keynum * data_Length, pData);
		*(RID *)(ix_Node->rids + pindexHandle->fileHeader.attrLength + ix_Node->keynum * data_Length)
			= *(RID *)(pData + pindexHandle->fileHeader.attrLength);
		*(RID *)(ix_Node->rids + ix_Node->keynum * data_Length) = *rid;
		ix_Node->keynum++;
	}
	MarkDirty(&insertPage);
	UnpinPage(&insertPage);
	return SUCCESS;
}
