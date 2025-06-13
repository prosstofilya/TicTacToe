#include <windows.h>
#include <vector>
#include <random>
#include "game.h"

/**
 * @enum GameMode
 * @brief ����� ����: ��� ������ ��� ������ ����������.
 */
enum class GameMode {
    TwoPlayers,   ///< ���� �����
    VsComputer    ///< ���� ������ ����������
};

constexpr int CELL_SIZE = 100;  ///< ������ ������ �������� ���� (100x100 ��������)

GameMode currentMode = GameMode::TwoPlayers; ///< ������� ����� ����
Game game;                                  ///< ������ ����
bool currentPlayer = true;                   ///< ������� �����: true - �������� (X), false - ������ (O)
bool gameOver = false;                       ///< ���� ��������� ����

/**
 * @brief ������������ ������� ���� � ������� ������� (�������� � ������).
 * @param hdc �������� ���������� ��� ��������� (HDC)
 */
void drawBoard(HDC hdc) {
    // ������ ����� ����� �������� ����
    for (int i = 1; i < Game::BOARD_SIZE; ++i) {
        MoveToEx(hdc, 0, i * CELL_SIZE, nullptr);
        LineTo(hdc, Game::BOARD_SIZE * CELL_SIZE, i * CELL_SIZE);
        MoveToEx(hdc, i * CELL_SIZE, 0, nullptr);
        LineTo(hdc, i * CELL_SIZE, Game::BOARD_SIZE * CELL_SIZE);
    }

    // �������� ������� ��������� ����� � ������ �������� � ������
    const auto& board = game.getBoard();
    for (int y = 0; y < Game::BOARD_SIZE; ++y) {
        for (int x = 0; x < Game::BOARD_SIZE; ++x) {
            int centerX = x * CELL_SIZE + CELL_SIZE / 2;
            int centerY = y * CELL_SIZE + CELL_SIZE / 2;
            if (board[y][x] == Game::Cell::X) {
                // ������ �������
                MoveToEx(hdc, centerX - 30, centerY - 30, nullptr);
                LineTo(hdc, centerX + 30, centerY + 30);
                MoveToEx(hdc, centerX + 30, centerY - 30, nullptr);
                LineTo(hdc, centerX - 30, centerY + 30);
            }
            else if (board[y][x] == Game::Cell::O) {
                // ������ �����
                Ellipse(hdc, centerX - 30, centerY - 30, centerX + 30, centerY + 30);
            }
        }
    }
}

/**
 * @brief ������ ���� ���������� � ������ "������ ����������".
 * ��������� ������ ��� �� �����������: �����, ����, �������.
 * @param hwnd ���������� ���� ��� ������ ��������� � ����������
 */
void computerMove(HWND hwnd) {
    if (gameOver) return;

    const auto& board = game.getBoard();

    // ��������� 1: ������ �����, ���� �� ��������
    if (board[1][1] == Game::Cell::Empty) {
        game.makeMove(1, 1, Game::Cell::O);
    }
    else {
        // ��������� 2: ������ ���� �� �����
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

        // ��������� 3: ������ ���� �� ������, ���� ���� ������
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

    // ��������� ���������� ����� ���� ����������
    auto winner = game.checkWinner();
    if (winner != Game::Cell::Empty) {
        gameOver = true;
        MessageBox(hwnd,
            (winner == Game::Cell::X ? L"�������� ��������!" : L"�������� ������!"),
            L"���� ��������",
            MB_OK);
    }
    else if (game.isDraw()) {
        gameOver = true;
        MessageBox(hwnd, L"�����!", L"���� ��������", MB_OK);
    }

    currentPlayer = true;  // �������� ���� ������
    InvalidateRect(hwnd, nullptr, TRUE);  // ����������� ����
}

/**
 * @brief �������� ������� ��������� ��������� ��������� Windows.
 * ������������ ����� ����, ����������� ���� � ��������.
 */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_LBUTTONDOWN: {
        if (gameOver) {
            // ���� ���� �������� � ��������, ���������� ����
            game.reset();
            gameOver = false;
            currentPlayer = true;
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }

        // �������� ���������� ������, �� ������� ��������
        int x = LOWORD(lParam) / CELL_SIZE;
        int y = HIWORD(lParam) / CELL_SIZE;

        if (x >= 0 && x < Game::BOARD_SIZE && y >= 0 && y < Game::BOARD_SIZE) {
            if (currentMode == GameMode::VsComputer) {
                // ��� ������ � ������ ������ ����������
                if (currentPlayer && game.makeMove(y, x, Game::Cell::X)) {
                    auto winner = game.checkWinner();
                    if (winner != Game::Cell::Empty) {
                        gameOver = true;
                        MessageBox(hwnd,
                            (winner == Game::Cell::X ? L"�������� ��������!" : L"�������� ������!"),
                            L"���� ��������",
                            MB_OK);
                    }
                    else if (game.isDraw()) {
                        gameOver = true;
                        MessageBox(hwnd, L"�����!", L"���� ��������", MB_OK);
                    }
                    else {
                        currentPlayer = false; // ������ ��� ����������
                        computerMove(hwnd);
                    }
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
            }
            else { // ����� ��� ������
                if (game.makeMove(y, x, currentPlayer ? Game::Cell::X : Game::Cell::O)) {
                    auto winner = game.checkWinner();
                    if (winner != Game::Cell::Empty) {
                        gameOver = true;
                        MessageBox(hwnd,
                            (winner == Game::Cell::X ? L"�������� ��������!" : L"�������� ������!"),
                            L"���� ��������",
                            MB_OK);
                    }
                    else if (game.isDraw()) {
                        gameOver = true;
                        MessageBox(hwnd, L"�����!", L"���� ��������", MB_OK);
                    }
                    currentPlayer = !currentPlayer;  // ����� ������
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
            }
        }
        return 0;
    }
    case WM_PAINT: {
        // ��������� ����������� ����
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        drawBoard(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        // ���������� ���������� ��� �������� ����
        PostQuitMessage(0);
        return 0;
    }
    // ��������� ��������� ��������� ����������� ����������
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/**
 * @brief ����� ����� � ��������� � ������������� ���� � ������ �������� �����.
 * @param hInstance ���������� �������� ���������� ����������
 * @param hPrevInstance ���������������, ������ nullptr
 * @param lpCmdLine ��������� ������
 * @param nCmdShow ���� ������ ����
 * @return ��� ���������� ���������
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"TicTacToeClass";

    // ����������� ������ ����
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // �������� ����
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"��������-������",
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

    // ����� ������ ���� ����� MessageBox
    int result = MessageBox(hwnd,
        L"�������� ����� ����:\n�� � ��� ������\n��� � ������ ����������",
        L"����� ������",
        MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON1);

    if (result == IDYES) {
        currentMode = GameMode::TwoPlayers;
    }
    else {
        currentMode = GameMode::VsComputer;
    }

    // �������� ���� ��������� ��������� Windows
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
