#ifndef BALLS_COLLECTION_CLASS_HEADER
#define BALLS_COLLECTION_CLASS_HEADER

#include <vector>

class Ball;

struct BallsCollection
{
	struct Pair
	{
		Ball* first;
		Ball* second;

		Pair(Ball* f, Ball* s): first(f), second(s) {}
	};

	std::vector<Pair> balls;

	void removeCopies()
	{
		if (balls.size() < 2) return;

		for (unsigned int i = 0; i < balls.size() - 1; i++)
			for (unsigned int j = i + 1; j < balls.size(); j++)
				if ((balls[i].first  == balls[j].first) || (balls[i].first  == balls[j].second) ||
					(balls[i].second == balls[j].first) || (balls[i].second == balls[j].second))
						balls.erase(balls.begin() + j--);
	}
};

#endif //BALLS_COLLECTION_CLASS_HEADER