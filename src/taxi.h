/*
 * pole.h
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#ifndef POLE_H_
#define POLE_H_

#include <cstdlib>
#include <cmath>
#include <iostream>

#include "utils.h"
#include "state.h"
#include "table.h"

#include <vector>
#include <string>

enum Action {
  North = 0,
  South,
  East,
  West,
  Pickup,
  Putdown,
  Fillup,

  ActionSize,
  Null
};

inline std::ostream& operator<<(std::ostream& out, const Action value) {
  static unordered_map<int, string> strings;

  if (strings.size() == 0) {
#define INSERT_ELEMENT(p) strings[p] = #p
    INSERT_ELEMENT(North);
    INSERT_ELEMENT(South);
    INSERT_ELEMENT(East);
    INSERT_ELEMENT(West);
    INSERT_ELEMENT(Pickup);
    INSERT_ELEMENT(Putdown);
    INSERT_ELEMENT(Fillup);
    INSERT_ELEMENT(ActionSize);
    INSERT_ELEMENT(Null);
#undef INSERT_ELEMENT
  }

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
    case Fillup: return "Fillup";
    default: return "Null-" + to_string(action);
  }
}

class TaxiEnv {
public:
  static int SIZE;

public:
  struct Position {
    int x;
    int y;

    Position(int a = 0, int b = 0): x(a), y(b) {

    }

    Position normalize() {
      return Position(minmax(0, x, SIZE - 1), minmax(0, y, SIZE - 1));
    }

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

  class EnvModel {
  public:
    static const EnvModel &ins() {
      static EnvModel m;
      return m;
    }

  private:
    EnvModel() {
      terminals_.push_back(Position(0, 0));
      terminals_.push_back(Position(0, SIZE - 1));
      terminals_.push_back(Position(SIZE - 2, 0));
      terminals_.push_back(Position(SIZE - 1, SIZE - 1));

      inTaxi_ = terminals_.size();
      fuelPosition_ = Position(2, 1);

      for (int x = 0; x < SIZE; ++x) {
        for (int y = 0; y < SIZE; ++y) {
          delta_[x][y][North] = Position(0, 1);
          delta_[x][y][South] = Position(0, -1);
          delta_[x][y][East] = Position(1, 0);
          delta_[x][y][West] = Position(-1, 0);
        }
      }

      for (int x = 0; x < SIZE; ++x) {
        delta_[x][0][South] = Position(0, 0);
        delta_[x][SIZE - 1][North] = Position(0, 0);
      }

      for (int y = 0; y < SIZE; ++y) {
        delta_[0][y][West] = Position(0, 0);
        delta_[SIZE - 1][y][East] = Position(0, 0);
      }

      for (int i = 0; i < int((SIZE - 1) / 2); ++i) {
        set_wall(0, i, East); set_wall(1, i, West);
      }

      for (int i = 0; i < int((SIZE - 1) / 2); ++i) {
        set_wall(1, SIZE - i - 1, East); set_wall(2, SIZE - i - 1, West);
      }

      for (int i = 0; i < int((SIZE - 1) / 2); ++i) {
        set_wall(SIZE - 3, i, East); set_wall(SIZE - 2, i, West);
      }
    }

    void set_wall(int x, int y, Action dir) {
      delta_[x][y][dir] = Position(0, 0);
    }

  public:
    const std::vector<Position> & terminals() const {
      return terminals_;
    }

    static int distance(const Position& a, const Position& b) {
      return abs(a.x - b.x) + abs(a.y - b.y);
    }

  private:
    std::vector<Position> terminals_;
    Position fuelPosition_;
    int inTaxi_;

  public:
    int getInTaxi_() const {
      return inTaxi_;
    }

  public:
    const Position &getFuelPosition_() const {
      return fuelPosition_;
    }

  public:
    mutable HashMap<int, HashMap<int, HashMap<int, Position> > > delta_;
  };

public:
  TaxiEnv() {
  }

  ~TaxiEnv() {
  }

  void reset();

  double step(Action action);

  const State &get_state() {
    return state_;
  }

  bool unloaded() {
    return state_.passenger() == state_.destination();
  }

  bool loaded() {
    return state_.passenger() == EnvModel::ins().getInTaxi_();
  }

  bool refueled() {
    return taxi() == EnvModel::ins().getFuelPosition_() && state_.fuel() == State::MAX_FUEL;
  }

  bool terminated() {
    return state_.terminated();
  }

  Position destination() { return terminal(state_.destination()); };
  Position passenger() { return terminal(state_.passenger()); };
  Position terminal(int i) { return EnvModel::ins().terminals().operator[](i); };
  Position taxi() { return Position(state_.x(), state_.y()); }

private:
  Position get_random_position() {
    return Position(rand() % SIZE, rand() % SIZE);
  }

  int get_random_stop() {
    return rand() % EnvModel::ins().terminals().size();
  }

  int get_random_fuel() {
    return State::MIN_FUEL + rand() % (State::MAX_FUEL - State::MIN_FUEL + 1);
  }

private:
  State state_;

public:
  static std::pair<State, double> Sample(const State& state, Action action);
  static double Reward(const State & state, Action action);
  static std::vector<std::pair<State, double> > Transition(const State & state, Action action);
};

#endif /* POLE_H_ */
