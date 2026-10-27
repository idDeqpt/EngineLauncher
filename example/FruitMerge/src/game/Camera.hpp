#ifndef CAMERA_CLASS_HEADER
#define CAMERA_CLASS_HEADER

#include <Engine/Graphics/2D/Camera2D.hpp>

#include <Engine/Core/Delta.hpp>
#include <Engine/System/EventManager.hpp>
#include <Engine/Math/Vec2.hpp>

class Camera : public eng::gfx::Camera2D
{
public:
	void onUpdate(const eng::core::Delta& delta) override
	{
		return;
		eng::sys::EventManager& EM = m_context.get<eng::sys::EventManager>();
		eng::mth::Vec2 vel;
		constexpr float speed = 500;
		if (EM.getKeyboard().isPressed(eng::sys::Keyboard::Key::UP))
			vel.y -= speed;
		if (EM.getKeyboard().isPressed(eng::sys::Keyboard::Key::DOWN))
			vel.y += speed;
		if (EM.getKeyboard().isPressed(eng::sys::Keyboard::Key::LEFT))
			vel.x -= speed;
		if (EM.getKeyboard().isPressed(eng::sys::Keyboard::Key::RIGHT))
			vel.x += speed;
		if (EM.getKeyboard().isPressed(eng::sys::Keyboard::Key::Q))
			scale(1.1);
		if (EM.getKeyboard().isPressed(eng::sys::Keyboard::Key::E))
			scale(0.9);

		if (vel.x || vel.y)
			move(vel.norm(speed)*getScale().x*delta.frame);
	}
};

#endif //CAMERA_CLASS_HEADER