# A helper function for adding applications for libraries.
function(add_app)
    cmake_parse_arguments(
        PARSED_ARGS # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "APP_NAME" # list of names of mono-valued arguments
        "APP_SRCS;APP_DEPS" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )
    add_executable(${PARSED_ARGS_APP_NAME})

    target_sources(${PARSED_ARGS_APP_NAME}
        PRIVATE
        ${PARSED_ARGS_APP_SRCS}
    )

    target_include_directories(${PARSED_ARGS_APP_NAME}
        PRIVATE
        ${smorevision_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
    )

    if(UNIX)
        target_link_libraries(${PARSED_ARGS_APP_NAME}
            PRIVATE
            ${PARSED_ARGS_APP_DEPS} "stdc++fs"
            ${smorevision_LIBRARIES}
            ${OpenCV_LIBS}
            # Eigen3::Eigen
        )
    else()
        target_link_libraries(${PARSED_ARGS_APP_NAME}
            PRIVATE
            ${PARSED_ARGS_APP_DEPS}
            ${smorevision_LIBRARIES}
            ${OpenCV_LIBS}
            # Eigen3::Eigen
        )

        if(GENORATE_VS_DEBUG_INFORM)
            set_target_properties(${PARSED_ARGS_APP_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
            set_target_properties(${PARSED_ARGS_APP_NAME} PROPERTIES LINK_FLAGS "/DEBUG")
        endif()
    endif()

    add_test(${PARSED_ARGS_APP_NAME} ${PARSED_ARGS_APP_NAME})

    set_target_properties(${PARSED_ARGS_APP_NAME} PROPERTIES FOLDER "apps")

    install(
        TARGETS ${PARSED_ARGS_APP_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/apps
    )
    install(
        FILES ${PROJECT_SOURCE_DIR}/apps/${PARSED_ARGS_APP_SRCS}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/apps
    )
endfunction(add_app)