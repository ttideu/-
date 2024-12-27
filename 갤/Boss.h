#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>  // cos, sin 함수 사용

using namespace sf;

class Boss : public RectangleShape {
private:
    Texture texture;

    float speed = 1.0f; // 좌우로 움직이는 속도
    float direction = 1.0f; // 1.0: 오른쪽, -1.0: 왼쪽

public:
    int health = 30;  // 보스의 체력
    bool active = false; // 보스가 출현 중인지 여부

    Boss(float x, float y) {
        if (!texture.loadFromFile("boss.png")) {
            std::cerr << "Error: Failed to load boss.png" << std::endl;
            exit(EXIT_FAILURE);
        }
        setTexture(&texture);
        setSize({ 130.0f, 80.0f }); // 보스 크기
        setOrigin(getSize().x / 2, getSize().y / 2);
        setPosition(x, y);
    }

    void update() {
        if (!active) return;

        // 좌우로 움직임
        move(direction * speed, 0);
        if (getPosition().x <= 60 || getPosition().x >= 740) {
            direction *= -1;
        }
    }

    void takeDamage() {
        if (--health == 0) { // 체력이 0이 되면 비활성화
            active = false;
        }
    }

    int getHealth() const { return health; }

    void shoot(std::vector<Bullet>& bossBullets) {
        if (!active) return;

        // 다수의 총알 생성
        float x = getPosition().x;
        float y = getPosition().y + getSize().y / 2;

        // 여러 각도로 발사 (각도 범위 수정)
        for (float angle = 60; angle <= 120; angle += 30) { // 각도 범위를 -60도 ~ 60도로 설정
            Bullet bullet(x, y, false);  // 총알이 적을 향하지 않도록 false로 설정
            // 각도를 라디안으로 변환하고, 방향 설정
            float rad = angle * 3.14159 / 180;
            bullet.setDirection(cos(rad), sin(rad));  // 각도를 이용하여 방향 설정
            bullet.speed = 5.0f; // 총알 속도 설정
            bossBullets.push_back(bullet);  // 총알을 보스의 총알 리스트에 추가
        }
    }

    bool isActive() const { return active; }  // 보스가 활성화되어 있는지 확인하는 함수
};