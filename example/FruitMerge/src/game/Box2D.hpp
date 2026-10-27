#ifndef BOX_2D_CLASS_HEADER
#define BOX_2D_CLASS_HEADER

#include <game/Ball.hpp>

#include <Engine/Core/Node2D.hpp>

#include <Engine/Physics/2D/RectangleCollider2D.hpp>
#include <Engine/Physics/2D/StaticBody2D.hpp>
#include <Engine/Physics/2D/AreaBody2D.hpp>
#include <Engine/Physics/PhysicsWorld.hpp>

#include <Engine/Graphics/2D/Shape2D.hpp>
#include <Engine/Graphics/2D/RenderCanvas.hpp>
#include <Engine/Graphics/Color.hpp>
#include <Engine/Math/Vec2.hpp>
#include <Engine/Core/Logger.hpp>

class Box2D : public eng::phy::AreaBody2D
{
public:
	Box2D(float height, float left_bound, float right_bound):
		m_height(height),
		m_left_bound(left_bound),
		m_right_bound(right_bound)
	{}

	void onBuild() override
	{
		const float border_width = 20;
		const float w_size = m_right_bound - m_left_bound;

		auto col = setCollider<eng::phy::RectangleCollider2D>();
		col->setSize(eng::mth::Vec2(w_size*1.2, m_height*1.3));
		col->move(eng::mth::Vec2(0, -m_height*0.075));
		auto sh = addChild<eng::gfx::Shape2D>("shape", eng::gfx::Shape2D::Type::RECTANGLE);
		sh->setSize(eng::mth::Vec2(w_size*1.2, m_height*1.3));
		sh->move(eng::mth::Vec2(0, -m_height*0.075));
		sh->setColor(eng::gfx::Color(255, 255, 255, 64));

		auto b = addChild<eng::phy::StaticBody2D>("floor");
		b->setPosition(eng::mth::Vec2(0, 200 + border_width/2));
		sh = b->addChild<eng::gfx::Shape2D>("shape", eng::gfx::Shape2D::Type::RECTANGLE);
		sh->setSize(eng::mth::Vec2(w_size + border_width, border_width));
		sh->setColor(eng::gfx::Color(0, 0, 255));
		col = b->setCollider<eng::phy::RectangleCollider2D>();
		col->setSize(eng::mth::Vec2(w_size + border_width, border_width));

		b = addChild<eng::phy::StaticBody2D>("left_side");
		b->setPosition(eng::mth::Vec2(m_left_bound - border_width/2, border_width/4));
		sh = b->addChild<eng::gfx::Shape2D>("shape", eng::gfx::Shape2D::Type::RECTANGLE);
		sh->setSize(eng::mth::Vec2(border_width, m_height + border_width/2));
		sh->setColor(eng::gfx::Color(0, 0, 255));
		col = b->setCollider<eng::phy::RectangleCollider2D>();
		col->setSize(eng::mth::Vec2(border_width, m_height + border_width/2));

		b = addChild<eng::phy::StaticBody2D>("right_side");
		b->setPosition(eng::mth::Vec2(m_right_bound + border_width/2, border_width/4));
		sh = b->addChild<eng::gfx::Shape2D>("shape", eng::gfx::Shape2D::Type::RECTANGLE);
		sh->setSize(eng::mth::Vec2(border_width, m_height + border_width/2));
		sh->setColor(eng::gfx::Color(0, 0, 255));
		col = b->setCollider<eng::phy::RectangleCollider2D>();
		col->setSize(eng::mth::Vec2(border_width, m_height + border_width/2));
	}

	void onSetup() override
	{
		eng::phy::PhysicsWorld& PW = m_context.get<eng::phy::PhysicsWorld>();
		eng::gfx::RenderCanvas& RC = m_context.get<eng::gfx::RenderCanvas>();

		auto sh = static_cast<eng::gfx::Shape2D*>(getChildByName("shape"));
		PW.addBody(*this);
		RC.addObject(*sh);

		auto b = static_cast<eng::phy::StaticBody2D*>(getChildByName("floor"));
		sh = static_cast<eng::gfx::Shape2D*>(b->getChildByName("shape"));
		PW.addBody(*b);
		RC.addObject(*sh);

		b = static_cast<eng::phy::StaticBody2D*>(getChildByName("left_side"));
		sh = static_cast<eng::gfx::Shape2D*>(b->getChildByName("shape"));
		PW.addBody(*b);
		RC.addObject(*sh);

		b = static_cast<eng::phy::StaticBody2D*>(getChildByName("right_side"));
		sh = static_cast<eng::gfx::Shape2D*>(b->getChildByName("shape"));
		PW.addBody(*b);
		RC.addObject(*sh);
	}

	eng::mth::Vec2 getLeftBound()
	{
		return eng::mth::Vec2(m_left_bound + 1, -m_height/2);
	}

	eng::mth::Vec2 getRightBound()
	{
		return eng::mth::Vec2(m_right_bound - 1, -m_height/2);
	}

	void onCollisionExit(eng::phy::PhysicsBody2D& other) override
	{
		Ball* ball = dynamic_cast<Ball*>(&other);
		if (ball)
		{
			m_context.get<eng::core::SignalBus>().emit("ball_exit");
			eng::core::Logger::debug(ball->getTag().getPath());
		}
	}

protected:
	float m_height;
	float m_left_bound;
	float m_right_bound;
};

#endif //BOX_2D_CLASS_HEADER