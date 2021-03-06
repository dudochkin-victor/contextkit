FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(Qt5Core)
FIND_PACKAGE(Qt5DBus)

#INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})
ADD_DEFINITIONS(${Qt5DBus_DEFINITIONS})

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/libcontextsubscriber/src
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5DBus_INCLUDE_DIRS}
    )

SET (PROGRAMS update-contextkit-providers)

SET(PROGRAM_SRCS
    update-contextkit-providers.cpp
    )

SET(PROGRAM_HEADERS
    )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")

ADD_DEFINITIONS(-DDEFAULT_CONTEXT_PROVIDERS="${PKG_DATADIR}contextkit/providers/")
ADD_DEFINITIONS(-DCONTEXT_LOG_MODULE_NAME="update-contextkit-providers")

ADD_EXECUTABLE(${PROGRAMS} ${PROGRAM_SRCS} ${PROGRAM_HEADERS})

   
TARGET_LINK_LIBRARIES(${PROGRAMS}
    ${ZLIB_LIBRARIES}
    ${Qt5_LIBRARIES}
    ${DBUS_LIBRARIES}
    contextsubscriber
    )

INSTALL(TARGETS ${PROGRAMS} RUNTIME DESTINATION ${PKG_BINDIR})

