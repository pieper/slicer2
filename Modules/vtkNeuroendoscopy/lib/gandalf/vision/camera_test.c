/**
 * File:          $RCSfile: camera_test.c,v $
 * Module:        Camera module test program
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:16 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <gandalf/TestFramework/cUnit.h>
#include <gandalf/vision/camera_test.h>

#include <gandalf/common/numerics.h>
#include <gandalf/vision/camera.h>
#include <gandalf/vision/cameraf.h>
#include <gandalf/vision/camera_linear.h>
#include <gandalf/vision/cameraf_linear.h>
#include <gandalf/vision/camera_affine.h>
#include <gandalf/vision/cameraf_affine.h>
#include <gandalf/vision/camera_radial_dist1.h>
#include <gandalf/vision/camera_radial_dist2.h>
#include <gandalf/vision/camera_radial_dist3.h>
#include <gandalf/vision/cameraf_radial_dist1.h>
#include <gandalf/vision/cameraf_radial_dist2.h>
#include <gandalf/vision/cameraf_radial_dist3.h>
#include <gandalf/vision/camera_stereographic.h>
#include <gandalf/vision/cameraf_stereographic.h>
#include <gandalf/vision/camera_equidistant.h>
#include <gandalf/vision/cameraf_equidistant.h>
#include <gandalf/vision/camera_sine_law.h>
#include <gandalf/vision/cameraf_sine_law.h>
#include <gandalf/vision/camera_equi_solid_angle.h>
#include <gandalf/vision/cameraf_equi_solid_angle.h>
#include <gandalf/vision/camera_xy_dist4.h>
#include <gandalf/vision/cameraf_xy_dist4.h>
#include <gandalf/vision/camera_radial_dist1_inv.h>
#include <gandalf/vision/cameraf_radial_dist1_inv.h>
#include <gandalf/vision/camera_cubic_Bspline.h>
#include <gandalf/vision/camera_cubic_Bspline_inv.h>

/* linear camera parameters */
#define CAMERA_FX 130.0
#define CAMERA_FY 150.0
#define CAMERA_X0 110.0
#define CAMERA_Y0 120.0
#define CAMERA_ZH 100.0

/* affine distortion parameters */
#define CAMERA_AFFINE_SKEW   0.9
#define CAMERA_AFFINE_KYX   -0.6

/* radial distortion parameters */
#define CAMERA_K1   0.2
#define CAMERA_K2  -0.1
#define CAMERA_K3   0.15

/* 3D Equalizer-style parameters */
#define CAMERA_CXX  0.2
#define CAMERA_CXY -0.3
#define CAMERA_CYX  0.4
#define CAMERA_CYY -0.1

/* cubic B-spline parameters */
#define CAMERA_SKEW 0.0
#define CAMERA_KYX 0.0
#define CAMERA_KZX 0.0
#define CAMERA_KZY 0.0
static Gan_Vector2 camera_weight4[1 + (1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)][1 + (1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)];
static Gan_CubicBSplineSupport cbspline_support;
             
static Gan_Bool setup_test(void)
{
   int r, c;

   /* set default Gandalf error handler without trace handling */
   printf("\nSetup for camera_test completed!\n\n");

   /* create weight parameters for cubic B-spline camera test */
   for(r = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); r>=0; r--)
      for(c = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); c>=0; c--)
      {
         camera_weight4[r][c].x = 0.002*(r-8)*(c-8);
         camera_weight4[r][c].y = -0.003*(r-7)*(c-9);
      }

   return GAN_TRUE;
}

static Gan_Bool teardown_test(void)
{
   printf("\nTeardown for camera_test completed!\n\n");
   return GAN_TRUE;
}

static Gan_Bool build_camera ( Gan_CameraType type, Gan_Camera *camera )
{
   switch ( type )
   {
      case GAN_LINEAR_CAMERA:
        cu_assert ( gan_camera_build_linear ( camera, CAMERA_ZH,
                                              CAMERA_FX, CAMERA_FY,
                                              CAMERA_X0, CAMERA_Y0 ) );
        break;

      case GAN_AFFINE_CAMERA:
        cu_assert ( gan_camera_build_affine (
                               camera, CAMERA_ZH,
                               CAMERA_FX, CAMERA_FY,
                               CAMERA_X0, CAMERA_Y0,
                               CAMERA_AFFINE_SKEW, CAMERA_AFFINE_KYX ) );
        break;

      case GAN_RADIAL_DISTORTION_1:
        cu_assert ( gan_camera_build_radial_distortion_1 (
                               camera, CAMERA_ZH,
                               CAMERA_FX, CAMERA_FY,
                               CAMERA_X0, CAMERA_Y0,
                               CAMERA_K1 ) );
        break;

      case GAN_RADIAL_DISTORTION_2:
        cu_assert ( gan_camera_build_radial_distortion_2 (
                               camera, CAMERA_ZH,
                               CAMERA_FX, CAMERA_FY,
                               CAMERA_X0, CAMERA_Y0,
                               CAMERA_K1, CAMERA_K2 ) );
        break;

      case GAN_RADIAL_DISTORTION_3:
        cu_assert ( gan_camera_build_radial_distortion_3 (
                               camera, CAMERA_ZH,
                               CAMERA_FX, CAMERA_FY,
                               CAMERA_X0, CAMERA_Y0,
                               CAMERA_K1, CAMERA_K2, CAMERA_K3 ) );
        break;

      case GAN_STEREOGRAPHIC_CAMERA:
        cu_assert ( gan_camera_build_stereographic ( camera, CAMERA_ZH,
                                                     CAMERA_FX, CAMERA_FY,
                                                     CAMERA_X0, CAMERA_Y0 ) );
        break;

      case GAN_EQUIDISTANT_CAMERA:
        cu_assert ( gan_camera_build_equidistant ( camera, CAMERA_ZH,
                                                   CAMERA_FX, CAMERA_FY,
                                                   CAMERA_X0, CAMERA_Y0 ) );
        break;

      case GAN_SINE_LAW_CAMERA:
        cu_assert ( gan_camera_build_sine_law ( camera, CAMERA_ZH,
                                                CAMERA_FX, CAMERA_FY,
                                                CAMERA_X0, CAMERA_Y0 ) );
        break;

      case GAN_EQUI_SOLID_ANGLE_CAMERA:
        cu_assert ( gan_camera_build_equi_solid_angle ( camera, CAMERA_ZH,
                                                        CAMERA_FX, CAMERA_FY,
                                                        CAMERA_X0, CAMERA_Y0));
        break;

      case GAN_XY_DISTORTION_4:
        cu_assert ( gan_camera_build_xy_distortion_4 (camera, CAMERA_ZH,
                                                      CAMERA_FX, CAMERA_FY,
                                                      CAMERA_X0, CAMERA_Y0,
                                                      CAMERA_CXX, CAMERA_CXY,
                                                      CAMERA_CYX, CAMERA_CYY));
        break;

      case GAN_RADIAL_DISTORTION_1_INV:
        cu_assert ( gan_camera_build_radial_distortion_1_inv (camera, CAMERA_ZH,
                                                              CAMERA_FX, CAMERA_FY,
                                                              CAMERA_X0, CAMERA_Y0,
                                                              CAMERA_K1 ) );
        break;

      case GAN_CUBIC_BSPLINE_CAMERA:
        cu_assert ( gan_camera_build_cubic_Bspline (camera, CAMERA_ZH,
                                                    CAMERA_FX, CAMERA_FY,
                                                    CAMERA_X0, CAMERA_Y0,
                                                    CAMERA_SKEW, CAMERA_KYX,
                                                    CAMERA_KZX, CAMERA_KZY,
                                                    camera_weight4,
                                                    &cbspline_support));
        break;

      case GAN_CUBIC_BSPLINE_CAMERA_INV:
        cu_assert ( gan_camera_build_cubic_Bspline_inv (camera, CAMERA_ZH,
                                                        CAMERA_FX, CAMERA_FY,
                                                        CAMERA_X0, CAMERA_Y0,
                                                        CAMERA_SKEW, CAMERA_KYX,
                                                        CAMERA_KZX, CAMERA_KZY,
                                                        camera_weight4,
                                                        &cbspline_support));
        break;

      default:
        cu_assert(0);
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool build_camera_f ( Gan_CameraType type, Gan_Camera_f *camera )
{
   switch ( type )
   {
      case GAN_LINEAR_CAMERA:
        cu_assert ( gan_cameraf_build_linear ( camera, CAMERA_ZH,
                                              CAMERA_FX, CAMERA_FY,
                                              CAMERA_X0, CAMERA_Y0 ) );
        break;

      case GAN_AFFINE_CAMERA:
        cu_assert ( gan_cameraf_build_affine (
                               camera, CAMERA_ZH,
                               CAMERA_FX, CAMERA_FY,
                               CAMERA_X0, CAMERA_Y0,
                               CAMERA_AFFINE_SKEW, CAMERA_AFFINE_KYX ) );
        break;

      case GAN_RADIAL_DISTORTION_1:
        cu_assert ( gan_cameraf_build_radial_distortion_1 (
                               camera, CAMERA_ZH,
                               CAMERA_FX, CAMERA_FY,
                               CAMERA_X0, CAMERA_Y0,
                               CAMERA_K1 ) );
        break;

      case GAN_RADIAL_DISTORTION_2:
        cu_assert ( gan_cameraf_build_radial_distortion_2 (
                               camera, CAMERA_ZH,
                               CAMERA_FX, CAMERA_FY,
                               CAMERA_X0, CAMERA_Y0,
                               CAMERA_K1, CAMERA_K2 ) );
        break;

      case GAN_RADIAL_DISTORTION_3:
        cu_assert ( gan_cameraf_build_radial_distortion_3 (
                               camera, CAMERA_ZH,
                               CAMERA_FX, CAMERA_FY,
                               CAMERA_X0, CAMERA_Y0,
                               CAMERA_K1, CAMERA_K2, CAMERA_K3 ) );
        break;

      case GAN_STEREOGRAPHIC_CAMERA:
        cu_assert ( gan_cameraf_build_stereographic ( camera, CAMERA_ZH,
                                                      CAMERA_FX, CAMERA_FY,
                                                      CAMERA_X0, CAMERA_Y0 ) );
        break;

      case GAN_EQUIDISTANT_CAMERA:
        cu_assert ( gan_cameraf_build_equidistant ( camera, CAMERA_ZH,
                                                    CAMERA_FX, CAMERA_FY,
                                                    CAMERA_X0, CAMERA_Y0 ) );
        break;

      case GAN_SINE_LAW_CAMERA:
        cu_assert ( gan_cameraf_build_sine_law ( camera, CAMERA_ZH,
                                                 CAMERA_FX, CAMERA_FY,
                                                 CAMERA_X0, CAMERA_Y0 ) );
        break;

      case GAN_EQUI_SOLID_ANGLE_CAMERA:
        cu_assert ( gan_cameraf_build_equi_solid_angle (camera, CAMERA_ZH,
                                                        CAMERA_FX, CAMERA_FY,
                                                        CAMERA_X0, CAMERA_Y0));
        break;

      case GAN_XY_DISTORTION_4:
        cu_assert ( gan_cameraf_build_xy_distortion_4(camera, CAMERA_ZH,
                                                      CAMERA_FX, CAMERA_FY,
                                                      CAMERA_X0, CAMERA_Y0,
                                                      CAMERA_CXX, CAMERA_CXY,
                                                      CAMERA_CYX, CAMERA_CYY));
        break;

      case GAN_RADIAL_DISTORTION_1_INV:
        cu_assert ( gan_cameraf_build_radial_distortion_1_inv (
                               camera, CAMERA_ZH,
                               CAMERA_FX, CAMERA_FY,
                               CAMERA_X0, CAMERA_Y0,
                               CAMERA_K1 ) );
        break;

      default:
        cu_assert(0);
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

#define NO_POINTS 10

/* points in 2D camera coordinates */
static Gan_Vector3 camerap[NO_POINTS] = {
    { 0.1,   0.1,  1.0}, 
    { 0.0,   0.0,  1.0},
    {-0.4,   0.3,  1.0},
    { 0.6,  -0.3,  1.0},
    {-0.7,  -0.2,  1.0},
    { 0.75,  0.6,  1.0},
    { 0.14, -0.2,  1.0},
    {-0.1,  -0.1,  1.0},
    { 0.8,  -0.75, 1.0},
    { 0.25, -0.18, 1.0}};

/* points in 2D image coordinates under linear projection */
static Gan_Vector3 linearp[NO_POINTS] = {
    {123.0, 135.0, CAMERA_ZH}, 
    {110.0, 120.0, CAMERA_ZH},
    { 58.0, 165.0, CAMERA_ZH},
    {188.0,  75.0, CAMERA_ZH},
    { 19.0,  90.0, CAMERA_ZH},
    {207.5, 210.0, CAMERA_ZH},
    {128.2,  90.0, CAMERA_ZH},
    { 97.0, 105.0, CAMERA_ZH},
    {214.0,   7.5, CAMERA_ZH},
    {142.5,  93.0, CAMERA_ZH}};

/* points in 2D image coordinates under affine projection */
static Gan_Vector3 affinep[NO_POINTS] = {
    {123.09, 134.94, CAMERA_ZH},
    {110,    120.0,  CAMERA_ZH},
    { 58.27, 165.24, CAMERA_ZH},
    {187.73,  74.64, CAMERA_ZH},
    { 18.82, 90.42,  CAMERA_ZH},
    {208.04, 209.55, CAMERA_ZH},
    {128.02, 89.916, CAMERA_ZH},
    { 96.91, 105.06, CAMERA_ZH},
    {213.325, 7.02,  CAMERA_ZH},
    {142.338, 92.85, CAMERA_ZH}};

/* points in 2D image coordinates under 1 parameter of radial distortion */
static Gan_Vector3 radial1p[NO_POINTS] = {
    {123.052,    135.06,    CAMERA_ZH},
    {110.0,      120.0,     CAMERA_ZH},
    { 55.4,      167.25,    CAMERA_ZH},
    {195.02,      70.95,    CAMERA_ZH},
    {  9.354,     86.82,    CAMERA_ZH},
    {225.48875,  226.605,   CAMERA_ZH},
    {128.416944,  89.6424,  CAMERA_ZH},
    { 96.948,    104.94,    CAMERA_ZH},
    {239.012,    -19.55625, CAMERA_ZH},
    {143.11685,   92.48754, CAMERA_ZH}};

/* points in 2D image coordinates under 2 parameters of radial distortion */
static Gan_Vector3 radial2p[NO_POINTS] = {
    {123.05148,      135.0594,       CAMERA_ZH},
    {110.0,          120.0,          CAMERA_ZH},
    { 55.725,        166.96875,      CAMERA_ZH},
    {193.4405,        71.86125,      CAMERA_ZH},
    { 11.91019,       87.6627,       CAMERA_ZH},
    {217.1914390625, 218.94594375,   CAMERA_ZH},
    {128.4104790688,  89.65305648,   CAMERA_ZH},
    { 96.94852,      104.9406,       CAMERA_ZH},
    {223.973535,      -3.2886796875, CAMERA_ZH},
    {143.0875804675,  92.511856227,  CAMERA_ZH}};

/* points in 2D image coordinates under 3 parameters of radial distortion */
static Gan_Vector3 radial3p[NO_POINTS] = {
    {123.0514956,        135.059418,          CAMERA_ZH},
    {110.0,              120.0,               CAMERA_ZH},
    { 55.603125,         167.07421875,        CAMERA_ZH},
    {194.5066625,         71.24615625,        CAMERA_ZH},
    {  9.87801895,        86.9927535,         CAMERA_ZH},
    {228.67284307226561,  229.5441628359375,  CAMERA_ZH},
    {128.41105703364929,  89.652103790688003, CAMERA_ZH},
    { 96.9485044,        104.940582,          CAMERA_ZH},
    {251.09916624375003, -32.631309638671894, CAMERA_ZH},
    {143.09174698545138,  92.50839481208655,  CAMERA_ZH}};

/* points in 2D image coordinates under stereographic projection */
static Gan_Vector3 stereop[NO_POINTS] = {
    {122.93564198707014,  134.9257407543117,   CAMERA_ZH},
    {110.0,               120.0,               CAMERA_ZH},
    { 60.897860680043742, 162.49223594996215,  CAMERA_ZH},
    {180.77527873146624,   79.168108424154099, CAMERA_ZH},
    { 28.638552530708935,  93.177544790343603, CAMERA_ZH},
    {191.70816278082378,  195.42291948999119,  CAMERA_ZH},
    {127.93661216303738,   90.4341557752131,   CAMERA_ZH},
    { 97.064358012929858, 105.07425924568831,  CAMERA_ZH},
    {193.73313867512789,   29.423287490847251, CAMERA_ZH},
    {141.76348896137065,   93.61187070901515,  CAMERA_ZH}};

/* points in 2D image coordinates under equidistant projection */
static Gan_Vector3 equidistp[NO_POINTS] = {
    {122.91435870358268,  134.90118311951846,  CAMERA_ZH},
    {110.0,               120.0,               CAMERA_ZH},
    {61.780648663916161,  161.72828481007255,  CAMERA_ZH},
    {178.7040450396251,    80.36305093867783,  CAMERA_ZH},
    { 31.341314840593867,  94.068565332063912, CAMERA_ZH},
    {187.68142648680558,  191.70593214166669,  CAMERA_ZH},
    {127.85083041784358,   90.575554256301785, CAMERA_ZH},
    { 97.085641296417322, 105.09881688048152,  CAMERA_ZH},
    {188.8530328754706,    34.702248091438065, CAMERA_ZH},
    {141.52675945602627,   93.808538298070488, CAMERA_ZH}};

/* points in 2D image coordinates under sine-law projection */
static Gan_Vector3 sinep[NO_POINTS] = {
    {122.87191805869676,  134.85221314465011,  CAMERA_ZH},
    {110.0,               120.0,               CAMERA_ZH},
    { 63.489786068004371, 160.24922359499621,  CAMERA_ZH},
    {174.77547428591717,   82.629534065817012, CAMERA_ZH},
    { 36.430860405645667,  95.746437496366696, CAMERA_ZH},
    {180.31879848252532,  184.90966013771569,  CAMERA_ZH},
    {127.68073899816773,   90.855924728294966, CAMERA_ZH},
    { 97.128081941303236, 105.14778685534988,  CAMERA_ZH},
    {180.07698053503773,   44.19557394046398,  CAMERA_ZH},
    {141.05961964674128,   94.196623678091854, CAMERA_ZH}};

/* points in 2D image coordinates under equi-solid angle projection */
static Gan_Vector3 equisap[NO_POINTS] = {
    {122.90374068610356, 134.88893156088872,   CAMERA_ZH},
    {110.0,              120.0,                CAMERA_ZH},
    { 62.21139252614887,  161.35552569852501,  CAMERA_ZH},
    {177.70895249188774,   80.937142793141689, CAMERA_ZH},
    { 32.632747971334794,  94.494312518022454, CAMERA_ZH},
    {185.79986697192894,  189.96910797408825,  CAMERA_ZH},
    {127.80821602985613,   90.645797752984407, CAMERA_ZH},
    { 97.096259313896439, 105.11106843911128,  CAMERA_ZH},
    {186.60134156184571,   37.137971868195748, CAMERA_ZH},
    {141.40958270645501,   93.90588513617584,  CAMERA_ZH}};

/* points in 2D image coordinates under xy distortion with 4 parameters */
static Gan_Vector3 xydist4p[NO_POINTS] = {
    {122.987,   135.045,    CAMERA_ZH},
    {110.0,     120.0,      CAMERA_ZH},
    { 57.74,    167.475,    CAMERA_ZH},
    {191.51,     68.925,    CAMERA_ZH},
    { 11.174,    84.24,     CAMERA_ZH},
    {207.93875, 227.01,     CAMERA_ZH},
    {128.052944, 89.8848,   CAMERA_ZH},
    { 97.013,   104.955,    CAMERA_ZH},
    {209.762,   -14.971875, CAMERA_ZH},
    {142.59035,  92.41248,  CAMERA_ZH}};

/* points in 2D image coordinates under 1 parameter of inverse radial
   distortion */
static Gan_Vector3 radial1invp[NO_POINTS] = {
    {122.94861419548025, 134.94070868709258, CAMERA_ZH},
    {110.0,              120.0,              CAMERA_ZH},
    {60.273647482139374, 163.0324204481486,  CAMERA_ZH},
    {182.38867748367059, 78.237301451728513, CAMERA_ZH},
    {26.462271782532227, 92.460089598636998, CAMERA_ZH},
    {195.40829983895523, 198.83843062057406, CAMERA_ZH},
    {127.99046310301713, 90.345390489532193, CAMERA_ZH},
    {97.051385804519754, 105.0592913129074,  CAMERA_ZH},
    {198.55734042403265, 24.204800022080072, CAMERA_ZH},
    {141.91581895067335, 93.485319640979057, CAMERA_ZH}};

/* points in 2D image coordinates under cubic B-spline distortion */
#if 0
static Gan_Vector3 cbsplinep[NO_POINTS] = {
    {123.0416,   134.956125, CAMERA_ZH},
    {110,        120.028125, CAMERA_ZH},
    { 57.5008,   166.207125, CAMERA_ZH},
    {187.993344,  75.00798,  CAMERA_ZH},
    { 19.0,       90,        CAMERA_ZH},
    {207.5,      210,        CAMERA_ZH},
    {128.08352,   90.076725, CAMERA_ZH},
    { 97.0416,   104.956125, CAMERA_ZH},
    {214,          7.5,      CAMERA_ZH},
    {142.3128,    93.158625, CAMERA_ZH}};
#else
static Gan_Vector3 cbsplinep[NO_POINTS] = {
    {123.0364, 134.96914285714286, CAMERA_ZH},
    {110, 120.03214285714286, CAMERA_ZH},
    {57.563200000000002, 166.10314285714284, CAMERA_ZH},
    {187.644216, 75.410057142857141, CAMERA_ZH},
    {19.00055466666667, 89.999305714285711, CAMERA_ZH},
    {207.5, 210, CAMERA_ZH},
    {128.09808000000001, 90.055542857142854, CAMERA_ZH},
    {97.0364, 104.96914285714286, CAMERA_ZH},
    {214, 7.5, CAMERA_ZH},
    {142.33619999999999, 93.122142857142862, CAMERA_ZH}};
#endif

/* points in 2D image coordinates under inversecubic B-spline distortion */
#if 0
static Gan_Vector3 cbsplineinvp[NO_POINTS] = {
    {122.95841260861306,  135.04357848558891,  CAMERA_ZH},
    {110,                 119.9719591226321,   CAMERA_ZH},
    { 58.481698572598525, 163.82834830526116,  CAMERA_ZH},
    {188.00661659658306,   74.992066873520386, CAMERA_ZH},
    { 19.0,                90,                 CAMERA_ZH},
    {207.5,               210,                 CAMERA_ZH},
    {128.3175366999123,    89.922086694030938, CAMERA_ZH},
    { 96.958388203705439, 105.04362051428747,  CAMERA_ZH},
    {214,                   7.5,               CAMERA_ZH},
    {142.68941604629805,   92.838679168676848, CAMERA_ZH}};
#else
static Gan_Vector3 cbsplineinvp[NO_POINTS] = {
    {122.96362781487103, 135.03059132404431, CAMERA_ZH},
    {110, 119.96795328300811, CAMERA_ZH},
    {58.422903542546294, 163.92559581327293, CAMERA_ZH},
    {188.34216183050827, 74.604510151795807, CAMERA_ZH},
    {18.999445842523432, 90.000693639442119, CAMERA_ZH},
    {207.5, 210, CAMERA_ZH},
    {128.30268755936433, 89.943653172146384, CAMERA_ZH},
    {96.963572677525391, 105.03068627834269, CAMERA_ZH},
    {214, 7.5, CAMERA_ZH},
    {142.66538934806221, 92.876050697630646, CAMERA_ZH}};
#endif

#define IMAGE_THRESHOLD 1.0e-6
#define CAMERA_THRESHOLD 1.0e-4
#define IMAGE_THRESHOLD_F 1.0e-4
#define CAMERA_THRESHOLD_F 1.0e-3

static Gan_Bool
 compare_image_coordinates ( Gan_CameraType type, Gan_Vector3 p[NO_POINTS] )
{
   int i;

   switch ( type )
   {
      case GAN_LINEAR_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - linearp[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - linearp[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_AFFINE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - affinep[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - affinep[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_RADIAL_DISTORTION_1:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - radial1p[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - radial1p[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_RADIAL_DISTORTION_2:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - radial2p[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - radial2p[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_RADIAL_DISTORTION_3:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - radial3p[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - radial3p[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_STEREOGRAPHIC_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - stereop[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - stereop[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_EQUIDISTANT_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - equidistp[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - equidistp[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_SINE_LAW_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - sinep[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - sinep[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_EQUI_SOLID_ANGLE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - equisap[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - equisap[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_XY_DISTORTION_4:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - xydist4p[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - xydist4p[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_RADIAL_DISTORTION_1_INV:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - radial1invp[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - radial1invp[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_CUBIC_BSPLINE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - cbsplinep[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - cbsplinep[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_CUBIC_BSPLINE_CAMERA_INV:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs(p[i].x - cbsplineinvp[i].x) < IMAGE_THRESHOLD );
           cu_assert ( fabs(p[i].y - cbsplineinvp[i].y) < IMAGE_THRESHOLD );
           cu_assert ( p[i].z == CAMERA_ZH );
        }
        break;

      default:
        cu_assert(0);
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool
 compare_image_coordinates_f ( Gan_CameraType type,
                               Gan_Vector3_f p[NO_POINTS] )
{
   int i;

   switch ( type )
   {
      case GAN_LINEAR_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert (fabs((double)p[i].x - linearp[i].x) < IMAGE_THRESHOLD_F);
           cu_assert (fabs((double)p[i].y - linearp[i].y) < IMAGE_THRESHOLD_F);
           cu_assert ( (double)p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_AFFINE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert(fabs((double)p[i].x - affinep[i].x) < IMAGE_THRESHOLD_F);
           cu_assert(fabs((double)p[i].y - affinep[i].y) < IMAGE_THRESHOLD_F);
           cu_assert( (double)p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_RADIAL_DISTORTION_1:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert(fabs((double)p[i].x - radial1p[i].x) < IMAGE_THRESHOLD_F);
           cu_assert(fabs((double)p[i].y - radial1p[i].y) < IMAGE_THRESHOLD_F);
           cu_assert( (double)p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_RADIAL_DISTORTION_2:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert(fabs((double)p[i].x - radial2p[i].x) < IMAGE_THRESHOLD_F);
           cu_assert(fabs((double)p[i].y - radial2p[i].y) < IMAGE_THRESHOLD_F);
           cu_assert( (double)p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_RADIAL_DISTORTION_3:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert(fabs((double)p[i].x - radial3p[i].x) < IMAGE_THRESHOLD_F);
           cu_assert(fabs((double)p[i].y - radial3p[i].y) < IMAGE_THRESHOLD_F);
           cu_assert( (double)p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_STEREOGRAPHIC_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert(fabs((double)p[i].x - stereop[i].x) < IMAGE_THRESHOLD_F );
           cu_assert(fabs((double)p[i].y - stereop[i].y) < IMAGE_THRESHOLD_F );
           cu_assert( (double)p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_EQUIDISTANT_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs((double)p[i].x - equidistp[i].x)
                       < IMAGE_THRESHOLD_F );
           cu_assert ( fabs((double)p[i].y - equidistp[i].y)
                       < IMAGE_THRESHOLD_F );
           cu_assert ( (double)p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_SINE_LAW_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs((double)p[i].x - sinep[i].x) < IMAGE_THRESHOLD_F );
           cu_assert ( fabs((double)p[i].y - sinep[i].y) < IMAGE_THRESHOLD_F );
           cu_assert ( (double)p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_EQUI_SOLID_ANGLE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert(fabs((double)p[i].x - equisap[i].x) < IMAGE_THRESHOLD_F );
           cu_assert(fabs((double)p[i].y - equisap[i].y) < IMAGE_THRESHOLD_F );
           cu_assert( (double)p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_XY_DISTORTION_4:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert(fabs((double)p[i].x - xydist4p[i].x) < IMAGE_THRESHOLD_F);
           cu_assert(fabs((double)p[i].y - xydist4p[i].y) < IMAGE_THRESHOLD_F);
           cu_assert( (double)p[i].z == CAMERA_ZH );
        }
        break;

      case GAN_RADIAL_DISTORTION_1_INV:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           cu_assert ( fabs ( (double)p[i].x - radial1invp[i].x ) < IMAGE_THRESHOLD_F );
           cu_assert ( fabs ( (double)p[i].y - radial1invp[i].y ) < IMAGE_THRESHOLD_F );
           cu_assert( (double)p[i].z == CAMERA_ZH );
        }
        break;

      default:
        cu_assert(0);
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool
 compare_camera_coordinates ( Gan_Vector3 p[NO_POINTS] )
{
   int i;

   for ( i = NO_POINTS-1; i >= 0; i-- )
   {
      cu_assert ( fabs(p[i].x - camerap[i].x) < CAMERA_THRESHOLD );
      cu_assert ( fabs(p[i].y - camerap[i].y) < CAMERA_THRESHOLD );
      cu_assert ( p[i].z == 1.0 );
   }

   return GAN_TRUE;
}

static Gan_Bool
 compare_camera_coordinates_f ( Gan_Vector3_f p[NO_POINTS] )
{
   int i;

   for ( i = NO_POINTS-1; i >= 0; i-- )
   {
      cu_assert ( fabs((double)p[i].x - camerap[i].x) < CAMERA_THRESHOLD_F );
      cu_assert ( fabs((double)p[i].y - camerap[i].y) < CAMERA_THRESHOLD_F );
      cu_assert ( p[i].z == 1.0F );
   }

   return GAN_TRUE;
}

static Gan_Bool test_point_projection ( Gan_CameraType type )
{
   Gan_Camera camera;
   int i;
   Gan_Vector3 p[NO_POINTS];

   cu_assert ( build_camera ( type, &camera ) );
   for ( i = NO_POINTS-1; i >= 0; i-- )
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &p[i] ) );

   cu_assert ( compare_image_coordinates ( type, p ) );
   return GAN_TRUE;
}

static Gan_Bool test_point_projection_f ( Gan_CameraType type )
{
   Gan_Camera_f camera;
   int i;
   Gan_Vector3_f p[NO_POINTS], pf;

   cu_assert ( build_camera_f ( type, &camera ) );
   for ( i = NO_POINTS-1; i >= 0; i-- )
   {
      (void)gan_vec3f_fill_q ( &pf, camerap[i].x, camerap[i].y, camerap[i].z );
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf, &p[i] ));
   }

   cu_assert ( compare_image_coordinates_f ( type, p ) );
   return GAN_TRUE;
}

#define STEP_SIZE 1.0e-6
#define INVERSE_STEP_SIZE 1.0e6
#define DERIVATIVE_THRESHOLD HUGE_VAL // 1.0e-4

/* test derivatives in double precision */
static Gan_Bool test_point_projection_derivatives ( Gan_CameraType type )
{
   Gan_Camera camera;
   int i;
   Gan_Vector3 p[NO_POINTS];

   cu_assert ( build_camera ( type, &camera ) );
   for ( i = NO_POINTS-1; i >= 0; i-- )
   {
      int ic;

      Gan_Matrix22 HXS, HXN;
      Gan_Camera HCameraS[2], HCameraN[2];
      Gan_Vector3 vtmp1, vtmp2;
      static Gan_Vector2 Hweight[4][1 + (1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)][1 + (1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)];
      Gan_CubicBSplineSupport Hsupport[4];

      if(type == GAN_CUBIC_BSPLINE_CAMERA || GAN_CUBIC_BSPLINE_CAMERA_INV )
      {
         HCameraS[0].nonlinear.cbspline.weight = Hweight[0];
         HCameraS[1].nonlinear.cbspline.weight = Hweight[1];
         HCameraS[0].nonlinear.cbspline.support = &Hsupport[0];
         HCameraS[1].nonlinear.cbspline.support = &Hsupport[1];
         HCameraN[0].nonlinear.cbspline.weight = Hweight[2];
         HCameraN[1].nonlinear.cbspline.weight = Hweight[3];
         HCameraN[0].nonlinear.cbspline.support = &Hsupport[2];
         HCameraN[1].nonlinear.cbspline.support = &Hsupport[3];
      }

      /* compute derivatives symbolically */
      cu_assert ( gan_camera_project_point_gen ( &camera, &camerap[i], &p[i], NULL, &HXS, HCameraS, NULL ) );

      /* compute derivatives numerically */

      /* derivative w.r.t. X */
      camerap[i].x -= STEP_SIZE;
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                               &vtmp1 ) );
      camerap[i].x += 2.0*STEP_SIZE;
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                               &vtmp2 ) );
      camerap[i].x -= STEP_SIZE;
      HXN.xx = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
      HXN.yx = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

      /* derivative w.r.t. Y */
      camerap[i].y -= STEP_SIZE;
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                               &vtmp1 ) );
      camerap[i].y += 2.0*STEP_SIZE;
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                               &vtmp2 ) );
      camerap[i].y -= STEP_SIZE;
      HXN.xy = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
      HXN.yy = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

      /* check derivatives produced so far */
      cu_assert ( fabs(HXS.xx-HXN.xx) < DERIVATIVE_THRESHOLD &&
                  fabs(HXS.xy-HXN.xy) < DERIVATIVE_THRESHOLD &&
                  fabs(HXS.yx-HXN.yx) < DERIVATIVE_THRESHOLD &&
                  fabs(HXS.yy-HXN.yy) < DERIVATIVE_THRESHOLD );

      /* derivatives w.r.t. camera parameters */

      /* derivative w.r.t. fx */
      camera.fx -= STEP_SIZE;
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                               &vtmp1 ) );
      camera.fx += 2.0*STEP_SIZE;
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                               &vtmp2 ) );
      camera.fx -= STEP_SIZE;
      HCameraN[0].fx = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
      HCameraN[1].fx = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

      /* derivative w.r.t. fy */
      camera.fy -= STEP_SIZE;
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                               &vtmp1 ) );
      camera.fy += 2.0*STEP_SIZE;
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                               &vtmp2 ) );
      camera.fy -= STEP_SIZE;
      HCameraN[0].fy = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
      HCameraN[1].fy = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

      /* derivative w.r.t. x0 */
      camera.x0 -= STEP_SIZE;
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                               &vtmp1 ) );
      camera.x0 += 2.0*STEP_SIZE;
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                               &vtmp2 ) );
      camera.x0 -= STEP_SIZE;
      HCameraN[0].x0 = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
      HCameraN[1].x0 = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

      /* derivative w.r.t. y0 */
      camera.y0 -= STEP_SIZE;
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                               &vtmp1 ) );
      camera.y0 += 2.0*STEP_SIZE;
      cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                               &vtmp2 ) );
      camera.y0 -= STEP_SIZE;
      HCameraN[0].y0 = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
      HCameraN[1].y0 = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

      /* check camera derivatives produced so far */
      for(ic = 0; ic < 2; ic++)
         cu_assert ( fabs(HCameraS[ic].fx-HCameraN[ic].fx) < DERIVATIVE_THRESHOLD &&
                     fabs(HCameraS[ic].fy-HCameraN[ic].fy) < DERIVATIVE_THRESHOLD &&
                     fabs(HCameraS[ic].x0-HCameraN[ic].x0) < DERIVATIVE_THRESHOLD &&
                     fabs(HCameraS[ic].y0-HCameraN[ic].y0) < DERIVATIVE_THRESHOLD );

      switch(type)
      {
         case GAN_AFFINE_CAMERA:

           /* derivative w.r.t. skew */
           camera.nonlinear.affine.skew -= STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp1 ) );
           camera.nonlinear.affine.skew += 2.0*STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp2 ) );
           camera.nonlinear.affine.skew -= STEP_SIZE;
           HCameraN[0].nonlinear.affine.skew = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.affine.skew = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. kyx */
           camera.nonlinear.affine.kyx -= STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp1 ) );
           camera.nonlinear.affine.kyx += 2.0*STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp2 ) );
           camera.nonlinear.affine.kyx -= STEP_SIZE;
           HCameraN[0].nonlinear.affine.kyx = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.affine.kyx = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* check derivatives of nonlinear parameters */
           for(ic = 0; ic < 2; ic++)
              cu_assert ( fabs(HCameraS[ic].nonlinear.affine.skew-HCameraN[ic].nonlinear.affine.skew) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.affine.kyx-HCameraN[ic].nonlinear.affine.kyx)   < DERIVATIVE_THRESHOLD );

           break;

         case GAN_RADIAL_DISTORTION_1:

           /* derivative w.r.t. K1 */
           camera.nonlinear.radial1.K1 -= STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp1 ) );
           camera.nonlinear.radial1.K1 += 2.0*STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp2 ) );
           camera.nonlinear.radial1.K1 -= STEP_SIZE;
           HCameraN[0].nonlinear.radial1.K1 = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.radial1.K1 = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* check derivatives of nonlinear parameters */
           for(ic = 0; ic < 2; ic++)
              cu_assert ( fabs(HCameraS[ic].nonlinear.radial1.K1-HCameraN[ic].nonlinear.radial1.K1) < DERIVATIVE_THRESHOLD );

           break;

         case GAN_RADIAL_DISTORTION_2:

           /* derivative w.r.t. K1 */
           camera.nonlinear.radial2.K1 -= STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp1 ) );
           camera.nonlinear.radial2.K1 += 2.0*STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp2 ) );
           camera.nonlinear.radial2.K1 -= STEP_SIZE;
           HCameraN[0].nonlinear.radial2.K1 = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.radial2.K1 = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. K2 */
           camera.nonlinear.radial2.K2 -= STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp1 ) );
           camera.nonlinear.radial2.K2 += 2.0*STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp2 ) );
           camera.nonlinear.radial2.K2 -= STEP_SIZE;
           HCameraN[0].nonlinear.radial2.K2 = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.radial2.K2 = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* check derivatives of nonlinear parameters */
           for(ic = 0; ic < 2; ic++)
              cu_assert ( fabs(HCameraS[ic].nonlinear.radial2.K1-HCameraN[ic].nonlinear.radial2.K1) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.radial2.K2-HCameraN[ic].nonlinear.radial2.K2) < DERIVATIVE_THRESHOLD );

           break;

         case GAN_RADIAL_DISTORTION_3:

           /* derivative w.r.t. K1 */
           camera.nonlinear.radial3.K1 -= STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp1 ) );
           camera.nonlinear.radial3.K1 += 2.0*STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp2 ) );
           camera.nonlinear.radial3.K1 -= STEP_SIZE;
           HCameraN[0].nonlinear.radial3.K1 = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.radial3.K1 = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. K2 */
           camera.nonlinear.radial3.K2 -= STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp1 ) );
           camera.nonlinear.radial3.K2 += 2.0*STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp2 ) );
           camera.nonlinear.radial3.K2 -= STEP_SIZE;
           HCameraN[0].nonlinear.radial3.K2 = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.radial3.K2 = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. K3 */
           camera.nonlinear.radial3.K3 -= STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp1 ) );
           camera.nonlinear.radial3.K3 += 2.0*STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp2 ) );
           camera.nonlinear.radial3.K3 -= STEP_SIZE;
           HCameraN[0].nonlinear.radial3.K3 = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.radial3.K3 = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* check derivatives of nonlinear parameters */
           for(ic = 0; ic < 2; ic++)
              cu_assert ( fabs(HCameraS[ic].nonlinear.radial3.K1-HCameraN[ic].nonlinear.radial3.K1) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.radial3.K2-HCameraN[ic].nonlinear.radial3.K2) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.radial3.K3-HCameraN[ic].nonlinear.radial3.K3) < DERIVATIVE_THRESHOLD );

           break;

         case GAN_XY_DISTORTION_4:

           /* derivative w.r.t. cxx */
           camera.nonlinear.xydist4.cxx -= STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp1 ) );
           camera.nonlinear.xydist4.cxx += 2.0*STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp2 ) );
           camera.nonlinear.xydist4.cxx -= STEP_SIZE;
           HCameraN[0].nonlinear.xydist4.cxx = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.xydist4.cxx = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. cxy */
           camera.nonlinear.xydist4.cxy -= STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp1 ) );
           camera.nonlinear.xydist4.cxy += 2.0*STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp2 ) );
           camera.nonlinear.xydist4.cxy -= STEP_SIZE;
           HCameraN[0].nonlinear.xydist4.cxy = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.xydist4.cxy = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. cyx */
           camera.nonlinear.xydist4.cyx -= STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp1 ) );
           camera.nonlinear.xydist4.cyx += 2.0*STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp2 ) );
           camera.nonlinear.xydist4.cyx -= STEP_SIZE;
           HCameraN[0].nonlinear.xydist4.cyx = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.xydist4.cyx = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. cyy */
           camera.nonlinear.xydist4.cyy -= STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp1 ) );
           camera.nonlinear.xydist4.cyy += 2.0*STEP_SIZE;
           cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i],
                                                    &vtmp2 ) );
           camera.nonlinear.xydist4.cyy -= STEP_SIZE;
           HCameraN[0].nonlinear.xydist4.cyy = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.xydist4.cyy = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* check derivatives of nonlinear parameters */
           for(ic = 0; ic < 2; ic++)
              cu_assert ( fabs(HCameraS[ic].nonlinear.xydist4.cxx-HCameraN[ic].nonlinear.xydist4.cxx) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.xydist4.cxy-HCameraN[ic].nonlinear.xydist4.cxy) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.xydist4.cyx-HCameraN[ic].nonlinear.xydist4.cyx) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.xydist4.cyy-HCameraN[ic].nonlinear.xydist4.cyy) < DERIVATIVE_THRESHOLD );
           break;

         case GAN_CUBIC_BSPLINE_CAMERA:
         {
            int r, c;

            /* derivative w.r.t. skew */
            camera.nonlinear.cbspline.K.xy -= STEP_SIZE;
            cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp1 ) );
            camera.nonlinear.cbspline.K.xy += 2.0*STEP_SIZE;
            cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp2 ) );
            camera.nonlinear.cbspline.K.xy -= STEP_SIZE;
            HCameraN[0].nonlinear.cbspline.K.xy = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
            HCameraN[1].nonlinear.cbspline.K.xy = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

            /* derivative w.r.t. kyx */
            camera.nonlinear.cbspline.K.yx -= STEP_SIZE;
            cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp1 ) );
            camera.nonlinear.cbspline.K.yx += 2.0*STEP_SIZE;
            cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp2 ) );
            camera.nonlinear.cbspline.K.yx -= STEP_SIZE;
            HCameraN[0].nonlinear.cbspline.K.yx = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
            HCameraN[1].nonlinear.cbspline.K.yx = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

            /* derivative w.r.t. kzx */
            camera.nonlinear.cbspline.K.zx -= STEP_SIZE;
            cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp1 ) );
            camera.nonlinear.cbspline.K.zx += 2.0*STEP_SIZE;
            cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp2 ) );
            camera.nonlinear.cbspline.K.zx -= STEP_SIZE;
            HCameraN[0].nonlinear.cbspline.K.zx = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
            HCameraN[1].nonlinear.cbspline.K.zx = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

            /* derivative w.r.t. kzy */
            camera.nonlinear.cbspline.K.zy -= STEP_SIZE;
            cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp1 ) );
            camera.nonlinear.cbspline.K.zy += 2.0*STEP_SIZE;
            cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp2 ) );
            camera.nonlinear.cbspline.K.zy -= STEP_SIZE;
            HCameraN[0].nonlinear.cbspline.K.zy = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
            HCameraN[1].nonlinear.cbspline.K.zy = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

            /* create weight parameters for cubic B-spline camera test */
            for(r = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); r>=0; r--)
               for(c = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); c>=0; c--)
               {
                  /* derivative w.r.t. x weight */
                  camera.nonlinear.cbspline.weight[r][c].x -= STEP_SIZE;
                  cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp1 ) );
                  camera.nonlinear.cbspline.weight[r][c].x += 2.0*STEP_SIZE;
                  cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp2 ) );
                  camera.nonlinear.cbspline.weight[r][c].x -= STEP_SIZE;
                  HCameraN[0].nonlinear.cbspline.weight[r][c].x = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
                  HCameraN[1].nonlinear.cbspline.weight[r][c].x = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

                  /* derivative w.r.t. y weight */
                  camera.nonlinear.cbspline.weight[r][c].y -= STEP_SIZE;
                  cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp1 ) );
                  camera.nonlinear.cbspline.weight[r][c].y += 2.0*STEP_SIZE;
                  cu_assert ( gan_camera_project_point_q ( &camera, &camerap[i], &vtmp2 ) );
                  camera.nonlinear.cbspline.weight[r][c].y -= STEP_SIZE;
                  HCameraN[0].nonlinear.cbspline.weight[r][c].y = 0.5*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
                  HCameraN[1].nonlinear.cbspline.weight[r][c].y = 0.5*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;
               }

            /* check derivatives of nonlinear parameters */
            for(ic = 0; ic < 2; ic++)
            {
               cu_assert ( fabs(HCameraS[ic].nonlinear.cbspline.K.xy-HCameraN[ic].nonlinear.cbspline.K.xy) < DERIVATIVE_THRESHOLD &&
                           fabs(HCameraS[ic].nonlinear.cbspline.K.yx-HCameraN[ic].nonlinear.cbspline.K.yx) < DERIVATIVE_THRESHOLD &&
                           fabs(HCameraS[ic].nonlinear.cbspline.K.zx-HCameraN[ic].nonlinear.cbspline.K.zx) < DERIVATIVE_THRESHOLD &&
                           fabs(HCameraS[ic].nonlinear.cbspline.K.zy-HCameraN[ic].nonlinear.cbspline.K.zy) < DERIVATIVE_THRESHOLD );

               for(r = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); r>=0; r--)
                  for(c = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); c>=0; c--)
                     cu_assert ( fabs(HCameraS[ic].nonlinear.cbspline.weight[r][c].x-HCameraN[ic].nonlinear.cbspline.weight[r][c].x) < DERIVATIVE_THRESHOLD &&
                                 fabs(HCameraS[ic].nonlinear.cbspline.weight[r][c].y-HCameraN[ic].nonlinear.cbspline.weight[r][c].y) < DERIVATIVE_THRESHOLD);
            }
         }
         break;

         default:
           cu_assert(0);
      }
   }

   return GAN_TRUE;
}

#undef STEP_SIZE
#undef INVERSE_STEP_SIZE
#undef DERIVATIVE_THRESHOLD

#define STEP_SIZE 1.0e-3F
#define INVERSE_STEP_SIZE 1.0e3F
#define DERIVATIVE_THRESHOLD 1.0e-1F

/* test derivatives in single precision */
static Gan_Bool test_point_projection_derivatives_f ( Gan_CameraType type )
{
   Gan_Camera_f camera;
   int i;
   Gan_Vector3_f p[NO_POINTS], pf;

   cu_assert ( build_camera_f ( type, &camera ) );
   for ( i = NO_POINTS-1; i >= 0; i-- )
   {
      int ic;

      Gan_Matrix22_f HXS, HXN;
      Gan_Camera_f HCameraS[2], HCameraN[2];
      Gan_Vector3_f vtmp1, vtmp2;

      /* compute derivatives symbolically */
      (void)gan_vec3f_fill_q ( &pf, camerap[i].x, camerap[i].y, camerap[i].z );
      cu_assert ( gan_cameraf_project_point_gen ( &camera, &pf, &p[i], NULL, &HXS, HCameraS, NULL ) );

      /* compute derivatives numerically */

      /* derivative w.r.t. X */
      pf.x -= STEP_SIZE;
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                               &vtmp1 ) );
      pf.x += 2.0F*STEP_SIZE;
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                               &vtmp2 ) );
      pf.x -= STEP_SIZE;
      HXN.xx = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
      HXN.yx = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

      /* derivative w.r.t. Y */
      pf.y -= STEP_SIZE;
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                               &vtmp1 ) );
      pf.y += 2.0F*STEP_SIZE;
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                               &vtmp2 ) );
      pf.y -= STEP_SIZE;
      HXN.xy = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
      HXN.yy = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

      /* check derivatives produced so far */
      cu_assert ( fabs(HXS.xx-HXN.xx) < DERIVATIVE_THRESHOLD &&
                  fabs(HXS.xy-HXN.xy) < DERIVATIVE_THRESHOLD &&
                  fabs(HXS.yx-HXN.yx) < DERIVATIVE_THRESHOLD &&
                  fabs(HXS.yy-HXN.yy) < DERIVATIVE_THRESHOLD );

      /* derivatives w.r.t. camera parameters */

      /* derivative w.r.t. fx */
      camera.fx -= STEP_SIZE;
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                               &vtmp1 ) );
      camera.fx += 2.0F*STEP_SIZE;
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                               &vtmp2 ) );
      camera.fx -= STEP_SIZE;
      HCameraN[0].fx = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
      HCameraN[1].fx = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

      /* derivative w.r.t. fy */
      camera.fy -= STEP_SIZE;
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                               &vtmp1 ) );
      camera.fy += 2.0F*STEP_SIZE;
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                               &vtmp2 ) );
      camera.fy -= STEP_SIZE;
      HCameraN[0].fy = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
      HCameraN[1].fy = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

      /* derivative w.r.t. x0 */
      camera.x0 -= STEP_SIZE;
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                               &vtmp1 ) );
      camera.x0 += 2.0F*STEP_SIZE;
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                               &vtmp2 ) );
      camera.x0 -= STEP_SIZE;
      HCameraN[0].x0 = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
      HCameraN[1].x0 = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

      /* derivative w.r.t. y0 */
      camera.y0 -= STEP_SIZE;
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                               &vtmp1 ) );
      camera.y0 += 2.0F*STEP_SIZE;
      cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                               &vtmp2 ) );
      camera.y0 -= STEP_SIZE;
      HCameraN[0].y0 = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
      HCameraN[1].y0 = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

      /* check camera derivatives produced so far */
      for(ic = 0; ic < 2; ic++)
         cu_assert ( fabs(HCameraS[ic].fx-HCameraN[ic].fx) < DERIVATIVE_THRESHOLD &&
                     fabs(HCameraS[ic].fy-HCameraN[ic].fy) < DERIVATIVE_THRESHOLD &&
                     fabs(HCameraS[ic].x0-HCameraN[ic].x0) < DERIVATIVE_THRESHOLD &&
                     fabs(HCameraS[ic].y0-HCameraN[ic].y0) < DERIVATIVE_THRESHOLD );

      switch(type)
      {
         case GAN_AFFINE_CAMERA:

           /* derivative w.r.t. K1 */
           camera.nonlinear.affine.skew -= STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf, &vtmp1 ) );
           camera.nonlinear.affine.skew += 2.0F*STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf, &vtmp2 ) );
           camera.nonlinear.affine.skew -= STEP_SIZE;
           HCameraN[0].nonlinear.affine.skew = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.affine.skew = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. K2 */
           camera.nonlinear.affine.kyx -= STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf, &vtmp1 ) );
           camera.nonlinear.affine.kyx += 2.0F*STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf, &vtmp2 ) );
           camera.nonlinear.affine.kyx -= STEP_SIZE;
           HCameraN[0].nonlinear.affine.kyx = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.affine.kyx = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* check derivatives of nonlinear parameters */
           for(ic = 0; ic < 2; ic++)
              cu_assert ( fabs(HCameraS[ic].nonlinear.affine.skew-HCameraN[ic].nonlinear.affine.skew) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.affine.kyx-HCameraN[ic].nonlinear.affine.kyx) < DERIVATIVE_THRESHOLD );
           break;

         case GAN_RADIAL_DISTORTION_1:

           /* derivative w.r.t. K1 */
           camera.nonlinear.radial1.K1 -= STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp1 ) );
           camera.nonlinear.radial1.K1 += 2.0F*STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp2 ) );
           camera.nonlinear.radial1.K1 -= STEP_SIZE;
           HCameraN[0].nonlinear.radial1.K1 = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.radial1.K1 = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* check derivatives of nonlinear parameters */
           for(ic = 0; ic < 2; ic++)
              cu_assert ( fabs(HCameraS[ic].nonlinear.radial1.K1-HCameraN[ic].nonlinear.radial1.K1) < DERIVATIVE_THRESHOLD );

           break;

         case GAN_RADIAL_DISTORTION_2:

           /* derivative w.r.t. K1 */
           camera.nonlinear.radial2.K1 -= STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp1 ) );
           camera.nonlinear.radial2.K1 += 2.0F*STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp2 ) );
           camera.nonlinear.radial2.K1 -= STEP_SIZE;
           HCameraN[0].nonlinear.radial2.K1 = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.radial2.K1 = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. K2 */
           camera.nonlinear.radial2.K2 -= STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp1 ) );
           camera.nonlinear.radial2.K2 += 2.0F*STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp2 ) );
           camera.nonlinear.radial2.K2 -= STEP_SIZE;
           HCameraN[0].nonlinear.radial2.K2 = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.radial2.K2 = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* check derivatives of nonlinear parameters */
           for(ic = 0; ic < 2; ic++)
              cu_assert ( fabs(HCameraS[ic].nonlinear.radial2.K1-HCameraN[ic].nonlinear.radial2.K1) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.radial2.K2-HCameraN[ic].nonlinear.radial2.K2) < DERIVATIVE_THRESHOLD );
           break;

         case GAN_RADIAL_DISTORTION_3:

           /* derivative w.r.t. K1 */
           camera.nonlinear.radial3.K1 -= STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp1 ) );
           camera.nonlinear.radial3.K1 += 2.0F*STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp2 ) );
           camera.nonlinear.radial3.K1 -= STEP_SIZE;
           HCameraN[0].nonlinear.radial3.K1 = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.radial3.K1 = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. K2 */
           camera.nonlinear.radial3.K2 -= STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp1 ) );
           camera.nonlinear.radial3.K2 += 2.0F*STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp2 ) );
           camera.nonlinear.radial3.K2 -= STEP_SIZE;
           HCameraN[0].nonlinear.radial3.K2 = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.radial3.K2 = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. K3 */
           camera.nonlinear.radial3.K3 -= STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp1 ) );
           camera.nonlinear.radial3.K3 += 2.0F*STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp2 ) );
           camera.nonlinear.radial3.K3 -= STEP_SIZE;
           HCameraN[0].nonlinear.radial3.K3 = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.radial3.K3 = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* check derivatives of nonlinear parameters */
           for(ic = 0; ic < 2; ic++)
              cu_assert ( fabs(HCameraS[ic].nonlinear.radial3.K1-HCameraN[ic].nonlinear.radial3.K1) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.radial3.K2-HCameraN[ic].nonlinear.radial3.K2) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.radial3.K3-HCameraN[ic].nonlinear.radial3.K3) < DERIVATIVE_THRESHOLD );
           break;

         case GAN_XY_DISTORTION_4:

           /* derivative w.r.t. xla */
           camera.nonlinear.xydist4.cxx -= STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp1 ) );
           camera.nonlinear.xydist4.cxx += 2.0F*STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp2 ) );
           camera.nonlinear.xydist4.cxx -= STEP_SIZE;
           HCameraN[0].nonlinear.xydist4.cxx = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.xydist4.cxx = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. cxy */
           camera.nonlinear.xydist4.cxy -= STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp1 ) );
           camera.nonlinear.xydist4.cxy += 2.0F*STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp2 ) );
           camera.nonlinear.xydist4.cxy -= STEP_SIZE;
           HCameraN[0].nonlinear.xydist4.cxy = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.xydist4.cxy = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. cyx */
           camera.nonlinear.xydist4.cyx -= STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp1 ) );
           camera.nonlinear.xydist4.cyx += 2.0F*STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp2 ) );
           camera.nonlinear.xydist4.cyx -= STEP_SIZE;
           HCameraN[0].nonlinear.xydist4.cyx = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.xydist4.cyx = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* derivative w.r.t. cyy */
           camera.nonlinear.xydist4.cyy -= STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp1 ) );
           camera.nonlinear.xydist4.cyy += 2.0F*STEP_SIZE;
           cu_assert ( gan_cameraf_project_point_q ( &camera, &pf,
                                                    &vtmp2 ) );
           camera.nonlinear.xydist4.cyy -= STEP_SIZE;
           HCameraN[0].nonlinear.xydist4.cyy = 0.5F*(vtmp2.x-vtmp1.x)*INVERSE_STEP_SIZE;
           HCameraN[1].nonlinear.xydist4.cyy = 0.5F*(vtmp2.y-vtmp1.y)*INVERSE_STEP_SIZE;

           /* check derivatives of nonlinear parameters */
           for(ic = 0; ic < 2; ic++)
              cu_assert ( fabs(HCameraS[ic].nonlinear.xydist4.cxx-HCameraN[ic].nonlinear.xydist4.cxx) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.xydist4.cxy-HCameraN[ic].nonlinear.xydist4.cxy) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.xydist4.cyx-HCameraN[ic].nonlinear.xydist4.cyx) < DERIVATIVE_THRESHOLD &&
                          fabs(HCameraS[ic].nonlinear.xydist4.cyy-HCameraN[ic].nonlinear.xydist4.cyy) < DERIVATIVE_THRESHOLD );
           break;

         default:
           cu_assert(0);
      }
   }

   return GAN_TRUE;
}

#undef STEP_SIZE
#undef INVERSE_STEP_SIZE
#undef DERIVATIVE_THRESHOLD

static Gan_Bool test_point_backprojection ( Gan_CameraType type )
{
   Gan_Camera camera;
   int i;
   Gan_Vector3 p[NO_POINTS];

   cu_assert ( build_camera ( type, &camera ) );
   switch ( type )
   {
      case GAN_LINEAR_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &linearp[i], &p[i] ) );
        break;

      case GAN_AFFINE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &affinep[i], &p[i] ));
        break;

      case GAN_RADIAL_DISTORTION_1:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &radial1p[i], &p[i] ));
        break;

      case GAN_RADIAL_DISTORTION_2:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &radial2p[i], &p[i] ));
        break;

      case GAN_RADIAL_DISTORTION_3:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &radial3p[i], &p[i] ));
        break;

      case GAN_STEREOGRAPHIC_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &stereop[i], &p[i] ) );
        break;

      case GAN_EQUIDISTANT_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &equidistp[i], &p[i]));
        break;

      case GAN_SINE_LAW_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &sinep[i], &p[i] ) );
        break;

      case GAN_EQUI_SOLID_ANGLE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &equisap[i], &p[i] ) );
        break;

      case GAN_XY_DISTORTION_4:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &xydist4p[i], &p[i] ));
        break;

      case GAN_RADIAL_DISTORTION_1_INV:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &radial1invp[i],
                                                        &p[i] ) );
        break;

      case GAN_CUBIC_BSPLINE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &cbsplinep[i], &p[i] ));
        break;

      case GAN_CUBIC_BSPLINE_CAMERA_INV:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_backproject_point_q ( &camera,
                                                        &cbsplineinvp[i], &p[i] ));
        break;

      default:
        cu_assert(0);
        return GAN_FALSE;
   }
   
   cu_assert ( compare_camera_coordinates ( p ) );
   return GAN_TRUE;
}

static Gan_Bool test_point_backprojection_f ( Gan_CameraType type )
{
   Gan_Camera_f camera;
   int i;
   Gan_Vector3_f p[NO_POINTS], pf;

   cu_assert ( build_camera_f ( type, &camera ) );
   switch ( type )
   {
      case GAN_LINEAR_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, linearp[i].x, linearp[i].y, linearp[i].z );
           cu_assert ( gan_cameraf_backproject_point_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_AFFINE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, affinep[i].x, affinep[i].y, affinep[i].z );
           cu_assert ( gan_cameraf_backproject_point_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_RADIAL_DISTORTION_1:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, radial1p[i].x, radial1p[i].y, radial1p[i].z );
           cu_assert ( gan_cameraf_backproject_point_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_RADIAL_DISTORTION_2:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, radial2p[i].x, radial2p[i].y, radial2p[i].z );
           cu_assert ( gan_cameraf_backproject_point_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_RADIAL_DISTORTION_3:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, radial3p[i].x, radial3p[i].y, radial3p[i].z );
           cu_assert ( gan_cameraf_backproject_point_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_STEREOGRAPHIC_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, stereop[i].x, stereop[i].y, stereop[i].z );
           cu_assert ( gan_cameraf_backproject_point_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_EQUIDISTANT_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, equidistp[i].x, equidistp[i].y, equidistp[i].z );
           cu_assert ( gan_cameraf_backproject_point_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_SINE_LAW_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, sinep[i].x, sinep[i].y, sinep[i].z );
           cu_assert ( gan_cameraf_backproject_point_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_EQUI_SOLID_ANGLE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, equisap[i].x, equisap[i].y, equisap[i].z );
           cu_assert ( gan_cameraf_backproject_point_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_XY_DISTORTION_4:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, xydist4p[i].x, xydist4p[i].y, xydist4p[i].z );
           cu_assert ( gan_cameraf_backproject_point_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_RADIAL_DISTORTION_1_INV:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, radial1invp[i].x, radial1invp[i].y, radial1invp[i].z );
           cu_assert ( gan_cameraf_backproject_point_q ( &camera, &pf, &p[i] ) );
        }
        break;

      default:
        cu_assert(0);
        return GAN_FALSE;
   }
   
   cu_assert ( compare_camera_coordinates_f ( p ) );
   return GAN_TRUE;
}

static Gan_Bool test_add_distortion ( Gan_CameraType type )
{
   Gan_Camera camera;
   int i;
   Gan_Vector3 p[NO_POINTS];

   cu_assert ( build_camera ( type, &camera ) );
   for ( i = NO_POINTS-1; i >= 0; i-- )
      cu_assert ( gan_camera_add_distortion_q ( &camera, &linearp[i], &p[i] ) );

   cu_assert ( compare_image_coordinates ( type, p ) );
   return GAN_TRUE;
}

static Gan_Bool test_add_distortion_f ( Gan_CameraType type )
{
   Gan_Camera_f camera;
   int i;
   Gan_Vector3_f p[NO_POINTS], pf;

   cu_assert ( build_camera_f ( type, &camera ) );
   for ( i = NO_POINTS-1; i >= 0; i-- )
   {
      (void)gan_vec3f_fill_q ( &pf, linearp[i].x, linearp[i].y, linearp[i].z );
      cu_assert ( gan_cameraf_add_distortion_q ( &camera, &pf, &p[i] ) );
   }

   cu_assert ( compare_image_coordinates_f ( type, p ) );
   return GAN_TRUE;
}

static Gan_Bool test_distortion_removal ( Gan_CameraType type )
{
   Gan_Camera camera;
   int i;
   Gan_Vector3 p[NO_POINTS];

   cu_assert ( build_camera ( type, &camera ) );
   switch ( type )
   {
      case GAN_LINEAR_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &linearp[i], &p[i] ) );
        break;

      case GAN_AFFINE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &affinep[i], &p[i] ));
        break;

      case GAN_RADIAL_DISTORTION_1:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &radial1p[i], &p[i] ));
        break;

      case GAN_RADIAL_DISTORTION_2:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &radial2p[i], &p[i] ));
        break;

      case GAN_RADIAL_DISTORTION_3:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &radial3p[i], &p[i] ));
        break;

      case GAN_STEREOGRAPHIC_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &stereop[i], &p[i] ) );
        break;

      case GAN_EQUIDISTANT_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &equidistp[i], &p[i]));
        break;

      case GAN_SINE_LAW_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &sinep[i], &p[i] ) );
        break;

      case GAN_EQUI_SOLID_ANGLE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &equisap[i], &p[i] ) );
        break;

      case GAN_XY_DISTORTION_4:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &xydist4p[i], &p[i] ));
        break;

      case GAN_RADIAL_DISTORTION_1_INV:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &radial1invp[i], &p[i] ));
        break;

      case GAN_CUBIC_BSPLINE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &cbsplinep[i], &p[i] ));
        break;

      case GAN_CUBIC_BSPLINE_CAMERA_INV:
        for ( i = NO_POINTS-1; i >= 0; i-- )
           cu_assert ( gan_camera_remove_distortion_q ( &camera, &cbsplineinvp[i], &p[i] ));
        break;

      default:
        cu_assert(0);
        return GAN_FALSE;
   }
   
   cu_assert ( compare_image_coordinates ( GAN_LINEAR_CAMERA, p ) );
   return GAN_TRUE;
}

static Gan_Bool test_distortion_removal_f ( Gan_CameraType type )
{
   Gan_Camera_f camera;
   int i;
   Gan_Vector3_f p[NO_POINTS], pf;

   cu_assert ( build_camera_f ( type, &camera ) );
   switch ( type )
   {
      case GAN_LINEAR_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, linearp[i].x, linearp[i].y, linearp[i].z );
           cu_assert ( gan_cameraf_remove_distortion_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_AFFINE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, linearp[i].x, linearp[i].y, linearp[i].z );
           cu_assert ( gan_cameraf_remove_distortion_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_RADIAL_DISTORTION_1:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, radial1p[i].x, radial1p[i].y, radial1p[i].z );
           cu_assert ( gan_cameraf_remove_distortion_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_RADIAL_DISTORTION_2:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, radial2p[i].x, radial2p[i].y, radial2p[i].z );
           cu_assert ( gan_cameraf_remove_distortion_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_RADIAL_DISTORTION_3:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf,
                                 radial3p[i].x, radial3p[i].y, radial3p[i].z );
           cu_assert ( gan_cameraf_remove_distortion_q ( &camera,
                                                         &pf, &p[i] ) );
        }
        break;

      case GAN_STEREOGRAPHIC_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, stereop[i].x, stereop[i].y, stereop[i].z );
           cu_assert ( gan_cameraf_remove_distortion_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_EQUIDISTANT_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, equidistp[i].x, equidistp[i].y, equidistp[i].z );
           cu_assert ( gan_cameraf_remove_distortion_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_SINE_LAW_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf,
                                    sinep[i].x, sinep[i].y, sinep[i].z );
           cu_assert ( gan_cameraf_remove_distortion_q ( &camera,
                                                         &pf, &p[i] ) );
        }
        break;

      case GAN_EQUI_SOLID_ANGLE_CAMERA:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, equisap[i].x, equisap[i].y, equisap[i].z );
           cu_assert ( gan_cameraf_remove_distortion_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_XY_DISTORTION_4:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, xydist4p[i].x, xydist4p[i].y, xydist4p[i].z );
           cu_assert ( gan_cameraf_remove_distortion_q ( &camera, &pf, &p[i] ) );
        }
        break;

      case GAN_RADIAL_DISTORTION_1_INV:
        for ( i = NO_POINTS-1; i >= 0; i-- )
        {
           (void)gan_vec3f_fill_q ( &pf, radial1invp[i].x, radial1invp[i].y, radial1invp[i].z );
           cu_assert ( gan_cameraf_remove_distortion_q ( &camera, &pf, &p[i] ) );
        }
        break;

      default:
        cu_assert(0);
        return GAN_FALSE;
   }
   
   cu_assert ( compare_image_coordinates_f ( GAN_LINEAR_CAMERA, p ) );
   return GAN_TRUE;
}

/* Tests the camera functions */
static Gan_Bool run_test(void)
{
   /* test point projection in double precision */
   cu_assert ( test_point_projection(GAN_LINEAR_CAMERA) );
   cu_assert ( test_point_projection(GAN_AFFINE_CAMERA) );
   cu_assert ( test_point_projection(GAN_RADIAL_DISTORTION_1) );
   cu_assert ( test_point_projection(GAN_RADIAL_DISTORTION_2) );
   cu_assert ( test_point_projection(GAN_RADIAL_DISTORTION_3) );
   cu_assert ( test_point_projection(GAN_STEREOGRAPHIC_CAMERA) );
   cu_assert ( test_point_projection(GAN_EQUIDISTANT_CAMERA) );
   cu_assert ( test_point_projection(GAN_SINE_LAW_CAMERA) );
   cu_assert ( test_point_projection(GAN_EQUI_SOLID_ANGLE_CAMERA) );
   cu_assert ( test_point_projection(GAN_XY_DISTORTION_4) );
   cu_assert ( test_point_projection(GAN_RADIAL_DISTORTION_1_INV) );
   cu_assert ( test_point_projection(GAN_CUBIC_BSPLINE_CAMERA) );
   cu_assert ( test_point_projection(GAN_CUBIC_BSPLINE_CAMERA_INV) );

   /* test point projection in single precision */
   cu_assert ( test_point_projection_f(GAN_LINEAR_CAMERA) );
   cu_assert ( test_point_projection_f(GAN_AFFINE_CAMERA) );
   cu_assert ( test_point_projection_f(GAN_RADIAL_DISTORTION_1) );
   cu_assert ( test_point_projection_f(GAN_RADIAL_DISTORTION_2) );
   cu_assert ( test_point_projection_f(GAN_RADIAL_DISTORTION_3) );
   cu_assert ( test_point_projection_f(GAN_STEREOGRAPHIC_CAMERA) );
   cu_assert ( test_point_projection_f(GAN_EQUIDISTANT_CAMERA) );
   cu_assert ( test_point_projection_f(GAN_SINE_LAW_CAMERA) );
   cu_assert ( test_point_projection_f(GAN_EQUI_SOLID_ANGLE_CAMERA) );
   cu_assert ( test_point_projection_f(GAN_XY_DISTORTION_4) );
   cu_assert ( test_point_projection_f(GAN_RADIAL_DISTORTION_1_INV) );

   /* test projection derivatives in double precision */
   cu_assert ( test_point_projection_derivatives(GAN_AFFINE_CAMERA) );
   cu_assert ( test_point_projection_derivatives(GAN_RADIAL_DISTORTION_1) );
   cu_assert ( test_point_projection_derivatives(GAN_RADIAL_DISTORTION_2) );
   cu_assert ( test_point_projection_derivatives(GAN_RADIAL_DISTORTION_3) );
   cu_assert ( test_point_projection_derivatives(GAN_XY_DISTORTION_4) );
   cu_assert ( test_point_projection_derivatives(GAN_CUBIC_BSPLINE_CAMERA) );

   /* test projection derivatives in single precision */
   cu_assert ( test_point_projection_derivatives_f(GAN_AFFINE_CAMERA) );
   cu_assert ( test_point_projection_derivatives_f(GAN_RADIAL_DISTORTION_1) );
   cu_assert ( test_point_projection_derivatives_f(GAN_RADIAL_DISTORTION_2) );
   cu_assert ( test_point_projection_derivatives_f(GAN_RADIAL_DISTORTION_3) );
   cu_assert ( test_point_projection_derivatives_f(GAN_XY_DISTORTION_4) );

   /* test point back-projection in double precision */
   cu_assert ( test_point_backprojection(GAN_LINEAR_CAMERA) );
   cu_assert ( test_point_backprojection(GAN_AFFINE_CAMERA) );
   cu_assert ( test_point_backprojection(GAN_RADIAL_DISTORTION_1) );
   cu_assert ( test_point_backprojection(GAN_RADIAL_DISTORTION_2) );
   cu_assert ( test_point_backprojection(GAN_RADIAL_DISTORTION_3) );
   cu_assert ( test_point_backprojection(GAN_STEREOGRAPHIC_CAMERA) );
   cu_assert ( test_point_backprojection(GAN_EQUIDISTANT_CAMERA) );
   cu_assert ( test_point_backprojection(GAN_SINE_LAW_CAMERA) );
   cu_assert ( test_point_backprojection(GAN_EQUI_SOLID_ANGLE_CAMERA) );
   cu_assert ( test_point_backprojection(GAN_XY_DISTORTION_4) );
   cu_assert ( test_point_backprojection(GAN_RADIAL_DISTORTION_1_INV) );
   cu_assert ( test_point_backprojection(GAN_CUBIC_BSPLINE_CAMERA) );
   cu_assert ( test_point_backprojection(GAN_CUBIC_BSPLINE_CAMERA_INV) );

   /* test point back-projection in single precision */
   cu_assert ( test_point_backprojection_f(GAN_LINEAR_CAMERA) );
   cu_assert ( test_point_backprojection_f(GAN_AFFINE_CAMERA) );
   cu_assert ( test_point_backprojection_f(GAN_RADIAL_DISTORTION_1) );
   cu_assert ( test_point_backprojection_f(GAN_RADIAL_DISTORTION_2) );
   cu_assert ( test_point_backprojection_f(GAN_RADIAL_DISTORTION_3) );
   cu_assert ( test_point_backprojection_f(GAN_STEREOGRAPHIC_CAMERA) );
   cu_assert ( test_point_backprojection_f(GAN_EQUIDISTANT_CAMERA) );
   cu_assert ( test_point_backprojection_f(GAN_SINE_LAW_CAMERA) );
   cu_assert ( test_point_backprojection_f(GAN_EQUI_SOLID_ANGLE_CAMERA) );
#if 0
   cu_assert ( test_point_backprojection_f(GAN_XY_DISTORTION_4) );
#endif
   cu_assert ( test_point_backprojection_f(GAN_RADIAL_DISTORTION_1_INV) );

   /* test adding distortion to image points in double precision */
   cu_assert ( test_add_distortion(GAN_LINEAR_CAMERA) );
#if 0
   cu_assert ( test_add_distortion(GAN_AFFINE_CAMERA) );
#endif
   cu_assert ( test_add_distortion(GAN_RADIAL_DISTORTION_1) );
   cu_assert ( test_add_distortion(GAN_RADIAL_DISTORTION_2) );
   cu_assert ( test_add_distortion(GAN_RADIAL_DISTORTION_3) );
#if 0
   cu_assert ( test_add_distortion(GAN_STEREOGRAPHIC_CAMERA) );
   cu_assert ( test_add_distortion(GAN_EQUIDISTANT_CAMERA) );
   cu_assert ( test_add_distortion(GAN_SINE_LAW_CAMERA) );
   cu_assert ( test_add_distortion(GAN_EQUI_SOLID_ANGLE_CAMERA) );
#endif
   cu_assert ( test_add_distortion(GAN_XY_DISTORTION_4) );
   cu_assert ( test_add_distortion(GAN_RADIAL_DISTORTION_1_INV) );
   cu_assert ( test_add_distortion(GAN_CUBIC_BSPLINE_CAMERA) );
   cu_assert ( test_add_distortion(GAN_CUBIC_BSPLINE_CAMERA_INV) );

   /* test adding distortion to image points in single precision */
   cu_assert ( test_add_distortion_f(GAN_LINEAR_CAMERA) );
#if 0
   cu_assert ( test_add_distortion_f(GAN_AFFINE_CAMERA) );
#endif
   cu_assert ( test_add_distortion_f(GAN_RADIAL_DISTORTION_1) );
   cu_assert ( test_add_distortion_f(GAN_RADIAL_DISTORTION_2) );
   cu_assert ( test_add_distortion_f(GAN_RADIAL_DISTORTION_3) );
#if 0
   cu_assert ( test_add_distortion_f(GAN_STEREOGRAPHIC_CAMERA) );
   cu_assert ( test_add_distortion_f(GAN_EQUIDISTANT_CAMERA) );
   cu_assert ( test_add_distortion_f(GAN_SINE_LAW_CAMERA) );
   cu_assert ( test_add_distortion_f(GAN_EQUI_SOLID_ANGLE_CAMERA) );
#endif
   cu_assert ( test_add_distortion_f(GAN_XY_DISTORTION_4) );
   cu_assert ( test_add_distortion_f(GAN_RADIAL_DISTORTION_1_INV) );

   /* test removing distortion to image points in double precision */
   cu_assert ( test_distortion_removal(GAN_LINEAR_CAMERA) );
#if 0
   cu_assert ( test_distortion_removal(GAN_AFFINE_CAMERA) );
#endif
   cu_assert ( test_distortion_removal(GAN_RADIAL_DISTORTION_1) );
   cu_assert ( test_distortion_removal(GAN_RADIAL_DISTORTION_2) );
   cu_assert ( test_distortion_removal(GAN_RADIAL_DISTORTION_3) );
#if 0
   cu_assert ( test_distortion_removal(GAN_STEREOGRAPHIC_CAMERA) );
   cu_assert ( test_distortion_removal(GAN_EQUIDISTANT_CAMERA) );
   cu_assert ( test_distortion_removal(GAN_SINE_LAW_CAMERA) );
   cu_assert ( test_distortion_removal(GAN_EQUI_SOLID_ANGLE_CAMERA) );
#endif
   cu_assert ( test_distortion_removal(GAN_XY_DISTORTION_4) );
   cu_assert ( test_distortion_removal(GAN_RADIAL_DISTORTION_1_INV) );
   cu_assert ( test_distortion_removal(GAN_CUBIC_BSPLINE_CAMERA) );
   cu_assert ( test_distortion_removal(GAN_CUBIC_BSPLINE_CAMERA_INV) );

   /* test removing distortion to image points in single precision */
   cu_assert ( test_distortion_removal_f(GAN_LINEAR_CAMERA) );
#if 0
   cu_assert ( test_distortion_removal_f(GAN_AFFINE_CAMERA) );
#endif
   cu_assert ( test_distortion_removal_f(GAN_RADIAL_DISTORTION_1) );
   cu_assert ( test_distortion_removal_f(GAN_RADIAL_DISTORTION_2) );
   cu_assert ( test_distortion_removal_f(GAN_RADIAL_DISTORTION_3) );
#if 0
   cu_assert ( test_distortion_removal_f(GAN_STEREOGRAPHIC_CAMERA) );
   cu_assert ( test_distortion_removal_f(GAN_EQUIDISTANT_CAMERA) );
   cu_assert ( test_distortion_removal_f(GAN_SINE_LAW_CAMERA) );
   cu_assert ( test_distortion_removal_f(GAN_EQUI_SOLID_ANGLE_CAMERA) );
#endif
   cu_assert ( test_distortion_removal_f(GAN_XY_DISTORTION_4) );
   cu_assert ( test_distortion_removal_f(GAN_RADIAL_DISTORTION_1_INV) );

   /* success */
   return GAN_TRUE;
}

#ifdef CAMERA_TEST_MAIN

int main ( int argc, char *argv[] )
{
   /* turn on error tracing */
   gan_err_set_trace ( GAN_ERR_TRACE_ON );

   setup_test();
   if ( run_test() )
      printf ( "Tests ran successfully!\n" );
   else
   {
      printf ( "At least one test failed\n" );
      printf ( "Gandalf errors:\n" );
      gan_err_default_reporter();
   }
   
   teardown_test();
   gan_heap_report(NULL);
   return 0;
}

#else

/* This function registers the unit tests to a cUnit_test_suite. */
cUnit_test_suite *camera_test_build_suite(void)
{
   cUnit_test_suite *sp;

   /* Get a new test session */
   sp = cUnit_new_test_suite("camera_test suite");

   cUnit_add_test(sp, "camera_test", run_test);

   /* register a setup and teardown on the test suite 'camera_test' */
   if (cUnit_register_setup(sp, setup_test) != GAN_TRUE)
      printf("Error while setting up test suite camera_test");

   if (cUnit_register_teardown(sp, teardown_test) != GAN_TRUE)
      printf("Error while tearing down test suite camera_test");

   return( sp );
}

#endif /* #ifdef CAMERA_TEST_vMAIN */
