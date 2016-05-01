

set(SOURCE_FILES foo.pb.cc
        basic.cpp)
add_executable(gtest_main ${SOURCE_FILES} main.cpp )
target_link_libraries(gtest_main ${CONAN_LIBS})

#    add_test(NAME basic COMMAND basic_test)


