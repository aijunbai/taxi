/*
 * pole.cpp
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#include "taxi.h"

#include <cassert>

#include "state.h"

using namespace std;

int TaxiEnv::SIZE = 5;

double TaxiEnv::step(Action action)
{
  std::pair<State, double> sample = Sample(state_, action);
  state_ = sample.first;

  return sample.second;
}

std::pair<State, double> TaxiEnv::Sample(const State& state, Action action)
{
  double reward = Reward(state, action);
  State post_state(state);

  vector<pair<State, double> > samples = Transition(state, action);

  double cumulative_prob = 0.0, prob = drand48();

  for (uint i = 0; i < samples.size(); ++i) {
    cumulative_prob += samples[i].second;

    if (prob <= cumulative_prob) {
      post_state = samples[i].first;
      break;
    }
  }

  return make_pair(post_state, reward);
}

vector<pair<State, double> > TaxiEnv::Transition(const State & state, Action action)
{
  assert(!state.terminated());

  vector<pair<State, double> > samples;
  auto pos = state.taxiPosition();

  if (state.terminated()) {
    samples.push_back(make_pair(state, 1.0));
  }
  else {
    switch (action) {
      case Putdown:
        if (!state.loaded() || pos != Model::ins().terminals()[state.destination()]) {
          samples.push_back(make_pair(state, 1.0));
        }
        else {
          State tmp(state);
          tmp.passenger() = tmp.destination();
          samples.push_back(make_pair(tmp, 1.0));
        }
        break;
      case Pickup:
        if (state.loaded() || pos != Model::ins().terminals()[state.passenger()]) {
          samples.push_back(make_pair(state, 1.0));
        }
        else {
          State tmp(state);
          tmp.passenger() = Model::ins().inTaxi();
          samples.push_back(make_pair(tmp, 1.0));
        }
        break;
      case South:
      case North:
      case West:
      case East:
      {
#if 0
        State tmp(state);
        tmp.fuel() -= 1;

        Position location(state.x(), state.y());
        location = location + TaxiEnv::Model::ins().delta_[location.x][location.y][action];

        tmp.x() = location.x;
        tmp.y() = location.y;

        samples.push_back(make_pair(tmp, 1.0));
#else
        vector<Action> actions(3, action);
        vector<double> distribution(3, 0.1);
        vector<Position> locations(3, Position(state.x(), state.y()));

        distribution[0] = 0.8;

        switch (action) {
          case North: actions[1] = East; actions[2] = West; break;
          case South: actions[1] = East; actions[2] = West; break;
          case East: actions[1] = South; actions[2] = North; break;
          case West: actions[1] = South; actions[2] = North; break;
          default: assert(0); break;
        }

        State tmp(state);

        for (int i = 0; i < 3; ++i) {
          Position delta = Model::ins().delta_[locations[i].x][locations[i].y][actions[i]];
          locations[i] = locations[i] + delta;

          tmp.x() = locations[i].x;
          tmp.y() = locations[i].y;

          samples.push_back(make_pair(tmp, distribution[i]));
        }
#endif
      }
        break;
      default: assert(0); break;
    }
  }

  return samples;
}

double TaxiEnv::Reward(const State & state, Action action)
{
  assert(!state.terminated());

  if (state.terminated()) return 0.0;
  auto pos = state.taxiPosition();

  switch (action) {
    case Pickup:
      if (state.loaded() || pos != Model::ins().terminals()[state.passenger()]) {
        return -10;
      }
      else {
        return -1;
      }
    case Putdown:
      if (state.loaded() && pos == Model::ins().terminals()[state.destination()]) {
        return 20;
      }
      else {
        return -10;
      }
    case North:
    case South:
    case East:
    case West:
      return -1;
    default: assert(0); return -100000;
  }
}

void TaxiEnv::reset()
{
  Position taxi;
  int passenger, destination;

  do {
    taxi = get_random_position();
    passenger = get_random_stop();
    destination = get_random_stop();
  } while (passenger == destination);

  state_ = State(taxi.x, taxi.y, passenger, destination);
}

TaxiEnv::cond_t TaxiEnv::stateConditions(const State &state)
{
  return make_tuple(state.passenger(),
                    state.destination(),
                    state.passenger() == 4? 1: state.taxiPosition() == terminal(state.passenger()),
                    state.taxiPosition() == terminal(state.destination()));
}
