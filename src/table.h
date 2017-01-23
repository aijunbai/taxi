/*
 * qtable.h
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#ifndef QTABLE_H_
#define QTABLE_H_

#include "state.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <unordered_map>
#include <unordered_set>

template<typename K, typename V>
struct HashMap : std::unordered_map<K, V> {};

template<typename K>
struct HashSet : std::unordered_set<K> {};


#endif /* QTABLE_H_ */
