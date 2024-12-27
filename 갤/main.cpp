#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Explosion.h"
#include "Boss.h"
#include <windows.h>

using namespace sf;

Texture Enemy::texture1;

// �Լ� ����
void TitleLoop(RenderWindow& window);
void GameLoop(RenderWindow& window);
void GameOverScreen(RenderWindow& window);

// ���� �Լ�
int main() {
    RenderWindow window(VideoMode(800, 600), "Galaga", Style::Close);
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        TitleLoop(window); // Ÿ��Ʋ ȭ������ ����
        GameLoop(window);  // ���� �÷��� ���� ����
        GameOverScreen(window); // ���� ���� ȭ�� ����
    }

    return 0;
}

// Ÿ��Ʋ ����
void TitleLoop(RenderWindow& window) {
    Font font;
    font.loadFromFile("neodgm.ttf"); // ��Ʈ �ε�

    Text titleText("Galaga", font, 50);
    titleText.setPosition(200, 200);
    titleText.setFillColor(Color::White);

    Text startText("Press ENTER to Start", font, 30);
    startText.setPosition(220, 300);
    startText.setFillColor(Color::White);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                return; // ENTER Ű�� ������ Ÿ��Ʋ ���� ����
            }
        }

        window.clear();
        window.draw(titleText);
        window.draw(startText);
        window.display();
    }
}

Clock bossRespawnClock;
float bossRespawnDelay = 0.0f; // ���� ������ ��� �ð� 

// ���� ���� ����
bool bossDefeated = false;       // ���� óġ ����
int lastBossDefeatScore = 0;     // ������ óġ�� ������ ����

// ���� �÷��� ����
void GameLoop(RenderWindow& window) {
    Player player(400, 500);
    std::vector<Bullet> bullets;
    std::vector<Bullet> enemyBullets; // ���� �Ѿ�
    std::vector<Enemy> enemies;
    std::vector<Explosion> explosions;

    // ���� �ʱ�ȭ
    Boss boss(400, 50); // ȭ�� ��� �߾ӿ��� ����
    std::vector<Bullet> bossBullets; // ���� �Ѿ� ����
    bool bossActive = false; // ���� ���� ����

    Clock clock;

    Clock enemySpawnClock; // �� ���� Ÿ�̸�
    Clock enemyFireClock;  // �� �߻� Ÿ�̸�
    Clock bulletFireClock;  // �Ѿ� �߻� Ÿ�̸� �߰�

    float spawnInterval = 2.0f; // �� ���� ���� (��)
    float fireInterval = 1.5f;  // �� �߻� ���� (��)
    float bulletFireInterval = 0.2f;  // �Ѿ� �߻� ���� (0.2��)
    int score = 0;
    bool isGameOver = false;

    // �ؽ�Ʈ ����
    Font font;
    font.loadFromFile("neodgm.ttf");// ��Ʈ �ε� 

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, 10);

    Text healthText;
    healthText.setFont(font);
    healthText.setCharacterSize(24);
    healthText.setFillColor(Color::White);
    healthText.setPosition(10, 40);  // ���� �Ʒ��� ü�� ǥ��

    // ��� �̹��� �ε�
    Texture backgroundTexture;
    backgroundTexture.loadFromFile("space.png");

    // ��� ũ�� ���߱� (��� ũ�Ⱑ ȭ��� �ٸ� ���)
    backgroundTexture.setSmooth(true);  // �ε巴�� ó��
    Sprite backgroundSprite1(backgroundTexture);  // ù ��° ���
    Sprite backgroundSprite2(backgroundTexture);  // �� ��° ���

    // ��� ��ġ �ʱ�ȭ
    backgroundSprite1.setPosition(0, 0);
    backgroundSprite2.setPosition(0, -600);  // �� ��° ����� ù ��° ��� �ؿ� ��ġ

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        // ��� ��ũ�� ó��
        backgroundSprite1.move(0, 2);  // ����� �Ʒ��� �̵�
        backgroundSprite2.move(0, 2);

        // �� ����� ȭ���� ����� �ٽ� ���� �̵�
        if (backgroundSprite1.getPosition().y >= 600) {
            backgroundSprite1.setPosition(0, -600);  // ù ��° ����� �ٽ� ����
        }
        if (backgroundSprite2.getPosition().y >= 600) {
            backgroundSprite2.setPosition(0, -600);  // �� ��° ����� �ٽ� ����
        }

        // �Է� ó��
        if (Keyboard::isKeyPressed(Keyboard::Left)) player.moveLeft();
        if (Keyboard::isKeyPressed(Keyboard::Right)) player.moveRight();
        if (Keyboard::isKeyPressed(Keyboard::Up)) player.moveUp();
        if (Keyboard::isKeyPressed(Keyboard::Down)) player.moveDown();
        //if (Keyboard::isKeyPressed(Keyboard::W)) { score += 100; Sleep(1000); }

        if (Keyboard::isKeyPressed(Keyboard::Space) && bulletFireClock.getElapsedTime().asSeconds() > bulletFireInterval) {
            bullets.push_back(Bullet(
                (player.getPosition().x + player.getSize().x / 2) - 25, // X ��ġ: �÷��̾� �߽ɿ��� �Ѿ� ���� ���� ����
                player.getPosition().y - 10,                            // Y ��ġ: �÷��̾� ����
                true
            ));
            bulletFireClock.restart();  // Ÿ�̸� ����
        }

        // ���� ����

        // �Ѿ� ������Ʈ
        for (auto& bullet : bullets) bullet.update();

        // �� ������Ʈ
        for (auto& enemy : enemies) enemy.update();

        // �� �Ѿ� ������Ʈ
        for (auto& enemyBullet : enemyBullets) {
            if (enemyBullet.active) {
                enemyBullet.update(); // �̵� ó��
            }
        }

        // ��ƼŬ ������Ʈ
        for (auto& explosion : explosions) { explosion.update(); }

        // ���� �Ѿ� ������Ʈ
        for (auto& bullet : bossBullets) {
            if (bullet.active) {
                bullet.update();
                // �÷��̾�� �浹 üũ
                if (bullet.getGlobalBounds().intersects(player.getGlobalBounds())) {
                    bullet.active = false;
                    player.health--; // �÷��̾� ü�� ����
                    if (player.health <= 0) { isGameOver = true; }
                }
            }
        }

        // ���� ������ ���� ����
        if (score != 0 && score % 200 == 0 && !boss.isActive() && !bossDefeated) {
            boss.active = true;
            boss.health = 30;
            boss.setPosition(400, 50);
        }

        // ������ Ȱ��ȭ�� ���
        if (boss.isActive()) {
            boss.update();

            // ���� �Ѿ� �߻�
            static int bossShootTimer = 0;
            if (++bossShootTimer > 100) {
                boss.shoot(bossBullets);
                bossShootTimer = 0;
            }

            // ���� ü�� ���� ó��
            for (auto& bullet : bullets) {
                if (bullet.active && boss.getGlobalBounds().intersects(bullet.getGlobalBounds())) {
                    bullet.active = false;
                    boss.takeDamage();
                }
            }

            // ���� ü���� 0�� ��
            if (boss.getHealth() <= 0) {
                boss.active = false;
                bossDefeated = true; // ���� óġ ���·� ��ȯ
                score += 10; // óġ => score ����

                lastBossDefeatScore = score; // óġ ���� ���
                explosions.push_back(Explosion(boss.getPosition().x, boss.getPosition().y));
                boss.setPosition(-200, -200);
                bossRespawnClock.restart(); // ���� ������ Ÿ�̸� ����
            }
        }

        // ���� óġ �� ������ ���
        if (bossDefeated && bossRespawnClock.getElapsedTime().asSeconds() > 5.0f && score - lastBossDefeatScore >= 200) {
            bossDefeated = false;
        }

        // �� ���� ����
        if (enemySpawnClock.getElapsedTime().asSeconds() > spawnInterval && !boss.active) {
            float x = rand() % (800 - 40); // ȭ�� �ʺ�(800) - �� ũ��(40)
            enemies.push_back(Enemy(x, 0));
            enemySpawnClock.restart(); // Ÿ�̸� ����
        }

        // �� �߻� ����
        if (enemyFireClock.getElapsedTime().asSeconds() > fireInterval) {
            for (auto& enemy : enemies) {
                if (enemy.active) {
                    float playerX = player.getPosition().x + 40; // �÷��̾��� �߽� X
                    float playerY = player.getPosition().y + 10; // �÷��̾��� �߽� Y
                    float enemyX = enemy.getPosition().x + 30;  // ���� �߽� X
                    float enemyY = enemy.getPosition().y + 20;  // ���� �߽� Y

                    // �÷��̾�� ���� ��� ��ġ�� ������� ���� ���
                    float dx = playerX - enemyX;
                    float dy = playerY - enemyY;

                    // ���� ���� ����ȭ
                    float length = sqrt(dx * dx + dy * dy);
                    dx /= length;
                    dy /= length;

                    // ���� �Ѿ� �߻� (�÷��̾� ��������)
                    Bullet enemyBullet(enemyX, enemyY, false);
                    enemyBullet.setDirection(dx * 4.0f, dy * 4.0f); // ���⿡ ���� �ӵ� ����
                    enemyBullets.push_back(enemyBullet);
                }
            }
            enemyFireClock.restart();
        }

        // �浹 ó�� (�÷��̾� �Ѿ� -> ��)
        for (auto& bullet : bullets) {
            for (auto& enemy : enemies) {
                if (bullet.active && enemy.active && bullet.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
                    bullet.active = false;
                    enemy.active = false;
                    score += 10; // óġ => score ����

                    //��
                    explosions.push_back(Explosion(enemy.getPosition().x, enemy.getPosition().y));
                }
            }
        }

        // �浹 ���� (�� �� -> �÷��̾�)
        for (auto& enemyBullet : enemyBullets) {
            if (enemyBullet.active && enemyBullet.getGlobalBounds().intersects(player.getGlobalBounds())) {
                enemyBullet.active = false; // �� �Ѿ� ��Ȱ��ȭ
                player.takeDamage();        // �÷��̾� ü�� ����

                if (player.health <= 0) { isGameOver = true; }
            }
        }

        // �� ���� ��Ȱ��ȭ ó�� - ȭ�� ������ ������ ��Ȱ��ȭ
        for (auto& enemyBullet : enemyBullets) {
            if (enemyBullet.getPosition().y > 600) {
                enemyBullet.active = false;
            }
        }

        // ü�� ǥ��
        healthText.setString("Forces : " + to_string(player.health));

        // �ؽ�Ʈ ������Ʈ
        scoreText.setString("Score : " + to_string(score));

        if (isGameOver) return; // ���� ����

        

        // ������
        window.clear(Color::Black);

        window.draw(backgroundSprite1);
        window.draw(backgroundSprite2);

        window.draw(player);

        if (boss.isActive()) {
            window.draw(boss);
        }
        
        for (auto& bullet : bullets) {
            if (bullet.active) window.draw(bullet);
        }
        for (auto& enemy : enemies) {
            if (enemy.active) window.draw(enemy);
        }
        for (auto& enemyBullet : enemyBullets) {
            if (enemyBullet.active) window.draw(enemyBullet);
        }
        for (auto& bossBullet : bossBullets) {
            if (bossBullet.active) window.draw(bossBullet);
        }
        
        for (auto& explosion : explosions) {
            explosion.draw(window);
        }

        window.draw(scoreText); // ���� �ؽ�Ʈ �׸���
        window.draw(healthText); // ü�� �ؽ�Ʈ �׸���

        window.display();
        Sleep(1);
        
    }
}

// ���� ���� ȭ��
void GameOverScreen(RenderWindow& window) {
    Font font;
    font.loadFromFile("neodgm.ttf"); // ��Ʈ �ε�

    Text gameOverText("Game Over", font, 50);
    gameOverText.setPosition(250, 200);
    gameOverText.setFillColor(Color::Red);

    Text restartText("Press R to Restart", font, 30);
    restartText.setPosition(250, 300);
    restartText.setFillColor(Color::White);

    Text quitText("Press Q to Quit", font, 30);
    quitText.setPosition(250, 350);
    quitText.setFillColor(Color::White);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::R) return; // R Ű�� ������ ���� �����
                if (event.key.code == Keyboard::Q) window.close(); // Q Ű�� ������ â �ݱ�
            }
        }

        window.clear();
        window.draw(gameOverText);
        window.draw(restartText);
        window.draw(quitText);
        window.display();
    }
}
