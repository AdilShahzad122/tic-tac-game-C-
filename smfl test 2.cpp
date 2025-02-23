#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace std;

vector<vector<char>> board(3, vector<char>(3, ' '));
char currentPlayer = 'X';
bool gameOver = false;
string winnerText = "";

sf::Font loadFont() {
    sf::Font font;
    if (!font.loadFromFile("ariblk.ttf")) {
        cerr << "Error loading font" << endl;
    }
    return font;
}

void resetBoard() {
    for (auto& row : board) fill(row.begin(), row.end(), ' ');
    currentPlayer = 'X';
    gameOver = false;
    winnerText = "";
}

char checkWinner() {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ') return board[i][0];
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ') return board[0][i];
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') return board[0][0];
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ') return board[0][2];
    for (auto& row : board) for (auto& cell : row) if (cell == ' ') return ' ';
    return 'D'; // Draw
}

void handleClick(sf::Vector2i mousePos, int cellSize, sf::RenderWindow& window) {
    if (gameOver) return;
    sf::Vector2u windowSize = window.getSize();
    int gridOffsetX = (windowSize.x - cellSize * 3) / 2;
    int gridOffsetY = (windowSize.y - cellSize * 3) / 2;

    int row = (mousePos.y - gridOffsetY) / cellSize;
    int col = (mousePos.x - gridOffsetX) / cellSize;
    if (row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ') {
        board[row][col] = currentPlayer;
        char winner = checkWinner();
        if (winner != ' ') {
            gameOver = true;
            winnerText = (winner == 'D') ? "It's a Draw!" : string("Player ") + winner + " Wins!";
        }
        else {
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }
    }
}

void drawShapes(sf::RenderWindow& window, int cellSize, int offsetX, int offsetY) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int centerX = offsetX + j * cellSize + cellSize / 2;
            int centerY = offsetY + i * cellSize + cellSize / 2;
            if (board[i][j] == 'X') {
                sf::RectangleShape line1(sf::Vector2f(cellSize - 40, 10));
                sf::RectangleShape line2(sf::Vector2f(cellSize - 40, 10));
                line1.setFillColor(sf::Color::Red);
                line2.setFillColor(sf::Color::Red);
                line1.setOrigin((cellSize - 40) / 2, 5);
                line2.setOrigin((cellSize - 40) / 2, 5);
                line1.setPosition(centerX, centerY);
                line2.setPosition(centerX, centerY);
                line1.setRotation(45);
                line2.setRotation(-45);
                window.draw(line1);
                window.draw(line2);
            }
            else if (board[i][j] == 'O') {
                sf::CircleShape circle((cellSize - 40) / 2);
                circle.setFillColor(sf::Color::Transparent);
                circle.setOutlineThickness(10);
                circle.setOutlineColor(sf::Color::Blue);
                circle.setOrigin((cellSize - 40) / 2, (cellSize - 40) / 2);
                circle.setPosition(centerX, centerY);
                window.draw(circle);
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Tic-Tac-Toe", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    sf::Font font = loadFont();
    sf::Text statusText("", font, 40);
    statusText.setFillColor(sf::Color::Black);

    sf::RectangleShape restartButton(sf::Vector2f(200, 60));
    restartButton.setFillColor(sf::Color(100, 100, 250));

    sf::Text restartText("Restart", font, 30);
    restartText.setFillColor(sf::Color::White);

    while (window.isOpen()) {
        sf::Vector2u windowSize = window.getSize();
        int cellSize = min(windowSize.x, windowSize.y - 100) / 3;
        int gridOffsetX = (windowSize.x - cellSize * 3) / 2;
        int gridOffsetY = (windowSize.y - cellSize * 3) / 2;

        statusText.setPosition(10, windowSize.y - 80);
        restartButton.setPosition(windowSize.x - 220, windowSize.y - 90);
        restartText.setPosition(windowSize.x - 190, windowSize.y - 80);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (restartButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    resetBoard();
                }
                else if (mousePos.y >= gridOffsetY && mousePos.y <= gridOffsetY + cellSize * 3 &&
                    mousePos.x >= gridOffsetX && mousePos.x <= gridOffsetX + cellSize * 3) {
                    handleClick(mousePos, cellSize, window);
                }
            }
        }

        window.clear(sf::Color::White);

        // Draw responsive grid
        sf::RectangleShape line;
        line.setFillColor(sf::Color::Black);
        for (int i = 1; i < 3; i++) {
            line.setSize(sf::Vector2f(cellSize * 3, 5));
            line.setPosition(gridOffsetX, gridOffsetY + i * cellSize);
            window.draw(line);

            line.setSize(sf::Vector2f(5, cellSize * 3));
            line.setPosition(gridOffsetX + i * cellSize, gridOffsetY);
            window.draw(line);
        }

        drawShapes(window, cellSize, gridOffsetX, gridOffsetY);

        statusText.setString(gameOver ? winnerText : (string("Player ") + currentPlayer + "'s Turn"));
        statusText.setFillColor((currentPlayer == 'X') ? sf::Color::Red : sf::Color::Blue);
        window.draw(statusText);

        window.draw(restartButton);
        window.draw(restartText);

        window.display();
    }
    return 0;
}
