#########################################################################################################################
# Special libs
#########################################################################################################################

# Defaults
SHARED_EXT=so
SHARED_LDFLAGS=-shared
SHARED_CFLAGS=-fPIC

# MinGW32
ifeq ($(TARGET_OS),mingw32)
	LIBS+=-lwsock32 -lwinmm -lkernel32 -luser32 -lgdi32
	SHARED_EXT=dll
	SHARED_LDFLAGS=-shared
	SHARED_CFLAGS=-shared
	CFLAGS+=
	SERVERLIBS+=
	GAMELIBS+=
endif

# Linux like
ifeq ($(TARGET_OS),linux-gnu)
	CFLAGS +=-D_GNU_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE
endif

# FreeBSD like
ifeq ($(TARGET_OS),freebsd)
	CFLAGS +=-D_BSD_SOURCE -D_XOPEN_SOURCE
endif

ifeq ($(TARGET_OS),netbsd)
	CFLAGS +=-I/usr/X11R6/include -D_BSD_SOURCE -D_NETBSD_SOURCE
	SERVERLIBS +=
	GAMELIBS +=
endif

# Darwin
ifeq ($(TARGET_OS),darwin)
	SHARED_EXT=dylib
	SHARED_CFLAGS=-fPIC -fno-common
	SHARED_LDFLAGS=-dynamiclib
	LDFLAGS += -framework SDL -framework OpenGL
	CFLAGS += -D_BSD_SOURCE -D_XOPEN_SOURCE
endif

#########################################################################################################################
# Release flags
#########################################################################################################################

RELEASE_CFLAGS=-ffast-math -funroll-loops -D_FORTIFY_SOURCE=2

#ifeq ($(TARGET_CPU),axp)
#	RELEASE_CFLAGS+=-fomit-frame-pointer -fexpensive-optimizations
#endif

#ifeq ($(TARGET_CPU),sparc)
#	RELEASE_CFLAGS+=-fomit-frame-pointer -fexpensive-optimizations
#endif

ifeq ($(TARGET_CPU),powerpc)
	RELEASE_CFLAGS+=-O2 -fomit-frame-pointer -fexpensive-optimizations
	CFLAGS+=-DALIGN_BYTES=4
endif

ifeq ($(TARGET_CPU),i386)
	ifeq ($(TARGET_OS),freebsd)
		RELEASE_CFLAGS+=-falign-loops=2 -falign-jumps=2 -falign-functions=2 -fno-strict-aliasing
	else
		RELEASE_CFLAGS+=-O2 -falign-loops=2 -falign-jumps=2 -falign-functions=2 -fno-strict-aliasing
	endif
endif

ifeq ($(TARGET_CPU),x86_64)
	ifeq ($(TARGET_OS),freebsd)
		RELEASE_CFLAGS+=-fomit-frame-pointer -fexpensive-optimizations -fno-strict-aliasing
	else
		RELEASE_CFLAGS+=-O2 -fomit-frame-pointer -fexpensive-optimizations -fno-strict-aliasing
	endif
endif


