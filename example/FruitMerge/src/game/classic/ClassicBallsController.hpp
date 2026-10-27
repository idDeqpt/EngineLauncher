#ifndef CLASSIC_BALLS_CONTROLLER_CLASS_HEADER
#define CLASSIC_BALLS_CONTROLLER_CLASS_HEADER

#include <game/BallsController.hpp>
#include <game/BallsCollection.hpp>

#include <Engine/System/EventManager.hpp>
#include <Engine/Math/Vec2.hpp>

class ClassicBallsController : public BallsController
{
public:
	ClassicBallsController(eng::mth::Vec2 left_bound, eng::mth::Vec2 right_bound):
		m_left_bound(left_bound),
		m_right_bound(right_bound),
		m_game_over(false) {}

	void onSetup() override
	{
		BallsController::onSetup();

		m_ball_exit_signal_id = m_context.get<eng::core::SignalBus>().subscribe("ball_exit",
			[this]() {
				m_game_over = true;
		});
	}

	void onDestroy() override
	{
		BallsController::onDestroy();
		m_context.get<eng::core::SignalBus>().unsubscribe(m_ball_exit_signal_id);
	}


	bool isGameOver() override
	{
		return m_game_over;
	}

protected:
	eng::mth::Vec2 m_left_bound;
	eng::mth::Vec2 m_right_bound;
	bool m_game_over;
	eng::core::SubscriptionId m_ball_exit_signal_id;

	virtual eng::mth::Vec2 computeBallPosition(const eng::mth::Vec2& mouse_pos) override
	{
		eng::mth::Vec2 mouse_point = m_context.get<eng::gfx::RenderCanvas>().getActiveCamera().convertWindowPoint(
			mouse_pos,
			m_context.get<eng::sys::Window>()
		);
		eng::mth::Vec2 lr = m_left_bound - m_right_bound;
		eng::mth::Vec2 lp = m_left_bound - mouse_point;
		float t = lp.dot(lr)/lr.dot(lr);
		if (t < 0)      t = 0;
		else if (t > 1) t = 1;
		eng::mth::Vec2 res = m_left_bound + (-lr)*t;
		return res;
	}
};

#endif //CLASSIC_BALLS_CONTROLLER_CLASS_HEADER