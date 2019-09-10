#include "Dynamic.h"

Player::Player(int x, int y) {
	x_ = x;
	y_ = y;
}

void Player::Move(Direction dir, float diff) {
	switch (dir)
	{
	case Direction::UPWARD:
		//if (y_ > 1)
			y_ -= diff;
		break;
	case Direction::DOWNWARD:
		//if (y_ < maxy)
			y_ += diff;
		break;
	case Direction::LEFT:
		//if (x_ > 1)
			x_ -= diff;
		break;
	case Direction::RIGHT:
		//if (x_ < maxx)
			x_ += diff;
		break;
	}
}

int Player::GetX() const {
	return x_;
}

int Player::GetY() const {
	return y_;
}