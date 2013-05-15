plugindir = $(libdir)/contextkit/subscriber-test-plugins/
plugin_LTLIBRARIES = contextsubscribertime1.la
contextsubscribertime1_la_SOURCES = timeplugin.cpp timeplugin.h

BUILT_SOURCES = $(contextsubscribertime1_la_SOURCES)
timeplugin.cpp: ../timeplugin.cpp
	ln -sf $< $@
timeplugin.h: ../timeplugin.h
	ln -sf $< $@

clean-local:
	rm -f timeplugin.cpp timeplugin.h

AM_CXXFLAGS = -I$(top_srcdir)/common	\
	      -I$(srcdir)/../../../src $(QtCore_CFLAGS)		\
	      $(QtDBus_CFLAGS)					\
	      '-DCONTEXT_LOG_MODULE_NAME="time1plugin"'		\
	      '-DTIME_PLUGIN_PREFIX="Time1: "'

contextsubscribertime1_la_LDFLAGS = -avoid-version -module

$(top_builddir)/common/libcommon.a: FORCE
	$(MAKE) -C $(top_builddir)/common libcommon.a

LIBS += $(CDB_LIBS) $(QtCore_LIBS) $(QtDBus_LIBS)
contextsubscribertime1_la_LIBADD=$(top_builddir)/common/libcommon.la

.PHONY: FORCE

# moccing
nodist_contextsubscribertime1_la_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextsubscribertime1_la_SOURCES))

## Basic idea from here: http://blog.hartwork.org/?p=133
## If you want to do the qt moccing for your header files and link them
## to your executable:
# nodist_yourtarget_SOURCES = mocs.cpp
# QT_TOMOC = $(filter %.h, $(yourtarget_SOURCES))
#            ^^^^ or you can use explicit header file listing here

## If you have a .cpp file, which you want to moc, you have to use an
## explicit dependency like this:
# main.o: main.moc
## and of course you have to put #include "main.moc" at the end of
## main.cpp.
## Declaring the cpp file in QT_TOMOC is not needed.

QT_MOCS = $(QT_TOMOC:%.h=moc_%_h.cpp)

# mocs.cpp depends on Makefile so that it is also regenerated when an
# entry is removed from QT_MOCS.

mocs.cpp: $(QT_MOCS) Makefile
	cat $(QT_MOCS) >mocs.cpp

moc_%_h.cpp: %.h
	$(MOC) -o "$@" "$<"

%.moc: %.cpp
	$(MOC) -o "$@" "$<"

clean-moc-extra:
	rm -f moc_*.cpp mocs.cpp *.moc mocs.lo

clean-local: clean-moc-extra

.PHONY: clean-moc-extra
