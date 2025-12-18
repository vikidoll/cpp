/*
 * Snake with second snake (bot) using std::thread
 * macOS + ncurses
 */

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <atomic>
#include <ncurses.h>

enum { LEFT = 1, UP, RIGHT, DOWN, STOP_GAME = 'q' };
enum { MAX_TAIL_SIZE = 1000, START_TAIL_SIZE = 3, SPEED = 120 };

struct Tail {
    int x, y;
};

class Snake {
public:
    int x, y, direction;
    size_t tsize;
    std::vector<Tail> tail;
    bool isBot;

    Snake(bool bot = false)
        : x(bot ? 10 : 2),
          y(bot ? 10 : 2),
          direction(RIGHT),
          tsize(START_TAIL_SIZE + 1),
          isBot(bot) {
        tail.resize(MAX_TAIL_SIZE);
    }

    void autoChangeDirection() {
        if (!isBot) return;
        int r = rand() % 4;
        direction = LEFT + r;
    }

    void move() {
        int max_x, max_y;
        getmaxyx(stdscr, max_y, max_x);

        mvprintw(y, x, " ");

        switch (direction) {
            case LEFT:  x = (x <= 0) ? max_x - 1 : x - 1; break;
            case RIGHT: x = (x >= max_x - 1) ? 0 : x + 1; break;
            case UP:    y = (y <= 1) ? max_y - 1 : y - 1; break;
            case DOWN:  y = (y >= max_y - 1) ? 1 : y + 1; break;
        }

        mvprintw(y, x, isBot ? "B" : "@");
    }

    void moveTail() {
        mvprintw(tail[tsize - 1].y, tail[tsize - 1].x, " ");
        for (size_t i = tsize - 1; i > 0; --i) {
            tail[i] = tail[i - 1];
            mvprintw(tail[i].y, tail[i].x, "*");
        }
        tail[0] = { x, y };
    }

    bool crashWith(const Snake& other) const {
        for (size_t i = 0; i < other.tsize; ++i) {
            if (x == other.tail[i].x && y == other.tail[i].y)
                return true;
        }
        return false;
    }
};

void changeDirection(int& dir, int key) {
    switch (key) {
        case KEY_LEFT:  dir = LEFT; break;
        case KEY_RIGHT: dir = RIGHT; break;
        case KEY_UP:    dir = UP; break;
        case KEY_DOWN:  dir = DOWN; break;
    }
}

int main() {
    srand(time(nullptr));

    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(FALSE);
    timeout(0);

    mvprintw(0, 0, "Player: arrows | Bot: auto | q - quit");

    Snake player(false);
    Snake bot(true);

    std::mutex gameMutex;
    std::atomic<bool> running{true};

    std::thread botThread([&]() {
        while (running) {
            {
                std::lock_guard<std::mutex> lock(gameMutex);
                bot.autoChangeDirection();
                bot.move();
                bot.moveTail();

                if (bot.crashWith(player)) {
                    running = false;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(SPEED));
        }
    });

    int key = 0;
    while (running && key != STOP_GAME) {
        {
            std::lock_guard<std::mutex> lock(gameMutex);
            key = getch();
            changeDirection(player.direction, key);

            player.move();
            player.moveTail();

            if (player.crashWith(bot)) {
                running = false;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(SPEED));
    }

    running = false;
    if (botThread.joinable())
        botThread.join();

    clear();
    mvprintw(5, 5, "Game over");
    mvprintw(6, 5, "Player length: %zu", player.tsize);
    mvprintw(7, 5, "Bot length: %zu", bot.tsize);
    refresh();

    timeout(2000);
    getch();
    endwin();

    return 0;
}
