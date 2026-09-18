#!/bin/sh

# This script activates Proton Experimental 11 and symlink DXVK and VKD3D-Proton for use outside of Steam.
# <<Important>> Be sure to install 'Proton Experimental' or 'Proton Experimental [bleeding-edge]' tools in Linux Steam.

STEAM_APPS="$HOME/.steam/steam/steamapps/common"
EXP_LIB="$STEAM_APPS/Proton - Experimental/files/lib/wine"
GFX="$HOME/WineGfx"

mkdir -p "$GFX/dxvk-master/x32" "$GFX/dxvk-master/x64"
mkdir -p "$GFX/vkd3d-proton-master/x32" "$GFX/vkd3d-proton-master/x64"

# Symlinks for Proton Experimental DXVK + VKD3D-Proton builds
ln -sf "$EXP_LIB/dxvk/i386-windows/"*        "$GFX/dxvk-master/x32/"
ln -sf "$EXP_LIB/dxvk/x86_64-windows/"*      "$GFX/dxvk-master/x64/"
ln -sf "$EXP_LIB/vkd3d-proton/i386-windows/"*   "$GFX/vkd3d-proton-master/x32/"
ln -sf "$EXP_LIB/vkd3d-proton/x86_64-windows/"* "$GFX/vkd3d-proton-master/x64/"

# Backup Proton 11.0 and symlink to Proton Experimental if backup does not exist
PE="$STEAM_APPS/Proton - Experimental"
P9="$STEAM_APPS/Proton 9.0 (Beta)"
P10="$STEAM_APPS/Proton 10.0"
P11="$STEAM_APPS/Proton 11.0"

if [ ! -e "${P11}.bak" ]; then
    find "$P9" "$P10" "$P11" "$PE" -type d ! -name "proton" -exec chmod 755 {} +
    mv "$P11" "${P11}.bak"
    ln -sf "$PE" "$P11"
    find "$P9" "$P10" "$P11" "$PE" -type f ! -name "proton" -exec chmod 555 {} +
fi

# Symlinks for Proton Experimental 10 DXVK + VKD3D-Proton builds
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/i386-windows/* ~/.steam/steam/steamapps/common/Proton\ 10.0/files/lib/wine/dxvk/i386-windows
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/x86_64-windows/* ~/.steam/steam/steamapps/common/Proton\ 10.0/files/lib/wine/dxvk/x86_64-windows
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/i386-windows/* ~/.steam/steam/steamapps/common/Proton\ 10.0/files/lib/wine/vkd3d-proton/i386-windows
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/x86_64-windows/* ~/.steam/steam/steamapps/common/Proton\ 10.0/files/lib/wine/vkd3d-proton/x86_64-windows

# Symlinks for Proton Experimental 9 DXVK + VKD3D-Proton builds
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/i386-windows/* ~/.steam/steam/steamapps/common/Proton\ 9.0\ \(Beta\)/files/lib/wine/dxvk
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/x86_64-windows/* ~/.steam/steam/steamapps/common/Proton\ 9.0\ \(Beta\)/files/lib64/wine/dxvk
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/i386-windows/* ~/.steam/steam/steamapps/common/Proton\ 9.0\ \(Beta\)/files/lib/wine/vkd3d-proton
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/x86_64-windows/* ~/.steam/steam/steamapps/common/Proton\ 9.0\ \(Beta\)/files/lib64/wine/vkd3d-proton

# Legacy Symlinks for Proton Experimental 9 DXVK + VKD3D-Proton builds
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/* ~/.steam/steam/steamapps/common/Proton\ 9.0\ \(Beta\)/files/lib/wine/dxvk
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib64/wine/dxvk/* ~/.steam/steam/steamapps/common/Proton\ 9.0\ \(Beta\)/files/lib64/wine/dxvk
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/* ~/.steam/steam/steamapps/common/Proton\ 9.0\ \(Beta\)/files/lib/wine/vkd3d-proton
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib64/wine/vkd3d-proton/* ~/.steam/steam/steamapps/common/Proton\ 9.0\ \(Beta\)/files/lib64/wine/vkd3d-proton
