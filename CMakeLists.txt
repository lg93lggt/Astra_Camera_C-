cmake_minimum_required(VERSION 2.6)
project(fdg)

FIND_PACKAGE(OpenCV  REQUIRED )
aux_source_directory(src/ DIR_SRCS)
set(OPENNI2_INCLUDE "/home/supeng/Program/orbbec/Astra/Include")
set(OPENNI2_REDIST "/home/supeng/Program/orbbec/Astra/lib_sp/OpenNI2/Drivers")
set(OPENNI2_REDIST_2 "/home/supeng/Program/orbbec/Astra/lib_sp")
link_directories(${OPENNI2_REDIST})
link_directories(${OPENNI2_REDIST_2})
include_directories(${OPENNI2_INCLUDE})



# add_executable(fdg ${DIR_SRCS})

add_executable(fdg main.cpp)

TARGET_LINK_LIBRARIES(fdg ${OPENNI2_REDIST}/libOniFile.so)
TARGET_LINK_LIBRARIES(fdg ${OPENNI2_REDIST}/liborbbec.so)
TARGET_LINK_LIBRARIES(fdg ${OPENNI2_REDIST}/libPSLink.so)

TARGET_LINK_LIBRARIES(fdg ${OPENNI2_REDIST_2}/libOpenNI2.jni.so)
TARGET_LINK_LIBRARIES(fdg ${OPENNI2_REDIST_2}/libOpenNI2.so)



TARGET_LINK_LIBRARIES(fdg ${OpenCV_LIBS})
message(${OpenCV_LIBS})
install(TARGETS fdg RUNTIME DESTINATION bin)
