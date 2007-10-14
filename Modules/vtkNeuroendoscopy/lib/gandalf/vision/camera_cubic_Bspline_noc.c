/* do not compile this file separately */

#include <gandalf/common/numerics.h>

#define CZH camera->zh
#define CFX camera->fx
#define CFY camera->fy
#define CX0 camera->x0
#define CY0 camera->y0
#define SKEW camera->nonlinear.cbspline.K.xy
#define KYX camera->nonlinear.cbspline.K.yx
#define KZX camera->nonlinear.cbspline.K.zx
#define KZY camera->nonlinear.cbspline.K.zy
#define WEIGHT camera->nonlinear.cbspline.weight

/* minimum/maximum number of iterations to apply when applying inverse distortion */
#define MIN_UND_ITERATIONS   6
#define MAX_UND_ITERATIONS 100

#if 1
/* basis function contribution for control point before t value */
#define BASIS_V_BEFORE(t) (CAM_ONE_SIXTH*(CAM_THREE*(t)*(t)*((t) - CAM_TWO) + CAM_FOUR))
#define BASIS_V_DERIV_BEFORE(t) (CAM_HALF*(t)*(CAM_THREE*(t) - CAM_FOUR))

/* basis function contribution for control point after the t value */
#define BASIS_V_AFTER(t) (CAM_ONE_SIXTH*(-CAM_THREE*(CAM_ONE-(t))*(CAM_ONE-(t)*(t)) + CAM_FOUR))
#define BASIS_V_DERIV_AFTER(t) (-CAM_HALF*(t)*(CAM_THREE*(t) - CAM_TWO) + CAM_HALF)

/* basis function contribution for control point two before the t value */
#define BASIS_V_BEFORE2(t) (CAM_ONE_SIXTH*(CAM_ONE - (t)*(CAM_THREE - (t)*(CAM_THREE - (t)))))
#define BASIS_V_DERIV_BEFORE2(t) (-CAM_HALF*(CAM_ONE - (t)*(CAM_TWO - (t))))

/* basis function contribution for control point two after the t value */
#define BASIS_V_AFTER2(t) (CAM_ONE_SIXTH*(t)*(t)*(t))
#define BASIS_V_DERIV_AFTER2(t) (CAM_HALF*(t)*(t))
#else
/* basis function contribution for control point before t value */
static double BASIS_V_BEFORE(double t)
{
   return (CAM_ONE_SIXTH*(CAM_THREE*t*t*(t - CAM_TWO) + CAM_FOUR));
}

static double BASIS_V_DERIV_BEFORE(double t)
{
   return (CAM_HALF*t*(CAM_THREE*t - CAM_FOUR));
}


/* basis function contribution for control point after the t value */
static double BASIS_V_AFTER(double t)
{
   return (CAM_ONE_SIXTH*(-CAM_THREE*(CAM_ONE-t)*(CAM_ONE-t*t) + CAM_FOUR));
}

static double BASIS_V_DERIV_AFTER(double t)
{
   return (-CAM_HALF*t*(CAM_THREE*t - CAM_TWO) + CAM_HALF);
}

/* basis function contribution for control point two before the t value */
static double BASIS_V_BEFORE2(double t)
{
   return (CAM_ONE_SIXTH*(CAM_ONE - t*(CAM_THREE - t*(CAM_THREE - t))));
}

static double BASIS_V_DERIV_BEFORE2(double t)
{
   return (-CAM_HALF*(CAM_ONE - t*(CAM_TWO - t)));
}

/* basis function contribution for control point two after the t value */
static double BASIS_V_AFTER2(double t)
{
   return (CAM_ONE_SIXTH*t*t*t);
}

static double BASIS_V_DERIV_AFTER2(double t)
{
   return (CAM_HALF*t*t);
}
#endif

static void
 fill_camera_with_zeros(CAM_STRUCT *camera)
{
   camera->zh = CAM_ZERO;
   camera->fx = CAM_ZERO;
   camera->fy = CAM_ZERO;
   camera->x0 = CAM_ZERO;
   camera->y0 = CAM_ZERO;
   camera->type = GAN_CUBIC_BSPLINE_CAMERA;
   (void)CAM_MAT33_ZERO_Q(&camera->nonlinear.cbspline.K);
   (void)CAM_MAT33_ZERO_Q(&camera->nonlinear.cbspline.Kinv);
   CAM_FILL_ARRAY_FLOAT(&camera->nonlinear.cbspline.weight[0][0].x, 2*(1+(1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL))*(1+(1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)), 1, CAM_ZERO);
}

/* apply B-spline warp */
static Gan_Bool
 point_cubic_Bspline_warp ( CAM_VECTOR2 *X,
                            const CAM_VECTOR2 weight[1 + (1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)][1 + (1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)],
                            CAM_VECTOR2 *Xp, CAM_MATRIX22 *HX, CAM_STRUCT HC[2], int *error_code )
{
   const CAM_VECTOR2 *wptr;
   CAM_FLOAT XZ, YZ, dx=CAM_ZERO, dy=CAM_ZERO;
   int xblock, yblock, block_limit = 1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL;
   CAM_FLOAT block_scale;
   CAM_FLOAT B2weightx=CAM_ZERO, B2weighty=CAM_ZERO, Bweightx=CAM_ZERO, Bweighty=CAM_ZERO, Aweightx=CAM_ZERO, Aweighty=CAM_ZERO, A2weightx=CAM_ZERO, A2weighty=CAM_ZERO;
   CAM_FLOAT B2derivx=CAM_ZERO, B2derivy=CAM_ZERO, Bderivx=CAM_ZERO, Bderivy=CAM_ZERO, Aderivx=CAM_ZERO, Aderivy=CAM_ZERO, A2derivx=CAM_ZERO, A2derivy=CAM_ZERO;
   CAM_FLOAT block_lim2;
   
   /* determine which block we are in */
   block_lim2 = (CAM_FLOAT)(block_limit-2);
   XZ = CAM_ONE + block_lim2*(X->x + CAM_HALF);
   YZ = CAM_ONE + block_lim2*(X->y + CAM_HALF);
   if(XZ < -CAM_TWO || YZ < -CAM_TWO) /* outside basis function range */
   {
      *Xp = *X;
      if(HX != NULL)
         (void)CAM_MAT22_IDENT_Q(HX);

      if(HC != NULL)
      {
         fill_camera_with_zeros(&HC[0]);
         fill_camera_with_zeros(&HC[1]);
      }

      return GAN_TRUE;
   }

   /* set XZ,YZ to values local to block */
   if(XZ < CAM_ZERO)
      xblock = (int)(XZ + CAM_TWO) - 2;
   else
      xblock = (int)XZ;

   if(YZ < CAM_ZERO)
      yblock = (int)(YZ + CAM_TWO) - 2;
   else
      yblock = (int)YZ;

   if(xblock > block_limit+1 || yblock > block_limit+1) /* outside basis function range */
   {
      
      *Xp = *X;
      if(HX != NULL)
         (void)CAM_MAT22_IDENT_Q(HX);

      if(HC != NULL)
      {
         fill_camera_with_zeros(&HC[0]);
         fill_camera_with_zeros(&HC[1]);
      }

      return GAN_TRUE;
   }
   
   XZ -= (CAM_FLOAT)xblock;
   YZ -= (CAM_FLOAT)yblock;

   /* evaluate basis functions, for blocks numbered 0,1,2,3, where the point XZ,YZ is in block 1,1 */
   if(xblock > 0) B2weightx = BASIS_V_BEFORE2(XZ);
   if(yblock > 0) B2weighty = BASIS_V_BEFORE2(YZ);
   if(xblock >= 0) Bweightx = BASIS_V_BEFORE(XZ);
   if(yblock >= 0) Bweighty = BASIS_V_BEFORE(YZ);
   if(xblock < block_limit) Aweightx = BASIS_V_AFTER(XZ);
   if(yblock < block_limit) Aweighty = BASIS_V_AFTER(YZ);
   if(xblock < block_limit-1) A2weightx = BASIS_V_AFTER2(XZ);
   if(yblock < block_limit-1) A2weighty = BASIS_V_AFTER2(YZ);

   if(HX != NULL)
   {
      if(xblock > 0) B2derivx = BASIS_V_DERIV_BEFORE2(XZ);
      if(yblock > 0) B2derivy = BASIS_V_DERIV_BEFORE2(YZ);
      if(xblock >= 0) Bderivx = BASIS_V_DERIV_BEFORE(XZ);
      if(yblock >= 0) Bderivy = BASIS_V_DERIV_BEFORE(YZ);
      if(xblock < block_limit) Aderivx = BASIS_V_DERIV_AFTER(XZ);
      if(yblock < block_limit) Aderivy = BASIS_V_DERIV_AFTER(YZ);
      if(xblock < block_limit-1) A2derivx = BASIS_V_DERIV_AFTER2(XZ);
      if(yblock < block_limit-1) A2derivy = BASIS_V_DERIV_AFTER2(YZ);

      /* initialize Jacobian to zero */
      (void)CAM_MAT22_ZERO_Q(HX);
   }

   /* add term and Jacobians for each block */
   if(xblock > 0)
   {
      if(yblock > 0)
      {
         wptr = &weight[yblock-1][xblock-1];
         dx += wptr->x*B2weightx*B2weighty;
         dy += wptr->y*B2weightx*B2weighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*B2derivx*B2weighty;
            HX->xy += wptr->x*B2weightx*B2derivy;
            HX->yx += wptr->y*B2derivx*B2weighty;
            HX->yy += wptr->y*B2weightx*B2derivy;
         }
      }

      if(yblock >= 0 && yblock <= block_limit)
      {
         wptr = &weight[yblock][xblock-1];
         dx += wptr->x*B2weightx*Bweighty;
         dy += wptr->y*B2weightx*Bweighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*B2derivx*Bweighty;
            HX->xy += wptr->x*B2weightx*Bderivy;
            HX->yx += wptr->y*B2derivx*Bweighty;
            HX->yy += wptr->y*B2weightx*Bderivy;
         }
      }

      if(yblock > -2 && yblock < block_limit)
      {
         wptr = &weight[yblock+1][xblock-1];
         dx += wptr->x*B2weightx*Aweighty;
         dy += wptr->y*B2weightx*Aweighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*B2derivx*Aweighty;
            HX->xy += wptr->x*B2weightx*Aderivy;
            HX->yx += wptr->y*B2derivx*Aweighty;
            HX->yy += wptr->y*B2weightx*Aderivy;
         }
      }

      if(yblock < block_limit-1)
      {
         wptr = &weight[yblock+2][xblock-1];
         dx += wptr->x*B2weightx*A2weighty;
         dy += wptr->y*B2weightx*A2weighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*B2derivx*A2weighty;
            HX->xy += wptr->x*B2weightx*A2derivy;
            HX->yx += wptr->y*B2derivx*A2weighty;
            HX->yy += wptr->y*B2weightx*A2derivy;
         }
      }
   }

   if(xblock >= 0 && xblock <= block_limit)
   {
      if(yblock > 0)
      {
         wptr = &weight[yblock-1][xblock];
         dx += wptr->x*Bweightx*B2weighty;
         dy += wptr->y*Bweightx*B2weighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*Bderivx*B2weighty;
            HX->xy += wptr->x*Bweightx*B2derivy;
            HX->yx += wptr->y*Bderivx*B2weighty;
            HX->yy += wptr->y*Bweightx*B2derivy;
         }
      }

      if(yblock >= 0 && yblock <= block_limit)
      {
         wptr = &weight[yblock][xblock];
         dx += wptr->x*Bweightx*Bweighty;
         dy += wptr->y*Bweightx*Bweighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*Bderivx*Bweighty;
            HX->xy += wptr->x*Bweightx*Bderivy;
            HX->yx += wptr->y*Bderivx*Bweighty;
            HX->yy += wptr->y*Bweightx*Bderivy;
         }
      }
      
      if(yblock > -2 && yblock < block_limit)
      {
         wptr = &weight[yblock+1][xblock];
         dx += wptr->x*Bweightx*Aweighty;
         dy += wptr->y*Bweightx*Aweighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*Bderivx*Aweighty;
            HX->xy += wptr->x*Bweightx*Aderivy;
            HX->yx += wptr->y*Bderivx*Aweighty;
            HX->yy += wptr->y*Bweightx*Aderivy;
         }
      }

      if(yblock < block_limit-1)
      {
         wptr = &weight[yblock+2][xblock];
         dx += wptr->x*Bweightx*A2weighty;
         dy += wptr->y*Bweightx*A2weighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*Bderivx*A2weighty;
            HX->xy += wptr->x*Bweightx*A2derivy;
            HX->yx += wptr->y*Bderivx*A2weighty;
            HX->yy += wptr->y*Bweightx*A2derivy;
         }
      }
   }

   if(xblock > -2 && xblock < block_limit)
   {
      if(yblock > 0)
      {
         wptr = &weight[yblock-1][xblock+1];
         dx += wptr->x*Aweightx*B2weighty;
         dy += wptr->y*Aweightx*B2weighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*Aderivx*B2weighty;
            HX->xy += wptr->x*Aweightx*B2derivy;
            HX->yx += wptr->y*Aderivx*B2weighty;
            HX->yy += wptr->y*Aweightx*B2derivy;
         }
      }

      if(yblock >= 0 && yblock <= block_limit)
      {
         wptr = &weight[yblock][xblock+1];
         dx += wptr->x*Aweightx*Bweighty;
         dy += wptr->y*Aweightx*Bweighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*Aderivx*Bweighty;
            HX->xy += wptr->x*Aweightx*Bderivy;
            HX->yx += wptr->y*Aderivx*Bweighty;
            HX->yy += wptr->y*Aweightx*Bderivy;
         }
      }

      if(yblock > -2 && yblock < block_limit)
      {
         wptr = &weight[yblock+1][xblock+1];
         dx += wptr->x*Aweightx*Aweighty;
         dy += wptr->y*Aweightx*Aweighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*Aderivx*Aweighty;
            HX->xy += wptr->x*Aweightx*Aderivy;
            HX->yx += wptr->y*Aderivx*Aweighty;
            HX->yy += wptr->y*Aweightx*Aderivy;
         }
      }

      if(yblock < block_limit-1)
      {
         wptr = &weight[yblock+2][xblock+1];
         dx += wptr->x*Aweightx*A2weighty;
         dy += wptr->y*Aweightx*A2weighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*Aderivx*A2weighty;
            HX->xy += wptr->x*Aweightx*A2derivy;
            HX->yx += wptr->y*Aderivx*A2weighty;
            HX->yy += wptr->y*Aweightx*A2derivy;
         }
      }
   }

   if(xblock < block_limit-1)
   {
      if(yblock > 0)
      {
         wptr = &weight[yblock-1][xblock+2];
         dx += wptr->x*A2weightx*B2weighty;
         dy += wptr->y*A2weightx*B2weighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*A2derivx*B2weighty;
            HX->xy += wptr->x*A2weightx*B2derivy;
            HX->yx += wptr->y*A2derivx*B2weighty;
            HX->yy += wptr->y*A2weightx*B2derivy;
         }
      }

      if(yblock >= 0 && yblock <= block_limit)
      {
         wptr = &weight[yblock][xblock+2];
         dx += wptr->x*A2weightx*Bweighty;
         dy += wptr->y*A2weightx*Bweighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*A2derivx*Bweighty;
            HX->xy += wptr->x*A2weightx*Bderivy;
            HX->yx += wptr->y*A2derivx*Bweighty;
            HX->yy += wptr->y*A2weightx*Bderivy;
         }
      }

      if(yblock > -2 && yblock < block_limit)
      {
         wptr = &weight[yblock+1][xblock+2];
         dx += wptr->x*A2weightx*Aweighty;
         dy += wptr->y*A2weightx*Aweighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*A2derivx*Aweighty;
            HX->xy += wptr->x*A2weightx*Aderivy;
            HX->yx += wptr->y*A2derivx*Aweighty;
            HX->yy += wptr->y*A2weightx*Aderivy;
         }
      }

      if(yblock < block_limit-1)
      {
         wptr = &weight[yblock+2][xblock+2];
         dx += wptr->x*A2weightx*A2weighty;
         dy += wptr->y*A2weightx*A2weighty;
         if ( HX != NULL )
         {
            HX->xx += wptr->x*A2derivx*A2weighty;
            HX->xy += wptr->x*A2weightx*A2derivy;
            HX->yx += wptr->y*A2derivx*A2weighty;
            HX->yy += wptr->y*A2weightx*A2derivy;
         }
      }
   }

#ifdef DEBUG_ALL
   printf("X=%f Y=%f\n", X->x, X->y);
   printf("  wx\t(%d)=%f\t(%d)=%f\t(%d)=%f\t(%d)=%f\n", xblock-1, B2weightx, xblock, Bweightx, xblock+1, Aweightx, xblock+2, A2weightx);
   printf("  wy\t(%d)=%f\t(%d)=%f\t(%d)=%f\t(%d)=%f\n", yblock-1, B2weighty, yblock, Bweighty, yblock+1, Aweighty, yblock+2, A2weighty);
#endif   
   if(HC != NULL)
   {
      fill_camera_with_zeros(&HC[0]);
      fill_camera_with_zeros(&HC[1]);

      if(xblock > 0)
      {
         if(yblock > 0)
         {
            HC[0].nonlinear.cbspline.weight[yblock-1][xblock-1].x =
            HC[1].nonlinear.cbspline.weight[yblock-1][xblock-1].y = B2weightx*B2weighty;
         }

         if(yblock >= 0 && yblock <= block_limit)
         {
            HC[0].nonlinear.cbspline.weight[yblock][xblock-1].x =
            HC[1].nonlinear.cbspline.weight[yblock][xblock-1].y = B2weightx*Bweighty;
         }

         if(yblock > -2 && yblock < block_limit)
         {
            HC[0].nonlinear.cbspline.weight[yblock+1][xblock-1].x =
            HC[1].nonlinear.cbspline.weight[yblock+1][xblock-1].y = B2weightx*Aweighty;
         }

         if(yblock < block_limit-1)
         {
            HC[0].nonlinear.cbspline.weight[yblock+2][xblock-1].x =
            HC[1].nonlinear.cbspline.weight[yblock+2][xblock-1].y = B2weightx*A2weighty;
         }
      }

      if(xblock >= 0 && xblock <= block_limit)
      {
         if(yblock > 0)
         {
            HC[0].nonlinear.cbspline.weight[yblock-1][xblock].x =
            HC[1].nonlinear.cbspline.weight[yblock-1][xblock].y = Bweightx*B2weighty;
         }

         if(yblock >= 0 && yblock <= block_limit)
         {
            HC[0].nonlinear.cbspline.weight[yblock][xblock].x =
            HC[1].nonlinear.cbspline.weight[yblock][xblock].y = Bweightx*Bweighty;
         }

         if(yblock > -2 && yblock < block_limit)
         {
            HC[0].nonlinear.cbspline.weight[yblock+1][xblock].x =
            HC[1].nonlinear.cbspline.weight[yblock+1][xblock].y = Bweightx*Aweighty;
         }

         if(yblock < block_limit-1)
         {
            HC[0].nonlinear.cbspline.weight[yblock+2][xblock].x =
            HC[1].nonlinear.cbspline.weight[yblock+2][xblock].y = Bweightx*A2weighty;
         }
      }

      if(xblock > -2 && xblock < block_limit)
      {
         if(yblock > 0)
         {
            HC[0].nonlinear.cbspline.weight[yblock-1][xblock+1].x =
            HC[1].nonlinear.cbspline.weight[yblock-1][xblock+1].y = Aweightx*B2weighty;
         }

         if(yblock >= 0 && yblock <= block_limit)
         {
            HC[0].nonlinear.cbspline.weight[yblock][xblock+1].x =
            HC[1].nonlinear.cbspline.weight[yblock][xblock+1].y = Aweightx*Bweighty;
         }

         if(yblock > -2 && yblock < block_limit)
         {
            HC[0].nonlinear.cbspline.weight[yblock+1][xblock+1].x =
            HC[1].nonlinear.cbspline.weight[yblock+1][xblock+1].y = Aweightx*Aweighty;
         }

         if(yblock < block_limit-1)
         {
            HC[0].nonlinear.cbspline.weight[yblock+2][xblock+1].x =
            HC[1].nonlinear.cbspline.weight[yblock+2][xblock+1].y = Aweightx*A2weighty;
         }
      }

      if(xblock < block_limit-1)
      {
         if(yblock > 0)
         {
            HC[0].nonlinear.cbspline.weight[yblock-1][xblock+2].x =
            HC[1].nonlinear.cbspline.weight[yblock-1][xblock+2].y = A2weightx*B2weighty;
         }

         if(yblock >= 0 && yblock <= block_limit)
         {

            HC[0].nonlinear.cbspline.weight[yblock][xblock+2].x =
            HC[1].nonlinear.cbspline.weight[yblock][xblock+2].y = A2weightx*Bweighty;
         }

         if(yblock > -2 && yblock < block_limit)
         {
            HC[0].nonlinear.cbspline.weight[yblock+1][xblock+2].x =
            HC[1].nonlinear.cbspline.weight[yblock+1][xblock+2].y = A2weightx*Aweighty;
         }

         if(yblock < block_limit-1)
         {
            HC[0].nonlinear.cbspline.weight[yblock+2][xblock+2].x =
            HC[1].nonlinear.cbspline.weight[yblock+2][xblock+2].y = A2weightx*A2weighty;
         }
      }
   }

   /* adjust for initialize position and derivatives */
   block_scale = CAM_ONE/block_lim2;
   Xp->x = X->x + block_scale*dx;
   Xp->y = X->y + block_scale*dy;
   if(HX != NULL)
   {
      HX->xx += CAM_ONE;
      HX->yy += CAM_ONE;
   }

   if(HC != NULL)
   {
      int r, c;

      for(r = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); r>=0; r--)
         for(c = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); c>=0; c--)
         {
            HC[0].nonlinear.cbspline.weight[r][c].x *= block_scale;
            HC[0].nonlinear.cbspline.weight[r][c].y *= block_scale;
            HC[1].nonlinear.cbspline.weight[r][c].x *= block_scale;
            HC[1].nonlinear.cbspline.weight[r][c].y *= block_scale;
         }
   }

   return GAN_TRUE;
}

static void build_warped_positions ( const CAM_VECTOR2 weight[1 + (1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)][1 + (1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)],
                                     CAM_SUPPORTSTRUCT* support )
{
   int r, c, block_limit = 1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL;
   CAM_FLOAT block_lim2, det;
   CAM_VECTOR2 Xu;
   CAM_MATRIX22 HX;

   block_lim2 = (CAM_FLOAT)(block_limit-2);

   gan_assert(support != NULL, "build_warped_positions");
   for(r = block_limit; r>=0; r--)
      for(c = block_limit; c>=0; c--)
      {
         (void)CAM_VEC2_FILL_Q(&Xu, (CAM_FLOAT)(c-1)/block_lim2-CAM_HALF, (CAM_FLOAT)(r-1)/block_lim2-CAM_HALF);
         point_cubic_Bspline_warp ( &Xu, weight, &support->warped_pos[r][c], &HX, NULL, NULL );

         /* try to invert the derivative matrix */
         det = HX.xx*HX.yy-HX.yx*HX.xy;
         if(fabs(det) < CAM_DETERMINANT_LIMIT)
         {
            /* put large values into matrix so that this point will not be chosen */
            (void)CAM_MAT22_FILL_Q(&support->warped_deriv[r][c], CAM_LARGE_VALUE, CAM_ZERO,
                                                                 CAM_ZERO, CAM_LARGE_VALUE);
         }
         else
            (void)CAM_MAT22_FILL_Q(&support->warped_deriv[r][c], HX.yy/det, -HX.xy/det,
                                                                 -HX.yx/det, HX.xx/det);
      }
}

/* compute undistorted coordinates */
static Gan_Bool
 compute_unwarped_coordinates ( CAM_VECTOR2 *Xd,
                                const CAM_VECTOR2 weight[1 + (1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)][1 + (1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)],
                                const CAM_SUPPORTSTRUCT* support,
                                CAM_VECTOR2 *Xu, int *error_code )
{
   CAM_MATRIX22 G;
   CAM_VECTOR2 v, d;
   CAM_FLOAT det;
   int it;

   /* get nearest mesh point using multiple levels */
   {
      int rbest, cbest, block_limit = 1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL;
      int level;
      CAM_FLOAT mindistance2, dist2, block_lim2 = (CAM_FLOAT)(block_limit-2);
#if 0
      /* check top level */
      mindistance2 = CAM_SQR(support->warped_pos[0][0].x-Xd->x) + CAM_SQR(support->warped_pos[0][0].y-Xd->y);
      rbest = cbest = 0;

      dist2 = CAM_SQR(support->warped_pos[0][block_limit].x-Xd->x) + CAM_SQR(support->warped_pos[0][block_limit].y-Xd->y);
      if(dist2 < mindistance2)
      {
         mindistance2 = dist2;
         rbest = 0;
         cbest = block_limit;
      }
      
      dist2 = CAM_SQR(support->warped_pos[block_limit][0].x-Xd->x) + CAM_SQR(support->warped_pos[block_limit][0].y-Xd->y);
      if(dist2 < mindistance2)
      {
         mindistance2 = dist2;
         rbest = block_limit;
         cbest = 0;
      }
      
      dist2 = CAM_SQR(support->warped_pos[block_limit][block_limit].x-Xd->x) + CAM_SQR(support->warped_pos[block_limit][block_limit].y-Xd->y);
      if(dist2 < mindistance2)
      {
         mindistance2 = dist2;
         rbest = block_limit;
         cbest = block_limit;
      }

      for(level=1; level<=GAN_CUBIC_BSPLINE_CAMERA_LEVEL; level++)
#else
      /* check top level */
      (void)CAM_VEC2_SUB_Q(Xd, &support->warped_pos[block_limit/2][block_limit/2], &d);
      (void)CAM_MAT22_MULTV2_Q(&support->warped_deriv[block_limit/2][block_limit/2], &d, &v);
      mindistance2 = CAM_SQR(v.x) + CAM_SQR(v.y);
      rbest = cbest = block_limit/2;

      for(level=2; level<=GAN_CUBIC_BSPLINE_CAMERA_LEVEL; level++)
#endif
      {
         int step = (1<<(GAN_CUBIC_BSPLINE_CAMERA_LEVEL - level)), rbestold=rbest, cbestold=cbest;

         if(rbestold > 0)
         {
            (void)CAM_VEC2_SUB_Q(Xd, &support->warped_pos[rbestold-step][cbestold], &d);
            (void)CAM_MAT22_MULTV2_Q(&support->warped_deriv[rbestold-step][cbestold], &d, &v);
            dist2 = CAM_SQR(v.x) + CAM_SQR(v.y);
            if(dist2 < mindistance2)
            {
               mindistance2 = dist2;
               rbest = rbestold-step;
               cbest = cbestold;
            }

            if(cbestold > 0)
            {
               (void)CAM_VEC2_SUB_Q(Xd, &support->warped_pos[rbestold-step][cbestold-step], &d);
               (void)CAM_MAT22_MULTV2_Q(&support->warped_deriv[rbestold-step][cbestold-step], &d, &v);
               dist2 = CAM_SQR(v.x) + CAM_SQR(v.y);
               if(dist2 < mindistance2)
               {
                  mindistance2 = dist2;
                  rbest = rbestold-step;
                  cbest = cbestold-step;
               }
            }

            if(cbestold < block_limit)
            {
               (void)CAM_VEC2_SUB_Q(Xd, &support->warped_pos[rbestold-step][cbestold+step], &d);
               (void)CAM_MAT22_MULTV2_Q(&support->warped_deriv[rbestold-step][cbestold+step], &d, &v);
               dist2 = CAM_SQR(v.x) + CAM_SQR(v.y);
               if(dist2 < mindistance2)
               {
                  mindistance2 = dist2;
                  rbest = rbestold-step;
                  cbest = cbestold+step;
               }
            }
         }

         if(rbestold < block_limit)
         {
            (void)CAM_VEC2_SUB_Q(Xd, &support->warped_pos[rbestold+step][cbestold], &d);
            (void)CAM_MAT22_MULTV2_Q(&support->warped_deriv[rbestold+step][cbestold], &d, &v);
            dist2 = CAM_SQR(v.x) + CAM_SQR(v.y);
            if(dist2 < mindistance2)
            {
               mindistance2 = dist2;
               rbest = rbestold+step;
               cbest = cbestold;
            }

            if(cbestold > 0)
            {
               (void)CAM_VEC2_SUB_Q(Xd, &support->warped_pos[rbestold+step][cbestold-step], &d);
               (void)CAM_MAT22_MULTV2_Q(&support->warped_deriv[rbestold+step][cbestold-step], &d, &v);
               dist2 = CAM_SQR(v.x) + CAM_SQR(v.y);
               if(dist2 < mindistance2)
               {
                  mindistance2 = dist2;
                  rbest = rbestold+step;
                  cbest = cbestold-step;
               }
            }

            if(cbestold < block_limit)
            {
               (void)CAM_VEC2_SUB_Q(Xd, &support->warped_pos[rbestold+step][cbestold+step], &d);
               (void)CAM_MAT22_MULTV2_Q(&support->warped_deriv[rbestold+step][cbestold+step], &d, &v);
               dist2 = CAM_SQR(v.x) + CAM_SQR(v.y);
               if(dist2 < mindistance2)
               {
                  mindistance2 = dist2;
                  rbest = rbestold+step;
                  cbest = cbestold+step;
               }
            }
         }

         if(cbestold > 0)
         {
            (void)CAM_VEC2_SUB_Q(Xd, &support->warped_pos[rbestold][cbestold-step], &d);
            (void)CAM_MAT22_MULTV2_Q(&support->warped_deriv[rbestold][cbestold-step], &d, &v);
            dist2 = CAM_SQR(v.x) + CAM_SQR(v.y);
            if(dist2 < mindistance2)
            {
               mindistance2 = dist2;
               rbest = rbestold;
               cbest = cbestold-step;
            }
         }

         if(cbestold < block_limit)
         {
            (void)CAM_VEC2_SUB_Q(Xd, &support->warped_pos[rbestold][cbestold+step], &d);
            (void)CAM_MAT22_MULTV2_Q(&support->warped_deriv[rbestold][cbestold+step], &d, &v);
            dist2 = CAM_SQR(v.x) + CAM_SQR(v.y);
            if(dist2 < mindistance2)
            {
               mindistance2 = dist2;
               rbest = rbestold;
               cbest = cbestold+step;
            }
         }
      }

      /* compute sub-block position */
      (void)CAM_VEC2_SUB_Q(Xd, &support->warped_pos[rbest][cbest], &d);
      (void)CAM_MAT22_MULTV2_Q(&support->warped_deriv[rbest][cbest], &d, &v);

      Xu->x = ((float)(cbest-1) + v.x)/block_lim2-CAM_HALF;
      Xu->y = ((float)(rbest-1) + v.y)/block_lim2-CAM_HALF;
   }

#if 0
   /* old way of initialising calculation */
   if(!point_cubic_Bspline_warp(Xd, weight, &v, NULL, NULL, error_code))
   {
      gan_err_flush_trace();
      gan_err_register ( "compute_unwarped_coordinates", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   Xu->x = CAM_TWO*Xd->x - v.x;
   Xu->y = CAM_TWO*Xd->y - v.y;
#endif

   for ( it = 1; ; it++ )
   {
      if(!point_cubic_Bspline_warp(Xu, weight, &v, &G, NULL, error_code))
      {
         gan_err_flush_trace();
         gan_err_register ( "compute_unwarped_coordinates", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      (void)CAM_VEC2_DECREMENT ( &v, Xd );
      det = CAM_MAT22_DET_Q(&G);
      if ( det == CAM_ZERO )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "compute_unwarped_coordinates", GAN_ERROR_DIVISION_BY_ZERO, "" );
         }
         else
            *error_code = GAN_ERROR_DIVISION_BY_ZERO;

         return GAN_FALSE;
      }

      (void)CAM_VEC2_FILL_Q ( &d, (G.yy*v.x - G.xy*v.y)/det, (G.xx*v.y - G.yx*v.x)/det );
      (void)CAM_VEC2_DECREMENT ( Xu, &d );

      /* check whether we have converged, observing the minimum number of iterations */
      if ( fabs(d.x)+fabs(d.y) < TERMINATION_THRESHOLD && it >= MIN_UND_ITERATIONS )
         break;
        
      /* check whether we have exceeded the maximum number of iterations */
      if ( it > MAX_UND_ITERATIONS )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "compute_unwarped_coordinates", GAN_ERROR_NO_CONVERGENCE, "" );
         }
         else
            *error_code = GAN_ERROR_OUTSIDE_RANGE;

         return GAN_FALSE;
      }
   }

   /* check that the point projects correctly */
   if(!point_cubic_Bspline_warp(Xu, weight, &v, NULL, NULL, error_code))
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "compute_unwarped_coordinates", GAN_ERROR_NO_CONVERGENCE, "" );
      }
      else
         *error_code = GAN_ERROR_OUTSIDE_RANGE;

      return GAN_FALSE;
   }

#define PROJECTION_THRESHOLD 0.001
   if(fabs(v.x-Xd->x) >= PROJECTION_THRESHOLD || fabs(v.x-Xd->x) >= PROJECTION_THRESHOLD)
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "compute_unwarped_coordinates", GAN_ERROR_NO_CONVERGENCE, "" );
      }
      else
         *error_code = GAN_ERROR_OUTSIDE_RANGE;

      return GAN_FALSE;
   }
      
   return GAN_TRUE;
}

/* point projection function */
static Gan_Bool
 point_project ( const CAM_STRUCT *camera, CAM_VECTOR3 *X, CAM_VECTOR3 *p, GAN_POSITIONSTATE *Xpprev,
                 CAM_MATRIX22 *HX, CAM_STRUCT HC[2], int *error_code )
{
   CAM_VECTOR2 Xp, pp;
   CAM_VECTOR3 pb;

   /* consistency check */
#ifdef CAM_INVERT_WARP
   gan_err_test_bool ( camera->type == GAN_CUBIC_BSPLINE_CAMERA_INV, "point_project", GAN_ERROR_INCOMPATIBLE, "" );
#else
   gan_err_test_bool ( camera->type == GAN_CUBIC_BSPLINE_CAMERA, "point_project", GAN_ERROR_INCOMPATIBLE, "" );
#endif /* #ifdef CAM_INVERT_WARP */

   /* if we are computing Jacobians, the camera coordinates must have Z=1 */
#ifndef NDEBUG
   if ( (HX != NULL || HC != NULL) && X->z != CAM_ONE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_project", GAN_ERROR_INCOMPATIBLE, "" );
      }
      else
         *error_code = GAN_ERROR_INCOMPATIBLE;

      return GAN_FALSE;
   }
#endif

   if ( X->z == CAM_ZERO )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_project", GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   if ( X->z != CAM_ONE )
   {
      /* divide through by Z coordinate to normalise it to 1 */
      Xp.x = X->x/X->z;
      Xp.y = X->y/X->z;
   }
   else
   {
      Xp.x = X->x;
      Xp.y = X->y;
   }

#ifdef CAM_INVERT_WARP
   if(!compute_unwarped_coordinates(&Xp, (const CAM_WEIGHTBLOCK*)camera->nonlinear.cbspline.weight, camera->nonlinear.cbspline.support,
                                    &pp, error_code ))
   {
      if ( error_code == NULL )
         gan_err_register ( "point_project", GAN_ERROR_FAILURE, "" );
         
      return GAN_FALSE;
   }
#else
   if(!point_cubic_Bspline_warp(&Xp, (const CAM_WEIGHTBLOCK *)camera->nonlinear.cbspline.weight, &pp, HX, HC, error_code))
   {
      gan_err_flush_trace();
      gan_err_register ( "point_project", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }
#endif

#ifdef DEBUG_ALL
   if(HX != NULL)
   {
      CAM_VECTOR2 vtmp1, vtmp2;
      CAM_MATRIX22 mtmp;

      Xp.x -= CAM_SMALL_NUMBER;
      point_cubic_Bspline_warp(&Xp, camera->nonlinear.cbspline.weight, &vtmp1, NULL, NULL, error_code);
      Xp.x += CAM_TWO*CAM_SMALL_NUMBER;
      point_cubic_Bspline_warp(&Xp, camera->nonlinear.cbspline.weight, &vtmp2, NULL, NULL, error_code);
      Xp.x -= CAM_SMALL_NUMBER;
      mtmp.xx = (vtmp2.x - vtmp1.x)*CAM_HALF*CAM_SMALL_NUMBER_INV;
      mtmp.yx = (vtmp2.y - vtmp1.y)*CAM_HALF*CAM_SMALL_NUMBER_INV;

      Xp.y -= CAM_SMALL_NUMBER;
      point_cubic_Bspline_warp(&Xp, camera->nonlinear.cbspline.weight, &vtmp1, NULL, NULL, error_code);
      Xp.y += CAM_TWO*CAM_SMALL_NUMBER;
      point_cubic_Bspline_warp(&Xp, camera->nonlinear.cbspline.weight, &vtmp2, NULL, NULL, error_code);
      Xp.y -= CAM_SMALL_NUMBER;
      mtmp.xy = (vtmp2.x - vtmp1.x)*CAM_HALF*CAM_SMALL_NUMBER_INV;
      mtmp.yy = (vtmp2.y - vtmp1.y)*CAM_HALF*CAM_SMALL_NUMBER_INV;

      printf ( "Test\n");
      printf("Symbolic %f %f %f %f\n", HX->xx, HX->xy, HX->yx, HX->yy);
      printf("Numeric  %f %f %f %f\n", mtmp.xx, mtmp.xy, mtmp.yx, mtmp.yy);
   }
#endif /* #ifdef CAM_INVERT_WARP */

   (void)CAM_VEC3_SET_PARTS_Q(&pb, &pp, CAM_ONE);

   /* apply projective transformation */
   (void)CAM_MAT33_MULTV3_Q(&camera->nonlinear.cbspline.K, &pb, p);

   /* normalise x,y coordinates */
   if(p->z == CAM_ZERO)
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_project", GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   p->x *= CZH/p->z;
   p->y *= CZH/p->z;

#ifndef CAM_INVERT_WARP
   /* compute Jacobian w.r.t. X,Y */
   if(HX != NULL)
   {
      CAM_MATRIX22 HXp;

      (void)CAM_MAT22_FILL_Q(&HXp,
                             (CZH*camera->nonlinear.cbspline.K.xx - p->x*camera->nonlinear.cbspline.K.zx)/p->z,
                             (CZH*camera->nonlinear.cbspline.K.xy - p->x*camera->nonlinear.cbspline.K.zy)/p->z,
                             (CZH*camera->nonlinear.cbspline.K.yx - p->y*camera->nonlinear.cbspline.K.zx)/p->z,
                             (CZH*camera->nonlinear.cbspline.K.yy - p->y*camera->nonlinear.cbspline.K.zy)/p->z);
      HXp = CAM_MAT22_RMULTM22_S(&HXp, HX);
      *HX = HXp;
   }

   if(HC != NULL)
   {
      int r, c;
      CAM_VECTOR2 w[2];

      for(r = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); r>=0; r--)
         for(c = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); c>=0; c--)
         {
            w[0] = HC[0].nonlinear.cbspline.weight[r][c];
            w[1] = HC[1].nonlinear.cbspline.weight[r][c];
            HC[0].nonlinear.cbspline.weight[r][c].x = (CZH*camera->nonlinear.cbspline.K.xx*w[0].x - p->x*camera->nonlinear.cbspline.K.xy*w[0].y)/p->z;
            HC[0].nonlinear.cbspline.weight[r][c].x = (CZH*camera->nonlinear.cbspline.K.yx*w[0].x - p->x*camera->nonlinear.cbspline.K.yy*w[0].y)/p->z;
            HC[1].nonlinear.cbspline.weight[r][c].x = (CZH*camera->nonlinear.cbspline.K.xx*w[1].x - p->x*camera->nonlinear.cbspline.K.xy*w[1].y)/p->z;
            HC[1].nonlinear.cbspline.weight[r][c].x = (CZH*camera->nonlinear.cbspline.K.yx*w[1].x - p->x*camera->nonlinear.cbspline.K.yy*w[1].y)/p->z;
         }
   }
#endif /* #ifndef CAM_INVERT_WARP */

   /* finally normalise z-coordinate */
   p->z = CZH;

   return GAN_TRUE;
}

/* point back-projection function */
static Gan_Bool
 point_backproject ( const CAM_STRUCT *camera, CAM_VECTOR3 *p, CAM_VECTOR3 *X, GAN_POSITIONSTATE *pXprev, int *error_code )
{
   CAM_VECTOR2 Xd, Xu;

   /* consistency check */
#ifdef CAM_INVERT_WARP
   gan_err_test_bool ( camera->type == GAN_CUBIC_BSPLINE_CAMERA_INV, "point_backproject", GAN_ERROR_INCOMPATIBLE, "" );
#else
   gan_err_test_bool ( camera->type == GAN_CUBIC_BSPLINE_CAMERA, "point_backproject", GAN_ERROR_INCOMPATIBLE, "" );
#endif /* #ifdef CAM_INVERT_WARP */

   /* undo projective warp */
   *X = CAM_MAT33_MULTV3_S(&camera->nonlinear.cbspline.Kinv, p);

   /* can't unwarp a point on z=0 plane */
   if ( X->z == CAM_ZERO )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_backproject", GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   /* compute distorted X/Y coordinates of scene point Xd on plane Z=1 */
   Xd.x = X->x/X->z;
   Xd.y = X->y/X->z;

#ifdef CAM_INVERT_WARP
   if(!point_cubic_Bspline_warp(&Xd, (const CAM_WEIGHTBLOCK *)camera->nonlinear.cbspline.weight, &Xu, NULL, NULL, error_code))
   {
      if ( error_code == NULL )
         gan_err_register ( "point_backproject", GAN_ERROR_FAILURE, "" );

      return GAN_FALSE;
   }
#else
   if(!compute_unwarped_coordinates(&Xd, (const CAM_WEIGHTBLOCK *)camera->nonlinear.cbspline.weight,
                                    camera->nonlinear.cbspline.support, &Xu, error_code ))
   {
      if ( error_code == NULL )
         gan_err_register ( "point_backproject", GAN_ERROR_FAILURE, "" );

      return GAN_FALSE;
   }
#endif /* #ifdef CAM_INVERT_WARP */

   /* build undistorted scene point */
   CAM_VEC3_SET_PARTS_Q ( X, &Xu, CAM_ONE );

   /* success */
   return GAN_TRUE;
}

/* function to add distortion to a point */
static Gan_Bool
 point_add_distortion ( const CAM_STRUCT *camera, CAM_VECTOR3 *pu, CAM_VECTOR3 *p, GAN_POSITIONSTATE *pupprev, int *error_code )
{
   CAM_VECTOR3 X;

   /* backproject using inverse projective warp only */
   (void)CAM_MAT33_MULTV3_Q(&camera->nonlinear.cbspline.Kinv, pu, &X);

   /* now project point */
   if(!point_project(camera, &X, p, pupprev, NULL, NULL, error_code))
   {
      if(error_code == NULL)
         gan_err_register ( "point_add_distortion", GAN_ERROR_DIVISION_BY_ZERO, "" );

      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}
   
/* function to remove distortion from a point */
static Gan_Bool
 point_remove_distortion ( const CAM_STRUCT *camera, CAM_VECTOR3 *p, CAM_VECTOR3 *pu, GAN_POSITIONSTATE *ppuprev, int *error_code)
{
   CAM_VECTOR3 X;

   /* first backproject point */
   if(!point_backproject(camera, p, &X, ppuprev, error_code))
   {
      if(error_code == NULL)
         gan_err_register ( "point_remove_distortion", GAN_ERROR_DIVISION_BY_ZERO, "" );

      return GAN_FALSE;
   }

   /* project using projective warp only */
   (void)CAM_MAT33_MULTV3_Q(&camera->nonlinear.cbspline.K, &X, pu);

   /* normalise x,y coordinates */
   if(pu->z == CAM_ZERO)
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_remove_distortion", GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   pu->x *= CZH/pu->z;
   pu->y *= CZH/pu->z;
   pu->z = CZH;
   
   /* success */
   return GAN_TRUE;
}

/* line functions */

/* line projection function */
static Gan_Bool
 line_project ( const CAM_STRUCT *camera, CAM_VECTOR3 *L, CAM_VECTOR3 *l )
{
   /* consistency check */
#ifdef CAM_INVERT_WARP
   gan_err_test_bool ( camera->type == GAN_CUBIC_BSPLINE_CAMERA_INV, "line_project", GAN_ERROR_INCOMPATIBLE, "" );
#else
   gan_err_test_bool ( camera->type == GAN_CUBIC_BSPLINE_CAMERA, "line_project", GAN_ERROR_INCOMPATIBLE, "" );
#endif /* #ifdef CAM_INVERT_WARP */

   /* not implemented */
   gan_err_flush_trace();
   gan_err_register ( "line_project", GAN_ERROR_NOT_IMPLEMENTED, "" );
   return GAN_FALSE;
}

/* line back-projection function */
static Gan_Bool
 line_backproject ( const CAM_STRUCT *camera, CAM_VECTOR3 *l, CAM_VECTOR3 *L )
{
   /* consistency check */
#ifdef CAM_INVERT_WARP
   gan_err_test_bool ( camera->type == GAN_CUBIC_BSPLINE_CAMERA_INV, "line_backproject", GAN_ERROR_INCOMPATIBLE, "" );
#else
   gan_err_test_bool ( camera->type == GAN_CUBIC_BSPLINE_CAMERA, "line_backproject", GAN_ERROR_INCOMPATIBLE, "" );
#endif /* #ifdef CAM_INVERT_WARP */

   /* not implemented */
   gan_err_flush_trace();
   gan_err_register ( "line_backproject", GAN_ERROR_NOT_IMPLEMENTED, "" );
   return GAN_FALSE;
}

