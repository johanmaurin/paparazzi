/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Gen_UKF.c
 *
 * Code generated for Simulink model 'Gen_UKF'.
 *
 * Model version                  : 1.32
 * Simulink Coder version         : 8.10 (R2016a) 10-Feb-2016
 * C/C++ source code generated on : Thu Jun 30 23:12:10 2016
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "Gen_UKF.h"

/* Block signals and states (auto storage) */
DW rtDW;

/* External inputs (root inport signals with auto storage) */
ExtU rtU;

/* External outputs (root outports fed by signals with auto storage) */
ExtY rtY;

/* Forward declaration for local functions */
static real32_T xnrm2_f(int32_T n, const real32_T x[108], int32_T ix0);
static void xgeqrf_f(real32_T A[108], real32_T tau[6]);
static void qr_e(const real32_T A[108], real32_T Q[108], real32_T R[36]);
static real32_T xnrm2(int32_T n, const real32_T x[85], int32_T ix0);
static void xgeqrf(real32_T A[85], real32_T tau[5]);
static void qr(const real32_T A[85], real32_T Q[85], real32_T R[25]);
static void mrdivide(real32_T A[30], const real32_T B_0[25]);
static void h(const real32_T x[6], const real32_T q[4], real32_T y[5]);
static void f(const real32_T x[6], const real32_T u[6], real32_T phi, real32_T
              theta, real32_T dx[6]);

/* Function for MATLAB Function: '<Root>/UKF_prediction' */
static real32_T xnrm2_f(int32_T n, const real32_T x[108], int32_T ix0)
{
  real32_T y;
  real32_T scale;
  int32_T kend;
  real32_T absxk;
  real32_T t;
  int32_T k;
  y = 0.0F;
  if (!(n < 1)) {
    if (n == 1) {
      y = fabsf(x[ix0 - 1]);
    } else {
      scale = 1.17549435E-38F;
      kend = (ix0 + n) - 1;
      for (k = ix0; k <= kend; k++) {
        absxk = fabsf(x[k - 1]);
        if (absxk > scale) {
          t = scale / absxk;
          y = y * t * t + 1.0F;
          scale = absxk;
        } else {
          t = absxk / scale;
          y += t * t;
        }
      }

      y = scale * sqrtf(y);
    }
  }

  return y;
}

/* Function for MATLAB Function: '<Root>/UKF_prediction' */
static void xgeqrf_f(real32_T A[108], real32_T tau[6])
{
  real32_T work[6];
  int32_T i_i;
  real32_T b_atmp;
  real32_T xnorm;
  int32_T knt;
  int32_T lastv;
  int32_T lastc;
  int32_T ix;
  real32_T b_c;
  int32_T iy;
  int32_T f_1;
  int32_T g;
  int32_T b_ia;
  int32_T b_ix;
  boolean_T exitg2;
  int32_T i;
  for (i = 0; i < 6; i++) {
    work[i] = 0.0F;
  }

  for (i = 0; i < 6; i++) {
    i_i = i * 18 + i;
    b_atmp = A[i_i];
    b_c = 0.0F;
    xnorm = xnrm2_f(17 - i, A, i_i + 2);
    if (xnorm != 0.0F) {
      xnorm = hypotf(A[i_i], xnorm);
      if (A[i_i] >= 0.0F) {
        xnorm = -xnorm;
      }

      if (fabsf(xnorm) < 9.86076132E-32F) {
        knt = 0;
        do {
          knt++;
          lastv = (i_i - i) + 18;
          for (lastc = i_i + 1; lastc + 1 <= lastv; lastc++) {
            A[lastc] *= 1.01412048E+31F;
          }

          xnorm *= 1.01412048E+31F;
          b_atmp *= 1.01412048E+31F;
        } while (!(fabsf(xnorm) >= 9.86076132E-32F));

        xnorm = hypotf(b_atmp, xnrm2_f(17 - i, A, i_i + 2));
        if (b_atmp >= 0.0F) {
          xnorm = -xnorm;
        }

        b_c = (xnorm - b_atmp) / xnorm;
        b_atmp = 1.0F / (b_atmp - xnorm);
        lastv = (i_i - i) + 18;
        for (lastc = i_i + 1; lastc + 1 <= lastv; lastc++) {
          A[lastc] *= b_atmp;
        }

        for (lastv = 1; lastv <= knt; lastv++) {
          xnorm *= 9.86076132E-32F;
        }

        b_atmp = xnorm;
      } else {
        b_c = (xnorm - A[i_i]) / xnorm;
        b_atmp = 1.0F / (A[i_i] - xnorm);
        knt = (i_i - i) + 18;
        for (lastv = i_i + 1; lastv + 1 <= knt; lastv++) {
          A[lastv] *= b_atmp;
        }

        b_atmp = xnorm;
      }
    }

    tau[i] = b_c;
    A[i_i] = b_atmp;
    if (i + 1 < 6) {
      b_atmp = A[i_i];
      A[i_i] = 1.0F;
      knt = (i + 1) * 18 + i;
      if (tau[i] != 0.0F) {
        lastv = 18 - i;
        lastc = (i_i - i) + 17;
        while ((lastv > 0) && (A[lastc] == 0.0F)) {
          lastv--;
          lastc--;
        }

        lastc = 5 - i;
        exitg2 = false;
        while ((!exitg2) && (lastc > 0)) {
          iy = ((lastc - 1) * 18 + knt) + 1;
          f_1 = iy;
          do {
            b_ix = 0;
            if (f_1 <= (iy + lastv) - 1) {
              if (A[f_1 - 1] != 0.0F) {
                b_ix = 1;
              } else {
                f_1++;
              }
            } else {
              lastc--;
              b_ix = 2;
            }
          } while (b_ix == 0);

          if (b_ix == 1) {
            exitg2 = true;
          }
        }
      } else {
        lastv = 0;
        lastc = 0;
      }

      if (lastv > 0) {
        if (lastc != 0) {
          for (iy = 1; iy <= lastc; iy++) {
            work[iy - 1] = 0.0F;
          }

          iy = 0;
          f_1 = ((lastc - 1) * 18 + knt) + 1;
          for (b_ix = knt + 1; b_ix <= f_1; b_ix += 18) {
            ix = i_i;
            b_c = 0.0F;
            g = (b_ix + lastv) - 1;
            for (b_ia = b_ix; b_ia <= g; b_ia++) {
              b_c += A[b_ia - 1] * A[ix];
              ix++;
            }

            work[iy] += b_c;
            iy++;
          }
        }

        if (!(-tau[i] == 0.0F)) {
          iy = 0;
          for (f_1 = 1; f_1 <= lastc; f_1++) {
            if (work[iy] != 0.0F) {
              b_c = work[iy] * -tau[i];
              b_ix = i_i;
              ix = lastv + knt;
              for (g = knt; g + 1 <= ix; g++) {
                A[g] += A[b_ix] * b_c;
                b_ix++;
              }
            }

            iy++;
            knt += 18;
          }
        }
      }

      A[i_i] = b_atmp;
    }
  }
}

/* Function for MATLAB Function: '<Root>/UKF_prediction' */
static void qr_e(const real32_T A[108], real32_T Q[108], real32_T R[36])
{
  real32_T tau[6];
  int32_T c_i;
  int32_T itau;
  real32_T work[6];
  int32_T iaii;
  int32_T lastv;
  int32_T lastc;
  int32_T ix;
  real32_T c;
  int32_T iy;
  int32_T iac;
  int32_T d;
  int32_T b_ia;
  int32_T jy;
  boolean_T exitg2;
  memcpy(&Q[0], &A[0], 108U * sizeof(real32_T));
  xgeqrf_f(Q, tau);
  for (itau = 0; itau < 6; itau++) {
    for (c_i = 0; c_i + 1 <= itau + 1; c_i++) {
      R[c_i + 6 * itau] = Q[18 * itau + c_i];
    }

    for (c_i = itau + 1; c_i + 1 < 7; c_i++) {
      R[c_i + 6 * itau] = 0.0F;
    }

    work[itau] = 0.0F;
  }

  itau = 5;
  for (c_i = 5; c_i >= 0; c_i += -1) {
    iaii = (c_i * 18 + c_i) + 1;
    if (c_i + 1 < 6) {
      Q[iaii - 1] = 1.0F;
      if (tau[itau] != 0.0F) {
        lastv = 18 - c_i;
        lastc = (iaii - c_i) + 16;
        while ((lastv > 0) && (Q[lastc] == 0.0F)) {
          lastv--;
          lastc--;
        }

        lastc = 5 - c_i;
        exitg2 = false;
        while ((!exitg2) && (lastc > 0)) {
          iy = (lastc - 1) * 18 + iaii;
          jy = iy;
          do {
            iac = 0;
            if (jy + 18 <= (iy + lastv) + 17) {
              if (Q[jy + 17] != 0.0F) {
                iac = 1;
              } else {
                jy++;
              }
            } else {
              lastc--;
              iac = 2;
            }
          } while (iac == 0);

          if (iac == 1) {
            exitg2 = true;
          }
        }
      } else {
        lastv = 0;
        lastc = 0;
      }

      if (lastv > 0) {
        if (lastc != 0) {
          for (iy = 1; iy <= lastc; iy++) {
            work[iy - 1] = 0.0F;
          }

          iy = 0;
          jy = ((lastc - 1) * 18 + iaii) + 18;
          for (iac = iaii + 18; iac <= jy; iac += 18) {
            ix = iaii;
            c = 0.0F;
            d = (iac + lastv) - 1;
            for (b_ia = iac; b_ia <= d; b_ia++) {
              c += Q[b_ia - 1] * Q[ix - 1];
              ix++;
            }

            work[iy] += c;
            iy++;
          }
        }

        if (!(-tau[itau] == 0.0F)) {
          iy = iaii + 17;
          jy = 0;
          for (iac = 1; iac <= lastc; iac++) {
            if (work[jy] != 0.0F) {
              c = work[jy] * -tau[itau];
              ix = iaii;
              d = lastv + iy;
              for (b_ia = iy; b_ia + 1 <= d; b_ia++) {
                Q[b_ia] += Q[ix - 1] * c;
                ix++;
              }
            }

            jy++;
            iy += 18;
          }
        }
      }
    }

    lastv = (iaii - c_i) + 17;
    for (lastc = iaii; lastc + 1 <= lastv; lastc++) {
      Q[lastc] *= -tau[itau];
    }

    Q[iaii - 1] = 1.0F - tau[itau];
    for (lastv = 1; lastv <= c_i; lastv++) {
      Q[(iaii - lastv) - 1] = 0.0F;
    }

    itau--;
  }
}

/* Function for MATLAB Function: '<Root>/UKF_correction' */
static real32_T xnrm2(int32_T n, const real32_T x[85], int32_T ix0)
{
  real32_T y;
  real32_T scale;
  int32_T kend;
  real32_T absxk;
  real32_T t;
  int32_T k;
  y = 0.0F;
  if (!(n < 1)) {
    if (n == 1) {
      y = fabsf(x[ix0 - 1]);
    } else {
      scale = 1.17549435E-38F;
      kend = (ix0 + n) - 1;
      for (k = ix0; k <= kend; k++) {
        absxk = fabsf(x[k - 1]);
        if (absxk > scale) {
          t = scale / absxk;
          y = y * t * t + 1.0F;
          scale = absxk;
        } else {
          t = absxk / scale;
          y += t * t;
        }
      }

      y = scale * sqrtf(y);
    }
  }

  return y;
}

/* Function for MATLAB Function: '<Root>/UKF_correction' */
static void xgeqrf(real32_T A[85], real32_T tau[5])
{
  real32_T work[5];
  int32_T i_i;
  real32_T b_atmp;
  real32_T xnorm;
  int32_T knt;
  int32_T lastv;
  int32_T lastc;
  int32_T ix;
  real32_T b_c;
  int32_T iy;
  int32_T f_0;
  int32_T g;
  int32_T b_ia;
  int32_T b_ix;
  boolean_T exitg2;
  int32_T i;
  for (i = 0; i < 5; i++) {
    work[i] = 0.0F;
  }

  for (i = 0; i < 5; i++) {
    i_i = i * 17 + i;
    b_atmp = A[i_i];
    b_c = 0.0F;
    xnorm = xnrm2(16 - i, A, i_i + 2);
    if (xnorm != 0.0F) {
      xnorm = hypotf(A[i_i], xnorm);
      if (A[i_i] >= 0.0F) {
        xnorm = -xnorm;
      }

      if (fabsf(xnorm) < 9.86076132E-32F) {
        knt = 0;
        do {
          knt++;
          lastv = (i_i - i) + 17;
          for (lastc = i_i + 1; lastc + 1 <= lastv; lastc++) {
            A[lastc] *= 1.01412048E+31F;
          }

          xnorm *= 1.01412048E+31F;
          b_atmp *= 1.01412048E+31F;
        } while (!(fabsf(xnorm) >= 9.86076132E-32F));

        xnorm = hypotf(b_atmp, xnrm2(16 - i, A, i_i + 2));
        if (b_atmp >= 0.0F) {
          xnorm = -xnorm;
        }

        b_c = (xnorm - b_atmp) / xnorm;
        b_atmp = 1.0F / (b_atmp - xnorm);
        lastv = (i_i - i) + 17;
        for (lastc = i_i + 1; lastc + 1 <= lastv; lastc++) {
          A[lastc] *= b_atmp;
        }

        for (lastv = 1; lastv <= knt; lastv++) {
          xnorm *= 9.86076132E-32F;
        }

        b_atmp = xnorm;
      } else {
        b_c = (xnorm - A[i_i]) / xnorm;
        b_atmp = 1.0F / (A[i_i] - xnorm);
        knt = (i_i - i) + 17;
        for (lastv = i_i + 1; lastv + 1 <= knt; lastv++) {
          A[lastv] *= b_atmp;
        }

        b_atmp = xnorm;
      }
    }

    tau[i] = b_c;
    A[i_i] = b_atmp;
    if (i + 1 < 5) {
      b_atmp = A[i_i];
      A[i_i] = 1.0F;
      knt = (i + 1) * 17 + i;
      if (tau[i] != 0.0F) {
        lastv = 17 - i;
        lastc = (i_i - i) + 16;
        while ((lastv > 0) && (A[lastc] == 0.0F)) {
          lastv--;
          lastc--;
        }

        lastc = 4 - i;
        exitg2 = false;
        while ((!exitg2) && (lastc > 0)) {
          iy = ((lastc - 1) * 17 + knt) + 1;
          f_0 = iy;
          do {
            b_ix = 0;
            if (f_0 <= (iy + lastv) - 1) {
              if (A[f_0 - 1] != 0.0F) {
                b_ix = 1;
              } else {
                f_0++;
              }
            } else {
              lastc--;
              b_ix = 2;
            }
          } while (b_ix == 0);

          if (b_ix == 1) {
            exitg2 = true;
          }
        }
      } else {
        lastv = 0;
        lastc = 0;
      }

      if (lastv > 0) {
        if (lastc != 0) {
          for (iy = 1; iy <= lastc; iy++) {
            work[iy - 1] = 0.0F;
          }

          iy = 0;
          f_0 = ((lastc - 1) * 17 + knt) + 1;
          for (b_ix = knt + 1; b_ix <= f_0; b_ix += 17) {
            ix = i_i;
            b_c = 0.0F;
            g = (b_ix + lastv) - 1;
            for (b_ia = b_ix; b_ia <= g; b_ia++) {
              b_c += A[b_ia - 1] * A[ix];
              ix++;
            }

            work[iy] += b_c;
            iy++;
          }
        }

        if (!(-tau[i] == 0.0F)) {
          iy = 0;
          for (f_0 = 1; f_0 <= lastc; f_0++) {
            if (work[iy] != 0.0F) {
              b_c = work[iy] * -tau[i];
              b_ix = i_i;
              ix = lastv + knt;
              for (g = knt; g + 1 <= ix; g++) {
                A[g] += A[b_ix] * b_c;
                b_ix++;
              }
            }

            iy++;
            knt += 17;
          }
        }
      }

      A[i_i] = b_atmp;
    }
  }
}

/* Function for MATLAB Function: '<Root>/UKF_correction' */
static void qr(const real32_T A[85], real32_T Q[85], real32_T R[25])
{
  real32_T tau[5];
  int32_T c_i;
  int32_T itau;
  real32_T work[5];
  int32_T iaii;
  int32_T lastv;
  int32_T lastc;
  int32_T ix;
  real32_T c;
  int32_T iy;
  int32_T iac;
  int32_T d;
  int32_T b_ia;
  int32_T jy;
  boolean_T exitg2;
  memcpy(&Q[0], &A[0], 85U * sizeof(real32_T));
  xgeqrf(Q, tau);
  for (itau = 0; itau < 5; itau++) {
    for (c_i = 0; c_i + 1 <= itau + 1; c_i++) {
      R[c_i + 5 * itau] = Q[17 * itau + c_i];
    }

    for (c_i = itau + 1; c_i + 1 < 6; c_i++) {
      R[c_i + 5 * itau] = 0.0F;
    }

    work[itau] = 0.0F;
  }

  itau = 4;
  for (c_i = 4; c_i >= 0; c_i += -1) {
    iaii = (c_i * 17 + c_i) + 1;
    if (c_i + 1 < 5) {
      Q[iaii - 1] = 1.0F;
      if (tau[itau] != 0.0F) {
        lastv = 17 - c_i;
        lastc = (iaii - c_i) + 15;
        while ((lastv > 0) && (Q[lastc] == 0.0F)) {
          lastv--;
          lastc--;
        }

        lastc = 4 - c_i;
        exitg2 = false;
        while ((!exitg2) && (lastc > 0)) {
          iy = (lastc - 1) * 17 + iaii;
          jy = iy;
          do {
            iac = 0;
            if (jy + 17 <= (iy + lastv) + 16) {
              if (Q[jy + 16] != 0.0F) {
                iac = 1;
              } else {
                jy++;
              }
            } else {
              lastc--;
              iac = 2;
            }
          } while (iac == 0);

          if (iac == 1) {
            exitg2 = true;
          }
        }
      } else {
        lastv = 0;
        lastc = 0;
      }

      if (lastv > 0) {
        if (lastc != 0) {
          for (iy = 1; iy <= lastc; iy++) {
            work[iy - 1] = 0.0F;
          }

          iy = 0;
          jy = ((lastc - 1) * 17 + iaii) + 17;
          for (iac = iaii + 17; iac <= jy; iac += 17) {
            ix = iaii;
            c = 0.0F;
            d = (iac + lastv) - 1;
            for (b_ia = iac; b_ia <= d; b_ia++) {
              c += Q[b_ia - 1] * Q[ix - 1];
              ix++;
            }

            work[iy] += c;
            iy++;
          }
        }

        if (!(-tau[itau] == 0.0F)) {
          iy = iaii + 16;
          jy = 0;
          for (iac = 1; iac <= lastc; iac++) {
            if (work[jy] != 0.0F) {
              c = work[jy] * -tau[itau];
              ix = iaii;
              d = lastv + iy;
              for (b_ia = iy; b_ia + 1 <= d; b_ia++) {
                Q[b_ia] += Q[ix - 1] * c;
                ix++;
              }
            }

            jy++;
            iy += 17;
          }
        }
      }
    }

    lastv = (iaii - c_i) + 16;
    for (lastc = iaii; lastc + 1 <= lastv; lastc++) {
      Q[lastc] *= -tau[itau];
    }

    Q[iaii - 1] = 1.0F - tau[itau];
    for (lastv = 1; lastv <= c_i; lastv++) {
      Q[(iaii - lastv) - 1] = 0.0F;
    }

    itau--;
  }
}

/* Function for MATLAB Function: '<Root>/main' */
static void mrdivide(real32_T A[30], const real32_T B_0[25])
{
  int32_T jp;
  real32_T temp;
  real32_T b_A[25];
  int8_T ipiv[5];
  int32_T j;
  int32_T ix;
  real32_T s;
  int32_T c_ix;
  int32_T d;
  int32_T ijA;
  int32_T b_jAcol;
  int32_T b_kBcol;
  memcpy(&b_A[0], &B_0[0], 25U * sizeof(real32_T));
  for (j = 0; j < 5; j++) {
    ipiv[j] = (int8_T)(1 + j);
  }

  for (j = 0; j < 4; j++) {
    jp = j * 6;
    b_jAcol = 0;
    ix = jp;
    temp = fabsf(b_A[jp]);
    for (b_kBcol = 2; b_kBcol <= 5 - j; b_kBcol++) {
      ix++;
      s = fabsf(b_A[ix]);
      if (s > temp) {
        b_jAcol = b_kBcol - 1;
        temp = s;
      }
    }

    if (b_A[jp + b_jAcol] != 0.0F) {
      if (b_jAcol != 0) {
        ipiv[j] = (int8_T)((j + b_jAcol) + 1);
        ix = j;
        b_jAcol += j;
        for (b_kBcol = 0; b_kBcol < 5; b_kBcol++) {
          temp = b_A[ix];
          b_A[ix] = b_A[b_jAcol];
          b_A[b_jAcol] = temp;
          ix += 5;
          b_jAcol += 5;
        }
      }

      b_jAcol = (jp - j) + 5;
      for (ix = jp + 1; ix + 1 <= b_jAcol; ix++) {
        b_A[ix] /= b_A[jp];
      }
    }

    b_jAcol = jp;
    ix = jp + 5;
    for (b_kBcol = 1; b_kBcol <= 4 - j; b_kBcol++) {
      temp = b_A[ix];
      if (b_A[ix] != 0.0F) {
        c_ix = jp + 1;
        d = (b_jAcol - j) + 10;
        for (ijA = 6 + b_jAcol; ijA + 1 <= d; ijA++) {
          b_A[ijA] += b_A[c_ix] * -temp;
          c_ix++;
        }
      }

      ix += 5;
      b_jAcol += 5;
    }
  }

  for (j = 0; j < 5; j++) {
    jp = 6 * j;
    b_jAcol = 5 * j;
    for (ix = 1; ix <= j; ix++) {
      b_kBcol = (ix - 1) * 6;
      if (b_A[(ix + b_jAcol) - 1] != 0.0F) {
        for (c_ix = 0; c_ix < 6; c_ix++) {
          A[c_ix + jp] -= b_A[(ix + b_jAcol) - 1] * A[c_ix + b_kBcol];
        }
      }
    }

    temp = 1.0F / b_A[j + b_jAcol];
    for (b_jAcol = 0; b_jAcol < 6; b_jAcol++) {
      A[b_jAcol + jp] *= temp;
    }
  }

  for (j = 4; j >= 0; j += -1) {
    jp = 6 * j;
    b_jAcol = 5 * j - 1;
    for (ix = j + 2; ix < 6; ix++) {
      b_kBcol = (ix - 1) * 6;
      if (b_A[ix + b_jAcol] != 0.0F) {
        for (c_ix = 0; c_ix < 6; c_ix++) {
          A[c_ix + jp] -= b_A[ix + b_jAcol] * A[c_ix + b_kBcol];
        }
      }
    }
  }

  for (j = 3; j >= 0; j += -1) {
    if (j + 1 != ipiv[j]) {
      jp = ipiv[j] - 1;
      for (b_jAcol = 0; b_jAcol < 6; b_jAcol++) {
        temp = A[6 * j + b_jAcol];
        A[b_jAcol + 6 * j] = A[6 * jp + b_jAcol];
        A[b_jAcol + 6 * jp] = temp;
      }
    }
  }
}

/* Function for MATLAB Function: '<Root>/UKF_correction' */
static void h(const real32_T x[6], const real32_T q[4], real32_T y[5])
{
  real32_T qnrm;
  real32_T qinv_idx_0;
  real32_T qinv_idx_1;
  real32_T qinv_idx_2;
  real32_T qinv_idx_3;
  real32_T qinter_idx_2;
  real32_T qinter_idx_3;
  real32_T qinter_idx_1;

  /* -----------------Observation_model------------------- */
  /* description : */
  /*  */
  /* Dim of the measurement vector dim(zk) = 10; */
  /* y=[y1 y2 y3 y4 y5 y6 y7 y8 y9 y10] */
  /* y=[V1 V2 V3 P1 P2 P3 B1 B2 B3 YH ] */
  /*  */
  /* Inputs :  */
  /* 		-States:                                x      	[15 x 1] */
  /* 		-Measures :                             zk		[10  x 1] */
  /* Outputs : */
  /* 		-outputs model   :                      y 		[10 x 1] */
  /* ------------------------------------------------------ */
  /* ==> u,v,w  */
  /* description : */
  /* q*D*q-1  */
  qnrm = ((q[0] * q[0] + q[1] * q[1]) + q[2] * q[2]) + q[3] * q[3];
  qinv_idx_0 = q[0] / qnrm;
  qinv_idx_1 = -q[1] / qnrm;
  qinv_idx_2 = -q[2] / qnrm;
  qinv_idx_3 = -q[3] / qnrm;
  qnrm = (q[1] * x[0] + q[2] * x[1]) + q[3] * x[2];
  qinter_idx_1 = (q[2] * x[2] - q[3] * x[1]) + x[0] * q[0];
  qinter_idx_2 = (q[3] * x[0] - q[1] * x[2]) + x[1] * q[0];
  qinter_idx_3 = (q[1] * x[1] - q[2] * x[0]) + x[2] * q[0];

  /* ==>Va */
  /* ==> AOA */
  y[0] = (((qinter_idx_2 * qinv_idx_3 - qinter_idx_3 * qinv_idx_2) +
           qinter_idx_1 * qinv_idx_0) + qinv_idx_1 * -qnrm) + x[3];
  y[1] = (((qinter_idx_3 * qinv_idx_1 - qinter_idx_1 * qinv_idx_3) +
           qinter_idx_2 * qinv_idx_0) + qinv_idx_2 * -qnrm) + x[4];
  y[2] = (((qinter_idx_1 * qinv_idx_2 - qinter_idx_2 * qinv_idx_1) +
           qinter_idx_3 * qinv_idx_0) + qinv_idx_3 * -qnrm) + x[5];
  y[3] = x[0];
  y[4] = atanf(x[2] / x[0]);
}

/* Function for MATLAB Function: '<Root>/UKF_prediction' */
static void f(const real32_T x[6], const real32_T u[6], real32_T phi, real32_T
              theta, real32_T dx[6])
{
  /* -------------------Process_model--------------------- */
  /* description : */
  /*  */
  /* Dim of the state vector : dim(x) = 6; */
  /* x=[x1 x2 x3 x4 x5 x6] */
  /* x=[uk vk wk uw0 vw0 ww0] */
  /*  */
  /*    dx=f(x,u) */
  /*    dx1=f(x1,u) */
  /*    dx2=f(x2,u) */
  /*    ... */
  /*    dx=[dx1; dx2; ...] */
  /* Inputs :  */
  /* 		-States:                            x      	 */
  /* 		-Command inputs :                   omega,a    */
  /* Outputs : */
  /* 		-state dot   :                      dx 		 */
  /* ------------------------------------------------------ */
  /* dot_uvw */
  /* dot_uvw0 */
  /* ==> dot_UVW */
  /* ==> dot_biais */
  dx[0] = ((u[3] - 9.81F * sinf(theta)) + u[2] * x[1]) - u[1] * x[2];
  dx[3] = 0.0F;
  dx[1] = ((9.81F * cosf(theta) * sinf(phi) + u[4]) + u[0] * x[2]) - u[2] * x[0];
  dx[4] = 0.0F;
  dx[2] = ((9.81F * cosf(theta) * cosf(phi) + u[5]) + u[1] * x[0]) - u[0] * x[1];
  dx[5] = 0.0F;
}

/* Model step function */
void Gen_UKF_step(void)
{
  real32_T lambda;
  int32_T jmax;
  int32_T colj;
  int32_T b_j;
  boolean_T exitg1;
  real32_T Y[36];
  real32_T SQ[36];
  real32_T p[6];
  real32_T Temp[108];
  int32_T jj;
  real32_T ajj;
  int32_T ix;
  int32_T iy;
  int32_T b_ix;
  int32_T e;
  int32_T ia;
  real32_T k3[6];
  real32_T SR[25];
  real32_T p_0[5];
  real32_T Temp_0[85];
  int32_T d_ix;
  real32_T K[30];
  real32_T U[30];
  real32_T d[169];
  real32_T rtb_x1[6];
  real32_T rtb_z[5];
  real32_T rtb_Wc[13];
  real32_T rtb_Wm[13];
  real32_T rtb_P1[36];
  real32_T rtb_X[78];
  real32_T rtb_Y[78];
  real32_T rtb_Z[65];
  int32_T i;
  real32_T lambda_0[85];
  real32_T lambda_1[108];
  real32_T tmp[6];
  real32_T rtb_X_0[6];
  real32_T SQ_0[36];
  real32_T SR_0[25];
  real32_T lambda_2[25];
  real32_T rtb_Z_0[65];

  /* MATLAB Function: '<Root>/initialization' incorporates:
   *  Inport: '<Root>/P'
   *  Inport: '<Root>/alpha'
   *  Inport: '<Root>/beta'
   *  Inport: '<Root>/ki'
   *  Inport: '<Root>/x0'
   *  UnitDelay: '<Root>/ Delay'
   *  UnitDelay: '<Root>/ Delay1'
   */
  /* MATLAB Function 'initialization': '<S3>:1' */
  /* ----------------Initialisation--------------------- */
  /* P=0*eye(11); */
  /* x=(linspace(0,0,11))'; */
  /* persistent Q */
  /* persistent R */
  if (!rtDW.P_not_empty) {
    /* '<S3>:1:11' */
    /* '<S3>:1:12' */
    memcpy(&SQ[0], &rtU.P_i[0], 36U * sizeof(real32_T));
    jmax = 0;
    colj = 0;
    b_j = 1;
    exitg1 = false;
    while ((!exitg1) && (b_j < 7)) {
      jj = (colj + b_j) - 1;
      lambda = 0.0F;
      if (!(b_j - 1 < 1)) {
        ix = colj;
        iy = colj;
        for (i = 1; i < b_j; i++) {
          lambda += SQ[ix] * SQ[iy];
          ix++;
          iy++;
        }
      }

      ajj = SQ[jj] - lambda;
      if (ajj > 0.0F) {
        ajj = sqrtf(ajj);
        SQ[jj] = ajj;
        if (b_j < 6) {
          if (b_j - 1 != 0) {
            ix = jj + 6;
            iy = ((5 - b_j) * 6 + colj) + 7;
            for (d_ix = colj + 7; d_ix <= iy; d_ix += 6) {
              b_ix = colj;
              lambda = 0.0F;
              e = (d_ix + b_j) - 2;
              for (ia = d_ix; ia <= e; ia++) {
                lambda += SQ[ia - 1] * SQ[b_ix];
                b_ix++;
              }

              SQ[ix] += -lambda;
              ix += 6;
            }
          }

          lambda = 1.0F / ajj;
          ix = ((5 - b_j) * 6 + jj) + 7;
          for (i = jj + 6; i + 1 <= ix; i += 6) {
            SQ[i] *= lambda;
          }

          colj += 6;
        }

        b_j++;
      } else {
        SQ[jj] = ajj;
        jmax = b_j;
        exitg1 = true;
      }
    }

    if (jmax == 0) {
      jmax = 6;
    } else {
      jmax--;
    }

    for (b_j = 0; b_j + 1 <= jmax; b_j++) {
      for (colj = b_j + 1; colj + 1 <= jmax; colj++) {
        SQ[colj + 6 * b_j] = 0.0F;
      }
    }

    for (jmax = 0; jmax < 6; jmax++) {
      for (b_j = 0; b_j < 6; b_j++) {
        rtb_P1[b_j + 6 * jmax] = SQ[6 * b_j + jmax];
      }
    }

    rtDW.P_not_empty = true;
  } else {
    /* '<S3>:1:14' */
    memcpy(&rtb_P1[0], &rtDW.Delay_DSTATE[0], 36U * sizeof(real32_T));
  }

  if (!rtDW.x_not_empty) {
    /* '<S3>:1:17' */
    /* '<S3>:1:18' */
    for (i = 0; i < 6; i++) {
      rtb_x1[i] = rtU.x0[i];
    }

    rtDW.x_not_empty = true;
  } else {
    /* '<S3>:1:20' */
    for (i = 0; i < 6; i++) {
      rtb_x1[i] = rtDW.Delay1_DSTATE[i];
    }
  }

  /* %%%%%%%%%%%%%%%--PROGRAMME PRINCIPAL--%%%%%%%%%%%%%%%% */
  /* alpha=0.01;  %10^-1=<alpha=<1 : dispersion des sigma points. */
  /* Default alpha=0.5; */
  /* ki=0;       %ki>=0 : garantis que la covariance soit semi-definit positive. */
  /* Default k=0; */
  /* beta=2;     %informations sur les moments d'ordre 3 */
  /* Default beta=2 choix optimal quand la distrib est sup. gaussienne. */
  /* dt=0.02;    %dt pour le Runge-Kutta */
  /* --------Calcul du lambda----------------- */
  /* '<S3>:1:40' */
  /* dimension de l'etat */
  /* '<S3>:1:41' */
  /* dimension de la mesures */
  /* '<S3>:1:43' */
  lambda = rtU.alpha * rtU.alpha * (6.0F + rtU.ki) - 6.0F;

  /* '<S3>:1:44' */
  /* ----------------Calcul des ponderations Wm et Wc-------------------- */
  /* '<S3>:1:48' */
  ajj = 0.5F / (6.0F + lambda);
  rtb_Wm[0] = lambda / (6.0F + lambda);
  for (i = 0; i < 12; i++) {
    rtb_Wm[i + 1] = ajj;
  }

  /* '<S3>:1:49' */
  for (i = 0; i < 13; i++) {
    rtb_Wc[i] = rtb_Wm[i];
  }

  /* '<S3>:1:50' */
  rtb_Wc[0] = ((1.0F - rtU.alpha * rtU.alpha) + rtU.beta) + rtb_Wm[0];

  /* '<S3>:1:51' */
  /* '<S3>:1:52' */
  /* '<S3>:1:53' */
  lambda = sqrtf(6.0F + lambda);

  /* MATLAB Function: '<Root>/sigmas' */
  /* MATLAB Function 'sigmas': '<S5>:1' */
  /* ----------------Sigma-point--------------------------- */
  /* sigmas points et concatenation : n*(2n+1) */
  /* Inputs :  */
  /* 		-States:                                 x 		 */
  /* 		-Covariance on state :                   P 		 */
  /* 		-Weighting squate-root  */
  /* 		-sigmas-point      (Wm,Wc)               c		 */
  /* Outputs :   */
  /* 		-Sigmas-points (state) :                 X      	 */
  /* ---------------------------------------------------- */
  /* Covariance with ponderation: A = c * P  */
  /* '<S5>:1:15' */
  for (jmax = 0; jmax < 36; jmax++) {
    SQ[jmax] = lambda * rtb_P1[jmax];
  }

  /* dimension modification on state  */
  /* '<S5>:1:18' */
  /* concatenation : X=[x Y+A Y-A] */
  /* '<S5>:1:21' */
  for (jmax = 0; jmax < 6; jmax++) {
    for (b_j = 0; b_j < 6; b_j++) {
      Y[b_j + 6 * jmax] = rtb_x1[b_j];
    }

    rtb_X[jmax] = rtb_x1[jmax];
  }

  for (jmax = 0; jmax < 36; jmax++) {
    rtb_X[jmax + 6] = Y[jmax] + SQ[jmax];
    rtb_X[jmax + 42] = Y[jmax] - SQ[jmax];
  }

  /* End of MATLAB Function: '<Root>/sigmas' */

  /* MATLAB Function: '<Root>/UKF_prediction' incorporates:
   *  Inport: '<Root>/Q'
   *  Inport: '<Root>/[omega a]'
   *  Inport: '<Root>/dt'
   *  Inport: '<Root>/phi'
   *  Inport: '<Root>/theta'
   *  MATLAB Function: '<Root>/initialization'
   */
  /* '<S5>:1:23' */
  /* MATLAB Function 'UKF_prediction': '<S2>:1' */
  /* '<S2>:1:35' */
  /* '<S2>:1:28' */
  /* ----------------Prediction Step------------------------- */
  /* Sigma point and model fonction f(x,u) */
  /* Inputs :  */
  /* 		-Sigmas-points (state) :                X      	 */
  /* 		-Weighting sigmas-points  :             Wm, Wc 	 */
  /* 		-Command input	 :                      omega, a    */
  /* 		-Dimension vector state:                n		 */
  /* 		-Integration step :                     dt		 */
  /* 		-Dimension 2n+1 :                       L  		 */
  /* 		-Weighting state (Wk) :                 Q  		 */
  /* Outputs : */
  /* 		-Sigmas_state   :                       Y 		 */
  /* 		-Predicted state:                       y 		 */
  /* 		-Covariance on state :                  J 		 */
  /* ---------------------------------------------------- */
  /* ----------------Prediction------------------------- */
  /* description : */
  /* Output : SigmaX(k+1) et xmoy(k+1) */
  /*  */
  /* '<S2>:1:26' */
  memset(&rtb_Y[0], 0, 78U * sizeof(real32_T));

  /* '<S2>:1:21' */
  /* '<S2>:1:27' */
  for (i = 0; i < 13; i++) {
    /* '<S2>:1:27' */
    /* '<S2>:1:28' */
    /* ----------------Runge Kutta 4----------------- */
    /* description : */
    /* dt is an integration step (tk+1-tk) */
    /* h_2 is define as step/2 */
    /*  */
    /* Non linear function f(x,u), results : X(:,k)=[Xk+1(0) ... Xk+1(2n+1)] */
    /* =RungeKutta(X(:,k),dt,omega,a); */
    f(&rtb_X[6 * i], rtU.omegaa, rtU.phi, rtU.theta, rtb_x1);
    for (jmax = 0; jmax < 6; jmax++) {
      rtb_X_0[jmax] = rtb_X[6 * i + jmax] + rtb_x1[jmax] / 2.0F * rtU.dt;
    }

    f(rtb_X_0, rtU.omegaa, rtU.phi, rtU.theta, p);
    for (jmax = 0; jmax < 6; jmax++) {
      rtb_X_0[jmax] = rtb_X[6 * i + jmax] + p[jmax] / 2.0F * rtU.dt;
    }

    f(rtb_X_0, rtU.omegaa, rtU.phi, rtU.theta, k3);
    ajj = rtU.dt / 6.0F;
    for (jmax = 0; jmax < 6; jmax++) {
      rtb_X_0[jmax] = rtb_X[6 * i + jmax] + rtU.dt * k3[jmax];
    }

    f(rtb_X_0, rtU.omegaa, rtU.phi, rtU.theta, tmp);
    for (jmax = 0; jmax < 6; jmax++) {
      rtb_Y[jmax + 6 * i] = (((p[jmax] + k3[jmax]) * 2.0F + rtb_x1[jmax]) +
        tmp[jmax]) * ajj + rtb_X[6 * i + jmax];
    }

    /* non linear function */
    /* y=y+Wm(k)*Y(:,k);                    %predicted vector on the state */
    /* '<S2>:1:30' */
    for (jmax = 0; jmax < 6; jmax++) {
      rtb_x1[jmax] = rtb_Y[jmax];
    }

    /* '<S2>:1:27' */
  }

  /* '<S2>:1:33' */
  for (i = 0; i < 36; i++) {
    SQ[i] = sqrtf(rtU.Q[i]);
  }

  /* square-root of R */
  /* '<S2>:1:35' */
  lambda = sqrtf(rtb_Wm[1]);

  /* '<S2>:1:37' */
  /* 33*11=>[11*22 11*11] */
  /* '<S2>:1:39' */
  for (jmax = 0; jmax < 6; jmax++) {
    for (b_j = 0; b_j < 12; b_j++) {
      lambda_1[b_j + 18 * jmax] = (rtb_Y[(1 + b_j) * 6 + jmax] - rtb_x1[jmax]) *
        lambda;
    }
  }

  for (jmax = 0; jmax < 6; jmax++) {
    for (b_j = 0; b_j < 6; b_j++) {
      lambda_1[(b_j + 18 * jmax) + 12] = SQ[6 * b_j + jmax];
    }
  }

  qr_e(lambda_1, Temp, SQ);

  /* '<S2>:1:41' */
  for (jmax = 0; jmax < 6; jmax++) {
    p[jmax] = rtb_Y[jmax] - rtb_x1[jmax];
  }

  if (rtb_Wc[0] < 0.0F) {
    /* '<S2>:1:43' */
    /* '<S2>:1:44' */
    lambda = powf(-rtb_Wc[0], 0.25F);
    ajj = powf(-rtb_Wc[0], 0.25F);
    for (jmax = 0; jmax < 6; jmax++) {
      for (b_j = 0; b_j < 6; b_j++) {
        SQ_0[jmax + 6 * b_j] = 0.0F;
        for (colj = 0; colj < 6; colj++) {
          SQ_0[jmax + 6 * b_j] += SQ[6 * jmax + colj] * SQ[6 * b_j + colj];
        }

        Y[jmax + 6 * b_j] = lambda * p[jmax] * (ajj * p[b_j]);
      }
    }

    for (jmax = 0; jmax < 6; jmax++) {
      for (b_j = 0; b_j < 6; b_j++) {
        rtb_P1[b_j + 6 * jmax] = SQ_0[6 * jmax + b_j] - Y[6 * jmax + b_j];
      }
    }

    i = 0;
    jmax = 0;
    b_j = 1;
    exitg1 = false;
    while ((!exitg1) && (b_j < 7)) {
      colj = (jmax + b_j) - 1;
      lambda = 0.0F;
      if (!(b_j - 1 < 1)) {
        jj = jmax;
        ix = jmax;
        for (iy = 1; iy < b_j; iy++) {
          lambda += rtb_P1[jj] * rtb_P1[ix];
          jj++;
          ix++;
        }
      }

      lambda = rtb_P1[colj] - lambda;
      if (lambda > 0.0F) {
        lambda = sqrtf(lambda);
        rtb_P1[colj] = lambda;
        if (b_j < 6) {
          if (b_j - 1 != 0) {
            jj = colj + 6;
            ix = ((5 - b_j) * 6 + jmax) + 7;
            for (iy = jmax + 7; iy <= ix; iy += 6) {
              d_ix = jmax;
              ajj = 0.0F;
              b_ix = (iy + b_j) - 2;
              for (e = iy; e <= b_ix; e++) {
                ajj += rtb_P1[e - 1] * rtb_P1[d_ix];
                d_ix++;
              }

              rtb_P1[jj] += -ajj;
              jj += 6;
            }
          }

          lambda = 1.0F / lambda;
          jj = ((5 - b_j) * 6 + colj) + 7;
          for (colj += 6; colj + 1 <= jj; colj += 6) {
            rtb_P1[colj] *= lambda;
          }

          jmax += 6;
        }

        b_j++;
      } else {
        rtb_P1[colj] = lambda;
        i = b_j;
        exitg1 = true;
      }
    }

    if (i == 0) {
      i = 6;
    } else {
      i--;
    }

    for (b_j = 0; b_j + 1 <= i; b_j++) {
      for (jmax = b_j + 1; jmax + 1 <= i; jmax++) {
        rtb_P1[jmax + 6 * b_j] = 0.0F;
      }
    }
  } else {
    /* '<S2>:1:46' */
    lambda = powf(rtb_Wc[0], 0.25F);
    ajj = powf(rtb_Wc[0], 0.25F);
    for (jmax = 0; jmax < 6; jmax++) {
      for (b_j = 0; b_j < 6; b_j++) {
        SQ_0[jmax + 6 * b_j] = 0.0F;
        for (colj = 0; colj < 6; colj++) {
          SQ_0[jmax + 6 * b_j] += SQ[6 * jmax + colj] * SQ[6 * b_j + colj];
        }

        Y[jmax + 6 * b_j] = lambda * p[jmax] * (ajj * p[b_j]);
      }
    }

    for (jmax = 0; jmax < 6; jmax++) {
      for (b_j = 0; b_j < 6; b_j++) {
        rtb_P1[b_j + 6 * jmax] = SQ_0[6 * jmax + b_j] + Y[6 * jmax + b_j];
      }
    }

    jmax = 0;
    colj = 0;
    b_j = 1;
    exitg1 = false;
    while ((!exitg1) && (b_j < 7)) {
      jj = (colj + b_j) - 1;
      lambda = 0.0F;
      if (!(b_j - 1 < 1)) {
        ix = colj;
        iy = colj;
        for (d_ix = 1; d_ix < b_j; d_ix++) {
          lambda += rtb_P1[ix] * rtb_P1[iy];
          ix++;
          iy++;
        }
      }

      ajj = rtb_P1[jj] - lambda;
      if (ajj > 0.0F) {
        ajj = sqrtf(ajj);
        rtb_P1[jj] = ajj;
        if (b_j < 6) {
          if (b_j - 1 != 0) {
            ix = jj + 6;
            iy = ((5 - b_j) * 6 + colj) + 7;
            for (d_ix = colj + 7; d_ix <= iy; d_ix += 6) {
              b_ix = colj;
              lambda = 0.0F;
              e = (d_ix + b_j) - 2;
              for (ia = d_ix; ia <= e; ia++) {
                lambda += rtb_P1[ia - 1] * rtb_P1[b_ix];
                b_ix++;
              }

              rtb_P1[ix] += -lambda;
              ix += 6;
            }
          }

          lambda = 1.0F / ajj;
          ix = ((5 - b_j) * 6 + jj) + 7;
          for (jj += 6; jj + 1 <= ix; jj += 6) {
            rtb_P1[jj] *= lambda;
          }

          colj += 6;
        }

        b_j++;
      } else {
        rtb_P1[jj] = ajj;
        jmax = b_j;
        exitg1 = true;
      }
    }

    if (jmax == 0) {
      jmax = 6;
    } else {
      jmax--;
    }

    for (b_j = 0; b_j + 1 <= jmax; b_j++) {
      for (colj = b_j + 1; colj + 1 <= jmax; colj++) {
        rtb_P1[colj + 6 * b_j] = 0.0F;
      }
    }
  }

  /* End of MATLAB Function: '<Root>/UKF_prediction' */

  /* MATLAB Function: '<Root>/UKF_correction' incorporates:
   *  Inport: '<Root>/R'
   *  Inport: '<Root>/q'
   *  MATLAB Function: '<Root>/initialization'
   */
  /* O=1; */
  /* S=1; */
  /* s=1; */
  /* MATLAB Function 'UKF_correction': '<S1>:1' */
  /* '<S1>:1:47' */
  /* '<S1>:1:36' */
  /* ----------------Correction Step------------------------- */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*       By Condomines Jean_Philippe - 1 may 2015-    */
  /*                    jp.condomines@gmail.com */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* Sigma point and output fonction  h(x_k) */
  /* Inputs :  */
  /* 		-Sigmas-points (state)   :                  X      	 */
  /* 		-Weighting sigmas-points  :                 Wm, Wc 	 */
  /* 		-Dimension of the measurement vector:       n		 */
  /* 		-Dimension 2n+1 :                           L  		 */
  /* 		-Weighting measures (Vk+1):                 R  		 */
  /* 		-Measures :                                 zk 		 */
  /* Outputs : */
  /* 		-Sigmas_measures :                          Z 		 */
  /* 		-Predicted measurement :                    z 		 */
  /* 		-Covariance on measures :                   J 		 */
  /* ---------------------------------------------------- */
  /* Zeros vector for predicted measures */
  /* Zeros vector for predicted Sigmas_measures */
  /* '<S1>:1:30' */
  memset(&rtb_Z[0], 0, 65U * sizeof(real32_T));

  /* "for" loop for "measures predicted" and "sigmas_measures" predicted */
  /* '<S1>:1:22' */
  /* '<S1>:1:33' */
  for (i = 0; i < 13; i++) {
    /* '<S1>:1:33' */
    /* non linear function h(x), results : Z(:,k)=[zk+1(0) ... zk+1(2n+1)] */
    /* '<S1>:1:36' */
    h(&rtb_Y[6 * i], rtU.q, &rtb_Z[5 * i]);

    /* Predicted measurement */
    /*  z=z+Wm(k)*Z(:,k);  */
    /* '<S1>:1:40' */
    for (jmax = 0; jmax < 5; jmax++) {
      rtb_z[jmax] = rtb_Z[jmax];
    }

    /* '<S1>:1:33' */
  }

  /* Square-root of R */
  /* '<S1>:1:44' */
  for (i = 0; i < 25; i++) {
    SR[i] = sqrtf(rtU.R[i]);
  }

  /* Subtracting of "predicted Sigma measures" et "predicted measurement" with ponderation "Wc" */
  /* '<S1>:1:47' */
  lambda = sqrtf(rtb_Wm[1]);

  /* Concatenation with the square root of the matrix "R" */
  /* '<S1>:1:50' */
  /* factorization qr, result use in "J" and rest in "Temp"  */
  /* Sz,k+1 = qr{[sqrt(Wc(1)) * [Zk+1(:,2:2n+1)-zk+1]  sqrt(R)] */
  /* '<S1>:1:54' */
  for (jmax = 0; jmax < 5; jmax++) {
    for (b_j = 0; b_j < 12; b_j++) {
      lambda_0[b_j + 17 * jmax] = (rtb_Z[(1 + b_j) * 5 + jmax] - rtb_z[jmax]) *
        lambda;
    }
  }

  for (jmax = 0; jmax < 5; jmax++) {
    for (b_j = 0; b_j < 5; b_j++) {
      lambda_0[(b_j + 17 * jmax) + 12] = SR[5 * b_j + jmax];
    }
  }

  qr(lambda_0, Temp_0, SR);

  /* Subtracting the first column "zk+1(0)" with all the measurements zk+1 predicted */
  /* '<S1>:1:57' */
  for (jmax = 0; jmax < 5; jmax++) {
    p_0[jmax] = rtb_Z[jmax] - rtb_z[jmax];
  }

  /* Test on Wc(1) for cholesky factorization : Cholupdate{Sz,k+1,p,Wc(0)}  */
  if (rtb_Wc[0] < 0.0F) {
    /* '<S1>:1:60' */
    /* '<S1>:1:61' */
    lambda = powf(-rtb_Wc[0], 0.25F);
    ajj = powf(-rtb_Wc[0], 0.25F);
    for (jmax = 0; jmax < 5; jmax++) {
      for (b_j = 0; b_j < 5; b_j++) {
        SR_0[jmax + 5 * b_j] = 0.0F;
        for (colj = 0; colj < 5; colj++) {
          SR_0[jmax + 5 * b_j] += SR[5 * jmax + colj] * SR[5 * b_j + colj];
        }

        lambda_2[jmax + 5 * b_j] = lambda * p_0[jmax] * (ajj * p_0[b_j]);
      }
    }

    for (jmax = 0; jmax < 5; jmax++) {
      for (b_j = 0; b_j < 5; b_j++) {
        SR[b_j + 5 * jmax] = SR_0[5 * jmax + b_j] - lambda_2[5 * jmax + b_j];
      }
    }

    i = 0;
    jmax = 0;
    b_j = 1;
    exitg1 = false;
    while ((!exitg1) && (b_j < 6)) {
      colj = (jmax + b_j) - 1;
      lambda = 0.0F;
      if (!(b_j - 1 < 1)) {
        jj = jmax;
        ix = jmax;
        for (iy = 1; iy < b_j; iy++) {
          lambda += SR[jj] * SR[ix];
          jj++;
          ix++;
        }
      }

      lambda = SR[colj] - lambda;
      if (lambda > 0.0F) {
        lambda = sqrtf(lambda);
        SR[colj] = lambda;
        if (b_j < 5) {
          if (b_j - 1 != 0) {
            jj = colj + 5;
            ix = ((4 - b_j) * 5 + jmax) + 6;
            for (iy = jmax + 6; iy <= ix; iy += 5) {
              d_ix = jmax;
              ajj = 0.0F;
              b_ix = (iy + b_j) - 2;
              for (e = iy; e <= b_ix; e++) {
                ajj += SR[e - 1] * SR[d_ix];
                d_ix++;
              }

              SR[jj] += -ajj;
              jj += 5;
            }
          }

          lambda = 1.0F / lambda;
          jj = ((4 - b_j) * 5 + colj) + 6;
          for (colj += 5; colj + 1 <= jj; colj += 5) {
            SR[colj] *= lambda;
          }

          jmax += 5;
        }

        b_j++;
      } else {
        SR[colj] = lambda;
        i = b_j;
        exitg1 = true;
      }
    }

    if (i == 0) {
      i = 5;
    } else {
      i--;
    }

    for (b_j = 0; b_j + 1 <= i; b_j++) {
      for (jmax = b_j + 1; jmax + 1 <= i; jmax++) {
        SR[jmax + 5 * b_j] = 0.0F;
      }
    }
  } else {
    /* '<S1>:1:63' */
    lambda = powf(rtb_Wc[0], 0.25F);
    ajj = powf(rtb_Wc[0], 0.25F);
    for (jmax = 0; jmax < 5; jmax++) {
      for (b_j = 0; b_j < 5; b_j++) {
        SR_0[jmax + 5 * b_j] = 0.0F;
        for (colj = 0; colj < 5; colj++) {
          SR_0[jmax + 5 * b_j] += SR[5 * jmax + colj] * SR[5 * b_j + colj];
        }

        lambda_2[jmax + 5 * b_j] = lambda * p_0[jmax] * (ajj * p_0[b_j]);
      }
    }

    for (jmax = 0; jmax < 5; jmax++) {
      for (b_j = 0; b_j < 5; b_j++) {
        SR[b_j + 5 * jmax] = SR_0[5 * jmax + b_j] + lambda_2[5 * jmax + b_j];
      }
    }

    jmax = 0;
    colj = 0;
    b_j = 1;
    exitg1 = false;
    while ((!exitg1) && (b_j < 6)) {
      jj = (colj + b_j) - 1;
      lambda = 0.0F;
      if (!(b_j - 1 < 1)) {
        ix = colj;
        iy = colj;
        for (d_ix = 1; d_ix < b_j; d_ix++) {
          lambda += SR[ix] * SR[iy];
          ix++;
          iy++;
        }
      }

      ajj = SR[jj] - lambda;
      if (ajj > 0.0F) {
        ajj = sqrtf(ajj);
        SR[jj] = ajj;
        if (b_j < 5) {
          if (b_j - 1 != 0) {
            ix = jj + 5;
            iy = ((4 - b_j) * 5 + colj) + 6;
            for (d_ix = colj + 6; d_ix <= iy; d_ix += 5) {
              b_ix = colj;
              lambda = 0.0F;
              e = (d_ix + b_j) - 2;
              for (ia = d_ix; ia <= e; ia++) {
                lambda += SR[ia - 1] * SR[b_ix];
                b_ix++;
              }

              SR[ix] += -lambda;
              ix += 5;
            }
          }

          lambda = 1.0F / ajj;
          ix = ((4 - b_j) * 5 + jj) + 6;
          for (jj += 5; jj + 1 <= ix; jj += 5) {
            SR[jj] *= lambda;
          }

          colj += 5;
        }

        b_j++;
      } else {
        SR[jj] = ajj;
        jmax = b_j;
        exitg1 = true;
      }
    }

    if (jmax == 0) {
      jmax = 5;
    } else {
      jmax--;
    }

    for (b_j = 0; b_j + 1 <= jmax; b_j++) {
      for (colj = b_j + 1; colj + 1 <= jmax; colj++) {
        SR[colj + 5 * b_j] = 0.0F;
      }
    }
  }

  /* Transposition to have a lower triangular matrix */
  /* '<S1>:1:67' */
  for (jmax = 0; jmax < 5; jmax++) {
    for (b_j = 0; b_j < 5; b_j++) {
      SR_0[b_j + 5 * jmax] = SR[5 * b_j + jmax];
    }
  }

  for (jmax = 0; jmax < 5; jmax++) {
    for (b_j = 0; b_j < 5; b_j++) {
      SR[b_j + 5 * jmax] = SR_0[5 * jmax + b_j];
    }
  }

  /* End of MATLAB Function: '<Root>/UKF_correction' */

  /* MATLAB Function: '<Root>/main' incorporates:
   *  Inport: '<Root>/zk'
   */
  /* T=1; */
  /* A=1; */
  /* a=1; */
  memcpy(&SQ[0], &rtb_P1[0], 36U * sizeof(real32_T));

  /* MATLAB Function 'main': '<S4>:1' */
  /* '<S4>:1:5' */
  memset(&d[0], 0, 169U * sizeof(real32_T));

  /* Kalman gain : (Pxz/Sz')/Sz */
  /* Caution "/" is least squares root  for "Ax = b" */
  /* '<S4>:1:9' */
  for (b_j = 0; b_j < 13; b_j++) {
    d[b_j + 13 * b_j] = rtb_Wc[b_j];
    for (jmax = 0; jmax < 6; jmax++) {
      rtb_X[jmax + 6 * b_j] = rtb_Y[6 * b_j + jmax] - rtb_x1[jmax];
    }
  }

  for (jmax = 0; jmax < 6; jmax++) {
    for (b_j = 0; b_j < 13; b_j++) {
      rtb_Y[jmax + 6 * b_j] = 0.0F;
      for (colj = 0; colj < 13; colj++) {
        rtb_Y[jmax + 6 * b_j] += rtb_X[6 * colj + jmax] * d[13 * b_j + colj];
      }
    }
  }

  for (jmax = 0; jmax < 5; jmax++) {
    for (b_j = 0; b_j < 13; b_j++) {
      rtb_Z_0[b_j + 13 * jmax] = rtb_Z[5 * b_j + jmax] - rtb_z[jmax];
    }
  }

  for (jmax = 0; jmax < 6; jmax++) {
    for (b_j = 0; b_j < 5; b_j++) {
      K[jmax + 6 * b_j] = 0.0F;
      for (colj = 0; colj < 13; colj++) {
        K[jmax + 6 * b_j] += rtb_Y[6 * colj + jmax] * rtb_Z_0[13 * b_j + colj];
      }
    }
  }

  for (jmax = 0; jmax < 5; jmax++) {
    for (b_j = 0; b_j < 5; b_j++) {
      SR_0[b_j + 5 * jmax] = SR[5 * b_j + jmax];
    }
  }

  mrdivide(K, SR_0);
  mrdivide(K, SR);

  /* Predicted state with Kalman gain "K" : x = xpred + K * (zk - z) */
  /* '<S4>:1:12' */
  /* Quaternion normalisation */
  /* x(1:4)=normalQ(x(1:4)'); */
  /* Cholesky factorization : U = K * Sz    */
  /* '<S4>:1:18' */
  for (jmax = 0; jmax < 6; jmax++) {
    for (b_j = 0; b_j < 5; b_j++) {
      U[jmax + 6 * b_j] = 0.0F;
      for (colj = 0; colj < 5; colj++) {
        U[jmax + 6 * b_j] += K[6 * colj + jmax] * SR[5 * b_j + colj];
      }
    }
  }

  /* "for" loop for cholesky factorization */
  /* '<S4>:1:21' */
  for (i = 0; i < 5; i++) {
    /* '<S4>:1:21' */
    /* '<S4>:1:22' */
    jmax = 1 + i;
    b_j = 1 + i;
    for (colj = 0; colj < 6; colj++) {
      for (jj = 0; jj < 6; jj++) {
        SQ_0[colj + 6 * jj] = 0.0F;
        for (ix = 0; ix < 6; ix++) {
          SQ_0[colj + 6 * jj] += SQ[6 * colj + ix] * SQ[6 * jj + ix];
        }

        rtb_P1[colj + 6 * jj] = U[(jmax - 1) * 6 + colj] * U[(b_j - 1) * 6 + jj];
      }
    }

    for (jmax = 0; jmax < 6; jmax++) {
      for (b_j = 0; b_j < 6; b_j++) {
        Y[b_j + 6 * jmax] = SQ_0[6 * jmax + b_j] - rtb_P1[6 * jmax + b_j];
      }
    }

    jmax = 0;
    colj = 0;
    b_j = 1;
    exitg1 = false;
    while ((!exitg1) && (b_j < 7)) {
      jj = (colj + b_j) - 1;
      lambda = 0.0F;
      if (!(b_j - 1 < 1)) {
        ix = colj;
        iy = colj;
        for (d_ix = 1; d_ix < b_j; d_ix++) {
          lambda += Y[ix] * Y[iy];
          ix++;
          iy++;
        }
      }

      ajj = Y[jj] - lambda;
      if (ajj > 0.0F) {
        ajj = sqrtf(ajj);
        Y[jj] = ajj;
        if (b_j < 6) {
          if (b_j - 1 != 0) {
            ix = jj + 6;
            iy = ((5 - b_j) * 6 + colj) + 7;
            for (d_ix = colj + 7; d_ix <= iy; d_ix += 6) {
              b_ix = colj;
              lambda = 0.0F;
              e = (d_ix + b_j) - 2;
              for (ia = d_ix; ia <= e; ia++) {
                lambda += Y[ia - 1] * Y[b_ix];
                b_ix++;
              }

              Y[ix] += -lambda;
              ix += 6;
            }
          }

          lambda = 1.0F / ajj;
          ix = ((5 - b_j) * 6 + jj) + 7;
          for (jj += 6; jj + 1 <= ix; jj += 6) {
            Y[jj] *= lambda;
          }

          colj += 6;
        }

        b_j++;
      } else {
        Y[jj] = ajj;
        jmax = b_j;
        exitg1 = true;
      }
    }

    if (jmax == 0) {
      jmax = 6;
    } else {
      jmax--;
    }

    for (b_j = 0; b_j + 1 <= jmax; b_j++) {
      for (colj = b_j + 1; colj + 1 <= jmax; colj++) {
        Y[colj + 6 * b_j] = 0.0F;
      }
    }

    memcpy(&SQ[0], &Y[0], 36U * sizeof(real32_T));

    /* '<S4>:1:21' */
  }

  /* "Sk" in "P" and Transpose of "P" */
  /* '<S4>:1:27' */
  /* '<S4>:1:29' */
  for (jmax = 0; jmax < 6; jmax++) {
    for (b_j = 0; b_j < 6; b_j++) {
      Y[jmax + 6 * b_j] = 0.0F;
      for (colj = 0; colj < 6; colj++) {
        Y[jmax + 6 * b_j] += SQ[6 * colj + jmax] * SQ[6 * colj + b_j];
      }
    }
  }

  for (b_j = 0; b_j < 6; b_j++) {
    p[b_j] = Y[b_j * 7];
    p[b_j] = sqrtf(p[b_j]);

    /* Outport: '<Root>/sqrtP' */
    rtY.sqrtP[b_j] = p[b_j];
    for (jmax = 0; jmax < 6; jmax++) {
      rtb_P1[jmax + 6 * b_j] = SQ[6 * jmax + b_j];
    }
  }

  for (jmax = 0; jmax < 5; jmax++) {
    p_0[jmax] = rtU.zk[jmax] - rtb_z[jmax];
  }

  for (i = 0; i < 6; i++) {
    lambda = 0.0F;
    for (jmax = 0; jmax < 5; jmax++) {
      lambda += K[6 * jmax + i] * p_0[jmax];
    }

    p[i] = rtb_x1[i] + lambda;

    /* Outport: '<Root>/xout' */
    rtY.xout[i] = p[i];
  }

  /* End of MATLAB Function: '<Root>/main' */

  /* Outport: '<Root>/Pout' */
  memcpy(&rtY.Pout[0], &rtb_P1[0], 36U * sizeof(real32_T));

  /* Update for UnitDelay: '<Root>/ Delay' */
  memcpy(&rtDW.Delay_DSTATE[0], &rtb_P1[0], 36U * sizeof(real32_T));

  /* Update for UnitDelay: '<Root>/ Delay1' */
  for (i = 0; i < 6; i++) {
    rtDW.Delay1_DSTATE[i] = p[i];
  }

  /* End of Update for UnitDelay: '<Root>/ Delay1' */
}

/* Model initialize function */
void Gen_UKF_initialize(void)
{
  /* (no initialization code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
