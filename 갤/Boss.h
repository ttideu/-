#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>  // cos, sin �Լ� ���

using namespace sf;

class Boss : public RectangleShape {
private:
    Texture texture;

    float speed = 1.0f; // �¿�� �����̴� �ӵ�
    float direction = 1.0f; // 1.0: ������, -1.0: ����

public:
    int health = 30;  // ������ ü��
    bool active = false; // ������ ���� ������ ����

    Boss(float x, float y) {
        if (!texture.loadFromFile("boss.png")) {
            std::cerr << "Error: Failed to load boss.png" << std::endl;
            exit(EXIT_FAILURE);
        }
        setTexture(&texture);
        setSize({ 130.0f, 80.0f }); // ���� ũ��
        setOrigin(getSize().x / 2, getSize().y / 2);
        setPosition(x, y);
    }

    void update() {
        if (!active) return;

        // �¿�� ������
        move(direction * speed, 0);
        if (getPosition().x <= 60 || getPosition().x >= 740) {
            direction *= -1;
        }
    }

    void takeDamage() {
        if (--health == 0) { // ü���� 0�� �Ǹ� ��Ȱ��ȭ
            active = false;
        }
    }

    int getHealth() const { return health; }

    void shoot(std::vector<Bullet>& bossBullets) {
        if (!active) return;

        // �ټ��� �Ѿ� ����
        float x = getPosition().x;
        float y = getPosition().y + getSize().y / 2;

        // ���� ������ �߻� (���� ���� ����)
        for (float angle = 60; angle <= 120; angle += 30) { // ���� ������ -60�� ~ 60���� ����
            Bullet bullet(x, y, false);  // �Ѿ��� ���� ������ �ʵ��� false�� ����
            // ������ �������� ��ȯ�ϰ�, ���� ����
            float rad = angle * 3.14159 / 180;
            bullet.setDirection(cos(rad), sin(rad));  // ������ �̿��Ͽ� ���� ����
            bullet.speed = 5.0f; // �Ѿ� �ӵ� ����
            bossBullets.push_back(bullet);  // �Ѿ��� ������ �Ѿ� ����Ʈ�� �߰�
        }
    }

    bool isActive() const { return active; }  // ������ Ȱ��ȭ�Ǿ� �ִ��� Ȯ���ϴ� �Լ�
};