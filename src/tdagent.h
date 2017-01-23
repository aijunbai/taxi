/*
 * tdagent.h
 *
 *  Created on: Sep 18, 2010
 *      Author: baj
 */

#ifndef TDAGENT_H_
#define TDAGENT_H_

#include <string>

#include "rlagent.h"
#include "table.h"

/**
 * temporal difference based method
 */
class TemporalDifferenceAgent: public RLAgent {
public:
	static double alpha; //learning rate - which is somehow good according to empirical resutls
	static double gamma; //constant step parameter

public:
	TemporalDifferenceAgent(const std::string name, const bool test): RLAgent(test), name_(name) {
	}

	virtual ~TemporalDifferenceAgent() {
	}

	double & qvalue(const State &, const int &);

private:
	HashMap<State, HashMap<int, double>> qtable_;
	const std::string name_;
};

#endif /* TDAGENT_H_ */
