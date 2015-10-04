#ifndef CORE_QUEUE_H_
#define CORE_QUEUE_H_

#include "Core/Common.h"

// 移植 nginx 的 ngx_queue_t, 特点是删除没有查找操作, 所以非常高效, 而且是侵入式设计, 无需动态申请申请内存维护 queue 本身
struct CORE_API QUEUE_T
{
    QUEUE_T* prev;
    QUEUE_T* next;
};

#define QUEUE_INIT(q) \
    (q)->prev = q; \
    (q)->next = q

#define QUEUE_EMPTY(h) \
    (h == (h)->prev)

#define QUEUE_INSERT_HEAD(h, x) \
    (x)->next = (h)->next; \
    (x)->next->prev = x; \
    (x)->prev = h; \
    (h)->next = x

#define QUEUE_INSERT_AFTER   QUEUE_INSERT_HEAD

#define QUEUE_INSERT_TAIL(h, x) \
    (x)->prev = (h)->prev; \
    (x)->prev->next = x; \
    (x)->next = h; \
    (h)->prev = x

#define QUEUE_HEAD(h) \
    (h)->next

#define QUEUE_LAST(h) \
    (h)->prev

#define QUEUE_SENTINEL(h) \
    (h)

#define QUEUE_NEXT(q) \
    (q)->next

#define QUEUE_PREV(q) \
    (q)->prev

#define QUEUE_REMOVE(x) \
    (x)->next->prev = (x)->prev; \
    (x)->prev->next = (x)->next

#define QUEUE_SPLIT(h, q, n) \
    (n)->prev = (h)->prev; \
    (n)->prev->next = n; \
    (n)->next = q; \
    (h)->prev = (q)->prev; \
    (h)->prev->next = h; \
    (q)->prev = n;

#define QUEUE_ADD(h, n) \
    (h)->prev->next = (n)->next; \
    (n)->next->prev = (h)->prev; \
    (h)->prev = (n)->prev; \
    (h)->prev->next = h;

#define QUEUE_DATA(q, t, link) \
    (t *)((char *)q - offsetof(t, link))

CORE_API QUEUE_T* QUEUE_MIDDLE(QUEUE_T* queue);

// 稳定的插入排序
CORE_API void QUEUE_SORT(QUEUE_T* queue, int (*cmp)(const QUEUE_T*, const QUEUE_T*));

#endif // CORE_QUEUE_H_
