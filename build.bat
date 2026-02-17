@echo off

clang++ src/main.cpp -o main -lraylib -lGL -lm -lm -lpthread -ldl -lX11

main.exe
