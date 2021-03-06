
IF (CPPADCG_INCLUDES AND CPPADCG_LIBRARIES)
  SET(CPPADCG_FIND_QUIETLY TRUE)
ENDIF ()


IF(DEFINED CPPADCG_HOME)

  FIND_PATH(CPPADCG_INCLUDE_DIR NAMES cppadcg/cppadcg.hpp
            PATHS  "${CPPADCG_HOME}"
            NO_DEFAULT_PATH)

  SET(CPPADCG_INCLUDE_DIRS ${CPPADCG_INCLUDE_DIR})
  SET(CPPADCG_LIBRARIES ${CPPADCG_LIBRARY})

ELSE()

  FIND_PACKAGE(PkgConfig)

  IF( PKG_CONFIG_FOUND )
    pkg_check_modules( CPPADCG QUIET cppadcg)
  ENDIF()


  IF( NOT CPPADCG_FOUND )
    FIND_PATH(CPPADCG_INCLUDE_DIR NAMES cppadcg/cppadcg.hpp
              HINTS  "$ENV{CPPADCG_HOME}"
                     "/usr/include" )

    IF( CPPADCG_INCLUDE_DIR )
      SET(CPPADCG_INCLUDE_DIRS ${CPPADCG_INCLUDE_DIR} ${CLANG_INCLUDE_DIRS})
    ENDIF()

    IF( CPPADCG_LIBRARY )
        SET(CPPADCG_LIBRARIES ${CPPADCG_LIBRARY})
    ENDIF()

    INCLUDE(FindPackageHandleStandardArgs)
    # handle the QUIETLY and REQUIRED arguments and set CPPADCG_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(CppADCG  DEFAULT_MSG
                                      CPPADCG_INCLUDE_DIRS)

    MARK_AS_ADVANCED(CPPADCG_INCLUDE_DIRS CPPADCG_LIBRARIES)

  ENDIF()
ENDIF()


IF( CPPADCG_FOUND AND NOT CPPADCG_FIND_QUIETLY )
  MESSAGE(STATUS "package CppADCG found")
ENDIF()

