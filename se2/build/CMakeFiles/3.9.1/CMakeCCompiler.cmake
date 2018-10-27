set(CMAKE_C_COMPILER "/usr/local/pacerepov1/pgi/14.10/linux86-64/14.10/bin/pgcc")
set(CMAKE_C_COMPILER_ARG1 "")
set(CMAKE_C_COMPILER_ID "PGI")
set(CMAKE_C_COMPILER_VERSION "14.10.0")
set(CMAKE_C_COMPILER_WRAPPER "")
set(CMAKE_C_STANDARD_COMPUTED_DEFAULT "99")
set(CMAKE_C_COMPILE_FEATURES "c_std_90;c_std_99")
set(CMAKE_C90_COMPILE_FEATURES "c_std_90")
set(CMAKE_C99_COMPILE_FEATURES "c_std_99")
set(CMAKE_C11_COMPILE_FEATURES "")

set(CMAKE_C_PLATFORM_ID "Linux")
set(CMAKE_C_SIMULATE_ID "")
set(CMAKE_C_SIMULATE_VERSION "")


set(CMAKE_AR "/usr/bin/ar")
set(CMAKE_C_COMPILER_AR "")
set(CMAKE_RANLIB "/usr/bin/ranlib")
set(CMAKE_C_COMPILER_RANLIB "")
set(CMAKE_LINKER "/usr/bin/ld")
set(CMAKE_COMPILER_IS_GNUCC )
set(CMAKE_C_COMPILER_LOADED 1)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_C_ABI_COMPILED TRUE)
set(CMAKE_COMPILER_IS_MINGW )
set(CMAKE_COMPILER_IS_CYGWIN )
if(CMAKE_COMPILER_IS_CYGWIN)
  set(CYGWIN 1)
  set(UNIX 1)
endif()

set(CMAKE_C_COMPILER_ENV_VAR "CC")

if(CMAKE_COMPILER_IS_MINGW)
  set(MINGW 1)
endif()
set(CMAKE_C_COMPILER_ID_RUN 1)
set(CMAKE_C_SOURCE_FILE_EXTENSIONS c;m)
set(CMAKE_C_IGNORE_EXTENSIONS h;H;o;O;obj;OBJ;def;DEF;rc;RC)
set(CMAKE_C_LINKER_PREFERENCE 10)

# Save compiler ABI information.
set(CMAKE_C_SIZEOF_DATA_PTR "8")
set(CMAKE_C_COMPILER_ABI "")
set(CMAKE_C_LIBRARY_ARCHITECTURE "")

if(CMAKE_C_SIZEOF_DATA_PTR)
  set(CMAKE_SIZEOF_VOID_P "${CMAKE_C_SIZEOF_DATA_PTR}")
endif()

if(CMAKE_C_COMPILER_ABI)
  set(CMAKE_INTERNAL_PLATFORM_ABI "${CMAKE_C_COMPILER_ABI}")
endif()

if(CMAKE_C_LIBRARY_ARCHITECTURE)
  set(CMAKE_LIBRARY_ARCHITECTURE "")
endif()

set(CMAKE_C_CL_SHOWINCLUDES_PREFIX "")
if(CMAKE_C_CL_SHOWINCLUDES_PREFIX)
  set(CMAKE_CL_SHOWINCLUDES_PREFIX "${CMAKE_C_CL_SHOWINCLUDES_PREFIX}")
endif()





set(CMAKE_C_IMPLICIT_LINK_LIBRARIES "pgmp;numa;pthread;nspgc;pgc;m;gcc;c;gcc")
set(CMAKE_C_IMPLICIT_LINK_DIRECTORIES "/usr/local/pacerepov1/openmpi/1.8/pgi-14.10/lib;/usr/local/pacerepov1/pgi/14.10/linux86-64/14.10/libso;/usr/local/pacerepov1/pgi/14.10/linux86-64/14.10/lib;/opt/torque/current/lib;/opt/oracle/current/lib;/nv/usr-local-rhel6.7/pacerepov1/pgi/14.10/linux86-64/14.10/lib;/usr/lib64;/usr/lib/gcc/x86_64-redhat-linux/4.4.7")
set(CMAKE_C_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")
