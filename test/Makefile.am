# Copyright (C) 2004,2009 Remik Ziemlinski <first d0t surname att n0aa d0t g0v>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES =                                                    \
    group*.nc subgroups*.nc                                     \
    make_big make_big.exe                                       \
    make_compound_array_atomic make_compound_nest_atomic        \
    make_compound_array_user_type                               \
    make_compound*.nc make_compound*.exe                        \
    make_compound_vlen_nest make_compound_vlen_nest*.exe        \
    make_group make_group.exe                                   \
    make_nans make_nans.exe missing_value*.nc                   \
    make_nc4_encodings make_nc4_encodings.exe                   \
    not_encoded*.nc encoded*.nc                                 \
    padded*.nc padheader padheader.exe                          \
    stderr*.tmp test*.nc                                        \
    test_nccmp_darray test_nccmp_darray.exe                     \
    test_nccmp_darray_sort test_nccmp_darray_sort.exe           \
    test_nccmp_odometer test_nccmp_odometer.exe                 \
    test_nccmp_stats test_nccmp_stats.exe                       \
    test_nccmp_strlist test_nccmp_strlist.exe                   \
    test_nccmp_user_type test_nccmp_user_type.exe               \
    tmp*                                                        \
    unpadded*.c unpadded*.nc                                    \
    _FillValue*.nc

# Run in this order.
TESTS =                                                                 \
    test_nccmp_strlist                                                  \
    test_nccmp_odometer                                                 \
    test_nccmp_darray                                                   \
    test_nccmp_darray_sort                                              \
    test_nccmp_stats                                                    \
    test_nccmp_user_type.sh						\
    test_nccmp_01.sh test_nccmp_02.sh test_nccmp_03.sh                  \
    test_nccmp_04.sh test_nccmp_05.sh test_nccmp_06.sh test_nccmp_07.sh \
    test_nccmp_08.sh test_nccmp_09.sh test_nccmp_10.sh test_nccmp_11.sh \
    test_nccmp_12.sh test_nccmp_13.sh test_nccmp_14.sh test_nccmp_15.sh \
    test_nccmp_16.sh test_nccmp_17.sh test_nccmp_18.sh test_nccmp_19.sh \
    test_nccmp_20.sh test_nccmp_21.sh test_nccmp_22.sh test_nccmp_23.sh \
    test_nccmp_24.sh test_nccmp_25.sh test_nccmp_26.sh test_nccmp_27.sh \
    test_nccmp_28.sh test_nccmp_29.sh                                   \
    test_nccmp_30.sh test_nccmp_31.sh test_nccmp_32.sh test_nccmp_33.sh \
    test_nccmp_34.sh test_nccmp_35.sh test_nccmp_36.sh test_nccmp_37.sh \
    test_nccmp_38.sh test_nccmp_39.sh                                   \
    test_nccmp_40.sh test_nccmp_41.sh test_nccmp_42.sh test_nccmp_43.sh \
    test_nccmp_44.sh test_nccmp_45.sh test_nccmp_46.sh test_nccmp_47.sh \
    test_nccmp_48.sh test_nccmp_49.sh test_nccmp_50.sh test_nccmp_51.sh \
    test_nccmp_52.sh test_nccmp_53.sh test_nccmp_54.sh test_nccmp_55.sh \
    test_nccmp_56.sh test_nccmp_57.sh test_nccmp_58.sh test_nccmp_59.sh \
    test_nccmp_60.sh test_nccmp_61.sh test_nccmp_62.sh test_nccmp_63.sh \
    test_nccmp_64.sh test_nccmp_65.sh test_nccmp_66.sh test_nccmp_67.sh \
    test_nccmp_68.sh test_nccmp_69.sh test_nccmp_70.sh test_nccmp_71.sh	\
    test_nccmp_72.sh test_nccmp_73.sh test_nccmp_74.sh test_nccmp_75.sh \
    test_nccmp_76.sh test_nccmp_77.sh test_nccmp_78.sh test_nccmp_79.sh \
    test_nccmp_80.sh test_nccmp_81.sh test_nccmp_82.sh test_nccmp_83.sh \
	test_nccmp_84.sh test_nccmp_85.sh

if ENABLE_TEST_BIG
    TESTS += test_big.sh
endif

EXTRA_DIST =                                                \
    group*.cdl                                              \
    subgroups*.cdl                                          \
    missing_value*.cdl                                      \
    ncgen_formats.sh                                        \
    stderr*.txt                                             \
    test*.cdl                                               \
    test_*.sh                                               \
    _FillValue*.cdl

check_PROGRAMS =                                            \
     make_compound_array_atomic                             \
     make_compound_array_user_type                          \
     make_compound_nest_atomic                              \
     make_compound_vlen_nest                                \
     make_big make_nans                                     \
     make_nc4_encodings padheader                           \
     test_nccmp_darray                                      \
     test_nccmp_darray_sort                                 \
     test_nccmp_odometer                                    \
     test_nccmp_stats                                       \
     test_nccmp_strlist                                     \
     test_nccmp_user_type

padheader_LDADD = -lnetcdf
make_compound_array_atomic_LDADD = -lnetcdf
make_compound_array_user_type_LDADD = -lnetcdf
make_compound_nest_atomic_LDADD = -lnetcdf
make_compound_vlen_nest = -lnetcdf
make_big_LDADD = -lnetcdf
make_nans_LDADD = -lnetcdf
make_nc4_encodings_LDADD = -lnetcdf

test_nccmp_odometer_CPPFLAGS = -I$(top_srcdir)/src
test_nccmp_odometer_LDADD = $(top_builddir)/src/nccmp-nccmp_odometer.o

test_nccmp_darray_CPPFLAGS = -I$(top_srcdir)/src
test_nccmp_darray_LDADD = $(top_builddir)/src/nccmp-log.o                \
                         $(top_builddir)/src/nccmp-nccmp_darray.o        \
                         $(top_builddir)/src/nccmp-xmalloc.o

test_nccmp_darray_sort_CPPFLAGS = -I$(top_srcdir)/src
test_nccmp_darray_sort_LDADD = $(top_builddir)/src/nccmp-log.o           \
                         $(top_builddir)/src/nccmp-nccmp_darray.o        \
                         $(top_builddir)/src/nccmp-nccmp_darray_sort.o   \
                         $(top_builddir)/src/nccmp-xmalloc.o

test_nccmp_stats_CPPFLAGS = -I$(top_srcdir)/src
test_nccmp_stats_LDADD = $(top_builddir)/src/nccmp-log.o                 \
                         $(top_builddir)/src/nccmp-nccmp_darray.o        \
                         $(top_builddir)/src/nccmp-nccmp_stats.o         \
                         $(top_builddir)/src/nccmp-nccmp_strlist.o       \
                         $(top_builddir)/src/nccmp-nccmp_user_type.o     \
                         $(top_builddir)/src/nccmp-nccmp_utils.o         \
                         $(top_builddir)/src/nccmp-xmalloc.o

test_nccmp_strlist_CPPFLAGS = -I$(top_srcdir)/src
test_nccmp_strlist_LDADD = $(top_builddir)/src/nccmp-xmalloc.o           \
                           $(top_builddir)/src/nccmp-nccmp_strlist.o

test_nccmp_user_type_CPPFLAGS = -I$(top_srcdir)/src
test_nccmp_user_type_LDADD = $(top_builddir)/src/nccmp-log.o             \
                             $(top_builddir)/src/nccmp-nccmp_darray.o    \
                             $(top_builddir)/src/nccmp-nccmp_strlist.o   \
                             $(top_builddir)/src/nccmp-nccmp_user_type.o \
                             $(top_builddir)/src/nccmp-nccmp_utils.o     \
                             $(top_builddir)/src/nccmp-xmalloc.o 
