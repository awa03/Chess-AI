#ifndef _BIAS_HPP_
#define _BIAS_HPP_

#include <iostream>

template<typename T>
inline T AddBias(std::vector<T>& v1, std::vector<T>& bias){
  if(v1.size() != bias.size())
    throw std::invalid_argument("ERROR | \tAccuracy \t|\tInvalid Vector Size");

  for(size_t i = 0; i < v1.size(); i++){
    v1[i] += bias[i];
  }
}

#endif

