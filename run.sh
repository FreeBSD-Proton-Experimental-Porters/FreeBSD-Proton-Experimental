#!/bin/sh
__dir__="$(dirname "$(realpath "$0")")"
export LD_32_LIBRARY_PATH="$HOME/.i386-wine-pkg/usr/local/lib"
cp -r $HOME/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/share/openxr $HOME/.steam/tmp/FreeBSD_Proton/proton_10.0/files/share
exec lsu-linux-to-freebsd-env "$__dir__/run.rb" "$@"
