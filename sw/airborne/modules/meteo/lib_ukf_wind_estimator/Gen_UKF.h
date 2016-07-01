/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Gen_UKF.h
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

#ifndef RTW_HEADER_Gen_UKF_h_
#define RTW_HEADER_Gen_UKF_h_
#include <math.h>
#include <string.h>
#ifndef Gen_UKF_COMMON_INCLUDES_
# define Gen_UKF_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* Gen_UKF_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */

/* Block signals and states (auto storage) for system '<Root>' */
typedef struct {
  real32_T Delay_DSTATE[36];           /* '<Root>/ Delay' */
  real32_T Delay1_DSTATE[6];           /* '<Root>/ Delay1' */
  boolean_T x_not_empty;               /* '<Root>/initialization' */
  boolean_T P_not_empty;               /* '<Root>/initialization' */
} DW;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real32_T omegaa[6];                  /* '<Root>/[omega a]' */
  real32_T zk[5];                      /* '<Root>/zk' */
  real32_T x0[6];                      /* '<Root>/x0' */
  real32_T P_i[36];                    /* '<Root>/P' */
  real32_T alpha;                      /* '<Root>/alpha' */
  real32_T ki;                         /* '<Root>/ki' */
  real32_T beta;                       /* '<Root>/beta' */
  real32_T Q[36];                      /* '<Root>/Q' */
  real32_T R[25];                      /* '<Root>/R' */
  real32_T dt;                         /* '<Root>/dt' */
  real32_T phi;                        /* '<Root>/phi' */
  real32_T theta;                      /* '<Root>/theta' */
  real32_T q[4];                       /* '<Root>/q' */
} ExtU;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real32_T sqrtP[6];                   /* '<Root>/sqrtP' */
  real32_T xout[6];                    /* '<Root>/xout' */
  real32_T Pout[36];                   /* '<Root>/Pout' */
} ExtY;

/* Block signals and states (auto storage) */
extern DW rtDW;

/* External inputs (root inport signals with auto storage) */
extern ExtU rtU;

/* External outputs (root outports fed by signals with auto storage) */
extern ExtY rtY;

/* Model entry point functions */
extern void Gen_UKF_initialize(void);
extern void Gen_UKF_step(void);

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'Gen_UKF'
 * '<S1>'   : 'Gen_UKF/UKF_correction'
 * '<S2>'   : 'Gen_UKF/UKF_prediction'
 * '<S3>'   : 'Gen_UKF/initialization'
 * '<S4>'   : 'Gen_UKF/main'
 * '<S5>'   : 'Gen_UKF/sigmas'
 */
#endif                                 /* RTW_HEADER_Gen_UKF_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
