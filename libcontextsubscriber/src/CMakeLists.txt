FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(Qt5Core)
FIND_PACKAGE(Qt5DBus)
FIND_PACKAGE(Qt5Xml)
#FIND_PACKAGE(Qt4 4.7 COMPONENTS QtCore QtXml QtDbus REQUIRED)
FIND_PACKAGE(TinyCDB REQUIRED)

SET(VERSION_MAJOR 0)
SET(VERSION_MINOR 0)
SET(VERSION_PATCH 0)

PKG_CHECK_MODULES(DBUS dbus-1 REQUIRED)

#INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})
ADD_DEFINITIONS(${Qt5DBus_DEFINITIONS})
ADD_DEFINITIONS(${Qt5Xml_DEFINITIONS})

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/common
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5DBus_INCLUDE_DIRS}
    ${Qt5Xml_INCLUDE_DIRS}
    ${TINYCDB_INCLUDE_DIR}
    )

SET (LIBRARY contextsubscriber)

SET(LIBRARY_SRCS
        contextproperty.cpp
        propertyhandle.cpp
        provider.cpp
        subscriberinterface.cpp
        contextpropertyinfo.cpp
        contextregistryinfo.cpp
        infobackend.cpp
        infoxmlbackend.cpp
        cdbwriter.cpp
        cdbreader.cpp
        infocdbbackend.cpp
        dbusnamelistener.cpp
        handlesignalrouter.cpp
        queuedinvoker.cpp
        contextkitplugin.cpp
        nanoxml.cpp
        asyncdbusinterface.cpp
        contexttypeinfo.cpp
        contexttyperegistryinfo.cpp
        assoctree.cpp
        duration.cpp
    )

SET(LIBRARY_HEADERS
        contextproperty.h
        propertyhandle.h
        safedbuspendingcallwatcher.h
        subscriberinterface.h
        contextpropertyinfo.h
        contextregistryinfo.h
        infobackend.h
        infoxmlbackend.h
        cdbwriter.h
#        infokeydata.h
        cdbreader.h
        infocdbbackend.h
        dbusnamelistener.h
        handlesignalrouter.h
        queuedinvoker.h
#       loggingfeatures.h
        contextkitplugin.h
        iproviderplugin.h
#        contextproviderinfo.h
#        nanoxml.h
#        timedvalue.h
#        contexttypeinfo.h
        contexttyperegistryinfo.h
#        assoctree.h
#        duration.h
        provider.h
    )

QT5_WRAP_CPP(LIBRARY_MOC_HEADERS ${LIBRARY_HEADERS})

ADD_DEFINITIONS(-DDEFAULT_CONTEXT_SUBSCRIBER_PLUGINS="${PKG_LIBDIR}contextkit/subscriber-plugins")
ADD_DEFINITIONS(-DDEFAULT_CONTEXT_PROVIDERS="${PKG_DATADIR}contextkit/providers/")
ADD_DEFINITIONS(-DDEFAULT_CONTEXT_CORE_DECLARATIONS="${PKG_DATADIR}contextkit/core.context")
ADD_DEFINITIONS(-DDEFAULT_CONTEXT_TYPES="${PKG_DATADIR}contextkit/types/")
ADD_DEFINITIONS(-DDEFAULT_CONTEXT_CORE_TYPES="${PKG_DATADIR}contextkit/types/core.types")
ADD_DEFINITIONS(-DCONTEXT_LOG_MODULE_NAME="libcontextsubscriber")

ADD_LIBRARY(${LIBRARY} SHARED ${LIBRARY_SRCS} ${LIBRARY_HEADERS} ${LIBRARY_MOC_HEADERS})

SET_TARGET_PROPERTIES( ${LIBRARY} PROPERTIES
     SOVERSION 0
     VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
   )


TARGET_LINK_LIBRARIES(${LIBRARY}
    ${ZLIB_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Xml_LIBRARIES}
    ${TINYCDB_LIBRARY}
    common
    )

INSTALL(TARGETS ${LIBRARY} DESTINATION ${PKG_LIBDIR})
INSTALL(FILES assoctree.h asyncdbusinterface.h contextjson.h 
	contextproperty.h contextpropertyinfo.h contextproviderinfo.h 
	contextregistryinfo.h contexttypeinfo.h contexttyperegistryinfo.h 
	duration.h iproviderplugin.h timedvalue.h 
	DESTINATION ${PKG_INCLUDEDIR}/contextsubscriber)

