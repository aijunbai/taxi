#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>

using namespace std;

#include "taxi.h"
#include "system.h"
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
#include "statistic.h"

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

bool double_is_int(double trouble) {
   double absolute = abs( trouble );
   return absolute == floor(absolute);
}

mutex g_rewards_mutex;
mutex g_delete_mutex;

double Run(Algorithm algorithm, Agent *agent, bool leverageInternalTransitions, bool verbose) {
  double r = 0.0;
  if (algorithm == ALG_HierarchicalFSM || algorithm == ALG_MaxQ0 || algorithm == ALG_MaxQQ) {
    r = System().simulateHierarchicalAgent(*static_cast<HierarchicalAgent *>(agent), verbose,
                                           {{"leverageInternalTransitions", leverageInternalTransitions}});
  } else {
    r = System().simulate(*agent, verbose);
  }
  return r;
}

void Profile(Algorithm algorithm, Agent *agent, bool leverageInternalTransitions, bool verbose, STATISTIC *avg_reward) {
  const int N = 10000;

  agent->set_test(true);
  for (int n = 0; n < N; ++n) {
    double r = Run(algorithm, agent, leverageInternalTransitions, verbose);
    avg_reward->Add(r);
  }
}

void call_from_thread(int tid,
                      int num_threads,
                      Algorithm algorithm,
                      vector<STATISTIC> *rewards,
                      vector<STATISTIC> *crewards,
                      int episodes,
                      int trials,
                      bool verbose,
                      bool leverageInternalTransitions,
                      bool profile_after_train,
                      STATISTIC *avg_reward) {
  for (int i = 0; i < trials; ++i) {
    if (i % num_threads == tid) {
      cerr << "#" << algorithm << " @ trials #" << i << endl;

      Agent *agent = CreatorAgent(algorithm, true);

      double cr = 0.0;
      for (int j = 0; j < episodes; ++j) {
        if (j % (episodes / 100) == 0) {
          cerr << "#" << algorithm << " @ episodes #" << j << endl;
        }

        double r = Run(algorithm, agent, leverageInternalTransitions, verbose);
        cr += r;
        g_rewards_mutex.lock();
        (*rewards)[j].Add(r);
        (*crewards)[j].Add(cr);
        g_rewards_mutex.unlock();
      }

      if (profile_after_train) {
        g_rewards_mutex.lock();
        Profile(algorithm, agent, leverageInternalTransitions, verbose, avg_reward);
        g_rewards_mutex.unlock();
      }

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
        ("MaxQ-OP", "use MaxQ-OP algorithm")
        ("dynamicprogramming", "use DynamicProgramming algorithm")
        ("astar", "use A* algorithm")
        ("uct", "use UCT algorithm")
        ("hierarchicalfsm", "use HAMQ algorithm")
        ("HAMQ", "use HAMQ algorithm")
        ("hierarchicalfsmint", "use HAMQ-INT algorithm")
        ("HAMQ-INT", "use HAMQ-INT algorithm")
        ("maxq0", "use MaxQ-0 algorithm")
        ("MaxQ-0", "use MaxQ-0 algorithm")
        ("maxqq", "use MaxQ-Q algorithm")
        ("MaxQ-Q", "use MaxQ-Q algorithm")
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
    leverageInternalTransitions = vm.count("hierarchicalfsmint") || vm.count("HAMQ-INT");

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
    else if (vm.count("maxqop") || vm.count("MaxQ-OP")) {
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
    } else if (vm.count("hierarchicalfsm")
               || vm.count("hierarchicalfsmint")
               || vm.count("HAMQ")
               || vm.count("HAMQ-INT")) {
      algorithm = ALG_HierarchicalFSM;
    } else if (vm.count("maxq0") || vm.count("MaxQ-0")) {
      algorithm = ALG_MaxQ0;
    } else if (vm.count("maxqq") || vm.count("MaxQ-Q")) {
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

  // initialize cache
  PRINT_VALUE(ActionSize);
  PRINT_VALUE(ALG_None);

  if (train) { //train for rl agent
    int num_threads = 1;
    if (multithreaded) num_threads = thread::hardware_concurrency();

    vector<STATISTIC> rewards(episodes);
    vector<STATISTIC> crewards(episodes);
    STATISTIC avg_reward;

    std::thread t[num_threads];

    for (int i = 0; i < num_threads; ++i) {
      t[i] = std::thread(
          call_from_thread,
          i,
          num_threads,
          algorithm,
          &rewards,
          &crewards,
          episodes,
          trials,
          verbose,
          leverageInternalTransitions,
          profile,
          &avg_reward);
    }

    //Join the threads with the main thread
    for (int i = 0; i < num_threads; ++i) {
      t[i].join();
    }

    for (int i = 0; i < episodes; ++i) {
      int j = i + 1;
      if (j >= 1 && double_is_int(log(j)/log(2.0))) {
        cout << j << " " << rewards[i].GetMean() << " " << rewards[i].GetConfidenceInt()
             << " " << crewards[i].GetMean() << " " << crewards[i].GetConfidenceInt()
             << " #" << rewards[i] << " " << crewards[i] << endl;
      }
    }

    if (profile)
      cout << "#" << algorithm << " profile after train --  avg reward:" << avg_reward << endl;
  } else if (profile) { //profile a trained or online agent
    Agent *agent = CreatorAgent(algorithm, false);
    STATISTIC avg_reward;
    Profile(algorithm, agent, leverageInternalTransitions, verbose, &avg_reward);
    cout << "#" << algorithm << " profile --  avg reward:" << avg_reward << endl;
    delete agent;
  } else { // run once
    Agent *agent = CreatorAgent(algorithm, false);
    double reward = Run(algorithm, agent, leverageInternalTransitions, verbose);
    cout << "Reward: " << reward << endl;
    delete agent;
  }

  return 0;
}
