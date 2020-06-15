
# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)

#设置OpenCv sdk的路径
set(openCVPath /home/zh/Android/OpenCV-android-sdk/)

#支持-std=gnu++11
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

#加载opencv库
add_library(lib_opencv STATIC IMPORTED )

#引入libopencv_java3.so文件
set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libopencv_java3.so )

# 查找当前目录下的所有源文件，并保存到 DIR_LIB_SRCS 变量
aux_source_directory(src/main/cpp/src DIR_SRCS)

#配置include路径
include_directories(
                    ${openCVPath}/sdk/native/jni/include
                    src/main/cpp/include)

# 生成共享库
ADD_LIBRARY(native-lib SHARED ${DIR_SRCS})

#查找库
find_library(
        log-lib
        log
)

#链接库
target_link_libraries(
        native-lib
        lib_opencv
        android -ljnigraphics
        ${log-lib}
)
