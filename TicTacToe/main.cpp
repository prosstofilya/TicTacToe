#include <windows.h>
#include <vector>
#include <random>
#include "game.h"

enum class GameMode {
    TwoPlayers,
    VsComputer
};

constexpr int CELL_SIZE = 100;
bool playerIsX = true; // ����� � �������� ��� ������ (��� ���� � �����������)
bool computerIsX = false; // ������������� �������������� ������

void endGame(HWND hwnd, Game::Cell winner);
bool AskResetStatistics(HWND hwnd);
void ShowPlayerChoiceDialog(HWND hwnd);

GameMode currentMode = GameMode::TwoPlayers;
Game game;

bool currentPlayer = true; // true � ��������, false � ������; � ���� � ������ � ����� ���� � �����, � ����
bool gameOver = false;

int winsX = 0;
int winsO = 0;
int draws = 0;

constexpr int OFFSET_Y = 30;

void drawBoard(HDC hdc) {
    wchar_t stats[200];

    // ���������� � ������������ ��� �������� � ��� ������
    swprintf_s(stats, L"X: %d   O: %d   �����: %d", winsX, winsO, draws);

    TextOut(hdc, 10, 10, stats, wcslen(stats));

    // ���������� ������� ��� � ��������� � ������ � �����������
    const wchar_t* turnText = nullptr;
    if (gameOver) {
        turnText = L"���� ��������";
    }
    else {
        if (currentMode == GameMode::VsComputer) {
            if ((currentPlayer == playerIsX)) {
                turnText = L"��� ���";
            }
            else {
                turnText = L"��� ����������...";
            }
        }
        else {
            // ��� ������
            if (currentPlayer) {
                turnText = L"������ ����� ����� (X)";
            }
            else {
                turnText = L"������ ����� ����� (O)";
            }
        }
    }
    TextOut(hdc, 10, 30, turnText, wcslen(turnText));

    int offset = 50;

    for (int i = 1; i < Game::BOARD_SIZE; ++i) {
        MoveToEx(hdc, 0, offset + i * CELL_SIZE, nullptr);
        LineTo(hdc, Game::BOARD_SIZE * CELL_SIZE, offset + i * CELL_SIZE);
        MoveToEx(hdc, i * CELL_SIZE, offset, nullptr);
        LineTo(hdc, i * CELL_SIZE, offset + Game::BOARD_SIZE * CELL_SIZE);
    }

    const auto& board = game.getBoard();
    for (int y = 0; y < Game::BOARD_SIZE; ++y) {
        for (int x = 0; x < Game::BOARD_SIZE; ++x) {
            int centerX = x * CELL_SIZE + CELL_SIZE / 2;
            int centerY = offset + y * CELL_SIZE + CELL_SIZE / 2;

            if (board[y][x] == Game::Cell::X) {
                MoveToEx(hdc, centerX - 30, centerY - 30, nullptr);
                LineTo(hdc, centerX + 30, centerY + 30);
                MoveToEx(hdc, centerX + 30, centerY - 30, nullptr);
                LineTo(hdc, centerX - 30, centerY + 30);
            }
            else if (board[y][x] == Game::Cell::O) {
                Ellipse(hdc, centerX - 30, centerY - 30, centerX + 30, centerY + 30);
            }
        }
    }
}

void computerMove(HWND hwnd) {
    if (gameOver) return;

    // ��������� ������ ����� ����� ��������
    Game::Cell compCell = computerIsX ? Game::Cell::X : Game::Cell::O;

    const auto& board = game.getBoard();

    // ������� ��: �����, ����� ����, ����� �������

    if (board[1][1] == Game::Cell::Empty) {
        game.makeMove(1, 1, compCell);
    }
    else {
        std::vector<std::pair<int, int>> corners = {
            {0, 0}, {0, 2}, {2, 0}, {2, 2}
        };
        bool moved = false;
        for (const auto& [y, x] : corners) {
            if (board[y][x] == Game::Cell::Empty) {
                game.makeMove(y, x, compCell);
                moved = true;
                break;
            }
        }
        if (!moved) {
            std::vector<std::pair<int, int>> sides = {
                {0, 1}, {1, 0}, {1, 2}, {2, 1}
            };
            for (const auto& [y, x] : sides) {
                if (board[y][x] == Game::Cell::Empty) {
                    game.makeMove(y, x, compCell);
                    break;
                }
            }
        }
    }

    auto winner = game.checkWinner();
    if (winner != Game::Cell::Empty) {
        endGame(hwnd, winner);
        return;
    }
    else if (game.isDraw()) {
        endGame(hwnd, Game::Cell::Empty);
        return;
    }

    currentPlayer = playerIsX;  // ��� ������������ ������
    RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
    InvalidateRect(hwnd, nullptr, TRUE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_LBUTTONDOWN: {
        if (gameOver) {
            game.reset();
            gameOver = false;
            currentPlayer = playerIsX; // ��� ������ ���� ����� �����
            RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }

        int x = LOWORD(lParam) / CELL_SIZE;
        int y = (HIWORD(lParam) - OFFSET_Y) / CELL_SIZE;

        if (x >= 0 && x < Game::BOARD_SIZE && y >= 0 && y < Game::BOARD_SIZE) {
            if (currentMode == GameMode::VsComputer) {
                if (currentPlayer == playerIsX && game.makeMove(y, x, playerIsX ? Game::Cell::X : Game::Cell::O)) {
                    auto winner = game.checkWinner();
                    if (winner != Game::Cell::Empty) {
                        endGame(hwnd, winner);
                        return 0;
                    }
                    else if (game.isDraw()) {
                        endGame(hwnd, Game::Cell::Empty);
                        return 0;
                    }
                    else {
                        currentPlayer = computerIsX; // ��� ����������
                        computerMove(hwnd);          // ����� ����� ��� ��������
                    }
                    RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
            }
            else {
                if (game.makeMove(y, x, currentPlayer ? Game::Cell::X : Game::Cell::O)) {
                    auto winner = game.checkWinner();
                    if (winner != Game::Cell::Empty) {
                        endGame(hwnd, winner);
                        return 0;
                    }
                    else if (game.isDraw()) {
                        endGame(hwnd, Game::Cell::Empty);
                        return 0;
                    }
                    currentPlayer = !currentPlayer;
                    RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
            }
        }
        return 0;
    }

    case WM_RBUTTONDOWN: {
        winsX = winsO = draws = 0;
        int result = MessageBox(hwnd,
            L"�������� ����� ����:\n�� � ��� ������\n��� � ������ ����������",
            L"����� ������", MB_YESNO | MB_ICONQUESTION);
        currentMode = (result == IDYES) ? GameMode::TwoPlayers : GameMode::VsComputer;

        if (currentMode == GameMode::TwoPlayers) {
            // ��� ���� �������: �������� ����� ������� �� ���������
            currentPlayer = true;
        }
        else {
            // ��� ���� � ����������� �������� ������ ������ ������ � ��� ����� ������
            ShowPlayerChoiceDialog(hwnd);
        }

        game.reset();
        gameOver = false;
        RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
        InvalidateRect(hwnd, nullptr, TRUE);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));  // ������� ����

        drawBoard(hdc);  // ������ ������� ����

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"TicTacToeClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    RECT rect = { 0, 0, CELL_SIZE * Game::BOARD_SIZE, CELL_SIZE * Game::BOARD_SIZE + OFFSET_Y + 10 };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"��������-������",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    int result = MessageBox(hwnd,
        L"�������� �����:\n�� � ��� ������\n��� � ������ ����������",
        L"����� ����", MB_YESNO | MB_ICONQUESTION);

    currentMode = (result == IDYES) ? GameMode::TwoPlayers : GameMode::VsComputer;

    ShowPlayerChoiceDialog(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void endGame(HWND hwnd, Game::Cell winner) {
    gameOver = true;

    if (winner == Game::Cell::X) ++winsX;
    else if (winner == Game::Cell::O) ++winsO;
    else ++draws;

    const wchar_t* message = (winner == Game::Cell::Empty) ? L"�����!" :
        (winner == Game::Cell::X ? L"�������� ��������!" : L"�������� ������!");

    MessageBox(hwnd, message, L"���� ��������", MB_OK);

    int newGame = MessageBox(hwnd,
        L"������ ����� ����?\n�� � ������� �����\n��� � �����",
        L"����� ����", MB_YESNO | MB_ICONQUESTION);

    if (newGame == IDYES) {
        // ��������� ����� ����, ��� ��� ���
        int result = MessageBox(hwnd,
            L"�������� �����:\n�� � ��� ������\n��� � ������ ����������",
            L"����� ����", MB_YESNO | MB_ICONQUESTION);

        currentMode = (result == IDYES) ? GameMode::TwoPlayers : GameMode::VsComputer;

        if (AskResetStatistics(hwnd)) {
            winsX = 0;
            winsO = 0;
            draws = 0;
        }

        ShowPlayerChoiceDialog(hwnd);
    }
    else {
        PostQuitMessage(0);
    }
}

void ShowPlayerChoiceDialog(HWND hwnd) {
    if (currentMode == GameMode::TwoPlayers) {
        // ����� 1 �������� ������
        int result1 = MessageBox(hwnd,
            L"����� 1: ��������, ��� ������\n"
            L"�� � �������� (X)\n"
            L"��� � ������ (O)",
            L"����� ������ 1", MB_YESNO | MB_ICONQUESTION);

        playerIsX = (result1 == IDYES);
        computerIsX = !playerIsX; // �� ������������, �� ����� ����� ��� �������

        wchar_t msg[256];
        swprintf_s(msg, L"����� 1 ������ %s\n����� 2 ������ %s",
            playerIsX ? L"�������� (X)" : L"������ (O)",
            playerIsX ? L"������ (O)" : L"�������� (X)");

        MessageBox(hwnd, msg, L"������������� ��������", MB_OK | MB_ICONINFORMATION);

        currentPlayer = true; // �������� ����� �������
    }
    else {
        // ���� ������ ���������� � ����� �������� ������
        int whoFirst = MessageBox(hwnd,
            L"�������� ���� ������:\n"
            L"�� � �������� (X)\n"
            L"��� � ������ (O)",
            L"����� ������", MB_YESNO | MB_ICONQUESTION);

        playerIsX = (whoFirst == IDYES);
        computerIsX = !playerIsX;

        // ����� ���, ��� ������ ����������
        currentPlayer = true; // �������� ��������

        wchar_t msg[256];
        swprintf_s(msg, L"�� ������� %s\n��������� ������ %s",
            playerIsX ? L"�������� (X)" : L"������ (O)",
            computerIsX ? L"�������� (X)" : L"������ (O)");

        MessageBox(hwnd, msg, L"������������� ��������", MB_OK | MB_ICONINFORMATION);
    }

    // ����� ���� � ���������
    game.reset();
    gameOver = false;

    RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
    InvalidateRect(hwnd, nullptr, TRUE);

    // ���� ����� ������ ��� ���������� (�� ������ ����������), �� ����� �����
    if (currentMode == GameMode::VsComputer && currentPlayer == computerIsX) {
        computerMove(hwnd);
    }
}


bool AskResetStatistics(HWND hwnd)
{
    int result = MessageBox(hwnd, L"�������� ����������?", L"����������", MB_YESNO | MB_ICONQUESTION);
    return (result == IDYES);
}
