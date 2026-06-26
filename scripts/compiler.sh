#!/bin/bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"

usage() {
    cat << 'EOF'
Usage: compiler.sh [comando]

Comandos:
    test                Ejecuta todos los tests
    test <regex>        Ejecuta tests que hagan match con regex
    rebuild             Limpia build/ y recompila desde cero
    list                Lista los tests disponibles
    install             Instala el proyecto (cmake --install)
    clean               Limpiar artefactos del build
    help                Muestra esta ayuda
EOF
}

cmd="${1:-''}"

configure_and_build() {
    if [ ! -d "$BUILD_DIR" ]; then
        echo "--> Creando directorio de build y configurando..."
        cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
    else
        cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release --log-level=WARNING
    fi
    echo "--> Compilando cambios..."
    cmake --build "$BUILD_DIR" -j
}

force_rebuild() {
    echo "--> Forzando recompilación completa..."
    rm -rf "$BUILD_DIR"
    configure_and_build
}

case "$cmd" in
    help|-h|--help)
        usage
        ;;
    test)
        configure_and_build
        if [[ $# -ge 2 ]]; then
            # segundo argumento = regex o nombre de test
            regex="$2"
            (cd "$BUILD_DIR" && ctest -R "$regex" --output-on-failure --timeout 0)
        else
            (cd "$BUILD_DIR" && ctest --output-on-failure --timeout 0)
        fi
        ;;
    rebuild)
        force_rebuild
        ;;
    list)
        configure_and_build
        (cd "$BUILD_DIR" && ctest -N)
        ;;
    install)
        configure_and_build
        (cd "$BUILD_DIR" && sudo cmake --install .)
        ;;
    clean)
        bash "$ROOT_DIR/scripts/compiler_cleaner.sh"
        ;;
    *)
        echo "Comando no válido: $cmd"
        usage
        ;;
esac
