#ifndef SANDBOX_BALLS_CONTROLLER_CLASS_HEADER
#define SANDBOX_BALLS_CONTROLLER_CLASS_HEADER

#include <game/BallsController.hpp>

#include <Engine/Math/Vec2.hpp>

class SandboxBallsController : public BallsController
{
public:
	bool isGameOver() override
	{
		return false;
	}

protected:
	virtual eng::mth::Vec2 computeBallPosition(const eng::mth::Vec2& mouse_pos) override
	{
		return m_context.get<eng::gfx::RenderCanvas>().getActiveCamera().convertWindowPoint(
			mouse_pos,
			m_context.get<eng::sys::Window>()
		);
	}
};

#endif //SANDBOX_BALLS_CONTROLLER_CLASS_HEADER