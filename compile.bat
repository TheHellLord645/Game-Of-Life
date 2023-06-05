@echo off
g++ -o main.exe main.cpp -I sfml/include -I imgui -L sfml/lib -lsfml-graphics -lsfml-window -lsfml-system
pause
main