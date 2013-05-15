check_PROGRAMS = serviceunittest

# test's sources
serviceunittest_SOURCES = serviceunittest.cpp servicebackend.h manager.h manageradaptor.h

COVERAGE_FILES = service.cpp 

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)
AM_LDFLAGS = $(QtCore_LIBS) $(QtDBus_LIBS)
FROM_SOURCE = loggingfeatures.h service.cpp service.h
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_serviceunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(serviceunittest_SOURCES) $(FROM_SOURCE))

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
# when MOCing from .cpp file, you also has to help the dependency tracker
serviceunittest.o: serviceunittest.moc
