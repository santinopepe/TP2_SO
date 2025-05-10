#include <test_list.h>
#include <video.h>
#include <lib.h>

#define OK_MSG "[OK]\n"
#define FAIL_MSG "[FAIL]\n"

uint64_t test_list(uint64_t argc, char *argv[])
{
    int fails = 0;
    DoubleLinkedListADT list = createDoubleLinkedList();
    if (list == NULL)
    {
        print("Error al crear la lista\n" FAIL_MSG);
        return -1;
    }
    print("Lista creada correctamente\n" OK_MSG);

    // Test: lista vacía
    if (!isEmpty(list))
    {
        print("Fallo: lista no vacía al crear\n" FAIL_MSG);
        fails++;
    }
    if (getSize(list) != 0)
    {
        print("Fallo: tamaño no es 0\n" FAIL_MSG);
        fails++;
    }
    if (getFirst(list) != NULL)
    {
        print("Fallo: getFirst no es NULL\n" FAIL_MSG);
        fails++;
    }
    if (getLast(list) != NULL)
    {
        print("Fallo: getLast no es NULL\n" FAIL_MSG);
        fails++;
    }

    // Insertar elementos
    insertFirst(list, "A");
    insertLast(list, "B");
    insertFirst(list, "C"); // Lista: C, A, B

    if (getSize(list) != 3)
    {
        print("Fallo: tamaño no es 3\n" FAIL_MSG);
        fails++;
    }
    if (getFirst(list) != (void *)"C")
    {
        print("Fallo: getFirst no es C\n" FAIL_MSG);
        fails++;
    }
    if (getLast(list) != (void *)"B")
    {
        print("Fallo: getLast no es B\n" FAIL_MSG);
        fails++;
    }
    if (isEmpty(list))
    {
        print("Fallo: lista vacía tras insertar\n" FAIL_MSG);
        fails++;
    }

    // Eliminar primero
    removeFirst(list); // Lista: A, B
    if (getFirst(list) != (void *)"A")
    {
        print("Fallo: getFirst no es A tras removeFirst\n" FAIL_MSG);
        fails++;
    }
    if (getSize(list) != 2)
    {
        print("Fallo: tamaño no es 2 tras removeFirst\n" FAIL_MSG);
        fails++;
    }

    // Eliminar último
    removeLast(list); // Lista: A
    if (getLast(list) != (void *)"A")
    {
        print("Fallo: getLast no es A tras removeLast\n" FAIL_MSG);
        fails++;
    }
    if (getSize(list) != 1)
    {
        print("Fallo: tamaño no es 1 tras removeLast\n" FAIL_MSG);
        fails++;
    }

    // Eliminar único elemento
    removeFirst(list); // Lista vacía
    if (!isEmpty(list))
    {
        print("Fallo: lista no vacía tras eliminar todo\n" FAIL_MSG);
        fails++;
    }
    if (getSize(list) != 0)
    {
        print("Fallo: tamaño no es 0 tras eliminar todo\n" FAIL_MSG);
        fails++;
    }

    // Insertar y eliminar por valor
    insertLast(list, "X");
    insertLast(list, "Y");
    insertLast(list, "Z");    // Lista: X, Y, Z
    removeElement(list, "Y"); // Lista: X, Z
    if (getSize(list) != 2)
    {
        print("Fallo: tamaño no es 2 tras removeElement\n" FAIL_MSG);
        fails++;
    }
    if (getFirst(list) != (void *)"X")
    {
        print("Fallo: getFirst no es X tras removeElement\n" FAIL_MSG);
        fails++;
    }
    if (getLast(list) != (void *)"Z")
    {
        print("Fallo: getLast no es Z tras removeElement\n" FAIL_MSG);
        fails++;
    }

    // Eliminar elementos inexistentes
    if (removeElement(list, "Q") != -1)
    {
        print("Fallo: removeElement de inexistente no da -1\n" FAIL_MSG);
        fails++;
    }

    // Liberar lista
    if (freeList(list) != 0)
    {
        print("Fallo: freeList no retorna 0\n" FAIL_MSG);
        fails++;
    }
    else
        print("freeList ejecutado correctamente\n" OK_MSG);

    if (fails == 0)
        print("Todos los tests de DoubleLinkedListADT PASARON\n");
    else
    {
        print("FALLARON tests de DoubleLinkedListADT: ");
        char buf[8];
        itoa(fails, buf, 10);
        print(buf);
        print(" fallos\n");
    }

    return fails;
}
