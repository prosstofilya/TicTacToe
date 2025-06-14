/**
 * @file Game.h
 * @brief Реализация логики игры "Крестики-нолики".
 */

#include "Game.h"

 /**
  * @brief Конструктор класса Game. Вызывает метод reset() для очистки игрового поля.
  */
Game::Game() {
    reset();
}

/**
 * @brief Делает ход игрока на поле.
 *
 * @param y Координата строки (0–2).
 * @param x Координата столбца (0–2).
 * @param player Символ игрока (X или O).
 * @return true, если ход успешен (ячейка была пуста и в пределах поля).
 * @return false, если координаты вне поля или ячейка занята.
 */
bool Game::makeMove(int y, int x, Cell player) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
        return false;
    if (board_[y][x] != Empty)
        return false;
    board_[y][x] = player;
    return true;
}

/**
 * @brief Проверяет наличие победителя.
 *
 * @return Символ победителя (X или O), либо Empty, если победителя нет.
 */
Game::Cell Game::checkWinner() const {
    // Проверка строк и столбцов
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (board_[i][0] != Empty &&
            board_[i][0] == board_[i][1] && board_[i][1] == board_[i][2])
            return board_[i][0];
        if (board_[0][i] != Empty &&
            board_[0][i] == board_[1][i] && board_[1][i] == board_[2][i])
            return board_[0][i];
    }
    // Проверка диагоналей
    if (board_[0][0] != Empty &&
        board_[0][0] == board_[1][1] && board_[1][1] == board_[2][2])
        return board_[0][0];
    if (board_[0][2] != Empty &&
        board_[0][2] == board_[1][1] && board_[1][1] == board_[2][0])
        return board_[0][2];

    return Empty;
}

/**
 * @brief Проверяет, является ли игра ничьей.
 *
 * @return true, если всё поле заполнено и победителя нет.
 * @return false, если ещё есть ходы или есть победитель.
 */
bool Game::isDraw() const {
    if (checkWinner() != Empty) return false;
    for (const auto& row : board_)
        for (Cell c : row)
            if (c == Empty)
                return false;
    return true;
}

/**
 * @brief Сбрасывает игровое поле, делая все ячейки пустыми.
 */
void Game::reset() {
    for (auto& row : board_)
        for (auto& cell : row)
            cell = Empty;
}

/**
 * @brief Возвращает текущее состояние игрового поля.
 *
 * @return Константная ссылка на 2D-массив игрового поля.
 */
const std::array<std::array<Game::Cell, Game::BOARD_SIZE>, Game::BOARD_SIZE>& Game::getBoard() const {
    return board_;
}
