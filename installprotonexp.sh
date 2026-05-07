#!/bin/sh

# <<Important>> Be sure to install 'Proton Experimental' or 'Proton Experimental [bleeding-edge]' tools in Linux Steam. 

# For Proton Experimental 11 Scripts and DXVK + VKD3D-Proton builds
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/i386-windows/* ~/WineGfx/dxvk-master/x32
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/dxvk/x86_64-windows/* ~/WineGfx/dxvk-master/x64
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/i386-windows/* ~/WineGfx/vkd3d-proton-master/x32
#ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/lib/wine/vkd3d-proton/x86_64-windows/* ~/WineGfx/vkd3d-proton-master/x64

if ! [ -e ~/.steam/steam/steamapps/common/Proton\ 11.0.bak ]; then
    mv ~/.steam/steam/steamapps/common/Proton\ 11.0 ~/.steam/steam/steamapps/common/Proton\ 11.0.bak
    ln -sf ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/ ~/.steam/steam/steamapps/common/Proton\ 11.0
fi
