cmake_minimum_required(VERSION 3.15)
project(TicTacToe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути к исходникам
set(SRC_DIR "${PROJECT_SOURCE_DIR}/TicTacToe")
set(EXTERNAL_DIR "${SRC_DIR}/external/doctest")

# Основной исполняемый файл
add_executable(TicTacToe
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/Game.cpp 
)

# Тесты с doctest (добавляем doctest.cpp вручную)
add_executable(TicTacToeTests
    ${SRC_DIR}/tests.cpp
    ${SRC_DIR}/Game.cpp
    ${SRC_DIR}/main.cpp            # если нужны функции из main.cpp
)

# Пути к заголовочным файлам (для doctest)
target_include_directories(TicTacToe PRIVATE ${EXTERNAL_DIR})
target_include_directories(TicTacToeTests PRIVATE ${EXTERNAL_DIR})

# Линки библиотек для оконного приложения (WinAPI)
target_link_libraries(TicTacToe PRIVATE user32 gdi32)
target_link_libraries(TicTacToeTests PRIVATE user32 gdi32)

add_definitions(-DUNICODE -D_UNICODE)

set_target_properties(TicTacToe PROPERTIES
    WIN32_EXECUTABLE TRUE
)

enable_testing()

# Регистрируем тест
add_test(NAME TicTacToeTests COMMAND TicTacToeTests)

