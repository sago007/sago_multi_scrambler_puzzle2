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

#ifndef CONFETTI_HPP
#define CONFETTI_HPP

#include <SDL.h>
#include <vector>

/**
 * Confetti particle system for celebrating puzzle completion.
 * Creates and animates colorful particles with physics simulation.
 */
class Confetti {
public:
	Confetti();
	~Confetti();

	/**
	 * Trigger a confetti burst
	 * @param screenWidth Width of the screen for particle distribution
	 * @param screenHeight Height of the screen (particles spawn from top)
	 */
	void Burst(int screenWidth, int screenHeight);

	/**
	 * Update particle physics
	 * @param deltaTime Time elapsed since last update in seconds
	 */
	void Update(float deltaTime);

	/**
	 * Render all active confetti particles
	 * @param target SDL renderer to draw to
	 */
	void Draw(SDL_Renderer* target);

	/**
	 * Clear all active particles
	 */
	void Clear();

	/**
	 * Check if there are active particles
	 * @return true if confetti is still animating
	 */
	bool IsActive() const;

private:
	struct Particle {
		float x, y;           // Position
		float vx, vy;         // Velocity
		float rotation;       // Rotation angle
		float rotationSpeed;  // Rotation speed
		Uint8 r, g, b;       // Color
		float size;          // Size of the confetti
		float lifetime;      // Time to live
		float gravity;       // Gravity multiplier
	};

	std::vector<Particle> particles;
};

#endif // CONFETTI_HPP
