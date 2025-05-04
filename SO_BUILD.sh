#!/bin/bash
set -e  # Termina el script si algún comando falla

# Imagen de Docker
IMAGE_NAME="agodio/itba-so-multi-platform:3.0"
PROJECT_PATH="/root"
SKIP_RUN=false
BUILD_TEST_MM=false

# Verificar argumentos del script
for arg in "$@"; do
  if [[ "$arg" == "--no-run" ]]; then
    SKIP_RUN=true
  elif [[ "$arg" == "--test-mm" ]]; then
    BUILD_TEST_MM=true
  fi
done

# <-- CORRECCIÓN 1: Inicializa MAKE_COMMAND con el comando base ANTES del if
MAKE_COMMAND="make -C ${PROJECT_PATH}"

# Si el flag BUILD_TEST_MM está activado, añadir TEST=mm a la variable MAKE_COMMAND
if [ "$BUILD_TEST_MM" = true ]; then
  MAKE_COMMAND="${MAKE_COMMAND} TEST=mm" # <-- Ahora esto concatena correctamente
  echo ">>> Building kernel with TEST_MM enabled (passing TEST=mm to make)..."
else
  echo ">>> Building kernel without TEST_MM enabled..."
fi


docker run --rm -v "${PWD}:/root" --privileged -e TERM=xterm "$IMAGE_NAME" bash -c "
    set -e
    echo '>>> Cleaning Toolchain...'
    make clean -C ${PROJECT_PATH}/Toolchain
    echo '>>> Cleaning Project...'
    make clean -C ${PROJECT_PATH}
    echo '>>> Building Toolchain...'
    make -C ${PROJECT_PATH}/Toolchain
    echo '>>> Building Project using command: ${MAKE_COMMAND}'
    ${MAKE_COMMAND}
    echo '>>> Build finished inside Docker.'
    exit 0
"
# Limpia la terminal del host (opcional)
clear

if [ "$SKIP_RUN" = false ]; then
  echo ">>> Running the kernel using run.sh..."
  ./run.sh
else
  echo ">>> Ejecución de run.sh omitida (--no-run)"
fi