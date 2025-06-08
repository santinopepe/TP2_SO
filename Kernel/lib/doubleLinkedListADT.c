#include <doubleLinkedListADT.h>
#include <MemoryManager.h>



static Node *createNode(void *data);

DoubleLinkedListADT createDoubleLinkedList()
{
    DoubleLinkedListADT list = (DoubleLinkedListADT)malloc(sizeof(struct DoubleLinkedListCDT));
    list->first = NULL;
    list->last = NULL;
    list->size = 0;
    return list;
}

static Node *createNode(void *data)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        return NULL;
    }
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

int insertFirst(DoubleLinkedListADT list, void *data)
{
    Node *newNode = createNode(data);
    if (newNode == NULL)
    {
        return -1;
    }
    if (list->first == NULL)
    {
        list->first = newNode;
        list->last = newNode;
    }
    else
    {
        newNode->next = list->first;
        list->first->prev = newNode;
        list->first = newNode;
    }
    list->size++;
    return 0;
}

int insertLast(DoubleLinkedListADT list, void *data)
{
    Node *newNode = createNode(data);
    if (newNode == NULL)
    {
        return -1;
    }
    if (list->first == NULL)
    {
        list->first = newNode;
        list->last = newNode;
    }
    else
    {
        newNode->prev = list->last;
        list->last->next = newNode;
        list->last = newNode;
    }
    list->size++;
    return 0;
}

int removeFirst(DoubleLinkedListADT list)
{

    if (list->first == NULL)
    {
        return -1;
    }

    Node *temp = list->first;
    list->first = list->first->next;
    if (list->first != NULL)
    {
        list->first->prev = NULL;
    }
    else
    {
        list->last = NULL;
    }
    free(temp);
    list->size--;
    return 0;
}

int removeLast(DoubleLinkedListADT list)
{
    if (list->last == NULL)
    {
        return -1;
    }
    Node *temp = list->last;
    list->last = list->last->prev;
    if (list->last != NULL)
    {
        list->last->next = NULL;
    }
    else
    {
        list->first = NULL;
    }
    free(temp);
    list->size--;
    return 0;
}

int removeElement(DoubleLinkedListADT list, void *data)
{
    if (list->first == NULL)
    {
        return -1;
    }
    Node *current = list->first;
    while (current != NULL)
    {
        if ((char *)current->data - (char *)data == 0)
        {
            if (current == list->first)
            {
                return removeFirst(list);
            }
            else if (current == list->last)
            {
                return removeLast(list);
            }
            else
            {
                current->prev->next = current->next;
                current->next->prev = current->prev;
                list->size--;
                free(current);
                return 0;
            }
        }
        current = current->next;
    }
    return -1;
}

int freeList(DoubleLinkedListADT list)
{
    Node *current = list->first;
    while (current != NULL)
    {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    free(list);
    return 0;
}

void *getFirst(DoubleLinkedListADT list)
{
    if (list->first == NULL)
        return NULL;
    return list->first->data;
}

void *getLast(DoubleLinkedListADT list)
{
    if (list->last == NULL)
        return NULL;
    return list->last->data;
}

int getSize(DoubleLinkedListADT list)
{
    return list->size;
}

int isEmpty(DoubleLinkedListADT list)
{
    return list->size == 0;
}
