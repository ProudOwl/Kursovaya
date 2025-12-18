cmake_minimum_required(VERSION 3.16)
project(AstroLearn)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск зависимостей
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(PostgreSQL REQUIRED)

# Настройка исходных файлов
set(SOURCE_FILES
    src/main.cpp
    src/game.cpp
    src/game_states.cpp
    src/game_ui.cpp
    src/game_logic.cpp
    src/game_database.cpp
    src/player.cpp
    src/quiz.cpp
    src/database.cpp
    src/celestial_body.cpp
    src/solar_system.cpp
)

# Настройка исполняемого файла
add_executable(astrolearn ${SOURCE_FILES})

# Подключение библиотек
target_link_libraries(astrolearn 
    sfml-graphics 
    sfml-window 
    sfml-system
    pqxx
    pq
)

# Настройка флагов компиляции
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(astrolearn PRIVATE -g -O0)
else()
    target_compile_options(astrolearn PRIVATE -O2)
endif()
