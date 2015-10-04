#include "Core/Queue.h"

QUEUE_T* QUEUE_MIDDLE(QUEUE_T* queue)
{
    QUEUE_T* middle = NULL;
    QUEUE_T* next = NULL;

    assert(queue);

    middle = QUEUE_HEAD(queue);

    if (middle == QUEUE_LAST(queue))
    {
        return middle;
    }

    next = QUEUE_HEAD(queue);

    for (;;)
    {
        middle = QUEUE_NEXT(middle);

        next = QUEUE_NEXT(next);

        if (next == QUEUE_LAST(queue))
        {
            return middle;
        }

        next = QUEUE_NEXT(next);

        if (next == QUEUE_LAST(queue))
        {
            return middle;
        }
    }

    assert(!"can not be here!");
}

// 稳定的插入排序
void QUEUE_SORT(QUEUE_T* queue, int (*cmp)(const QUEUE_T*, const QUEUE_T*))
{
    QUEUE_T* q = NULL;
    QUEUE_T* prev = NULL;
    QUEUE_T* next = NULL;

    assert(queue);
    assert(cmp);

    q = QUEUE_HEAD(queue);

    if (q == QUEUE_LAST(queue))
    {
        return;
    }

    for (q = QUEUE_NEXT(q); q != QUEUE_SENTINEL(queue); q = next)
    {

        prev = QUEUE_PREV(q);
        next = QUEUE_NEXT(q);

        QUEUE_REMOVE(q);

        do
        {
            if (cmp(prev, q) <= 0)
            {
                break;
            }

            prev = QUEUE_PREV(prev);

        }
        while (prev != QUEUE_SENTINEL(queue));

        QUEUE_INSERT_AFTER(prev, q);
    }
}
