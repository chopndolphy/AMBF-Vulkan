

add_executable(ipc_test_client ${CMAKE_CURRENT_LIST_DIR}/ipc_test_client.cpp)


set(INTERPROCESS_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../lib/boost)
target_include_directories(ipc_test_client PUBLIC ${INTERPROCESS_INCLUDE_DIR} ${CMAKE_CURRENT_LIST_DIR}/../lib)
target_compile_definitions(ipc_test_client PUBLIC BOOST_DATE_TIME_NO_LIB)

find_library(XXF86VM Xxf86vm)
target_link_libraries(ipc_test_client PUBLIC ${XXF86VM})

find_library(X11 X11)
target_link_libraries(ipc_test_client PUBLIC ${X11})

find_library(PTHREAD pthread)
target_link_libraries(ipc_test_client PUBLIC ${PTHREAD})

find_library(XRANDR Xrandr)
target_link_libraries(ipc_test_client PUBLIC ${XRANDR})

find_library(XI Xi)
target_link_libraries(ipc_test_client PUBLIC ${XI})

find_library(DL dl)
target_link_libraries(ipc_test_client PUBLIC ${DL})