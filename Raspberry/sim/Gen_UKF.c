/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Gen_UKF.c
 *
 * Code generated for Simulink model 'Gen_UKF'.
 *
 * Model version                  : 1.8
 * Simulink Coder version         : 8.8 (R2015a) 09-Feb-2015
 * C/C++ source code generated on : Tue Apr 26 10:28:27 2016
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: 32-bit Generic
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "Gen_UKF.h"
#include "Gen_UKF_private.h"

/* Block states (auto storage) */
DW_Gen_UKF_T Gen_UKF_DW;

/* External inputs (root inport signals with auto storage) */
ExtU_Gen_UKF_T Gen_UKF_U;

/* External outputs (root outports fed by signals with auto storage) */
ExtY_Gen_UKF_T Gen_UKF_Y;

/* Real-time model */
RT_MODEL_Gen_UKF_T Gen_UKF_M_;
RT_MODEL_Gen_UKF_T *const Gen_UKF_M = &Gen_UKF_M_;

/* Forward declaration for local functions */
static void Gen_UKF_sqrt(real_T x[36]);
static real_T Gen_UKF_eml_xnrm2_i(int32_T n, const real_T x[108], int32_T ix0);
static void Gen_UKF_eml_xgeqrf_g(real_T A[108], real_T tau[6]);
static void Gen_UKF_qr_m(const real_T A[108], real_T Q[108], real_T R[36]);
static real_T Gen_UKF_eml_xnrm2(int32_T n, const real_T x[85], int32_T ix0);
static void Gen_UKF_eml_xgeqrf(real_T A[85], real_T tau[5]);
static void Gen_UKF_qr(const real_T A[85], real_T Q[85], real_T R[25]);
static void Gen_UKF_mrdivide(real_T A[30], const real_T B[25]);
static void Gen_UKF_h(const real_T x[6], const real_T q[4], real_T y[5]);
static void Gen_UKF_RungeKutta(const real_T x[6], real_T dt, const real_T u[6],
  real_T phi, real_T theta, real_T xi[6]);

/* Function for MATLAB Function: '<Root>/UKF_prediction' */
static void Gen_UKF_sqrt(real_T x[36])
{
  int32_T k;
  for (k = 0; k < 36; k++) {
    x[k] = sqrt(x[k]);
  }
}

/* Function for MATLAB Function: '<Root>/UKF_prediction' */
static real_T Gen_UKF_eml_xnrm2_i(int32_T n, const real_T x[108], int32_T ix0)
{
  real_T y;
  real_T scale;
  int32_T kend;
  real_T absxk;
  real_T t;
  int32_T k;
  y = 0.0;
  if (!(n < 1)) {
    if (n == 1) {
      y = fabs(x[ix0 - 1]);
    } else {
      scale = 2.2250738585072014E-308;
      kend = (ix0 + n) - 1;
      for (k = ix0; k <= kend; k++) {
        absxk = fabs(x[k - 1]);
        if (absxk > scale) {
          t = scale / absxk;
          y = y * t * t + 1.0;
          scale = absxk;
        } else {
          t = absxk / scale;
          y += t * t;
        }
      }

      y = scale * sqrt(y);
    }
  }

  return y;
}

/* Function for MATLAB Function: '<Root>/UKF_prediction' */
static void Gen_UKF_eml_xgeqrf_g(real_T A[108], real_T tau[6])
{
  real_T work[6];
  int32_T i_i;
  real_T atmp;
  int32_T i_ip1;
  real_T b;
  real_T xnorm;
  int32_T knt;
  int32_T lastv;
  int32_T b_i;
  int32_T lastc;
  int32_T ix;
  int32_T g;
  int32_T b_ia;
  int32_T b_ix;
  boolean_T exitg2;
  int32_T i;
  for (i = 0; i < 6; i++) {
    work[i] = 0.0;
  }

  for (i = 0; i < 6; i++) {
    i_i = i * 18 + i;
    atmp = A[i_i];
    b = 0.0;
    xnorm = Gen_UKF_eml_xnrm2_i(17 - i, A, i_i + 2);
    if (xnorm != 0.0) {
      xnorm = hypot(A[i_i], xnorm);
      if (A[i_i] >= 0.0) {
        xnorm = -xnorm;
      }

      if (fabs(xnorm) < 1.0020841800044864E-292) {
        knt = 0;
        do {
          knt++;
          b_i = (i_i - i) + 18;
          for (b_ix = i_i + 1; b_ix + 1 <= b_i; b_ix++) {
            A[b_ix] *= 9.9792015476736E+291;
          }

          xnorm *= 9.9792015476736E+291;
          atmp *= 9.9792015476736E+291;
        } while (!(fabs(xnorm) >= 1.0020841800044864E-292));

        xnorm = hypot(atmp, Gen_UKF_eml_xnrm2_i(17 - i, A, i_i + 2));
        if (atmp >= 0.0) {
          xnorm = -xnorm;
        }

        b = (xnorm - atmp) / xnorm;
        atmp = 1.0 / (atmp - xnorm);
        b_i = (i_i - i) + 18;
        for (b_ix = i_i + 1; b_ix + 1 <= b_i; b_ix++) {
          A[b_ix] *= atmp;
        }

        for (b_i = 1; b_i <= knt; b_i++) {
          xnorm *= 1.0020841800044864E-292;
        }

        atmp = xnorm;
      } else {
        b = (xnorm - A[i_i]) / xnorm;
        atmp = 1.0 / (A[i_i] - xnorm);
        b_i = (i_i - i) + 18;
        for (b_ix = i_i + 1; b_ix + 1 <= b_i; b_ix++) {
          A[b_ix] *= atmp;
        }

        atmp = xnorm;
      }
    }

    tau[i] = b;
    A[i_i] = atmp;
    if (i + 1 < 6) {
      atmp = A[i_i];
      A[i_i] = 1.0;
      i_ip1 = (i + 1) * 18 + i;
      if (tau[i] != 0.0) {
        lastv = 18 - i;
        b_i = (i_i - i) + 17;
        while ((lastv > 0) && (A[b_i] == 0.0)) {
          lastv--;
          b_i--;
        }

        lastc = 5 - i;
        exitg2 = false;
        while ((!exitg2) && (lastc > 0)) {
          b_ix = ((lastc - 1) * 18 + i_ip1) + 1;
          b_i = b_ix;
          do {
            knt = 0;
            if (b_i <= (b_ix + lastv) - 1) {
              if (A[b_i - 1] != 0.0) {
                knt = 1;
              } else {
                b_i++;
              }
            } else {
              lastc--;
              knt = 2;
            }
          } while (knt == 0);

          if (knt == 1) {
            exitg2 = true;
          }
        }
      } else {
        lastv = 0;
        lastc = 0;
      }

      if (lastv > 0) {
        if (lastc != 0) {
          for (b_ix = 1; b_ix <= lastc; b_ix++) {
            work[b_ix - 1] = 0.0;
          }

          b_i = 0;
          knt = ((lastc - 1) * 18 + i_ip1) + 1;
          for (b_ix = i_ip1 + 1; b_ix <= knt; b_ix += 18) {
            ix = i_i;
            xnorm = 0.0;
            g = (b_ix + lastv) - 1;
            for (b_ia = b_ix; b_ia <= g; b_ia++) {
              xnorm += A[b_ia - 1] * A[ix];
              ix++;
            }

            work[b_i] += xnorm;
            b_i++;
          }
        }

        if (!(-tau[i] == 0.0)) {
          b_i = 0;
          for (knt = 1; knt <= lastc; knt++) {
            if (work[b_i] != 0.0) {
              xnorm = work[b_i] * -tau[i];
              b_ix = i_i;
              ix = lastv + i_ip1;
              for (g = i_ip1; g + 1 <= ix; g++) {
                A[g] += A[b_ix] * xnorm;
                b_ix++;
              }
            }

            b_i++;
            i_ip1 += 18;
          }
        }
      }

      A[i_i] = atmp;
    }
  }
}

/* Function for MATLAB Function: '<Root>/UKF_prediction' */
static void Gen_UKF_qr_m(const real_T A[108], real_T Q[108], real_T R[36])
{
  real_T tau[6];
  int32_T c_i;
  int32_T itau;
  real_T work[6];
  int32_T iaii;
  int32_T lastv;
  int32_T d_i;
  int32_T lastc;
  int32_T ix;
  real_T c;
  int32_T iac;
  int32_T d;
  int32_T b_ia;
  int32_T jy;
  boolean_T exitg2;
  memcpy(&Q[0], &A[0], 108U * sizeof(real_T));
  Gen_UKF_eml_xgeqrf_g(Q, tau);
  for (d_i = 0; d_i < 6; d_i++) {
    for (jy = 0; jy + 1 <= d_i + 1; jy++) {
      R[jy + 6 * d_i] = Q[18 * d_i + jy];
    }

    for (jy = d_i + 1; jy + 1 < 7; jy++) {
      R[jy + 6 * d_i] = 0.0;
    }
  }

  itau = 5;
  for (d_i = 0; d_i < 6; d_i++) {
    work[d_i] = 0.0;
  }

  for (c_i = 5; c_i >= 0; c_i += -1) {
    iaii = (c_i * 18 + c_i) + 1;
    if (c_i + 1 < 6) {
      Q[iaii - 1] = 1.0;
      if (tau[itau] != 0.0) {
        lastv = 18 - c_i;
        d_i = (iaii - c_i) + 16;
        while ((lastv > 0) && (Q[d_i] == 0.0)) {
          lastv--;
          d_i--;
        }

        lastc = 5 - c_i;
        exitg2 = false;
        while ((!exitg2) && (lastc > 0)) {
          d_i = (lastc - 1) * 18 + iaii;
          jy = d_i;
          do {
            iac = 0;
            if (jy + 18 <= (d_i + lastv) + 17) {
              if (Q[jy + 17] != 0.0) {
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
          for (d_i = 1; d_i <= lastc; d_i++) {
            work[d_i - 1] = 0.0;
          }

          d_i = 0;
          jy = ((lastc - 1) * 18 + iaii) + 18;
          for (iac = iaii + 18; iac <= jy; iac += 18) {
            ix = iaii;
            c = 0.0;
            d = (iac + lastv) - 1;
            for (b_ia = iac; b_ia <= d; b_ia++) {
              c += Q[b_ia - 1] * Q[ix - 1];
              ix++;
            }

            work[d_i] += c;
            d_i++;
          }
        }

        if (!(-tau[itau] == 0.0)) {
          d_i = iaii + 17;
          jy = 0;
          for (iac = 1; iac <= lastc; iac++) {
            if (work[jy] != 0.0) {
              c = work[jy] * -tau[itau];
              ix = iaii;
              d = lastv + d_i;
              for (b_ia = d_i; b_ia + 1 <= d; b_ia++) {
                Q[b_ia] += Q[ix - 1] * c;
                ix++;
              }
            }

            jy++;
            d_i += 18;
          }
        }
      }
    }

    jy = (iaii - c_i) + 17;
    for (d_i = iaii; d_i + 1 <= jy; d_i++) {
      Q[d_i] *= -tau[itau];
    }

    Q[iaii - 1] = 1.0 - tau[itau];
    for (jy = 1; jy <= c_i; jy++) {
      Q[(iaii - jy) - 1] = 0.0;
    }

    itau--;
  }
}

/* Function for MATLAB Function: '<Root>/UKF_correction' */
static real_T Gen_UKF_eml_xnrm2(int32_T n, const real_T x[85], int32_T ix0)
{
  real_T y;
  real_T scale;
  int32_T kend;
  real_T absxk;
  real_T t;
  int32_T k;
  y = 0.0;
  if (!(n < 1)) {
    if (n == 1) {
      y = fabs(x[ix0 - 1]);
    } else {
      scale = 2.2250738585072014E-308;
      kend = (ix0 + n) - 1;
      for (k = ix0; k <= kend; k++) {
        absxk = fabs(x[k - 1]);
        if (absxk > scale) {
          t = scale / absxk;
          y = y * t * t + 1.0;
          scale = absxk;
        } else {
          t = absxk / scale;
          y += t * t;
        }
      }

      y = scale * sqrt(y);
    }
  }

  return y;
}

/* Function for MATLAB Function: '<Root>/UKF_correction' */
static void Gen_UKF_eml_xgeqrf(real_T A[85], real_T tau[5])
{
  real_T work[5];
  int32_T i_i;
  real_T atmp;
  int32_T i_ip1;
  real_T b;
  real_T xnorm;
  int32_T knt;
  int32_T lastv;
  int32_T b_i;
  int32_T lastc;
  int32_T ix;
  int32_T g;
  int32_T b_ia;
  int32_T b_ix;
  boolean_T exitg2;
  int32_T i;
  for (i = 0; i < 5; i++) {
    work[i] = 0.0;
  }

  for (i = 0; i < 5; i++) {
    i_i = i * 17 + i;
    atmp = A[i_i];
    b = 0.0;
    xnorm = Gen_UKF_eml_xnrm2(16 - i, A, i_i + 2);
    if (xnorm != 0.0) {
      xnorm = hypot(A[i_i], xnorm);
      if (A[i_i] >= 0.0) {
        xnorm = -xnorm;
      }

      if (fabs(xnorm) < 1.0020841800044864E-292) {
        knt = 0;
        do {
          knt++;
          b_i = (i_i - i) + 17;
          for (b_ix = i_i + 1; b_ix + 1 <= b_i; b_ix++) {
            A[b_ix] *= 9.9792015476736E+291;
          }

          xnorm *= 9.9792015476736E+291;
          atmp *= 9.9792015476736E+291;
        } while (!(fabs(xnorm) >= 1.0020841800044864E-292));

        xnorm = hypot(atmp, Gen_UKF_eml_xnrm2(16 - i, A, i_i + 2));
        if (atmp >= 0.0) {
          xnorm = -xnorm;
        }

        b = (xnorm - atmp) / xnorm;
        atmp = 1.0 / (atmp - xnorm);
        b_i = (i_i - i) + 17;
        for (b_ix = i_i + 1; b_ix + 1 <= b_i; b_ix++) {
          A[b_ix] *= atmp;
        }

        for (b_i = 1; b_i <= knt; b_i++) {
          xnorm *= 1.0020841800044864E-292;
        }

        atmp = xnorm;
      } else {
        b = (xnorm - A[i_i]) / xnorm;
        atmp = 1.0 / (A[i_i] - xnorm);
        b_i = (i_i - i) + 17;
        for (b_ix = i_i + 1; b_ix + 1 <= b_i; b_ix++) {
          A[b_ix] *= atmp;
        }

        atmp = xnorm;
      }
    }

    tau[i] = b;
    A[i_i] = atmp;
    if (i + 1 < 5) {
      atmp = A[i_i];
      A[i_i] = 1.0;
      i_ip1 = (i + 1) * 17 + i;
      if (tau[i] != 0.0) {
        lastv = 17 - i;
        b_i = (i_i - i) + 16;
        while ((lastv > 0) && (A[b_i] == 0.0)) {
          lastv--;
          b_i--;
        }

        lastc = 4 - i;
        exitg2 = false;
        while ((!exitg2) && (lastc > 0)) {
          b_ix = ((lastc - 1) * 17 + i_ip1) + 1;
          b_i = b_ix;
          do {
            knt = 0;
            if (b_i <= (b_ix + lastv) - 1) {
              if (A[b_i - 1] != 0.0) {
                knt = 1;
              } else {
                b_i++;
              }
            } else {
              lastc--;
              knt = 2;
            }
          } while (knt == 0);

          if (knt == 1) {
            exitg2 = true;
          }
        }
      } else {
        lastv = 0;
        lastc = 0;
      }

      if (lastv > 0) {
        if (lastc != 0) {
          for (b_ix = 1; b_ix <= lastc; b_ix++) {
            work[b_ix - 1] = 0.0;
          }

          b_i = 0;
          knt = ((lastc - 1) * 17 + i_ip1) + 1;
          for (b_ix = i_ip1 + 1; b_ix <= knt; b_ix += 17) {
            ix = i_i;
            xnorm = 0.0;
            g = (b_ix + lastv) - 1;
            for (b_ia = b_ix; b_ia <= g; b_ia++) {
              xnorm += A[b_ia - 1] * A[ix];
              ix++;
            }

            work[b_i] += xnorm;
            b_i++;
          }
        }

        if (!(-tau[i] == 0.0)) {
          b_i = 0;
          for (knt = 1; knt <= lastc; knt++) {
            if (work[b_i] != 0.0) {
              xnorm = work[b_i] * -tau[i];
              b_ix = i_i;
              ix = lastv + i_ip1;
              for (g = i_ip1; g + 1 <= ix; g++) {
                A[g] += A[b_ix] * xnorm;
                b_ix++;
              }
            }

            b_i++;
            i_ip1 += 17;
          }
        }
      }

      A[i_i] = atmp;
    }
  }
}

/* Function for MATLAB Function: '<Root>/UKF_correction' */
static void Gen_UKF_qr(const real_T A[85], real_T Q[85], real_T R[25])
{
  real_T tau[5];
  int32_T c_i;
  int32_T itau;
  real_T work[5];
  int32_T iaii;
  int32_T lastv;
  int32_T d_i;
  int32_T lastc;
  int32_T ix;
  real_T c;
  int32_T iac;
  int32_T d;
  int32_T b_ia;
  int32_T jy;
  boolean_T exitg2;
  memcpy(&Q[0], &A[0], 85U * sizeof(real_T));
  Gen_UKF_eml_xgeqrf(Q, tau);
  for (d_i = 0; d_i < 5; d_i++) {
    for (jy = 0; jy + 1 <= d_i + 1; jy++) {
      R[jy + 5 * d_i] = Q[17 * d_i + jy];
    }

    for (jy = d_i + 1; jy + 1 < 6; jy++) {
      R[jy + 5 * d_i] = 0.0;
    }
  }

  itau = 4;
  for (d_i = 0; d_i < 5; d_i++) {
    work[d_i] = 0.0;
  }

  for (c_i = 4; c_i >= 0; c_i += -1) {
    iaii = (c_i * 17 + c_i) + 1;
    if (c_i + 1 < 5) {
      Q[iaii - 1] = 1.0;
      if (tau[itau] != 0.0) {
        lastv = 17 - c_i;
        d_i = (iaii - c_i) + 15;
        while ((lastv > 0) && (Q[d_i] == 0.0)) {
          lastv--;
          d_i--;
        }

        lastc = 4 - c_i;
        exitg2 = false;
        while ((!exitg2) && (lastc > 0)) {
          d_i = (lastc - 1) * 17 + iaii;
          jy = d_i;
          do {
            iac = 0;
            if (jy + 17 <= (d_i + lastv) + 16) {
              if (Q[jy + 16] != 0.0) {
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
          for (d_i = 1; d_i <= lastc; d_i++) {
            work[d_i - 1] = 0.0;
          }

          d_i = 0;
          jy = ((lastc - 1) * 17 + iaii) + 17;
          for (iac = iaii + 17; iac <= jy; iac += 17) {
            ix = iaii;
            c = 0.0;
            d = (iac + lastv) - 1;
            for (b_ia = iac; b_ia <= d; b_ia++) {
              c += Q[b_ia - 1] * Q[ix - 1];
              ix++;
            }

            work[d_i] += c;
            d_i++;
          }
        }

        if (!(-tau[itau] == 0.0)) {
          d_i = iaii + 16;
          jy = 0;
          for (iac = 1; iac <= lastc; iac++) {
            if (work[jy] != 0.0) {
              c = work[jy] * -tau[itau];
              ix = iaii;
              d = lastv + d_i;
              for (b_ia = d_i; b_ia + 1 <= d; b_ia++) {
                Q[b_ia] += Q[ix - 1] * c;
                ix++;
              }
            }

            jy++;
            d_i += 17;
          }
        }
      }
    }

    jy = (iaii - c_i) + 16;
    for (d_i = iaii; d_i + 1 <= jy; d_i++) {
      Q[d_i] *= -tau[itau];
    }

    Q[iaii - 1] = 1.0 - tau[itau];
    for (jy = 1; jy <= c_i; jy++) {
      Q[(iaii - jy) - 1] = 0.0;
    }

    itau--;
  }
}

/* Function for MATLAB Function: '<Root>/main' */
static void Gen_UKF_mrdivide(real_T A[30], const real_T B[25])
{
  int32_T jp;
  real_T temp;
  real_T b_A[25];
  int8_T ipiv[5];
  int32_T j;
  int32_T c;
  real_T s;
  int32_T c_ix;
  int32_T jBcol;
  int32_T b_jAcol;
  int32_T b_jBcol;
  int32_T b_kBcol;
  memcpy(&b_A[0], &B[0], 25U * sizeof(real_T));
  for (j = 0; j < 5; j++) {
    ipiv[j] = (int8_T)(1 + j);
  }

  for (j = 0; j < 4; j++) {
    c = j * 6;
    b_jBcol = 0;
    b_jAcol = c;
    temp = fabs(b_A[c]);
    for (jp = 2; jp <= 5 - j; jp++) {
      b_jAcol++;
      s = fabs(b_A[b_jAcol]);
      if (s > temp) {
        b_jBcol = jp - 1;
        temp = s;
      }
    }

    if (b_A[c + b_jBcol] != 0.0) {
      if (b_jBcol != 0) {
        ipiv[j] = (int8_T)((j + b_jBcol) + 1);
        b_jAcol = j;
        jp = j + b_jBcol;
        for (b_jBcol = 0; b_jBcol < 5; b_jBcol++) {
          temp = b_A[b_jAcol];
          b_A[b_jAcol] = b_A[jp];
          b_A[jp] = temp;
          b_jAcol += 5;
          jp += 5;
        }
      }

      b_jAcol = (c - j) + 5;
      for (jp = c + 1; jp + 1 <= b_jAcol; jp++) {
        b_A[jp] /= b_A[c];
      }
    }

    jp = c;
    b_jBcol = c + 5;
    for (b_jAcol = 1; b_jAcol <= 4 - j; b_jAcol++) {
      temp = b_A[b_jBcol];
      if (b_A[b_jBcol] != 0.0) {
        c_ix = c + 1;
        b_kBcol = (jp - j) + 10;
        for (jBcol = 6 + jp; jBcol + 1 <= b_kBcol; jBcol++) {
          b_A[jBcol] += b_A[c_ix] * -temp;
          c_ix++;
        }
      }

      b_jBcol += 5;
      jp += 5;
    }
  }

  for (b_kBcol = 0; b_kBcol < 5; b_kBcol++) {
    jBcol = 6 * b_kBcol;
    jp = 5 * b_kBcol;
    for (b_jBcol = 1; b_jBcol <= b_kBcol; b_jBcol++) {
      b_jAcol = (b_jBcol - 1) * 6;
      if (b_A[(b_jBcol + jp) - 1] != 0.0) {
        for (c_ix = 0; c_ix < 6; c_ix++) {
          A[c_ix + jBcol] -= b_A[(b_jBcol + jp) - 1] * A[c_ix + b_jAcol];
        }
      }
    }

    temp = 1.0 / b_A[b_kBcol + jp];
    for (b_jAcol = 0; b_jAcol < 6; b_jAcol++) {
      A[b_jAcol + jBcol] *= temp;
    }
  }

  for (jp = 4; jp >= 0; jp += -1) {
    b_jBcol = 6 * jp;
    b_jAcol = 5 * jp - 1;
    for (c_ix = jp + 2; c_ix < 6; c_ix++) {
      b_kBcol = (c_ix - 1) * 6;
      if (b_A[c_ix + b_jAcol] != 0.0) {
        for (jBcol = 0; jBcol < 6; jBcol++) {
          A[jBcol + b_jBcol] -= b_A[c_ix + b_jAcol] * A[jBcol + b_kBcol];
        }
      }
    }
  }

  for (b_jAcol = 3; b_jAcol >= 0; b_jAcol += -1) {
    if (b_jAcol + 1 != ipiv[b_jAcol]) {
      jp = ipiv[b_jAcol] - 1;
      for (b_jBcol = 0; b_jBcol < 6; b_jBcol++) {
        temp = A[6 * b_jAcol + b_jBcol];
        A[b_jBcol + 6 * b_jAcol] = A[6 * jp + b_jBcol];
        A[b_jBcol + 6 * jp] = temp;
      }
    }
  }
}

/* Function for MATLAB Function: '<Root>/UKF_correction' */
static void Gen_UKF_h(const real_T x[6], const real_T q[4], real_T y[5])
{
  real_T qnrm;
  real_T qinv_idx_0;
  real_T qinv_idx_1;
  real_T qinv_idx_2;
  real_T qinv_idx_3;
  real_T qinter_idx_2;
  real_T qinter_idx_3;
  real_T qinter_idx_1;

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
  printf("h : %f %f %f | %f %f %f | %f %f %f\n",
      y[0], y[0] - x[3], x[3],
      y[1], y[1] - x[4], x[4],
      y[2], y[2] - x[5], x[5]);
  y[3] = x[0];
  y[4] = atan(x[2] / x[0]);
}

/* Function for MATLAB Function: '<Root>/UKF_prediction' */
static void Gen_UKF_RungeKutta(const real_T x[6], real_T dt, const real_T u[6],
  real_T phi, real_T theta, real_T xi[6])
{
  real_T k1[6];
  real_T k2[6];
  real_T k3[6];
  real_T b_x[6];
  real_T y;
  int32_T i;
  real_T b_dx1_idx_0;
  real_T b_dx1_idx_1;
  real_T b_dx1_idx_2;
  real_T c_dx1_idx_0;
  real_T c_dx1_idx_1;
  real_T c_dx1_idx_2;

  /* ----------------Runge Kutta 4----------------- */
  /* description : */
  /* dt is an integration step (tk+1-tk) */
  /* h_2 is define as step/2 */
  /*  */
  /* Non linear function f(x,u), results : X(:,k)=[Xk+1(0) ... Xk+1(2n+1)] */
  /* =RungeKutta(X(:,k),dt,omega,a); */
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
  k1[0] = ((u[3] - 9.81 * sin(theta)) + u[2] * x[1]) - u[1] * x[2];
  k1[1] = ((9.81 * cos(theta) * sin(phi) + u[4]) + u[0] * x[2]) - u[2] * x[0];
  k1[2] = ((9.81 * cos(theta) * cos(phi) + u[5]) + u[1] * x[0]) - u[0] * x[1];
  k1[3] = 0.0;
  k1[4] = 0.0;
  k1[5] = 0.0;
  for (i = 0; i < 6; i++) {
    b_x[i] = k1[i] / 2.0 * dt + x[i];
  }

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
  b_dx1_idx_0 = ((u[3] - 9.81 * sin(theta)) + u[2] * b_x[1]) - u[1] * b_x[2];
  b_dx1_idx_1 = ((9.81 * cos(theta) * sin(phi) + u[4]) + u[0] * b_x[2]) - u[2] *
    b_x[0];
  b_dx1_idx_2 = ((9.81 * cos(theta) * cos(phi) + u[5]) + u[1] * b_x[0]) - u[0] *
    b_x[1];

  /* ==> dot_biais */
  k2[0] = ((u[3] - 9.81 * sin(theta)) + u[2] * b_x[1]) - u[1] * b_x[2];
  k2[1] = ((9.81 * cos(theta) * sin(phi) + u[4]) + u[0] * b_x[2]) - u[2] * b_x[0];
  k2[2] = ((9.81 * cos(theta) * cos(phi) + u[5]) + u[1] * b_x[0]) - u[0] * b_x[1];
  k2[3] = 0.0;
  k2[4] = 0.0;
  k2[5] = 0.0;
  for (i = 0; i < 6; i++) {
    b_x[i] = k2[i] / 2.0 * dt + x[i];
  }

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
  c_dx1_idx_0 = ((u[3] - 9.81 * sin(theta)) + u[2] * b_x[1]) - u[1] * b_x[2];
  c_dx1_idx_1 = ((9.81 * cos(theta) * sin(phi) + u[4]) + u[0] * b_x[2]) - u[2] *
    b_x[0];
  c_dx1_idx_2 = ((9.81 * cos(theta) * cos(phi) + u[5]) + u[1] * b_x[0]) - u[0] *
    b_x[1];

  /* ==> dot_biais */
  k3[0] = ((u[3] - 9.81 * sin(theta)) + u[2] * b_x[1]) - u[1] * b_x[2];
  k3[1] = ((9.81 * cos(theta) * sin(phi) + u[4]) + u[0] * b_x[2]) - u[2] * b_x[0];
  k3[2] = ((9.81 * cos(theta) * cos(phi) + u[5]) + u[1] * b_x[0]) - u[0] * b_x[1];
  k3[3] = 0.0;
  k3[4] = 0.0;
  k3[5] = 0.0;
  for (i = 0; i < 6; i++) {
    b_x[i] = dt * k3[i] + x[i];
  }

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
  y = dt / 6.0;
  xi[0] = (((((u[3] - 9.81 * sin(theta)) + u[2] * x[1]) - u[1] * x[2]) +
            (b_dx1_idx_0 + c_dx1_idx_0) * 2.0) + (((u[3] - 9.81 * sin(theta)) +
             u[2] * b_x[1]) - u[1] * b_x[2])) * y + x[0];
  xi[1] = (((((9.81 * cos(theta) * sin(phi) + u[4]) + u[0] * x[2]) - u[2] * x[0])
            + (b_dx1_idx_1 + c_dx1_idx_1) * 2.0) + (((9.81 * cos(theta) * sin
              (phi) + u[4]) + u[0] * b_x[2]) - u[2] * b_x[0])) * y + x[1];
  xi[2] = (((((9.81 * cos(theta) * cos(phi) + u[5]) + u[1] * x[0]) - u[0] * x[1])
            + (b_dx1_idx_2 + c_dx1_idx_2) * 2.0) + (((9.81 * cos(theta) * cos
              (phi) + u[5]) + u[1] * b_x[0]) - u[0] * b_x[1])) * y + x[2];
  xi[3] = x[3];
  xi[4] = x[4];
  xi[5] = x[5];
}

/* Model step function */
void Gen_UKF_step(void)
{
  real_T lambda;
  int32_T jmax;
  int32_T info;
  int32_T jj;
  real_T ajj;
  int32_T colj;
  int32_T c_j;
  int32_T iy;
  int32_T k;
  int32_T b_ix;
  real_T c;
  int32_T b_iy;
  int32_T e;
  int32_T ia;
  boolean_T exitg2;
  real_T SQ[36];
  real_T Temp[108];
  int32_T d_j;
  real_T SR[25];
  real_T p[5];
  real_T Temp_0[85];
  real_T K[30];
  real_T U[30];
  real_T d[169];
  real_T rtb_x1[6];
  real_T rtb_P1[36];
  real_T rtb_Wm[13];
  real_T rtb_Wc[13];
  real_T rtb_X[78];
  real_T rtb_y[6];
  real_T rtb_Y[78];
  real_T rtb_z[5];
  real_T rtb_Z[65];
  real_T rtb_J[25];
  int32_T i;
  real_T lambda_0[85];
  real_T lambda_1[108];
  real_T SQ_0[36];
  real_T SR_0[25];
  real_T rtb_Z_0[65];

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
  if (!Gen_UKF_DW.P_not_empty) {
    /* '<S3>:1:11' */
    /* '<S3>:1:12' */
    memcpy(&SQ[0], &Gen_UKF_U.P[0], 36U * sizeof(real_T));
    i = 0;
    do {
      b_iy = 0;
      if (i + 1 < 7) {
        i++;
      } else {
        info = 0;
        colj = 0;
        c_j = 1;
        exitg2 = false;
        while ((!exitg2) && (c_j < 7)) {
          jj = (colj + c_j) - 1;
          lambda = 0.0;
          if (!(c_j - 1 < 1)) {
            jmax = colj;
            iy = colj;
            for (k = 1; k < c_j; k++) {
              lambda += SQ[jmax] * SQ[iy];
              jmax++;
              iy++;
            }
          }

          ajj = SQ[jj] - lambda;
          if (ajj > 0.0) {
            ajj = sqrt(ajj);
            SQ[jj] = ajj;
            if (c_j < 6) {
              if (c_j - 1 != 0) {
                b_iy = jj + 6;
                jmax = ((5 - c_j) * 6 + colj) + 7;
                for (iy = colj + 7; iy <= jmax; iy += 6) {
                  b_ix = colj;
                  c = 0.0;
                  e = (iy + c_j) - 2;
                  for (ia = iy; ia <= e; ia++) {
                    c += SQ[ia - 1] * SQ[b_ix];
                    b_ix++;
                  }

                  SQ[b_iy] += -c;
                  b_iy += 6;
                }
              }

              lambda = 1.0 / ajj;
              jmax = ((5 - c_j) * 6 + jj) + 7;
              for (b_iy = jj + 6; b_iy + 1 <= jmax; b_iy += 6) {
                SQ[b_iy] *= lambda;
              }

              colj += 6;
            }

            c_j++;
          } else {
            SQ[jj] = ajj;
            info = c_j;
            exitg2 = true;
          }
        }

        if (info == 0) {
          jmax = 6;
        } else {
          jmax = info - 1;
        }

        for (b_iy = 0; b_iy + 1 <= jmax; b_iy++) {
          for (iy = b_iy + 1; iy + 1 <= jmax; iy++) {
            SQ[iy + 6 * b_iy] = 0.0;
          }
        }

        b_iy = 1;
      }
    } while (b_iy == 0);

    for (i = 0; i < 6; i++) {
      for (b_iy = 0; b_iy < 6; b_iy++) {
        rtb_P1[b_iy + 6 * i] = SQ[6 * b_iy + i];
      }
    }

    Gen_UKF_DW.P_not_empty = true;
  } else {
    /* '<S3>:1:14' */
    memcpy(&rtb_P1[0], &Gen_UKF_DW.Delay_DSTATE[0], 36U * sizeof(real_T));
  }

  if (!Gen_UKF_DW.x_not_empty) {
    /* '<S3>:1:17' */
    /* '<S3>:1:18' */
    for (i = 0; i < 6; i++) {
      rtb_x1[i] = Gen_UKF_U.x0[i];
    }

    Gen_UKF_DW.x_not_empty = true;
  } else {
    /* '<S3>:1:20' */
    for (i = 0; i < 6; i++) {
      rtb_x1[i] = Gen_UKF_DW.Delay1_DSTATE[i];
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
  lambda = Gen_UKF_U.alpha * Gen_UKF_U.alpha * (6.0 + Gen_UKF_U.ki) - 6.0;

  /* '<S3>:1:44' */
  /* ----------------Calcul des ponderations Wm et Wc-------------------- */
  /* '<S3>:1:48' */
  ajj = 0.5 / (6.0 + lambda);
  rtb_Wm[0] = lambda / (6.0 + lambda);
  for (i = 0; i < 12; i++) {
    rtb_Wm[i + 1] = ajj;
  }

  /* '<S3>:1:49' */
  memcpy(&rtb_Wc[0], &rtb_Wm[0], 13U * sizeof(real_T));

  /* '<S3>:1:50' */
  rtb_Wc[0] = ((1.0 - Gen_UKF_U.alpha * Gen_UKF_U.alpha) + Gen_UKF_U.beta) +
    rtb_Wm[0];

  /* '<S3>:1:51' */
  /* '<S3>:1:52' */
  /* '<S3>:1:53' */
  lambda = sqrt(6.0 + lambda);

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
  for (i = 0; i < 36; i++) {
    SQ[i] = lambda * rtb_P1[i];
  }

  /* dimension modification on state  */
  /* '<S5>:1:18' */
  for (i = 0; i < 6; i++) {
    for (b_iy = 0; b_iy < 6; b_iy++) {
      rtb_P1[b_iy + 6 * i] = rtb_x1[b_iy];
    }
  }

  /* concatenation : X=[x Y+A Y-A] */
  /* '<S5>:1:21' */
  for (i = 0; i < 6; i++) {
    rtb_X[i] = rtb_x1[i];
  }

  for (i = 0; i < 36; i++) {
    rtb_X[i + 6] = rtb_P1[i] + SQ[i];
  }

  for (i = 0; i < 36; i++) {
    rtb_X[i + 42] = rtb_P1[i] - SQ[i];
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
  /* '<S2>:1:25' */
  /* '<S2>:1:26' */
  memset(&rtb_Y[0], 0, 78U * sizeof(real_T));

    for (i = 0; i < 6; i++) {
      printf("x%d = %f\n",i, rtb_X[i]);
    }
  /* '<S2>:1:27' */
  for (k = 0; k < 13; k++) {
    /* '<S2>:1:27' */
    /* '<S2>:1:28' */
    Gen_UKF_RungeKutta(*(real_T (*)[6])&rtb_X[6 * k], Gen_UKF_U.dt,
                       Gen_UKF_U.omegaa, Gen_UKF_U.phi, Gen_UKF_U.theta,
                       *(real_T (*)[6])&rtb_Y[6 * k]);

    /* non linear function */
    /* y=y+Wm(k)*Y(:,k);                    %predicted vector on the state */
    /* '<S2>:1:30' */
    for (i = 0; i < 6; i++) {
      rtb_y[i] = rtb_Y[i];
    }

    /* '<S2>:1:27' */
  }
    for (i = 0; i < 6; i++) {
      printf("xp%d = %f\n",i, rtb_y[i]);
    }

  /* '<S2>:1:33' */
  memcpy(&SQ[0], &Gen_UKF_U.Q[0], 36U * sizeof(real_T));
  Gen_UKF_sqrt(SQ);

  /* square-root of R */
  /* '<S2>:1:35' */
  lambda = sqrt(rtb_Wm[1]);

  /* '<S2>:1:37' */
  /* 33*11=>[11*22 11*11] */
  for (i = 0; i < 6; i++) {
    for (b_iy = 0; b_iy < 12; b_iy++) {
      lambda_1[b_iy + 18 * i] = (rtb_Y[(1 + b_iy) * 6 + i] - rtb_y[i]) * lambda;
    }
  }

  for (i = 0; i < 6; i++) {
    for (b_iy = 0; b_iy < 6; b_iy++) {
      lambda_1[(b_iy + 18 * i) + 12] = SQ[6 * b_iy + i];
    }
  }

  Gen_UKF_qr_m(lambda_1, Temp, SQ);

  /* '<S2>:1:41' */
  for (i = 0; i < 6; i++) {
    rtb_x1[i] = rtb_Y[i] - rtb_y[i];
  }

  if (rtb_Wc[0] < 0.0) {
    /* '<S2>:1:43' */
    /* '<S2>:1:44' */
    lambda = pow(-rtb_Wc[0], 0.25);
    ajj = pow(-rtb_Wc[0], 0.25);
    for (i = 0; i < 6; i++) {
      for (b_iy = 0; b_iy < 6; b_iy++) {
        SQ_0[i + 6 * b_iy] = 0.0;
        for (info = 0; info < 6; info++) {
          SQ_0[i + 6 * b_iy] += SQ[6 * i + info] * SQ[6 * b_iy + info];
        }
      }
    }

    for (i = 0; i < 6; i++) {
      for (b_iy = 0; b_iy < 6; b_iy++) {
        SQ[i + 6 * b_iy] = lambda * rtb_x1[i] * (ajj * rtb_x1[b_iy]);
      }
    }

    for (i = 0; i < 6; i++) {
      for (b_iy = 0; b_iy < 6; b_iy++) {
        rtb_P1[b_iy + 6 * i] = SQ_0[6 * i + b_iy] - SQ[6 * i + b_iy];
      }
    }

    d_j = 0;
    do {
      b_iy = 0;
      if (d_j + 1 < 7) {
        d_j++;
      } else {
        info = 0;
        colj = 0;
        i = 1;
        exitg2 = false;
        while ((!exitg2) && (i < 7)) {
          jj = (colj + i) - 1;
          lambda = 0.0;
          if (!(i - 1 < 1)) {
            jmax = colj;
            iy = colj;
            for (b_iy = 1; b_iy < i; b_iy++) {
              lambda += rtb_P1[jmax] * rtb_P1[iy];
              jmax++;
              iy++;
            }
          }

          lambda = rtb_P1[jj] - lambda;
          if (lambda > 0.0) {
            lambda = sqrt(lambda);
            rtb_P1[jj] = lambda;
            if (i < 6) {
              if (i - 1 != 0) {
                b_iy = jj + 6;
                jmax = ((5 - i) * 6 + colj) + 7;
                for (iy = colj + 7; iy <= jmax; iy += 6) {
                  b_ix = colj;
                  ajj = 0.0;
                  e = (iy + i) - 2;
                  for (ia = iy; ia <= e; ia++) {
                    ajj += rtb_P1[ia - 1] * rtb_P1[b_ix];
                    b_ix++;
                  }

                  rtb_P1[b_iy] += -ajj;
                  b_iy += 6;
                }
              }

              lambda = 1.0 / lambda;
              jmax = ((5 - i) * 6 + jj) + 7;
              for (b_iy = jj + 6; b_iy + 1 <= jmax; b_iy += 6) {
                rtb_P1[b_iy] *= lambda;
              }

              colj += 6;
            }

            i++;
          } else {
            rtb_P1[jj] = lambda;
            info = i;
            exitg2 = true;
          }
        }

        if (info == 0) {
          b_iy = 6;
        } else {
          b_iy = info - 1;
        }

        for (i = 0; i + 1 <= b_iy; i++) {
          for (jmax = i + 1; jmax + 1 <= b_iy; jmax++) {
            rtb_P1[jmax + 6 * i] = 0.0;
          }
        }

        b_iy = 1;
      }
    } while (b_iy == 0);
  } else {
    /* '<S2>:1:46' */
    c = pow(rtb_Wc[0], 0.25);
    lambda = pow(rtb_Wc[0], 0.25);
    for (i = 0; i < 6; i++) {
      for (b_iy = 0; b_iy < 6; b_iy++) {
        SQ_0[i + 6 * b_iy] = 0.0;
        for (info = 0; info < 6; info++) {
          SQ_0[i + 6 * b_iy] += SQ[6 * i + info] * SQ[6 * b_iy + info];
        }
      }
    }

    for (i = 0; i < 6; i++) {
      for (b_iy = 0; b_iy < 6; b_iy++) {
        SQ[i + 6 * b_iy] = c * rtb_x1[i] * (lambda * rtb_x1[b_iy]);
      }
    }

    for (i = 0; i < 6; i++) {
      for (b_iy = 0; b_iy < 6; b_iy++) {
        rtb_P1[b_iy + 6 * i] = SQ_0[6 * i + b_iy] + SQ[6 * i + b_iy];
      }
    }

    i = 0;
    do {
      b_iy = 0;
      if (i + 1 < 7) {
        i++;
      } else {
        info = 0;
        colj = 0;
        c_j = 1;
        exitg2 = false;
        while ((!exitg2) && (c_j < 7)) {
          jj = (colj + c_j) - 1;
          lambda = 0.0;
          if (!(c_j - 1 < 1)) {
            jmax = colj;
            iy = colj;
            for (b_iy = 1; b_iy < c_j; b_iy++) {
              lambda += rtb_P1[jmax] * rtb_P1[iy];
              jmax++;
              iy++;
            }
          }

          ajj = rtb_P1[jj] - lambda;
          if (ajj > 0.0) {
            ajj = sqrt(ajj);
            rtb_P1[jj] = ajj;
            if (c_j < 6) {
              if (c_j - 1 != 0) {
                b_iy = jj + 6;
                jmax = ((5 - c_j) * 6 + colj) + 7;
                for (iy = colj + 7; iy <= jmax; iy += 6) {
                  b_ix = colj;
                  lambda = 0.0;
                  e = (iy + c_j) - 2;
                  for (ia = iy; ia <= e; ia++) {
                    lambda += rtb_P1[ia - 1] * rtb_P1[b_ix];
                    b_ix++;
                  }

                  rtb_P1[b_iy] += -lambda;
                  b_iy += 6;
                }
              }

              lambda = 1.0 / ajj;
              jmax = ((5 - c_j) * 6 + jj) + 7;
              for (b_iy = jj + 6; b_iy + 1 <= jmax; b_iy += 6) {
                rtb_P1[b_iy] *= lambda;
              }

              colj += 6;
            }

            c_j++;
          } else {
            rtb_P1[jj] = ajj;
            info = c_j;
            exitg2 = true;
          }
        }

        if (info == 0) {
          jmax = 6;
        } else {
          jmax = info - 1;
        }

        for (b_iy = 0; b_iy + 1 <= jmax; b_iy++) {
          for (iy = b_iy + 1; iy + 1 <= jmax; iy++) {
            rtb_P1[iy + 6 * b_iy] = 0.0;
          }
        }

        b_iy = 1;
      }
    } while (b_iy == 0);
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
  /* '<S1>:1:27' */
  /* Zeros vector for predicted Sigmas_measures */
  /* '<S1>:1:30' */
  memset(&rtb_Z[0], 0, 65U * sizeof(real_T));

    for (i = 0; i < 6; i++) {
      printf("y%d = %f\n", i, rtb_Y[i]);
    }
  /* "for" loop for "measures predicted" and "sigmas_measures" predicted */
  /* '<S1>:1:33' */
  for (k = 0; k < 13; k++) {
    /* '<S1>:1:33' */
    /* non linear function h(x), results : Z(:,k)=[zk+1(0) ... zk+1(2n+1)] */
    /* '<S1>:1:36' */
    Gen_UKF_h(*(real_T (*)[6])&rtb_Y[6 * k], Gen_UKF_U.q, *(real_T (*)[5])&
              rtb_Z[5 * k]);

    /* Predicted measurement */
    /*  z=z+Wm(k)*Z(:,k);  */
    /* '<S1>:1:40' */
    for (i = 0; i < 5; i++) {
      rtb_z[i] = rtb_Z[i];
    }

    /* '<S1>:1:33' */
  }
    for (i = 0; i < 5; i++) {
      printf("z%d = %f\n", i, rtb_z[i]);
    }

  /* Square-root of R */
  /* '<S1>:1:44' */
  for (i = 0; i < 25; i++) {
    SR[i] = sqrt(Gen_UKF_U.R[i]);
  }

  /* Subtracting of "predicted Sigma measures" et "predicted measurement" with ponderation "Wc" */
  /* '<S1>:1:47' */
  lambda = sqrt(rtb_Wm[1]);

  /* Concatenation with the square root of the matrix "R" */
  /* '<S1>:1:50' */
  /* factorization qr, result use in "J" and rest in "Temp"  */
  /* Sz,k+1 = qr{[sqrt(Wc(1)) * [Zk+1(:,2:2n+1)-zk+1]  sqrt(R)] */
  for (i = 0; i < 5; i++) {
    for (b_iy = 0; b_iy < 12; b_iy++) {
      lambda_0[b_iy + 17 * i] = (rtb_Z[(1 + b_iy) * 5 + i] - rtb_z[i]) * lambda;
    }
  }

  for (i = 0; i < 5; i++) {
    for (b_iy = 0; b_iy < 5; b_iy++) {
      lambda_0[(b_iy + 17 * i) + 12] = SR[5 * b_iy + i];
    }
  }

  Gen_UKF_qr(lambda_0, Temp_0, SR);

  /* Subtracting the first column "zk+1(0)" with all the measurements zk+1 predicted */
  /* '<S1>:1:57' */
  for (i = 0; i < 5; i++) {
    p[i] = rtb_Z[i] - rtb_z[i];
  }

  /* Test on Wc(1) for cholesky factorization : Cholupdate{Sz,k+1,p,Wc(0)}  */
  if (rtb_Wc[0] < 0.0) {
    /* '<S1>:1:60' */
    /* '<S1>:1:61' */
    lambda = pow(-rtb_Wc[0], 0.25);
    ajj = pow(-rtb_Wc[0], 0.25);
    for (i = 0; i < 5; i++) {
      for (b_iy = 0; b_iy < 5; b_iy++) {
        SR_0[i + 5 * b_iy] = 0.0;
        for (info = 0; info < 5; info++) {
          SR_0[i + 5 * b_iy] += SR[5 * i + info] * SR[5 * b_iy + info];
        }
      }
    }

    for (i = 0; i < 5; i++) {
      for (b_iy = 0; b_iy < 5; b_iy++) {
        SR[i + 5 * b_iy] = lambda * p[i] * (ajj * p[b_iy]);
      }
    }

    for (i = 0; i < 5; i++) {
      for (b_iy = 0; b_iy < 5; b_iy++) {
        rtb_J[b_iy + 5 * i] = SR_0[5 * i + b_iy] - SR[5 * i + b_iy];
      }
    }

    d_j = 0;
    do {
      b_iy = 0;
      if (d_j + 1 < 6) {
        d_j++;
      } else {
        info = 0;
        colj = 0;
        i = 1;
        exitg2 = false;
        while ((!exitg2) && (i < 6)) {
          jj = (colj + i) - 1;
          lambda = 0.0;
          if (!(i - 1 < 1)) {
            jmax = colj;
            iy = colj;
            for (b_iy = 1; b_iy < i; b_iy++) {
              lambda += rtb_J[jmax] * rtb_J[iy];
              jmax++;
              iy++;
            }
          }

          lambda = rtb_J[jj] - lambda;
          if (lambda > 0.0) {
            lambda = sqrt(lambda);
            rtb_J[jj] = lambda;
            if (i < 5) {
              if (i - 1 != 0) {
                b_iy = jj + 5;
                jmax = ((4 - i) * 5 + colj) + 6;
                for (iy = colj + 6; iy <= jmax; iy += 5) {
                  b_ix = colj;
                  ajj = 0.0;
                  e = (iy + i) - 2;
                  for (ia = iy; ia <= e; ia++) {
                    ajj += rtb_J[ia - 1] * rtb_J[b_ix];
                    b_ix++;
                  }

                  rtb_J[b_iy] += -ajj;
                  b_iy += 5;
                }
              }

              lambda = 1.0 / lambda;
              jmax = ((4 - i) * 5 + jj) + 6;
              for (b_iy = jj + 5; b_iy + 1 <= jmax; b_iy += 5) {
                rtb_J[b_iy] *= lambda;
              }

              colj += 5;
            }

            i++;
          } else {
            rtb_J[jj] = lambda;
            info = i;
            exitg2 = true;
          }
        }

        if (info == 0) {
          b_iy = 5;
        } else {
          b_iy = info - 1;
        }

        for (i = 0; i + 1 <= b_iy; i++) {
          for (jmax = i + 1; jmax + 1 <= b_iy; jmax++) {
            rtb_J[jmax + 5 * i] = 0.0;
          }
        }

        b_iy = 1;
      }
    } while (b_iy == 0);
  } else {
    /* '<S1>:1:63' */
    c = pow(rtb_Wc[0], 0.25);
    lambda = pow(rtb_Wc[0], 0.25);
    for (i = 0; i < 5; i++) {
      for (b_iy = 0; b_iy < 5; b_iy++) {
        SR_0[i + 5 * b_iy] = 0.0;
        for (info = 0; info < 5; info++) {
          SR_0[i + 5 * b_iy] += SR[5 * i + info] * SR[5 * b_iy + info];
        }
      }
    }

    for (i = 0; i < 5; i++) {
      for (b_iy = 0; b_iy < 5; b_iy++) {
        SR[i + 5 * b_iy] = c * p[i] * (lambda * p[b_iy]);
      }
    }

    for (i = 0; i < 5; i++) {
      for (b_iy = 0; b_iy < 5; b_iy++) {
        rtb_J[b_iy + 5 * i] = SR_0[5 * i + b_iy] + SR[5 * i + b_iy];
      }
    }

    i = 0;
    do {
      b_iy = 0;
      if (i + 1 < 6) {
        i++;
      } else {
        info = 0;
        colj = 0;
        c_j = 1;
        exitg2 = false;
        while ((!exitg2) && (c_j < 6)) {
          jj = (colj + c_j) - 1;
          lambda = 0.0;
          if (!(c_j - 1 < 1)) {
            jmax = colj;
            iy = colj;
            for (b_iy = 1; b_iy < c_j; b_iy++) {
              lambda += rtb_J[jmax] * rtb_J[iy];
              jmax++;
              iy++;
            }
          }

          ajj = rtb_J[jj] - lambda;
          if (ajj > 0.0) {
            ajj = sqrt(ajj);
            rtb_J[jj] = ajj;
            if (c_j < 5) {
              if (c_j - 1 != 0) {
                b_iy = jj + 5;
                jmax = ((4 - c_j) * 5 + colj) + 6;
                for (iy = colj + 6; iy <= jmax; iy += 5) {
                  b_ix = colj;
                  lambda = 0.0;
                  e = (iy + c_j) - 2;
                  for (ia = iy; ia <= e; ia++) {
                    lambda += rtb_J[ia - 1] * rtb_J[b_ix];
                    b_ix++;
                  }

                  rtb_J[b_iy] += -lambda;
                  b_iy += 5;
                }
              }

              lambda = 1.0 / ajj;
              jmax = ((4 - c_j) * 5 + jj) + 6;
              for (b_iy = jj + 5; b_iy + 1 <= jmax; b_iy += 5) {
                rtb_J[b_iy] *= lambda;
              }

              colj += 5;
            }

            c_j++;
          } else {
            rtb_J[jj] = ajj;
            info = c_j;
            exitg2 = true;
          }
        }

        if (info == 0) {
          jmax = 5;
        } else {
          jmax = info - 1;
        }

        for (b_iy = 0; b_iy + 1 <= jmax; b_iy++) {
          for (iy = b_iy + 1; iy + 1 <= jmax; iy++) {
            rtb_J[iy + 5 * b_iy] = 0.0;
          }
        }

        b_iy = 1;
      }
    } while (b_iy == 0);
  }

  /* Transposition to have a lower triangular matrix */
  /* '<S1>:1:67' */
  for (i = 0; i < 5; i++) {
    for (b_iy = 0; b_iy < 5; b_iy++) {
      SR[b_iy + 5 * i] = rtb_J[5 * b_iy + i];
    }
  }

  for (i = 0; i < 5; i++) {
    for (b_iy = 0; b_iy < 5; b_iy++) {
      rtb_J[b_iy + 5 * i] = SR[5 * i + b_iy];
    }
  }

  /* End of MATLAB Function: '<Root>/UKF_correction' */

  /* MATLAB Function: '<Root>/main' */
  /* T=1; */
  /* A=1; */
  /* a=1; */
  memcpy(&SQ[0], &rtb_P1[0], 36U * sizeof(real_T));

  /* MATLAB Function 'main': '<S4>:1' */
  /* '<S4>:1:5' */
  memset(&d[0], 0, 169U * sizeof(real_T));
  for (i = 0; i < 13; i++) {
    d[i + 13 * i] = rtb_Wc[i];
  }

  /* Kalman gain : (Pxz/Sz')/Sz */
  /* Caution "/" is least squares root  for "Ax = b" */
  /* '<S4>:1:9' */
  for (i = 0; i < 13; i++) {
    for (b_iy = 0; b_iy < 6; b_iy++) {
      rtb_X[b_iy + 6 * i] = rtb_Y[6 * i + b_iy] - rtb_y[b_iy];
    }
  }

  for (i = 0; i < 6; i++) {
    for (b_iy = 0; b_iy < 13; b_iy++) {
      rtb_Y[i + 6 * b_iy] = 0.0;
      for (info = 0; info < 13; info++) {
        rtb_Y[i + 6 * b_iy] += rtb_X[6 * info + i] * d[13 * b_iy + info];
      }
    }
  }

  for (i = 0; i < 5; i++) {
    for (b_iy = 0; b_iy < 13; b_iy++) {
      rtb_Z_0[b_iy + 13 * i] = rtb_Z[5 * b_iy + i] - rtb_z[i];
    }
  }

  for (i = 0; i < 6; i++) {
    for (b_iy = 0; b_iy < 5; b_iy++) {
      K[i + 6 * b_iy] = 0.0;
      for (info = 0; info < 13; info++) {
        K[i + 6 * b_iy] += rtb_Y[6 * info + i] * rtb_Z_0[13 * b_iy + info];
      }
    }
  }

  for (i = 0; i < 5; i++) {
    for (b_iy = 0; b_iy < 5; b_iy++) {
      SR[b_iy + 5 * i] = rtb_J[5 * b_iy + i];
    }
  }

  Gen_UKF_mrdivide(K, SR);
  Gen_UKF_mrdivide(K, rtb_J);

  /* Predicted state with Kalman gain "K" : x = xpred + K * (zk - z) */
  /* '<S4>:1:12' */
  /* Quaternion normalisation */
  /* x(1:4)=normalQ(x(1:4)'); */
  /* Cholesky factorization : U = K * Sz    */
  /* '<S4>:1:18' */
  for (i = 0; i < 6; i++) {
    printf("k%d = ",i);
    for (b_iy = 0; b_iy < 5; b_iy++) {
      printf("%f ",K[6*b_iy + i]);
      U[i + 6 * b_iy] = 0.0;
      for (info = 0; info < 5; info++) {
        U[i + 6 * b_iy] += K[6 * info + i] * rtb_J[5 * b_iy + info];
      }
    }
    printf("\n");
  }

  /* "for" loop for cholesky factorization */
  /* '<S4>:1:21' */
  for (k = 0; k < 5; k++) {
    /* '<S4>:1:21' */
    /* '<S4>:1:22' */
    for (i = 0; i < 6; i++) {
      for (b_iy = 0; b_iy < 6; b_iy++) {
        SQ_0[i + 6 * b_iy] = 0.0;
        for (info = 0; info < 6; info++) {
          SQ_0[i + 6 * b_iy] += SQ[6 * i + info] * SQ[6 * b_iy + info];
        }
      }
    }

    for (i = 0; i < 6; i++) {
      for (b_iy = 0; b_iy < 6; b_iy++) {
        SQ[i + 6 * b_iy] = U[6 * k + i] * U[6 * k + b_iy];
      }
    }

    for (i = 0; i < 6; i++) {
      for (b_iy = 0; b_iy < 6; b_iy++) {
        rtb_P1[b_iy + 6 * i] = SQ_0[6 * i + b_iy] - SQ[6 * i + b_iy];
      }
    }

    c_j = 0;
    do {
      b_iy = 0;
      if (c_j + 1 < 7) {
        c_j++;
      } else {
        info = 0;
        colj = 0;
        i = 1;
        exitg2 = false;
        while ((!exitg2) && (i < 7)) {
          jj = (colj + i) - 1;
          lambda = 0.0;
          if (!(i - 1 < 1)) {
            jmax = colj;
            iy = colj;
            for (b_iy = 1; b_iy < i; b_iy++) {
              lambda += rtb_P1[jmax] * rtb_P1[iy];
              jmax++;
              iy++;
            }
          }

          ajj = rtb_P1[jj] - lambda;
          if (ajj > 0.0) {
            ajj = sqrt(ajj);
            rtb_P1[jj] = ajj;
            if (i < 6) {
              if (i - 1 != 0) {
                b_iy = jj + 6;
                jmax = ((5 - i) * 6 + colj) + 7;
                for (iy = colj + 7; iy <= jmax; iy += 6) {
                  b_ix = colj;
                  c = 0.0;
                  e = (iy + i) - 2;
                  for (ia = iy; ia <= e; ia++) {
                    c += rtb_P1[ia - 1] * rtb_P1[b_ix];
                    b_ix++;
                  }

                  rtb_P1[b_iy] += -c;
                  b_iy += 6;
                }
              }

              lambda = 1.0 / ajj;
              jmax = ((5 - i) * 6 + jj) + 7;
              for (b_iy = jj + 6; b_iy + 1 <= jmax; b_iy += 6) {
                rtb_P1[b_iy] *= lambda;
              }

              colj += 6;
            }

            i++;
          } else {
            rtb_P1[jj] = ajj;
            info = i;
            exitg2 = true;
          }
        }

        if (info == 0) {
          jmax = 6;
        } else {
          jmax = info - 1;
        }

        for (d_j = 0; d_j + 1 <= jmax; d_j++) {
          for (iy = d_j + 1; iy + 1 <= jmax; iy++) {
            rtb_P1[iy + 6 * d_j] = 0.0;
          }
        }

        b_iy = 1;
      }
    } while (b_iy == 0);

    memcpy(&SQ[0], &rtb_P1[0], 36U * sizeof(real_T));

    /* '<S4>:1:21' */
  }

  /* "Sk" in "P" and Transpose of "P" */
  /* '<S4>:1:27' */
  /* '<S4>:1:29' */
  for (i = 0; i < 6; i++) {
    for (b_iy = 0; b_iy < 6; b_iy++) {
      rtb_P1[i + 6 * b_iy] = 0.0;
      for (info = 0; info < 6; info++) {
        rtb_P1[i + 6 * b_iy] += SQ[6 * info + i] * SQ[6 * info + b_iy];
      }
    }
  }

  for (b_iy = 0; b_iy < 6; b_iy++) {
    /* Outport: '<Root>/sqrtP' incorporates:
     *  MATLAB Function: '<Root>/main'
     */
    Gen_UKF_Y.sqrtP[b_iy] = sqrt(rtb_P1[b_iy * 7]);
  }

  /* MATLAB Function: '<Root>/main' incorporates:
   *  Inport: '<Root>/zk'
   */
  for (i = 0; i < 6; i++) {
    for (b_iy = 0; b_iy < 6; b_iy++) {
      rtb_P1[b_iy + 6 * i] = SQ[6 * b_iy + i];
    }
  }

  printf("res ");
  for (i = 0; i < 5; i++) {
    p[i] = Gen_UKF_U.zk[i] - rtb_z[i];
    printf("%f ",p[i]);
  }
  printf("\nlbd ");

  for (i = 0; i < 6; i++) {
    lambda = 0.0;
    for (b_iy = 0; b_iy < 5; b_iy++) {
      lambda += K[6 * b_iy + i] * p[b_iy];
    }
    printf("%f ",lambda);

    rtb_x1[i] = rtb_y[i] + lambda;
  }
  printf("\nout ");

  /* Outport: '<Root>/xout' */
  for (i = 0; i < 6; i++) {
    Gen_UKF_Y.xout[i] = rtb_x1[i];
    printf("%f ",rtb_x1[i]);
  }
  printf("\n");

  for (i = 0; i < 36; i++) {
    /* Outport: '<Root>/Pout' */
    Gen_UKF_Y.Pout[i] = rtb_P1[i];

    /* Update for UnitDelay: '<Root>/ Delay' */
    Gen_UKF_DW.Delay_DSTATE[i] = rtb_P1[i];
  }

  /* End of Outport: '<Root>/xout' */

  /* Update for UnitDelay: '<Root>/ Delay1' */
  for (i = 0; i < 6; i++) {
    Gen_UKF_DW.Delay1_DSTATE[i] = rtb_x1[i];
  }

  /* End of Update for UnitDelay: '<Root>/ Delay1' */
}

/* Model initialize function */
void Gen_UKF_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(Gen_UKF_M, (NULL));

  /* states (dwork) */
  (void) memset((void *)&Gen_UKF_DW, 0,
                sizeof(DW_Gen_UKF_T));

  /* external inputs */
  (void) memset((void *)&Gen_UKF_U, 0,
                sizeof(ExtU_Gen_UKF_T));

  /* external outputs */
  (void) memset((void *)&Gen_UKF_Y, 0,
                sizeof(ExtY_Gen_UKF_T));

  {
    int32_T i;

    /* InitializeConditions for UnitDelay: '<Root>/ Delay' */
    for (i = 0; i < 36; i++) {
      Gen_UKF_DW.Delay_DSTATE[i] = Gen_UKF_P.Delay_InitialCondition;
    }

    /* End of InitializeConditions for UnitDelay: '<Root>/ Delay' */

    /* InitializeConditions for UnitDelay: '<Root>/ Delay1' */
    for (i = 0; i < 6; i++) {
      Gen_UKF_DW.Delay1_DSTATE[i] = Gen_UKF_P.Delay1_InitialCondition;
    }

    /* End of InitializeConditions for UnitDelay: '<Root>/ Delay1' */

    /* InitializeConditions for MATLAB Function: '<Root>/initialization' */
    Gen_UKF_DW.x_not_empty = false;
    Gen_UKF_DW.P_not_empty = false;
  }
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
