
EXTRA_DIST = Makefile.mak

AM_CPPFLAGS = -I @top_srcdir@/include -I @top_srcdir@/include/world

lib_LIBRARIES = libHTSEngine.a

libHTSEngine_a_SOURCES = HTS_audio.c  HTS_engine.c  HTS_hidden.h  HTS_misc.c  \
                         HTS_pstream.c  HTS_sstream.c  HTS_model.c  HTS_vocoder.c \
                         HTS_gstream.c HTS_label.c cheaptrick.cpp common.cpp d4c.cpp dio.cpp fft.cpp \
	matlabfunctions.cpp  stonemask.cpp  synthworld.cpp synthesis.cpp SPTKfunctions.c synthesisrealtime.cpp

DISTCLEANFILES = *.log *.out *~

MAINTAINERCLEANFILES = Makefile.in
