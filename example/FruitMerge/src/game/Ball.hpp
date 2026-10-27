#ifndef BALL_CLASS_HEADER
#define BALL_CLASS_HEADER

#include <Engine/Physics/2D/RigidBody2D.hpp>

#include <Engine/Physics/2D/AreaBody2D.hpp>
#include <Engine/Physics/2D/CircleCollider2D.hpp>
#include <Engine/Physics/PhysicsWorld.hpp>
#include <Engine/Core/ResourceManager.hpp>
#include <Engine/Core/SignalBus.hpp>
#include <Engine/Core/Delta.hpp>

#include <Engine/Graphics/2D/Sprite2D.hpp>
#include <Engine/Graphics/2D/Text2D.hpp>
#include <Engine/Graphics/2D/RenderCanvas.hpp>
#include <Engine/Graphics/Color.hpp>
#include <Engine/Graphics/Texture.hpp>
#include <Engine/Graphics/Font.hpp>
#include <Engine/Math/Vec2.hpp>

#include <random>
#include <string>

class Ball;

class BallArea : public eng::phy::AreaBody2D
{
public:
	void onCollisionStay(eng::phy::PhysicsBody2D& other) override;
};

class Ball : public eng::phy::RigidBody2D
{
protected:
	BallArea* m_area = nullptr;
	eng::gfx::Sprite2D* m_sprite = nullptr;

public:
	Ball(unsigned int start_level): m_level(start_level) {}

	void onBuild() override
	{
		m_area = addChild<BallArea>("area");
		m_sprite = addChild<eng::gfx::Sprite2D>("sprite");

		setCollider<eng::phy::CircleCollider2D>();
		m_area->setCollider<eng::phy::CircleCollider2D>();
	}

	void onSetup() override
	{
		m_context.get<eng::gfx::RenderCanvas>().addObject(*m_sprite);

		if (m_level)
		{
			m_context.get<eng::phy::PhysicsWorld>().addBody(*m_area);
			m_context.get<eng::phy::PhysicsWorld>().addBody(*this);
			setRestitution(0.8);
		}

		setLevel(m_level);
	}

	void onDestroy() override
	{
		m_context.get<eng::gfx::RenderCanvas>().removeObject(*m_sprite);
		m_context.get<eng::phy::PhysicsWorld>().removeBody(*m_area);
		m_context.get<eng::phy::PhysicsWorld>().removeBody(*this);
	}

	void onUpdate(const eng::core::Delta& delta) override
	{
		applyForce(eng::mth::Vec2(0, 200)*getMass());
	}

	void setLevel(unsigned int level)
	{
		if (!level) return;

		m_level = level;
		unsigned int rad = level*10 + 10;

		auto* tex = m_context.get<eng::core::ResourceManager>().load<eng::gfx::Texture>({"res/fruit" + std::to_string(level - 1) + ".png"}).second;
		if (tex)
		{
			tex->setSmooth(true);
			m_sprite->setTexture(tex);
			m_sprite->setScale((rad*2)/tex->getSize().x);
		}

		auto area_collider = static_cast<eng::phy::CircleCollider2D*>(m_area->getCollider());
		area_collider->setRadius(rad*1.15);

		auto collider = static_cast<eng::phy::CircleCollider2D*>(getCollider());
		collider->setRadius(rad*0.95);
		setMass(rad*3);
	}

	unsigned int getLevel()
	{
		return m_level;
	}

protected:
	unsigned int m_level;
};

void BallArea::onCollisionStay(eng::phy::PhysicsBody2D& other)
{
	BallArea* other_area = dynamic_cast<BallArea*>(&other);
	if (other_area)
	{
		Ball* self_ball = static_cast<Ball*>(getParent());
		Ball* other_ball = static_cast<Ball*>(other_area->getParent());
		if (self_ball->getLevel() == other_ball->getLevel())
			m_context.get<eng::core::SignalBus>().emit("balls_collided", self_ball, other_ball);
	}
}

#endif //BALL_CLASS_HEADER