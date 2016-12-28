//
//  main.c
//  ListDemo
//
//  Created by foogrywang(王中周) on 16/12/7.
//  Copyright © 2016年 foogrywang(王中周). All rights reserved.
//

#include <stdio.h>
#include "FGSqStack.h"
#include "FGRPN.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    SqStack S;
    InitStack(&S);
    
    push(&S, 10);
    
    ElemType e;
    GetTop(&S, &e);
    printf("%d \n",e);
    
    char str[] = "9+(3-1)*3+10/2";
    NifixExpression2PostfixExpression(str, NULL);
    
    
    
    
    return 0;
}
