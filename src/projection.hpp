#ifndef PROJECTION_HPP
#define PROJECTION_HPP

/**
 * \file projection.hpp
 * \brief Utilities for conversion from coordinates in an image to
 * geographical coordinates.
 * \author Le Bars, Yoann
 * \version 1.0
 * \date 2013/06/27
 * \date 2013/06/28
 */

#include <boost/concept_check.hpp>
#include <cassert>
#include <vector>
#include <eigen3/Eigen/Dense>

/// \brief Namespace for projection computations.
namespace Projection {
  /// \brief Class for 2D points.
  class Point2D {
    public:
      /**
       * \brief Construct a point with two double.
       * \param _x Abscissa.
       * \param _y Ordinate.
       */
      Point2D (double _x = 0., double _y = 0.): x_ (_x), y_ (_y) {}

      /**
       * \brief Copy constructor.
       * \param p Point2D to be copied.
       */
      Point2D (const Point2D &p): x_ (p.x_), y_ (p.y_) {}

      /// \brief Destructor.
      ~Point2D () {}

      /// \brief Access to abscissa, modification is not possible.
      double x () const {return x_;}

      /// \brief Access to abscissa, modification is possible.
      double &x () {return x_;}

      /// \brief Access to ordinate, modification is not possible.
      double y () const {return y_;}

      /// \brief Access to ordinate, modification is possible.
      double &y () {return y_;}

      /**
       * \brief Copy operator.
       * \param p Point2D to be copied.
       * \return A reference to current point.
       */
      Point2D &operator = (const Point2D &p) {
        x_ = p.x_;
        y_ = p.y_;
        return *this;
      }

    private:
      /// \brief Abscissa.
      double x_;

      /// \brief Ordinate;
      double y_;
  };

  /// \brief Type for projection coefficients.
  typedef Eigen::Matrix<double, 3, 2> Coefficients;

  /**
   * \brief Compute the projection coefficients.
   * \param r1 Vector containing reference points in image coordinates.
   * \param r2 Vector containing reference points in geographical coordinates.
   * \return Vector containing projection coefficients.
   *
   * Information on coefficients can be found at:
   * <http://en.wikipedia.org/wiki/World_file>
   */
  inline Coefficients computeCoefficients (const std::vector<Point2D> &r1,
                                           const std::vector<Point2D> &r2) {
    assert(r1.size() == 3);
    assert(r2.size() == 3);
    /* Matrix to compute coefficients. */
    Eigen::Matrix3d a;
    a << r1[0].x(), r1[0].y(), 1.,
         r1[1].x(), r1[1].y(), 1.,
         r1[2].x(), r1[2].y(), 1.;
    /* Matrix of points in geographical coordinates. */
    Coefficients b;
    b << r2[0].x(), r2[0].y(),
         r2[1].x(), r2[1].y(),
         r2[2].x(), r2[2].y();
    return a.fullPivLu().solve(b);
  }
}

#endif  // #ifndef PROJECTION_HPP
