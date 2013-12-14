AC_INIT([Possible Worlds], [0.0.1],
        [Igor Gnatenko <i.gnatenko.brain@gmail.com>],
        [psworlds], [https://github.com/ignatenkobrain/psworlds])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall])
AC_PROG_CXX
AC_PROG_INSTALL
PKG_CHECK_MODULES(SDL, sdl)
PKG_CHECK_MODULES(SDL_MIXER, SDL_mixer)
PKG_CHECK_MODULES(LIBCONFIG, libconfig++)
AX_CHECK_GL
AX_CHECK_GLU
AX_CHECK_GLUT
AC_CONFIG_FILES([Makefile src/Makefile src/version.h])
AC_OUTPUT
