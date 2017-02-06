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
#include "MaxQ0Agent.h"
#include "MaxQQAgent.h"

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
    case ALG_MaxQOP: return new MaxQOPAgent(!train);
    case ALG_DynamicProgramming: return new DPAgent(!train);
    case ALG_AStar: return new AStarAgent(!train);
    case ALG_UCT: return new UCTAgent(!train);
    case ALG_HierarchicalFSM: return new HierarchicalFSMAgent(!train);
    case ALG_MaxQ0: return new MaxQ0Agent(!train);
    case ALG_MaxQQ: return new MaxQQAgent(!train);
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
                      bool leverageInternalTransitions) {
  for (int i = 0; i < trials; ++i) {
    if (i % num_threads == tid) {
      cerr << "#" << algorithm << " @ trials #" << i << endl;

      Agent *agent = CreatorAgent(algorithm, true);

      struct timeval start, end;
      double time_use;
      gettimeofday(&start, 0);

      for (int j = 0; j < episodes; ++j) {
        if (j % 10000 == 0) {
          cerr << "#" << algorithm << " @ episodes #" << j << endl;
        }

        double r = 0.0;
        if (algorithm == ALG_HierarchicalFSM || algorithm == ALG_MaxQ0 || algorithm == ALG_MaxQQ) {
          r = System().simulateHierarchicalAgent(*static_cast<HierarchicalAgent *>(agent), verbose,
                                                 {{"leverageInternalTransitions", leverageInternalTransitions}});
        } else {
          r = System().simulate(*agent, verbose);
        }

        g_rewards_mutex.lock();
        (*rewards)[j] += r;
        g_rewards_mutex.unlock();
      }

      gettimeofday(&end, 0);
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
  bool leverageInternalTransitions = false;
  bool multithreaded = false;
  int trials = 4;
  int episodes = 10000;

  try {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("verbose", "verbose mode")
        ("debug", "debug mode (do not set random seed)")
        ("profile", "profile a policy or an online policy")
        ("train", "set as train mode")
        ("montecarlo", "use MonteCarlo algorithm")
        ("sasar", "use SASAR algorithm")
        ("qlearning", "use Qlearning algorithm")
        ("sasarlambda", "use SASAR-lambda algorithm")
        ("maxqop", "use MaxQ-OP algorithm")
        ("dynamicprogramming", "use DynamicProgramming algorithm")
        ("astar", "use A* algorithm")
        ("uct", "use UCT algorithm")
        ("hierarchicalfsm", "use hierarchical FSM algorithm")
        ("hierarchicalfsmdet", "take advantage of internal transitions for hierarchicalfsm")
        ("maxq0", "use MaxQ0 algorithm")
        ("maxqq", "use MaxQQ algorithm")
        ("size", po::value<int>(&TaxiEnv::SIZE), "Problem size")
        ("trials", po::value<int>(&trials), "Training trials")
        ("episodes", po::value<int>(&episodes), "Training episodes")
        ("multithreaded", "use multi thread mode")
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
    leverageInternalTransitions = vm.count("hierarchicalfsmdet");

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
      algorithm = ALG_MaxQOP;
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
    else if (vm.count("hierarchicalfsm") || vm.count("hierarchicalfsmdet")) {
      algorithm = ALG_HierarchicalFSM;
    }
    else if (vm.count("maxq0")) {
      algorithm = ALG_MaxQ0;
    }
    else if (vm.count("maxqq")) {
      algorithm = ALG_MaxQQ;
    }
    else {
      cout << "Can not find an algorithm" << endl;
      cout << desc << endl;
      return 1;
    }

    if (!vm.count("debug")) {
      set_random_seed(getpid());
    }
    else {
      set_random_seed(0);
    }
  }
  catch(exception& e) {
    cout << "error: " << e.what() << "\n";
    return 1;
  }
  catch(...) {
    cout << "Exception of unknown type!\n";
    return 1;
  }

  PRINT_VALUE(ActionSize); // initialize cache

  if (profile) {
    double avg_reward = 0.0, avg_time = 0.0;
    int N = 10000;

    vector<double> rewards, time;

    Agent *agent = CreatorAgent(algorithm, false);

    for (int n = 0; n < N; ++n) {
      struct timeval start, end;
      double time_use;
      gettimeofday(&start, 0);

      double reward = 0.0;
      if (algorithm == ALG_HierarchicalFSM || algorithm == ALG_MaxQ0 || algorithm == ALG_MaxQQ) {
        reward = System().simulateHierarchicalAgent(*static_cast<HierarchicalAgent *>(agent), verbose,
                                                    {{"leverageInternalTransitions", leverageInternalTransitions}});
      }
      else {
        reward = System().simulate(*agent, verbose);
      }

      gettimeofday(&end, 0);
      time_use = 1000000.0 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
      time_use /= 1000.0;

      avg_reward += reward;
      avg_time += time_use;

      rewards.push_back(reward);
      time.push_back(time_use);
    }

    delete agent;

    avg_reward /= N;
    avg_time /= N;

    double sum_rewards = 0.0, sum_time = 0.0;
    for (int i = 0; i < N; ++i) {
      sum_rewards += (rewards[i] - avg_reward) * (rewards[i] - avg_reward);

      sum_time += (time[i] - avg_time) * (time[i] - avg_time);
    }

    cout << TaxiEnv::SIZE << " ";
    cout << avg_reward << " " << sqrt(sum_rewards) / N << " ";
    cout << avg_time << " " << sqrt(sum_time) / N << endl;
  }
  else if (!train) { //test
    Agent *agent = CreatorAgent(algorithm, false);

    if (algorithm == ALG_HierarchicalFSM || algorithm == ALG_MaxQ0 || algorithm == ALG_MaxQQ) {
      cout << "Reward: " << System().simulateHierarchicalAgent(*static_cast<HierarchicalAgent *>(agent), verbose,
                                                               {{"leverageInternalTransitions", leverageInternalTransitions}}) << endl;
    }
    else {
      cout << "Reward: " << System().simulate(*agent, verbose) << endl;
    }

    delete agent;
  }
  else { //train for rl agent
    int num_threads = 1;
    if (multithreaded) num_threads = thread::hardware_concurrency();

    vector<double> rewards(episodes, 0.0);
    vector<double> crewards(episodes, 0.0);
    vector<double> time;
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
          leverageInternalTransitions);
    }

    //Join the threads with the main thread
    for (int i = 0; i < num_threads; ++i) {
      t[i].join();
    }

    double avg = 0.0;
    double cavg = 0.0;

    queue<double> Q;
    queue<double> cQ;

    const int win = 1000;
    const int N = max(1, min(win, episodes / 10));
    for (int i = 0; i < N; ++i) {
      rewards[i] /= trials;

      if (i == 0)
        crewards[i] = rewards[i];
      else
        crewards[i] = crewards[i - 1] + rewards[i];

      avg = (avg * Q.size() + rewards[i]) / (Q.size() + 1);
      Q.push(rewards[i]);

      cavg = (cavg * cQ.size() + crewards[i]) / (cQ.size() + 1);
      cQ.push(crewards[i]);
    }

    for (int i = N; i < episodes; ++i) {
      rewards[i] /= trials;
      crewards[i] = crewards[i - 1] + rewards[i];

      avg = (avg * Q.size() - Q.front() + rewards[i]) / Q.size();
      Q.pop();
      Q.push(rewards[i]);

      cavg = (cavg * cQ.size() - cQ.front() + crewards[i]) / cQ.size();
      cQ.pop();
      cQ.push(crewards[i]);

      if ((i - N) % (max(1, episodes / 100)) == 0 || i == episodes - 1) {
        cout << i << " " << avg << " " << cavg << endl;
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
