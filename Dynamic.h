#pragma once
#include <windows.h>
#include "Static.h"
//направления движения и взаимодействия
enum class Direction {
	UPWARD,
	DOWNWARD,
	LEFT,
	RIGHT,
};
//инвентарь(не богато, но может появиться ещё)
enum class Inventory {
	KEY,
};
//класс игрока: обработка его действий
class Player {
public:
	Player(int x, int y);
	//передвижение по карте
	bool Move(Direction dir, float diff, bool walkable);
	//взаимодействие с объектами
	std::string Use(std::shared_ptr<StaticObject>& object);
	
	int GetX() const;
	int GetY() const;
private: 
	//инвентарь
	std::vector<std::pair<Inventory, std::string>> inventory;
	//координаты
	float x_;
	float y_;
};