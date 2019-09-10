#include <iostream>
#include <fstream>
#include "Screen.h"

int main()
{
	setlocale(LC_ALL, "");
	//установка параметов экрана и точки появления
	int nScreenWidth = 120;
	int nScreenHeight = 34;
	int SpawnPointX = 80;
	int SpawnPointY = 29;
	//считывание имени файла с картой
	std::string file_name;
	std::cin >> file_name;
	//проверка на существование файла
	std::ifstream str(file_name + ".txt");
	if (!str) {
		std::cout << "No such file in directory!";
		exit(1);
	}
	//создание экземпляра класса игры
	Screen main(nScreenHeight, nScreenWidth, SpawnPointX, SpawnPointY, file_name + ".txt");
	//запуск цикла
	main.Loop();
	
	return 0;
}