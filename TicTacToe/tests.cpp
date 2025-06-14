/**
 * @file test_game.cpp
 * @brief ����� ��� ������ Game (��������-������).
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Game.h"

 /**
  * @brief ��������� ������� makeMove � ��������� �������� ����.
  */
TEST_CASE("Test makeMove and board state") {
    Game game;
    CHECK(game.makeMove(0, 0, Game::Cell::X) == true);      /**< ��� � ������ ������ */
    CHECK(game.makeMove(0, 0, Game::Cell::O) == false);     /**< ��� � ��� ������� ���� */
    CHECK(game.makeMove(3, 3, Game::Cell::X) == false);     /**< ��� ��� �������� ���� */
}

/**
 * @brief ��������� ����������� ����������.
 */
TEST_CASE("Test winner detection") {
    Game game;

    // �������������� ����� X
    game.makeMove(0, 0, Game::Cell::X);
    game.makeMove(1, 0, Game::Cell::X);
    game.makeMove(2, 0, Game::Cell::X);
    CHECK(game.checkWinner() == Game::Cell::X);

    game.reset();

    // ��������� O
    game.makeMove(0, 0, Game::Cell::O);
    game.makeMove(1, 1, Game::Cell::O);
    game.makeMove(2, 2, Game::Cell::O);
    CHECK(game.checkWinner() == Game::Cell::O);
}

/**
 * @brief ��������� �������� ������.
 */
TEST_CASE("Test draw") {
    Game game;
    game.makeMove(0, 0, Game::Cell::X);
    game.makeMove(0, 1, Game::Cell::O);
    game.makeMove(0, 2, Game::Cell::X);
    game.makeMove(1, 0, Game::Cell::X);
    game.makeMove(1, 1, Game::Cell::O);
    game.makeMove(1, 2, Game::Cell::O);
    game.makeMove(2, 0, Game::Cell::O);
    game.makeMove(2, 1, Game::Cell::X);
    game.makeMove(2, 2, Game::Cell::X);
    CHECK(game.isDraw() == true);
    CHECK(game.checkWinner() == Game::Cell::Empty);
}
