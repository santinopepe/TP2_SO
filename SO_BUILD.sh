#!/bin/bash
set -e  # Termina el script si algún comando falla

# Imagen de Docker
IMAGE_NAME="agodio/itba-so-multi-platform:3.0"
PROJECT_PATH="/root"
SKIP_RUN=false

# Verificar argumentos
for arg in "$@"; do
  if [[ $arg == "--no-run" ]]; then
    SKIP_RUN=true
  fi
done


docker run --rm -v "${PWD}:/root" --privileged -ti "$IMAGE_NAME" bash -c "
    make clean -C ${PROJECT_PATH}/Toolchain
    make clean -C ${PROJECT_PATH}
    make -C ${PROJECT_PATH}/Toolchain
    make -C ${PROJECT_PATH}
"

clear

if [ "$SKIP_RUN" = false ]; then
  ./run.sh
else
  echo " Ejecución de run.sh omitida (--no-run)"
fi

