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
        float lifetime; // 남은 수명
    };

    vector<Particle> particles;
    float maxLifetime = 1.0f; // 파티클의 최대 수명 (초)
    float explosionRadius = 30.0f; // 적 크기 정도로 폭발 반경 설정

public:
    Explosion(float x, float y) {
        // 폭발 위치를 기준으로 파티클을 상하좌우 1칸 반경 정도로 퍼지게 설정
        for (int i = 0; i < 20; i++) {
            Particle p;
            p.shape.setRadius(4.0f);  // 파티클 크기
            p.shape.setFillColor(Color::Yellow); // 파티클 색상
            p.shape.setPosition(x, y); // 폭발 위치 설정

            // 파티클의 이동 범위와 방향을 제한하여 퍼지게 함
            float angle = rand() % 360; // 0~360도 범위로 랜덤 각도 생성
            float speed = rand() % 10 + 5; // 속도 범위: 5~15 (속도를 제한하여 덜 퍼지도록)

            // 1칸 반경 안에서만 파티클이 퍼지도록 계산
            float dx = cos(angle * 3.14159f / 180.0f) * speed;
            float dy = sin(angle * 3.14159f / 180.0f) * speed;

            // 파티클의 속도를 제한하여 적의 주변 반경 내에서만 퍼지게 설정
            p.velocity = Vector2f(dx, dy);
            p.lifetime = maxLifetime; // 파티클의 초기 수명 설정
            particles.push_back(p); // 파티클 추가
        }
    }

    void update() {
        // 파티클 업데이트
        for (auto& particle : particles) {
            particle.shape.move(particle.velocity); // 파티클 이동
            particle.lifetime -= 0.15f; // 시간 경과에 따라 수명 감소
        }

        // 수명이 다한 파티클 제거
        particles.erase(remove_if(particles.begin(), particles.end(), [](const Particle& p) {
            return p.lifetime <= 0.0f; // 수명이 다한 파티클을 제거
            }), particles.end());
    }

    void draw(RenderWindow& window) {
        // 파티클을 화면에 그리기
        for (auto& particle : particles) {
            window.draw(particle.shape);
        }
    }

    bool isFinished() {
        return particles.empty(); // 모든 파티클이 사라졌으면 종료
    }
};