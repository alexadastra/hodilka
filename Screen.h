#pragma once
#include <deque>
#include <memory>
#include "Dynamic.h"
#include "Static.h"
#include "notes.h"
//класс игры
class Screen {
public:
	Screen(int height, int width, int spawn_x, int spawn_y, const std::string& file_path);
	void Loop();
	//экзепл€р игрока
	Player self = Player(1, 1);
	//экземпл€р класса уведомлений
	Notes NoteString = Notes("Notes");
	//дек со смарт-указател€ми на объекты, "расположенные" на экране
	std::deque<std::shared_ptr<StaticObject>> objects;

private:
	//путь к файлу с картой
	std::string FilePath;
	//буфер консоли и массив символов дл€ этого буфера
	wchar_t* screen;
	HANDLE hConsole;
	DWORD dwBytesWritten;
	//параметры экрана, ширина и высота
	const int nScreenWidth;
	const int nScreenHeight;
	int nMapWidth = 0; //ширина карты
	int nMapHeight = 0; //высота карты
	int SpawnPointX, SpawnPointY; //точка на экране, где по€витс€ игрок
	//диапазон движений по ос€м, при котором карта не будет двигатьс€
	const int CameraX = 10;
	const int CameraY = 3;
	//сдвиг экрана отн-но карты
	int x_border = 0;
	int y_border = 0;
	//расположение отдельно вз€того объекта
	std::string location;
	//флаги на то, касаетс€ ли экран какого-либо из краЄв карты
	bool TouchUp = true;
	bool TouchLeft = true;
	bool TouchRight = false;
	bool TouchDown = false;

	void ParseMap();
	void SetToCoordinates(char ch, int x, int y);
	void MovePicture(Direction dir);
	void UpdatePicture();

	//шаблонный метод обределени€ типа класса по его обозначению в текстовом файле
	template<typename T>
	std::shared_ptr<StaticObject> ObjectDefine(const std::string& s, const std::vector<T>& key) {
		std::shared_ptr<StaticObject> object;
		if (s == "e") {
			object = std::make_shared<Space>(location);
		}
		else if (s == "w") {
			object = std::make_shared<Wall>(location);
		}
		else if (s == "b") {
			object = std::make_shared<BreakableWall>(location);
		}
		else if (s == "bh") {
			object = std::make_shared<BreakableWallHorizontal>(location);
		}
		else if (s == "k") {
			object = std::make_shared<Key>(key.at(0), key.at(1), location);
		}
		else if (s == "d") {
			object = std::make_shared<Door>(key.at(0), key.at(1), location);
		}
		else if (s == "n") {
			object = std::make_shared<None>(location);
		}
		else if (s == "X") {
			object = std::make_shared<StaticPlayer>(location);
		}
		else if (s == "o") {
			object = std::make_shared<Endgame>(location);
		}
		else {
			location = s;
		}
		return object;
	}
	//шаблонна€ функци€ по считыванию количества и типа объектов
	template<typename stream>
	void ParseObject(stream& input, int& num, std::string& obj,
		std::string& location, std::string& name, std::string& key) {
		input >> num;
		input >> obj;
		if (num == -1) {
			location = obj;
			input >> num;
			input >> obj;
		}
		if (obj == "k" || obj == "d") {
			input >> name;
			input >> key;
		}
	}
};