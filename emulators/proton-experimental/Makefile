PORTNAME=	wine-proton
DISTVERSION=	experimental-10.0-20250630
PORTREVISION=	0
CATEGORIES=	emulators

MAINTAINER=	tmp386@live.com
COMMENT=	Proton experimental port for FreeBSD with a cool patch.
WWW=		https://github.com/ValveSoftware/wine

LICENSE=	LGPL21 LGPL3
LICENSE_COMB=	dual
LICENSE_FILE=	${WRKSRC}/LICENSE

ONLY_FOR_ARCHS=	amd64 i386

BUILD_DEPENDS=	${LOCALBASE}/bin/flex:textproc/flex \
		${LOCALBASE}/ffmpeg4/include/libavformat/avformat.h:multimedia/ffmpeg4 \
		${LOCALBASE}/include/linux/input.h:devel/evdev-proto

LIB_DEPENDS=	libexpat.so:textproc/expat2 \
		libfontconfig.so:x11-fonts/fontconfig \
		libfreetype.so:print/freetype2 \
		libgnutls.so:security/gnutls \
		libvosk.so:audio/vosk-api \
		libvulkan.so:graphics/vulkan-loader

# amd_ags_x64.so
LIB_DEPENDS+=	libdrm.so:graphics/libdrm \
		libdrm_amdgpu.so:graphics/libdrm

# winewayland.so
LIB_DEPENDS+=	libwayland-client.so:graphics/wayland \
		libxkbcommon.so:x11/libxkbcommon \
		libxkbregistry.so:x11/libxkbcommon

RUN_DEPENDS+=	ffmpeg4>=0:multimedia/ffmpeg4 # we are avoiding LIB_DEPENDS because find-lib.sh brings ffmpeg 6, which is ridiculous

USES=		autoreconf bison desktop-file-utils gl gmake gnome gstreamer llvm:min=17,build,noexport localbase perl5 pkgconfig python:build sdl shebangfix tar:xz xorg
USE_GCC=	yes

USE_GITHUB=	yes
GH_ACCOUNT=	ValveSoftware
GH_PROJECT=	wine
GH_TAGNAME=	e134656c0b7fdca7abd8e8025f97a2366cc56fce

USE_GL=		gl
USE_GNOME=	glib20
USE_GSTREAMER=	gl good mpeg2dec x264 # https://github.com/ValveSoftware/wine/commit/8cdd506d7e68c27cd564c317320bfc8d3a999eda
USE_PERL5=	build
USE_SDL=	sdl2
USE_XORG=	x11 xext xcomposite xcursor xi xinerama xrandr xrender

NO_SHLIB_REQUIRES_GLOB= * # some of us never wanted that functionality in the first place

SHEBANG_FILES=	tools/make_requests tools/winemaker/winemaker \
		tools/winedump/function_grep.pl

GNU_CONFIGURE=		yes
GNU_CONFIGURE_PREFIX=	${PREFIX}/${PORTNAME}

CONFIGURE_ARGS=	--verbose \
		--with-mingw CROSSCC="clang" \
		--libdir=${PREFIX}/${PORTNAME}/lib \
		--disable-kerberos \
		--disable-tests \
                --with-expat \
		--with-fontconfig \
		--with-freetype \
		--with-gnutls \
		--with-gstreamer \
		--with-opengl \
		--with-oss \
		--with-pthread \
		--with-sdl \
		--with-vulkan \
		--with-wayland \
		--with-x \
		--with-xinerama \
		--with-xinput2 \
		--with-xrandr \
		--with-xrender \
		--without-alsa \
		--without-capi \
		--without-coreaudio \
		--without-cups \
		--without-dbus \
		--without-gettext \
		--without-gettextpo \
		--without-gphoto \
		--without-gssapi \
		--without-inotify \
		--without-krb5 \
		--without-netapi \
		--without-opencl \
		--without-osmesa \
		--without-pcap \
		--without-pcsclite \
		--without-sane \
		--without-udev \
		--without-unwind \
		--without-usb \
		--without-v4l2

CONFIGURE_ENV=	CPPBIN="false" CPPFLAGS="" FLEX="${LOCALBASE}/bin/flex" JXRLIB_CFLAGS="-I${LOCALBASE}/include/jxrlib" \
		PKG_CONFIG_PATH="${LOCALBASE}/ffmpeg4/libdata/pkgconfig"

CFLAGS+=	-D__NR_futex="-1" -Werror=implicit-function-declaration -Wno-int-conversion
CFLAGS_i386+=	-mpreferred-stack-boundary=4 -mstackrealign
PIE_UNSAFE=	yes

BINARY_ALIAS+=	clang=${LOCALBASE}/bin/clang${LLVM_VERSION} \
		lld-link=${LOCALBASE}/bin/lld-link${LLVM_VERSION}

OPTIONS_DEFINE=	PULSEAUDIO
OPTIONS_DEFAULT=PULSEAUDIO
OPTIONS_SUB=	yes

PULSEAUDIO_DESC=		Build winepulse.drv

PULSEAUDIO_BUILD_DEPENDS=	${LOCALBASE}/include/pulse/pulseaudio.h:audio/pulseaudio
PULSEAUDIO_CONFIGURE_ON=	--with-pulse
PULSEAUDIO_CONFIGURE_OFF=	--without-pulse

.include <bsd.port.pre.mk>

.if ${ARCH} == amd64
CONFIGURE_ARGS+=--enable-archs=i386,x86_64
PLIST_SUB+=	WINE32="@comment " WINE64="" WINEARCH="x86_64"
.else
CONFIGURE_ARGS+=--enable-archs=i386
PLIST_SUB+=	WINE32="" WINE64="@comment " WINEARCH="i386"
.endif

pre-configure:
	cd ${WRKSRC} && ${SETENV} ${MAKE_ENV} ${PYTHON_CMD} dlls/winevulkan/make_vulkan --xml vk.xml --video-xml video.xml
	cd ${WRKSRC} && ${PERL5} tools/make_specfiles

pre-build:
	cd ${WRKSRC} && ${MAKE_CMD} depend
	cd ${WRKSRC} && ${PERL5} tools/make_requests

post-install:
.if ${ARCH} == i386
	${MV} ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/wineserver ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/wineserver32
	${MV} ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/wine       ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/wine.bin
	-${ELFCTL} -e +noaslr                                 ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/wine.bin
	${INSTALL_SCRIPT} ${FILESDIR}/wine.sh                 ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/wine
.else
	${INSTALL_SCRIPT} ${FILESDIR}/pkg32.sh                ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/pkg32.sh
	${MV} ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/wine       ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/wine64.bin
	${INSTALL_SCRIPT} ${FILESDIR}/wine.sh                 ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/wine64
	${INSTALL_SCRIPT} ${FILESDIR}/wine-wow64.sh           ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/wine
	${MKDIR} ${STAGEDIR}${PREFIX}/${PORTNAME}/bin-wow64
	${CP} ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/msidb      ${STAGEDIR}${PREFIX}/${PORTNAME}/bin-wow64/msidb
	${CP} ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/wine64.bin ${STAGEDIR}${PREFIX}/${PORTNAME}/bin-wow64/wine
	${CP} ${STAGEDIR}${PREFIX}/${PORTNAME}/bin/wineserver ${STAGEDIR}${PREFIX}/${PORTNAME}/bin-wow64/wineserver
.endif
	${RM} -r ${STAGEDIR}${PREFIX}/${PORTNAME}/include
	${RM} -r ${STAGEDIR}${PREFIX}/share/man
	${MKDIR} ${STAGEDIR}${DOCSDIR}
.for i in README.md ANNOUNCE.md AUTHORS
	${INSTALL_DATA} ${WRKSRC}/${i} ${STAGEDIR}${DOCSDIR}
.endfor

.include <bsd.port.post.mk>
