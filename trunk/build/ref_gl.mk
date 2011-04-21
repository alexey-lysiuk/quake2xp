REF_GL_SRCS = \
	ref_gl/gl_anim.c \
	ref_gl/gl_draw.c \
	ref_gl/gl_font.c \
	ref_gl/gl_image.c \
	ref_gl/gl_light.c \
	ref_gl/gl_mesh.c \
	ref_gl/gl_model.c \
	ref_gl/gl_obj.c \
	ref_gl/gl_drawmd3.c \
	ref_gl/gl_rmain.c \
	ref_gl/gl_rmisc.c \
	ref_gl/gl_rsurf.c \
	ref_gl/gl_warp.c \
	ref_gl/gl_particle.c \
	ref_gl/gl_arb_shader.c \
	ref_gl/gl_shadows.c \
	ref_gl/qgl.c \
	\
	game/q_shared.c

ifeq ($(TARGET_OS),mingw32)
	REF_GL_SRCS += \
		win32/qgl_win.c \
		win32/glw_imp.c \
		win32/q_shwin.c
	REF_SDL_SRCS =
	REF_SDL_TARGET=ref_gl.$(SHARED_EXT)
else
	ifeq ($(TARGET_OS),darwin)
		REF_GL_SRCS += \
				macosx/qgl_osx.c \
				macosx/q_shosx.c
		REF_SDL_SRCS = macosx/glimp_osx.m
		REF_SDL_TARGET=ref_gl.$(SHARED_EXT)
	else
		REF_GL_SRCS += \
				linux/qgl_linux.c \
				linux/q_shlinux.c \
				unix/glob.c
		REF_SDL_SRCS = linux/gl_sdl.c
		REF_SDL_TARGET=ref_sdl.$(SHARED_EXT)
	endif
endif

REF_GLX_SRCS = linux/gl_glx.c

#---------------------------------------------------------------------------------------------------------------------

REF_GL_OBJS=$(REF_GL_SRCS:%.c=$(BUILDDIR)/ref_gl/%.o)
REF_GL_DEPS=$(REF_GL_OBJS:%.o=%.d)

REF_GLX_OBJS=$(REF_GLX_SRCS:%.c=$(BUILDDIR)/ref_gl/%.o)
REF_GLX_DEPS=$(REF_GLX_OBJS:%.o=%.d)
REF_GLX_TARGET=ref_glx.$(SHARED_EXT)

ifeq ($(BUILD_CLIENT), 1)
ifeq ($(HAVE_VID_GLX), 1)
	TARGETS += $(REF_GLX_TARGET)
	ALL_OBJS += $(REF_GL_OBJS) $(REF_GLX_OBJS)
	ALL_DEPS += $(REF_GL_DEPS) $(REF_GLX_DEPS)
endif
endif

# Say about to build the target
$(REF_GLX_TARGET) : $(REF_GLX_OBJS) $(REF_GL_OBJS) $(BUILDDIR)/.dirs
	@echo " * [GLX] ... linking"; \
		$(CC) $(LDFLAGS) $(SHARED_LDFLAGS) -o $@ $(REF_GLX_OBJS) $(REF_GL_OBJS) $(LIBS) $(LNKFLAGS) $(SDL_LIBS)

#---------------------------------------------------------------------------------------------------------------------

REF_SDL_OBJS=$(REF_SDL_SRCS:%.c=$(BUILDDIR)/ref_gl/%.o)
REF_SDL_DEPS=$(REF_SDL_OBJS:%.o=%.d)

ifeq ($(BUILD_CLIENT), 1)
ifeq ($(HAVE_SDL), 1)
	TARGETS += $(REF_SDL_TARGET)
	ALL_OBJS += $(REF_GL_OBJS) $(REF_SDL_OBJS)
	ALL_DEPS += $(REF_GL_DEPS) $(REF_SDL_DEPS)
endif
endif

# Say about to build the target
$(REF_SDL_TARGET) : $(REF_SDL_OBJS) $(REF_GL_OBJS) $(BUILDDIR)/.dirs
	@echo " * [SDL ] ... linking"; \
		$(CC) $(LDFLAGS) $(LNKFLAGS) $(SHARED_LDFLAGS) -o $@ $(REF_SDL_OBJS) $(REF_GL_OBJS) $(LIBS) $(LNKFLAGS) $(SDL_LIBS)

#---------------------------------------------------------------------------------------------------------------------

# Say how to build .o files from .c files for this module
$(BUILDDIR)/ref_gl/%.o: $(SRCDIR)/%.c $(BUILDDIR)/.dirs
	@echo " * [RGL ] $<"; \
		$(CC) $(CFLAGS) $(SHARED_CFLAGS) $(SDL_CFLAGS) -o $@ -c $<

# Say how to build the dependencies
$(BUILDDIR)/ref_gl/%.d: $(SRCDIR)/%.c $(BUILDDIR)/.dirs
	@echo " * [DEP ] $<"; $(DEP) $(SDL_CFLAGS)
