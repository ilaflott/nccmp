SET(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(nccmp_SRCS
    log.c
    nccmp.c
    nccmp_data.c
    nccmp_dim.c
    nccmp_format.c
    nccmp_group.c
    nccmp_io.c
    nccmp_metadata.c
    nccmp_nc_type.c
    nccmp_ncinfo.c
    nccmp_nest.c
    nccmp_odometer.c
    nccmp_opt.c
    nccmp_state.c
    nccmp_strlist.c
    nccmp_utils.c
    nccmp_user_type.c
    nccmp_var.c
    xmalloc.c)

ADD_EXECUTABLE(nccmp ${nccmp_SRCS})

TARGET_LINK_LIBRARIES(nccmp ${NETCDF_LIB_PATH} ${CMAKE_EXE_LINKER_FLAGS} m)

IF(CMAKE_THREAD_LIBS_INIT)
  TARGET_LINK_LIBRARIES(nccmp "${CMAKE_THREAD_LIBS_INIT}")
ENDIF()

IF(WITH_NETCDF)
    TARGET_COMPILE_OPTIONS(nccmp PUBLIC "-I${WITH_NETCDF}/include -I${WITH_NETCDF}/libsrc4")
ENDIF()

INSTALL(TARGETS nccmp RUNTIME DESTINATION bin)
