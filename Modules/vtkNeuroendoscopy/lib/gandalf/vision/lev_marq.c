/**
 * File:          $RCSfile: lev_marq.c,v $
 * Module:        Levenberg-Marquardt non-linear minimisation algorithm
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:19 $
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

#include <gandalf/vision/lev_marq.h>
#include <gandalf/common/allocate.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \defgroup LevenbergMarquardt Levenberg-Marquardt algorithm
 * \{
 */


/**
 * \brief Forms a Levenberg-Marquardt structure.
 * \param lm Pointer to a Levenberg-Marquardt structure or \c NULL
 *
 * Allocates/fills a structure for computing Levenberg-Marquardt optimisation.
 * \return Pointer to structure or <b>NULL</b> on failure.
 * \sa gan_lev_marq_alloc().
 */
Gan_LevMarqStruct *
 gan_lev_marq_form ( Gan_LevMarqStruct *lm )
{
   if ( lm == NULL )
   {
      /* allocate structure dynamically */
      lm = gan_malloc_object(Gan_LevMarqStruct);
      if ( lm == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_lev_marq_form", GAN_ERROR_MALLOC_FAILED, "", sizeof(*lm) );
         return NULL;
      }

      lm->alloc = GAN_TRUE;
   }
   else
      lm->alloc = GAN_FALSE;

   /* allocate matrices & vectors */
   if ( gan_symmat_form ( &lm->A, 0 ) == NULL ||
        gan_vec_form ( &lm->a, 0 ) == NULL ||
        gan_symmat_form ( &lm->P, 0 ) == NULL ||
        gan_vec_form ( &lm->x, 0 ) == NULL ||
        gan_vec_form ( &lm->xold, 0 ) == NULL ||
        gan_vec_form ( &lm->hF, 0 ) == NULL ||
        gan_mat_form ( &lm->Hx, 0, 0 ) == NULL ||
        gan_mat_form ( &lm->Hz, 0, 0 ) == NULL ||
        gan_mat_form ( &lm->NiHx, 0, 0 ) == NULL ||
        gan_symmat_form ( &lm->HxTNiHx, 0 ) == NULL ||
        gan_vec_form ( &lm->Niv, 0 ) == NULL ||
        gan_vec_form ( &lm->HxTNiv, 0 ) == NULL ||
        gan_symmat_form ( &lm->Ni, 0 ) == NULL )
   {
      gan_err_register ( "gan_lev_marq_form", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* initialise list of observations */
   lm->obs_list = gan_list_new();
   if ( lm->obs_list == NULL )
   {
      gan_err_register ( "gan_lev_marq_form", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* initialise residual to an illegal value */
   lm->residual = -1.0;

   /* successful exit */
   return lm;
}

/**
 * \brief Passes an observation to a Levenberg-Marquardt structure.
 * \param lm Pointer to a structure
 * \param z Observation vector
 * \param zdata Data pointer to pass along with \a z
 * \param Ni Observation inverse covariance
 * \param obs_func Observation callback function
 *
 * Adds a new observation for Levenberg-Marquardt minimisation.
 * The observation is a direct observation relating state to observation
 * \f$z = h(x) + w\f$ where \f$z\f$ is the observation vector, \f$h()\f$
 * is the observation function, \f$x\f$ is the state vector and \f$w\f$ is a
 * zero-mean noise vector, with covariance \f$N\f$.
 *
 * \return Handle for the new observation or <b>NULL</b> on failure.
 * \sa gan_lev_marq_form().
 */
Gan_LevMarqObs *
 gan_lev_marq_obs_h ( Gan_LevMarqStruct *lm,
                      Gan_Vector *z, void *zdata, Gan_SquMatrix *Ni,
                      Gan_LevMarqObsFunc_h obs_func )
{
   Gan_LevMarqObs *obs;

   /* check that size of observation vector and covariance matrix are
      compatible */
   gan_err_test_ptr ( z->rows == Ni->size, "gan_lev_marq_obs_h",
                      GAN_ERROR_INCOMPATIBLE, "vector and covariance" );

   /* allocate observation structure and test for failure */
   obs = gan_malloc_object(Gan_LevMarqObs);
   if ( obs == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_lev_marq_obs_h", GAN_ERROR_MALLOC_FAILED, "", sizeof(*obs) );
      return NULL;
   }

   obs->type = GAN_LEV_MARQ_OBS_H;
   obs->details.h.func = obs_func;
   if ( gan_vec_form ( &obs->details.h.z, z->rows ) == NULL ||
        gan_symmat_form ( &obs->details.h.Ni, z->rows ) == NULL )
   {
      gan_err_register ( "gan_lev_marq_obs_h", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* copy observation vector and inverse covariance */
   obs->details.h.zdata = zdata;
   if ( gan_vec_copy_q ( z, &obs->details.h.z ) == NULL ||
        gan_squmat_copy_q ( Ni, &obs->details.h.Ni ) == NULL )
   {
      gan_err_register ( "gan_lev_marq_obs_h", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* insert new observation in the list stored with the algorithm */
   if ( !gan_list_insert_first ( lm->obs_list, obs ) )
   {
      gan_err_register ( "gan_lev_marq_obs_h", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* fields involving robustness not involved here */
   obs->details.h.var_scale   = 0.0;
   obs->details.h.chi2        = 0.0;
   obs->details.h.chi2_offset = 0.0;
   obs->outlier = GAN_FALSE;

   /* success */
   return obs;
}
                     
/**
 * \brief Passes a robust observation to a Levenberg-Marquardt structure.
 * \param lm Pointer to a structure
 * \param z Observation vector
 * \param zdata Data pointer to pass along with \a z
 * \param Ni Observation inverse covariance
 * \param obs_func Observation callback function
 * \param var_scale Scaling of outlier covariance w.r.t. inlier covariance
 * \param chi2 Cutoff point on innovation for switching distributions
 *
 * Adds a new observation for Levenberg-Marquardt minimisation.
 * The observation is a direct observation relating state to observation
 * \f$z = h(x) + w\f$ where \f$z\f$ is the observation vector, \f$h()\f$ is
 * the observation function, \f$x\f$ is the state vector and \f$w\f$ is a
 * zero-mean noise vector, with covariance \f$N\f$.
 *
 * \return Handle for the new observation or \c NULL on failure.
 * \sa gan_lev_marq_form().
 */
Gan_LevMarqObs *
 gan_lev_marq_obs_h_robust ( Gan_LevMarqStruct *lm,
                             Gan_Vector *z, void *zdata, Gan_SquMatrix *Ni,
                             Gan_LevMarqObsFunc_h obs_func,
                             double var_scale, double chi2 )
{
   Gan_LevMarqObs *obs;

   /* check that size of observation vector and covariance matrix are
      compatible */
   gan_err_test_ptr ( z->rows == Ni->size, "gan_lev_marq_obs_h_robust",
                      GAN_ERROR_INCOMPATIBLE, "vector and covariance" );

   /* check that variance scale is valid */
   gan_err_test_ptr ( var_scale > 1.0, "gan_lev_marq_obs_h_robust",
                      GAN_ERROR_ILLEGAL_ARGUMENT, "robust variance scaling" );
   
   /* allocate observation structure and test for failure */
   obs = gan_malloc_object(Gan_LevMarqObs);
   if ( obs == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_lev_marq_obs_h_robust", GAN_ERROR_MALLOC_FAILED, "", sizeof(*obs) );
      return NULL;
   }

   obs->type = GAN_LEV_MARQ_OBS_H_ROBUST;
   obs->details.h.func = obs_func;
   if ( gan_vec_form ( &obs->details.h.z, z->rows ) == NULL ||
        gan_symmat_form ( &obs->details.h.Ni, z->rows ) == NULL )
   {
      gan_err_register ( "gan_lev_marq_obs_h_robust", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* copy observation vector and inverse covariance */
   obs->details.h.zdata = zdata;
   if ( gan_vec_copy_q ( z, &obs->details.h.z ) == NULL ||
        gan_squmat_copy_q ( Ni, &obs->details.h.Ni ) )
   {
      gan_err_register ( "gan_lev_marq_obs_h_robust", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* insert new observation in the list stored with the algorithm */
   if ( !gan_list_insert_first ( lm->obs_list, obs ) )
   {
      gan_err_register ( "gan_lev_marq_obs_h_robust", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* copy fields specific to robustness */
   obs->details.h.var_scale   = var_scale;
   obs->details.h.chi2        = chi2;
   obs->details.h.chi2_offset = (1.0 - 1.0/var_scale)*chi2;

   /* mark observation initially as an inlier */
   obs->outlier = GAN_FALSE;

   /* success */
   return obs;
}
                     
/**
 * \brief Passes an observation to a Levenberg-Marquardt structure.
 * \param lm Pointer to a Levenberg-Marquardt structure
 * \param z Observation vector
 * \param zdata Data pointer to pass along with z
 * \param Ni Observation inverse covariance
 * \param obs_func Observation callback function
 *
 * Adds a new observation for Levenberg-Marquardt minimisation. It relates
 * state to observation according to the model \f$F(x,z+w) = 0\f$ where
 * \f$z\f$ is the observation vector, \f$F()\f$ is the observation function,
 * \f$x\f$ is the state vector and \f$w\f$ is a zero-mean noise vector, with
 * covariance \f$N\f$.
 * \return Handle for the new observation or <b>NULL</b> on failure.
 * \sa gan_lev_marq_form().
 */
Gan_LevMarqObs *
 gan_lev_marq_obs_F ( Gan_LevMarqStruct *lm,
                      Gan_Vector *z, void *zdata, Gan_SquMatrix *Ni,
                      Gan_LevMarqObsFunc_F obs_func )
{
   Gan_LevMarqObs *obs;

   /* check that size of observation vector and covariance matrix are
      compatible */
   gan_err_test_ptr ( z->rows == Ni->size, "gan_lev_marq_obs_h",
                      GAN_ERROR_INCOMPATIBLE, "vector and covariance" );

   /* allocate observation structure and test for failure */
   obs = gan_malloc_object(Gan_LevMarqObs);
   if ( obs == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_lev_marq_obs_F", GAN_ERROR_MALLOC_FAILED, "", sizeof(*obs) );
      return NULL;
   }

   obs->type = GAN_LEV_MARQ_OBS_F;
   obs->details.F.func = obs_func;
   if ( gan_vec_form ( &obs->details.F.z, z->rows ) == NULL ||
        gan_symmat_form ( &obs->details.F.N, z->rows ) == NULL )
   {
      gan_err_register ( "gan_lev_marq_obs_F", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* copy observation vector and invert inverse covariance */
   obs->details.F.zdata = zdata;
   if ( gan_vec_copy_q ( z, &obs->details.F.z ) == NULL ||
        gan_squmat_invert_q ( Ni, &obs->details.F.N ) == NULL )
   {
      gan_err_register ( "gan_lev_marq_obs_F", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* insert new observation in the list stored with the algorithm */
   if ( !gan_list_insert_first ( lm->obs_list, obs ) )
   {
      gan_err_register ( "gan_lev_marq_obs_F", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   obs->outlier = GAN_FALSE;

   /* success */
   return obs;
}

/* compute residual given existing state vector */
static double
 compute_residual ( Gan_LevMarqStruct *lm )
{
   int i;
   Gan_LevMarqObs *obs;
   double residual = 0.0;

   /* go through observations one by one, adding up residual */
   gan_list_goto_head ( lm->obs_list );
   for ( i = gan_list_get_size(lm->obs_list)-1; i >= 0; i-- )
   {
      /* get observation pointer from list */
      obs = gan_list_get_next ( lm->obs_list, Gan_LevMarqObs );

      /* evaluate observation */
      switch ( obs->type )
      {
         case GAN_LEV_MARQ_OBS_H:
         case GAN_LEV_MARQ_OBS_H_ROBUST:
           if ( !obs->details.h.func ( &lm->x, &obs->details.h.z,
                                       obs->details.h.zdata, &lm->hF, NULL ) )
           {
              gan_err_register ( "compute_residual", GAN_ERROR_FAILURE, "" );
              return GAN_FALSE;
           }

           /* check dimensions match */
           gan_err_test_bool ( lm->hF.rows == obs->details.h.z.rows,
                               "compute_residual", GAN_ERROR_INCOMPATIBLE, "");

           /* compute innovation vector v=z-h(x), overwriting h with the
              result */
           gan_vec_sub_i2 ( &obs->details.h.z, &lm->hF );

           /* compute N^-1*v */
           gan_squmat_multv_q ( &obs->details.h.Ni, &lm->hF, &lm->Niv );

           if ( obs->type == GAN_LEV_MARQ_OBS_H )
              /* compute v^T*N^-1*v and increment residual with it */
              residual += gan_vec_dot ( &lm->Niv, &lm->hF );
           else /* obs->type == GAN_LEV_MARQ_OBS_H_ROBUST */
           {
              double chi2;

              /* compare squared normalised innovation v^T*N^-1*v with
                 threshold */
              chi2 = gan_vec_dot ( &lm->Niv, &lm->hF );
              if ( chi2 <= obs->details.h.chi2 )
              {
                 /* inlier distribution: increment residual with squared
                    innovation as normal */
                 residual += chi2;
                 obs->outlier = GAN_FALSE;
              }
              else /* chi2 > obs->details.h.chi2 */
              {
                 /* adjust residual using outlier distribution offset */
                 residual += chi2/obs->details.h.var_scale +
                             obs->details.h.chi2_offset;
                 obs->outlier = GAN_TRUE;
              }
           }

           break;

         case GAN_LEV_MARQ_OBS_F:
           if ( !obs->details.F.func ( &lm->x, &obs->details.F.z,
                                       obs->details.F.zdata,
                                       &lm->hF, NULL, &lm->Hz ) )
           {
              gan_err_register ( "compute_residual", GAN_ERROR_FAILURE, "" );
              return GAN_FALSE;
           }

           /* check dimensions match */
           gan_err_test_bool ( lm->hF.rows <= obs->details.F.z.rows,
                               "compute_residual", GAN_ERROR_INCOMPATIBLE,
                               "" );

           /* compute Hz*N*Hz^T */
           gan_symmat_lrmult_q ( &obs->details.F.N, &lm->Hz, &lm->NiHx,
                                 &lm->HxTNiHx );

           /* invert observation covariance */
           gan_squmat_invert_q ( &lm->HxTNiHx, &lm->Ni );

           /* compute v^T*N^-1*v and increment residual with it */
           gan_squmat_multv_q ( &lm->Ni, &lm->hF, &lm->Niv );
           residual += gan_vec_dot ( &lm->Niv, &lm->hF );

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "compute_residual", GAN_ERROR_ILLEGAL_TYPE, "" );
           return GAN_FALSE;
      }
   }

   return residual;
}

/**
 * \brief Initialise Levenberg-Marquardt algorithm.
 * \param lm Pointer to a Levenberg-Marquardt structure
 * \param init_func Function to initialise value of state vector
 * \param data User pointer to be passed into \a init_func
 * \param residualp Pointer to initial residual
 *
 * After passing some observations to the structure using gan_lev_marq_obs_h()
 * or gan_lev_marq_obs_F(), this function invokes the callback function
 * \a init_func to set the state to initial values, before applying
 * Levenberg-Marquardt iterations using gan_lev_marq_iteration().
 *
 * \return #GAN_TRUE on successful initialisation of structure, #GAN_FALSE on failure.
 * \sa gan_lev_marq_iteration().
 */
Gan_Bool
 gan_lev_marq_init ( Gan_LevMarqStruct *lm,
                     Gan_LevMarqInitFunc init_func,
                     void *data, double *residualp )
{
   /* attempt to initialise state vector */
   if ( !init_func ( &lm->x, lm->obs_list, data ) ) return GAN_FALSE;

   /* compute initial residual */
   lm->residual = compute_residual ( lm );

   if ( residualp != NULL )
      /* set residual output */
      *residualp = lm->residual;
   
   /* successful initialisation */
   return GAN_TRUE;
}

/**
 * \brief Applies Levenberg-Marquardt iteration.
 * \param lm Pointer to a Levenberg-Marquardt structure
 * \param lambda Adjustment factor for adjusting the information matrix
 * \param residualp Output residual after iteration
 *
 * Applies a single Levenberg-Marquardt iteration. The residual after the
 * adjustment is returned in the provided pointer. The adjustment is only
 * actually applied to the state vector \f$x\f$ if the residual has been
 * reduced from the existing value.
 *
 * \return #GAN_TRUE on successful completion of iteration, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_lev_marq_iteration ( Gan_LevMarqStruct *lm, double lambda, double *residualp )
{
   int i;
   Gan_LevMarqObs *obs;
   double residual;

   /* initialise information to zero */
   if ( gan_symmat_set_size ( &lm->A, lm->x.rows ) == NULL ||
        gan_symmat_fill_const_q ( &lm->A, lm->x.rows, 0.0 ) == NULL ||
        gan_vec_fill_const_q ( &lm->a, lm->x.rows, 0.0 ) == NULL )
   {
      gan_err_register ( "gan_lev_marq_iteration", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* store existing state vector */
   gan_vec_copy_q ( &lm->x, &lm->xold );

   /* go through observations one by one */
   gan_list_goto_head ( lm->obs_list );
   for ( i = gan_list_get_size(lm->obs_list)-1; i >= 0; i-- )
   {
      /* get observation pointer from list */
      obs = gan_list_get_next ( lm->obs_list, Gan_LevMarqObs );

      /* evaluate observation */
      switch ( obs->type )
      {
         case GAN_LEV_MARQ_OBS_H:
         case GAN_LEV_MARQ_OBS_H_ROBUST:
           if ( !obs->details.h.func ( &lm->x, &obs->details.h.z,
                                       obs->details.h.zdata,
                                       &lm->hF, &lm->Hx ) )
           {
              gan_err_register ( "gan_lev_marq_iteration", GAN_ERROR_FAILURE,
                                 "" );
              return GAN_FALSE;
           }

           /* check dimensions match */
           gan_err_test_bool ( lm->hF.rows == obs->details.h.z.rows &&
                               gan_mat_test_dims ( &lm->Hx,
                                                   lm->hF.rows, lm->x.rows ),
                               "gan_lev_marq_iteration",
                               GAN_ERROR_INCOMPATIBLE, "" );

           /* compute Hx^T*N^-1*Hx */
           gan_symmat_lrmultT_q ( &obs->details.h.Ni, &lm->Hx, &lm->NiHx,
                                  &lm->HxTNiHx );

           /* compute innovation vector v=z-h(x), overwriting h with the
              result */
           gan_vec_sub_i2 ( &obs->details.h.z, &lm->hF );

           /* compute Hx^T*N^-1*v */
           gan_matT_multv_q ( &lm->NiHx, &lm->hF, &lm->HxTNiv );

           if ( obs->type == GAN_LEV_MARQ_OBS_H_ROBUST )
           {
              double chi2;

              /* compute N^-1*v */
              gan_squmat_multv_q ( &obs->details.h.Ni, &lm->hF, &lm->Niv );

              /* compare squared normalised innovation v^T*N^-1*v with
                 threshold */
              chi2 = gan_vec_dot ( &lm->Niv, &lm->hF );
              if ( chi2 > obs->details.h.chi2 )
              {
                 /* adjust increments to information matrix & vector */
                 gan_squmat_divide_i ( &lm->HxTNiHx,
                                       obs->details.h.var_scale );
                 gan_vec_divide_i ( &lm->HxTNiv, obs->details.h.var_scale );
                 obs->outlier = GAN_TRUE;
              }
              else /* chi2 <= obs->details.h.chi2 */
                 obs->outlier = GAN_FALSE;
           }

           /* increment information matrix sum */
           gan_squmat_increment ( &lm->A, &lm->HxTNiHx );

           /* increment information vector */
           gan_vec_increment ( &lm->a, &lm->HxTNiv );

           break;

         case GAN_LEV_MARQ_OBS_F:
           if ( !obs->details.F.func ( &lm->x, &obs->details.F.z,
                                       obs->details.F.zdata,
                                       &lm->hF, &lm->Hx, &lm->Hz ) )
           {
              gan_err_register ( "gan_lev_marq_iteration", GAN_ERROR_FAILURE,
                                 "" );
              return GAN_FALSE;
           }

           /* check dimensions match */
           gan_err_test_bool ( lm->hF.rows <= obs->details.F.z.rows &&
                               gan_mat_test_dims ( &lm->Hx,
                                                   lm->hF.rows, lm->x.rows ) &&
                               gan_mat_test_dims ( &lm->Hz, lm->hF.rows,
                                                   obs->details.F.z.rows ),
                               "gan_lev_marq_iteration",
                               GAN_ERROR_INCOMPATIBLE, "" );

           /* compute Hz*N*Hz^T */
           gan_symmat_lrmult_q ( &obs->details.F.N, &lm->Hz, &lm->NiHx,
                                 &lm->HxTNiHx );

           /* invert observation covariance */
           gan_squmat_invert_q ( &lm->HxTNiHx, &lm->Ni );

           /* compute Hx^T*N^-1*Hx */
           gan_symmat_lrmultT_q ( &lm->Ni, &lm->Hx, &lm->NiHx, &lm->HxTNiHx );

           /* increment information matrix sum */
           gan_squmat_increment ( &lm->A, &lm->HxTNiHx );

           /* compute innovation -F */
           gan_vec_negate_i ( &lm->hF );

           /* compute Hx^T*N^-1*v */
           gan_matT_multv_q ( &lm->NiHx, &lm->hF, &lm->HxTNiv );

           /* increment information vector */
           gan_vec_increment ( &lm->a, &lm->HxTNiv );

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_lev_marq_iteration", GAN_ERROR_ILLEGAL_TYPE,
                              "" );
           return GAN_FALSE;
      }
   }

   /* add lambda*I to A */
   for ( i = (int)lm->x.rows-1; i >= 0; i-- )
      gan_squmat_inc_el ( &lm->A, i, i, lambda );

   /* compute Cholesky factorisation of the information matrix */
   if ( gan_squmat_cholesky_i ( &lm->A ) == NULL )
   {
      gan_err_register ( "gan_lev_marq_iteration", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* compute A^-1*a */
   if ( gan_squmatI_multv_i ( &lm->A, &lm->a ) == NULL ||
        gan_squmatIT_multv_i ( &lm->A, &lm->a ) == NULL )
   {
      gan_err_register ( "gan_lev_marq_iteration", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* add A^-1*a to state vector x */
   gan_vec_increment ( &lm->x, &lm->a );

   /* compute covariance A^-1 */
   gan_squmatIT_srmultI_squ_q ( &lm->A, &lm->P );

   /* compute residual after update */
   residual = compute_residual ( lm );

   /* reinstate state vector if the new residual is equal or higher */
   if ( residual >= lm->residual )
      gan_vec_copy_q ( &lm->xold, &lm->x );
   else
      /* update best residual */
      lm->residual = residual;

   if ( residualp != NULL )
      /* set residual output */
      *residualp = residual;

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Returns state of Levenberg-Marquardt minimisation.
 * \param lm Pointer to a Levenberg-Marquardt structure
 *
 * Returns the state of a Levenberg-Marquardt minimisation. The vector is not
 * copied, but is rather passed back directly from the state structure.
 * There is therefore no need to free the vector after use. It is essential
 * also that the contents of the vector are not modified; i.e. the vector
 * should be treated as read only.
 *
 * \return Pointer to current solution vector or <b>NULL</b> on failure.
 * \sa gan_lev_marq_get_P().
 */
Gan_Vector *
 gan_lev_marq_get_x ( Gan_LevMarqStruct *lm )
{
   /* make sure residual is valid */
   gan_err_test_ptr ( lm->residual >= 0.0, "gan_lev_marq_get_x",
                      GAN_ERROR_INCOMPATIBLE, "" );

   /* return state vector */
   return &lm->x;
}

/**
 * \brief Returns state covariance of Levenberg-Marquardt minimisation.
 * \param lm Pointer to a Levenberg-Marquardt structure
 *
 * Returns the state covariance of a Levenberg-Marquardt minimisation.
 * The matrix is not copied, but is rather passed back directly from the state
 * structure. There is therefore no need to free the matrix after use.
 * It is essential also that the contents of the matrix are not modified;
 * i.e. the matrix should be treated as read only.
 *
 * \return Pointer to current solution covariance or <b>NULL</b> on failure.
 * \sa gan_lev_marq_get_x().
 */
Gan_SquMatrix *
 gan_lev_marq_get_P ( Gan_LevMarqStruct *lm )
{
   /* make sure residual is valid */
   gan_err_test_ptr ( lm->residual >= 0.0, "gan_lev_marq_get_P",
                      GAN_ERROR_INCOMPATIBLE, "" );

   /* return state vector covariance */
   return &lm->P;
}

static void lev_marq_obs_free ( Gan_LevMarqObs *obs )
{
   switch ( obs->type )
   {
      case GAN_LEV_MARQ_OBS_H:
      case GAN_LEV_MARQ_OBS_H_ROBUST:
        gan_vec_free ( &obs->details.h.z );
        gan_squmat_free ( &obs->details.h.Ni );
        break;

      case GAN_LEV_MARQ_OBS_F:
        gan_vec_free ( &obs->details.F.z );
        gan_squmat_free ( &obs->details.F.N );
        break;

      default:
        assert(0);
        break;
   }

   free ( obs );
}

/**
 * \brief Frees a Levenberg-Marquardt structure.
 * \param lm Pointer to Levenberg-Marquardt structure
 *
 * Frees a structure created to compute Levenberg-Marquardt minimisation.
 * \return None
 * \sa gan_lev_marq_form().
 */
void
 gan_lev_marq_free ( Gan_LevMarqStruct *lm )
{
   /* free fields of structure followed by the structure itself */
   gan_squmat_free ( &lm->Ni );
   gan_vec_free ( &lm->HxTNiv );
   gan_vec_free ( &lm->Niv );
   gan_squmat_free ( &lm->HxTNiHx );
   gan_mat_free ( &lm->NiHx );
   gan_mat_free ( &lm->Hz );
   gan_mat_free ( &lm->Hx );
   gan_vec_free ( &lm->hF );
   gan_squmat_free ( &lm->P );
   gan_vec_free ( &lm->a );
   gan_squmat_free ( &lm->A );
   gan_vec_free ( &lm->xold );
   gan_vec_free ( &lm->x );
   gan_list_free ( lm->obs_list, (void(*)(void *)) lev_marq_obs_free );
   if ( lm->alloc ) free ( lm );
}

/**
 * \}
 */

/**
 * \}
 */
