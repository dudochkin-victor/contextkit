FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(Qt5Core)
FIND_PACKAGE(Qt5DBus)
#FIND_PACKAGE(Qt4 4.7 COMPONENTS QtCore QtDbus REQUIRED)

SET(VERSION_MAJOR 0)
SET(VERSION_MINOR 0)
SET(VERSION_PATCH 0)

PKG_CHECK_MODULES(DBUS dbus-1 REQUIRED)

#INCLUDE(${QT5_USE_FILE})
ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})
ADD_DEFINITIONS(${Qt5DBus_DEFINITIONS})

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/common
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5DBus_INCLUDE_DIRS}
    ${DBUS_INCLUDE_DIRS}
    )

SET (LIBRARY contextprovider)

SET(LIBRARY_SRCS
        service.cpp
        property.cpp
        propertyprivate.cpp
        group.cpp
        contextc.cpp
        listeners.cpp
        servicebackend.cpp
        propertyadaptor.cpp
    )

SET(LIBRARY_HEADERS
        propertyadaptor.h
        servicebackend.h
#        context_provider.h
        listeners.h
#        contextc.h
        group.h
        propertyprivate.h
        property.h
#        loggingfeatures.h
        service.h
    )

QT5_WRAP_CPP(LIBRARY_MOC_HEADERS ${LIBRARY_HEADERS})

ADD_LIBRARY(${LIBRARY} SHARED ${LIBRARY_SRCS} ${LIBRARY_HEADERS} ${LIBRARY_MOC_HEADERS})

SET_TARGET_PROPERTIES( ${LIBRARY} PROPERTIES
     SOVERSION 0
     VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
   )

TARGET_LINK_LIBRARIES(${LIBRARY}
    ${ZLIB_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    common
    )

INSTALL(TARGETS ${LIBRARY} DESTINATION ${PKG_LIBDIR})
INSTALL(FILES ContextProvider contextc.h context_provider.h DESTINATION ${PKG_INCLUDEDIR}/contextprovider)
INSTALL(FILES group.h property.h service.h DESTINATION ${PKG_INCLUDEDIR}/contextprovider/context)
