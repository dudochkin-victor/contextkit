lib_LTLIBRARIES = libcontextsubscriber.la
libcontextsubscriber_la_SOURCES = contextproperty.cpp			\
	contextproperty.h propertyhandle.cpp propertyhandle.h		\
	provider.cpp provider.h safedbuspendingcallwatcher.h		\
	subscriberinterface.h subscriberinterface.cpp			\
	contextpropertyinfo.cpp contextpropertyinfo.h			\
	contextregistryinfo.cpp contextregistryinfo.h infobackend.h	\
	infobackend.cpp infoxmlbackend.h infoxmlbackend.cpp		\
	infokeydata.h cdbwriter.h cdbwriter.cpp cdbreader.cpp		\
	cdbreader.h infocdbbackend.cpp infocdbbackend.h			\
	dbusnamelistener.h dbusnamelistener.cpp handlesignalrouter.cpp	\
	handlesignalrouter.h queuedinvoker.cpp queuedinvoker.h		\
	loggingfeatures.h contextkitplugin.h contextkitplugin.cpp	\
	iproviderplugin.h contextproviderinfo.h nanoxml.h nanoxml.cpp	\
	asyncdbusinterface.cpp timedvalue.h contexttypeinfo.cpp		\
	contexttypeinfo.h contexttyperegistryinfo.cpp			\
	contexttyperegistryinfo.h assoctree.h assoctree.cpp duration.h  \
	duration.cpp

includecontextsubscriberdir=$(includedir)/contextsubscriber
includecontextsubscriber_HEADERS = contextproperty.h			\
	contextpropertyinfo.h contextregistryinfo.h iproviderplugin.h	\
	contextproviderinfo.h asyncdbusinterface.h timedvalue.h		\
	contexttypeinfo.h 						\
	contexttyperegistryinfo.h assoctree.h duration.h contextjson.h

AM_CXXFLAGS = -I$(top_srcdir)/common \
	      $(QtCore_CFLAGS) $(QtXml_CFLAGS) $(QtDBus_CFLAGS)	\
	      '-DDEFAULT_CONTEXT_SUBSCRIBER_PLUGINS="@libdir@/contextkit/subscriber-plugins"' \
	      '-DDEFAULT_CONTEXT_PROVIDERS="@datadir@/contextkit/providers/"' \
	      '-DDEFAULT_CONTEXT_CORE_DECLARATIONS="@datadir@/contextkit/core.context"' \
	      '-DDEFAULT_CONTEXT_TYPES="@datadir@/contextkit/types/"' \
	      '-DDEFAULT_CONTEXT_CORE_TYPES="@datadir@/contextkit/types/core.types"' \
	      '-DCONTEXT_LOG_MODULE_NAME="libcontextsubscriber"' \
	      '-DCONTEXT_LOG_HIDE_DEBUG'

$(top_builddir)/common/libcommon.la: FORCE
	$(MAKE) -C $(top_builddir)/common libcommon.la

LIBS += $(CDB_LIBS) $(QtCore_LIBS) $(QtXml_LIBS) $(QtDBus_LIBS)
libcontextsubscriber_la_LIBADD=$(top_builddir)/common/libcommon.la

.PHONY: FORCE

# moccing
nodist_libcontextsubscriber_la_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(libcontextsubscriber_la_SOURCES))

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

# because if you change configure parameter DEFAULT_CONTEXT_PROVIDERS,
# you should do a recompile
infocdbbackend.lo infoxmlbackend.lo: Makefile

# and the same for DEFAULT_CONTEXT_SUBSCRIBER_PLUGINS
provider.lo: Makefile
