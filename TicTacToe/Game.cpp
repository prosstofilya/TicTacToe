#include "Game.h"

Game::Game() {
    reset();
}

bool Game::makeMove(int y, int x, Cell player) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
        return false;
    if (board_[y][x] != Empty)
        return false;
    board_[y][x] = player;
    return true;
}

Game::Cell Game::checkWinner() const {
    // Горизонтали и вертикали
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (board_[i][0] != Empty &&
            board_[i][0] == board_[i][1] && board_[i][1] == board_[i][2])
            return board_[i][0];
        if (board_[0][i] != Empty &&
            board_[0][i] == board_[1][i] && board_[1][i] == board_[2][i])
            return board_[0][i];
    }
    // Диагонали
    if (board_[0][0] != Empty &&
        board_[0][0] == board_[1][1] && board_[1][1] == board_[2][2])
        return board_[0][0];
    if (board_[0][2] != Empty &&
        board_[0][2] == board_[1][1] && board_[1][1] == board_[2][0])
        return board_[0][2];

    return Empty;
}

bool Game::isDraw() const {
    if (checkWinner() != Empty) return false;
    for (const auto& row : board_)
        for (Cell c : row)
            if (c == Empty)
                return false;
    return true;
}

void Game::reset() {
    for (auto& row : board_)
        for (auto& cell : row)
            cell = Empty;
}

const std::array<std::array<Game::Cell, Game::BOARD_SIZE>, Game::BOARD_SIZE>& Game::getBoard() const {
    return board_;
}
