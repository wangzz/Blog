//
//  FGSqStack.h
//  ListDemo
//
//  Created by foogrywang(王中周) on 16/12/13.
//  Copyright © 2016年 foogrywang(王中周). All rights reserved.
//

#ifndef FGSqStack_h
#define FGSqStack_h

#include <stdio.h>
#include "CommonDefine.h"

typedef struct {
    ElemType data[MAXSIZE];
    int top;
}SqStack;

Status push(SqStack *S, ElemType e);

Status pop(SqStack *S, ElemType *e);

Status InitStack(SqStack *S);

Status DestoryStack(SqStack *S);

Status ClearStack(SqStack *S);

Status GetTop(SqStack *S, ElemType *e);

int StackLength(SqStack *S);

#endif /* FGSqStack_h */
