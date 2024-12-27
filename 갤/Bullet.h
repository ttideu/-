#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Bullet : public RectangleShape {
public:
    float dx = 0.0f;  // X ���� �ӵ�
    float dy = -8.0f; // Y ���� �ӵ� (���� �߻�)
    bool active = true; // Ȱ�� ����
    float speed = 1.0f; // �Ѿ��� �ӵ� (�ӵ��� �����ϴ� ����)
    bool isPlayerBullet = false; // �÷��̾� �Ѿ����� ���θ� ��Ÿ���� ����

    // ������: �ʱ� ��ġ, �ӵ�, ���� ����
    Bullet(float x, float y, bool playerBullet = true, float directionX = 0.0f, float directionY = -8.0f) {
        setSize({ 5.0f, 15.0f });
        setPosition(x, y);
        setFillColor(playerBullet ? Color::White : Color::Color(255, 165, 0));  // �÷��̾� �Ѿ��� ���, �� �Ѿ��� ������
        setOrigin(2.5f, 7.5f);     // �߾� ����

        // �ʱ� ���� ����
        dx = directionX;
        dy = directionY;
        isPlayerBullet = playerBullet;
        if (isPlayerBullet) {
            speed = 2.0f;  // �÷��̾� �Ѿ� �ӵ� ������ ����
        }
    }

    // ���� ������Ʈ (dx, dy�� �̵�)
    void update() {
        if (active) {
            move(dx * speed, dy * speed);  // ���⿡ ���� �̵�
            if (getPosition().y < 0 || getPosition().y > 600) active = false; // ȭ�� ������ ������ ��Ȱ��ȭ
        }
    }

    // ������ �����ϴ� �Լ� (���ο� ���� ����)
    void setDirection(float directionX, float directionY) {
        dx = directionX;
        dy = directionY;
    }
};