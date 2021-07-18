IF(BUILD_TESTS)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)

ADD_EXECUTABLE       (make_big make_big.c)
TARGET_LINK_LIBRARIES(make_big netcdf)

ADD_EXECUTABLE       (make_compound_array_atomic make_compound_array_atomic.c)
TARGET_LINK_LIBRARIES(make_compound_array_atomic netcdf)

ADD_EXECUTABLE       (make_compound_array_user_type make_compound_array_user_type.c)
TARGET_LINK_LIBRARIES(make_compound_array_user_type netcdf)

ADD_EXECUTABLE       (make_compound_nest_atomic make_compound_nest_atomic.c)
TARGET_LINK_LIBRARIES(make_compound_nest_atomic netcdf)

ADD_EXECUTABLE       (make_compound_vlen_nest make_compound_vlen_nest.c)
TARGET_LINK_LIBRARIES(make_compound_vlen_nest netcdf)

ADD_EXECUTABLE       (make_nans make_nans.c)
TARGET_LINK_LIBRARIES(make_nans netcdf)

ADD_EXECUTABLE       (make_nc4_encodings make_nc4_encodings.c)
TARGET_LINK_LIBRARIES(make_nc4_encodings netcdf)

ADD_EXECUTABLE       (padheader padheader.c)
TARGET_LINK_LIBRARIES(padheader netcdf)

ADD_EXECUTABLE(test_nccmp_darray 
               test_nccmp_darray.c 
               ${CMAKE_SOURCE_DIR}/src/log.c 
               ${CMAKE_SOURCE_DIR}/src/nccmp_darray.c
               ${CMAKE_SOURCE_DIR}/src/xmalloc.c)

ADD_EXECUTABLE(test_nccmp_darray_sort 
               test_nccmp_darray_sort.c
               ${CMAKE_SOURCE_DIR}/src/log.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_darray.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_darray_sort.c
               ${CMAKE_SOURCE_DIR}/src/xmalloc.c)

ADD_EXECUTABLE(test_nccmp_odometer 
               test_nccmp_odometer.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_odometer.c)

ADD_EXECUTABLE(test_nccmp_stats
               test_nccmp_stats.c
               ${CMAKE_SOURCE_DIR}/src/log.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_darray.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_stats.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_strlist.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_user_type.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_utils.c
               ${CMAKE_SOURCE_DIR}/src/xmalloc.c)
TARGET_LINK_LIBRARIES(test_nccmp_stats netcdf m)

ADD_EXECUTABLE(test_nccmp_strlist 
               test_nccmp_strlist.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_strlist.c
               ${CMAKE_SOURCE_DIR}/src/xmalloc.c)

ADD_EXECUTABLE(test_nccmp_user_type 
               test_nccmp_user_type.c
               ${CMAKE_SOURCE_DIR}/src/log.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_darray.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_strlist.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_user_type.c
               ${CMAKE_SOURCE_DIR}/src/nccmp_utils.c
               ${CMAKE_SOURCE_DIR}/src/xmalloc.c)
TARGET_LINK_LIBRARIES(test_nccmp_user_type netcdf)

ADD_TEST(NAME test_nccmp_strlist     COMMAND test_nccmp_strlist)
ADD_TEST(NAME test_nccmp_odometer    COMMAND test_nccmp_odometer)
ADD_TEST(NAME test_nccmp_darray      COMMAND test_nccmp_darray)
ADD_TEST(NAME test_nccmp_darray_sort COMMAND test_nccmp_darray_sort)
ADD_TEST(NAME test_nccmp_stats       COMMAND test_nccmp_stats)
ADD_TEST(NAME test_nccmp_user_type   COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_user_type.sh)
ADD_TEST(NAME test_nccmp_01          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_01.sh)
ADD_TEST(NAME test_nccmp_02          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_02.sh)
ADD_TEST(NAME test_nccmp_03          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_03.sh)
ADD_TEST(NAME test_nccmp_04          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_04.sh)
ADD_TEST(NAME test_nccmp_05          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_05.sh)
ADD_TEST(NAME test_nccmp_06          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_06.sh)
ADD_TEST(NAME test_nccmp_07          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_07.sh)
ADD_TEST(NAME test_nccmp_08          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_08.sh)
ADD_TEST(NAME test_nccmp_09          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_09.sh)
ADD_TEST(NAME test_nccmp_10          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_10.sh)
ADD_TEST(NAME test_nccmp_11          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_11.sh)
ADD_TEST(NAME test_nccmp_12          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_12.sh)
ADD_TEST(NAME test_nccmp_13          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_13.sh)
ADD_TEST(NAME test_nccmp_14          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_14.sh)
ADD_TEST(NAME test_nccmp_15          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_15.sh)
ADD_TEST(NAME test_nccmp_16          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_16.sh)
ADD_TEST(NAME test_nccmp_17          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_17.sh)
ADD_TEST(NAME test_nccmp_18          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_18.sh)
ADD_TEST(NAME test_nccmp_19          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_19.sh)
ADD_TEST(NAME test_nccmp_20          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_20.sh)
ADD_TEST(NAME test_nccmp_21          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_21.sh)
ADD_TEST(NAME test_nccmp_22          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_22.sh)
ADD_TEST(NAME test_nccmp_23          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_23.sh)
ADD_TEST(NAME test_nccmp_24          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_24.sh)
ADD_TEST(NAME test_nccmp_25          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_25.sh)
ADD_TEST(NAME test_nccmp_26          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_26.sh)
ADD_TEST(NAME test_nccmp_27          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_27.sh)
ADD_TEST(NAME test_nccmp_28          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_28.sh)
ADD_TEST(NAME test_nccmp_29          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_29.sh)
ADD_TEST(NAME test_nccmp_30          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_30.sh)
ADD_TEST(NAME test_nccmp_31          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_31.sh)
ADD_TEST(NAME test_nccmp_32          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_32.sh)
ADD_TEST(NAME test_nccmp_33          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_33.sh)
ADD_TEST(NAME test_nccmp_34          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_34.sh)
ADD_TEST(NAME test_nccmp_35          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_35.sh)
ADD_TEST(NAME test_nccmp_36          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_36.sh)
ADD_TEST(NAME test_nccmp_37          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_37.sh)
ADD_TEST(NAME test_nccmp_38          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_38.sh)
ADD_TEST(NAME test_nccmp_39          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_39.sh)
ADD_TEST(NAME test_nccmp_40          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_40.sh)
ADD_TEST(NAME test_nccmp_41          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_41.sh)
ADD_TEST(NAME test_nccmp_42          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_42.sh)
ADD_TEST(NAME test_nccmp_43          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_43.sh)
ADD_TEST(NAME test_nccmp_44          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_44.sh)
ADD_TEST(NAME test_nccmp_45          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_45.sh)
ADD_TEST(NAME test_nccmp_46          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_46.sh)
ADD_TEST(NAME test_nccmp_47          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_47.sh)
ADD_TEST(NAME test_nccmp_48          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_48.sh)
ADD_TEST(NAME test_nccmp_49          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_49.sh)
ADD_TEST(NAME test_nccmp_50          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_50.sh)
ADD_TEST(NAME test_nccmp_51          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_51.sh)
ADD_TEST(NAME test_nccmp_52          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_52.sh)
ADD_TEST(NAME test_nccmp_53          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_53.sh)
ADD_TEST(NAME test_nccmp_54          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_54.sh)
ADD_TEST(NAME test_nccmp_55          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_55.sh)
ADD_TEST(NAME test_nccmp_56          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_56.sh)
ADD_TEST(NAME test_nccmp_57          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_57.sh)
ADD_TEST(NAME test_nccmp_58          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_58.sh)
ADD_TEST(NAME test_nccmp_59          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_59.sh)
ADD_TEST(NAME test_nccmp_60          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_60.sh)
ADD_TEST(NAME test_nccmp_61          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_61.sh)
ADD_TEST(NAME test_nccmp_62          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_62.sh)
ADD_TEST(NAME test_nccmp_63          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_63.sh)
ADD_TEST(NAME test_nccmp_64          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_64.sh)
ADD_TEST(NAME test_nccmp_65          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_65.sh)
ADD_TEST(NAME test_nccmp_66          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_66.sh)
ADD_TEST(NAME test_nccmp_67          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_67.sh)
ADD_TEST(NAME test_nccmp_68          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_68.sh)
ADD_TEST(NAME test_nccmp_69          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_69.sh)
ADD_TEST(NAME test_nccmp_70          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_70.sh)
ADD_TEST(NAME test_nccmp_71          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_71.sh)
ADD_TEST(NAME test_nccmp_72          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_72.sh)
ADD_TEST(NAME test_nccmp_73          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_73.sh)
ADD_TEST(NAME test_nccmp_74          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_74.sh)
ADD_TEST(NAME test_nccmp_75          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_75.sh)
ADD_TEST(NAME test_nccmp_76          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_76.sh)
ADD_TEST(NAME test_nccmp_77          COMMAND ${CMAKE_SOURCE_DIR}/test/test_nccmp_77.sh)
ADD_TEST(NAME test_big               COMMAND ${CMAKE_SOURCE_DIR}/test/test_big.sh)

SET(CLEAN_FILES group*.nc subgroup*.nc big make_big.exe make_compound_array_atomic make_compound_nest_atomic make_compound_array_user_type make_compound*.nc make_compound*.exe make_compound_vlen_nest make_compound_vlen_nest*.exe make_group make_group.exe make_nans make_nans.exe missing_value*.nc make_nc4_encodings make_nc4_encodings.exe not_encoded*.nc encoded*.nc padded*.nc padheader padheader.exe stderr*.tmp test*.nc test_nccmp_darray test_nccmp_darray.exe test_nccmp_darray_sort test_nccmp_darray_sort.exe test_nccmp_odometer test_nccmp_odometer.exe test_nccmp_stats test_nccmp_stats.exe
test_nccmp_strlist test_nccmp_strlist.exe test_nccmp_user_type test_nccmp_user_type.exe tmp* unpadded*.c unpadded*.nc _FillValue*.nc)

ADD_CUSTOM_TARGET(clean-test COMMAND rm -f ${CLEAN_FILES})
  
ENDIF(BUILD_TESTS)

