#include <windows.h>
#include <vector>
#include <random>
#include "game.h"

/**
 * @enum GameMode
 * @brief Режим игры: два игрока или против компьютера.
 */
enum class GameMode {
    TwoPlayers,   ///< Игра вдвоём
    VsComputer    ///< Игра против компьютера
};

constexpr int CELL_SIZE = 100;  ///< Размер клетки игрового поля (100x100 пикселей)

GameMode currentMode = GameMode::TwoPlayers; ///< Текущий режим игры
Game game;                                  ///< Объект игры
bool currentPlayer = true;                   ///< Текущий игрок: true - крестики (X), false - нолики (O)
bool gameOver = false;                       ///< Флаг окончания игры

/**
 * @brief Отрисовывает игровое поле и текущие символы (крестики и нолики).
 * @param hdc Контекст устройства для рисования (HDC)
 */
void drawBoard(HDC hdc) {
    // Рисуем линии сетки игрового поля
    for (int i = 1; i < Game::BOARD_SIZE; ++i) {
        MoveToEx(hdc, 0, i * CELL_SIZE, nullptr);
        LineTo(hdc, Game::BOARD_SIZE * CELL_SIZE, i * CELL_SIZE);
        MoveToEx(hdc, i * CELL_SIZE, 0, nullptr);
        LineTo(hdc, i * CELL_SIZE, Game::BOARD_SIZE * CELL_SIZE);
    }

    // Получаем текущее состояние доски и рисуем крестики и нолики
    const auto& board = game.getBoard();
    for (int y = 0; y < Game::BOARD_SIZE; ++y) {
        for (int x = 0; x < Game::BOARD_SIZE; ++x) {
            int centerX = x * CELL_SIZE + CELL_SIZE / 2;
            int centerY = y * CELL_SIZE + CELL_SIZE / 2;
            if (board[y][x] == Game::Cell::X) {
                // Рисуем крестик
                MoveToEx(hdc, centerX - 30, centerY - 30, nullptr);
                LineTo(hdc, centerX + 30, centerY + 30);
                MoveToEx(hdc, centerX + 30, centerY - 30, nullptr);
                LineTo(hdc, centerX - 30, centerY + 30);
            }
            else if (board[y][x] == Game::Cell::O) {
                // Рисуем нолик
                Ellipse(hdc, centerX - 30, centerY - 30, centerX + 30, centerY + 30);
            }
        }
    }
}

/**
 * @brief Логика хода компьютера в режиме "Против компьютера".
 * Компьютер делает ход по приоритетам: центр, углы, стороны.
 * @param hwnd Дескриптор окна для вызова сообщений и обновления
 */
void computerMove(HWND hwnd) {
    if (gameOver) return;

    const auto& board = game.getBoard();

    // Приоритет 1: занять центр, если он свободен
    if (board[1][1] == Game::Cell::Empty) {
        game.makeMove(1, 1, Game::Cell::O);
    }
    else {
        // Приоритет 2: занять один из углов
        std::vector<std::pair<int, int>> corners = {
            {0, 0}, {0, 2}, {2, 0}, {2, 2}
        };
        bool moved = false;
        for (const auto& [y, x] : corners) {
            if (board[y][x] == Game::Cell::Empty) {
                game.makeMove(y, x, Game::Cell::O);
                moved = true;
                break;
            }
        }

        // Приоритет 3: занять одну из сторон, если углы заняты
        if (!moved) {
            std::vector<std::pair<int, int>> sides = {
                {0, 1}, {1, 0}, {1, 2}, {2, 1}
            };
            for (const auto& [y, x] : sides) {
                if (board[y][x] == Game::Cell::Empty) {
                    game.makeMove(y, x, Game::Cell::O);
                    break;
                }
            }
        }
    }

    // Проверяем победителя после хода компьютера
    auto winner = game.checkWinner();
    if (winner != Game::Cell::Empty) {
        gameOver = true;
        MessageBox(hwnd,
            (winner == Game::Cell::X ? L"Победили крестики!" : L"Победили нолики!"),
            L"Игра окончена",
            MB_OK);
    }
    else if (game.isDraw()) {
        gameOver = true;
        MessageBox(hwnd, L"Ничья!", L"Игра окончена", MB_OK);
    }

    currentPlayer = true;  // передача хода игроку
    InvalidateRect(hwnd, nullptr, TRUE);  // перерисовка окна
}

/**
 * @brief Основная оконная процедура обработки сообщений Windows.
 * Обрабатывает клики мыши, перерисовку окна и закрытие.
 */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_LBUTTONDOWN: {
        if (gameOver) {
            // Если игра окончена и кликнули, сбрасываем игру
            game.reset();
            gameOver = false;
            currentPlayer = true;
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }

        // Получаем координаты клетки, по которой кликнули
        int x = LOWORD(lParam) / CELL_SIZE;
        int y = HIWORD(lParam) / CELL_SIZE;

        if (x >= 0 && x < Game::BOARD_SIZE && y >= 0 && y < Game::BOARD_SIZE) {
            if (currentMode == GameMode::VsComputer) {
                // Ход игрока в режиме против компьютера
                if (currentPlayer && game.makeMove(y, x, Game::Cell::X)) {
                    auto winner = game.checkWinner();
                    if (winner != Game::Cell::Empty) {
                        gameOver = true;
                        MessageBox(hwnd,
                            (winner == Game::Cell::X ? L"Победили крестики!" : L"Победили нолики!"),
                            L"Игра окончена",
                            MB_OK);
                    }
                    else if (game.isDraw()) {
                        gameOver = true;
                        MessageBox(hwnd, L"Ничья!", L"Игра окончена", MB_OK);
                    }
                    else {
                        currentPlayer = false; // теперь ход компьютера
                        computerMove(hwnd);
                    }
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
            }
            else { // Режим два игрока
                if (game.makeMove(y, x, currentPlayer ? Game::Cell::X : Game::Cell::O)) {
                    auto winner = game.checkWinner();
                    if (winner != Game::Cell::Empty) {
                        gameOver = true;
                        MessageBox(hwnd,
                            (winner == Game::Cell::X ? L"Победили крестики!" : L"Победили нолики!"),
                            L"Игра окончена",
                            MB_OK);
                    }
                    else if (game.isDraw()) {
                        gameOver = true;
                        MessageBox(hwnd, L"Ничья!", L"Игра окончена", MB_OK);
                    }
                    currentPlayer = !currentPlayer;  // смена игрока
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
            }
        }
        return 0;
    }
    case WM_PAINT: {
        // Обработка перерисовки окна
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        drawBoard(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        // Завершение приложения при закрытии окна
        PostQuitMessage(0);
        return 0;
    }
    // Обработка остальных сообщений стандартной процедурой
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/**
 * @brief Точка входа в программу — инициализация окна и начало игрового цикла.
 * @param hInstance Дескриптор текущего экземпляра приложения
 * @param hPrevInstance Зарезервировано, всегда nullptr
 * @param lpCmdLine Командная строка
 * @param nCmdShow Флаг показа окна
 * @return Код завершения программы
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"TicTacToeClass";

    // Регистрация класса окна
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // Создание окна
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Крестики-нолики",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CELL_SIZE * Game::BOARD_SIZE + 16,
        CELL_SIZE * Game::BOARD_SIZE + 39,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!hwnd)
        return 0;

    ShowWindow(hwnd, nCmdShow);

    // Выбор режима игры через MessageBox
    int result = MessageBox(hwnd,
        L"Выберите режим игры:\nДа — два игрока\nНет — против компьютера",
        L"Выбор режима",
        MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON1);

    if (result == IDYES) {
        currentMode = GameMode::TwoPlayers;
    }
    else {
        currentMode = GameMode::VsComputer;
    }

    // Основной цикл обработки сообщений Windows
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
