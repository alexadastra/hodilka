#include "Static.h"

StaticObject::StaticObject() : WalkableByPlayer(false), pol(' '), location("???"){}
//виртуальный метод для взаимодействия игрока
std::string StaticObject::Used(std::shared_ptr<StaticObject>& object) { return ""; }
//обращения к приватным полям
char StaticObject::GetPol() const {
	return pol;
}

bool StaticObject::GetWalkableByPlayer() const {
	return WalkableByPlayer;
}

std::string StaticObject::GetLocation() const {
	return location;
}
//конструкторы различных объектов
Wall::Wall(const std::string& s) {
	pol = '#';
}

Space::Space(const std::string& s) {
	WalkableByPlayer = true;
	pol = '.';
	location = s;
}

None::None(const std::string& s) {
	location = s;
	pol = ' ';
}

StaticPlayer::StaticPlayer(const std::string& s) {
	location = s;
	pol = 'X';
}

BreakableWall::BreakableWall(const std::string& s) {
	location = s;
	pol = '|';
}

BreakableWall::BreakableWall() {
	pol = '|';
}
//при взаимодействии со стенами они ломаются
std::string BreakableWall::Used(std::shared_ptr<StaticObject>& object){
	object = std::make_shared<Space>(location);
	return "";
}

BreakableWallHorizontal::BreakableWallHorizontal(const std::string& s){
	location = s;
	pol = '_';
}

Key::Key(const std::string& a, const std::string& b, const std::string& s) {
	WalkableByPlayer = false;
	pol = '+';
	name = a;
	value = b;
	location = s;
}
//при взаимодействии с ключом он попадает в инвентарь и выводится сообщение
std::string Key::Used(std::shared_ptr<StaticObject>& object){
	std::string s = value;
	object = std::make_shared<Space>(location);
	return "Key " + s;
}

Door::Door(const std::string& a, const std::string& b, const std::string& s) {
	pol = '/';
	name = a;
	value = b;
	location = s;
}
//при взаимодействии с дверью она откроется, только если в инвентаре (эти самые попытки) найдётся подходящий ключ
std::string Door::Used(std::shared_ptr<StaticObject>& object) {
	for (auto i : object->tries) {
		if (i == value) {
			std::string buf = name;
			object = std::make_shared<Space>(location);
			return "Door " + buf + " is open!";
		}
	}
	return "Door " + name + " is locked! You need " + value + " key to open it!";
}

Endgame::Endgame(const std::string& s) {
	pol = 'o';
	location = s;
}
//при взаимодействии "окончателем игры" выводится сообщениепd
std::string Endgame::Used(std::shared_ptr<StaticObject>& object) {
	return "Game over!";
}
