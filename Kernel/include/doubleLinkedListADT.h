#ifndef DOUBLE_LINKED_LIST_ADT_H
#define DOUBLE_LINKED_LIST_ADT_H

#include <stddef.h>



typedef struct DoubleLinkedListCDT * DoubleLinkedListADT;


/*
* @brief Crea una lista doblemente enlazada
* @return DoubleLinkedListADT
*/
DoubleLinkedListADT  createDoubleLinkedList();


/*
* @brief Inserta un elemento al principio de la lista
* @param list: Lista doblemente enlazada
* @param data: Dato a insertar
* @return 0: Si el dato se insertó correctamente
* @return -1: Si el dato no se insertó correctamente
*/
int insertFirst(DoubleLinkedListADT list, void *data);


/*
* @brief Inserta un elemento al final de la lista
* @param list: Lista doblemente enlazada
* @param data: Dato a insertar
* @return 0: Si el dato se insertó correctamente
* @return -1: Si el dato no se insertó correctamente
*/
int insertLast(DoubleLinkedListADT list, void *data);


/*
* @brief Elimina el primer elemento de la lista
* @param list: Lista doblemente enlazada
* @return 0: Si el elemento se eliminó correctamente
* @return -1: Si el elemento no se eliminó correctamente
*/
int removeFirst(DoubleLinkedListADT list);


/*
* @brief Elimina el último elemento de la lista
* @param list: Lista doblemente enlazada
* @return 0: Si el elemento se eliminó correctamente
* @return -1: Si el elemento no se eliminó correctamente
*/
int removeLast(DoubleLinkedListADT list);


/*
* @brief Elimina un elemento de la lista
* @param list: Lista doblemente enlazada
* @param data: Dato a eliminar
* @return 0: Si el elemento se eliminó correctamente
* @return -1: Si el elemento no se eliminó correctamente
*/
int removeElement(DoubleLinkedListADT list, void *data);


/*
* @brief Obtiene el primer elemento de la lista
* @param list: Lista doblemente enlazada
* @return Dato del primer elemento
*/
void * getFirst(DoubleLinkedListADT list);


/*
* @brief Obtiene el último elemento de la lista
* @param list: Lista doblemente enlazada
* @return Dato del último elemento
*/
void * getLast(DoubleLinkedListADT list);


/*
* @brief Obtiene el tamaño de la lista
* @param list: Lista doblemente enlazada
* @return Tamaño de la lista
*/
int getSize(DoubleLinkedListADT list);


/*
* @brief Verifica si la lista está vacía
* @param list: Lista doblemente enlazada
* @return 1: Si la lista está vacía
* @return 0: Si la lista no está vacía
*/
int isEmpty(DoubleLinkedListADT list);


/*
* @brief Libera la memoria de la lista
* @param list: Lista doblemente enlazada
*/
int freeList(DoubleLinkedListADT list);

#endif
