testdir = $(libdir)/libcontextprovider-tests
test_PROGRAMS = valuechangestests

valuechangestests_SOURCES = valuechangestests.cpp valuechangestests.h

check-customer: $(test_PROGRAMS)
	PATH=../client:$$PATH CONTEXT_LOG_VERBOSITY=WARNING ./.libs/valuechangestests

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)  $(QtTest_CFLAGS) -I$(srcdir)/../../src -I$(top_srcdir)/common
LIBS += $(QtCore_LIBS) $(QtDBus_LIBS) $(QtTest_LIBS)
valuechangestests_LDADD = ../../src/libcontextprovider.la

../../src/libcontextprovider.la: FORCE
	$(MAKE) -C ../../src libcontextprovider.la
.PHONY: FORCE

# moccing
nodist_valuechangestests_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(valuechangestests_SOURCES))

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