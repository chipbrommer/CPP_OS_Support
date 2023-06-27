# CMakeList.txt : CMake project for CPP_OS_Support, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (
    CPP_OS_Support 
    "main.cpp"  
    "CPP_OS_Support/os_support.h" 
    "CPP_OS_Support/os_support.cpp"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CPP_OS_Support PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
