
#define _MATH_HPP__
#ifndef _MATH_HPP_

#include <vector>

inline std::vector<std::vector<int>> MultiplySquareMatrix(std::vector<std::vector<int>> v1, std::vector<std::vector<int>> v2){
  std::vector<std::vector<int>> result(v1.size());
  if(v1.size() != v2.size()){
    throw "Matrix Is Not Square: Rows";
  }
  for(int i =0; i < v1.size(); i++){
    if(v1[i].size() != v2[i].size()) 
      throw "Matrix Is Not Square: Cols";

    for(int j = 0; j < v1.size(); j++) {
      result[i].push_back(v1[i][j] * v2[i][j]);
    }
  }
  return result;
}

inline std::vector<std::vector<int>> getDotProd(std::vector<std::vector<int>> v1, std::vector<std::vector<int>> v2){
  return {};
}


#endif
