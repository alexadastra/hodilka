#include "Screen.h"
#include <chrono>
#include <fstream>
#include <string>
#include <iterator>
#include <thread>

Screen::Screen(int height, int width, int spawn_x, int spawn_y, const std::string& file_path)
	: nScreenHeight(height), nScreenWidth(width), SpawnPointX(spawn_x), SpawnPointY(spawn_y), FilePath(file_path) {
	screen = new wchar_t[nScreenWidth * (nScreenHeight + 1)];
	hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	dwBytesWritten = 0;
}
//цикл игры
void Screen::Loop() {
	//установка времени, чтобы скорость игрока не менялась в зависимости от нагруженности системы
	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();
	ParseMap();
	while (true) {
		//определения времени на предыдущей итерации
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();
		//освобождаем место игрока на случай, если он сдвинется
		SetToCoordinates('e', self.GetX(), self.GetY());
		//анализ команд

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) { //если нажата клавиша(wasd для ходьбы)
			StaticObject obj = *objects[(self.GetY() - 1) * nScreenWidth + self.GetX()];
			if (self.Move(Direction::UPWARD, 8.0f * fElapsedTime, obj.GetWalkableByPlayer())) {
				if (self.GetY() < nScreenHeight / 2 - CameraY && !(TouchUp)) {
					MovePicture(Direction::UPWARD);
					self.Move(Direction::DOWNWARD, 8.0f * fElapsedTime, true);
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
				}
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			//выбирается объект на который игрок собирается походить
			StaticObject obj = *objects[(self.GetY() + 1) * nScreenWidth + self.GetX()];
			if (self.Move(Direction::DOWNWARD, 8.0f * fElapsedTime, obj.GetWalkableByPlayer())) {
				if (self.GetY() > nScreenHeight / 2 + CameraY && !(TouchDown)) {
					MovePicture(Direction::DOWNWARD);
					self.Move(Direction::UPWARD, 8.0f * fElapsedTime, true);
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
				}
			}
		}

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
			StaticObject obj = *objects[self.GetY() * nScreenWidth + self.GetX() - 1];
			//если это возможно и игрок движется, надо проверить, не нужно ли сдвигать изображение
			if (self.Move(Direction::LEFT, 24.0f * fElapsedTime, obj.GetWalkableByPlayer())) {
				if (self.GetX() < nScreenWidth / 2 - CameraX && !(TouchLeft)) {
					MovePicture(Direction::LEFT);
					self.Move(Direction::RIGHT, 24.0f * fElapsedTime, true);
				}
			}
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
			StaticObject obj = *objects[self.GetY() * nScreenWidth + self.GetX() + 1];
			if (self.Move(Direction::RIGHT, 24.0f * fElapsedTime, obj.GetWalkableByPlayer())) {
				if (self.GetX() > nScreenWidth / 2 + CameraX && !(TouchRight)) {
					MovePicture(Direction::RIGHT); // экран сдвигается в определённую сторону
					self.Move(Direction::LEFT, 24.0f * fElapsedTime, true);
				}
			}
		}
		//установка игрока на новые координаты
		SetToCoordinates('X', self.GetX(), self.GetY());
		//постоянным уведомлением становится местоположение игрока
		NoteString.SetConstantNote(objects[nScreenWidth * self.GetY() + self.GetX()]->GetLocation());
		//при нажатии на стрелки, отвечающие за взаимодействие, проверяется, появилась ли новая запись
		std::string announcement = "";
		if (GetAsyncKeyState((unsigned short)VK_UP) & 0x8000) {
			announcement = self.Use(objects[nScreenWidth * (self.GetY() - 1) + self.GetX()]);
		}
		if (GetAsyncKeyState((unsigned short)VK_DOWN) & 0x8000) {
			announcement = self.Use(objects[nScreenWidth * (self.GetY() + 1) + self.GetX()]);
		}
		if (GetAsyncKeyState((unsigned short)VK_LEFT) & 0x8000) {
			announcement = self.Use(objects[nScreenWidth * self.GetY() + self.GetX() - 1]);
		}
		if (GetAsyncKeyState((unsigned short)VK_RIGHT) & 0x8000) {
			announcement = self.Use(objects[nScreenWidth * self.GetY() + self.GetX() + 1]);
		}
		if (announcement != "") {
			NoteString.SetTemporalNote(announcement, 20.0);
		}
		NoteString.UpdateNote(0.1);

		if (GetAsyncKeyState((unsigned short)VK_ESCAPE) & 0x8000) {
			break;
		}
		UpdatePicture();
		//вывод буфера в консоль
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * (nScreenHeight + 1), { 0,0 }, &dwBytesWritten);
	}
}
//изменение дека по конкретным координатам
void Screen::SetToCoordinates(char ch, int x, int y) {
	std::vector<std::string> v(0);
	std::shared_ptr<StaticObject> object = ObjectDefine(std::string(1, ch), v);
	objects[y * nScreenWidth + x] = object;
}
//начальное считывание карты
void Screen::ParseMap() {
	std::ifstream input(FilePath);
	int num;
	std::string obj, name, key;
	//считывание параметров карты, широты и высоты
	input >> nMapWidth;
	input >> nMapHeight;
	std::shared_ptr<StaticObject> object;
	//определение сдвига по Оу:
	//Если при бездействии игрок окажется во нижней части экрана или вообще не виден
	//считывать ненужные строки, пока игрок не окажется в центре или пока не будет 
	//достигнута нижняя граница.
	if (SpawnPointY - nScreenHeight / 2 > 0) {
		while (SpawnPointY - nScreenHeight / 2 > y_border && !TouchDown) {
			++y_border;
			getline(input, obj);
			if (y_border + nScreenHeight >= nMapHeight)
				TouchDown = true;
		}
		TouchUp = false;
	}
	else TouchUp = true;
	//Аналогично сдвигаем правую границу
	if (SpawnPointX - nScreenWidth / 2 > 0) {
		while (SpawnPointX - nScreenWidth / 2 > x_border && !TouchRight) {
			++x_border;
			if (x_border + nScreenWidth >= nMapWidth)
				TouchRight = true;
		}
		TouchLeft = false;
	}
	else TouchLeft = true;

	//могут возникнуть исключения, когда необходимо проверить флаги повторно
	if (y_border + nScreenHeight >= nMapHeight)
		TouchDown = true;
	if (x_border + nScreenWidth >= nMapWidth)
		TouchRight = true;
	//считывание пар "число-символ"
	for (int i = 0; i < nScreenHeight; ++i) {//проходим по всей высоте
		std::string str;
		getline(input, str);
		std::istringstream s(str);
		int j = 0;
		while (j < nScreenWidth + x_border) { //проходим через всю ширину и сдвиг по Ох
			//считываем объект и их число по горизонтали
			input >> num;
			input >> obj;
			if (num == -1) { //указатель на местоположение как бы пропускается
				location = obj;
			}
			else {
				if (obj == "k" || obj == "d") { //для ключей и дверей так же нужно считать название и значение
					input >> name;
					input >> key;
				}
				int k = 0;
				while (k < num && j < nScreenWidth + x_border) {
					if (j >= x_border) {//если сдвиг пройден, добавляем считанный объект в дек
						std::vector<std::string> v = { name, key };
						object = ObjectDefine(obj, v);

						objects.push_back(object);

					}
					++k;
					++j;
				}
			}
		}
	}
	UpdatePicture(); //совмещение буфера консоли и дека объектов
	self = Player(SpawnPointX - x_border, SpawnPointY - y_border); //переопределение координат игрока с учётом сдвига
}

void Screen::MovePicture(Direction dir) {
	std::string str, obj, name, key;
	std::shared_ptr<StaticObject> object;
	std::vector<std::shared_ptr<StaticObject>> objects_buf;
	int num = 0, j = 0;
	//создаём поток для считывания и пропускаем параметры 
	std::ifstream input(FilePath);
	getline(input, str);
	getline(input, str);

	switch (dir)
	{
	case Direction::UPWARD: {
		//пропускаем все ненужные строки в соответствии со сдвигом
		for (int i = 0; i < y_border; ++i)
			getline(input, str);
		--y_border;
		//создаём дополнительный поток и пропускаем все ненужные элементы
		std::istringstream s(str);
		int i = 0;
		while (i < x_border) {
			ParseObject(s, num, obj, location, name, key);
			while (num > 0 && i < x_border) {
				--num;
				++i;
			}
		}
		//стираем последние ScreenWidth элементов из дека
		objects.erase(objects.end() - nScreenWidth, objects.end());
		//добавляем новые в буферный вектор
		while (j < nScreenWidth) {
			if (num == 0) { //если были считаны все num элементов, необходимо считать новый
				ParseObject(s, num, obj, location, name, key);
			}
			while (num > 0 && j < nScreenWidth) {
				std::vector<std::string> v = { name, key };
				object = ObjectDefine(obj, v);
				objects_buf.push_back(object);
				++j;
				--num;
			}
		}
		//итерируемся с кронца вектора и записываем объекты в дек
		for (auto i = objects_buf.rbegin(); i < objects_buf.rend(); ++i) {
			objects.push_front(*i);
		}
		//определяем флаги
		if (y_border == 0)
			TouchUp = true;
		else TouchUp = false;
		TouchDown = false;

		break;
	}
	case Direction::DOWNWARD: {
		++y_border;
		//пропускаем все ненужные строки в соответствии со сдвигом
		for (int i = 0; i < nScreenHeight + y_border; ++i)
			getline(input, str);
		//создаём дополнительный поток и пропускаем все ненужные элементы
		std::istringstream s(str);
		int i = 0;
		while (i < x_border) {
			ParseObject(s, num, obj, location, name, key);
			while (num > 0 && i < x_border) {
				--num;
				++i;
			}
		}
		//стираем первые ScreenWidth элементов из дека
		objects.erase(objects.begin(), objects.begin() + nScreenWidth);
		//добавляем новые сразу в вектор
		while (j < nScreenWidth) {
			if (num == 0) {
				ParseObject(s, num, obj, location, name, key);
			}
			while (num > 0 && j < nScreenWidth) {
				std::vector<std::string> v = { name, key };
				object = ObjectDefine(obj, v);
				objects.push_back(object);
				++j;
				--num;
			}
		}

		if (y_border + nScreenHeight == nMapHeight)
			TouchDown = true;
		else TouchDown = false;
		TouchUp = false;
		
		break;
	}
	case Direction::LEFT: {
		//пропускаем все ненужные строки в соответствии со сдвигом
		for (int i = 0; i < y_border; ++i)
			getline(input, str);
		//удаляем каждый последний в ряду элемент
		for (int i = 0; i < nScreenHeight; ++i) {
			auto it = objects.begin() + (nScreenWidth - 1) * (i + 1);
			objects.erase(it);
		}
		//проходимся по всей высоте
		for (int i = 0; i < nScreenHeight; ++i) {
			j = 0;
			while (j < x_border) { //считываем, пока не дойдём до левой границы
				ParseObject(input, num, obj, location, name, key);
				while (num > 0 && j < x_border) {
					--num;
					++j;
				}
			}
			//добавляем считанный элемент и переходим на новую строку
			std::vector<std::string> v = { name, key };
			object = ObjectDefine(obj, v);
			objects_buf.push_back(object);
			getline(input, obj);
		}
		//считанные объекты вставим перед первыми в ряду 
		for (int i = 0; i < nScreenHeight; ++i) {
			auto it = objects.begin() + nScreenWidth * i;
			objects.insert(it, objects_buf[i]);
		}
		--x_border;
		if (x_border == 0)
			TouchLeft = true;
		else TouchLeft = false;
		TouchRight = false;
		break;
	}
	case Direction::RIGHT: {
		for (int i = 0; i < y_border; ++i)
			getline(input, str);
		//удаляем каждый последний в ряду элемент
		for (int i = 0; i < nScreenHeight; ++i) {
			auto it = objects.begin() + (nScreenWidth - 1) * i;
			objects.erase(it);
		}
		++x_border;
		for (int i = 0; i < nScreenHeight; ++i) {
			j = 0;
			while (j < x_border + nScreenWidth - 1) { //считываем, пока не дойдём до правой границы
				ParseObject(input, num, obj, location, name, key);
				while (num > 0 && j < x_border + nScreenWidth - 1) {
					--num;
					++j;
				}
			}
			//необходимо проверить, остался ли считанный элемент
			if (num == 0 || j < x_border + nScreenWidth - 1) {
				ParseObject(input, num, obj, location, name, key);
			}
			//добавляем считанный элемент и переходим на новую строку
			std::vector<std::string> v = { name, key };
			object = ObjectDefine(obj, v);
			objects_buf.push_back(object);
			getline(input, obj);
		}
		//считанные объекты вставим перед последними в ряду 
		for (int i = 0; i < nScreenHeight; ++i) {
			auto it = objects.begin() + nScreenWidth * (i + 1) - 1;
			objects.insert(it, objects_buf[i]);
		}
		//переопределяем флаги
		if (x_border + nScreenWidth == nMapWidth)
			TouchRight = true;
		else TouchRight = false;
		TouchLeft = false;
		break;
	}
	}
}
//обновление буфера в соответствии со всеми изменениями в деке
void Screen::UpdatePicture() {
	for (int i = 0; i < nScreenHeight * nScreenWidth; ++i)
		screen[i] = objects[i]->GetPol();
	std::string s = NoteString.GetNote();
	for (int i = 0; i < nScreenWidth; ++i) {
		if (i < static_cast<int>(s.size()))
			screen[nScreenHeight * nScreenWidth + i] = s[i];
		else 
			screen[nScreenHeight * nScreenWidth + i] = ' ';
	}
}