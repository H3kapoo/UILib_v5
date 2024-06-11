cmake_minimum_required(VERSION 3.22)
project(test)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    #Linux-specific code or configuration here
    message(STATUS "Configuring for Linux")
    set(CXX_STANDARD 23)

    # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")
    # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -ggdb -g -DTRACY_ENABLE")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -ggdb -g")
    # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -ggdb -g -DRENDER_FRONT_TO_BACK")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../debug)

    # set(HKUI_LIBRARY_INCLUDE_DIRECTORY "/home/hekapoo/lib_hkui/include")
    set(FT_LIBRARY_INCLUDE_DIRECTORY "/home/hekapoo/Downloads/freeType2/freetype-2.13.1/include/")
    set(GLEW_LIBRARY_INCLUDE_DIRECTORY "/home/hekapoo/Downloads/glew-2.1.0/include")
    set(GLFW_LIBRARY_INCLUDE_DIRECTORY "/home/hekapoo/Downloads/glfw-3.3.8/include")

    # include_directories (${HKUI_LIBRARY_INCLUDE_DIRECTORY})
    include_directories (${GLEW_LIBRARY_INCLUDE_DIRECTORY})
    include_directories (${GLFW_LIBRARY_INCLUDE_DIRECTORY})
    
    add_executable(${PROJECT_NAME}
        main.cpp
        App.cpp
        src/Utility.cpp
        src/assetLoaders/TextureLoader.cpp
        src/components/ScrollBar.cpp
        src/components/SeparatorBar.cpp
        src/components/Div.cpp
        src/components/ResizeDiv.cpp
        src/components/Button.cpp
        src/components/TabSwitcher.cpp
        src/components/AbstractComponent.cpp
        src/components/ComponentManager.cpp
        src/components/layoutCalc/LayoutCalculator.cpp
        src/components/layoutCalc/PinchLayoutCalculator.cpp
        src/inputManagement/Input.cpp
        src/mesh/QuadMesh.cpp
        src/renderer/Renderer.cpp
        src/shaderManagement/ShaderLoader.cpp
        )
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
    
    # Needed for absolute include paths
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

    # set(HKUI_LIBRARY_PATH_DIRECTORY "/home/hekapoo/lib_hkui/lib/")
    set(FT_LIBRARY_PATH_DIRECTORY "/home/hekapoo/Downloads/freeType2/freetype-2.13.1/objs/.libs")
    set(GLEW_LIBRARY_PATH_DIRECTORY "/home/hekapoo/Downloads/glew-2.1.0/lib")
    set(GLFW_LIBRARY_PATH_DIRECTORY "/home/hekapoo/Downloads/glfw-3.3.8/src")

    target_link_directories(${PROJECT_NAME} PRIVATE
        ${HKUI_LIBRARY_PATH_DIRECTORY}        # Path to the directory containing HKUI library file
        ${GLEW_LIBRARY_PATH_DIRECTORY}        # Path to the directory containing GLEW library file
        ${GLFW_LIBRARY_PATH_DIRECTORY}        # Path to the directory containing GLFW library file
    )
    target_link_libraries(${PROJECT_NAME} glfw3 GLEW GL X11 Xrandr pthread dl freetype)

# If the operating system is not recognized
else()
    message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()