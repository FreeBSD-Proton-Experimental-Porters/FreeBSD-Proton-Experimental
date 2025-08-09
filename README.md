### FreeBSD Proton Experimental

### What is this?
> This repo is simply wine-proton but bumped to the experimental version.

> This also includes the https://gitlab.winehq.org/wine/wine/-/merge_requests/7339/diffs patch, which fixes Unreal Engine games and some Unity ones too.

## How to use?
> Feel free to use the pre-built packages, available in the releases tab. Security-conscious users may want to build it themselves, so here are some instructions for that:

> Make sure you have git before continuing.

First, install poudriere:
```
pkg install poudriere
```

If using ZFS, let's go ahead and set our zpool to zroot, or whatever you may have set it to.
```
su -l root -c 'echo ZPOOL=zroot >> /usr/local/etc/poudriere.conf'
```

Let's now clone the poudriere-side FreeBSD Ports Tree
```
poudriere ports -c
```

Now, we a poudriere jail for FreeBSD 14.x-RELEASE: 
```
poudriere jail -c -j amd64 -v 14.3-RELEASE -a amd64
```
You may change the RELEASE part to whatever your FreeBSD version is.

*or* using the pkgbase method for FreeBSD 14-STABLE: 
```
poudriere jail -c -v 14 -j 14-stable-amd64 -a amd64 -m pkgbase=base_latest -U https://pkg.freebsd.org/
```

*or* using the pkgbase method for FreeBSD 15-CURRENT: 
```
poudriere jail -c -v 15 -j main-amd64 -a amd64 -m pkgbase=base_latest -U https://pkg.freebsd.org/
```

If a jail already exists and needs updating, use 
```
poudriere jail -u -j amd64
``` 
for FreeBSD 14.x-RELEASE, 
```
poudriere jail -u -j 14-stable-amd64
``` 
for FreeBSD 14-STABLE, or 
```
poudriere jail -u -j main-amd64
``` 
for FreeBSD 15-CURRENT.

Now, let's clone this repo, cd into it, and the emulators/ folder.
```
git clone https://github.com/es-j3/FreeBSD-Proton-Experimental.git && cd FreeBSD-Proton-Experimental/emulators
``` 

Copy the folder to the poudriere ports tree 
```
cp -r proton-experimental /usr/local/poudriere/ports/default/emulators/proton-experimental
``` 

we can start building the package 
```
poudriere bulk -j amd64 -b latest emulators/proton-experimental
``` 
but replace ```amd64``` with ```14-stable-amd64``` for FreeBSD 14-STABLE or ```main-amd64``` for FreeBSD 15-CURRENT.

Once it finishes building (hopefully,) then run 
```
pkg install -y /usr/local/poudriere/data/packages/amd64-default/All/wine-proton-e.10.0.20250805,1.pkg
``` 
If that works, congratulations. 

We're not done yet, because we still need to install the 32 bit version.

Create a 32 bit poudriere jail for FreeBSD 14.x-RELEASE: 
```
poudriere jail -c -j i386 -v 14.3-RELEASE -a i386
``` 
you may change the RELEASE part to whatever your FreeBSD version is

*or* using the pkgbase method for FreeBSD 14-STABLE: 
```
poudriere jail -c -v 14 -j 14-stable-i386 -a i386 -m pkgbase=base_latest -U https://pkg.freebsd.org/
```

*or* using the pkgbase method for FreeBSD 15-CURRENT: 
```
poudriere jail -c -v 15 -j main-i386 -a i386 -m pkgbase=base_latest -U https://pkg.freebsd.org/
```

if a jail already exists and needs updating use 
```
poudriere jail -u -j i386
``` 
for FreeBSD 14.x-RELEASE, 
```
poudriere jail -u -j 14-stable-i386
``` 
for FreeBSD 14-STABLE or 
```
poudriere jail -u -j main-i386
``` 
for FreeBSD 15-CURRENT

We can now build the i386 version of the package 
```
poudriere bulk -j i386 -b latest emulators/proton-experimental
``` 
but replace ```i386``` with ```14-stable-i386``` for FreeBSD 14-STABLE or ```main-i386``` for FreeBSD 15-CURRENT

After that, run: 
```
/usr/local/wine-proton/bin/pkg32.sh install -y /usr/local/poudriere/data/packages/i386-default/All/wine-proton-e.10.0.20250805,1.pkg
``` 
and you should be all good

Thanks for reading!
