#pragma once
#include<string>
//класс уведомлений, всплывающих внизу экрана(пополнение инвентаря, взаимодействие с предметами и т.д.)
class Notes {
public:
	Notes(std::string const_note) {
		ConstantNote = const_note;
		CurrentNote = const_note;
		duration = 0.0;
	}
	
	std::string GetNote() const;
	
	void UpdateNote(const float diff);
	
	void SetConstantNote(const std::string& note);
	
	void SetTemporalNote(const std::string& note, float dur);

private:
	//уведомление, появляющееся на данный момент
	std::string CurrentNote;
	//уведомление, которое появляется или может появиться
	std::string ConstantNote;
	//поле, отвечающее за появление временной записи на экране
	float duration;
};