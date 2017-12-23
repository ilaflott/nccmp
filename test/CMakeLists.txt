#set(make_compound_array_atomic_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

if(BUILD_TESTS)

add_executable(make_compound_array_atomic ${make_compound_array_atomic_SRCS})

target_link_libraries(make_compound_array_atomic netcdf)

endif(BUILD_TESTS)

########### next target ###############

set(make_compound_array_user_type_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

if(BUILD_TESTS)

add_executable(make_compound_array_user_type ${make_compound_array_user_type_SRCS})

target_link_libraries(make_compound_array_user_type  netcdf)

endif(BUILD_TESTS)

########### next target ###############

set(make_compound_nest_atomic_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

if(BUILD_TESTS)

add_executable(make_compound_nest_atomic ${make_compound_nest_atomic_SRCS})

target_link_libraries(make_compound_nest_atomic  netcdf)

endif(BUILD_TESTS)

########### next target ###############

set(make_compound_vlen_nest_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

if(BUILD_TESTS)

add_executable(make_compound_vlen_nest ${make_compound_vlen_nest_SRCS})

target_link_libraries(make_compound_vlen_nest )

endif(BUILD_TESTS)

########### next target ###############

set(make_big_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

if(BUILD_TESTS)

add_executable(make_big ${make_big_SRCS})

target_link_libraries(make_big  netcdf)

endif(BUILD_TESTS)

########### next target ###############

set(make_nans_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

if(BUILD_TESTS)

add_executable(make_nans ${make_nans_SRCS})

target_link_libraries(make_nans  netcdf)

endif(BUILD_TESTS)

########### next target ###############

set(make_nc4_encodings_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

if(BUILD_TESTS)

add_executable(make_nc4_encodings ${make_nc4_encodings_SRCS})

target_link_libraries(make_nc4_encodings  netcdf)

endif(BUILD_TESTS)

########### next target ###############

set(padheader_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

if(BUILD_TESTS)

add_executable(padheader ${padheader_SRCS})

target_link_libraries(padheader  netcdf)

endif(BUILD_TESTS)

########### next target ###############

set(test_nccmp_odometer_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

if(BUILD_TESTS)

add_executable(test_nccmp_odometer ${test_nccmp_odometer_SRCS})

target_link_libraries(test_nccmp_odometer )

endif(BUILD_TESTS)

########### next target ###############

set(test_nccmp_strlist_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

if(BUILD_TESTS)

add_executable(test_nccmp_strlist ${test_nccmp_strlist_SRCS})

target_link_libraries(test_nccmp_strlist )

endif(BUILD_TESTS)

########### next target ###############

set(test_nccmp_user_type_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

if(BUILD_TESTS)

add_executable(test_nccmp_user_type ${test_nccmp_user_type_SRCS})

target_link_libraries(test_nccmp_user_type )

endif(BUILD_TESTS)

