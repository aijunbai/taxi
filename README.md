# Hierarchical online planning and reinforcement learning on Taxi
[![Build Status](https://travis-ci.org/aijunbai/taxi.svg?branch=master)](https://travis-ci.org/aijunbai/taxi)

This release consists of codes for two projects:
- The MAXQ-based hierarchical online planning algorithm: MAXQ-OP
- The HAMQ-based hierarchical reinforcement learning algorithm: HAMQ-INT

# hamq-int
This is an ongoing work. The idea is to identify and take advantage of internal transitions within a HAM for efficient hierarchical reinforcement learning. Some details can be found in:
-   **Efficient Reinforcement Learning with Hierarchies of Machines by Leveraging Internal Transitions**, *Aijun Bai*, and Stuart Russell,\
    *Proceedings of the Twenty-Fifth International Joint Conference on Artificial Intelligence (**IJCAI**), Melbourne, Australia, August 19 - 25, 2017.*
    \[[pdf](https://aijunbai.github.io/publications/IJCAI17-Bai.pdf)\]
    \[[bib](https://aijunbai.github.io/publications/IJCAI17-Bai.bib)\]

# maxq-op
This is the code release of MAXQ-OP algorithm for the Taxi domain as shown in papers:
- [Online planning for large Markov decision processes with hierarchical decomposition](https://aijunbai.github.io/publications/TIST15-Bai.pdf), Aijun Bai, Feng Wu, and Xiaoping Chen, ACM Transactions on Intelligent Systems and Technology (ACM TIST),6(4):45:1–45:28, July 2015.
- [Online Planning for Large MDPs with MAXQ Decomposition (Extended Abstract)](http://aijunbai.github.io/publications/AAMAS12-Bai.pdf), Aijun Bai, Feng Wu, and Xiaoping Chen, Proceedings of the 11th International Conference on Autonomous Agents and Multiagent Systems (AAMAS), Valencia, Spain, June 2012.
- [Towards a Principled Solution to Simulated Robot Soccer](http://aijunbai.github.io/publications/LNAI12-Bai.pdf), Aijun Bai, Feng Wu, and Xiaoping Chen, RoboCup-2012: Robot Soccer World Cup XVI, Lecture Notes in Artificial Intelligence, Vol. 7500, Springer Verlag, Berlin, 2013.
- [WrightEagle and UT Austin Villa: RoboCup 2011 Simulation League Champions](http://aijunbai.github.io/publications/LNAI11-Bai.pdf), Aijun Bai, Xiaoping Chen, Patrick MacAlpine, Daniel Urieli, Samuel Barrett, and Peter Stone, RoboCup-2011: Robot Soccer World Cup XV, Lecture Notes in Artificial Intelligence, Vol. 7416, Springer Verlag, Berlin, 2012.

It has also some less-tested implementations of other reinforcement learning and offline/online planning algorithms, such as dynamic programming, Q learning, SARSA learning, expected A\*, UCT, etc. 

# Files
- `maxqop.{h, cpp}`: the MAXQ-OP algorithm
- `HierarchicalFSMAgent.{h, cpp}`: the HAMQ-INT algorithm
- `MaxQ0Agent.{h, cpp}`: the MAXQ-0 algorithm
- `MaxQQAgent.{h, cpp}`: the MAXQ-Q algorithm
- `agent.h`: abstract `Agent` class
- `state.{h, cpp}`: abstract `State` class
- `policy.{h, cpp}`: `Policy` classes
- `taxi.{h, cpp}`: the Taxi domain
- `system.{h, cpp}`: agent-environment driver code
- `table.h`: tabular V/Q functions
- `dot_graph.{h, cpp}`: tools to generate graphviz `dot` files

# WrightEagle
The base code of WrightEagle soccer simulation 2D team (following the maxq-op algorithm) can be found at: [https://github.com/wrighteagle2d/wrighteaglebase](https://github.com/wrighteagle2d/wrighteaglebase)

# Dependencies
- libboost-dev 
- libboost-program-options-dev
