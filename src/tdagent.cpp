/*
 * td-agent.cpp
 *
 *  Created on: Sep 27, 2010
 *      Author: baj
 */

#include "tdagent.h"

double TemporalDifferenceAgent::alpha = 0.15;
double TemporalDifferenceAgent::gamma = 1.0 - 1.0e-6;

double & TemporalDifferenceAgent::qvalue(const State & state, const int & action)
{
	return qtable_[state][action];
}
