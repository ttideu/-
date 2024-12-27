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

// 함수 선언
void TitleLoop(RenderWindow& window);
void GameLoop(RenderWindow& window);
void GameOverScreen(RenderWindow& window);

// 메인 함수
int main() {
    RenderWindow window(VideoMode(800, 600), "Galaga", Style::Close);
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        TitleLoop(window); // 타이틀 화면으로 시작
        GameLoop(window);  // 게임 플레이 루프 실행
        GameOverScreen(window); // 게임 오버 화면 실행
    }

    return 0;
}

// 타이틀 루프
void TitleLoop(RenderWindow& window) {
    Font font;
    font.loadFromFile("neodgm.ttf"); // 폰트 로딩

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
                return; // ENTER 키를 누르면 타이틀 루프 종료
            }
        }

        window.clear();
        window.draw(titleText);
        window.draw(startText);
        window.display();
    }
}

Clock bossRespawnClock;
float bossRespawnDelay = 0.0f; // 보스 재출현 대기 시간 

// 보스 관련 변수
bool bossDefeated = false;       // 보스 처치 여부
int lastBossDefeatScore = 0;     // 보스를 처치한 시점의 점수

// 게임 플레이 루프
void GameLoop(RenderWindow& window) {
    Player player(400, 500);
    std::vector<Bullet> bullets;
    std::vector<Bullet> enemyBullets; // 적의 총알
    std::vector<Enemy> enemies;
    std::vector<Explosion> explosions;

    // 보스 초기화
    Boss boss(400, 50); // 화면 상단 중앙에서 시작
    std::vector<Bullet> bossBullets; // 보스 총알 관리
    bool bossActive = false; // 보스 출현 여부

    Clock clock;

    Clock enemySpawnClock; // 적 생성 타이머
    Clock enemyFireClock;  // 적 발사 타이머
    Clock bulletFireClock;  // 총알 발사 타이머 추가

    float spawnInterval = 2.0f; // 적 생성 간격 (초)
    float fireInterval = 1.5f;  // 적 발사 간격 (초)
    float bulletFireInterval = 0.2f;  // 총알 발사 간격 (0.2초)
    int score = 0;
    bool isGameOver = false;

    // 텍스트 설정
    Font font;
    font.loadFromFile("neodgm.ttf");// 폰트 로딩 

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, 10);

    Text healthText;
    healthText.setFont(font);
    healthText.setCharacterSize(24);
    healthText.setFillColor(Color::White);
    healthText.setPosition(10, 40);  // 점수 아래에 체력 표시

    // 배경 이미지 로딩
    Texture backgroundTexture;
    backgroundTexture.loadFromFile("space.png");

    // 배경 크기 맞추기 (배경 크기가 화면과 다를 경우)
    backgroundTexture.setSmooth(true);  // 부드럽게 처리
    Sprite backgroundSprite1(backgroundTexture);  // 첫 번째 배경
    Sprite backgroundSprite2(backgroundTexture);  // 두 번째 배경

    // 배경 위치 초기화
    backgroundSprite1.setPosition(0, 0);
    backgroundSprite2.setPosition(0, -600);  // 두 번째 배경은 첫 번째 배경 밑에 위치

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        // 배경 스크롤 처리
        backgroundSprite1.move(0, 2);  // 배경을 아래로 이동
        backgroundSprite2.move(0, 2);

        // 두 배경이 화면을 벗어나면 다시 위로 이동
        if (backgroundSprite1.getPosition().y >= 600) {
            backgroundSprite1.setPosition(0, -600);  // 첫 번째 배경을 다시 위로
        }
        if (backgroundSprite2.getPosition().y >= 600) {
            backgroundSprite2.setPosition(0, -600);  // 두 번째 배경을 다시 위로
        }

        // 입력 처리
        if (Keyboard::isKeyPressed(Keyboard::Left)) player.moveLeft();
        if (Keyboard::isKeyPressed(Keyboard::Right)) player.moveRight();
        if (Keyboard::isKeyPressed(Keyboard::Up)) player.moveUp();
        if (Keyboard::isKeyPressed(Keyboard::Down)) player.moveDown();
        //if (Keyboard::isKeyPressed(Keyboard::W)) { score += 100; Sleep(1000); }

        if (Keyboard::isKeyPressed(Keyboard::Space) && bulletFireClock.getElapsedTime().asSeconds() > bulletFireInterval) {
            bullets.push_back(Bullet(
                (player.getPosition().x + player.getSize().x / 2) - 25, // X 위치: 플레이어 중심에서 총알 폭의 절반 빼기
                player.getPosition().y - 10,                            // Y 위치: 플레이어 위쪽
                true
            ));
            bulletFireClock.restart();  // 타이머 리셋
        }

        // 게임 로직

        // 총알 업데이트
        for (auto& bullet : bullets) bullet.update();

        // 적 업데이트
        for (auto& enemy : enemies) enemy.update();

        // 적 총알 업데이트
        for (auto& enemyBullet : enemyBullets) {
            if (enemyBullet.active) {
                enemyBullet.update(); // 이동 처리
            }
        }

        // 파티클 업데이트
        for (auto& explosion : explosions) { explosion.update(); }

        // 보스 총알 업데이트
        for (auto& bullet : bossBullets) {
            if (bullet.active) {
                bullet.update();
                // 플레이어와 충돌 체크
                if (bullet.getGlobalBounds().intersects(player.getGlobalBounds())) {
                    bullet.active = false;
                    player.health--; // 플레이어 체력 감소
                    if (player.health <= 0) { isGameOver = true; }
                }
            }
        }

        // 보스 재출현 로직 수정
        if (score != 0 && score % 200 == 0 && !boss.isActive() && !bossDefeated) {
            boss.active = true;
            boss.health = 30;
            boss.setPosition(400, 50);
        }

        // 보스가 활성화된 경우
        if (boss.isActive()) {
            boss.update();

            // 보스 총알 발사
            static int bossShootTimer = 0;
            if (++bossShootTimer > 100) {
                boss.shoot(bossBullets);
                bossShootTimer = 0;
            }

            // 보스 체력 감소 처리
            for (auto& bullet : bullets) {
                if (bullet.active && boss.getGlobalBounds().intersects(bullet.getGlobalBounds())) {
                    bullet.active = false;
                    boss.takeDamage();
                }
            }

            // 보스 체력이 0일 때
            if (boss.getHealth() <= 0) {
                boss.active = false;
                bossDefeated = true; // 보스 처치 상태로 전환
                score += 10; // 처치 => score 증가

                lastBossDefeatScore = score; // 처치 시점 기록
                explosions.push_back(Explosion(boss.getPosition().x, boss.getPosition().y));
                boss.setPosition(-200, -200);
                bossRespawnClock.restart(); // 보스 재출현 타이머 리셋
            }
        }

        // 보스 처치 후 재출현 대기
        if (bossDefeated && bossRespawnClock.getElapsedTime().asSeconds() > 5.0f && score - lastBossDefeatScore >= 200) {
            bossDefeated = false;
        }

        // 적 지속 생성
        if (enemySpawnClock.getElapsedTime().asSeconds() > spawnInterval && !boss.active) {
            float x = rand() % (800 - 40); // 화면 너비(800) - 적 크기(40)
            enemies.push_back(Enemy(x, 0));
            enemySpawnClock.restart(); // 타이머 리셋
        }

        // 적 발사 로직
        if (enemyFireClock.getElapsedTime().asSeconds() > fireInterval) {
            for (auto& enemy : enemies) {
                if (enemy.active) {
                    float playerX = player.getPosition().x + 40; // 플레이어의 중심 X
                    float playerY = player.getPosition().y + 10; // 플레이어의 중심 Y
                    float enemyX = enemy.getPosition().x + 30;  // 적의 중심 X
                    float enemyY = enemy.getPosition().y + 20;  // 적의 중심 Y

                    // 플레이어와 적의 상대 위치를 기반으로 방향 계산
                    float dx = playerX - enemyX;
                    float dy = playerY - enemyY;

                    // 방향 벡터 정규화
                    float length = sqrt(dx * dx + dy * dy);
                    dx /= length;
                    dy /= length;

                    // 적의 총알 발사 (플레이어 방향으로)
                    Bullet enemyBullet(enemyX, enemyY, false);
                    enemyBullet.setDirection(dx * 4.0f, dy * 4.0f); // 방향에 맞춰 속도 설정
                    enemyBullets.push_back(enemyBullet);
                }
            }
            enemyFireClock.restart();
        }

        // 충돌 처리 (플레이어 총알 -> 적)
        for (auto& bullet : bullets) {
            for (auto& enemy : enemies) {
                if (bullet.active && enemy.active && bullet.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
                    bullet.active = false;
                    enemy.active = false;
                    score += 10; // 처치 => score 증가

                    //펑
                    explosions.push_back(Explosion(enemy.getPosition().x, enemy.getPosition().y));
                }
            }
        }

        // 충돌 감지 (적 포 -> 플레이어)
        for (auto& enemyBullet : enemyBullets) {
            if (enemyBullet.active && enemyBullet.getGlobalBounds().intersects(player.getGlobalBounds())) {
                enemyBullet.active = false; // 적 총알 비활성화
                player.takeDamage();        // 플레이어 체력 감소

                if (player.health <= 0) { isGameOver = true; }
            }
        }

        // 적 포격 비활성화 처리 - 화면 밖으로 나가면 비활성화
        for (auto& enemyBullet : enemyBullets) {
            if (enemyBullet.getPosition().y > 600) {
                enemyBullet.active = false;
            }
        }

        // 체력 표시
        healthText.setString("Forces : " + to_string(player.health));

        // 텍스트 업데이트
        scoreText.setString("Score : " + to_string(score));

        if (isGameOver) return; // 게임 오버

        

        // 렌더링
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

        window.draw(scoreText); // 점수 텍스트 그리기
        window.draw(healthText); // 체력 텍스트 그리기

        window.display();
        Sleep(1);
        
    }
}

// 게임 오버 화면
void GameOverScreen(RenderWindow& window) {
    Font font;
    font.loadFromFile("neodgm.ttf"); // 폰트 로딩

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
                if (event.key.code == Keyboard::R) return; // R 키를 누르면 게임 재시작
                if (event.key.code == Keyboard::Q) window.close(); // Q 키를 누르면 창 닫기
            }
        }

        window.clear();
        window.draw(gameOverText);
        window.draw(restartText);
        window.draw(quitText);
        window.display();
    }
}
