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
#include <tr1/unordered_map>
#include <tr1/unordered_set>

template<typename K, typename V>
struct HashMap : std::tr1::unordered_map<K, V> {};

template<typename K>
struct HashSet : std::tr1::unordered_set<K> {};

template<class KeyType, class DataType>
class Table: public std::map<KeyType, DataType> {
public:
	void save(std::string file_name) const {
		file_name += ".tbl";
		std::ofstream fout(file_name.c_str());

		if (fout.good()) {
			fout << *this;
		}

		fout.close();
	}

	bool load(std::string file_name) {
		file_name += ".tbl";
		std::ifstream fin(file_name.c_str());

		if (fin.good()) {
			fin >> *this;
			fin.close();
			return true;
		}

		return false;
	}

	friend std::ostream & operator<<(std::ostream & os, const Table & o) {
		os << o.size() << std::endl;
		for (typename std::map<KeyType, DataType>::const_iterator it = o.begin(); it != o.end(); ++it) {
			os << std::setprecision(13) << it->first << " " << it->second << std::endl;
		}

		return os;
	}

	friend std::istream & operator>>(std::istream & is, Table & o) {
		uint size;
		KeyType key(0, 0, 0, 0);
		DataType data;

		is >> size;
		for (uint i = 0; i < size; ++i) {
			is >> key >> data;
			o[key] = data;
		}

		return is;
	}
};

typedef boost::tuples::tuple<State, int> state_action_pair_t;

template<class DataType>
class ActionDistribution: public boost::tuples::tuple<DataType, DataType, DataType, DataType, DataType, DataType, DataType> {
public:
	DataType & operator[](const int action) {
		switch (action) {
		case 0: return this->template get<0>();
		case 1: return this->template get<1>();
		case 2: return this->template get<2>();
		case 3: return this->template get<3>();
		case 4: return this->template get<4>();
		case 5: return this->template get<5>();
		case 6: return this->template get<6>();
		default: assert(0); return this->template get<0>();
		}
	}
};

template<class DataType>
class StateActionPairTable: public Table<State, ActionDistribution<DataType> > {
public:
	DataType & operator()(const State & state, int action) {
		return this->operator[](state)[action];
	}

	DataType & operator()(const state_action_pair_t & pair) {
		return this->operator()(pair.get<0>(), pair.get<1>());
	}
};

#endif /* QTABLE_H_ */
