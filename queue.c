#include "queue.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "natsort/strnatcmp.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q == NULL)
        return NULL;
    q->head = q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (q == NULL)
        return;
    list_ele_t *e = q->head;
    while (e != NULL) {
        if (e->value != NULL) {
            free(e->value);
            e->value = NULL;
        }
        list_ele_t *ne = e->next;
        free(e);
        e = ne;
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, const char *s)
{
    if (q == NULL)
        return false;

    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (newh == NULL)
        return false;

    size_t sl = strlen(s);
    newh->value = malloc(sl + 1);
    if (newh->value == NULL) {
        // Note: remember to free newh
        // fail on test 11
        free(newh);
        return false;
    }
    strncpy(newh->value, s, sl);
    newh->value[sl] = '\0';

    newh->next = q->head;
    q->head = newh;
    if (q->tail == NULL)
        q->tail = newh;
    ++q->size;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, const char *s)
{
    if (q == NULL)
        return false;

    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (newh == NULL)
        return false;

    size_t sl = strlen(s);
    newh->value = malloc(sl + 1);
    if (newh->value == NULL) {
        // Note: remember to free newh
        // fail on test 11
        free(newh);
        return false;
    }
    strncpy(newh->value, s, sl);
    newh->value[sl] = '\0';

    newh->next = NULL;
    if (q->head == NULL)
        q->head = newh;
    q->tail->next = newh;
    q->tail = newh;
    ++q->size;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (q == NULL || q->head == NULL)
        return false;

    if (q->head->value != NULL) {
        // Note: remember to free the value
        // even if sp is null (rhq: remove head quiet),
        // otherwise fail on test 09
        if (sp != NULL) {
            strncpy(sp, q->head->value, bufsize - 1);
            sp[bufsize - 1] = '\0';
        }
        free(q->head->value);
        q->head->value = NULL;
    }

    if (q->head == q->tail)
        q->tail = NULL;
    list_ele_t *nh = q->head->next;
    free(q->head);
    q->head = nh;
    --q->size;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (q == NULL || q->head == NULL)
        return 0;
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (q == NULL || q->head == NULL)
        return;

    list_ele_t *pe = NULL;
    list_ele_t *e = q->head;
    while (e != NULL) {
        list_ele_t *ne = e->next;
        e->next = pe;
        pe = e;
        e = ne;
    }
    q->tail = q->head;
    q->head = pe;
}

static inline int __cmp_list_ele(const list_ele_t *x, const list_ele_t *y)
{
    if (x->value == NULL || y->value == NULL) {
        if (x->value == NULL && y->value == NULL)
            return 0;
        else if (x->value == NULL)
            return 1;
        else
            return -1;
    }
    return strnatcasecmp(x->value, y->value);
}

static list_ele_t *__mergesort(list_ele_t *l)
{
    if (l == NULL || l->next == NULL)
        return l;

    list_ele_t *slow = l;
    list_ele_t *fast = l->next;  // Note: what if fast = l ?
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    list_ele_t *t = slow->next;
    slow->next = NULL;

    l = __mergesort(l);
    t = __mergesort(t);

    list_ele_t _nl;
    list_ele_t *nl = &_nl;
    while (l != NULL || t != NULL) {
        if (t == NULL || (l != NULL && __cmp_list_ele(l, t) <= 0)) {
            nl->next = l;
            nl = l;
            l = l->next;
        } else {
            nl->next = t;
            nl = t;
            t = t->next;
        }
    }
    nl->next = NULL;
    return _nl.next;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (q == NULL || q->head == NULL || q->head == q->tail)
        return;

    q->head = __mergesort(q->head);
}
