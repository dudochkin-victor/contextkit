ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(update-contextkit-providers)

########### Add pkgconfig target ###############
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/contextsubscriber-1.0.pc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/contextsubscriber-1.0.pc"
  IMMEDIATE @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/contextsubscriber-1.0.pc DESTINATION lib/pkgconfig/)
#ADD_CUSTOM_TARGET(pkgconfigfile
#  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake") 
