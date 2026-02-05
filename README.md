### FreeBSD Proton Experimental

### What is this?
> This repo is simply wine-proton but bumped to the experimental version.

> This also includes the https://gitlab.winehq.org/wine/wine/-/merge_requests/7339/diffs patch, which fixes Unreal Engine games and some Unity ones too.

---

## Configuration (set these once)

Adjust these variables to match your system and FreeBSD version.

```
# FreeBSD version
# Examples:
#   14.3-RELEASE (14-STABLE)
#   15.0-RELEASE (15-CURRENT, pkgbase required for i386)
export FREEBSD_VERSION="15.0-RELEASE"

# amd64 jail name
export FREEBSD_JAIL_AMD64="amd64"

# i386 jail name
export FREEBSD_JAIL_I386="i386"

#export FREEBSD_JAIL_AMD64_PKGB="14-stable-amd64"
#export FREEBSD_JAIL_I386_PKGB="14-stable-i386"

# pkgbase jail names FreeBSD 15-CURRENT
export FREEBSD_JAIL_AMD64_PKGB="main-amd64"
export FREEBSD_JAIL_I386_PKGB="main-i386"
```

---

## How to use?
> Feel free to use the pre-built packages, available in the releases tab. Security-conscious users may want to build it themselves, so here are some instructions for that:

> Make sure you have git before continuing.

---

## Install poudriere

```
pkg install poudriere
```

This is a good time to go over the poudriere.conf since default settings may take a very long time to build and may use a lot of RAM.

Modify `/usr/local/etc/poudriere.conf`:
```
ZPOOL=zroot
USE_TMPFS=yes
TMPFS_BLACKLIST="rust"
TMPFS_BLACKLIST_TMPDIR=${BASEFS}/data/cache/tmp
MAX_MEMORY=16
PARALLEL_JOBS=1
ALLOW_MAKE_JOBS=YES
```

- `ZPOOL` - Set this to your zpool if using ZFS
- `TMPFS` - Can use a lot of RAM (Rust is a common offender)
- `TMPDIR` - Required if TMPFS is disabled or partially disabled
- `MAX_MEMORY` - RAM limit inside the jail
- `PARALLEL_JOBS` - Ports built at once
- `ALLOW_MAKE_JOBS` - Cores per port build

---

## Clone the poudriere ports tree

```
poudriere ports -c
```

---

## Create an amd64 jail (classic method)

```
poudriere jail -c -j ${FREEBSD_JAIL_AMD64} -v ${FREEBSD_VERSION} -a amd64
```

---

## Create an amd64 jail (pkgbase method)

Recommended for FreeBSD 15.x.

```
poudriere jail -c \
  -j ${FREEBSD_JAIL_AMD64_PKGB} \
  -v ${FREEBSD_VERSION} \
  -a amd64 \
  -m pkgbase=base_latest \
  -U https://pkg.freebsd.org/
```

---

## Jail management

List jails:
```
poudriere -l
```

Delete a jail:
```
poudriere -d ${FREEBSD_JAIL_AMD64}
```

Update an existing jail:
```
poudriere jail -u -j ${FREEBSD_JAIL_AMD64}
```

or (pkgbase):
```
poudriere jail -u -j ${FREEBSD_JAIL_AMD64_PKGB}
```

---

## Clone this repo and poudriere overlay the port

```
cd ~
git clone https://github.com/es-j3/FreeBSD-Proton-Experimental.git
poudriere overlay -c -n proton-experimental-overlay -m null -p ~/FreeBSD-Proton-Experimental
```

---

## Build the amd64 package

```
poudriere bulk -j ${FREEBSD_JAIL_AMD64} \
  -b latest emulators/proton-experimental \
  -O proton-experimental-overlay
```

or (pkgbase):
```
poudriere bulk -j ${FREEBSD_JAIL_AMD64_PKGB} \
  -b latest emulators/proton-experimental \
  -O proton-experimental-overlay \
  -m pkgbase=base_latest \
  -U https://pkg.freebsd.org/
```

Install it:
```
pkg install -y /usr/local/poudriere/data/packages/amd64-default/All/wine-proton-e.10.0.20260127,1.pkg
```

---

## 32-bit (i386) build

> Required for Proton.  
> Note: i386 packages are **removed in FreeBSD 15**, so pkgbase is required there.

### Create i386 jail (classic, FreeBSD 14.x only)

```
poudriere jail -c -j ${FREEBSD_JAIL_I386} -v ${FREEBSD_VERSION} -a i386
```

---

### Create i386 jail (pkgbase)

```
poudriere jail -c \
  -j ${FREEBSD_JAIL_I386_PKGB} \
  -v ${FREEBSD_VERSION} \
  -a i386 \
  -m pkgbase=base_latest \
  -U https://pkg.freebsd.org/
```

Update if needed:
```
poudriere jail -u -j ${FREEBSD_JAIL_I386}
```

or (pkgbase):
```
poudriere jail -u -j ${FREEBSD_JAIL_I386_PKGB}
```

---

## Build the i386 package

```
poudriere bulk -j ${FREEBSD_JAIL_I386} \
  -b latest emulators/proton-experimental \
  -O proton-experimental-overlay
```

or (pkgbase):
```
poudriere bulk -j ${FREEBSD_JAIL_I386_PKGB} \
  -b latest emulators/proton-experimental \
  -O proton-experimental-overlay
```

---

## Install the 32-bit package

```
/usr/local/wine-proton/bin/pkg32.sh install -y \
  -r FreeBSD-ports \
  /usr/local/poudriere/data/packages/i386-default/All/wine-proton-e.10.0.20260127,1.pkg
```

If that works - you’re done!

Thanks for reading!
