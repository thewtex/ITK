/* arpack/dsaupd.f -- translated by f2c (version 20090411).
   You must link the resulting object file with libf2c:
        on Microsoft Windows system, link with libf2c.lib;
        on Linux or Unix systems, link with .../path/to/libf2c.a -lm
        or, if you install libf2c.a in a standard place, with -lf2c -lm
        -- in that order, at the end of the command line, as in
                cc *.o -lf2c -lm
        Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

                http://www.netlib.org/f2c/libf2c.zip
*/

#ifdef __cplusplus
extern "C" {
#endif
#include "v3p_netlib.h"

/* Common Block Declarations */

/*Extern struct { */
/*  integer logfil, ndigit, mgetv0, msaupd, msaup2, msaitr, mseigt, msapps, */
/*          msgets, mseupd, mnaupd, mnaup2, mnaitr, mneigh, mnapps, mngets, */
/*          mneupd, mcaupd, mcaup2, mcaitr, mceigh, mcapps, mcgets, mceupd; */
/*} debug_; */

/*#define debug_1 debug_ */

/*Extern struct { */
/*  integer nopx, nbx, nrorth, nitref, nrstrt; */
/*  real tsaupd, tsaup2, tsaitr, tseigt, tsgets, tsapps, tsconv, tnaupd, */
/*          tnaup2, tnaitr, tneigh, tngets, tnapps, tnconv, tcaupd, tcaup2, */
/*          tcaitr, tceigh, tcgets, tcapps, tcconv, tmvopx, tmvbx, tgetv0, */
/*          titref, trvec; */
/*} timing_; */

/*#define timing_1 timing_ */

/* ----------------------------------------------------------------------- */
/* \BeginDoc */

/* \Name: dsaupd */

/* \Description: */

/*  Reverse communication interface for the Implicitly Restarted Arnoldi */
/*  Iteration.  For symmetric problems this reduces to a variant of the Lanczos */
/*  method.  This method has been designed to compute approximations to a */
/*  few eigenpairs of a linear operator OP that is real and symmetric */
/*  with respect to a real positive semi-definite symmetric matrix B, */
/*  i.e. */

/*       B*OP = (OP')*B. */

/*  Another way to express this condition is */

/*       < x,OPy > = < OPx,y >  where < z,w > = z'Bw  . */

/*  In the standard eigenproblem B is the identity matrix. */
/*  ( A' denotes transpose of A) */

/*  The computed approximate eigenvalues are called Ritz values and */
/*  the corresponding approximate eigenvectors are called Ritz vectors. */

/*  dsaupd is usually called iteratively to solve one of the */
/*  following problems: */

/*  Mode 1:  A*x = lambda*x, A symmetric */
/*           ===> OP = A  and  B = I. */

/*  Mode 2:  A*x = lambda*M*x, A symmetric, M symmetric positive definite */
/*           ===> OP = inv[M]*A  and  B = M. */
/*           ===> (If M can be factored see remark 3 below) */

/*  Mode 3:  K*x = lambda*M*x, K symmetric, M symmetric positive semi-definite */
/*           ===> OP = (inv[K - sigma*M])*M  and  B = M. */
/*           ===> Shift-and-Invert mode */

/*  Mode 4:  K*x = lambda*KG*x, K symmetric positive semi-definite, */
/*           KG symmetric indefinite */
/*           ===> OP = (inv[K - sigma*KG])*K  and  B = K. */
/*           ===> Buckling mode */

/*  Mode 5:  A*x = lambda*M*x, A symmetric, M symmetric positive semi-definite */
/*           ===> OP = inv[A - sigma*M]*[A + sigma*M]  and  B = M. */
/*           ===> Cayley transformed mode */

/*  NOTE: The action of w <- inv[A - sigma*M]*v or w <- inv[M]*v */
/*        should be accomplished either by a direct method */
/*        using a sparse matrix factorization and solving */

/*           [A - sigma*M]*w = v  or M*w = v, */

/*        or through an iterative method for solving these */
/*        systems.  If an iterative method is used, the */
/*        convergence test must be more stringent than */
/*        the accuracy requirements for the eigenvalue */
/*        approximations. */

/* \Usage: */
/*  call dsaupd */
/*     ( IDO, BMAT, N, WHICH, NEV, TOL, RESID, NCV, V, LDV, IPARAM, */
/*       IPNTR, WORKD, WORKL, LWORKL, INFO ) */

/* \Arguments */
/*  IDO     Integer.  (INPUT/OUTPUT) */
/*          Reverse communication flag.  IDO must be zero on the first */
/*          call to dsaupd.  IDO will be set internally to */
/*          indicate the type of operation to be performed.  Control is */
/*          then given back to the calling routine which has the */
/*          responsibility to carry out the requested operation and call */
/*          dsaupd with the result.  The operand is given in */
/*          WORKD(IPNTR(1)), the result must be put in WORKD(IPNTR(2)). */
/*          (If Mode = 2 see remark 5 below) */
/*          ------------------------------------------------------------- */
/*          IDO =  0: first call to the reverse communication interface */
/*          IDO = -1: compute  Y = OP * X  where */
/*                    IPNTR(1) is the pointer into WORKD for X, */
/*                    IPNTR(2) is the pointer into WORKD for Y. */
/*                    This is for the initialization phase to force the */
/*                    starting vector into the range of OP. */
/*          IDO =  1: compute  Y = OP * X where */
/*                    IPNTR(1) is the pointer into WORKD for X, */
/*                    IPNTR(2) is the pointer into WORKD for Y. */
/*                    In mode 3,4 and 5, the vector B * X is already */
/*                    available in WORKD(ipntr(3)).  It does not */
/*                    need to be recomputed in forming OP * X. */
/*          IDO =  2: compute  Y = B * X  where */
/*                    IPNTR(1) is the pointer into WORKD for X, */
/*                    IPNTR(2) is the pointer into WORKD for Y. */
/*          IDO =  3: compute the IPARAM(8) shifts where */
/*                    IPNTR(11) is the pointer into WORKL for */
/*                    placing the shifts. See remark 6 below. */
/*          IDO = 99: done */
/*          ------------------------------------------------------------- */

/*  BMAT    Character*1.  (INPUT) */
/*          BMAT specifies the type of the matrix B that defines the */
/*          semi-inner product for the operator OP. */
/*          B = 'I' -> standard eigenvalue problem A*x = lambda*x */
/*          B = 'G' -> generalized eigenvalue problem A*x = lambda*B*x */

/*  N       Integer.  (INPUT) */
/*          Dimension of the eigenproblem. */

/*  WHICH   Character*2.  (INPUT) */
/*          Specify which of the Ritz values of OP to compute. */

/*          'LA' - compute the NEV largest (algebraic) eigenvalues. */
/*          'SA' - compute the NEV smallest (algebraic) eigenvalues. */
/*          'LM' - compute the NEV largest (in magnitude) eigenvalues. */
/*          'SM' - compute the NEV smallest (in magnitude) eigenvalues. */
/*          'BE' - compute NEV eigenvalues, half from each end of the */
/*                 spectrum.  When NEV is odd, compute one more from the */
/*                 high end than from the low end. */
/*           (see remark 1 below) */

/*  NEV     Integer.  (INPUT) */
/*          Number of eigenvalues of OP to be computed. 0 < NEV < N. */

/*  TOL     Double precision scalar.  (INPUT) */
/*          Stopping criterion: the relative accuracy of the Ritz value */
/*          is considered acceptable if BOUNDS(I) .LE. TOL*ABS(RITZ(I)). */
/*          If TOL .LE. 0. is passed a default is set: */
/*          DEFAULT = DLAMCH('EPS')  (machine precision as computed */
/*                    by the LAPACK auxiliary subroutine DLAMCH). */

/*  RESID   Double precision array of length N.  (INPUT/OUTPUT) */
/*          On INPUT: */
/*          If INFO .EQ. 0, a random initial residual vector is used. */
/*          If INFO .NE. 0, RESID contains the initial residual vector, */
/*                          possibly from a previous run. */
/*          On OUTPUT: */
/*          RESID contains the final residual vector. */

/*  NCV     Integer.  (INPUT) */
/*          Number of columns of the matrix V (less than or equal to N). */
/*          This will indicate how many Lanczos vectors are generated */
/*          at each iteration.  After the startup phase in which NEV */
/*          Lanczos vectors are generated, the algorithm generates */
/*          NCV-NEV Lanczos vectors at each subsequent update iteration. */
/*          Most of the cost in generating each Lanczos vector is in the */
/*          matrix-vector product OP*x. (See remark 4 below). */

/*  V       Double precision N by NCV array.  (OUTPUT) */
/*          The NCV columns of V contain the Lanczos basis vectors. */

/*  LDV     Integer.  (INPUT) */
/*          Leading dimension of V exactly as declared in the calling */
/*          program. */

/*  IPARAM  Integer array of length 11.  (INPUT/OUTPUT) */
/*          IPARAM(1) = ISHIFT: method for selecting the implicit shifts. */
/*          The shifts selected at each iteration are used to restart */
/*          the Arnoldi iteration in an implicit fashion. */
/*          ------------------------------------------------------------- */
/*          ISHIFT = 0: the shifts are provided by the user via */
/*                      reverse communication.  The NCV eigenvalues of */
/*                      the current tridiagonal matrix T are returned in */
/*                      the part of WORKL array corresponding to RITZ. */
/*                      See remark 6 below. */
/*          ISHIFT = 1: exact shifts with respect to the reduced */
/*                      tridiagonal matrix T.  This is equivalent to */
/*                      restarting the iteration with a starting vector */
/*                      that is a linear combination of Ritz vectors */
/*                      associated with the "wanted" Ritz values. */
/*          ------------------------------------------------------------- */

/*          IPARAM(2) = LEVEC */
/*          No longer referenced. See remark 2 below. */

/*          IPARAM(3) = MXITER */
/*          On INPUT:  maximum number of Arnoldi update iterations allowed. */
/*          On OUTPUT: actual number of Arnoldi update iterations taken. */

/*          IPARAM(4) = NB: blocksize to be used in the recurrence. */
/*          The code currently works only for NB = 1. */

/*          IPARAM(5) = NCONV: number of "converged" Ritz values. */
/*          This represents the number of Ritz values that satisfy */
/*          the convergence criterion. */

/*          IPARAM(6) = IUPD */
/*          No longer referenced. Implicit restarting is ALWAYS used. */

/*          IPARAM(7) = MODE */
/*          On INPUT determines what type of eigenproblem is being solved. */
/*          Must be 1,2,3,4,5; See under \Description of dsaupd for the */
/*          five modes available. */

/*          IPARAM(8) = NP */
/*          When ido = 3 and the user provides shifts through reverse */
/*          communication (IPARAM(1)=0), dsaupd returns NP, the number */
/*          of shifts the user is to provide. 0 < NP <=NCV-NEV. See Remark */
/*          6 below. */

/*          IPARAM(9) = NUMOP, IPARAM(10) = NUMOPB, IPARAM(11) = NUMREO, */
/*          OUTPUT: NUMOP  = total number of OP*x operations, */
/*                  NUMOPB = total number of B*x operations if BMAT='G', */
/*                  NUMREO = total number of steps of re-orthogonalization. */

/*  IPNTR   Integer array of length 11.  (OUTPUT) */
/*          Pointer to mark the starting locations in the WORKD and WORKL */
/*          arrays for matrices/vectors used by the Lanczos iteration. */
/*          ------------------------------------------------------------- */
/*          IPNTR(1): pointer to the current operand vector X in WORKD. */
/*          IPNTR(2): pointer to the current result vector Y in WORKD. */
/*          IPNTR(3): pointer to the vector B * X in WORKD when used in */
/*                    the shift-and-invert mode. */
/*          IPNTR(4): pointer to the next available location in WORKL */
/*                    that is untouched by the program. */
/*          IPNTR(5): pointer to the NCV by 2 tridiagonal matrix T in WORKL. */
/*          IPNTR(6): pointer to the NCV RITZ values array in WORKL. */
/*          IPNTR(7): pointer to the Ritz estimates in array WORKL associated */
/*                    with the Ritz values located in RITZ in WORKL. */
/*          IPNTR(11): pointer to the NP shifts in WORKL. See Remark 6 below. */

/*          Note: IPNTR(8:10) is only referenced by dseupd. See Remark 2. */
/*          IPNTR(8): pointer to the NCV RITZ values of the original system. */
/*          IPNTR(9): pointer to the NCV corresponding error bounds. */
/*          IPNTR(10): pointer to the NCV by NCV matrix of eigenvectors */
/*                     of the tridiagonal matrix T. Only referenced by */
/*                     dseupd if RVEC = .TRUE. See Remarks. */
/*          ------------------------------------------------------------- */

/*  WORKD   Double precision work array of length 3*N.  (REVERSE COMMUNICATION) */
/*          Distributed array to be used in the basic Arnoldi iteration */
/*          for reverse communication.  The user should not use WORKD */
/*          as temporary workspace during the iteration. Upon termination */
/*          WORKD(1:N) contains B*RESID(1:N). If the Ritz vectors are desired */
/*          subroutine dseupd uses this output. */
/*          See Data Distribution Note below. */

/*  WORKL   Double precision work array of length LWORKL.  (OUTPUT/WORKSPACE) */
/*          Private (replicated) array on each PE or array allocated on */
/*          the front end.  See Data Distribution Note below. */

/*  LWORKL  Integer.  (INPUT) */
/*          LWORKL must be at least NCV**2 + 8*NCV . */

/*  INFO    Integer.  (INPUT/OUTPUT) */
/*          If INFO .EQ. 0, a randomly initial residual vector is used. */
/*          If INFO .NE. 0, RESID contains the initial residual vector, */
/*                          possibly from a previous run. */
/*          Error flag on output. */
/*          =  0: Normal exit. */
/*          =  1: Maximum number of iterations taken. */
/*                All possible eigenvalues of OP has been found. IPARAM(5) */
/*                returns the number of wanted converged Ritz values. */
/*          =  2: No longer an informational error. Deprecated starting */
/*                with release 2 of ARPACK. */
/*          =  3: No shifts could be applied during a cycle of the */
/*                Implicitly restarted Arnoldi iteration. One possibility */
/*                is to increase the size of NCV relative to NEV. */
/*                See remark 4 below. */
/*          = -1: N must be positive. */
/*          = -2: NEV must be positive. */
/*          = -3: NCV must be greater than NEV and less than or equal to N. */
/*          = -4: The maximum number of Arnoldi update iterations allowed */
/*                must be greater than zero. */
/*          = -5: WHICH must be one of 'LM', 'SM', 'LA', 'SA' or 'BE'. */
/*          = -6: BMAT must be one of 'I' or 'G'. */
/*          = -7: Length of private work array WORKL is not sufficient. */
/*          = -8: Error return from trid. eigenvalue calculation; */
/*                Informatinal error from LAPACK routine dsteqr. */
/*          = -9: Starting vector is zero. */
/*          = -10: IPARAM(7) must be 1,2,3,4,5. */
/*          = -11: IPARAM(7) = 1 and BMAT = 'G' are incompatable. */
/*          = -12: IPARAM(1) must be equal to 0 or 1. */
/*          = -13: NEV and WHICH = 'BE' are incompatable. */
/*          = -9999: Could not build an Arnoldi factorization. */
/*                   IPARAM(5) returns the size of the current Arnoldi */
/*                   factorization. The user is advised to check that */
/*                   enough workspace and array storage has been allocated. */


/* \Remarks */
/*  1. The converged Ritz values are always returned in ascending */
/*     algebraic order.  The computed Ritz values are approximate */
/*     eigenvalues of OP.  The selection of WHICH should be made */
/*     with this in mind when Mode = 3,4,5.  After convergence, */
/*     approximate eigenvalues of the original problem may be obtained */
/*     with the ARPACK subroutine dseupd. */

/*  2. If the Ritz vectors corresponding to the converged Ritz values */
/*     are needed, the user must call dseupd immediately following completion */
/*     of dsaupd. This is new starting with version 2.1 of ARPACK. */

/*  3. If M can be factored into a Cholesky factorization M = LL' */
/*     then Mode = 2 should not be selected.  Instead one should use */
/*     Mode = 1 with  OP = inv(L)*A*inv(L').  Appropriate triangular */
/*     linear systems should be solved with L and L' rather */
/*     than computing inverses.  After convergence, an approximate */
/*     eigenvector z of the original problem is recovered by solving */
/*     L'z = x  where x is a Ritz vector of OP. */

/*  4. At present there is no a-priori analysis to guide the selection */
/*     of NCV relative to NEV.  The only formal requirement is that NCV > NEV. */
/*     However, it is recommended that NCV .ge. 2*NEV.  If many problems of */
/*     the same type are to be solved, one should experiment with increasing */
/*     NCV while keeping NEV fixed for a given test problem.  This will */
/*     usually decrease the required number of OP*x operations but it */
/*     also increases the work and storage required to maintain the orthogonal */
/*     basis vectors.   The optimal "cross-over" with respect to CPU time */
/*     is problem dependent and must be determined empirically. */

/*  5. If IPARAM(7) = 2 then in the Reverse commuication interface the user */
/*     must do the following. When IDO = 1, Y = OP * X is to be computed. */
/*     When IPARAM(7) = 2 OP = inv(B)*A. After computing A*X the user */
/*     must overwrite X with A*X. Y is then the solution to the linear set */
/*     of equations B*Y = A*X. */

/*  6. When IPARAM(1) = 0, and IDO = 3, the user needs to provide the */
/*     NP = IPARAM(8) shifts in locations: */
/*     1   WORKL(IPNTR(11)) */
/*     2   WORKL(IPNTR(11)+1) */
/*                        . */
/*                        . */
/*                        . */
/*     NP  WORKL(IPNTR(11)+NP-1). */

/*     The eigenvalues of the current tridiagonal matrix are located in */
/*     WORKL(IPNTR(6)) through WORKL(IPNTR(6)+NCV-1). They are in the */
/*     order defined by WHICH. The associated Ritz estimates are located in */
/*     WORKL(IPNTR(8)), WORKL(IPNTR(8)+1), ... , WORKL(IPNTR(8)+NCV-1). */

/* ----------------------------------------------------------------------- */

/* \Data Distribution Note: */

/*  Fortran-D syntax: */
/*  ================ */
/*  REAL       RESID(N), V(LDV,NCV), WORKD(3*N), WORKL(LWORKL) */
/*  DECOMPOSE  D1(N), D2(N,NCV) */
/*  ALIGN      RESID(I) with D1(I) */
/*  ALIGN      V(I,J)   with D2(I,J) */
/*  ALIGN      WORKD(I) with D1(I)     range (1:N) */
/*  ALIGN      WORKD(I) with D1(I-N)   range (N+1:2*N) */
/*  ALIGN      WORKD(I) with D1(I-2*N) range (2*N+1:3*N) */
/*  DISTRIBUTE D1(BLOCK), D2(BLOCK,:) */
/*  REPLICATED WORKL(LWORKL) */

/*  Cray MPP syntax: */
/*  =============== */
/*  REAL       RESID(N), V(LDV,NCV), WORKD(N,3), WORKL(LWORKL) */
/*  SHARED     RESID(BLOCK), V(BLOCK,:), WORKD(BLOCK,:) */
/*  REPLICATED WORKL(LWORKL) */


/* \BeginLib */

/* \References: */
/*  1. D.C. Sorensen, "Implicit Application of Polynomial Filters in */
/*     a k-Step Arnoldi Method", SIAM J. Matr. Anal. Apps., 13 (1992), */
/*     pp 357-385. */
/*  2. R.B. Lehoucq, "Analysis and Implementation of an Implicitly */
/*     Restarted Arnoldi Iteration", Rice University Technical Report */
/*     TR95-13, Department of Computational and Applied Mathematics. */
/*  3. B.N. Parlett, "The Symmetric Eigenvalue Problem". Prentice-Hall, */
/*     1980. */
/*  4. B.N. Parlett, B. Nour-Omid, "Towards a Black Box Lanczos Program", */
/*     Computer Physics Communications, 53 (1989), pp 169-179. */
/*  5. B. Nour-Omid, B.N. Parlett, T. Ericson, P.S. Jensen, "How to */
/*     Implement the Spectral Transformation", Math. Comp., 48 (1987), */
/*     pp 663-673. */
/*  6. R.G. Grimes, J.G. Lewis and H.D. Simon, "A Shifted Block Lanczos */
/*     Algorithm for Solving Sparse Symmetric Generalized Eigenproblems", */
/*     SIAM J. Matr. Anal. Apps.,  January (1993). */
/*  7. L. Reichel, W.B. Gragg, "Algorithm 686: FORTRAN Subroutines */
/*     for Updating the QR decomposition", ACM TOMS, December 1990, */
/*     Volume 16 Number 4, pp 369-377. */
/*  8. R.B. Lehoucq, D.C. Sorensen, "Implementation of Some Spectral */
/*     Transformations in a k-Step Arnoldi Method". In Preparation. */

/* \Routines called: */
/*     dsaup2  ARPACK routine that implements the Implicitly Restarted */
/*             Arnoldi Iteration. */
/*     dstats  ARPACK routine that initialize timing and other statistics */
/*             variables. */
/*     second  ARPACK utility routine for timing. */
/*     dlamch  LAPACK routine that determines machine constants. */

/* \Authors */
/*     Danny Sorensen               Phuong Vu */
/*     Richard Lehoucq              CRPC / Rice University */
/*     Dept. of Computational &     Houston, Texas */
/*     Applied Mathematics */
/*     Rice University */
/*     Houston, Texas */

/* \Revision history: */
/*     12/15/93: Version ' 2.4' */

/* \SCCS Information: @(#) */
/* FILE: saupd.F   SID: 2.7   DATE OF SID: 8/27/96   RELEASE: 2 */

/* \Remarks */
/*     1. None */

/* \EndLib */

/* ----------------------------------------------------------------------- */

/*<        >*/
/* Subroutine */ int dsaupd_(struct dsaupd_workspace *workspace,
        integer *ido, char *bmat, integer *n, char *
        which, integer *nev, doublereal *tol, doublereal *resid, integer *ncv,
        doublereal *v, integer *ldv, integer *iparam, integer *ipntr,
        doublereal *workd, doublereal *workl, integer *lworkl, integer *info,
        ftnlen bmat_len, ftnlen which_len)
{
    /* System generated locals */
    integer v_dim1, v_offset, i__1, i__2;

    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    integer j;
/*  static real t0, t1; */
/* MRB - moved to the input dsaupd_static workspace struct
    static integer nb, ih, iq, np, iw, ldh, ldq, nev0, mode, ierr, iupd, next,
             ritz;
*/
    struct dsaupd_static *_;

    extern /* Subroutine */ int dsaup2_(struct dsaupd_workspace *workspace,
            integer *, char *, integer *, char *,
            integer *, integer *, doublereal *, doublereal *, integer *,
            integer *, integer *, integer *, doublereal *, integer *,
            doublereal *, integer *, doublereal *, doublereal *, doublereal *,
             integer *, doublereal *, integer *, doublereal *, integer *,
            ftnlen, ftnlen);
    extern doublereal dlamch_(char *, ftnlen);
    extern /* Subroutine */ int second_(real *);
/* MRB - moved to the input dsaupd_static workspace struct
    static integer bounds, ishift, */ /* msglvl, */ /* mxiter;
*/
    extern /* Subroutine */ int dstats_();


/*     %----------------------------------------------------% */
/*     | Include files for debugging and timing information | */
/*     %----------------------------------------------------% */

/*<       include   'debug.h' >*/
/*<       include   'stat.h' >*/

/* \SCCS Information: @(#) */
/* FILE: debug.h   SID: 2.3   DATE OF SID: 11/16/95   RELEASE: 2 */

/*     %---------------------------------% */
/*     | See debug.doc for documentation | */
/*     %---------------------------------% */
/*<        >*/
/*<       character  bmat*1, which*2 >*/

/*     %------------------% */
/*     | Scalar Arguments | */
/*     %------------------% */

/*     %--------------------------------% */
/*     | See stat.doc for documentation | */
/*     %--------------------------------% */

/* \SCCS Information: @(#) */
/* FILE: stat.h   SID: 2.2   DATE OF SID: 11/16/95   RELEASE: 2 */

/*<       save       t0, t1, t2, t3, t4, t5 >*/

/*<       integer    nopx, nbx, nrorth, nitref, nrstrt >*/
/*<        >*/
/*<        >*/
/*<       integer    ido, info, ldv, lworkl, n, ncv, nev >*/
/*<        >*/

/*     %-----------------% */
/*     | Array Arguments | */
/*     %-----------------% */

/*<       integer    iparam(11), ipntr(11) >*/
/*<        >*/

/*     %------------% */
/*     | Parameters | */
/*     %------------% */

/*<        >*/
/*<       parameter (one = 1.0D+0, zero = 0.0D+0) >*/

/*     %---------------% */
/*     | Local Scalars | */
/*     %---------------% */

/*<        >*/
/*<        >*/

/*     %----------------------% */
/*     | External Subroutines | */
/*     %----------------------% */

/*<       external   dsaup2,  second, dstats >*/

/*     %--------------------% */
/*     | External Functions | */
/*     %--------------------% */

/*<        >*/
/*<       external   dlamch >*/

/*     %-----------------------% */
/*     | Executable Statements | */
/*     %-----------------------% */

/*<       if (ido .eq. 0) then >*/
    /* Parameter adjustments */
    --workd;
    --resid;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    --iparam;
    --ipntr;
    --workl;

    _ = &workspace->dsaupd_workspace;

    /* Function Body */
    if (*ido == 0) {

/*        %-------------------------------% */
/*        | Initialize timing statistics  | */
/*        | & message level for debugging | */
/*        %-------------------------------% */

/*<          call dstats >*/
/*      dstats_(); */
/*<          call second (t0) >*/
/*      second_(&t0); */
/*<          msglvl = msaupd >*/
/*      msglvl = debug_1.msaupd; */

/*<          ierr   = 0 >*/
        _->ierr = 0;
/*<          ishift = iparam(1) >*/
        _->ishift = iparam[1];
/*<          mxiter = iparam(3) >*/
        _->mxiter = iparam[3];
/*<          nb     = iparam(4) >*/
        _->nb = iparam[4];

/*        %--------------------------------------------% */
/*        | Revision 2 performs only implicit restart. | */
/*        %--------------------------------------------% */

/*<          iupd   = 1 >*/
        _->iupd = 1;
/*<          mode   = iparam(7) >*/
        _->mode = iparam[7];

/*        %----------------% */
/*        | Error checking | */
/*        %----------------% */

/*<          if (n .le. 0) then >*/
        if (*n <= 0) {
/*<             ierr = -1 >*/
            _->ierr = -1;
/*<          else if (nev .le. 0) then >*/
        } else if (*nev <= 0) {
/*<             ierr = -2 >*/
            _->ierr = -2;
/*<          else if (ncv .le. nev .or.  ncv .gt. n) then >*/
        } else if (*ncv <= *nev || *ncv > *n) {
/*<             ierr = -3 >*/
            _->ierr = -3;
/*<          end if >*/
        }

/*        %----------------------------------------------% */
/*        | NP is the number of additional steps to      | */
/*        | extend the length NEV Lanczos factorization. | */
/*        %----------------------------------------------% */

/*<          np     = ncv - nev >*/
        _->np = *ncv - *nev;

/*<          if (mxiter .le. 0)                     ierr = -4 >*/
        if (_->mxiter <= 0) {
            _->ierr = -4;
        }
/*<        >*/
        if (s_cmp(which, "LM", (ftnlen)2, (ftnlen)2) != 0 && s_cmp(which,
                "SM", (ftnlen)2, (ftnlen)2) != 0 && s_cmp(which, "LA", (
                ftnlen)2, (ftnlen)2) != 0 && s_cmp(which, "SA", (ftnlen)2, (
                ftnlen)2) != 0 && s_cmp(which, "BE", (ftnlen)2, (ftnlen)2) !=
                0) {
            _->ierr = -5;
        }
/*<          if (bmat .ne. 'I' .and. bmat .ne. 'G') ierr = -6 >*/
        if (*(unsigned char *)bmat != 'I' && *(unsigned char *)bmat != 'G') {
            _->ierr = -6;
        }

/*<          if (lworkl .lt. ncv**2 + 8*ncv)        ierr = -7 >*/
/* Computing 2nd power */
        i__1 = *ncv;
        if (*lworkl < i__1 * i__1 + (*ncv << 3)) {
            _->ierr = -7;
        }
/*<          if (mode .lt. 1 .or. mode .gt. 5) then >*/
        if (_->mode < 1 || _->mode > 5) {
/*<                                                 ierr = -10 >*/
            _->ierr = -10;
/*<          else if (mode .eq. 1 .and. bmat .eq. 'G') then >*/
        } else if (_->mode == 1 && *(unsigned char *)bmat == 'G') {
/*<                                                 ierr = -11 >*/
            _->ierr = -11;
/*<          else if (ishift .lt. 0 .or. ishift .gt. 1) then >*/
        } else if (_->ishift < 0 || _->ishift > 1) {
/*<                                                 ierr = -12 >*/
            _->ierr = -12;
/*<          else if (nev .eq. 1 .and. which .eq. 'BE') then >*/
        } else if (*nev == 1 && s_cmp(which, "BE", (ftnlen)2, (ftnlen)2) == 0)
                 {
/*<                                                 ierr = -13 >*/
            _->ierr = -13;
/*<          end if >*/
        }

/*        %------------% */
/*        | Error Exit | */
/*        %------------% */

/*<          if (ierr .ne. 0) then >*/
        if (_->ierr != 0) {
/*<             info = ierr >*/
            *info = _->ierr;
/*<             ido  = 99 >*/
            *ido = 99;
/*<             go to 9000 >*/
            goto L9000;
/*<          end if >*/
        }

/*        %------------------------% */
/*        | Set default parameters | */
/*        %------------------------% */

/*<          if (nb .le. 0)                         nb = 1 >*/
        if (_->nb <= 0) {
            _->nb = 1;
        }
/*<          if (tol .le. zero)                     tol = dlamch('EpsMach') >*/
        if (*tol <= 0.) {
            *tol = dlamch_("EpsMach", (ftnlen)7);
        }

/*        %----------------------------------------------% */
/*        | NP is the number of additional steps to      | */
/*        | extend the length NEV Lanczos factorization. | */
/*        | NEV0 is the local variable designating the   | */
/*        | size of the invariant subspace desired.      | */
/*        %----------------------------------------------% */

/*<          np     = ncv - nev >*/
        _->np = *ncv - *nev;
/*<          nev0   = nev  >*/
        _->nev0 = *nev;

/*        %-----------------------------% */
/*        | Zero out internal workspace | */
/*        %-----------------------------% */

/*<          do 10 j = 1, ncv**2 + 8*ncv >*/
/* Computing 2nd power */
        i__2 = *ncv;
        i__1 = i__2 * i__2 + (*ncv << 3);
        for (j = 1; j <= i__1; ++j) {
/*<             workl(j) = zero >*/
            workl[j] = 0.;
/*<  10      continue >*/
/* L10: */
        }

/*        %-------------------------------------------------------% */
/*        | Pointer into WORKL for address of H, RITZ, BOUNDS, Q  | */
/*        | etc... and the remaining workspace.                   | */
/*        | Also update pointer to be used on output.             | */
/*        | Memory is laid out as follows:                        | */
/*        | workl(1:2*ncv) := generated tridiagonal matrix        | */
/*        | workl(2*ncv+1:2*ncv+ncv) := ritz values               | */
/*        | workl(3*ncv+1:3*ncv+ncv) := computed error bounds     | */
/*        | workl(4*ncv+1:4*ncv+ncv*ncv) := rotation matrix Q     | */
/*        | workl(4*ncv+ncv*ncv+1:7*ncv+ncv*ncv) := workspace     | */
/*        %-------------------------------------------------------% */

/*<          ldh    = ncv >*/
        _->ldh = *ncv;
/*<          ldq    = ncv >*/
        _->ldq = *ncv;
/*<          ih     = 1 >*/
        _->ih = 1;
/*<          ritz   = ih     + 2*ldh >*/
        _->ritz = _->ih + (_->ldh << 1);
/*<          bounds = ritz   + ncv >*/
        _->bounds = _->ritz + *ncv;
/*<          iq     = bounds + ncv >*/
        _->iq = _->bounds + *ncv;
/*<          iw     = iq     + ncv**2 >*/
/* Computing 2nd power */
        i__1 = *ncv;
        _->iw = _->iq + i__1 * i__1;
/*<          next   = iw     + 3*ncv >*/
        _->next = _->iw + *ncv * 3;

/*<          ipntr(4) = next >*/
        ipntr[4] = _->next;
/*<          ipntr(5) = ih >*/
        ipntr[5] = _->ih;
/*<          ipntr(6) = ritz >*/
        ipntr[6] = _->ritz;
/*<          ipntr(7) = bounds >*/
        ipntr[7] = _->bounds;
/*<          ipntr(11) = iw >*/
        ipntr[11] = _->iw;
/*<       end if >*/
    }

/*     %-------------------------------------------------------% */
/*     | Carry out the Implicitly restarted Lanczos Iteration. | */
/*     %-------------------------------------------------------% */

/*<        >*/
    dsaup2_(workspace, ido, bmat, n, which, &_->nev0, &_->np, tol, &resid[1], &_->mode, &_->iupd, &
            _->ishift, &_->mxiter, &v[v_offset], ldv, &workl[_->ih], &_->ldh, &workl[_->ritz]
            , &workl[_->bounds], &workl[_->iq], &_->ldq, &workl[_->iw], &ipntr[1], &workd[
            1], info, (ftnlen)1, (ftnlen)2);

/*     %--------------------------------------------------% */
/*     | ido .ne. 99 implies use of reverse communication | */
/*     | to compute operations involving OP or shifts.    | */
/*     %--------------------------------------------------% */

/*<       if (ido .eq. 3) iparam(8) = np >*/
    if (*ido == 3) {
        iparam[8] = _->np;
    }
/*<       if (ido .ne. 99) go to 9000 >*/
    if (*ido != 99) {
        goto L9000;
    }

/*<       iparam(3) = mxiter >*/
    iparam[3] = _->mxiter;
/*<       iparam(5) = np >*/
    iparam[5] = _->np;
/*<       iparam(9) = nopx >*/
/*  iparam[9] = timing_1.nopx; */
/*<       iparam(10) = nbx >*/
/*  iparam[10] = timing_1.nbx; */
/*<       iparam(11) = nrorth >*/
/*  iparam[11] = timing_1.nrorth; */

/*     %------------------------------------% */
/*     | Exit if there was an informational | */
/*     | error within dsaup2.               | */
/*     %------------------------------------% */

/*<       if (info .lt. 0) go to 9000 >*/
    if (*info < 0) {
        goto L9000;
    }
/*<       if (info .eq. 2) info = 3 >*/
    if (*info == 2) {
        *info = 3;
    }

/*      if (msglvl .gt. 0) then */
/*         call ivout (logfil, 1, mxiter, ndigit, */
/*     &               '_saupd: number of update iterations taken') */
/*         call ivout (logfil, 1, np, ndigit, */
/*     &               '_saupd: number of "converged" Ritz values') */
/*         call dvout (logfil, np, workl(Ritz), ndigit, */
/*     &               '_saupd: final Ritz values') */
/*         call dvout (logfil, np, workl(Bounds), ndigit, */
/*     &               '_saupd: corresponding error bounds') */
/*      end if */

/*<       call second (t1) >*/
/*  second_(&t1); */
/*<       tsaupd = t1 - t0 >*/
/*  timing_1.tsaupd = t1 - t0; */

/*      if (msglvl .gt. 0) then */

/*        %--------------------------------------------------------% */
/*        | Version Number & Version Date are defined in version.h | */
/*        %--------------------------------------------------------% */

/*         write (6,1000) */
/*         write (6,1100) mxiter, nopx, nbx, nrorth, nitref, nrstrt, */
/*     &                  tmvopx, tmvbx, tsaupd, tsaup2, tsaitr, titref, */
/*     &                  tgetv0, tseigt, tsgets, tsapps, tsconv */
/* 1000    format (//, */
/*     &      5x, '==========================================',/ */
/*     &      5x, '= Symmetric implicit Arnoldi update code =',/ */
/*     &      5x, '= Version Number:', ' 2.4', 19x, ' =',/ */
/*     &      5x, '= Version Date:  ', ' 07/31/96', 14x, ' =',/ */
/*     &      5x, '==========================================',/ */
/*     &      5x, '= Summary of timing statistics           =',/ */
/*     &      5x, '==========================================',//) */
/* 1100    format ( */
/*     &      5x, 'Total number update iterations             = ', i5,/ */
/*     &      5x, 'Total number of OP*x operations            = ', i5,/ */
/*     &      5x, 'Total number of B*x operations             = ', i5,/ */
/*     &      5x, 'Total number of reorthogonalization steps  = ', i5,/ */
/*     &      5x, 'Total number of iterative refinement steps = ', i5,/ */
/*     &      5x, 'Total number of restart steps              = ', i5,/ */
/*     &      5x, 'Total time in user OP*x operation          = ', f12.6,/ */
/*     &      5x, 'Total time in user B*x operation           = ', f12.6,/ */
/*     &      5x, 'Total time in Arnoldi update routine       = ', f12.6,/ */
/*     &      5x, 'Total time in saup2 routine                = ', f12.6,/ */
/*     &      5x, 'Total time in basic Arnoldi iteration loop = ', f12.6,/ */
/*     &      5x, 'Total time in reorthogonalization phase    = ', f12.6,/ */
/*     &      5x, 'Total time in (re)start vector generation  = ', f12.6,/ */
/*     &      5x, 'Total time in trid eigenvalue subproblem   = ', f12.6,/ */
/*     &      5x, 'Total time in getting the shifts           = ', f12.6,/ */
/*     &      5x, 'Total time in applying the shifts          = ', f12.6,/ */
/*     &      5x, 'Total time in convergence testing          = ', f12.6) */
/*      end if */

/*<  9000 continue >*/
L9000:

/*<       return >*/
    return 0;

/*     %---------------% */
/*     | End of dsaupd | */
/*     %---------------% */

/*<       end >*/
} /* dsaupd_ */

#ifdef __cplusplus
        }
#endif
