

set(SOURCE_FILES foo.pb.cc
        basic.cpp)
add_executable(message_handlers_test ${SOURCE_FILES} main.cpp )
target_link_libraries(message_handlers_test ${CONAN_LIBS})

#    add_test(NAME basic COMMAND basic_test)


