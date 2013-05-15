bin_PROGRAMS = update-contextkit-providers

update_contextkit_providers_SOURCES = update-contextkit-providers.cpp

AM_CXXFLAGS = -I$(srcdir)/../src $(QtXml_CFLAGS) $(QtCore_CFLAGS) $(QtDBus_CFLAGS) \
              '-DDEFAULT_CONTEXT_PROVIDERS="@datadir@/contextkit/providers/"' \
	      '-DCONTEXT_LOG_MODULE_NAME="update-contextkit-providers"'

AM_LDFLAGS = $(QtXml_LIBS) $(QtCore_LIBS)

# library dependency hack for seamless make in update-contextkit-providers/
update_contextkit_providers_LDADD = ../src/libcontextsubscriber.la
../src/libcontextsubscriber.la: FORCE
	$(MAKE) -C ../src libcontextsubscriber.la
.PHONY: FORCE

