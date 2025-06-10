# Script de Build y Ejecución para el Kernel (SO - ITBA)

Este script automatiza el proceso de compilación, limpieza y ejecución del kernel dentro de un contenedor Docker especialmente preparado para el entorno de desarrollo de la materia Sistemas Operativos del ITBA.

## Imagen Docker utilizada

El script utiliza la imagen:

**agodio/itba-so-multi-platform:3.0**


Esta imagen contiene todas las herramientas necesarias para compilar el kernel y sus utilidades auxiliares.

---

## Uso del script

```bash
./SO_BUILD.sh [opciones]
```

### Opciones disponibles

| Flag              | Descripción                                                                 |
|-------------------|-----------------------------------------------------------------------------|
| `--no-run`        | Evita ejecutar el kernel al finalizar la compilación.                       |
| `--mm=<tipo>`     | Define el tipo de administrador de memoria (`MM_TYPE`) a pasar a `make` (`buddy` o `bitmap`). |

---

## ¿Qué hace este script?

1. **Parsea los argumentos** recibidos y configura los flags correspondientes.
2. **Define el comando `make`** dinámicamente según los flags usados.
3. **Ejecuta el proceso completo** de:
   - `make clean` para `Toolchain` y el proyecto.
   - `make` para `Toolchain` y el proyecto, pasando variables si corresponde.
4. **Ajusta permisos** del archivo de imagen si se usa `--manu`.
5. **Ejecuta el kernel** usando `./run.sh`, a menos que se haya pasado `--no-run`.

---

## Ejemplos de uso

### Compilar y ejecutar normalmente:
```bash
./SO_BUILD.sh 
```

### Compilar sin ejecutar:
```bash
./SO_BUILD.sh --no-run
```

### Compilar con test de memoria y tipo de MM personalizado:
```bash
./SO_BUILD.sh --mm=buddy
```

---

## Requisitos

- Docker instalado y funcionando.
- El proyecto debe estar ubicado en el mismo directorio que el script.
- `run.sh` debe estar presente para ejecutar el kernel.

---

## Información relevante

- Para usar las mayúsculas, o los caracteres `|` y `&`, se debe utilizar la tecla Shift izquierda.
- El teclado está mapeado como un teclado en inglés.

---

## Combinaciones de teclas relevantes

- `CTRL + C` mata el proceso actual en foreground.
- `CTRL + D` envía un EOF (-1).
- `CTRL + M` imprime la memoria en el momento dado.

---

## Foreground y background

Para enviar procesos a background, utilice el operador `&` al final del comando:

```bash
loop 1 &
```

---

## GDB

- Para usar GDB deben instalar `gdb-multiarch`.

---