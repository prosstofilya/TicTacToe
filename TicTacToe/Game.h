#pragma once
#include <array>

/**
 * @brief �����, ����������� ������� ������ ���������-�������.
 */
class Game {
public:
    /**
     * @brief ������ �������� ���� (3x3).
     */
    static const int BOARD_SIZE = 3;

    /**
     * @brief ������������ ��������� ��������� ������.
     */
    enum Cell {
        Empty,
        X,
        O
    };

    /**
     * @brief �����������. �������������� ������ ����.
     */
    Game();

    /**
     * @brief ������� ��� ������ � ��������� ������.
     * @param y ������
     * @param x �������
     * @param player ������ ������ (X ��� O)
     * @return true, ���� ��� ��� ������; false � ���� ������ ������ ��� ���������� �����������
     */
    bool makeMove(int y, int x, Cell player);

    /**
     * @brief �������� ����������.
     * @return ������ ����������� ������ (X ��� O), ���� Empty ���� ���������� ���
     */
    Cell checkWinner() const;

    /**
     * @brief �������� �� �����.
     * @return true, ���� �����; false � ���� ���� �� �������� ��� ���� ����������
     */
    bool isDraw() const;

    /**
     * @brief ����� ����. ������� ������� ����.
     */
    void reset();

    /**
     * @brief �������� ������� ��������� �������� ����.
     * @return ������ �� ��������� ������ ������
     */
    const std::array<std::array<Cell, BOARD_SIZE>, BOARD_SIZE>& getBoard() const;

private:
    std::array<std::array<Cell, BOARD_SIZE>, BOARD_SIZE> board_;
};