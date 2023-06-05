#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>

#include "SFML/Graphics.hpp"

const int WIDTH = 1360;
const int HEIGHT = 700;
const int FPS = 60;
const int TIMESTEP = 6;

const sf::Color DARK_GRAY(50, 50, 50);

/*
*                                           Rules
* -------------------------------------------------------------------------------------------
* Any live cell with fewer than two live neighbors dies as if caused by under-population.
* Any live cell with two or three live neighbors lives on to the next generation.
* Any live cell with more than three live neighbors dies, as if by over-population.
* Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
* -------------------------------------------------------------------------------------------
*/

struct GameOfLife {
    std::vector<std::vector<int>> board;
    int timeStepCounter = 0;
    bool paused = false;

    GameOfLife(int height, int width) {
        std::vector<std::vector<int>> board(height, std::vector<int>(width));
        this->board = board;
    }

    void add(int x, int y) {
        board[y][x] = 1;
    }

    void remove(int x, int y) {
        board[y][x] = 0;
    }

    void input(sf::RenderWindow& window, sf::Event& event) {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
            paused = !paused;
        }

        bool left = false, right = false;

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) left = true;
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) right = true;

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int x = clamp((mousePos.x * board[0].size())/WIDTH, board[0].size()-1, 0);
        int y = clamp((mousePos.y * board.size())/HEIGHT, board.size()-1, 0);

        if (left) add(x, y);
        else if (right) remove(x, y);
    }

    void clear() {
        board = std::vector<std::vector<int>>(board.size(), std::vector<int>(board[0].size()));
    }

    void update() {
        if (paused) return;
        timeStepCounter++;

        if (timeStepCounter >= TIMESTEP) {
            timeStepCounter = 0;
        }
        else {
            return;
        }

        for (int r = 0; r < board.size(); r++) {
            for (int c = 0; c < board[0].size(); c++) {
                int n = 0;
                for (int i = fmax(r-1, 0); i < fmin(r+2, board.size()); i++) {
                    for (int j = fmax(c-1, 0); j < fmin(c+2, board[0].size()); j++) {
                        n += board[i][j] & 1;
                    }
                }
                if (n == 3 || n - board[r][c] == 3)
                    board[r][c] |= 2;
            }
        }
        for (int i=0; i<board.size(); i++) {
            for (int j=0; j<board[0].size(); j++) {
                board[i][j] >>= 1;
            } 
        }
    }

    void save(std::string& filename, int boardHeight, int boardWidth) {
        std::ofstream file("saves/" + filename);
        file << boardHeight << '\n' << boardWidth << '\n';
        for (int r = 0; r < board.size(); r++) {
            for (int c = 0; c < board[0].size(); c++) {
                file << board[r][c];
            }
            file << '\n';
        }
        file.close();
    }
    // To Be Implemented
    /*void load(std::string& filename) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
            std::ifstream file("saves/" + filename);
            int boardHeight, boardWidth;

            for (int r = 0; r < board.size(); r++) {
                for (int c = 0; c < board[0].size(); c++) {
                    file >> board[r][c];
                }
            }
            file.close();
        }
    }*/

    void draw(sf::RenderWindow& window) {
        for (int i=0; i<board.size(); i++) {
            for (int j=0; j<board[0].size(); j++) {
                if (board[i][j]) {
                    sf::RectangleShape cell(sf::Vector2f(WIDTH/board[0].size(), HEIGHT/board.size()));
                    cell.setFillColor(sf::Color::White);
                    cell.setPosition((WIDTH/board[0].size()) * j, (HEIGHT/board.size())*i);
                    window.draw(cell);
                }
            }
        }
    }

    void drawGrid(sf::RenderWindow& window) {
        for (int i=0; i<WIDTH; i+=WIDTH/board[0].size()) {
            sf::RectangleShape gridX(sf::Vector2f(1, HEIGHT));
            gridX.setFillColor(DARK_GRAY);
            gridX.setPosition(i, 0);
            window.draw(gridX);
        }

        for (int j=0; j<HEIGHT; j+=HEIGHT/board.size()) {
            sf::RectangleShape gridY(sf::Vector2f(WIDTH, 1));
            gridY.setFillColor(DARK_GRAY);
            gridY.setPosition(0, j);
            window.draw(gridY);
        }
    }

    void print() {
        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board[0].size(); j++) {
                std::cout << board[i][j] << ' ';
            }
            std::cout << '\n';
        }
        std::cout << "\n----------------------------------------------------------------\n\n";
    }
private:
    int clamp(int num, int high, int low) {
        return std::min(std::max(num, low), high);
    }
};

int main() {
    int boardWidth = 136, boardHeight = 70;
    GameOfLife board(boardHeight, boardWidth);

    std::vector<std::vector<int>> pos = {{2,1},{3,2},{1,3},{2,3},{3,3}}; // Glider
    for (int i = 0; i < pos.size(); i++) {
        board.add(pos[i][0], pos[i][1]);
    }    

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "John Conway's Game Of Life");
    window.setFramerateLimit(FPS);

    sf::Font font;
    font.loadFromFile("fonts/arial.ttf");

    std::string filename = "save.txt";

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    bool drawUI = true;
    bool drawGrid = true;

    sf::Clock clock;

    while (window.isOpen()) {
        window.clear(sf::Color(0, 0, 0));

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();        
            }

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::G:
                        drawGrid = !drawGrid;
                        break;
                    case sf::Keyboard::S:
                        board.save(filename, boardHeight, boardWidth);
                        break;
                    case sf::Keyboard::U:
                        drawUI = !drawUI;
                        break;
                    case sf::Keyboard::C:
                        board.clear();
                        break;
                }
            }
            board.input(window, event);
        }

        board.update();
        board.draw(window);

        if (drawGrid) board.drawGrid(window);

        if (drawUI) {
            float currentTime = clock.restart().asSeconds();
            int fps = 1 / (currentTime);

            std::string textInput[] = {"FPS: " + std::to_string(fps) + " / " + std::to_string(currentTime*1000) + "ms",
                                        "[SPACE] Paused: " + std::to_string(board.paused),
                                        "[G] Draw Grid",
                                        "[S] Save",
                                        "[U] Draw UI",
                                        "[C] Clear"
            };

            for (int i = 0; i < 6; i++) {
                text.setString(textInput[i]);
                text.setPosition(20, (22*i)+10);
                window.draw(text);
            }
        }

        window.display();
    }

    return 0;
}