# FreeBSD Proton Experimental

**AI Disclosure:** _AI agents were used to assist with development._

### What is this?
> This repo contains Proton Experimental 11.0.
---

## Configuration (set these once)

Adjust these variables to match your system and FreeBSD version.

```
# FreeBSD version
# Examples:
#   14.4-RELEASE
#   15.0-RELEASE (15-CURRENT, pkgbase or FreeBSD 14.4 required for i386)
export FREEBSD_VERSION="15.0-RELEASE"
export FREEBSD_VERSION_I386="14.4-RELEASE"


# amd64 jail name
export FREEBSD_JAIL_AMD64=main-amd64

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
> Here are some instructions for building:

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

## Update the poudriere ports tree

```
poudriere ports -u
```

---

## Create an amd64 jail (classic method)

```
poudriere jail -c \
  -j ${FREEBSD_JAIL_AMD64} \
  -a amd64 \
  -v ${FREEBSD_VERSION}
```

---

## Create an amd64 jail (pkgbase method)

Recommended for FreeBSD 15.x.

```
poudriere jail -c \
  -j ${FREEBSD_JAIL_AMD64_PKGB} \
  -a amd64 \
  -m pkgbase=base_latest \
  -U https://pkg.freebsd.org/ \
  -v ${FREEBSD_VERSION}
```

---

## Jail management

List jails:
```
poudriere jail -l
```

Delete a jail:
```
poudriere -d -j ${FREEBSD_JAIL_AMD64}
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
git clone https://github.com/es-j3/FreeBSD-Proton-Experimental.git && cd FreeBSD-Proton-Experimental/emulators
cp -r /root/FreeBSD-Proton-Experimental/emulators/proton-experimental /usr/local/poudriere/ports/default/emulators/
```

---

## Build the amd64 package

```
poudriere bulk -j ${FREEBSD_JAIL_AMD64} \
  -b latest emulators/proton-experimental
```

Install it:
```
pkg install -y /usr/local/poudriere/data/packages/amd64-default/All/wine-proton-11-e.11.0.20260724.pkg
```
If you want to install with poudriere pkg.conf check the bottom of this README.

---

## 32-bit (i386) build

> Required for Proton.
> Note: i386 packages are **removed in FreeBSD 15**, so building with FreeBSD 14.4 is recommended.

### Create i386 jail

```
poudriere jail -c \
  -j ${FREEBSD_JAIL_I386} \
  -a i386 \
  -v ${FREEBSD_VERSION_I386}
```

---

### Create i386 jail (pkgbase)

```
poudriere jail -c \
  -j ${FREEBSD_JAIL_I386_PKGB} \
  -a i386 \
  -m pkgbase=base_latest \
  -U https://pkg.freebsd.org/ \
  -v ${FREEBSD_VERSION}
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
  -b latest emulators/proton-experimental
```

or (pkgbase):
```
poudriere bulk -j ${FREEBSD_JAIL_I386_PKGB} \
  -b latest emulators/proton-experimental
```

---

## Install the 32-bit package

```
/usr/local/wine-proton/bin/pkg32.sh install -y \
  -r FreeBSD-ports \
  /usr/local/poudriere/data/packages/i386-default/All/wine-proton-11-e.11.0.20260724.pkg
```

## Using poudriere pkg.conf
```
mkdir -p ~/poudriere-repos
ln -s /usr/local/poudriere/data/packages/${FREEBSD_JAIL_AMD64}/.latest /usr/local/poudriere-repos/FreeBSD:15:amd64
ln -s /usr/local/poudriere/data/packages/${FREEBSD_JAIL_I386_PKGB}/.latest /usr/local/poudriere-repos/FreeBSD:15:i386

cat > /usr/local/etc/pkg/repos/poudriere-wine-proton.conf << 'EOF'
poudriere-wine-proton: {
    url: "file:///usr/local/poudriere-repos/${ABI}",
    enabled: yes,
    priority: 20
}
EOF

pkg install wine-proton
/usr/local/wine-proton/bin/pkg32.sh install wine-proton
```


If that works - you’re done!

Thanks for reading!
