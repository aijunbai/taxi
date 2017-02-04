/*
 * td-agent.cpp
 *
 *  Created on: Sep 27, 2010
 *      Author: baj
 */

#include "tdagent.h"

double TemporalDifferenceAgent::alpha = 0.125;
double TemporalDifferenceAgent::gamma = 1.0;

double & TemporalDifferenceAgent::qvalue(const State & state, const int & action)
{
	return qtable_[state][action];
}
