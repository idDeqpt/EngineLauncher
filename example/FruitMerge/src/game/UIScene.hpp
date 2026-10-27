#ifndef UI_SCENE_CLASS_HEADER
#define UI_SCENE_CLASS_HEADER

#include <SceneLayer.hpp>

#include <Engine/Core/ConfigManager.hpp>
#include <Engine/Core/Delta.hpp>
#include <Engine/Graphics/2D/Camera2D.hpp>
#include <Engine/Graphics/2D/Text2D.hpp>
#include <Engine/Graphics/Font.hpp>
#include <Engine/Graphics/2D/RenderCanvas.hpp>
#include <Engine/Math/Vec2.hpp>

class UIScene : public SceneLayer
{
protected:
	eng::gfx::Camera2D* m_camera2d = nullptr;
	eng::gfx::Text2D* m_game_over_text = nullptr;
	eng::gfx::Text2D* m_frametime_text = nullptr;
	eng::gfx::Text2D* m_game_mode_text = nullptr;
	eng::gfx::Text2D* m_tip_free_mode_text    = nullptr;
	eng::gfx::Text2D* m_tip_classic_mode_text = nullptr;

public:
	void onBuild() override
	{
		m_camera2d = addChild<eng::gfx::Camera2D>("camera");
		m_game_over_text = addChild<eng::gfx::Text2D>("text_game_over");
		m_frametime_text = addChild<eng::gfx::Text2D>("text_frametime");
		m_game_mode_text = addChild<eng::gfx::Text2D>("text_game_mode");
		m_tip_free_mode_text    = addChild<eng::gfx::Text2D>("tip_free_mode");
		m_tip_classic_mode_text = addChild<eng::gfx::Text2D>("tip_classic_mode");
	}

	void onSetup() override
	{
		SceneLayer::onSetup();

		eng::mth::Vec2 v_size = m_context.get<eng::core::ConfigManager>().get<eng::mth::Vec2>("window_viewport_size");
		m_camera2d->setRect(0, v_size.x, 0, v_size.y);
		m_context.get<eng::gfx::RenderCanvas>().setActiveCamera(*m_camera2d);

		m_context.get<eng::gfx::RenderCanvas>().addObject(*m_game_over_text);
		m_game_over_text->setFont(*m_font);
		m_game_over_text->setCharacterSize(24);
		m_game_over_text->setString("Game over");
		m_game_over_text->setPosition(eng::mth::Vec2(v_size.x/2, 200));
		m_game_over_text->setOrigin(eng::mth::Vec2(100, 0));
		m_game_over_text->setVisible(false);
		m_game_over_text->setColor(eng::gfx::Color(255, 0, 0));

		m_context.get<eng::gfx::RenderCanvas>().addObject(*m_frametime_text);
		m_frametime_text->setFont(*m_font);
		m_frametime_text->setCharacterSize(24);
		m_frametime_text->setPosition(eng::mth::Vec2(10, 0));

		m_context.get<eng::gfx::RenderCanvas>().addObject(*m_game_mode_text);
		m_game_mode_text->setFont(*m_font);
		m_game_mode_text->setCharacterSize(16);
		m_game_mode_text->setPosition(eng::mth::Vec2(10, 30));

		m_context.get<eng::gfx::RenderCanvas>().addObject(*m_tip_free_mode_text);
		m_tip_free_mode_text->setFont(*m_font);
		m_tip_free_mode_text->setCharacterSize(12);
		m_tip_free_mode_text->setPosition(eng::mth::Vec2(10, 50));
		m_tip_free_mode_text->setString("Press (R + F) for restart in free mode");

		m_context.get<eng::gfx::RenderCanvas>().addObject(*m_tip_classic_mode_text);
		m_tip_classic_mode_text->setFont(*m_font);
		m_tip_classic_mode_text->setCharacterSize(12);
		m_tip_classic_mode_text->setPosition(eng::mth::Vec2(10, 70));
		m_tip_classic_mode_text->setString("Press (R + C) for restart in classic mode");

		m_camera_signal_id = m_context.get<eng::core::SignalBus>().subscribe("on_change_config_window_viewport_size",
			[this](eng::mth::Vec2 size){
				m_camera2d->setRect(0, size.x, 0, size.y);
		});

		m_game_over_signal_id = m_context.get<eng::core::SignalBus>().subscribe("game_over",
			[this](){
				m_game_over_text->setVisible(true);
		});

		m_game_restart_signal_id = m_context.get<eng::core::SignalBus>().subscribe("game_restart",
			[this](){
				m_game_over_text->setVisible(false);
		});

		m_game_mode_changed_signal_id = m_context.get<eng::core::SignalBus>().subscribe("game_mode_changed",
			[this](std::string mode){
				m_game_mode_text->setString("Game mode: " + mode);
		});
	}

	void onDestroy() override
	{
		SceneLayer::onDestroy();
		m_context.get<eng::core::SignalBus>().unsubscribe(m_camera_signal_id);
		m_context.get<eng::core::SignalBus>().unsubscribe(m_game_over_signal_id);
		m_context.get<eng::core::SignalBus>().unsubscribe(m_game_restart_signal_id);
		m_context.get<eng::core::SignalBus>().unsubscribe(m_game_mode_changed_signal_id);
		m_context.get<eng::gfx::RenderCanvas>().removeObject(*m_game_over_text);
		m_context.get<eng::gfx::RenderCanvas>().removeObject(*m_frametime_text);
		m_context.get<eng::gfx::RenderCanvas>().removeObject(*m_game_mode_text);
		m_context.get<eng::gfx::RenderCanvas>().removeObject(*m_tip_free_mode_text);
		m_context.get<eng::gfx::RenderCanvas>().removeObject(*m_tip_classic_mode_text);
	}

	void onUpdate(const eng::core::Delta& delta) override
	{
		auto t_ft = static_cast<eng::gfx::Text2D*>(getChildByName("text_frametime"));
		if (t_ft) t_ft->setString(std::to_string(delta.frame));

		SceneLayer::onUpdate(delta);
	}

	void setFont(eng::gfx::Font& font)
	{
		m_font = &font;
	}

protected:
	eng::gfx::Font* m_font = nullptr;
	eng::core::SubscriptionId m_camera_signal_id;
	eng::core::SubscriptionId m_game_over_signal_id;
	eng::core::SubscriptionId m_game_restart_signal_id;
	eng::core::SubscriptionId m_game_mode_changed_signal_id;
};

#endif //UI_SCENE_CLASS_HEADER