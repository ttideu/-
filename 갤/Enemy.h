#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;

class Enemy : public RectangleShape {
private:
    static Texture texture1; // ���� ��� ������ ����
    bool isFixed = false;
public:
    bool active = true;
    float speed = 2.0;

    Enemy(float x, float y) {
        if (!texture1.loadFromFile("enemy.png")) {
            std::cerr << "Error: Failed to load enemy.png" << std::endl;
            exit(EXIT_FAILURE); // ���� �� ���α׷� ����
        }

        setTexture(&texture1);
        setTextureRect(IntRect(0, 0, texture1.getSize().x, texture1.getSize().y)); // �ؽ�ó ��ü ����
        setSize({ 40.0, 40.0 });
        setOrigin(20.0, 10.0);
        setPosition(x, y);

    }

    void update() {
        // ���� �������� ������ �Ʒ��� �̵�
        if (!isFixed) {
            move(0, speed);  // �Ʒ��� �̵�
            if (getPosition().y > 100) {  // ��ܿ� ������ ��ġ (���⼭�� y > 100���� ����)
                isFixed = true;  // ��ܿ� �����ϸ� ����
                speed = 0.0f;  // �� �̻� �̵����� �ʵ��� �ӵ��� 0���� ����
            }
        }
    }
};