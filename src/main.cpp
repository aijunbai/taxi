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
#include "HierarchicalFSMAgent.h"

#include <sys/time.h>
#include <thread>
#include <mutex>

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
    case ALG_HierarchicalFSM: return new HierarchicalFSMAgent(!train);
    default: return 0;
  }
}

void set_random_seed(int seed)
{
  srand(seed);
  srand48(seed);
}

mutex g_rewards_mutex;
mutex g_time_mutex;
mutex g_delete_mutex;

void call_from_thread(int tid,
                      int num_threads,
                      Algorithm algorithm,
                      vector<double> *rewards,
                      vector<double> *time,
                      double *avg_time,
                      int episodes,
                      int trials,
                      bool verbose,
                      bool useStaticTransition) {
  for (int i = 0; i < trials; ++i) {
    if (i % num_threads == tid) {
      cerr << "#trials " << i << endl;
      Agent *agent = CreatorAgent(algorithm, true);

      struct timeval start, end;
      double time_use;
      gettimeofday(&start, NULL);

      for (int j = 0; j < episodes; ++j) {
        cerr << "#episodes " << j << endl;
        double r = 0.0;
        if (algorithm == ALG_HierarchicalFSM) {
          r = System().simulateFSM(*static_cast<HierarchicalFSMAgent *>(agent), verbose, useStaticTransition);
        } else {
          r = System().simulate(*agent, verbose);
        }
        g_rewards_mutex.lock();
        (*rewards)[j] += r;
        g_rewards_mutex.unlock();
      }

      gettimeofday(&end, NULL);
      time_use = 1000000.0 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
      time_use /= 1000.0;

      g_time_mutex.lock();
      *avg_time += time_use;
      time->push_back(time_use);
      g_time_mutex.unlock();

      g_delete_mutex.lock();
      delete agent;
      g_delete_mutex.unlock();
    }
  }
}

int main(int argc, char **argv) {
  Algorithm algorithm = ALG_None;
  bool train = false;
  bool profile = false;
  bool verbose = false;
  bool useStaticTransition = false;
  bool multithreaded = false;

  try {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("verbose,v", "verbose mode")
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
        ("hierarchicalfsm,H", "use hierarchical FSM algorithm")
        ("size,n", po::value<int>(&TaxiEnv::SIZE), "Problem size")
        ("multithreaded,M", "use multi thread mode")
        ("statictransition,S", "take advantage of static transitions")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, (const char *const *) argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << desc << "\n";
      return 1;
    }

    train = vm.count("train");
    profile = vm.count("profile");
    verbose = vm.count("verbose");
    multithreaded = vm.count("multithreaded");
    useStaticTransition = vm.count("statictransition");

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
    else if (vm.count("hierarchicalfsm")) {
      algorithm = ALG_HierarchicalFSM;
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

  if (profile) {
    double avg_reward = 0.0, avg_time = 0.0;
    int trials = 10000;

    vector<double> rewards, time;

    Agent *agent = CreatorAgent(algorithm, false);

    for (int trial = 0; trial < trials; ++trial) {
      System system;

      struct timeval start, end;
      double time_use;
      gettimeofday(&start, NULL);

      double reward = 0.0;
      if (algorithm == ALG_HierarchicalFSM) {
        reward = system.simulateFSM(*static_cast<HierarchicalFSMAgent*>(agent), verbose, useStaticTransition);
      }
      else {
        reward = system.simulate(*agent, verbose);
      }

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

    if (algorithm == ALG_HierarchicalFSM) {
      cout << "Reward: " << System().simulateFSM(*static_cast<HierarchicalFSMAgent*>(agent), verbose, useStaticTransition) << endl;
    }
    else {
      cout << "Reward: " << System().simulate(*agent, verbose) << endl;
    }

    delete agent;
  }
  else { //train for rl agent
    int num_threads = 1;
    if (multithreaded) num_threads = thread::hardware_concurrency();

    const int trials = 4;
    const int episodes = 100000;

    vector<double> rewards(episodes, 0.0), time;
    double avg_time = 0.0;

    std::thread t[num_threads];

    for (int i = 0; i < num_threads; ++i) {
      t[i] = std::thread(
          call_from_thread,
          i,
          num_threads,
          algorithm,
          &rewards,
          &time,
          &avg_time,
          episodes,
          trials,
          verbose,
          useStaticTransition);
    }

    //Join the threads with the main thread
    for (int i = 0; i < num_threads; ++i) {
      t[i].join();
    }

    double avg = 0.0;
    queue<double> Q;
    const int N = 1000;
    for (int i = 0; i < N; ++i) {
      rewards[i] /= trials;
      avg = (avg * Q.size() + rewards[i]) / (Q.size() + 1);
      Q.push(rewards[i]);
    }

    for (int i = N; i < episodes; ++i) {
      rewards[i] /= trials;
      avg = (avg * Q.size() - Q.front() + rewards[i]) / Q.size();
      Q.pop();
      Q.push(rewards[i]);

      if (i % (episodes / 100) == 0) {
        cout << i << " " << avg << endl;
      }
    }

    double sum_time = 0.0;
    for (int i = 0; i < trials; ++i) {
      sum_time += (time[i] - avg_time) * (time[i] - avg_time);
    }

    cout << "#Avg. Time: " << avg_time << "+/-" << sqrt(sum_time) / trials << endl;
  }

  return 0;
}
