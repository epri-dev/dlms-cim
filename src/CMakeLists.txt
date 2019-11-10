cmake_minimum_required(VERSION 3.1)
find_package(Threads REQUIRED)
find_package(GSOAP REQUIRED)

# we will put gsoap-generated files here
set(SOAP_GEN_DIR "${CMAKE_BINARY_DIR}/soap")
file(MAKE_DIRECTORY ${SOAP_GEN_DIR})

set(DLMS_LIBRARY_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../DLMS-COSEM")
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/ ${DLMS_LIBRARY_BASE_DIR}/lib/DLMS-COSEM/include/ ${DLMS_LIBRARY_BASE_DIR}/lib/asio-1.10.6/include/ ${GSOAP_INCLUDE_DIR} ${SOAP_GEN_DIR})

add_definitions(-DASIO_STANDALONE)
add_definitions(-DASIO_HAS_STD_CHRONO)

add_subdirectory(doc)


link_directories(${DLMS_LIBRARY_BASE_DIR}/build/lib/DLMS-COSEM/)

add_executable(DLMS_sim LinuxScheduler.cpp Linux.cpp LinuxBaseLibrary.cpp LinuxCore.cpp LinuxCOSEMServer.cpp LinuxDebug.cpp LinuxMemory.cpp LinuxSerial.cpp LinuxSimpleTimer.cpp LinuxSocket.cpp LinuxSynchronization.cpp LinuxClock.cpp LinuxData.cpp LinuxDisconnect.cpp LinuxImageTransfer.cpp)
add_executable(MeterSim LinuxScheduler.cpp LinuxMeterSim.cpp LinuxBaseLibrary.cpp LinuxCore.cpp LinuxCOSEMServer.cpp LinuxDebug.cpp LinuxMemory.cpp LinuxSerial.cpp LinuxSimpleTimer.cpp LinuxSocket.cpp LinuxSynchronization.cpp LinuxClock.cpp LinuxData.cpp LinuxDisconnect.cpp LinuxImageTransfer.cpp)
add_executable(HESSim LinuxScheduler.cpp LinuxHESSim.cpp LinuxBaseLibrary.cpp LinuxCore.cpp LinuxDebug.cpp LinuxMemory.cpp LinuxSerial.cpp LinuxSimpleTimer.cpp LinuxSocket.cpp LinuxSynchronization.cpp) 

## use gsoap to generate some source files
set_source_files_properties( ${SOAP_GEN_DIR}/soapC.cpp PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingService.cpp PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingProxy.cpp PROPERTIES GENERATED TRUE)

## custom commands to use gsoap
add_custom_command(
    OUTPUT 
        ${SOAP_GEN_DIR}/disconnect.h
    COMMAND ${GSOAP_WSDL2H} -c++11 -O3 -t ${CMAKE_CURRENT_SOURCE_DIR}/typemap.dat -o ${SOAP_GEN_DIR}/disconnect.h ${CMAKE_CURRENT_SOURCE_DIR}/wsdl/ExecuteEndDeviceControls.wsdl ${CMAKE_CURRENT_SOURCE_DIR}/xsd/*
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/wsdl/ExecuteEndDeviceControls.wsdl
    COMMENT "Building XML to C++ binding files for gsoap"
)

add_custom_command(
    OUTPUT 
        ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingService.cpp
        ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingProxy.cpp
    COMMAND ${GSOAP_SOAPCPP2} -c++11 -j -I${GSOAP_CUSTOM_DIR}/.. -d ${SOAP_GEN_DIR} ${SOAP_GEN_DIR}/disconnect.h
    DEPENDS ${SOAP_GEN_DIR}/disconnect.h
    COMMENT "Building gsoap-generated sources"
)

add_executable(disconnectserver disconnectserver.cpp ${SOAP_GEN_DIR}/soapC.cpp ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingService.cpp ${GSOAP_CUSTOM_DIR}/chrono_time_point.cpp UUID.cpp)
add_executable(disconnectclient disconnectclient.cpp ${SOAP_GEN_DIR}/soapC.cpp ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingProxy.cpp ${GSOAP_CUSTOM_DIR}/chrono_time_point.cpp UUID.cpp)
target_link_libraries(DLMS_sim DLMS-COSEM ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(MeterSim DLMS-COSEM ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(HESSim DLMS-COSEM ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(disconnectserver ${GSOAP_CXX_LIBRARIES})
target_link_libraries(disconnectclient ${GSOAP_CXX_LIBRARIES})
