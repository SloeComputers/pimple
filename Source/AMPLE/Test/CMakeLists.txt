

add_executable(test_ample
               TestMain.cpp
               TestStrStack.cpp
               TestDict.cpp)

target_link_libraries(test_ample PLT)

add_test(NAME test_AMPLE COMMAND test_AMPLE)
