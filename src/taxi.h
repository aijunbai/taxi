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

	ActionSize,
	Nil
};

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
		EnvModel() {
			stops_.push_back(Position(0, 0));
			stops_.push_back(Position(0, SIZE - 1));
			stops_.push_back(Position(SIZE - 2, 0));
			stops_.push_back(Position(SIZE - 1, SIZE - 1));

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
        set_wall(SIZE - 3, i, East); set_wall(SIZE - 3, i, West);
      }
		}

		void set_wall(int x, int y, Action dir) {
			delta_[x][y][dir] = Position(0, 0);
		}

		const std::vector<Position> & stops() const {
			return stops_;
		}

		static int distance(const Position& a, const Position& b) {
			return abs(a.x - b.x) + abs(a.y - b.y);
		}

	private:
		std::vector<Position> stops_;

	public:
		mutable HashMap<int, HashMap<int, HashMap<int, Position> > > delta_;
	};

public:
	TaxiEnv(): state_(0) {

	}

	~TaxiEnv() {
		delete state_;
	}

	void reset();

	void set(const State & state) {
		*state_ = state;
	}

	double step(Action action);

	State get_state() {
		return *state_;
	}

	bool success() {
		return state_->passenger() == state_->destination();
	}

	bool terminate() {
		return success();
	}

private:
	Position get_random_position() {
		return Position(rand() % SIZE, rand() % SIZE);
	}

	int get_random_stop() {
		return rand() % model->stops().size();
	}

private:
	State *state_;

public:
	static const EnvModel *model;

	static std::pair<State, double> Sample(const State& state, Action action);
	static double Reward(const State & state, Action action);
	static std::vector<std::pair<State, double> > Transition(const State & state, Action action);
};

#endif /* POLE_H_ */
