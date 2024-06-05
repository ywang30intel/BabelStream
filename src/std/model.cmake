register_flag_optional(CMAKE_CXX_COMPILER
        "Any CXX compiler that is supported by CMake detection"
        "c++")

register_flag_optional(NVHPC_OFFLOAD
        "Enable offloading support (via the non-standard `-stdpar=gpu`) for the new NVHPC SDK.
         The values are Nvidia architectures in ccXY format will be passed in via `-gpu=` (e.g `cc70`)

         Possible values are:
           cc35  - Compile for compute capability 3.5
           cc50  - Compile for compute capability 5.0
           cc60  - Compile for compute capability 6.0
           cc62  - Compile for compute capability 6.2
           cc70  - Compile for compute capability 7.0
           cc72  - Compile for compute capability 7.2
           cc75  - Compile for compute capability 7.5
           cc80  - Compile for compute capability 8.0
           cc90  - Compile for compute capability 8.0
           ccall - Compile for all supported compute capabilities
           ccnative - Compiles for compute capability of current device"
	"")

register_flag_optional(ACPP_OFFLOAD
        "Enable offloading support (via the non-standard `-acpp-stdpar`) for AdaptiveCpp."
	"OFF")
      
register_flag_optional(USE_TBB
        "No-op if ONE_TBB_DIR is set. Link against an in-tree oneTBB via FetchContent_Declare, see top level CMakeLists.txt for details."
        "OFF")

register_flag_optional(USE_ONEDPL
        "Link oneDPL which implements C++17 executor policies (via execution_policy_tag) for different backends.

        Possible values are:
          OPENMP - Implements policies using OpenMP.
                   CMake will handle any flags needed to enable OpenMP if the compiler supports it.
          TBB    - Implements policies using TBB.
                   TBB must be linked via USE_TBB or be available in LD_LIBRARY_PATH.
          DPCPP  - Implements policies through SYCL2020.
                   This requires the DPC++ compiler (other SYCL compilers are untested), required SYCL flags are added automatically."
        "OFF")

register_flag_optional(STDIMPL
  "Implementation strategy (default = DATA20):
     DATA17  - Parallel algorithms over data (requires C++17).
     DATA23  - (default) Parallel algorithms over data (requires C++20).
     INDICES - Parallel algorithms over indices (requires C++20)."
  "DATA20"
)

macro(setup)
    register_definitions(${STDIMPL})
    if (${STDIMPL} STREQUAL "DATA17")
      set(CMAKE_CXX_STANDARD 17)
    elseif (${STDIMPL} STREQUAL "INDICES")
      set(CMAKE_CXX_STANDARD 20)
    elseif (${STDIMPL} STREQUAL "DATA23")
      set(CMAKE_CXX_STANDARD 23)
    endif ()
    if (NVHPC_OFFLOAD)
        set(NVHPC_FLAGS -stdpar=gpu -gpu=${NVHPC_OFFLOAD})
        # propagate flags to linker so that it links with the gpu stuff as well
        register_append_cxx_flags(ANY ${NVHPC_FLAGS})
        register_append_link_flags(${NVHPC_FLAGS})
    endif ()
    if (ACPP_OFFLOAD)
        set(ACPP_FLAGS --acpp-stdpar)
        register_append_cxx_flags(ANY ${ACPP_FLAGS})
        register_append_link_flags(${ACPP_FLAGS})      
    endif ()
    if (USE_TBB)
      if (FETCH_TBB)
	register_link_library(TBB::tbb)
      else ()
	register_link_library(tbb)
      endif ()
    endif ()
    if (USE_ONEDPL)
        register_definitions(USE_ONEDPL)
        register_link_library(oneDPL)
    endif ()
endmacro()
