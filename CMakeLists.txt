cmake_minimum_required(VERSION 3.10)

project(cv_test)

set(CMAKE_CXX_STANDARD 14) # C14 required to compile Torch
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
add_compile_definitions(_GLIBCXX_USE_CXX14_ABI=0) # Torch is compiled with CXX11_ABI, so your program needs to be also, or you may have conflicts in some libraries (such as GTest for example)

link_directories(./lib)
# Specifying we are using pthread for UNIX systems.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS} -pthread -Wall -ldl -g")
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV = ${OpenCV_LIBS}")

## collect CYL src
message(STATUS "[${PROJECT_NAME}] aux_source_directory CYL")
aux_source_directory(./CYL mySRC)
add_library(cyl-static STATIC ${mySRC})
target_include_directories(cyl-static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ./include)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/testCase)
message(STATUS "[${PROJECT_NAME}] add_executable")
#add_executable(thermal src/main.cpp src/async.cpp src/misc.cpp src/Face/UltraFace.cpp src/Face/MnnModel.cpp src/Temp/temp.cpp src/Mask/maskTF.cpp src/Stereo/stereo.cpp src/usb_znp.cpp src/rbuf.c src/loguru.cpp src/SensorHub/SensorHub.cpp src/v4l2_lpbk.cpp src/SingleLens/onecam.cpp)
add_executable(${PROJECT_NAME} test.cpp)
# add_executable(${PROJECT_NAME} test.cpp _CYL/MnnModel.cpp _CYL/UltraFace.cpp)
#add_executable(thermal src/main.cpp src/async.cpp src/misc.cpp ${Face} ${Mask} ${Stereo} src/loguru.cpp ${SensorHub} src/v4l2_lpbk.cpp ${SingleLens} ${temp})

#target_link_libraries(${PROJECT_NAME} MNN ${OpenCV_LIBS} pthread wiringPi ncnn LEPTON_SDK dl util ${TORCH_LIBRARIES})
message(STATUS "[${PROJECT_NAME}] target_link_libraries")
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} MNN cyl-static)





