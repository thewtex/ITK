#include <cassert>
#include <iostream>
#include <utility>

#include "testlib/testlib_test.h"
#include "vnl/vnl_sparse_lst_sqr_function.h"
#include <vnl/algo/vnl_sparse_lm.h>
#include "vnl/vnl_random.h"
#include "vnl/vnl_math.h"


// translate, scale, and rotate all cameras and points into a
// canonical coordinate system for direct comparison
static void
normalize(vnl_vector<double> & a, vnl_vector<double> & b)
{
  double x_mean = 0.0;
  double y_mean = 0.0;
  const unsigned int num_pts = b.size() / 2;
  for (unsigned int i = 0; i < num_pts; ++i)
  {
    x_mean += b[2 * i];
    y_mean += b[2 * i + 1];
  }
  x_mean /= num_pts;
  y_mean /= num_pts;

  // translate points to the origin
  for (unsigned int i = 0; i < num_pts; ++i)
  {
    b[2 * i] -= x_mean;
    b[2 * i + 1] -= y_mean;
  }
  // translate cameras
  for (unsigned int i = 0; i < a.size() / 3; ++i)
  {
    const double sa = std::sin(a[3 * i]);
    const double ca = std::cos(a[3 * i]);
    a[3 * i + 1] += ca * x_mean - sa * y_mean;
    a[3 * i + 2] += sa * x_mean + ca * y_mean;
  }

  double mean_dist = 0.0;
  for (unsigned int i = 0; i < num_pts; ++i)
  {
    mean_dist += std::sqrt(b[2 * i] * b[2 * i] + b[2 * i + 1] * b[2 * i + 1]);
  }
  mean_dist /= num_pts;

  // scale points
  for (double & i : b)
  {
    i /= mean_dist;
  }
  // scale cameras
  for (unsigned int i = 0; i < a.size() / 3; ++i)
  {
    a[3 * i + 1] /= mean_dist;
    a[3 * i + 2] /= mean_dist;
  }

  // use the vector between the first two points for orientation
  const double dx = b[0] - b[2];
  const double dy = b[1] - b[3];
  const double angle = -std::atan2(dy, dx);
  const double sa = std::sin(angle);
  const double ca = std::cos(angle);

  // rotate points
  for (unsigned int i = 0; i < num_pts; ++i)
  {
    const double v1 = ca * b[2 * i] - sa * b[2 * i + 1];
    const double v2 = sa * b[2 * i] + ca * b[2 * i + 1];
    b[2 * i] = v1;
    b[2 * i + 1] = v2;
  }
  // rotate cameras
  for (unsigned int i = 0; i < a.size() / 3; ++i)
  {
    a[3 * i] -= angle;
  }
}

// compute difference in camera parameters accounting for cyclic angle parameters
vnl_vector<double>
camera_diff(const vnl_vector<double> & a1, const vnl_vector<double> & a2)
{
  vnl_vector<double> da = a1 - a2;
  for (unsigned int i = 0; i < da.size(); i += 3)
  {
    if (da[i] > vnl_math::pi)
      da[i] -= vnl_math::twopi;
    if (da[i] < -vnl_math::pi)
      da[i] += vnl_math::twopi;
  }
  return da;
}


// all ai.size() == 3, all bj.size() == 2, all fxij.size() == 1
// this problem solve for 2d to 1d projection camera and 2d points
// camera matrices are f=1; ai[0] = ang; ai[1] = tx; ai[1] = ty;
// where projection is |u| = |f 0| |cos(ang) -sin(ang) tx| |bj[0]|
//                     |v| = |0 1| |sin(ang)  cos(ang) ty| |bj[1]|
//                                                         |  1  |
// and inhomogeneous error is measured as (u/v - data)
class bundle_2d : public vnl_sparse_lst_sqr_function
{
public:
  bundle_2d(unsigned int num_cam,
            unsigned int num_pts,
            vnl_vector<double> data,
            const std::vector<std::vector<bool>> & xmask,
            UseGradient g = use_gradient,
            UseWeights w = no_weights)
    : vnl_sparse_lst_sqr_function(num_cam, 3, num_pts, 2, 0, xmask, 1, g, w)
    , data_(std::move(data))
  {}

  void
  fij(int i,
      int j,
      const vnl_vector<double> & ai,
      const vnl_vector<double> & bj,
      const vnl_vector<double> & /*c*/,
      vnl_vector<double> & fxij) override
  {
    const double sa = std::sin(ai[0]);
    const double ca = std::cos(ai[0]);
    fxij[0] = (ca * bj[0] - sa * bj[1] + ai[1]) / (sa * bj[0] + ca * bj[1] + ai[2]) - data_[residual_indices_(i, j)];
  }

  void
  jac_Aij(unsigned int /*i*/,
          unsigned int /*j*/,
          const vnl_vector<double> & ai,
          const vnl_vector<double> & bj,
          const vnl_vector<double> & /*c*/,
          vnl_matrix<double> & Aij) override
  {
    const double sa = std::sin(ai[0]);
    const double ca = std::cos(ai[0]);
    const double denom = (sa * bj[0] + ca * bj[1] + ai[2]);
    Aij[0][0] =
      -((sa * bj[0] + ca * bj[1]) + (ca * bj[0] - sa * bj[1] + ai[1]) * (ca * bj[0] - sa * bj[1]) / denom) / denom;
    Aij[0][1] = 1 / denom;
    Aij[0][2] = -(ca * bj[0] - sa * bj[1] + ai[1]) / (denom * denom);
  }

  void
  jac_Bij(unsigned int /*i*/,
          unsigned int /*j*/,
          const vnl_vector<double> & ai,
          const vnl_vector<double> & bj,
          const vnl_vector<double> & /*c*/,
          vnl_matrix<double> & Bij) override
  {
    const double sa = std::sin(ai[0]);
    const double ca = std::cos(ai[0]);
    const double denom = (sa * bj[0] + ca * bj[1] + ai[2]);
    const double numer = (ca * bj[0] - sa * bj[1] + ai[1]);
    Bij[0][0] = (ca - sa * numer / denom) / denom;
    Bij[0][1] = (-sa - ca * numer / denom) / denom;
  }

  void
  trace(int /*iteration*/,
        const vnl_vector<double> & /*a*/,
        const vnl_vector<double> & /*b*/,
        const vnl_vector<double> & /*c*/,
        const vnl_vector<double> & /*e*/) override
  {
    // std::cout << "trace "<<iteration<< " a: "<<a<<std::endl;
  }

  vnl_vector<double> data_;
};


void
test_prob1()
{
  const std::vector<bool> null_row(25, true);
  std::vector<std::vector<bool>> mask(4, null_row);

  const double a_data[] = { 0.0, 0.0, 0.0, 0.8, 10.0, 8.0, -0.7, -8.5, 8.5, 0.4, 4.0, 4.0 };
  const double b_data[] = { -4.0, 8.0,  -2.0, 8.0,  0.0,  8.0,  2.0,  8.0,  4.0,  8.0,  -4.0, 10.0, -2.0,
                            10.0, 0.0,  10.0, 2.0,  10.0, 4.0,  10.0, -4.0, 12.0, -2.0, 12.0, 0.0,  12.0,
                            2.0,  12.0, 4.0,  12.0, -4.0, 14.0, -2.0, 14.0, 0.0,  14.0, 2.0,  14.0, 4.0,
                            14.0, -4.0, 16.0, -2.0, 16.0, 0.0,  16.0, 2.0,  16.0, 4.0,  16.0 };

  vnl_vector<double> a(a_data, 12);
  vnl_vector<double> b(b_data, 50);
  vnl_vector<double> proj(100, 0.0);
  const vnl_vector<double> c;

  // create a generator function with ideal data and zeros for all projections
  // the residuals of this functions are the ideal project points
  bundle_2d gen_func(4, 25, proj, mask, vnl_sparse_lst_sqr_function::use_gradient);
  gen_func.f(a, b, c, proj);

  // scale, translate, and rotate such that the mean is zero,
  // the average distance to the origin is 1, and the vector between the
  // first two points is on the x-axis
  normalize(a, b);

  vnl_vector<double> proj_test(100, 0.0);
  gen_func.f(a, b, c, proj_test);
  std::cout << "test normalization: " << (proj - proj_test).rms() << std::endl;

  // test 2D bundle adjustment with all data and no noise
  {
    // initial conditions (all points at origin)
    vnl_vector<double> pa(12, 0.0);
    vnl_vector<double> pb(50, 0.0);
    vnl_vector<double> pc;
    pa[2] = pa[5] = pa[8] = pa[11] = 10;
    pa[4] = 5;
    pa[7] = -5;
    pa[10] = -2;

    bundle_2d my_func(4, 25, proj, mask, vnl_sparse_lst_sqr_function::use_gradient);

    vnl_sparse_lm slm(my_func);
    // slm.set_verbose(true);
    // slm.set_trace(true);
    slm.minimize(pa, pb, pc);
    slm.diagnose_outcome();

    // scale, translate, and rotate such that the mean is zero,
    // the average distance to the origin is 1, and the vector between the
    // first two points is on the x-axis
    normalize(pa, pb);

#ifdef DEBUG
    std::cout << a << '|' << b << '\n' << pa << '|' << pb << std::endl;
#endif

    double const rms_error_a = camera_diff(a, pa).rms();
    const double rms_error_b = (b - pb).rms();
    std::cout << "RMS camera error: " << rms_error_a << "\nRMS points error: " << rms_error_b << std::endl;
    TEST("convergence with all projections", rms_error_a + rms_error_b < 1e-10, true);
  }

  // remove several correspondences
  // we must see each point in at least 2 views
  // we must leave >= 62 residuals (since there are 62 unknowns)
  mask[0][1] = false;
  mask[0][17] = false;
  mask[0][18] = false;
  mask[0][19] = false;
  mask[1][2] = false;
  mask[1][6] = false;
  mask[1][8] = false;
  mask[1][15] = false;
  mask[1][16] = false;
  mask[2][0] = false;
  mask[2][11] = false;
  mask[2][12] = false;

  // create a subset of projections based on the mask
  const vnl_crs_index crs(mask);
  vnl_vector<double> proj2(crs.num_non_zero());
  for (int i = 0; i < crs.num_rows(); ++i)
  {
    for (int j = 0; j < crs.num_cols(); ++j)
    {
      const int k = crs(i, j);
      if (k >= 0)
        proj2[k] = proj[i * crs.num_cols() + j];
    }
  }

  // test 2D bundle adjustment with missing data and no noise
  {
    // initial conditions (all points at origin)
    vnl_vector<double> pa(12, 0.0);
    vnl_vector<double> pb(50, 0.0);
    vnl_vector<double> pc;
    pa[2] = pa[5] = pa[8] = pa[11] = 10;
    pa[4] = 5;
    pa[7] = -5;
    pa[10] = -2;

    bundle_2d my_func(4, 25, proj2, mask, vnl_sparse_lst_sqr_function::use_gradient);

    vnl_sparse_lm slm(my_func);
    slm.minimize(pa, pb, pc);
    slm.diagnose_outcome();

    // scale, translate, and rotate such that the mean is zero,
    // the average distance to the origin is 1, and the vector between the
    // first two points is on the x-axis
    normalize(pa, pb);

    const double rms_error_a = camera_diff(a, pa).rms();
    const double rms_error_b = (b - pb).rms();
    std::cout << "RMS camera error: " << rms_error_a << "\nRMS points error: " << rms_error_b << std::endl;
    TEST("convergence with missing projections", rms_error_a + rms_error_b < 1e-10, true);
  }

  vnl_random rnd;

  // add uniform random noise to each measurement
  for (double & i : proj2)
  {
    i += (rnd.drand32() - 0.5) * 1e-6;
  }

  // test 2D bundle adjustment with missing data and uniform noise
  {
    // initial conditions (all points at origin)
    vnl_vector<double> pa(12, 0.0);
    vnl_vector<double> pb(50, 0.0);
    vnl_vector<double> pc;
    pa[2] = pa[5] = pa[8] = pa[11] = 10;
    pa[4] = 5;
    pa[7] = -5;
    pa[10] = -2;
    ;

    bundle_2d my_func(4, 25, proj2, mask, vnl_sparse_lst_sqr_function::use_gradient);

    vnl_sparse_lm slm(my_func);
    // slm.set_verbose(true);
    slm.minimize(pa, pb, pc);
    slm.diagnose_outcome();

    // scale, translate, and rotate such that the mean is zero,
    // the average distance to the origin is 1, and the vector between the
    // first two points is on the x-axis
    normalize(pa, pb);

    const double rms_error_a = camera_diff(a, pa).rms();
    const double rms_error_b = (b - pb).rms();
    std::cout << "RMS camera error: " << rms_error_a << "\nRMS points error: " << rms_error_a << std::endl;
    TEST("convergence with missing projections and noise", rms_error_a < 1e-4 && rms_error_b < 1e-4, true);
  }
}


//----------------------------------------------------------------------------


// all ai.size() == 3, all bj.size() == 2, all fxij.size() == 1, size c == 1
// this problem solve for 2d to 1d projection camera and 2d points
// camera matrices are c[0] = f; ai[0] = ang; ai[1] = tx; ai[2] = ty;
// where projection is |u| = |f 0| |cos(ang) -sin(ang) tx| |bj[0]|
//                     |v| = |0 1| |sin(ang)  cos(ang) ty| |bj[1]|
//                                                         |  1  |
// and inhomogeneous error is measured as (u/v - data)
class bundle_2d_shared : public vnl_sparse_lst_sqr_function
{
public:
  bundle_2d_shared(unsigned int num_cam,
                   unsigned int num_pts,
                   vnl_vector<double> data,
                   const std::vector<std::vector<bool>> & xmask,
                   UseGradient g = use_gradient)
    : vnl_sparse_lst_sqr_function(num_cam, 3, num_pts, 2, 1, xmask, 1, g)
    , data_(std::move(data))
  {}

  void
  fij(int i,
      int j,
      const vnl_vector<double> & ai,
      const vnl_vector<double> & bj,
      const vnl_vector<double> & c,
      vnl_vector<double> & fxij) override
  {
    const double sa = std::sin(ai[0]);
    const double ca = std::cos(ai[0]);
    fxij[0] =
      c[0] * (ca * bj[0] - sa * bj[1] + ai[1]) / (sa * bj[0] + ca * bj[1] + ai[2]) - data_[residual_indices_(i, j)];
  }

  void
  jac_Aij(unsigned int /*i*/,
          unsigned int /*j*/,
          const vnl_vector<double> & ai,
          const vnl_vector<double> & bj,
          const vnl_vector<double> & c,
          vnl_matrix<double> & Aij) override
  {
    const double sa = std::sin(ai[0]);
    const double ca = std::cos(ai[0]);
    const double denom = (sa * bj[0] + ca * bj[1] + ai[2]);
    Aij[0][0] = -c[0] *
                ((sa * bj[0] + ca * bj[1]) + (ca * bj[0] - sa * bj[1] + ai[1]) * (ca * bj[0] - sa * bj[1]) / denom) /
                denom;
    Aij[0][1] = c[0] / denom;
    Aij[0][2] = -c[0] * (ca * bj[0] - sa * bj[1] + ai[1]) / (denom * denom);
  }

  void
  jac_Bij(unsigned int /*i*/,
          unsigned int /*j*/,
          const vnl_vector<double> & ai,
          const vnl_vector<double> & bj,
          const vnl_vector<double> & c,
          vnl_matrix<double> & Bij) override
  {
    const double sa = std::sin(ai[0]);
    const double ca = std::cos(ai[0]);
    const double denom = (sa * bj[0] + ca * bj[1] + ai[2]);
    const double numer = c[0] * (ca * bj[0] - sa * bj[1] + ai[1]);
    Bij[0][0] = (c[0] * ca - sa * numer / denom) / denom;
    Bij[0][1] = (-c[0] * sa - ca * numer / denom) / denom;
  }

  void
  jac_Cij(unsigned int /*i*/,
          unsigned int /*j*/,
          const vnl_vector<double> & ai,
          const vnl_vector<double> & bj,
          const vnl_vector<double> & /*c*/,
          vnl_matrix<double> & Cij) override
  {
    const double sa = std::sin(ai[0]);
    const double ca = std::cos(ai[0]);
    const double denom = (sa * bj[0] + ca * bj[1] + ai[2]);
    Cij[0][0] = (ca * bj[0] - sa * bj[1] + ai[1]) / denom;
  }

  vnl_vector<double> data_;
};


void
test_prob2()
{
  const std::vector<bool> null_row(25, true);
  std::vector<std::vector<bool>> mask(4, null_row);

  const double a_data[] = { 0.0, 0.0, 0.0, 0.8, 10.0, 8.0, -0.7, -8.5, 8.5, 0.4, 4.0, 4.0 };
  const double b_data[] = { -4.0, 8.0,  -2.0, 8.0,  0.0,  8.0,  2.0,  8.0,  4.0,  8.0,  -4.0, 10.0, -2.0,
                            10.0, 0.0,  10.0, 2.0,  10.0, 4.0,  10.0, -4.0, 12.0, -2.0, 12.0, 0.0,  12.0,
                            2.0,  12.0, 4.0,  12.0, -4.0, 14.0, -2.0, 14.0, 0.0,  14.0, 2.0,  14.0, 4.0,
                            14.0, -4.0, 16.0, -2.0, 16.0, 0.0,  16.0, 2.0,  16.0, 4.0,  16.0 };

  vnl_vector<double> a(a_data, 12);
  vnl_vector<double> b(b_data, 50);
  const vnl_vector<double> c(1, 1.5);
  vnl_vector<double> proj(100, 0.0);

  // create a generator function with ideal data and zeros for all projections
  // the residuals of this functions are the ideal project points
  bundle_2d_shared gen_func(4, 25, proj, mask, vnl_sparse_lst_sqr_function::use_gradient);
  gen_func.f(a, b, c, proj);

  // scale, translate, and rotate such that the mean is zero,
  // the average distance to the origin is 1, and the vector between the
  // first two points is on the x-axis
  normalize(a, b);

  // test 2D bundle adjustment with all data and no noise
  {
    // initial conditions (all points at origin)
    vnl_vector<double> pa(12, 0.0);
    vnl_vector<double> pb(50, 0.0);
    vnl_vector<double> pc(1, 1.0);
    pa[2] = pa[5] = pa[8] = pa[11] = 10;
    pa[4] = 5;
    pa[7] = -5;
    pa[10] = -2;

    bundle_2d_shared my_func(4, 25, proj, mask, vnl_sparse_lst_sqr_function::use_gradient);

    vnl_sparse_lm slm(my_func);
    // slm.set_verbose(true);
    slm.minimize(pa, pb, pc);
    slm.diagnose_outcome();

    // scale, translate, and rotate such that the mean is zero,
    // the average distance to the origin is 1, and the vector between the
    // first two points is on the x-axis
    normalize(pa, pb);

    const double rms_error_a = camera_diff(a, pa).rms();
    const double rms_error_b = (b - pb).rms();
    const double rms_error_c = (c - pc).rms();
    std::cout << "RMS camera error: " << rms_error_a << "\nRMS points error: " << rms_error_b
              << "\nRMS globals error: " << rms_error_c << std::endl;
    TEST("w/ globals: convergence with all projections", rms_error_a + rms_error_b + rms_error_c < 1e-10, true);
  }

  // remove several correspondences
  // we must see each point in at least 2 views
  // we must leave >= 62 residuals (since there are 62 unknowns)
  mask[0][1] = false;
  mask[0][17] = false;
  mask[0][18] = false;
  mask[0][19] = false;
  mask[1][2] = false;
  mask[1][6] = false;
  mask[1][7] = false;
  mask[1][8] = false;
  mask[1][15] = false;
  mask[1][16] = false;
  mask[2][0] = false;
  mask[2][10] = false;
  mask[2][11] = false;
  mask[2][12] = false;

  // create a subset of projections based on the mask
  const vnl_crs_index crs(mask);
  vnl_vector<double> proj2(crs.num_non_zero());
  for (int i = 0; i < crs.num_rows(); ++i)
  {
    for (int j = 0; j < crs.num_cols(); ++j)
    {
      const int k = crs(i, j);
      if (k >= 0)
        proj2[k] = proj[i * crs.num_cols() + j];
    }
  }

  // test 2D bundle adjustment with missing data and no noise
  {
    // initial conditions (all points at origin)
    vnl_vector<double> pa(12, 0.0);
    vnl_vector<double> pb(50, 0.0);
    vnl_vector<double> pc(1, 1.0);
    pa[2] = pa[5] = pa[8] = pa[11] = 10;
    pa[4] = 5;
    pa[7] = -5;
    pa[10] = -2;

    bundle_2d_shared my_func(4, 25, proj2, mask, vnl_sparse_lst_sqr_function::use_gradient);

    vnl_sparse_lm slm(my_func);
    slm.minimize(pa, pb, pc);
    slm.diagnose_outcome();

    // scale, translate, and rotate such that the mean is zero,
    // the average distance to the origin is 1, and the vector between the
    // first two points is on the x-axis
    normalize(pa, pb);

    const double rms_error_a = camera_diff(a, pa).rms();
    const double rms_error_b = (b - pb).rms();
    const double rms_error_c = (c - pc).rms();
    std::cout << "RMS camera error: " << rms_error_a << "\nRMS points error: " << rms_error_b
              << "\nRMS globals error: " << rms_error_c << std::endl;
    TEST("w/ globals: convergence with missing projections", rms_error_a + rms_error_b + rms_error_c < 1e-10, true);
  }

  vnl_random rnd;

  // add uniform random noise to each measurement
  for (double & i : proj2)
  {
    i += (rnd.drand32() - 0.5) * 1e-6;
  }

  // test 2D bundle adjustment with missing data and uniform noise
  {
    // initial conditions (all points at origin)
    vnl_vector<double> pa(12, 0.0);
    vnl_vector<double> pb(50, 0.0);
    vnl_vector<double> pc(1, 1.0);
    pa[2] = pa[5] = pa[8] = pa[11] = 10;
    pa[4] = 5;
    pa[7] = -5;
    pa[10] = -2;

    bundle_2d_shared my_func(4, 25, proj2, mask, vnl_sparse_lst_sqr_function::use_gradient);

    vnl_sparse_lm slm(my_func);
    // slm.set_verbose(true);
    slm.minimize(pa, pb, pc);
    slm.diagnose_outcome();

    // scale, translate, and rotate such that the mean is zero,
    // the average distance to the origin is 1, and the vector between the
    // first two points is on the x-axis
    normalize(pa, pb);

    const double rms_error_a = camera_diff(a, pa).rms();
    ;
    const double rms_error_b = (b - pb).rms();
    const double rms_error_c = (c - pc).rms();
    std::cout << "RMS camera error: " << rms_error_a << "\nRMS points error: " << rms_error_b
              << "\nRMS globals error: " << rms_error_c << std::endl;
    TEST("w/ globals: convergence with missing projections and noise",
         rms_error_a < 1e-4 && rms_error_b < 1e-4 && rms_error_c < 1e-4,
         true);
  }
}


//----------------------------------------------------------------------------


// same as bundle_2d but with robust estimation
class bundle_2d_robust : public bundle_2d
{
public:
  bundle_2d_robust(unsigned int num_cam,
                   unsigned int num_pts,
                   const vnl_vector<double> & data,
                   const std::vector<std::vector<bool>> & xmask,
                   UseGradient g = use_gradient)
    : bundle_2d(num_cam, num_pts, data, xmask, g, use_weights)

  {}

  void
  set_scale(double scale)
  {
    scale2_ = scale * scale;
  }

  void
  compute_weight_ij(int i,
                    int j,
                    const vnl_vector<double> & /*ai*/,
                    const vnl_vector<double> & /*bj*/,
                    const vnl_vector<double> & /*c*/,
                    const vnl_vector<double> & fij,
                    double & weight) override
  {
    const int k = residual_indices_(i, j);
    assert(k >= 0);
    const double ek2 = fij.squared_magnitude();
    weight = std::sqrt(mest(k, ek2));
  }

  double
  mest(int /*k*/, double ek2) const
  {
    // Beaton-Tukey
    if (ek2 > scale2_)
      return 0.0;
    else
    {
      const double tmp = 1 - ek2 / scale2_;
      return tmp * tmp;
    }
  }

  double
  d_mest(int /*k*/, double ek2) const
  {
    // Beaton-Tukey
    if (ek2 > scale2_)
      return 0.0;
    else
      return -2 * (1 - ek2 / scale2_) / scale2_;
  }

  void
  trace(int /*iteration*/,
        const vnl_vector<double> & /*a*/,
        const vnl_vector<double> & /*b*/,
        const vnl_vector<double> & /*c*/,
        const vnl_vector<double> & /*e*/) override
  {
    // std::cout << "trace "<<iteration<< " a: "<<a<<std::endl;
  }

  double scale2_{ 1.0 };
};


void
test_prob3()
{
  const std::vector<bool> null_row(25, true);
  std::vector<std::vector<bool>> mask(4, null_row);

  const double a_data[] = { 0.0, 0.0, 0.0, 0.8, 10.0, 8.0, -0.7, -8.5, 8.5, 0.4, 4.0, 4.0 };
  const double b_data[] = { -4.0, 8.0,  -2.0, 8.0,  0.0,  8.0,  2.0,  8.0,  4.0,  8.0,  -4.0, 10.0, -2.0,
                            10.0, 0.0,  10.0, 2.0,  10.0, 4.0,  10.0, -4.0, 12.0, -2.0, 12.0, 0.0,  12.0,
                            2.0,  12.0, 4.0,  12.0, -4.0, 14.0, -2.0, 14.0, 0.0,  14.0, 2.0,  14.0, 4.0,
                            14.0, -4.0, 16.0, -2.0, 16.0, 0.0,  16.0, 2.0,  16.0, 4.0,  16.0 };

  vnl_vector<double> a(a_data, 12);
  vnl_vector<double> b(b_data, 50);
  vnl_vector<double> proj(100, 0.0);
  const vnl_vector<double> c;

  // initial perturbed parameters, add random gaussian noise
  vnl_vector<double> init_a(a_data, 12);
  vnl_vector<double> init_b(b_data, 50);
  const double sigma_pos = 1.0;
  const double sigma_ang = 0.1;
  vnl_random rnd(1234);
  for (unsigned i = 0; i < init_a.size() / 3; ++i)
  {
    init_a[3 * i] += rnd.normal() * sigma_ang;
    init_a[3 * i + 1] += rnd.normal() * sigma_pos;
    init_a[3 * i + 2] += rnd.normal() * sigma_pos;
  }
  for (double & i : init_b)
  {
    i += rnd.normal() * sigma_pos;
  }

  // create a generator function with ideal data and zeros for all projections
  // the residuals of this functions are the ideal project points
  bundle_2d gen_func(4, 25, proj, mask, vnl_sparse_lst_sqr_function::use_gradient);
  gen_func.f(a, b, c, proj);

  {
    bundle_2d_robust func(4, 25, proj, mask, vnl_sparse_lst_sqr_function::use_gradient);
    func.set_scale(0.3);

    vnl_matrix<double> A1(1, 3);
    vnl_matrix<double> A2(1, 3);
    vnl_matrix<double> B1(1, 2);
    vnl_matrix<double> B2(1, 2);
    vnl_vector<double> ai(3, 0.0);
    vnl_vector<double> bj(2, 0.0);
    vnl_vector<double> fxij(1, 0.0);
    ai[0] = 0.1;
    ai[1] = -0.0;
    ai[2] = 0.0;
    bj[0] = -2;
    bj[1] = 10000;
    func.fij(0, 0, ai, bj, c, fxij);
    const double e2 = fxij.squared_magnitude();
    const double m1 = func.mest(0, e2);
    const double dm1 = func.d_mest(0, e2);
    const double dm2 = (func.mest(0, e2 + 1e-8) - m1) / (1e-8);
    TEST_NEAR("derive mest = finite diff", dm1, dm2, 1e-4);
    std::cout << fxij << std::endl;
    func.jac_Aij(0, 0, ai, bj, c, A1);
    func.jac_Bij(0, 0, ai, bj, c, B1);
    func.fd_jac_Aij(0, 0, ai, bj, c, A2, 1e-8);
    func.fd_jac_Bij(0, 0, ai, bj, c, B2, 1e-8);
    TEST_NEAR("Jacobian A = finite diff", (A1 - A2).array_inf_norm(), 0.0, 1e-4);
    TEST_NEAR("Jacobian B = finite diff", (B1 - B2).array_inf_norm(), 0.0, 1e-4);
  }

  // scale, translate, and rotate such that the mean is zero,
  // the average distance to the origin is 1, and the vector between the
  // first two points is on the x-axis
  normalize(a, b);

  // test 2D bundle adjustment with all data and no noise
  {
    // initial conditions (all points at origin)
    vnl_vector<double> pa(init_a);
    vnl_vector<double> pb(init_b);
    vnl_vector<double> pc;

    bundle_2d_robust my_func(4, 25, proj, mask, vnl_sparse_lst_sqr_function::use_gradient);
    my_func.set_scale(1.0);

    vnl_sparse_lm slm(my_func);
    slm.set_verbose(true);
    // slm.set_trace(true);
    slm.minimize(pa, pb, pc);
    slm.diagnose_outcome();

    // scale, translate, and rotate such that the mean is zero,
    // the average distance to the origin is 1, and the vector between the
    // first two points is on the x-axis
    normalize(pa, pb);

#ifdef DEBUG
    std::cout << a << '|' << b << '\n' << pa << '|' << pb << std::endl;
#endif

    double const rms_error_a = camera_diff(a, pa).rms();
    const double rms_error_b = (b - pb).rms();
    std::cout << "RMS camera error: " << rms_error_a << "\nRMS points error: " << rms_error_b << std::endl;
    TEST("robust convergence with all projections", rms_error_a + rms_error_b < 1e-10, true);
  }

  // remove several correspondences and add outliers
  // we must see each point in at least 2 views
  // we must leave >= 62 residuals (since there are 62 unknowns)

  mask[0][1] = false;
  mask[0][17] = false;
  mask[0][18] = false;
  mask[1][2] = false;
  mask[1][6] = false;
  mask[1][15] = false;
  mask[1][16] = false;
  mask[2][0] = false;
  mask[2][11] = false;
  mask[2][12] = false;

  // outliers
  proj[19] += 1.0;
  proj[33] -= 20.0;

  // create a subset of projections based on the mask
  const vnl_crs_index crs(mask);
  vnl_vector<double> proj2(crs.num_non_zero());
  for (int i = 0; i < crs.num_rows(); ++i)
  {
    for (int j = 0; j < crs.num_cols(); ++j)
    {
      const int k = crs(i, j);
      if (k >= 0)
        proj2[k] = proj[i * crs.num_cols() + j];
    }
  }

  // test 2D bundle adjustment with missing data and no noise
  {
    // initial conditions (all points at origin)
    vnl_vector<double> pa(init_a);
    vnl_vector<double> pb(init_b);
    vnl_vector<double> pc;

    bundle_2d_robust my_func(4, 25, proj2, mask, vnl_sparse_lst_sqr_function::use_gradient);
    my_func.set_scale(1.0);

    vnl_sparse_lm slm(my_func);
    // slm.set_verbose(true);
    slm.minimize(pa, pb, pc);
    slm.diagnose_outcome();

    // scale, translate, and rotate such that the mean is zero,
    // the average distance to the origin is 1, and the vector between the
    // first two points is on the x-axis
    normalize(pa, pb);

    const double rms_error_a = camera_diff(a, pa).rms();
    const double rms_error_b = (b - pb).rms();
    std::cout << "RMS camera error: " << rms_error_a << "\nRMS points error: " << rms_error_b << std::endl;
    TEST("convergence with missing projections", rms_error_a + rms_error_b < 1e-10, true);
  }

  // add uniform random noise to each measurement
  for (double & i : proj2)
  {
    i += (rnd.drand32() - 0.5) * 1e-6;
  }

  // test 2D bundle adjustment with missing data and uniform noise
  {
    // initial conditions (all points at origin)
    vnl_vector<double> pa(init_a);
    vnl_vector<double> pb(init_b);
    vnl_vector<double> pc;

    bundle_2d_robust my_func(4, 25, proj2, mask, vnl_sparse_lst_sqr_function::use_gradient);
    my_func.set_scale(1.0);

    vnl_sparse_lm slm(my_func);
    // slm.set_verbose(true);
    slm.minimize(pa, pb, pc);
    slm.diagnose_outcome();

    // scale, translate, and rotate such that the mean is zero,
    // the average distance to the origin is 1, and the vector between the
    // first two points is on the x-axis
    normalize(pa, pb);

    const double rms_error_a = camera_diff(a, pa).rms();
    const double rms_error_b = (b - pb).rms();
    std::cout << "RMS camera error: " << rms_error_a << "\nRMS points error: " << rms_error_b << std::endl;
    TEST("convergence with missing projections and noise", rms_error_a < 1e-4 && rms_error_b < 1e-4, true);
  }
}


static void
test_sparse_lm()
{
  test_prob1();
  test_prob2();
  test_prob3();
}

TESTMAIN(test_sparse_lm);
