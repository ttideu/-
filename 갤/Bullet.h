#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Bullet : public RectangleShape {
public:
    float dx = 0.0f;  // X 방향 속도
    float dy = -8.0f; // Y 방향 속도 (위로 발사)
    bool active = true; // 활성 상태
    float speed = 1.0f; // 총알의 속도 (속도를 설정하는 변수)
    bool isPlayerBullet = false; // 플레이어 총알인지 여부를 나타내는 변수

    // 생성자: 초기 위치, 속도, 색상 설정
    Bullet(float x, float y, bool playerBullet = true, float directionX = 0.0f, float directionY = -8.0f) {
        setSize({ 5.0f, 15.0f });
        setPosition(x, y);
        setFillColor(playerBullet ? Color::White : Color::Color(255, 165, 0));  // 플레이어 총알은 흰색, 적 총알은 빨간색
        setOrigin(2.5f, 7.5f);     // 중앙 정렬

        // 초기 방향 설정
        dx = directionX;
        dy = directionY;
        isPlayerBullet = playerBullet;
        if (isPlayerBullet) {
            speed = 2.0f;  // 플레이어 총알 속도 빠르게 설정
        }
    }

    // 방향 업데이트 (dx, dy로 이동)
    void update() {
        if (active) {
            move(dx * speed, dy * speed);  // 방향에 따라 이동
            if (getPosition().y < 0 || getPosition().y > 600) active = false; // 화면 밖으로 나가면 비활성화
        }
    }

    // 방향을 조정하는 함수 (새로운 방향 설정)
    void setDirection(float directionX, float directionY) {
        dx = directionX;
        dy = directionY;
    }
};