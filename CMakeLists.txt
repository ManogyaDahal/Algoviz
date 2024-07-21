cmake_minimum_required(VERSION 3.16)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
)
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.90.8
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG v2.6
)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)
FetchContent_MakeAvailable(imgui-sfml)

FetchContent_Declare(
    ImPlot
    GIT_REPOSITORY https://github.com/epezent/implot
    GIT_TAG v0.16
)
FetchContent_MakeAvailable(ImPlot)

add_executable(main src/main.cpp)

target_link_libraries(
    main PUBLIC
    sfml-graphics sfml-system sfml-window ImGui-SFML::ImGui-SFML
)

target_include_directories(main PRIVATE ${ImPlot_SOURCE_DIR}/include)

target_compile_features(main PRIVATE cxx_std_17)

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH_DIR x64)
    else()
        set(ARCH_DIR x86)
    endif()

    set(OPENAL_DLL_SOURCE ${SFML_SOURCE_DIR}/extlibs/bin/${ARCH_DIR}/openal32.dll)
    set(OPENAL_DLL_DESTINATION $<TARGET_FILE_DIR:main>)

    add_custom_command(
        TARGET main
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${OPENAL_DLL_SOURCE} ${OPENAL_DLL_DESTINATION}
        COMMENT "Copying OpenAL DLL"
        VERBATIM
    )
endif()
