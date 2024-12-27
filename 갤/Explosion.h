#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

using namespace sf;
using namespace std;

class Explosion {
private:
    struct Particle {
        CircleShape shape;
        Vector2f velocity;
        float lifetime; // ���� ����
    };

    vector<Particle> particles;
    float maxLifetime = 1.0f; // ��ƼŬ�� �ִ� ���� (��)
    float explosionRadius = 30.0f; // �� ũ�� ������ ���� �ݰ� ����

public:
    Explosion(float x, float y) {
        // ���� ��ġ�� �������� ��ƼŬ�� �����¿� 1ĭ �ݰ� ������ ������ ����
        for (int i = 0; i < 20; i++) {
            Particle p;
            p.shape.setRadius(4.0f);  // ��ƼŬ ũ��
            p.shape.setFillColor(Color::Yellow); // ��ƼŬ ����
            p.shape.setPosition(x, y); // ���� ��ġ ����

            // ��ƼŬ�� �̵� ������ ������ �����Ͽ� ������ ��
            float angle = rand() % 360; // 0~360�� ������ ���� ���� ����
            float speed = rand() % 10 + 5; // �ӵ� ����: 5~15 (�ӵ��� �����Ͽ� �� ��������)

            // 1ĭ �ݰ� �ȿ����� ��ƼŬ�� �������� ���
            float dx = cos(angle * 3.14159f / 180.0f) * speed;
            float dy = sin(angle * 3.14159f / 180.0f) * speed;

            // ��ƼŬ�� �ӵ��� �����Ͽ� ���� �ֺ� �ݰ� �������� ������ ����
            p.velocity = Vector2f(dx, dy);
            p.lifetime = maxLifetime; // ��ƼŬ�� �ʱ� ���� ����
            particles.push_back(p); // ��ƼŬ �߰�
        }
    }

    void update() {
        // ��ƼŬ ������Ʈ
        for (auto& particle : particles) {
            particle.shape.move(particle.velocity); // ��ƼŬ �̵�
            particle.lifetime -= 0.15f; // �ð� ����� ���� ���� ����
        }

        // ������ ���� ��ƼŬ ����
        particles.erase(remove_if(particles.begin(), particles.end(), [](const Particle& p) {
            return p.lifetime <= 0.0f; // ������ ���� ��ƼŬ�� ����
            }), particles.end());
    }

    void draw(RenderWindow& window) {
        // ��ƼŬ�� ȭ�鿡 �׸���
        for (auto& particle : particles) {
            window.draw(particle.shape);
        }
    }

    bool isFinished() {
        return particles.empty(); // ��� ��ƼŬ�� ��������� ����
    }
};