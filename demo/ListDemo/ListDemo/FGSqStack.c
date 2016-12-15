//
//  FGSqStack.c
//  ListDemo
//
//  Created by foogrywang(王中周) on 16/12/13.
//  Copyright © 2016年 foogrywang(王中周). All rights reserved.
//

#include "FGSqStack.h"
#include <stdlib.h>

Status push(SqStack *S, ElemType e) {
    if (S->top == MAXSIZE-1) {
        return ERROR;
    }
    
    S->top++;
    S->data[S->top] = e;
    
    return OK;
}

Status pop(SqStack *S, ElemType *e) {
    if (S->top < 0) {
        return ERROR;
    }
    
    *e = S->data[S->top];
    S->top--;
    
    return OK;
}

Status InitStack(SqStack *S) {
    *S = *(SqStack *)malloc(sizeof(SqStack));
    S->top = -1;
    
    return OK;
}

Status DestoryStack(SqStack *S) {
    free(S);
    return OK;
}

Status ClearStack(SqStack *S) {
    S->top = -1;
    
    return OK;
}

Status GetTop(SqStack *S, ElemType *e) {
    if (S->top < 0) {
        return ERROR;
    }
    
    *e = S->data[S->top];
    
    return OK;
}

int StackLength(SqStack *S) {
    return S->top+1;
}



