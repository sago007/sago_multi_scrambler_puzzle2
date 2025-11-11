/*
Copyright (c) 2025 Poul Sander

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Confetti.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cmath>
#include <random>

Confetti::Confetti() {
}

Confetti::~Confetti() {
}

void Confetti::Burst(int screenWidth, int screenHeight) {
	particles.clear();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> xDist(0.0f, static_cast<float>(screenWidth));
	std::uniform_real_distribution<float> yDist(-100.0f, 0.0f);
	std::uniform_real_distribution<float> vxDist(-100.0f, 100.0f);
	std::uniform_real_distribution<float> vyDist(50.0f, 200.0f);
	std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
	std::uniform_real_distribution<float> rotSpeedDist(-360.0f, 360.0f);
	std::uniform_real_distribution<float> sizeDist(4.0f, 12.0f);
	std::uniform_real_distribution<float> gravityDist(100.0f, 300.0f);

	// Create colorful confetti particles
	const int numParticles = 200;
	for (int i = 0; i < numParticles; ++i) {
		Particle particle;
		particle.x = xDist(gen);
		particle.y = yDist(gen);
		particle.vx = vxDist(gen);
		particle.vy = vyDist(gen);
		particle.rotation = rotDist(gen);
		particle.rotationSpeed = rotSpeedDist(gen);
		particle.size = sizeDist(gen);
		particle.lifetime = 5.0f;  // 5 seconds
		particle.gravity = gravityDist(gen);

		// Bright, saturated colors
		int colorChoice = i % 6;
		switch (colorChoice) {
		case 0:
			particle.r = 255;
			particle.g = 0;
			particle.b = 0;
			break;      // Red
		case 1:
			particle.r = 0;
			particle.g = 255;
			particle.b = 0;
			break;      // Green
		case 2:
			particle.r = 0;
			particle.g = 0;
			particle.b = 255;
			break;      // Blue
		case 3:
			particle.r = 255;
			particle.g = 255;
			particle.b = 0;
			break;    // Yellow
		case 4:
			particle.r = 255;
			particle.g = 0;
			particle.b = 255;
			break;    // Magenta
		case 5:
			particle.r = 0;
			particle.g = 255;
			particle.b = 255;
			break;    // Cyan
		}

		particles.push_back(particle);
	}
}

void Confetti::Update(float deltaTime) {
	// Update all confetti particles
	for (auto it = particles.begin(); it != particles.end();) {
		it->lifetime -= deltaTime;

		if (it->lifetime <= 0.0f) {
			it = particles.erase(it);
		}
		else {
			// Update position
			it->x += it->vx * deltaTime;
			it->y += it->vy * deltaTime;

			// Apply gravity
			it->vy += it->gravity * deltaTime;

			// Update rotation
			it->rotation += it->rotationSpeed * deltaTime;

			// Add some air resistance
			it->vx *= 0.99f;

			++it;
		}
	}
}

void Confetti::Draw(SDL_Renderer* target) {
	for (const auto& particle : particles) {
		// Calculate alpha based on lifetime (fade out in last second)
		Uint8 alpha = 255;
		if (particle.lifetime < 1.0f) {
			alpha = static_cast<Uint8>(255 * particle.lifetime);
		}

		// Draw confetti as small filled rectangles
		float halfSize = particle.size / 2.0f;
		float angle = particle.rotation * M_PI / 180.0f;

		// Simple rectangle for confetti pieces
		Sint16 x1 = static_cast<Sint16>(particle.x - halfSize * std::cos(angle));
		Sint16 y1 = static_cast<Sint16>(particle.y - halfSize * std::sin(angle));
		Sint16 x2 = static_cast<Sint16>(particle.x + halfSize * std::cos(angle));
		Sint16 y2 = static_cast<Sint16>(particle.y + halfSize * std::sin(angle));
		Sint16 x3 = static_cast<Sint16>(particle.x + halfSize * std::cos(angle) - halfSize * std::sin(angle));
		Sint16 y3 = static_cast<Sint16>(particle.y + halfSize * std::sin(angle) + halfSize * std::cos(angle));
		Sint16 x4 = static_cast<Sint16>(particle.x - halfSize * std::cos(angle) - halfSize * std::sin(angle));
		Sint16 y4 = static_cast<Sint16>(particle.y - halfSize * std::sin(angle) + halfSize * std::cos(angle));

		// Draw a filled polygon (quad)
		Sint16 vx[] = {x1, x2, x3, x4};
		Sint16 vy[] = {y1, y2, y3, y4};
		filledPolygonRGBA(target, vx, vy, 4, particle.r, particle.g, particle.b, alpha);
	}
}

void Confetti::Clear() {
	particles.clear();
}

bool Confetti::IsActive() const {
	return !particles.empty();
}
