include(Linux32CrossCompile)
set(CMAKE_C_COMPILER gcc -m32)
set(CMAKE_CXX_COMPILER g++ -m32)
set(CMAKE_ASM_FLAGS "${ASMFLAGS} -m32") # thanks to tmp64