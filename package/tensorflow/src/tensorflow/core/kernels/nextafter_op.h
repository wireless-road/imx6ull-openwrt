/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_CORE_KERNELS_NEXTAFTER_OP_H_
#define TENSORFLOW_CORE_KERNELS_NEXTAFTER_OP_H_

#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"
#include "tensorflow/core/kernels/cwise_ops.h"

namespace tensorflow {
namespace functor {

template <typename T>
struct nextafter_op {
  EIGEN_EMPTY_STRUCT_CTOR(nextafter_op)
  // GPU kernels on ROCm may have issues including standard C++ APIs. Use
  // specialized member functions and invoke HIP runtime APIs instead.
#if !TENSORFLOW_USE_ROCM
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE const T operator()(const T& x1,
                                                           const T& x2) const {
    return std::nextafter(x1, x2);
  }
#else
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE const float operator()(
      const float& x1, const float& x2) const {
    return nextafterf(x1, x2);
  }
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE const double operator()(
      const double& x1, const double& x2) const {
    return nextafter(x1, x2);
  }
#endif
};

template <typename T>
struct nextafter : base<T, nextafter_op<T>> {};

}  // namespace functor
}  // namespace tensorflow

#endif  // TENSORFLOW_CORE_KERNELS_NEXTAFTER_OP_H_