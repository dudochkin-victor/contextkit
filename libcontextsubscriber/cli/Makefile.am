bin_PROGRAMS = context-listen
context_listen_SOURCES = context-listen.cpp			        \
	propertylistener.h propertylistener.cpp commandwatcher.h	\
	commandwatcher.cpp

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS) $(QJson_CFLAGS) \
	      '-DCONTEXT_LOG_MODULE_NAME="context-listen"'
LIBS += $(QtCore_LIBS) $(QJson_LIBS)

# library dependency hack for seamless make in cli/
AM_CXXFLAGS += -I$(srcdir)/../src \
	       -I$(top_srcdir)/common

context_listen_LDADD = ../src/libcontextsubscriber.la $(top_builddir)/common/libcommon.la

../src/libcontextsubscriber.la: FORCE
	$(MAKE) -C ../src libcontextsubscriber.la

.PHONY: FORCE

# moccing
nodist_context_listen_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(context_listen_SOURCES))

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
