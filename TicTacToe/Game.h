#pragma once
#include <array>

/**
 * @brief Класс, реализующий игровую логику крестиков-ноликов.
 */
class Game {
public:
    /**
     * @brief Размер игрового поля (3x3).
     */
    static const int BOARD_SIZE = 3;

    /**
     * @brief Перечисление возможных состояний клетки.
     */
    enum Cell {
        Empty,
        X,
        O
    };

    /**
     * @brief Конструктор. Инициализирует пустое поле.
     */
    Game();

    /**
     * @brief Сделать ход игрока в указанную ячейку.
     * @param y Строка
     * @param x Столбец
     * @param player Символ игрока (X или O)
     * @return true, если ход был сделан; false — если ячейка занята или координаты некорректны
     */
    bool makeMove(int y, int x, Cell player);

    /**
     * @brief Проверка победителя.
     * @return Символ победившего игрока (X или O), либо Empty если победителя нет
     */
    Cell checkWinner() const;

    /**
     * @brief Проверка на ничью.
     * @return true, если ничья; false — если игра не окончена или есть победитель
     */
    bool isDraw() const;

    /**
     * @brief Сброс игры. Очищает игровое поле.
     */
    void reset();

    /**
     * @brief Получить текущее состояние игрового поля.
     * @return Ссылка на двумерный массив клеток
     */
    const std::array<std::array<Cell, BOARD_SIZE>, BOARD_SIZE>& getBoard() const;

private:
    std::array<std::array<Cell, BOARD_SIZE>, BOARD_SIZE> board_;
};