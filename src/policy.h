/*
 * policy.h
 *
 *  Created on: Oct 17, 2010
 *      Author: baj
 */

#ifndef POLICY_H_
#define POLICY_H_

#include "taxi.h"

#include <vector>
#include <map>
#include <string>

enum PolicyType {
	PT_None,

	PT_Random,
	PT_Greedy,
	PT_EpsilonGreedy
};


class Policy {
public:
	virtual ~Policy() { }
	virtual Action get_action(const std::vector<double> &distri) = 0;

	static std::string name(PolicyType type);
};

class RandomPolicy: public Policy {
public:
	virtual Action get_action(const std::vector<double> &distri);
};

class GreedyPolicy: public Policy {
public:
	virtual Action get_action(const std::vector<double> &distri);
};

class EpsilonGreedyPolicy: public GreedyPolicy {
private:
	static double epsilon_;

public:
	virtual Action get_action(const std::vector<double> &distri);
};

class PolicyFactory {
public:
	static PolicyFactory &instance() {
		static PolicyFactory policy_factory;
		return policy_factory;
	}

	Policy *CreatePolicy(PolicyType type) {
		if (policies_[type]) return policies_[type];

		switch(type) {
		case PT_Random: policies_[PT_Random] = new RandomPolicy(); break;
		case PT_Greedy: policies_[PT_Greedy] = new GreedyPolicy(); break;
		case PT_EpsilonGreedy: policies_[PT_EpsilonGreedy] = new EpsilonGreedyPolicy(); break;
		default: return 0;
		}

		return policies_[type];
	}

private:
	std::map<PolicyType, Policy*> policies_;
};

#endif /* POLICY_H_ */
