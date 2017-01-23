/*
 * state.cpp
 *
 *  Created on: Nov 28, 2011
 *      Author: baj
 */

#include "state.h"

State::State(int a, int b, int c, int d, int e):
    tuple<int, int, int, int, int>(a, b, c, d, e)
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

int State::fuel() const
{
  return get<4>(*this);
};

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

int& State::fuel()
{
  return get<4>(*this);
};

std::string State::str() const
{
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

bool State::terminated() const
{
	return passenger() == destination() || fuel() <= 0;
}

ostream &operator<<(ostream &os, const State &o) {
  int n = 5; //TaxiEnv::SIZE;
  char t[] = {'Y', 'R', 'B', 'G', 't'};

  os << "\n";
  os << std::make_tuple(o.x(), o.y(), t[o.passenger()], t[o.destination()], o.fuel()) << "\n";

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
      else if (x == 2 && y == 1) {
        os << 'F';
      }
      else {
        os << '.';
      }
    }
    os << '\n';
  }

  return os;
}