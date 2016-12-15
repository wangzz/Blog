//
//  FGLinkList.c
//  ListDemo
//
//  Created by foogrywang(王中周) on 16/12/7.
//  Copyright © 2016年 foogrywang(王中周). All rights reserved.
//

#include "FGLinkList.h"
#include <stdlib.h>

Status GetElem (Linklist L,int i,ElemType e) {
    Linklist p;
    p = L->next;
    int j = 1;
    while (p && j < i) {
        p = p->next;
        j++;
    }
    
    if (!p || j > i) {
        return ERROR;
    }
    
    e = p->data;
    
    return OK;
}

Status ListInsert (Linklist *L,int i,ElemType e) {
    Linklist p = (*L)->next;
    int j = 1;
    while (p && j < i) {
        p = p->next;
        j++;
    }
    
    if (!p || j > i) {
        return ERROR;
    }
    
    Linklist s = (Linklist)malloc(sizeof(Node));
    s->data = e;
    s->next = p->next;
    p->next = s;
    
    return OK;
}


Status ListDelete (Linklist *L,int i,ElemType e) {
    Linklist p,q;
    int j = 1;
    while (p && j < i) {
        p = p->next;
        j++;
    }
    
    if (!p || j > i) {
        return ERROR;
    }
    
    q = p->next;
    p->next = q->next;
    e = q->data;
    free(q);
    
    return OK;
}

Status CreateListHead (Linklist *L,int n) {
    *L = (Linklist)malloc(sizeof(Node));
    Linklist c = *L;
    
    while (n > 0) {
        Linklist s = (Linklist)malloc(sizeof(Node));
        s->data = n;
        
        c->next = s;
        c = s;
        
        n--;
    }
    
    c->next = NULL;
    
    return OK;
}

Status ClearList (Linklist *L) {
    Linklist q;
    Linklist p = (*L)->next;
    while (p) {
        q = p->next;
        free(p);
        
        p = q;
    }
    
    (*L)->next = NULL;
    
    return OK;
}




