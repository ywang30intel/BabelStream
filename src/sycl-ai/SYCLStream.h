
// Copyright (c) 2015-16 Tom Deakin, Simon McIntosh-Smith,
// University of Bristol HPC
//
// For full license terms please see the LICENSE file distributed with this
// source code

#pragma once

#include <sstream>
#include <memory>

#include "Stream.h"

#include <sycl/sycl.hpp>

#define IMPLEMENTATION_STRING "SYCL"

namespace sycl_kernels
{
  template <class T> class init;
  template <class T> class copy;
  template <class T> class mul;
  template <class T> class add;
  template <class T> class triad;
  template <class T> class nstream;
  template <class T> class dot;
}

template <class T>
class SYCLStream : public Stream<T>
{
  protected:
    // Size of arrays
    size_t array_size;

    // SYCL objects
    std::unique_ptr<sycl::queue> queue;

    // Device side pointers
    T *d_a{};
    T *d_b{};
    T *d_c{};
    T *d_sum{};
    T *d_dot{};
    T *h_sum{};
    size_t dot_sum_capacity{};

    // Host buffers for verification
    std::vector<T> h_a, h_b, h_c;

    // Dot reduction mode
    bool use_manual_dot_reduction{};

    // SYCL kernel names
    typedef sycl_kernels::init<T> init_kernel;
    typedef sycl_kernels::copy<T> copy_kernel;
    typedef sycl_kernels::mul<T> mul_kernel;
    typedef sycl_kernels::add<T> add_kernel;
    typedef sycl_kernels::triad<T> triad_kernel;
    typedef sycl_kernels::nstream<T> nstream_kernel;
    typedef sycl_kernels::dot<T> dot_kernel;

    // NDRange configuration for stream kernels
    size_t stream_wgsize;
    size_t copy_wgsize;
    size_t mul_wgsize;
    size_t add_wgsize;
    size_t triad_wgsize;
    size_t nstream_wgsize;

    // NDRange configuration for the dot kernel
    size_t dot_num_groups;
    size_t dot_wgsize;
    size_t dot_unroll;

    void autotune_stream_kernels(bool is_intel_gpu, size_t max_wg);
    void autotune_dot(bool is_intel_gpu, size_t max_wg);

  public:

    SYCLStream(BenchId bs, const intptr_t array_size, const int device_id,
           T initA, T initB, T initC);
    ~SYCLStream();

    virtual void copy() override;
    virtual void add() override;
    virtual void mul() override;
    virtual void triad() override;
    virtual void nstream() override;
    virtual T    dot() override;

    void get_arrays(T const*& a, T const*& b, T const*& c) override;
    void init_arrays(T initA, T initB, T initC);

};

// Populate the devices list
void getDeviceList(void);
