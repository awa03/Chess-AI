#ifndef _ANALYSIS_HPP_
#define _ANALYSIS_HPP_

#include <iostream>

// Only Use For Vectors of Double, Float, INT 
template<typename T>
T Accuracy(const std::vector<T>& predicted, const std::vector<T>& target){
  if(predicted.size() != target.size())
    throw std::invalid_argument("ERROR | \tAccuracy \t|\tInvalid Vector Size");

  int correct = 0;
  for(size_t i = 0; i < predicted.size(); i++){
    if(predicted[i] == target[i]){
      correct++;
    }
  }

  return static_cast<T>(correct) / static_cast<T>(target.size());
}

#endif
