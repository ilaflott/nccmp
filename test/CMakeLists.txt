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
TARGET_LINK_LIBRARIES(test_nccmp_stats netcdf)

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
ADD_TEST(NAME test_nccmp_user_type   COMMAND test_nccmp_user_type.sh)
ADD_TEST(NAME test_nccmp_01          COMMAND test_nccmp_01.sh)
ADD_TEST(NAME test_nccmp_02          COMMAND test_nccmp_02.sh)
ADD_TEST(NAME test_nccmp_03          COMMAND test_nccmp_03.sh)
ADD_TEST(NAME test_nccmp_04          COMMAND test_nccmp_04.sh)
ADD_TEST(NAME test_nccmp_05          COMMAND test_nccmp_05.sh)
ADD_TEST(NAME test_nccmp_06          COMMAND test_nccmp_06.sh)
ADD_TEST(NAME test_nccmp_07          COMMAND test_nccmp_07.sh)
ADD_TEST(NAME test_nccmp_08          COMMAND test_nccmp_08.sh)
ADD_TEST(NAME test_nccmp_09          COMMAND test_nccmp_09.sh)
ADD_TEST(NAME test_nccmp_10          COMMAND test_nccmp_10.sh)
ADD_TEST(NAME test_nccmp_11          COMMAND test_nccmp_11.sh)
ADD_TEST(NAME test_nccmp_12          COMMAND test_nccmp_12.sh)
ADD_TEST(NAME test_nccmp_13          COMMAND test_nccmp_13.sh)
ADD_TEST(NAME test_nccmp_14          COMMAND test_nccmp_14.sh)
ADD_TEST(NAME test_nccmp_15          COMMAND test_nccmp_15.sh)
ADD_TEST(NAME test_nccmp_16          COMMAND test_nccmp_16.sh)
ADD_TEST(NAME test_nccmp_17          COMMAND test_nccmp_17.sh)
ADD_TEST(NAME test_nccmp_18          COMMAND test_nccmp_18.sh)
ADD_TEST(NAME test_nccmp_19          COMMAND test_nccmp_19.sh)
ADD_TEST(NAME test_nccmp_20          COMMAND test_nccmp_20.sh)
ADD_TEST(NAME test_nccmp_21          COMMAND test_nccmp_21.sh)
ADD_TEST(NAME test_nccmp_22          COMMAND test_nccmp_22.sh)
ADD_TEST(NAME test_nccmp_23          COMMAND test_nccmp_23.sh)
ADD_TEST(NAME test_nccmp_24          COMMAND test_nccmp_24.sh)
ADD_TEST(NAME test_nccmp_25          COMMAND test_nccmp_25.sh)
ADD_TEST(NAME test_nccmp_26          COMMAND test_nccmp_26.sh)
ADD_TEST(NAME test_nccmp_27          COMMAND test_nccmp_27.sh)
ADD_TEST(NAME test_nccmp_28          COMMAND test_nccmp_28.sh)
ADD_TEST(NAME test_nccmp_29          COMMAND test_nccmp_29.sh)
ADD_TEST(NAME test_nccmp_30          COMMAND test_nccmp_30.sh)
ADD_TEST(NAME test_nccmp_31          COMMAND test_nccmp_31.sh)
ADD_TEST(NAME test_nccmp_32          COMMAND test_nccmp_32.sh)
ADD_TEST(NAME test_nccmp_33          COMMAND test_nccmp_33.sh)
ADD_TEST(NAME test_nccmp_34          COMMAND test_nccmp_34.sh)
ADD_TEST(NAME test_nccmp_35          COMMAND test_nccmp_35.sh)
ADD_TEST(NAME test_nccmp_36          COMMAND test_nccmp_36.sh)
ADD_TEST(NAME test_nccmp_37          COMMAND test_nccmp_37.sh)
ADD_TEST(NAME test_nccmp_38          COMMAND test_nccmp_38.sh)
ADD_TEST(NAME test_nccmp_39          COMMAND test_nccmp_39.sh)
ADD_TEST(NAME test_nccmp_40          COMMAND test_nccmp_40.sh)
ADD_TEST(NAME test_nccmp_41          COMMAND test_nccmp_41.sh)
ADD_TEST(NAME test_nccmp_42          COMMAND test_nccmp_42.sh)
ADD_TEST(NAME test_nccmp_43          COMMAND test_nccmp_43.sh)
ADD_TEST(NAME test_nccmp_44          COMMAND test_nccmp_44.sh)
ADD_TEST(NAME test_nccmp_45          COMMAND test_nccmp_45.sh)
ADD_TEST(NAME test_nccmp_46          COMMAND test_nccmp_46.sh)
ADD_TEST(NAME test_nccmp_47          COMMAND test_nccmp_47.sh)
ADD_TEST(NAME test_nccmp_48          COMMAND test_nccmp_48.sh)
ADD_TEST(NAME test_nccmp_49          COMMAND test_nccmp_49.sh)
ADD_TEST(NAME test_nccmp_50          COMMAND test_nccmp_50.sh)
ADD_TEST(NAME test_nccmp_51          COMMAND test_nccmp_51.sh)
ADD_TEST(NAME test_nccmp_52          COMMAND test_nccmp_52.sh)
ADD_TEST(NAME test_nccmp_53          COMMAND test_nccmp_53.sh)
ADD_TEST(NAME test_nccmp_54          COMMAND test_nccmp_54.sh)
ADD_TEST(NAME test_nccmp_55          COMMAND test_nccmp_55.sh)
ADD_TEST(NAME test_nccmp_56          COMMAND test_nccmp_56.sh)
ADD_TEST(NAME test_nccmp_57          COMMAND test_nccmp_57.sh)
ADD_TEST(NAME test_nccmp_58          COMMAND test_nccmp_58.sh)
ADD_TEST(NAME test_nccmp_59          COMMAND test_nccmp_59.sh)
ADD_TEST(NAME test_nccmp_60          COMMAND test_nccmp_60.sh)
ADD_TEST(NAME test_nccmp_61          COMMAND test_nccmp_61.sh)
ADD_TEST(NAME test_nccmp_62          COMMAND test_nccmp_62.sh)
ADD_TEST(NAME test_nccmp_63          COMMAND test_nccmp_63.sh)
ADD_TEST(NAME test_nccmp_64          COMMAND test_nccmp_64.sh)
ADD_TEST(NAME test_nccmp_65          COMMAND test_nccmp_65.sh)
ADD_TEST(NAME test_nccmp_66          COMMAND test_nccmp_66.sh)
ADD_TEST(NAME test_nccmp_67          COMMAND test_nccmp_67.sh)
ADD_TEST(NAME test_nccmp_68          COMMAND test_nccmp_68.sh)
ADD_TEST(NAME test_nccmp_69          COMMAND test_nccmp_69.sh)
ADD_TEST(NAME test_big               COMMAND test_big.sh)

ADD_CUSTOM_TARGET(clean-test
  COMMAND rm -f group*.nc big make_big.exe make_compound_array_atomic make_compound_nest_atomic make_compound_array_user_type make_compound*.nc make_compound*.exe make_compound_vlen_nest make_compound_vlen_nest*.exe make_group make_group.exe make_nans make_nans.exe missing_value*.nc make_nc4_encodings make_nc4_encodings.exe not_encoded*.nc encoded*.nc padded*.nc padheader padheader.exe stderr*.tmp test*.nc test_nccmp_darray test_nccmp_darray.exe test_nccmp_darray_sort test_nccmp_darray_sort.exe test_nccmp_odometer test_nccmp_odometer.exe test_nccmp_stats test_nccmp_stats.exe
test_nccmp_strlist test_nccmp_strlist.exe test_nccmp_user_type test_nccmp_user_type.exe tmp* unpadded*.c unpadded*.nc _FillValue*.nc
)
  
ENDIF(BUILD_TESTS)

