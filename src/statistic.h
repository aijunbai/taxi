#ifndef STATISTIC_H
#define STATISTIC_H

#include <math.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cassert>

template<class COUNT>
class VALUE {
public:
  VALUE() {
    Count = 0;
    Total = 0;
  }

  void Set(double count, double value) {
    Count = count;
    Total = value * count;
  }

  void Add(double x) {
    Count += 1.0;
    Total += x;
  }

  void Add(double x, COUNT weight) {
    Count += weight;
    Total += x * weight;
  }

  double GetValue() const { return Count == 0 ? Total : Total / double(Count); }

  COUNT GetCount() const { return Count; }

private:
  COUNT Count;
  double Total;
};

class STATISTIC {
public:
  STATISTIC();

  ~STATISTIC() { }

  int GetCount() const;

  double GetValue() const;            // merge from VALUE
  void Set(int count, double value);  // merge from VALUE

  void Initialise();

  double GetTotal() const;

  double GetMean() const;

  double GetVariance() const;

  double GetStdDev() const;
  double GetConfidenceInt() const;
  double GetStdErr() const;
  double GetMax() const;

  double GetMin() const;

  void SetMin(double min) { Min = min; }

  void SetMax(double max) { Max = max; }

  void AdjustRange(double min, double max) {
    Min = std::min(min, Min);
    Max = std::max(max, Max);
  }

  void Print(const std::string &name, std::ostream &ostr, bool endl = true) const;

  void Add(double val);

  friend std::ostream &operator<<(std::ostream &os, const STATISTIC &o) {
    o.Print("", os, false);
    return os;
  }

private:
  double Count;
  double Mean;
  double Variance;
  double Min, Max;
};

inline STATISTIC::STATISTIC() { Initialise(); }

inline void STATISTIC::Set(int count, double value) {
  Initialise();

  Count = count;
  Mean = value;
}

inline void STATISTIC::Add(double val) {
  double meanOld = Mean;
  int countOld = Count;

  ++Count;
  assert(Count > 0);  // overflow
  Mean += (val - Mean) / Count;
  Variance = (countOld * (Variance + meanOld * meanOld) + val * val) / Count -
             Mean * Mean;

  if (Variance < 0.0) Variance = 0.0;
  if (val > Max) Max = val;
  if (val < Min) Min = val;
}

inline void STATISTIC::Initialise() {
  Count = 0;
  Mean = 0;
  Variance = 0;
  Min = +10e6;
  Max = -10e6;
}

inline int STATISTIC::GetCount() const { return Count; }

inline double STATISTIC::GetTotal() const { return Mean * Count; }

inline double STATISTIC::GetValue() const { return GetMean(); }

inline double STATISTIC::GetMean() const { return Mean; }

inline double STATISTIC::GetVariance() const { return Variance; }

inline double STATISTIC::GetStdDev() const { return sqrt(Variance); }

inline double STATISTIC::GetConfidenceInt() const { return Count > 0 ? 1.96 * sqrt(Variance / Count) : 1000000.0; }
inline double STATISTIC::GetStdErr() const { return Count > 0 ? sqrt(Variance / Count) : 1000000.0; }

inline double STATISTIC::GetMax() const { return Max; }

inline double STATISTIC::GetMin() const { return Min; }

inline void STATISTIC::Print(const std::string &name,
                             std::ostream &ostr, bool endl) const {
  ostr << name << ": " << Mean << " (" << GetCount() << ") [" << Min << ", "
  << Max << "] +/- " << GetConfidenceInt() << ", SD=" << GetStdDev() << ", SE=" << GetStdErr();
  if (endl) {
    ostr << std::endl;
  }
}

#endif  // STATISTIC
