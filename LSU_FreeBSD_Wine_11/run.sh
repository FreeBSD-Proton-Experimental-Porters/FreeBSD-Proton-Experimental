#!/bin/sh
__dir__="$(dirname "$(realpath "$0")")"
export WINEDLLOVERRIDES="vrclient,vrclient_x64=" # Bypass assert failed popup window when using Proton 11 in LSU
export LD_32_LIBRARY_PATH="$HOME/.i386-wine-pkg/usr/local/lib" # Enable LSU to locate additional 32-bit libraries
ln -sf /usr/local/wine-proton-11/lib/wine/x86_64-unix/wine $HOME/.steam/tmp/FreeBSD_Proton/proton_11.0/files/lib/wine/x86_64-unix
cp -r "$HOME/.steam/steam/steamapps/common/Proton 11.0/files/share/openxr" $HOME/.steam/tmp/FreeBSD_Proton/proton_11.0/files/share
exec lsu-linux-to-freebsd-env "$__dir__/run.rb" "$@"
