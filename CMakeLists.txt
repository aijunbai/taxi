cmake_minimum_required(VERSION 3.5)
project(maxq_op)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

set(SOURCE_FILES
        src/agent.h
        src/astar.cpp
        src/astar.h
        src/dot_graph.cpp
        src/dot_graph.h
        src/dynamicprogramming.cpp
        src/dynamicprogramming.h
        src/main.cpp
        src/maxqop.cpp
        src/maxqop.h
        src/monte-carlo.cpp
        src/monte-carlo.h
        src/olagent.h
        src/policy.cpp
        src/policy.h
        src/qlearning.cpp
        src/qlearning.h
        src/rlagent.cpp
        src/rlagent.h
        src/sarsa-lambda.cpp
        src/sarsa-lambda.h
        src/sarsa.cpp
        src/sarsa.h
        src/state.cpp
        src/state.h
        src/system.cpp
        src/system.h
        src/table.h
        src/taxi.cpp
        src/taxi.h
        src/tdagent.cpp
        src/tdagent.h
        src/uct.cpp
        src/uct.h
        src/utils.h src/HierarchicalFSMAgent.cpp src/HierarchicalFSMAgent.h src/ChoicePoint.h src/HierarchicalFSM.cpp src/HierarchicalFSM.h src/prettyprint.h )

add_executable(maxq_op ${SOURCE_FILES})
TARGET_LINK_LIBRARIES( maxq_op boost_program_options boost_serialization pthread )
