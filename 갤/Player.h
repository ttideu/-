#include <SFML/Graphics.hpp>
using namespace sf;

class Player : public RectangleShape {
private:
    Texture texture;
public:
    float speed = 5.0;  
    int health = 3;     
    bool active = true;

    Player(float x, float y) {
        texture.loadFromFile("ship.png");
        setTexture(&texture);
        setPosition(x, y);
        setSize({ 40.0, 40.0 });
        setOrigin(20.0, 20.0); // �߽�
    }

    void moveLeft() {
        if (getPosition().x > 0) move(-speed, 0);
    }
    void moveRight() {
        if (getPosition().x < 800 - getSize().x) move(speed, 0);
    }
    void moveUp() {
        if (getPosition().y > 0) move(0, -speed);
    }
    void moveDown() {
        if (getPosition().y < 600 - getSize().y) move(0, speed);
    }

    void takeDamage() {
        if (active) {
            health--;
            if (health <= 0) {
                active = false;
            }
        }
    }

    // �ʱ�ȭ �Լ� �߰�
    void reset(float startX, float startY) {
        setPosition(startX, startY); // ��ġ �ʱ�ȭ
        health = 3;                 // ü�� �ʱ�ȭ
        active = true;              // Ȱ�� ���� ����
    }
};