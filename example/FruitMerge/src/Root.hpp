#ifndef ROOT_CLASS_HEADER
#define ROOT_CLASS_HEADER

#include <game/UIScene.hpp>
#include <game/GameScene.hpp>

#include <Engine/Core/Node.hpp>

#include <Engine/Core/ResourceManager.hpp>

#include <Engine/Graphics/2D/Camera2D.hpp>
#include <Engine/Graphics/Font.hpp>
#include <Engine/Graphics/2D/RenderCanvas.hpp>
#include <Engine/Math/Vec2.hpp>

#include <string>

class Root : public eng::core::Node
{
protected:
	eng::gfx::Camera2D* m_camera2d = nullptr;
	UIScene*   m_ui_scene   = nullptr;
	GameScene* m_game_scene = nullptr;

public:
	void onBuild() override
	{
		m_camera2d   = addChild<eng::gfx::Camera2D>("main_camera");
		m_ui_scene   = addChild<UIScene>("ui_scene");
		m_game_scene = addChild<GameScene>("game_scene");
	}

	void onSetup() override
	{
		eng::gfx::Font* font = m_context.get<eng::core::ResourceManager>().load<eng::gfx::Font>({"res/GameFont.ttf"}).second;

		eng::mth::Vec2 v_size = m_context.get<eng::core::ConfigManager>().get<eng::mth::Vec2>("window_viewport_size");
		m_camera2d->setSize(v_size);
		m_context.get<eng::gfx::RenderCanvas>().setActiveCamera(*m_camera2d);

		m_ui_scene->setLayer(100);
		m_ui_scene->setFont(*font);
		m_context.get<eng::gfx::RenderCanvas>().addObject(*m_ui_scene);

		m_game_scene->setLayer(10);
		m_context.get<eng::gfx::RenderCanvas>().addObject(*m_game_scene);

		m_viewport_signal_id = m_context.get<eng::core::SignalBus>().subscribe("on_change_config_window_viewport_size",
			[this](eng::mth::Vec2 size){
				m_camera2d->setSize(size);
		});

		m_key_pressed_signal_id = m_context.get<eng::core::SignalBus>().subscribe("keyboard_just_pressed",
			[this](eng::sys::Keyboard::Key key){
				if (key == eng::sys::Keyboard::Key::S)
				{
					auto scaling = m_context.get<eng::core::ConfigManager>().get<std::string>("window_viewport_scaling");
					m_context.get<eng::core::ConfigManager>().set<std::string>("window_viewport_scaling", (scaling == "fixed") ? "stretch" : "fixed");
				}

				if (key == eng::sys::Keyboard::Key::MINUS)
				{
					auto v_size = m_context.get<eng::core::ConfigManager>().get<eng::mth::Vec2>("window_viewport_size");
					v_size = v_size/1.1;
					m_context.get<eng::core::ConfigManager>().set<eng::mth::Vec2>("window_viewport_size", v_size);
				}
				if (key == eng::sys::Keyboard::Key::EQUAL)
				{
					auto v_size = m_context.get<eng::core::ConfigManager>().get<eng::mth::Vec2>("window_viewport_size");
					v_size = v_size*1.1;
					m_context.get<eng::core::ConfigManager>().set<eng::mth::Vec2>("window_viewport_size", v_size);
				}
		});

		m_key_pressed_signal_id = m_context.get<eng::core::SignalBus>().subscribe("keyboard_repeated",
			[this](eng::sys::Keyboard::Key key){
				if (key == eng::sys::Keyboard::Key::LEFT)
				{
					auto centering = m_context.get<eng::core::ConfigManager>().get<eng::mth::Vec2>("window_viewport_centering");
					centering.x -= 0.01;
					m_context.get<eng::core::ConfigManager>().set<eng::mth::Vec2>("window_viewport_centering", centering);
				}
				if (key == eng::sys::Keyboard::Key::RIGHT)
				{
					auto centering = m_context.get<eng::core::ConfigManager>().get<eng::mth::Vec2>("window_viewport_centering");
					centering.x += 0.01;
					m_context.get<eng::core::ConfigManager>().set<eng::mth::Vec2>("window_viewport_centering", centering);
				}
				if (key == eng::sys::Keyboard::Key::UP)
				{
					auto centering = m_context.get<eng::core::ConfigManager>().get<eng::mth::Vec2>("window_viewport_centering");
					centering.y += 0.01;
					m_context.get<eng::core::ConfigManager>().set<eng::mth::Vec2>("window_viewport_centering", centering);
				}
				if (key == eng::sys::Keyboard::Key::DOWN)
				{
					auto centering = m_context.get<eng::core::ConfigManager>().get<eng::mth::Vec2>("window_viewport_centering");
					centering.y -= 0.01;
					m_context.get<eng::core::ConfigManager>().set<eng::mth::Vec2>("window_viewport_centering", centering);
				}

				if (key == eng::sys::Keyboard::Key::MINUS)
				{
					auto v_size = m_context.get<eng::core::ConfigManager>().get<eng::mth::Vec2>("window_viewport_size");
					v_size = v_size/1.1;
					m_context.get<eng::core::ConfigManager>().set<eng::mth::Vec2>("window_viewport_size", v_size);
				}
				if (key == eng::sys::Keyboard::Key::EQUAL)
				{
					auto v_size = m_context.get<eng::core::ConfigManager>().get<eng::mth::Vec2>("window_viewport_size");
					v_size = v_size*1.1;
					m_context.get<eng::core::ConfigManager>().set<eng::mth::Vec2>("window_viewport_size", v_size);
				}
		});
	}

	void onDestroy() override
	{
		m_context.get<eng::core::SignalBus>().unsubscribe(m_viewport_signal_id);
		m_context.get<eng::core::SignalBus>().unsubscribe(m_key_pressed_signal_id);
		m_context.get<eng::core::SignalBus>().unsubscribe(m_key_just_pressed_signal_id);
		m_context.get<eng::gfx::RenderCanvas>().removeObject(*m_ui_scene);
		m_context.get<eng::gfx::RenderCanvas>().removeObject(*m_game_scene);
	}

protected:
	eng::core::SubscriptionId m_viewport_signal_id;
	eng::core::SubscriptionId m_key_pressed_signal_id;
	eng::core::SubscriptionId m_key_just_pressed_signal_id;
};

#endif //ROOT_CLASS_HEADER