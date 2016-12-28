//
//  FGRPN.c
//  ListDemo
//
//  Created by foogrywang(王中周) on 16/12/15.
//  Copyright © 2016年 foogrywang(王中周). All rights reserved.
//

#include "FGRPN.h"
#include <ctype.h>
#include <string.h>

int isNumber(char c) {
    if (c >= 0 && c <= 9) {
        return TRUE;
    }
    
    return FALSE;
}

int isOperator(char c) {
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') {
        return TRUE;
    }
    
    return FALSE;
}

typedef enum {
    RPNTypeUnknown = -1,
    RPNTypePop = 0,
    RPNTypePush = 1,
    RPNTypePoPLeftBracket = 2,
    RPNTypePopAll = 3,
} RPNType;

/**
 判断两个运算符s1（输入）,s2（栈顶元素）的优先顺序
 若s1优先或等于s2则返回0,若低于s2则返回1
 */
int Precede(char s1,char s2) {
    int f = RPNTypeUnknown;
    if (s1 == ')') {
        return 1;
    }
    
    switch(s1) {
        case '+':
        case '-':
            if (s2=='*'||s2=='/'||s2=='+'||s2=='-') {
                f=RPNTypePop;
            } else if (s2=='\0'||s2=='(') {
                f=RPNTypePush;
            }
            break;
        case '*':
        case '/':
            if (s2=='\0'||s2=='('||s2=='+'||s2=='-') {
                f=RPNTypePush;
            } else if (s2=='*'||s2=='/') {
                f=RPNTypePop;
            }
            break;
        case '\0':
            f=RPNTypePopAll;
            break;
        case '(':
            f=RPNTypePush;
            break;
        case ')':
            f=RPNTypePoPLeftBracket;
            break;
        default :
            printf("ERROR !");
    }
    
    return f;
}

// 中缀表达式转换成后缀表达式
// http://www.jb51.net/article/37456.htm
Status NifixExpression2PostfixExpression(char *input, char *output) {
    int i = 0;
    char str[] = {};
    SqStack S;
    InitStack(&S);
    
    int strLength = 0;
    while (input[i] != '\n') {
        char c = input[i];
        if (c == '(') {
            push(&S, c);
        } else if (c == ')') {
            char e = '\0';
            GetTop(&S, &e);
            while (e != '(') {
                str[strLength++] = e;
                pop(&S, &e);
                GetTop(&S, &e);
            }
            
            pop(&S, &e); // pop '('
        } else if (isdigit(c)) {
            str[strLength++] = c;
            printf("111 str=%s  c=%c  strLength=%d  index=%d\n",str,c,strLength,i);
        } else if (isOperator(c)) {
            char e = '\0';
            GetTop(&S, &e);
            RPNType result = Precede(c, e);
            if (result == RPNTypePush) {
                push(&S, c);
            } else if (result == RPNTypePop) {
                pop(&S, &e);
                str[strLength++] = e;
            }
        } else {
            continue;
        }
        
//        printf("bef %s  %d \n",str,index);
        
        i++;
        
//        printf("aft %s  %d \n",str,index);
    }
    
    
//    int length = (int)strlen(input);
//    for (int i = 0; i < length; i++) {
//        char c = input[i];
//        if (isdigit(c)) {
//            printf("111 %s  %c  %d  %d\n",str,c,length,index);
//            str[length-1] = c;
//            printf("222 %s  %c  %d  %d\n",str,c,length,index);
//        } else if (isOperator(c)) {
//            char e = '\0';
//            GetTop(&S, &e);
//            RPNType result = Precede(c, e);
//            if (result == RPNTypePush) {
//                push(&S, c);
//            } else if (result == RPNTypePop) {
//                pop(&S, &e);
//                str[length-1] = e;
//            }
//        } else {
//            continue;
//        }
//    }
    
    printf("%s",str);
    
    return OK;
}

