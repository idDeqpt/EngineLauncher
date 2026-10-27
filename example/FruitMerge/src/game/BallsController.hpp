#ifndef BALLS_CONTROLLER_CLASS_HEADER
#define BALLS_CONTROLLER_CLASS_HEADER

#include <game/Ball.hpp>
#include <game/BallsCollection.hpp>

#include <Engine/Core/Node.hpp>
#include <Engine/Core/SignalBus.hpp>
#include <Engine/Core/Delta.hpp>

#include <Engine/System/EventManager.hpp>

#include <algorithm>
#include <random>

class BallsController : public eng::core::Node
{
protected:
	Ball* m_ball_image = nullptr;

public:
	void onBuild() override
	{
		m_ball_image = addChild<Ball>("ball_image", 0);
	}

	void onSetup() override
	{
		m_next_ball_level = 1;
		srand(0);

		m_ball_signal_id = m_context.get<eng::core::SignalBus>().subscribe("balls_collided",
			[this](Ball* left, Ball* right) {
				m_collection.balls.push_back(BallsCollection::Pair(left, right));
		});

		m_mouse_signal_id = m_context.get<eng::core::SignalBus>().subscribe("mouse_just_clicked",
			[this](eng::sys::Mouse::Button button) {
				if (button == eng::sys::Mouse::LEFT)
				{
					auto ball = addChild<Ball>("ball", m_next_ball_level);
					ball->setPosition(computeBallPosition(m_context.get<eng::sys::EventManager>().getMouse().getPosition()));
					computeNextBallLevel();
				}
		});

		m_mouse_moved_signal_id = m_context.get<eng::core::SignalBus>().subscribe("mouse_moved",
			[this](const eng::mth::Vec2& pos, const eng::mth::Vec2& delta) {
				m_ball_image->setPosition(computeBallPosition(pos));
		});

		m_game_over_signal_id = m_context.get<eng::core::SignalBus>().subscribe("game_over",
			[this](){
				m_context.get<eng::core::SignalBus>().unsubscribe(m_mouse_signal_id);
				m_context.get<eng::core::SignalBus>().unsubscribe(m_mouse_moved_signal_id);
				removeChild(getChildByName("ball_image"));
		});

		m_ball_fall_signal_id = m_context.get<eng::core::SignalBus>().subscribe("ball_fall",
			[this](Ball* ball){
				m_balls_for_removing.push_back(ball);
		});
	}

	void onReady() override
	{
		computeNextBallLevel();
	}

	void onDestroy() override
	{
		m_context.get<eng::core::SignalBus>().unsubscribe(m_ball_signal_id);
		m_context.get<eng::core::SignalBus>().unsubscribe(m_mouse_signal_id);
		m_context.get<eng::core::SignalBus>().unsubscribe(m_mouse_moved_signal_id);
		m_context.get<eng::core::SignalBus>().unsubscribe(m_game_over_signal_id);
		m_context.get<eng::core::SignalBus>().unsubscribe(m_ball_fall_signal_id);
	}

	void onUpdate(const eng::core::Delta& delta) override
	{
		for (auto& ball : m_balls_for_removing)
		{
			m_collection.balls.erase(
				std::remove_if(m_collection.balls.begin(), m_collection.balls.end(),
					[ball](const BallsCollection::Pair& p){
						return p.first == ball || p.second == ball;
					}),
				m_collection.balls.end()
			);
			removeChild(ball);
		}
		m_balls_for_removing.clear();

		if (m_collection.balls.empty()) return;

		Ball* a = m_collection.balls.front().first;
		Ball* b = m_collection.balls.front().second;
		m_collection.balls.clear();

		if (!a || !b) return;
		if (a->isDestroyed() || b->isDestroyed()) return;
		mergeBalls(*a, *b);
	}

	virtual bool isGameOver() = 0;

protected:
	BallsCollection m_collection;
	unsigned int m_next_ball_level;
	eng::core::SubscriptionId m_ball_signal_id;
	eng::core::SubscriptionId m_mouse_signal_id;
	eng::core::SubscriptionId m_mouse_moved_signal_id;
	eng::core::SubscriptionId m_game_over_signal_id;
	eng::core::SubscriptionId m_ball_fall_signal_id;
	std::vector<Ball*> m_balls_for_removing;

	void mergeBalls(Ball& first, Ball& second)
	{
		if (first.getLevel() != second.getLevel()) return;

		auto ball = addChild<Ball>("ball", first.getLevel() + 1);
		ball->setPosition((first.getPosition() + second.getPosition())*0.5);
		removeChild(&first);
		removeChild(&second);
	}

	void computeNextBallLevel()
	{
		m_next_ball_level = (rand()%2 + 1);
		m_ball_image->setLevel(m_next_ball_level);
	}

	virtual eng::mth::Vec2 computeBallPosition(const eng::mth::Vec2& mouse_pos) = 0;
};

#endif //BALLS_CONTROLLER_CLASS_HEADER