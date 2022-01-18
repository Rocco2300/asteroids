#include <iostream>
#include <ctime>
#include <random>
#include "src/include/Game.h"
#include "src/include/TextureLoader.h"

#define DEBUG false

Game::Game()
{ 
    srand(time(NULL));
    createWindow("Asteroids", 60);
    TextureLoader::getInstance()->loadTextures();

    shipHpTexture = TextureLoader::getInstance()->getShipTexture();
    shipHpSprite.setTexture(*shipHpTexture);
    shipHpSprite.setTextureRect({0, 0, 32, 32});
    shipHpSprite.setOrigin({16.f, 16.f});
    shipHpSprite.setScale({.75f, .75f});
    
    if(!font.loadFromFile("fonts/ARCADECLASSIC.TTF"))
        std::cerr << "Error loading font!" << std::endl;
    scoreText.setFont(font);
    scoreText.setCharacterSize(36);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({5.f, 0.f});

    ship = Ship(bullets);

    manager.init(ship, asteroids, bullets);
    manager.spawnAsteroids(5);
    scoreText.setString(std::to_string(manager.getScore()));
}

void Game::createWindow(std::string name, int frameLimit)
{
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), name);
    window.setFramerateLimit(frameLimit);
    window.setVerticalSyncEnabled(true);
}

void Game::update()
{
    GameState state;
    sf::Clock clock;
    // sf::Clock clk;
    while (window.isOpen())
    {
        sf::Time dt = clock.restart();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                TextureLoader::getInstance()->destroyInstance();
                window.close();
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::R)
            {
                reset();
                manager.reset();
                state = GameState::Running;
            }
        }

        state = manager.checkCollisions();
        if(state != GameOver)
        {
            ship.update(dt);
            updateEntities(asteroids, dt);
            updateEntities(bullets, dt);
            checkDespawnedBullets();
            scoreText.setString(std::to_string(manager.getScore()));
            if(state == GameState::GameOver && !gameOver)
            {
                gameOver = true;
                std::cout << "Game over!" << std::endl;
            }   
        }

        // sf::Time frameCntTime;
        // frameCntTime = clk.getElapsedTime();
        // if(frameCntTime.asSeconds() >= 1.f)
        // {
        //     std::cout << 1.f/dt.asSeconds() << std::endl;
        //     frameCntTime = clk.restart();
        // }
        draw();
    }
}

void Game::reset()
{
    ship = Ship(bullets);
    int randAng;
    ast::Vector2 randPos;
    ast::Vector2 randDir;
    Asteroid asteroid;
    asteroids.clear();
    bullets.clear();
    manager.spawnAsteroids(5);
}

template <typename T>
void Game::updateEntities(std::vector<T>& v, sf::Time dt)
{
    for(size_t i = 0; i < v.size(); i++)
    {
        v[i].update(dt);
    }
}

template <typename T>
void Game::drawEntities(std::vector<T>& v)
{
    bool ceva = false;
    for(size_t i = 0; i < v.size(); i++)
    {
        window.draw(v[i].getSprite());
    }
}

template <typename T>
void Game::drawDebugEntities(std::vector<T>& v)
{
    for(size_t i = 0; i < v.size(); i++)
    {
        window.draw(v[i].getDebugCircle());
    }
}

void Game::drawLives()
{
    for(int i = 0; i < ship.getLives(); i++)
    {
        shipHpSprite.setPosition({16.f + 32.f * i, 64.f});
        window.draw(shipHpSprite);
    }
}

void Game::checkDespawnedBullets()
{
    for(int i = 0; i < bullets.size(); i++)
    {
        if(bullets[i].isDespawned())
        {
            bullets.erase(bullets.begin() + i);
        }
    }
}

void Game::draw()
{
    window.clear();
    window.draw(ship.getSprite());
    drawEntities(asteroids);
    drawEntities(bullets);
    // sf::RectangleShape rect({250, 200});
    // rect.setOrigin({250/2, 200/2});
    // rect.setPosition({WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2});
    // rect.setOutlineColor(sf::Color::Red);
    // rect.setOutlineThickness(1);
    // rect.setFillColor(sf::Color::Transparent);
    // window.draw(rect);
    #if DEBUG 
    window.draw(ship.getDebugCircle());
    drawDebugEntities(asteroids);
    drawDebugEntities(bullets);
    #endif
    window.draw(scoreText);
    drawLives();
    window.display();
}