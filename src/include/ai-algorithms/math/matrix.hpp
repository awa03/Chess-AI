
#define _MATRIX_HPP__
#ifndef _MATRIX_HPP_

#include <iostream>
#include <vector>
#include <cstdlib>


#include "utils/analysis.hpp"

template<typename T>
inline std::vector<std::vector<T>> RandomMatrix(int rows, int cols, T min, T max){
  std::vector<std::vector<T>> result(rows);
  for(auto& r : result)
      r.resize(cols);

  for(auto& r : result){
    for(auto& c : r){
      c = static_cast<T>(rand() % (max - min + 1) + min);
    }
  }

  return result;
}

template<typename T>
inline std::vector<std::vector<T>> HadmardProduct(std::vector<std::vector<T>> v1, std::vector<std::vector<T>> v2){
  std::vector<std::vector<int>> result(v1.size());
  if(v1.size() != v2.size())
    throw std::invalid_argument("ERROR | \tHamard Product \t|\tInvalid Vector Size");

  for(size_t i =0; i < v1.size(); i++){
    if(v1[i].size() != v2[i].size()) 
      throw std::invalid_argument("ERROR | \tHamard Product \t|\tInvalid Vector Size");

    for(int j = 0; j < v1.size(); j++) {
      try {
        result[i].push_back(v1[i][j] * v2[i][j]);
      }
      catch(std::exception e){
        throw std::invalid_argument("ERROR | \tHamard Product \t|\tINVALID MULT");
      }
    }
  }
  return result;
}


// vector dot product
template<typename T> 
inline double getDotProd(std::vector<T> v1, std::vector<T> v2){
  if(v1.size() != v2.size())
    throw std::invalid_argument("ERROR | \tDot Product \t|\tInvalid Vector Size");
  
  double result = 0.0;
  for(size_t i =0; i < v1.size(); i++){
    try {
      result += v1[i] * v2[i];
    }
    catch(std::exception e){
      throw std::invalid_argument("ERROR | \tDot Product \t|\tInvalid Type");
    }
  }
  return result;
}

// Matrix dot prod
template<typename T>
inline double getDotProd(std::vector<std::vector<T>> m1, std::vector<std::vector<T>> m2){
  if(m1.size() != m2.size())
    throw std::invalid_argument("ERROR | \tDot Product \t|\tInvalid Matix Size");

  double result = 0.0;
  for(size_t i =0; i < m1.size(); i++){
    result += getDotProd(m1[i], m2[i]);
  }
  return result;
}


template<typename T>
inline std::vector<std::vector<T>> Transpose(const std::vector<std::vector<T>>& mat) {
    if (mat.empty()) {
        return {};
    }

    size_t rows = mat.size();
    size_t cols = mat[0].size();

    for (const auto& row : mat) {
        if (row.size() != cols) {
            throw std::invalid_argument("All rows must have the same number of columns.");
        }
    }

    std::vector<std::vector<T>> transposed(cols, std::vector<float>(rows));

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            transposed[j][i] = mat[i][j];
        }
    }

    return transposed;
}

#endif
