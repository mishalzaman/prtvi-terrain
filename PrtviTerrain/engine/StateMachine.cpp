#include "StateMachine.h"

SM::State::State()
{
}

SM::State::~State()
{
}

void SM::State::ToLoad()
{
	this->state = SM::LOAD;
}

void SM::State::ToUpdate()
{
	this->state = SM::UPDATE;
}

void SM::State::ToRenderGeometry()
{
	this->state = SM::RENDER_GEOMETRY;
}

void SM::State::ToRenderText()
{
	this->state = SM::RENDER_TEXT;
}

void SM::State::ToQuit()
{
	this->state = SM::QUIT;
}

int SM::State::current()
{
	return this->state;
}
