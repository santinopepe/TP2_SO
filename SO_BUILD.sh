#!/bin/bash
set -e  # Termina el script si algún comando falla

# Imagen de Docker
IMAGE_NAME="agodio/itba-so-multi-platform:3.0"
PROJECT_PATH="/root"
SKIP_RUN=false
MM_TYPE_ARG="" # Variable para almacenar el tipo de MM especificado en el argumento

# Verificar argumentos del script
# Recorre todos los argumentos pasados al script
for arg in "$@"; do
  if [[ "$arg" == "--no-run" ]]; then
    SKIP_RUN=true
  elif [[ "$arg" == "--mm="* ]]; then # <--- NUEVO: Parsear argumento --mm=
    MM_TYPE_ARG="${arg#--mm=}" # Extrae el valor después de '--mm=' y lo guarda
    echo ">>> Tipo de Administrador de Memoria especificado: ${MM_TYPE_ARG}"
  fi
done

# Inicializa MAKE_COMMAND con el comando base y el directorio del proyecto
MAKE_COMMAND="make -C ${PROJECT_PATH}"


if [ -n "$MM_TYPE_ARG" ]; then 
  MAKE_COMMAND="${MAKE_COMMAND} MM_TYPE=${MM_TYPE_ARG}"
  echo ">>> Pasando MM_TYPE=${MM_TYPE_ARG} a make..."
fi

# Ejecuta los comandos dentro del contenedor Docker
docker run --rm -v "${PWD}:/root" --privileged -ti "$IMAGE_NAME" bash -c "
    make clean -C ${PROJECT_PATH}/Toolchain
    ${MAKE_COMMAND} clean # Usa la variable MAKE_COMMAND para limpiar también
    make -C ${PROJECT_PATH}/Toolchain
    ${MAKE_COMMAND} # <--- Ejecuta el comando make final con las variables incluidas
"

clear

if [ "$SKIP_RUN" = false ]; then
  echo ">>> Ejecutando el kernel usando run.sh..."
  ./run.sh
else
  echo ">>> Ejecución de run.sh omitida (--no-run)"
fi