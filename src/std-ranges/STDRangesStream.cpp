// Copyright (c) 2020 Tom Deakin
// University of Bristol HPC
//
// For full license terms please see the LICENSE file distributed with this
// source code

#include "STDRangesStream.hpp"
#include <ranges>

#ifndef ALIGNMENT
#define ALIGNMENT (2*1024*1024) // 2MB
#endif

template <class T>
STDRangesStream<T>::STDRangesStream(BenchId bs, const intptr_t array_size, const int device_id,
				    T initA, T initB, T initC)
  noexcept : array_size{array_size},
	     a(alloc_raw<T>(array_size)), b(alloc_raw<T>(array_size)), c(alloc_raw<T>(array_size))
{
    std::cout << "Backing storage typeid: " << typeid(a).name() << std::endl;
#ifdef USE_ONEDPL
    std::cout << "Using oneDPL backend: ";
#if ONEDPL_USE_DPCPP_BACKEND
    std::cout << "SYCL USM (device=" << exe_policy.queue().get_device().get_info<sycl::info::device::name>() << ")";
#elif ONEDPL_USE_TBB_BACKEND
    std::cout << "TBB " TBB_VERSION_STRING;
#elif ONEDPL_USE_OPENMP_BACKEND
    std::cout << "OpenMP";
#else
    std::cout << "Default";
#endif
    std::cout << std::endl;
#endif
    init_arrays(initA, initB, initC);
}

template<class T>
STDRangesStream<T>::~STDRangesStream() {
  dealloc_raw(a);
  dealloc_raw(b);
  dealloc_raw(c);
}

template <class T>
void STDRangesStream<T>::init_arrays(T initA, T initB, T initC)
{
  std::for_each_n(
    exe_policy,
    std::views::iota((intptr_t)0).begin(), array_size, // loop range
    [=, this] (intptr_t i) {
      a[i] = initA;
      b[i] = initB;
      c[i] = initC;
    }
  );
}

template <class T>
void STDRangesStream<T>::get_arrays(T const*& h_a, T const*& h_b, T const*& h_c)
{
  h_a = a;
  h_b = b;
  h_c = c;
}

template <class T>
void STDRangesStream<T>::copy()
{
  std::for_each_n(
    exe_policy,
    std::views::iota((intptr_t)0).begin(), array_size,
    [=, this] (intptr_t i) {
      c[i] = a[i];
    }
  );
}

template <class T>
void STDRangesStream<T>::mul()
{
  const T scalar = startScalar;

  std::for_each_n(
    exe_policy,
    std::views::iota((intptr_t)0).begin(), array_size,
    [=, this] (intptr_t i) {
      b[i] = scalar * c[i];
    }
  );
}

template <class T>
void STDRangesStream<T>::add()
{
  std::for_each_n(
    exe_policy,
    std::views::iota((intptr_t)0).begin(), array_size,
    [=, this] (intptr_t i) {
      c[i] = a[i] + b[i];
    }
  );
}

template <class T>
void STDRangesStream<T>::triad()
{
  const T scalar = startScalar;

  std::for_each_n(
    exe_policy,
    std::views::iota((intptr_t)0).begin(), array_size,
    [=, this] (intptr_t i) {
      a[i] = b[i] + scalar * c[i];
    }
  );
}

template <class T>
void STDRangesStream<T>::nstream()
{
  const T scalar = startScalar;

  std::for_each_n(
    exe_policy,
    std::views::iota((intptr_t)0).begin(), array_size,
    [=, this] (intptr_t i) {
      a[i] += b[i] + scalar * c[i];
    }
  );
}

template <class T>
T STDRangesStream<T>::dot()
{
  // sum += a[i] * b[i];
  return
    std::transform_reduce(
      exe_policy,
      a, a + array_size, b, T{});
}

void listDevices(void)
{
  std::cout << "C++20 does not expose devices" << std::endl;
}

std::string getDeviceName(const int)
{
  return std::string("Device name unavailable");
}

std::string getDeviceDriver(const int)
{
  return std::string("Device driver unavailable");
}

template class STDRangesStream<float>;
template class STDRangesStream<double>;
