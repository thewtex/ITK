#include "v3p_f2c_original.h"

struct dsaupd_static {
  v3p_netlib_integer nb, ih, iq, np, iw, ldh, ldq, nev0, mode, ierr, iupd, next, ritz, bounds, ishift, mxiter;
};

struct dsaup2_static {
  v3p_netlib_integer np0, nev0, iter, nconv, kplusp;
  v3p_netlib_logical getv0, cnorm, initv, update, ushift;
  v3p_netlib_doublereal eps23, rnorm;
};

struct dsapps_static {
  v3p_netlib_logical first;
  v3p_netlib_doublereal epsmch;
};

struct dsaitr_static {
  v3p_netlib_logical first, orth1, orth2, step3, step4, rstart;
  v3p_netlib_integer j, ipj, irj, ivj, ierr, iter, itry;
  v3p_netlib_doublereal wnorm, rnorm1, safmin;
};

struct dgetv0_static {
  v3p_netlib_logical inits, orth, first;
  v3p_netlib_integer iter, iseed[4];
  v3p_netlib_doublereal rnorm0;
};

struct dsaupd_workspace {
  struct dsaupd_static dsaupd_workspace;
  struct dsaup2_static dsaup2_workspace;
  struct dsapps_static dsapps_workspace;
  struct dsaitr_static dsaitr_workspace;
  struct dgetv0_static dgetv0_workspace;
};
