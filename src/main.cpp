#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <iterator>
using namespace std;

#include "taxi.h"
#include "system.h"
#include "agent.h"
#include "monte-carlo.h"
#include "sarsa.h"
#include "qlearning.h"
#include "sarsa-lambda.h"
#include "maxqop.h"
#include "dynamicprogramming.h"
#include "astar.h"
#include "uct.h"

#include <sys/time.h>

Agent *CreatorAgent(Algorithm algorithm_t, bool train)
{
	switch (algorithm_t) {
	case ALG_MonteCarlo: return new MonteCarloAgent(!train);
	case ALG_Sarsa: return new SarsaAgent(!train);
	case ALG_QLearning: return new QLearningAgent(!train);
	case ALG_SarsaLambda: return new SarsaLambdaAgent(!train);
	case ALG_MaxQOL: return new MaxQOPAgent(!train);
	case ALG_DynamicProgramming: return new DPAgent(!train);
	case ALG_AStar: return new AStarAgent(!train);
	case ALG_UCT: return new UCTAgent(!train);
	default: return 0;
	}
}

void set_random_seed(int seed)
{
	srand(seed);
	srand48(seed);
}

int main(int argc, char **argv) {
	Algorithm algorithm = ALG_None;
	bool train = false;
	bool profile = false;

	try {
		po::options_description desc("Allowed options");
		desc.add_options()
	            				("help,h", "produce help message")
	            				("debug,D", "debug mode (do not set random seed)")
	            				("profile,p", "profile a policy or an online policy")
	            				("train,t", "set as train mode")
	            				("monte-carlo,m", "use MonteCarlo algorithm")
	            				("sasar,s", "use SASAR algorithm")
	            				("qlearning,q", "use Qlearning algorithm")
	            				("sasar-lambda,l", "use SASAR-lambda algorithm")
	            				("maxq-ol,o", "use MaxQ-OP algorithm")
	            				("dynamicprogramming,d", "use DynamicProgramming algorithm")
	            				("astar,a", "use A* algorithm")
	            				("uct,u", "use UCT algorithm")
	            				("size,S", po::value<int>(&TaxiEnv::SIZE), "Problem size")
	            				;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			cout << desc << "\n";
			return 1;
		}

		if (vm.count("train")) {
			train = true;
		}

		if (vm.count("profile")) {
			profile = true;
		}

		if (vm.count("monte-carlo")) {
			algorithm = ALG_MonteCarlo;
		}
		else if (vm.count("sasar")) {
			algorithm = ALG_Sarsa;
		}
		else if (vm.count("qlearning")) {
			algorithm = ALG_QLearning;
		}
		else if (vm.count("sasar-lambda")) {
			algorithm = ALG_SarsaLambda;
		}
		else if (vm.count("maxq-ol")) {
			algorithm = ALG_MaxQOL;
		}
		else if (vm.count("dynamicprogramming")) {
			algorithm = ALG_DynamicProgramming;
		}
		else if (vm.count("astar")) {
			algorithm = ALG_AStar;
		}
		else if (vm.count("uct")) {
			algorithm = ALG_UCT;
		}
		else {
			cout << desc << "\n";
			return 1;
		}

		if (!vm.count("debug")) {
			set_random_seed(getpid());
		}
	}
	catch(exception& e) {
		cerr << "error: " << e.what() << "\n";
		return 1;
	}
	catch(...) {
		cerr << "Exception of unknown type!\n";
		return 1;
	}

	TaxiEnv::model = new TaxiEnv::EnvModel();

	if (profile) { //profile on all states with 100 trials for each state
		double avg_reward = 0.0, avg_time = 0.0;
		int trials = 1000;

		vector<double> rewards, time;

		Agent *agent = CreatorAgent(algorithm, false);

		for (int trial = 0; trial < trials; ++trial) {
			System system;

			struct timeval start, end;
			double time_use;
			gettimeofday(&start, NULL);

			double reward = system.simulate(*agent, false);

			gettimeofday(&end, NULL);
			time_use = 1000000.0 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
			time_use /= 1000.0;

			avg_reward += reward;
			avg_time += time_use;

			rewards.push_back(reward);
			time.push_back(time_use);
		}

		delete agent;

		avg_reward /= trials;
		avg_time /= trials;

		double sum_rewards = 0.0, sum_time = 0.0;
		for (int i = 0; i < trials; ++i) {
			sum_rewards += (rewards[i] - avg_reward) * (rewards[i] - avg_reward);

			sum_time += (time[i] - avg_time) * (time[i] - avg_time);
		}

    cout << TaxiEnv::SIZE << " ";
		cout << avg_reward << " " << sqrt(sum_rewards) / trials << " ";
		cout << avg_time << " " << sqrt(sum_time) / trials << endl;
	}
	else if (!train) { //test
		Agent *agent = CreatorAgent(algorithm, false);

		cout << "Reward: " << System().simulate(*agent, true) << endl;

		delete agent;
	}
	else { //train for rl agent
		//settings taken from:﻿ Jong, N.K., Stone, P.: Hierarchical model-based reinforcement learning: R-MAX + MAXQ. Proceedings of the 25th international conference on Machine learning - ICML  ’08. pp. 432-439. ACM Press, New York, New York, USA (2008).
		const int trials = 1;
		const int episodes = 50;

		vector<double> rewards(episodes, 0.0), time;
		double avg_time = 0.0;

		for (int i = 0; i < trials; ++i) {
		  Agent *agent = CreatorAgent(algorithm, true);

		  struct timeval start, end;
		  double time_use;
		  gettimeofday(&start, NULL);

		  for (int j = 0; j < episodes; ++j) {
		    rewards[j] += System().simulate(*agent, false);
		  }

		  gettimeofday(&end, NULL);
		  time_use = 1000000.0 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
		  time_use /= 1000.0;

		  avg_time += time_use;
		  time.push_back(time_use);

		  delete agent;
		}

//		for (int i = 0; i < episodes; ++i) {
//			rewards[i] /= trials;
//			cout << rewards[i] << endl;
//		}

		double sum_time = 0.0;
		for (int i = 0; i < trials; ++i) {
			sum_time += (time[i] - avg_time) * (time[i] - avg_time);
		}

		cout << "#Avg. Time: " << avg_time << "+/-" << sqrt(sum_time) / trials << endl;
	}

	return 0;
}
