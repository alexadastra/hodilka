#include "notes.h"
//метод, позволяющий обращаться к актуальному уведомлению
std::string Notes::GetNote() const
{
	return CurrentNote;
}
//обновление записи: если время не вышло, он отбавляется на какую-то величину,
//в противном случае, временная надпись сменяется на постоянную
void Notes::UpdateNote(const float diff)
{
	if (duration <= 0.0) {
		CurrentNote = ConstantNote;
	}
	else {
		duration -= diff;
	}
}
//установка новой "постоянной записи"
void Notes::SetConstantNote(const std::string& note)
{
	ConstantNote = note;
}
//установка записи, появляющейся на какое-то время
void Notes::SetTemporalNote(const std::string& note, float dur)
{
	CurrentNote = note;
	duration = dur;
}