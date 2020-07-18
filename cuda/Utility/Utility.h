
/*
* 定义全局的核函数，UtilVec数组类，Paramater参数类，错误处理宏等
*/

#ifndef __UTILITY_H__
#define __UTILITY_H__

// cuda
#include <cuda.h>
#include <cufft.h>
#include <cuComplex.h>
#include "cuda_runtime.h"

// std
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <fstream>
#include <math.h>
#include <unordered_map>

#define DEBUG

#define CHECK(call)                                                            \
{                                                                              \
    const cudaError_t error = call;                                            \
    if (error != cudaSuccess)                                                  \
    {                                                                          \
        fprintf(stderr, "Error: %s:%d, ", __FILE__, __LINE__);                 \
        fprintf(stderr, "code: %d, reason: %s\n", error,                       \
                cudaGetErrorString(error));                                    \
        exit(1);                                                               \
    }                                                                          \
}

using namespace std;

template<typename T>
class UtilVec
{
public:
    T* vecPointer;      
    int vecNx;       
    int vecNy;          
    
    UtilVec() = delete;
    UtilVec(int nx, int ny);
    UtilVec(const UtilVec<T>&);
    UtilVec& operator=(const UtilVec<T>&); 
    ~UtilVec();

    int GetRefNum();


private:
    int* refCount;
};


template<typename T>
UtilVec<T>::UtilVec(int nx, int ny) :
    vecPointer(nullptr),
    vecNx(nx),
    vecNy(ny),
    refCount(nullptr)
{
    CHECK( cudaMalloc((void**)&(vecPointer), sizeof(T) * vecNx * vecNy) );
    refCount = (int*)malloc(sizeof(int));
    assert(refCount != nullptr);
    *refCount = 1;
}

// 拷贝构造函数，使得GPU地址引用计数加一
template<typename T>
UtilVec<T>::UtilVec(const UtilVec<T>& vec){
    vecNx = vec.vecNx;
    vecNy = vec.vecNy;
    vecPointer = vec.vecPointer;
    refCount = vec.refCount;
    (*refCount)++;
}

// 拷贝赋值运算符
// 对于 vec1 = vec2;
// vec1的引用计数减1
// vec2的引用计数加1
template<typename T>
UtilVec<T>& UtilVec<T>::operator=(const UtilVec<T>& vec){
    if(this == &vec)
        return *this;
    int* OldRefCount = refCount;
    auto OldVecPointer = vecPointer;
    (*OldRefCount)--; 

    vecNx = vec.vecNx;
    vecNy = vec.vecNy;
    vecPointer = vec.vecPointer;
    refCount = vec.refCount;
    (*refCount)++;  

    if(*OldRefCount == 0){
        cudaFree(vecPointer);
        free(OldRefCount);
    }
    return *this;
}


template<typename T>
UtilVec<T>::~UtilVec(){
    (*refCount)--;

    if((*refCount) == 0){
        cudaFree(vecPointer);
        free(refCount);
    }  
}


template<typename T>
int UtilVec<T>::GetRefNum(){
    return *this->refCount;
}


typedef UtilVec<cuFloatComplex> cuVector;
#endif 