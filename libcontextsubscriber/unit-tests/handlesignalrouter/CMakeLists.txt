FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(Qt4 REQUIRED)

include (${QT_USE_FILE})

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${QT_INCLUDES}
    )

SET (PROGRAMS 1cextractor)

SET(PROGRAM_SRCS
    main.cpp
    pole.cpp
    )

SET(PROGRAM_HEADERS
	include/pole.h
    )

ADD_EXECUTABLE(${PROGRAMS} ${PROGRAM_SRCS} ${PROGRAM_HEADERS})

   
TARGET_LINK_LIBRARIES(${PROGRAMS}
    ${ZLIB_LIBRARIES}
    ${QT_LIBRARIES}
    )

INSTALL(TARGETS ${PROGRAMS} RUNTIME DESTINATION ${PKG_BINDIR})

