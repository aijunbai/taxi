/*
 * state.cpp
 *
 *  Created on: Nov 28, 2011
 *      Author: baj
 */

#include "state.h"
#include "taxi.h"

State::State(int a, int b, int c, int d):
    tuple<int, int, int, int>(a, b, c, d)
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

bool State::terminated() const
{
	return unloaded();
}

ostream &operator<<(ostream &os, const State &o) {
  int n = 5; //TaxiEnv::SIZE;
  char t[] = {'Y', 'R', 'B', 'G', 't'};

  os << "\n";
  os << std::make_tuple(o.x(), o.y(), t[o.passenger()], t[o.destination()]) << "\n";

  vector<pair<int, int>> pos = {{0, 0}, {0, n - 1}, {n - 2, 0}, {n - 1, n - 1}};
  if (o.passenger() < 4) t[o.passenger()] = 'p';

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      int x = j;
      int y = 4 - i;

      if (o.x() == x && o.y() == y) {
        os << ((o.passenger() == 4)? '@': '#');
      }
      else if (find(pos.begin(), pos.end(), make_pair(x, y)) != pos.end()) {
        int idx = find(pos.begin(), pos.end(),  make_pair(x, y)) - pos.begin();
        os << t[idx];
      }
      else {
        os << '.';
      }
    }
    os << '\n';
  }

  return os;
}

bool State::unloaded() const {
  return passenger() == destination();
}

bool State::loaded() const {
  return passenger() == TaxiEnv::Model::ins().inTaxi();
}

Position State::taxiPosition() const { return Position(x(), y()); }

Position Position::normalize() {
  return Position(minmax(0, x, TaxiEnv::SIZE - 1), minmax(0, y, TaxiEnv::SIZE - 1));
}