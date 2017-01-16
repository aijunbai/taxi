/*
 * state.cpp
 *
 *  Created on: Nov 28, 2011
 *      Author: baj
 */

#include "state.h"

State::State(const int & a, const int & b, const int & c, const int & d): boost::tuples::tuple<int, int, int, int>(a, b, c, d)
{

}

int State::x() const
{
	return this->get<0>();
}

int State::y() const
{
	return this->get<1>();
}

int State::passenger() const
{
	return this->get<2>();
}

int State::destination() const
{
	return this->get<3>();
}

int& State::x()
{
	return this->get<0>();
}

int& State::y() {
	return this->get<1>();
}

int& State::passenger()
{
	return this->get<2>();
}

int& State::destination()
{
	return this->get<3>();
}

std::string State::str() const
{
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

bool State::absorbing() const
{
	return passenger() == destination();
}
