#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;

class Enemy : public RectangleShape {
private:
    static Texture texture1; // 정적 멤버 변수로 변경
    bool isFixed = false;
public:
    bool active = true;
    float speed = 2.0;

    Enemy(float x, float y) {
        if (!texture1.loadFromFile("enemy.png")) {
            std::cerr << "Error: Failed to load enemy.png" << std::endl;
            exit(EXIT_FAILURE); // 실패 시 프로그램 종료
        }

        setTexture(&texture1);
        setTextureRect(IntRect(0, 0, texture1.getSize().x, texture1.getSize().y)); // 텍스처 전체 적용
        setSize({ 40.0, 40.0 });
        setOrigin(20.0, 10.0);
        setPosition(x, y);

    }

    void update() {
        // 적이 고정되지 않으면 아래로 이동
        if (!isFixed) {
            move(0, speed);  // 아래로 이동
            if (getPosition().y > 100) {  // 상단에 고정될 위치 (여기서는 y > 100으로 설정)
                isFixed = true;  // 상단에 도달하면 고정
                speed = 0.0f;  // 더 이상 이동하지 않도록 속도를 0으로 설정
            }
        }
    }
};