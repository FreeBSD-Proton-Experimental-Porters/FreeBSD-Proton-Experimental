#!/bin/sh
__dir__="$(dirname "$(realpath "$0")")"
export LD_32_LIBRARY_PATH="$HOME/.i386-wine-pkg/usr/local/lib"
exec lsu-linux-to-freebsd-env "$__dir__/run.rb" "$@"
