#ifndef GAME_SCENE_CLASS_HEADER
#define GAME_SCENE_CLASS_HEADER

#include <SceneLayer.hpp>
#include <game/Camera.hpp>
#include <game/Box2D.hpp>
#include <game/Ball.hpp>
#include <game/BallsController.hpp>
#include <game/sandbox/SandboxBallsController.hpp>
#include <game/classic/ClassicBallsController.hpp>

#include <Engine/Physics/2D/RectangleCollider2D.hpp>
#include <Engine/Graphics/2D/RenderCanvas.hpp>
#include <Engine/Core/ConfigManager.hpp>
#include <Engine/Core/SignalBus.hpp>
#include <Engine/Core/Delta.hpp>
#include <Engine/Math/Vec2.hpp>

class GameArea : public eng::phy::AreaBody2D
{
public:
	void onSetup() override
	{
		m_context.get<eng::phy::PhysicsWorld>().addBody(*this);
	}

	void onDestroy() override
	{
		m_context.get<eng::phy::PhysicsWorld>().removeBody(*this);
	}

	void onCollisionExit(eng::phy::PhysicsBody2D& other) override;
};

class GameScene : public SceneLayer
{
protected:
	Camera* m_camera2d = nullptr;
	Box2D* m_box = nullptr;
	BallsController* m_controller = nullptr;
	eng::phy::RectangleCollider2D* m_game_area_collider = nullptr;

public:
	void onBuild() override
	{
		eng::mth::Vec2 box_pos(0, 50);

		m_camera2d   = addChild<Camera>("camera");
		m_box        = addChild<Box2D>("box", 400, -150, 150);
		m_controller = addChild<ClassicBallsController>("controller", box_pos + m_box->getLeftBound(), box_pos + m_box->getRightBound());

		m_box->setPosition(box_pos);

		auto game_area = addChild<GameArea>("game_area");
		m_game_area_collider = game_area->setCollider<eng::phy::RectangleCollider2D>();
	}

	void onSetup() override
	{
		SceneLayer::onSetup();

		eng::mth::Vec2 v_size = m_context.get<eng::core::ConfigManager>().get<eng::mth::Vec2>("window_viewport_size");
		eng::mth::Vec2 c_size(1000*(v_size.x/v_size.y), 1000);
		m_camera2d->setSize(c_size);
		m_context.get<eng::gfx::RenderCanvas>().setActiveCamera(*m_camera2d);

		m_game_area_collider->setSize(c_size);

		m_context.get<eng::core::SignalBus>().emit("game_mode_changed", std::string("classic"));

		m_camera_signal_id = m_context.get<eng::core::SignalBus>().subscribe("on_change_config_window_viewport_size",
			[this](eng::mth::Vec2 size){
				m_camera2d->setSize(eng::mth::Vec2(1000*(size.x/size.y), 1000));
		});
	}

	void onDestroy() override
	{
		SceneLayer::onDestroy();
		m_context.get<eng::core::SignalBus>().unsubscribe(m_camera_signal_id);
	}

	void onUpdate(const eng::core::Delta& delta) override
	{
		auto event_manager = m_context.get<eng::sys::EventManager>();

		if (m_controller && m_controller->isGameOver())
			m_context.get<eng::core::SignalBus>().emit("game_over");

		if (event_manager.getKeyboard().isPressed(eng::sys::Keyboard::Key::R))
		{
			if (event_manager.getKeyboard().isJustPressed(eng::sys::Keyboard::Key::F))
			{
				removeChild(m_controller);
				m_controller = addChild<SandboxBallsController>("controller");
				m_context.get<eng::core::SignalBus>().emit("game_restart");
				m_context.get<eng::core::SignalBus>().emit("game_mode_changed", std::string("free"));
			}
			else if (event_manager.getKeyboard().isJustPressed(eng::sys::Keyboard::Key::C))
			{
				removeChild(m_controller);
				m_controller = addChild<ClassicBallsController>("controller", m_box->getPosition() + m_box->getLeftBound(), m_box->getPosition() + m_box->getRightBound());
				m_context.get<eng::core::SignalBus>().emit("game_restart");
				m_context.get<eng::core::SignalBus>().emit("game_mode_changed", std::string("classic"));
			}
		}

		SceneLayer::onUpdate(delta);
	}

protected:
	eng::core::SubscriptionId m_camera_signal_id;
};

void GameArea::onCollisionExit(eng::phy::PhysicsBody2D& other)
{
	Ball* ball = dynamic_cast<Ball*>(&other);
	if (ball)
		m_context.get<eng::core::SignalBus>().emit("ball_fall", ball);
}

#endif //GAME_SCENE_CLASS_HEADER