#!/bin/sh

# <<Important>> Be sure to install 'Proton Experimental' or 'Proton Experimental [bleeding-edge]' tools in Linux Steam. 

# For Proton Experimental 10 Scripts and DXVK + VKD3D-Proton builds
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/i386-windows/* ~/WineGfx/dxvk-master/x32
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/x86_64-windows/* ~/WineGfx/dxvk-master/x64
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/i386-windows/* ~/WineGfx/vkd3d-proton-master/x32
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/x86_64-windows/* ~/WineGfx/vkd3d-proton-master/x64

if ! [ -e ~/.steam/steam/steamapps/common/Proton\ 10.0.bak ]; then
    mv ~/.steam/steam/steamapps/common/Proton\ 10.0 ~/.steam/steam/steamapps/common/Proton\ 10.0.bak
    ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/ ~/.steam/steam/steamapps/common/Proton\ 10.0
fi


# Unused Symlinks for Proton Experimental 10 DXVK + VKD3D-Proton builds
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/i386-windows/* ~/.steam/steam/steamapps/common/Proton\ 10.0/files/lib/wine/dxvk/i386-windows
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/x86_64-windows/* ~/.steam/steam/steamapps/common/Proton\ 10.0/files/lib/wine/dxvk/x86_64-windows
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/i386-windows/* ~/.steam/steam/steamapps/common/Proton\ 10.0/files/lib/wine/vkd3d-proton/i386-windows
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/x86_64-windows/* ~/.steam/steam/steamapps/common/Proton\ 10.0/files/lib/wine/vkd3d-proton/x86_64-windows

# For Legacy Proton Experimental 9 DXVK + VKD3D-Proton builds
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/* ~/.steam/steam/steamapps/common/Proton\ 9.0\ \(Beta\)/files/lib/wine/dxvk
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib64/wine/dxvk/* ~/.steam/steam/steamapps/common/Proton\ 9.0\ \(Beta\)/files/lib64/wine/dxvk
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/* ~/.steam/steam/steamapps/common/Proton\ 9.0\ \(Beta\)/files/lib/wine/vkd3d-proton
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib64/wine/vkd3d-proton/* ~/.steam/steam/steamapps/common/Proton\ 9.0\ \(Beta\)/files/lib64/wine/vkd3d-proton
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/* ~/WineGfx/dxvk-master/x32
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib64/wine/dxvk/* ~/WineGfx/dxvk-master/x64
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/* ~/WineGfx/vkd3d-proton-master/x32
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib64/wine/vkd3d-proton/* ~/WineGfx/vkd3d-proton-master/x64
