#include "SFML/Graphics.hpp"
#include <vector>
#include <cstdlib> // Для rand()
#include <ctime>   // Для time()

// Константы
const int WIDTH = 800;
const int HEIGHT = 600;
const int SIZE = 20;
const int NUM_X = WIDTH / SIZE;
const int NUM_Y = HEIGHT / SIZE;
const float MOVE_INTERVAL = 0.1f; // Интервал обновления в секундах (чем больше, тем медленнее змейка)

enum Direction { UP, DOWN, LEFT, RIGHT };

struct SnakeSegment {
    int x, y;
    SnakeSegment(int x, int y) : x(x), y(y) {}
};

class Snake {
public:
    Snake() {
        direction = RIGHT;
        segments.push_back(SnakeSegment(NUM_X / 2, NUM_Y / 2));
    }

    void move() {
        // Сохранение предыдущего положения сегментов
        for (int i = segments.size() - 1; i > 0; --i) {
            segments[i].x = segments[i - 1].x;
            segments[i].y = segments[i - 1].y;
        }

        // Перемещение головы
        if (direction == UP) segments[0].y -= 1;
        if (direction == DOWN) segments[0].y += 1;
        if (direction == LEFT) segments[0].x -= 1;
        if (direction == RIGHT) segments[0].x += 1;

        // Проверка выхода за границы
        if (segments[0].x < 0) segments[0].x = NUM_X - 1;
        if (segments[0].x >= NUM_X) segments[0].x = 0;
        if (segments[0].y < 0) segments[0].y = NUM_Y - 1;
        if (segments[0].y >= NUM_Y) segments[0].y = 0;
    }

    void grow() {
        segments.push_back(SnakeSegment(segments.back().x, segments.back().y));
    }

    void changeDirection(Direction newDirection) {
        // Избегание обратного направления
        if ((direction == UP && newDirection != DOWN) ||
            (direction == DOWN && newDirection != UP) ||
            (direction == LEFT && newDirection != RIGHT) ||
            (direction == RIGHT && newDirection != LEFT)) {
            direction = newDirection;
        }
    }

    std::vector<SnakeSegment> getSegments() const {
        return segments;
    }

private:
    std::vector<SnakeSegment> segments;
    Direction direction;
};

class Game {
public:
    Game() : window(sf::VideoMode(WIDTH, HEIGHT), "Snake Game"), snake(), clock() {
        food.setPosition(rand() % NUM_X * SIZE, rand() % NUM_Y * SIZE);
    }

    void run() {
        sf::Time timeSinceLastMove = sf::Time::Zero;

        while (window.isOpen()) {
            sf::Time deltaTime = clock.restart();
            timeSinceLastMove += deltaTime;

            handleEvents();
            if (timeSinceLastMove.asSeconds() >= MOVE_INTERVAL) {
                timeSinceLastMove = sf::Time::Zero;
                update();
            }
            render();
        }
    }

private:
    sf::RenderWindow window;
    Snake snake;
    sf::RectangleShape food;
    sf::Clock clock;

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) snake.changeDirection(UP);
                if (event.key.code == sf::Keyboard::S) snake.changeDirection(DOWN);
                if (event.key.code == sf::Keyboard::A) snake.changeDirection(LEFT);
                if (event.key.code == sf::Keyboard::D) snake.changeDirection(RIGHT);
            }
        }
    }

    void update() {
        snake.move();

        // Проверка столкновения с едой
        if (snake.getSegments().front().x * SIZE == food.getPosition().x &&
            snake.getSegments().front().y * SIZE == food.getPosition().y) {
            snake.grow();
            food.setPosition(rand() % NUM_X * SIZE, rand() % NUM_Y * SIZE);
        }
    }

    void render() {
        window.clear();

        // Рисуем змею
        for (const auto& segment : snake.getSegments()) {
            sf::RectangleShape rectangle(sf::Vector2f(SIZE, SIZE));
            rectangle.setPosition(segment.x * SIZE, segment.y * SIZE);
            window.draw(rectangle);
        }

        // Рисуем еду
        food.setSize(sf::Vector2f(SIZE, SIZE));
        food.setFillColor(sf::Color::Red);
        window.draw(food);

        window.display();
    }
};

int main() {
    srand(static_cast<unsigned>(time(0))); // Инициализация генератора случайных чисел
    Game game;
    game.run();
    return 0;
}
