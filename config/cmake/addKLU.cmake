
OPTION(KLU_ENABLE "Enable KLU support" ON)

if(NOT DEFINED SuiteSparse_DIR)
  set(SuiteSparse_DIR ${PROJECT_BINARY_DIR}/libs CACHE PATH "path to SuiteSparse/KLU")
endif()

SHOW_VARIABLE(SuiteSparse_DIR PATH
  "KLU library directory" "${SuiteSparse_DIR}")


OPTION(AUTOBUILD_KLU "enable Suitesparse to automatically download and build" ON)
		

if(KLU_ENABLE)
  IF(MSVC)
    set(SuiteSparse_USE_LAPACK_BLAS ON)
  ENDIF(MSVC)
  FILE(GLOB_RECURSE SUITESPARSE_CONFIG_FILE ${SuiteSparse_DIR}/*/suitesparse-config.cmake)
  if (SUITESPARSE_CONFIG_FILE)
  message(status "loading suitesparse config a ${SUITESPARSE_CONFIG_FILE}")
	include(${SUITESPARSE_CONFIG_FILE})
	SET(KLU_CMAKE TRUE)
	set(SuiteSparse_DIRECT_LIBRARY_DIR ${SuiteSparse_DIR}/lib)
	set(SuiteSparse_DIRECT_INCLUDE_DIR ${SuiteSparse_INCLUDE_DIRS}/suitesparse)
  else()
	find_package(SuiteSparse COMPONENTS KLU AMD COLAMD BTF SUITESPARSECONFIG CXSPARSE)
	if(SuiteSparse_FOUND) 
	SET(KLU_CMAKE FALSE)
        list(APPEND external_library_list ${SuiteSparse_LIBRARIES})
        #list(APPEND external_link_directories ${SuiteSparse_LIBRARY_DIR})
		get_filename_component(SuiteSparse_DIRECT_LIBRARY_DIR ${SuiteSparse_KLU_LIBRARY_RELEASE} DIRECTORY)
		set(SuiteSparse_DIRECT_INCLUDE_DIR ${SuiteSparse_KLU_INCLUDE_DIR})
        IF (AUTOBUILD_KLU)
			OPTION(FORCE_KLU_REBUILD "force rebuild of KLU" OFF)
			IF(FORCE_KLU_REBUILD)
				include(buildSuiteSparse)
				build_suitesparse()
				set(FORCE_KLU_REBUILD OFF CACHE BOOL "force rebuild of KLU" FORCE)
			ENDIF(FORCE_KLU_REBUILD)
		ENDIF(AUTOBUILD_KLU)
	else(SuiteSparse_FOUND)
		
		IF (AUTOBUILD_KLU)
			include(buildSuiteSparse)
			build_suitesparse()
			set(SuiteSparse_DIR ${PROJECT_BINARY_DIR}/libs)
			FILE(GLOB_RECURSE SUITESPARSE_CONFIG_FILEb ${SuiteSparse_DIR}/*/suitesparse-config.cmake)
			if (SUITESPARSE_CONFIG_FILEb)
				include(${SUITESPARSE_CONFIG_FILEb})
				message(status "loading suitesparse config b ${SUITESPARSE_CONFIG_FILEb}")
				SET(KLU_CMAKE TRUE)
				set(SuiteSparse_DIRECT_LIBRARY_DIR ${SuiteSparse_DIR}/lib)
				set(SuiteSparse_DIRECT_INCLUDE_DIR ${SuiteSparse_INCLUDE_DIRS}/suitesparse)
			endif()
		ENDIF(AUTOBUILD_KLU)
	endif(SuiteSparse_FOUND)
  endif()

endif(KLU_ENABLE)
