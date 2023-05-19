set(target_name fourier_melin)
add_library(${target_name})

# lib
if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} PRIVATE __BUILD_SHARED__)
    target_compile_definitions(${target_name} PRIVATE __EXPORT_LIBS__)
endif()

file(GLOB_RECURSE SRC_FILES
    ${PROJECT_SOURCE_DIR}/fftm/fftm.cpp
)
target_sources(${target_name}
    PRIVATE
    ${SRC_FILES}
)

target_include_directories(${target_name}
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(${target_name}
    PRIVATE
    ${OpenCV_LIBS}
)

install(TARGETS ${target_name}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(MSVC AND GENORATE_VS_DEBUG_INFORM)
    set_target_properties(${target_name} PROPERTIES LINK_FLAGS "/DEBUG")
endif()

# install opencv dll in windows
if(MSVC)
    file(GLOB DllFiles
        ${OpenCV_DIR}/../bin/*.dll
        ${smorevision_DIR}/../../bin/*.dll
    )

    foreach(DllFile ${DllFiles})
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E
            copy ${DllFile} $<TARGET_FILE_DIR:${target_name}>
        )
    endforeach()
endif()