cmake_minimum_required(VERSION 3.10)
project(lang LANGUAGES CXX VERSION 1.0)

file(GLOB SRC
	"src/*.cpp"
	"src/*.h"
	"src/lexer/*.h"
	"src/lexer/*.cpp"
	"src/parser/*.h"
	"src/parser/*.cpp"
	"src/parser/detail/*.hpp"
)

add_executable(${PROJECT_NAME} ${SRC})
