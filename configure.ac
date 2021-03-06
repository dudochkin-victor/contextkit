AC_PREREQ([2.61])
AC_INIT([ContextKit], [0.5.27], [marius.vollmer@nokia.com], ContextKit)

AC_CONFIG_SRCDIR([Makefile.am])
AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-bzip2 tar-ustar 1.9])

AC_CONFIG_MACRO_DIR([m4])

#m4_pattern_allow([AM_PROG_AR])
AM_PROG_AR
AC_PROG_CC
CFLAGS="$CXXFLAGS -Wall"
AC_PROG_CXX
CXXFLAGS="$CXXFLAGS -Wall"
LT_INIT([disable-static])
DOLT
AM_PATH_PYTHON

# check for libraries
PKG_CHECK_MODULES_SUBST([QtTest], [QtTest])
PKG_CHECK_MODULES_SUBST([QtCore], [QtCore])
PKG_CHECK_MODULES_SUBST([QtDBus], [QtDBus >= 4.6])
PKG_CHECK_MODULES_SUBST([QtXml], [QtXml])
PKG_CHECK_MODULES_SUBST([QJson], [QJson])
# only for compiling c code
PKG_CHECK_MODULES_SUBST([DBUS], [dbus-1])
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.12.0])

AC_PROG_QT_MOC
AC_PROG_QT_UIC
AC_PROG_QT_RCC
AC_CHECK_LIB([cdb], [cdb_make_start], [CDB_LIBS=-lcdb], [AC_MSG_ERROR([Can't find cdb library])])
AC_SUBST([CDB_LIBS])

# tools for documentation
AX_FEATURE_OPTIONAL([doc], [BUILD_DOCS], [disable building of documentation])
AX_DISABLE_FEATURE_ON_PROG([Asciidoc], [BUILD_DOCS], [ASCIIDOC], [asciidoc])
AX_DISABLE_FEATURE_ON_PROG([xsltproc], [BUILD_DOCS], [XSLTPROC], [xsltproc])
AX_DISABLE_FEATURE_ON_PROG([xmllint], [BUILD_DOCS], [XMLLINT], [xmllint])
AX_DISABLE_FEATURE_ON_PROG([doxygen], [BUILD_DOCS], [DOXYGEN], [doxygen])
AX_PROG_PERL_MODULES([XML::DOM], ,
  [missing_deps_BUILD_DOCS="$missing_deps_BUILD_DOCS, libxml-dom-perl"])
AM_CONDITIONAL([CONTEXTKIT_BUILD_DOCS], [test "$missing_deps_BUILD_DOCS" = ""])

# coverage tools
AX_FEATURE_OPTIONAL([coverage], [COVERAGE], [disable unittests' coverage support])
AX_DISABLE_FEATURE_ON_PROG([GCov coverage recording tool], [COVERAGE], [GCOV], [gcov])
AX_DISABLE_FEATURE_ON_PROG([LCov coverage visualization tool], [COVERAGE], [LCOV], [lcov])
AM_CONDITIONAL([CONTEXTKIT_COVERAGE], [test "$missing_deps_COVERAGE" = ""])

AC_CONFIG_FILES([
        Makefile
        doc/Makefile
        common/Makefile
        libcontextsubscriber/Makefile
        libcontextsubscriber/cli/Makefile
        libcontextsubscriber/cls/Makefile
        libcontextsubscriber/contextsubscriber-1.0.pc
        libcontextsubscriber/contextsubscriber-1.0-uninstalled.pc
        libcontextsubscriber/doc/Makefile
        libcontextsubscriber/man/Makefile
        libcontextsubscriber/src/Makefile
        libcontextsubscriber/update-contextkit-providers/Makefile
        libcontextsubscriber/propertymonitor/Makefile
        libcontextprovider/contextprovider-1.0.pc
        libcontextprovider/contextprovider-1.0-uninstalled.pc
        libcontextprovider/Makefile
        libcontextprovider/doc/Makefile
        libcontextprovider/src/Makefile
        libcontextprovider/context-provide/Makefile
        spec/Makefile
        tools/Makefile
])

#        python/Makefile
# The user only needs to have the documentation tools (asciidoc, etc.)
# if she checked out the master Git branch, instead of using a release
# tag or a release tarball, as she should.
if test "$missing_deps_BUILD_DOCS" != "" -a \! -f ${srcdir}/spec/context-properties.html
then
	missing_deps=`echo $missing_deps_BUILD_DOCS | cut -c3-`
	AC_MSG_FAILURE([
Building from Git, but not all of the required doctools are available: $missing_deps
Please use a release tag instead or install the required tools!])
fi

AC_OUTPUT

AX_FEATURE_SUMMARIZE([BUILD_DOCS],
             [Building of documentation is enabled],
             [Documentation won't be built])

AX_FEATURE_SUMMARIZE([COVERAGE],
             [Unit testing coverage support is enabled],
             [Coverage won't be supported during unit testing])

ln -nfs $ac_abs_confdir/libcontextprovider/src/ libcontextprovider/src/context
