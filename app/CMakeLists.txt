cmake_minimum_required(VERSION 3.4.1)

#INCLUDE_DIRECTORIES(src/main/cpp/include)
#INCLUDE_DIRECTORIES(src/main/cpp/libyuv)

add_library( playvideo
             SHARED
             src/main/cpp/dn_video_player.c )

add_library( libavutil-54
             SHARED
             IMPORTED)
set_target_properties( libavutil-54
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavutil-54.so)

add_library( libswresample-1
             SHARED
             IMPORTED)
set_target_properties( libswresample-1
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libswresample-1.so)

add_library( libavcodec-56
             SHARED
             IMPORTED)
set_target_properties( libavcodec-56
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavcodec-56.so)

add_library( libavformat-56
             SHARED
             IMPORTED)
set_target_properties( libavformat-56
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavformat-56.so)

add_library( libswscale-3
             SHARED
             IMPORTED)
set_target_properties( libswscale-3
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libswscale-3.so)

add_library( libpostproc-53
             SHARED
             IMPORTED)
set_target_properties( libpostproc-53
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libpostproc-53.so)

add_library( libavfilter-5
             SHARED
             IMPORTED)
set_target_properties( libavfilter-5
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavfilter-5.so)

add_library( libavdevice-56
             SHARED
             IMPORTED)
set_target_properties( libavdevice-56
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavdevice-56.so)

add_library( libyuv
             SHARED
             IMPORTED)
set_target_properties( libyuv
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libyuv.so)

find_library( log-lib
                log )

target_link_libraries(  playvideo
                android
                        libavutil-54
                        libswresample-1
                        libavcodec-56
                        libavformat-56
                        libswscale-3
                        libpostproc-53
                        libavfilter-5
                        libavdevice-56
                        libyuv
                        ${log-lib} )