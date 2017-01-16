/*
 * utils.h
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include "prettyprint.h"

struct Double {
	double value_;

	Double(): value_(-1.0e6) {

	}

	const double& operator()() const {
		return value_;
	}

	double& operator()() {
		return value_;
	}
};

inline double get_prob()
{
	return drand48();
}

template<typename _Tp>
inline const _Tp&
minmax(const _Tp& min_, const _Tp& x, const _Tp& max_)
{
    return std::min(std::max(min_, x), max_);
}

template <class T>
inline std::string to_string (const T& t)
{
std::stringstream ss;
ss << t;
return ss.str();
}

#define PRINT_VALUE(x) \
  do { \
  std::cerr << #x " = '" << x << "'" << std::endl; \
  } while(0)

#define PRINT_VALUE_STREAM(os, x) \
  do { \
  os << #x " = '" << x << "'" << std::endl; \
  } while(0)



#endif /* UTILS_H_ */
