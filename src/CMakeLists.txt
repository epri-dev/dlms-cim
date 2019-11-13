set(DLMS_LIBRARY_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../DLMS-COSEM")
# we want ot use this particular version of the asio library
set(ASIO_ROOT ${DLMS_LIBRARY_BASE_DIR}/lib/asio-1.10.6)
find_package(Asio REQUIRED)
find_package(Threads REQUIRED)
find_package(GSOAP REQUIRED)

# specifics for asio
add_definitions(-DASIO_STANDALONE)
add_definitions(-DASIO_HAS_STD_CHRONO)

# we will put gsoap-generated files here
set(SOAP_GEN_DIR "${CMAKE_BINARY_DIR}/soap")
file(MAKE_DIRECTORY ${SOAP_GEN_DIR})

include_directories(${CMAKE_CURRENT_LIST_DIR}/include/ ${DLMS_LIBRARY_BASE_DIR}/lib/DLMS-COSEM/include/ ${ASIO_INCLUDE_DIR} ${GSOAP_INCLUDE_DIR} ${SOAP_GEN_DIR})

# Create the documentation 
add_subdirectory(doc)

link_directories(${DLMS_LIBRARY_BASE_DIR}/build/lib/DLMS-COSEM/)
set(DLMS_COMMON_SOURCES LinuxScheduler.cpp LinuxBaseLibrary.cpp LinuxCore.cpp LinuxDebug.cpp LinuxMemory.cpp LinuxSerial.cpp LinuxSimpleTimer.cpp LinuxSocket.cpp LinuxSynchronization.cpp)
set(DLMS_SERVER_COMMON_SOURCES LinuxCOSEMServer.cpp LinuxClock.cpp LinuxData.cpp LinuxDisconnect.cpp LinuxImageTransfer.cpp)

## use gsoap to generate some source files
set_source_files_properties( ${SOAP_GEN_DIR}/soapC.cpp PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingService.cpp PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingProxy.cpp PROPERTIES GENERATED TRUE)

## custom commands to use gsoap
add_custom_command(
    OUTPUT 
        ${SOAP_GEN_DIR}/disconnect.h
    COMMAND ${GSOAP_WSDL2H} -c++11 -O3 -t ${CMAKE_CURRENT_SOURCE_DIR}/typemap.dat -o ${SOAP_GEN_DIR}/disconnect.h ${CMAKE_CURRENT_SOURCE_DIR}/wsdl/ExecuteEndDeviceControls.wsdl ${CMAKE_CURRENT_SOURCE_DIR}/xsd/*
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/wsdl/ExecuteEndDeviceControls.wsdl ${CMAKE_CURRENT_SOURCE_DIR}/typemap.dat
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

## finally some executables
add_executable(DLMS_sim Linux.cpp ${DLMS_SERVER_COMMON_SOURCES} ${DLMS_COMMON_SOURCES})
add_executable(Metersim Metersim.cpp ${DLMS_SERVER_COMMON_SOURCES} ${DLMS_COMMON_SOURCES})
add_executable(HESsim HESsim.cpp ${DLMS_COMMON_SOURCES} ${SOAP_GEN_DIR}/soapC.cpp ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingService.cpp ${GSOAP_CUSTOM_DIR}/chrono_time_point.cpp UUID.cpp)

add_executable(disconnectserver disconnectserver.cpp ${SOAP_GEN_DIR}/soapC.cpp ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingService.cpp ${GSOAP_CUSTOM_DIR}/chrono_time_point.cpp UUID.cpp)
add_executable(disconnectclient disconnectclient.cpp ${SOAP_GEN_DIR}/soapC.cpp ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingProxy.cpp ${GSOAP_CUSTOM_DIR}/chrono_time_point.cpp UUID.cpp)
add_executable(CISsim CISsim.cpp ${SOAP_GEN_DIR}/soapC.cpp ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingService.cpp ${SOAP_GEN_DIR}/soapEndDeviceControls_USCOREBindingProxy.cpp ${GSOAP_CUSTOM_DIR}/chrono_time_point.cpp UUID.cpp)

## and the various required libraries
target_link_libraries(DLMS_sim DLMS-COSEM ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Metersim DLMS-COSEM ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(HESsim DLMS-COSEM ${GSOAP_CXX_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(disconnectserver ${GSOAP_CXX_LIBRARIES})
target_link_libraries(disconnectclient ${GSOAP_CXX_LIBRARIES})
target_link_libraries(CISsim ${GSOAP_CXX_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

## install things
INSTALL(TARGETS Metersim HESsim CISsim DESTINATION bin)
