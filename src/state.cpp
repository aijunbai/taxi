/*
 * state.cpp
 *
 *  Created on: Nov 28, 2011
 *      Author: baj
 */

#include "state.h"

State::State(const int & a, const int & b, const int & c, const int & d): tuple<int, int, int, int>(a, b, c, d)
{

}

int State::x() const
{
  return get<0>(*this);
}

int State::y() const
{
	return get<1>(*this);
}

int State::passenger() const
{
	return get<2>(*this);
}

int State::destination() const
{
	return get<3>(*this);
}

int& State::x()
{
	return get<0>(*this);
}

int& State::y() {
	return get<1>(*this);
}

int& State::passenger()
{
	return get<2>(*this);
}

int& State::destination()
{
	return get<3>(*this);
}

std::string State::str() const
{
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

bool State::terminate() const
{
	return passenger() == destination();
}
