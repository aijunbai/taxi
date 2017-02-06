/*
 * state.h
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#ifndef STATE_H_
#define STATE_H_

#include "utils.h"
#include "prettyprint.h"
#include <tuple>
#include <cassert>
#include <ostream>

#include <string>
#include <sstream>
#include <unordered_map>

using namespace std;

enum Action {
  North = 0,
  South,
  East,
  West,
  Pickup,
  Putdown,

  ActionSize,
  Null
};

inline std::ostream& operator<<(std::ostream& out, const Action value) {
  static std::unordered_map<int, string> strings;

  if (strings.size() == 0) {
#define INSERT_ELEMENT(p) strings[p] = #p
    INSERT_ELEMENT(North);
    INSERT_ELEMENT(South);
    INSERT_ELEMENT(East);
    INSERT_ELEMENT(West);
    INSERT_ELEMENT(Pickup);
    INSERT_ELEMENT(Putdown);
    INSERT_ELEMENT(ActionSize);
    INSERT_ELEMENT(Null);
#undef INSERT_ELEMENT
  }

  assert(strings.count(value));
  return out << strings[value];
}

inline std::string action_name(Action action)
{
  switch (action) {
    case North: return "North";
    case South: return "South";
    case East: return "East";
    case West: return "West";
    case Pickup: return "Pickup";
    case Putdown: return "Putdown";
    default: return "Null-" + to_string(action);
  }
}

struct Position {
  int x;
  int y;

  Position(int a = 0, int b = 0): x(a), y(b) {

  }

  Position normalize();

  bool operator==(const Position & o) const {
    return x == o.x && y == o.y;
  }

  bool operator!=(const Position & o) const {
    return x != o.x || y != o.y;
  }

  Position operator+(const Position & o) {
    return Position(x + o.x, y + o.y);
  }
};

class State: public tuple<int, int, int, int> {
public:
  State() {}

  State(int a, int b, int c, int d);

  int x() const;
  int y() const;
  int passenger() const;
  int destination() const;

  int& x();
  int& y();
  int& passenger();
  int& destination();

  bool terminated() const;

  bool unloaded() const;

  bool loaded() const;

  Position taxiPosition() const;

  std::string str() const;

  friend ostream &operator<<(ostream &os, const State &o);
};

namespace std {
namespace
{
// Code from boost
// Reciprocal of the golden ratio helps spread entropy
//     and handles duplicates.
// See Mike Seymour in magic-numbers-in-boosthash-combine:
//     http://stackoverflow.com/que--size 5 --train --hierarchicalfsmdet --debugstions/4948780

template <class T>
inline void hash_combine(std::size_t& seed, T const& v)
{
  seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

// Recursive template code derived from Matthieu M.
template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct HashValueImpl
{
  static void apply(size_t& seed, Tuple const& tuple)
  {
    HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
    hash_combine(seed, std::get<Index>(tuple));
  }
};

template <class Tuple>
struct HashValueImpl<Tuple,0>
{
  static void apply(size_t& seed, Tuple const& tuple)
  {
    hash_combine(seed, std::get<0>(tuple));
  }
};
}

template <typename ... TT>
struct hash<std::tuple<TT...>>
{
  size_t
  operator()(std::tuple<TT...> const& tt) const
  {
    size_t seed = 0;
    HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
    return seed;
  }

};

template <>
struct hash<State>
{
  std::size_t operator()(const State& k) const
  {
    return hash<tuple<int, int, int, int>>().operator()(k);
  }
};

}


#endif /* STATE_H_ */
