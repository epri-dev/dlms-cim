cmake_minimum_required(VERSION 3.1)
find_package(Threads REQUIRED)

set(DLMS_LIBRARY_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../DLMS-COSEM")
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/ ${DLMS_LIBRARY_BASE_DIR}/lib/DLMS-COSEM/include/ ${DLMS_LIBRARY_BASE_DIR}/lib/asio-1.10.6/include/)

add_definitions(-DASIO_STANDALONE)
add_definitions(-DASIO_HAS_STD_CHRONO)

add_subdirectory(doc)

link_directories(${DLMS_LIBRARY_BASE_DIR}/build/lib/DLMS-COSEM/)

add_executable(DLMS_sim LinuxScheduler.cpp Linux.cpp LinuxBaseLibrary.cpp LinuxCore.cpp LinuxCOSEMServer.cpp LinuxDebug.cpp LinuxMemory.cpp LinuxSerial.cpp LinuxSimpleTimer.cpp LinuxSocket.cpp LinuxSynchronization.cpp LinuxClock.cpp LinuxData.cpp LinuxDisconnect.cpp)
target_link_libraries(DLMS_sim DLMS-COSEM ${CMAKE_THREAD_LIBS_INIT} "${LIBRARIES_FROM_REFERENCES}")
