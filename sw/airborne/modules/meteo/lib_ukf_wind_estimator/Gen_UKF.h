/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Gen_UKF.h
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

#ifndef RTW_HEADER_Gen_UKF_h_
#define RTW_HEADER_Gen_UKF_h_
#include <math.h>
#include <string.h>
#include <stddef.h>
#ifndef Gen_UKF_COMMON_INCLUDES_
# define Gen_UKF_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* Gen_UKF_COMMON_INCLUDES_ */

#include "Gen_UKF_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  real_T Delay_DSTATE[36];             /* '<Root>/ Delay' */
  real_T Delay1_DSTATE[6];             /* '<Root>/ Delay1' */
  boolean_T x_not_empty;               /* '<Root>/initialization' */
  boolean_T P_not_empty;               /* '<Root>/initialization' */
} DW_Gen_UKF_T;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real_T omegaa[6];                    /* '<Root>/[omega a]' */
  real_T zk[5];                        /* '<Root>/zk' */
  real_T x0[6];                        /* '<Root>/x0' */
  real_T P[36];                        /* '<Root>/P' */
  real_T alpha;                        /* '<Root>/alpha' */
  real_T ki;                           /* '<Root>/ki' */
  real_T beta;                         /* '<Root>/beta' */
  real_T Q[36];                        /* '<Root>/Q' */
  real_T R[25];                        /* '<Root>/R' */
  real_T dt;                           /* '<Root>/dt' */
  real_T phi;                          /* '<Root>/phi' */
  real_T theta;                        /* '<Root>/theta' */
  real_T q[4];                         /* '<Root>/q' */
} ExtU_Gen_UKF_T;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real_T sqrtP[6];                     /* '<Root>/sqrtP' */
  real_T xout[6];                      /* '<Root>/xout' */
  real_T Pout[36];                     /* '<Root>/Pout' */
} ExtY_Gen_UKF_T;

/* Parameters (auto storage) */
struct P_Gen_UKF_T_ {
  real_T Delay_InitialCondition;       /* Expression: 0
                                        * Referenced by: '<Root>/ Delay'
                                        */
  real_T Delay1_InitialCondition;      /* Expression: 0
                                        * Referenced by: '<Root>/ Delay1'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_Gen_UKF_T {
  const char_T * volatile errorStatus;
};

/* Block parameters (auto storage) */
extern P_Gen_UKF_T Gen_UKF_P;

/* Block states (auto storage) */
extern DW_Gen_UKF_T Gen_UKF_DW;

/* External inputs (root inport signals with auto storage) */
extern ExtU_Gen_UKF_T Gen_UKF_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExtY_Gen_UKF_T Gen_UKF_Y;

/* Model entry point functions */
extern void Gen_UKF_initialize(void);
extern void Gen_UKF_step(void);

/* Real-time Model object */
extern RT_MODEL_Gen_UKF_T *const Gen_UKF_M;

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
