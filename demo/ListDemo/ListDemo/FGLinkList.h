//
//  FGLinkList.h
//  ListDemo
//
//  Created by foogrywang(王中周) on 16/12/7.
//  Copyright © 2016年 foogrywang(王中周). All rights reserved.
//

#ifndef FGLinkList_h
#define FGLinkList_h

#include <stdio.h>
#include "CommonDefine.h"

typedef struct Node {
    ElemType data;
    struct Node *next;
}Node;

typedef struct Node *Linklist;

Status GetElem (Linklist L,int i,ElemType e);
Status ListInsert (Linklist *L,int i,ElemType e);
Status ListDelete (Linklist *L,int i,ElemType e);
Status CreateListHead (Linklist *L,int n);
Status ClearList (Linklist *L);

#endif /* FGLinkList_h */
