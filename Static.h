#pragma once
#include <deque>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <utility>

//класс - предок для всех объектов, отображаемых на экране
class StaticObject {
public:
	StaticObject();
	//виртуальный метод для взаимодействия игрока
	virtual std::string Used(std::shared_ptr<StaticObject>& object);
	//"попытки" (необходимо для взаимодействия игрока с дверьми)
	std::vector<std::string> tries;
	//обращения к приватным полям
	virtual char GetPol() const;
	virtual bool GetWalkableByPlayer() const;
	virtual std::string GetLocation() const;
protected:
	//символ объекта, в виде которого он будет отбражаться
	char pol;
	//метка, можно ли проходить по объекту без взаимодействия (только wasd)
	bool WalkableByPlayer;
	//отметка местоположения
	std::string location;
};

//стена
class Wall : public StaticObject {
public:
	Wall(const std::string& s);
};
//свободное поле
class Space : public StaticObject {
public:
	Space(const std::string& s);
};
//пустое пространство(невидимая стена)
class None : public StaticObject {
public:
	None(const std::string& s);
};
//отображение игрока
class StaticPlayer : public StaticObject {
public:
	StaticPlayer(const std::string& s);
};
//ломаемая стена
class BreakableWall : public StaticObject {
public:
	BreakableWall(const std::string& s);
	BreakableWall();
	virtual std::string Used(std::shared_ptr<StaticObject>& object) override;
};
//горизонтальная ломаемая стена (отличается только символом)
class BreakableWallHorizontal : public BreakableWall {
public:
	BreakableWallHorizontal(const std::string& s);
};
//ключ
class Key : public StaticObject {
public:
	Key(const std::string& a, const std::string& b, const std::string& s);
	virtual std::string Used(std::shared_ptr<StaticObject>& object) override;
private:
	std::string value; //непосредственно ключ
	std::string name; //имя ключа
};
//дверь
class Door : public StaticObject {
public:
	Door(const std::string& a, const std::string& b, const std::string& s);
	virtual std::string Used(std::shared_ptr<StaticObject>& object) override;
private:
	std::string value; //"замок" двери, открываемый при совпадении с ключом
	std::string name; //имя двери
};
//завершение игры
class Endgame : public StaticObject {
public:
	Endgame(const std::string& s);
	virtual std::string Used(std::shared_ptr<StaticObject>& object) override;
};