
set(SOURCE_FILES main.cpp foo.pb.cc)
add_executable(message_handlers ${SOURCE_FILES})
target_link_libraries(message_handlers ${CONAN_LIBS})
