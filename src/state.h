/*
 * state.h
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#ifndef STATE_H_
#define STATE_H_

#include "utils.h"
#include <tuple>
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"

#include <string>
#include <sstream>

class State: public boost::tuples::tuple<int, int, int, int> {
public:
	State() {}

	State(const int & a, const int & b, const int & c, const int & d);

	int x() const;
	int y() const;
	int passenger() const;
	int destination() const ;

	int& x();
	int& y();
	int& passenger();
	int& destination();

	bool absorbing() const;

	std::string str() const;
};


#endif /* STATE_H_ */
