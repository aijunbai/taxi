/*
 * state.h
 *
 *  Created on: Sep 15, 2010
 *      Author: baj
 */

#ifndef STATE_H_
#define STATE_H_

#include "utils.h"
#include "prettyprint.h"
#include <tuple>
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"
#include <ostream>

#include <string>
#include <sstream>

using namespace std;

namespace aux{
template<std::size_t...> struct seq{};

template<std::size_t N, std::size_t... Is>
struct gen_seq : gen_seq<N-1, N-1, Is...>{};

template<std::size_t... Is>
struct gen_seq<0, Is...> : seq<Is...>{};

template<class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple(std::basic_ostream<Ch,Tr>& os, Tuple const& t, seq<Is...>){
  using swallow = int[];
  (void)swallow{0, (void(os << (Is == 0? "" : ", ") << std::get<Is>(t)), 0)...};
}
} // aux::

template<class Ch, class Tr, class... Args>
auto operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& t)
-> std::basic_ostream<Ch, Tr>&
{
  os << "(";
  aux::print_tuple(os, t, aux::gen_seq<sizeof...(Args)>());
  return os << ")";
}

class State: public tuple<int, int, int, int, int> {
public:
  State() {}

  State(int a, int b, int c, int d, int e);

  int x() const;
  int y() const;
  int passenger() const;
  int destination() const;
  int fuel() const;

  int& x();
  int& y();
  int& passenger();
  int& destination();
  int& fuel();

  bool terminated() const;

  std::string str() const;

  friend ostream &operator<<(ostream &os, const State &o);

  enum {
    MIN_FUEL = 12,
    MAX_FUEL = 24
  };
};

namespace std {
namespace
{
// Code from boost
// Reciprocal of the golden ratio helps spread entropy
//     and handles duplicates.
// See Mike Seymour in magic-numbers-in-boosthash-combine:
//     http://stackoverflow.com/questions/4948780

template <class T>
inline void hash_combine(std::size_t& seed, T const& v)
{
  seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

// Recursive template code derived from Matthieu M.
template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct HashValueImpl
{
  static void apply(size_t& seed, Tuple const& tuple)
  {
    HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
    hash_combine(seed, std::get<Index>(tuple));
  }
};

template <class Tuple>
struct HashValueImpl<Tuple,0>
{
  static void apply(size_t& seed, Tuple const& tuple)
  {
    hash_combine(seed, std::get<0>(tuple));
  }
};
}

template <typename ... TT>
struct hash<std::tuple<TT...>>
{
  size_t
  operator()(std::tuple<TT...> const& tt) const
  {
    size_t seed = 0;
    HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
    return seed;
  }

};

template <>
struct hash<State>
{
  std::size_t operator()(const State& k) const
  {
    return hash<tuple<int, int, int, int, int>>().operator()(k);
  }
};

}


#endif /* STATE_H_ */
