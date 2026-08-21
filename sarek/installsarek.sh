#!/bin/sh

# This script installs and configures DXVK-Sarek and D7VK for Proton Experimental 11
# https://github.com/pythonlover02/DXVK-Sarek
# https://github.com/WinterSnowfall/d7vk

SAREK_VER="1.12.0"
D7VK_VER="2.1"
DIR=$(dirname "$(realpath "$0")")
PROTON_EXP_PATH="$HOME/.steam/steam/steamapps/common/Proton - Experimental"

setup() {
    local name=${1}
    local gitproj=${2}
    local filename=${3}

    fetch -q ${gitproj}/${filename}
    rm -rf "${PROTON_EXP_PATH}/files/lib/wine/${name}"
    mkdir "${PROTON_EXP_PATH}/files/lib/wine/${name}"
    bsdtar -xf ${filename} --strip-components=1 -C "${PROTON_EXP_PATH}/files/lib/wine/${name}"
    mv "${PROTON_EXP_PATH}/files/lib/wine/${name}/x32" "${PROTON_EXP_PATH}/files/lib/wine/${name}/i386-windows"
    mv "${PROTON_EXP_PATH}/files/lib/wine/${name}/x64" "${PROTON_EXP_PATH}/files/lib/wine/${name}/x86_64-windows" 2> /dev/null
    rm ${filename}
}

cp "$DIR"/protonsarek.patch "${PROTON_EXP_PATH}" && cd "${PROTON_EXP_PATH}"
patch -p1 < protonsarek.patch

setup dxvk-sarek https://github.com/pythonlover02/DXVK-Sarek/releases/download/v${SAREK_VER} dxvk-sarek-dyasync-v${SAREK_VER}.tar.gz
setup d7vk https://github.com/WinterSnowfall/d7vk/releases/download/v${D7VK_VER} d7vk-v${D7VK_VER}.zip

echo "Installation complete."
