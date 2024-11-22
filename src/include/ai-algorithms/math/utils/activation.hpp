#ifndef _ACTIVATION_HPP_
#define _ACTIVATION_HPP_

#include <iostream>

template<typename T>
std::vector<T> SigmoidActivation(const std::vector<T>& vec){
  std::vector<T> result(vec.size());
  for(size_t i = 0; i < vec.size(); i++){
    result[i] = static_cast<T>(1.0 / (1.0 + std::exp(-vec[i])));
  }
  return result;
}

template<typename T>
std::vector<T> ReLUActivation(const std::vector<T>& vec){
  std::vector<T> result(vec.size());
  for(size_t i = 0; i < vec.size(); i++){
    result[i] = std::max(static_cast<T>(0), vec[i]);
  }
}

#endif
