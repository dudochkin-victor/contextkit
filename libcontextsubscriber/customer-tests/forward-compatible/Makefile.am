EXTRA_DIST = cache1.cdb cache2.cdb test.sh

bin_PROGRAMS = check-version

AM_CXXFLAGS = -I$(srcdir)/../../src $(QtXml_CFLAGS) $(QtCore_CFLAGS) $(QtDBus_CFLAGS) \
	      '-DCONTEXT_LOG_MODULE_NAME="check-version"' \
              '-DDEFAULT_CONTEXT_PROVIDERS="./"'

AM_LDFLAGS = $(QtXml_LIBS) $(QtCore_LIBS)

check_version_SOURCES = check-version.cpp

check_version_LDADD = ../../src/libcontextsubscriber.la
../../src/libcontextsubscriber.la: FORCE
	$(MAKE) -C ../../src libcontextsubscriber.la
.PHONY: FORCE

check-customer:
	./test.sh
