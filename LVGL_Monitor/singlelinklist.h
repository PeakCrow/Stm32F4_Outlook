#ifndef SINGLELINKLIST_H
#define SINGLELINKLIST_H
#include <stdlib.h>

/* 单链表的存储结构 */
#define ElemType double
#define OK 1
#define ERORR 0
#define OVERFLOW -2
typedef int status;

typedef struct _LNode
{
    ElemType data;/* 数据域 */
    struct LNode *next;/* 指针域 */
}LNode,*linkList;



#endif // SINGLELINKLIST_H
