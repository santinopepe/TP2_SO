# Script de Build y Ejecución para el Kernel (SO - ITBA)

Este script automatiza el proceso de compilación, limpieza y ejecución del kernel dentro de un contenedor Docker especialmente preparado para el entorno de desarrollo de la materia Sistemas Operativos del ITBA.

## Imagen Docker Utilizada

El script utiliza la imagen:

```
agodio/itba-so-multi-platform:3.0
```

Esta imagen contiene todas las herramientas necesarias para compilar el kernel y sus herramientas auxiliares.

---

## ⚙️ Uso del Script

```bash
./build.sh [opciones]
```

### Opciones disponibles:

| Flag              | Descripción                                                                 |
|-------------------|-----------------------------------------------------------------------------|
| `--no-run`        | Evita ejecutar el kernel al finalizar la compilación.                      |
| `--test-mm`       | Compila el kernel con tests de manejo de memoria (`TEST=mm`).              |
| `--test-list`     | Compila el kernel con tests de listas (`TEST=list`).                       |
| `--mm=<tipo>`     | Define el tipo de administrador de memoria (`MM_TYPE`) a pasar a `make`.   |

> ⚠️ Los flags pueden combinarse según necesidad.

---

## ¿Qué hace este script?

1. **Parsea los argumentos** recibidos y setea los flags correspondientes.
2. **Define el comando `make`** dinámicamente según los flags usados.
3. **Ejecuta el proceso completo** de:
   - `make clean` para `Toolchain` y el proyecto
   - `make` para `Toolchain` y el proyecto, pasando variables si corresponde
4. **Ajusta permisos** del archivo de imagen si se usa `--manu`.
5. **Ejecuta el kernel** usando `./run.sh`, a menos que se haya pasado `--no-run`.

---

##  Ejemplos de uso

### Compilar y ejecutar normalmente:
```bash
./build.sh
```

### Compilar sin ejecutar:
```bash
./build.sh --no-run
```

### Compilar con test de memoria y tipo de MM personalizado:
```bash
./build.sh --test-mm --mm=buddy
```
---

##  Requisitos

- Docker instalado y funcionando.
- El proyecto debe estar ubicado en el mismo directorio que el script.
- `run.sh` debe estar presente para ejecutar el kernel.

---

##  Notas

- El directorio del proyecto dentro del contenedor está fijado en `/root`.
- Este script utiliza el flag `--privileged` para permitir acceso extendido a Docker.

---

## GDB
- Para usar GDB deben instalar gdb-multiarch

