lib_LTLIBRARIES = libcontextprovider.la

libcontextprovider_la_SOURCES = loggingfeatures.h       \
                                service.h               \
                                service.cpp             \
                                property.h              \
                                property.cpp            \
                                propertyprivate.h       \
                                propertyprivate.cpp     \
                                group.h			\
                                group.cpp		\
                                contextc.h              \
                                contextc.cpp            \
                                listeners.h             \
                                listeners.cpp		\
                                context_provider.h	\
                                servicebackend.h	\
                                servicebackend.cpp      \
                                propertyadaptor.h       \
                                propertyadaptor.cpp


includecontextproviderdir=$(includedir)/contextprovider
includecontextprovider_HEADERS = ContextProvider contextc.h context_provider.h
includecontextprovidercontextdir=$(includecontextproviderdir)/context
includecontextprovidercontext_HEADERS = service.h property.h group.h

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS) $(DBUS_CFLAGS) \
	      -I$(top_srcdir)/common			       \
              '-DCONTEXT_LOG_MODULE_NAME="libcontextprovider"' \
              '-DCONTEXT_LOG_HIDE_DEBUG'

LIBS += $(QtCore_LIBS) $(QtDBus_LIBS)
libcontextprovider_la_LIBADD=$(top_builddir)/common/libcommon.la

$(top_builddir)/common/libcommon.la: FORCE
	$(MAKE) -C $(top_builddir)/common libcommon.la

.PHONY: FORCE

DISTCLEANFILES = context

# moccing
nodist_libcontextprovider_la_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(libcontextprovider_la_SOURCES))

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
