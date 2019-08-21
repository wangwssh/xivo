// Base class for camera models.
// Use CRTP (Curiously Recurring Template Pattern) to implement compile-time
// polymorphism.
// Author: Xiaohan Fei (feixh@cs.ucla.edu)
#pragma once

#include "Eigen/Dense"

namespace feh {

template <typename T, typename D> class BaseCamera {
public:
  // constructor
  BaseCamera(int rows, int cols, T fx, T fy, T cx, T cy)
      : rows_{rows}, cols_{cols}, fx_{fx}, fy_{fy}, cx_{cx}, cy_{cy} {}

  // copy constructor & assignment
  BaseCamera &operator=(const BaseCamera &) = default;
  BaseCamera(const BaseCamera &) = default;

  // move constructor & assignment
  BaseCamera &operator=(BaseCamera &&) = default;
  BaseCamera(BaseCamera &&) = default;

  // project a point xc in camera coordinates to pixel coordinates.
  // xc: camera coordinates of a point.
  // jac: jacobian matrix of xp w.r.t. xc, i.e., dxp/dxc
  // jacc: jacobian matrix of xp w.r.t. camera intrinsics, for online
  // calibration
  template <typename Derived>
  Eigen::Matrix<typename Derived::Scalar, 2, 1> Project(
      const Eigen::MatrixBase<Derived> &xc,
      Eigen::Matrix<typename Derived::Scalar, 2, 2> *jac = nullptr,
      Eigen::Matrix<typename Derived::Scalar, 2, -1> *jacc = nullptr) const {
    return static_cast<D *>(this)->Project(xc, jac);
  }

  // un-project a point xp in pixel coordinates to the camera coordinates.
  // xp: pixel coordinates of a point.
  // jac: jacobian matrix of xc w.r.t. xp, i.e., dxc/dxp
  // jacc: jacobian matrix of xc w.r.t. camera intrinsics, for online
  // calibration
  template <typename Derived>
  Eigen::Matrix<typename Derived::Scalar, 2, 1> UnProject(
      const Eigen::MatrixBase<Derived> &xp,
      Eigen::Matrix<typename Derived::Scalar, 2, 2> *jac = nullptr,
      Eigen::Matrix<typename Derived::Scalar, 2, -1> *jacc = nullptr) const {
    return static_cast<D *>(this)->UnProject(xp, jac);
  }

  // print intrinsics of the camer amodel
  void Print(std::ostream &out) const { static_cast<D *>(this)->Print(out); }

  // handy accessors
  int rows() const { return rows_; }
  int cols() const { return cols_; }
  T cx() const { return cx_; }
  T cy() const { return cy_; }
  T fx() const { return fx_; }
  T fy() const { return fy_; }

protected:
  int rows_, cols_;
  T fx_, fy_, cx_, cy_;
};

// check whether a given point is out of view.
template <typename T, typename Cam> bool OutOfView(T x, T y, const Cam &cam) {
  float margin(0.01f);
  return x < cam.cols() * margin || x >= cam.cols() * (1 - margin) ||
         y < cam.rows() * margin || y >= cam.rows() * (1 - margin);
}

// Eigen version of the above function.
template <typename Derived, typename Cam>
bool OutOfView(const Eigen::MatrixBase<Derived> &xp, const Cam &cam) {
  EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(Derived, 2, 1);
  return OutOfView(xp(0), xp(1), cam);
}

} // feh
