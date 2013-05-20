ADD_SUBDIRECTORY(src)
#ADD_SUBDIRECTORY(unit-tests)
#ADD_SUBDIRECTORY(doc)
#ADD_SUBDIRECTORY(customer-tests)
#ADD_SUBDIRECTORY(context-provide)

########### Add pkgconfig target ###############
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/contextprovider-1.0.pc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/contextprovider-1.0.pc"
  IMMEDIATE @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/contextprovider-1.0.pc DESTINATION lib/pkgconfig/)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/libcontextprovider0.conf DESTINATION /etc/dbus-1/system.d)
#ADD_CUSTOM_TARGET(pkgconfigfile
#  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake") 
