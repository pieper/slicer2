/**
 * File:          $RCSfile: pseudo_inverse.c,v $
 * Module:        Psseudo-inverse solution of linear system (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:23 $
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

#include <gandalf/linalg/pseudo_inverse.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/linalg/mat_diagonal.h>
#include <gandalf/linalg/mat_triangular.h>
#include <gandalf/linalg/mat_gen.h>
#include <gandalf/linalg/vec_gen.h>
#include <gandalf/linalg/mat_lq.h>
#include <gandalf/linalg/matvec_clapack.h>
#include <gandalf/linalg/matvec_blas.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/compare.h>

/**
 * \addtogroup LinearAlgebra
 * \{
 */

/**
 * \addtogroup GeneralSizeMatVec
 * \{
 */

/**
 * \addtogroup GeneralSizeMatrix
 * \{
 */

/**
 * \defgroup GeneralSizeMatrixPseudoInverse Pseudo-Inverse of a General Size Matrix
 * \{
 */

#define gel(M,i,j)   gan_mat_get_el(M,i,j)
#define sel(M,i,j,v) gan_mat_set_el(M,i,j,v)
#define sgel(M,i,j)   gan_squmat_get_el(M,i,j)
#define ssel(M,i,j,v) gan_squmat_set_el(M,i,j,v)
#define vgel(M,i)   gan_vec_get_el(M,i)
#define vsel(M,i,v) gan_vec_set_el(M,i,v)

static void init_extras ( Gan_PseudoInverseStruct *pis )
{
   /* no constraints initially */
   pis->C = pis->D = NULL;

   /* null-space has zero dimension by default */
   pis->null_space = 0;

   /* ratio threshold is zero by default, turning off its effect */
   pis->ratio_thres = 0.0;

   /* number of low singular values allowed is zero by default,
      turning off its effect */
   pis->low_values_allowed = 0;

   /* initialise left singular vector matrix to NULL */
   pis->U = NULL;

   /* no null-vectors initially */
   pis->null_vector = NULL;
}

static void free_extras ( Gan_PseudoInverseStruct *pis )
{
   if ( pis->null_vector != NULL ) gan_vec_free ( pis->null_vector );
   if ( pis->U != NULL ) gan_mat_free ( pis->U );
   if ( pis->D != NULL ) gan_mat_free_va ( pis->D, pis->C, NULL );
}

/**
 * \brief Forms a pseudo-inverse equation solving structure.
 * \param pis A pointer to a structure or \c NULL
 * \param max_asize The envisaged largest size of the matrix
 * \param max_bsize The envisaged largest number of solution vectors
 * \return Pointer to structure, or \c NULL on failure.
 *
 * Allocates/fills a structure with initial values to compute the
 * pseudo-inverse solution of a system of linear equations.
 *
 * \sa gan_pseudoinv_alloc().
 */
Gan_PseudoInverseStruct *
 gan_pseudoinv_form ( Gan_PseudoInverseStruct *pis,
                      long max_asize, long max_bsize )
{
   if ( pis == NULL )
   {
      /* allocate structure dynamically */
      pis = gan_malloc_object(Gan_PseudoInverseStruct);
      if ( pis == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_pseudoinv_form", GAN_ERROR_MALLOC_FAILED, "", sizeof(*pis) );
         return NULL;
      }

      pis->alloc = GAN_TRUE;
   }
   else
      pis->alloc = GAN_FALSE;

   /* allocate matrices to be accumulated and solution matrices */
   if ( gan_symmat_form ( &pis->SaaT, max_asize ) == NULL ||
        gan_mat_form ( &pis->SabT, max_asize, max_bsize ) == NULL ||
        gan_ltmat_form ( &pis->L, max_asize ) == NULL ||
        gan_mat_form ( &pis->X, max_asize, max_bsize ) == NULL )
   {
      gan_err_register ( "gan_pseudoinv_form", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* allocate individual vectors */
   if ( gan_vec_form ( &pis->a, max_asize ) == NULL ||
        gan_vec_form ( &pis->b, max_bsize ) == NULL )
   {
      gan_err_register ( "gan_pseudoinv_form", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* initialise other fields */
   init_extras ( pis );

   /* clear accumulated equation sums to zero */
   if ( !gan_pseudoinv_reset ( pis, max_asize, max_bsize ) )
   {
      gan_err_register ( "gan_pseudoinv_form", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* successful exit */
   return pis;
}

/**
 * \brief Frees a pseudo-inverse equation solving structure.
 * \param pis A pointer to a structure
 * \return No value.
 *
 * Frees a structure created to compute the pseudo-inverse solution of a system
 * of linear equations.
 *
 * \sa gan_pseudoinv_alloc().
 */
void
 gan_pseudoinv_free ( Gan_PseudoInverseStruct *pis )
{
   /* free fields of structure followed by the structure itself */
   free_extras ( pis );
   gan_vec_free ( &pis->b );
   gan_vec_free ( &pis->a );
   gan_mat_free ( &pis->X );
   gan_squmat_free ( &pis->L );
   gan_mat_free ( &pis->SabT );
   gan_squmat_free ( &pis->SaaT );
   if ( pis->alloc ) free ( pis );
}

/**
 * \brief Initialises a pseudo-inverse equation solver to zero.
 * \param pis A pointer to a structure
 * \param asize The size of the matrix (number of solution elements)
 * \param bsize The number of solution vectors
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Initialises a structure to compute the pseudo-inverse solution of a system
 * of linear equations. The size of the matrix and the number of solutions
 * is also set.
 *
 * \sa gan_pseudoinv_alloc().
 */
Gan_Bool
 gan_pseudoinv_reset ( Gan_PseudoInverseStruct *pis, long asize, long bsize )
{
   /* reset sum of a*a^T and a*b^T to zero, setting the sizes of each
      as requested, and testing for errors */
   if ( gan_symmat_set_size ( &pis->SaaT, asize ) == NULL ||
        gan_symmat_fill_const_q ( &pis->SaaT, asize, 0.0 ) == NULL ||
        gan_mat_fill_const_q ( &pis->SabT, asize, bsize, 0.0 ) == NULL ||
        gan_vec_set_size ( &pis->a, asize ) == NULL ||
        gan_vec_set_size ( &pis->b, bsize ) == NULL )
   {
      gan_err_register ( "gan_pseudoinv_reset", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* not solved yet, obviously */
   pis->solved = GAN_FALSE;

   /* initialise other fields */
   free_extras ( pis );
   init_extras ( pis );

   return GAN_TRUE;
}

/**
 * \brief Decrements a pseudo-inverse equation solver.
 * \param pis A pointer to a structure
 * \param scale A scaling for the equations
 * \param ... A variable argument list of vector components
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Increments a system of linear equations
 * \f[
 *     A X = B
 * \f]
 * to be solved by pseudo-inverse:
 * \f[
 *    X = (A^{\top} A)^{-1} A^{\top} B
 * \f]
 * This function takes the variable argument list of elements of a vectors
 * \a a and \a b, of sizes equal to the \a asize and \a bsize arguments to
 * the most recent call to gan_pseudoinv_reset() on the \a pis structure.
 * The matrix \f$ A^{\top} A \f$ is incremented by \f$ s\:a\:a^{\top} \f$,
 * where \f$ s \f$ is the \a scale value, and \f$ A^{\top} B \f$ is
 * incremented by \f$ s\:a\:b^{\top} \f$.
 *
 * \sa gan_pseudoinv_alloc().
 */
Gan_Bool
 gan_pseudoinv_increment ( Gan_PseudoInverseStruct *pis, double scale, ... )
{
   va_list ap;
   unsigned long i;

   /* read variable argument list into temporary vector */
   va_start ( ap, scale );
   for ( i = 0; i < pis->a.rows; i++ )
      gan_vec_set_el ( &pis->a, i, va_arg ( ap, double ) );

   for ( i = 0; i < pis->b.rows; i++ )
      gan_vec_set_el ( &pis->b, i, va_arg ( ap, double ) );

   va_end ( ap );

   /* accumulate pseudo-inverse matrices (A * A^T) and (A * B^T) */
   if ( gan_blas_spr ( &pis->SaaT, &pis->a, scale ) == NULL ||
        gan_blas_ger ( &pis->SabT, &pis->a, &pis->b, scale ) == NULL )
   {
      gan_err_register ( "gan_pseudoinv_increment", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Decrements a pseudo-inverse equation solver.
 * \param pis A pointer to a structure
 * \param scale A scaling for the equations
 * \param ... A variable argument list of vector components
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Decrements a system of linear equations
 * \f[
 *    A\:X = B
 * \f]
 * to be solved by pseudo-inverse:
 * \f[
 *     X = (A^{\top} A)^{-1} A^{\top} B
 * \f]
 * This function takes the variable argument list of elements of a vectors \a a
 * and \a b, of sizes equal to the \a asize and \a bsize arguments to the most
 * recent call to gan_pseudoinv_reset() on the \a pis structure.
 * The matrix \f$ (A^{\top} A) \f$ is decremented by \f$ s\:a\:a^{\top} \f$,
 * where \f$ s \f$ is the value \a scale, and \f$ (A^{\top} B) \f$ is
 * decremented by \f$ s\:a\:b^{\top} \f$. This function is used to remove data
 * added using gan_pseudoinv_increment().
 *
 * \sa gan_pseudoinv_alloc().
 */
Gan_Bool
 gan_pseudoinv_decrement ( Gan_PseudoInverseStruct *pis, double scale, ... )
{
   va_list ap;
   unsigned long i;

   /* read variable argument list into temporary vector */
   va_start ( ap, scale );
   for ( i = 0; i < pis->a.rows; i++ )
      gan_vec_set_el ( &pis->a, i, va_arg ( ap, double ) );

   for ( i = 0; i < pis->b.rows; i++ )
      gan_vec_set_el ( &pis->b, i, va_arg ( ap, double ) );

   va_end ( ap );

   /* accumulate pseudo-inverse matrices (A * A^T) and (A * B^T) */
   if ( gan_blas_spr ( &pis->SaaT, &pis->a, -scale ) == NULL ||
        gan_blas_ger ( &pis->SabT, &pis->a, &pis->b, -scale ) == NULL )
   {
      gan_err_register ( "gan_pseudoinv_decrement", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Build a constraint into a system of linear equations.
 * \param pis A pointer to the structure containing the equations
 * \param C a pointer to a LHS constraint matrix
 * \param D a pointer to an RHS constraint matrix
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Applies the constraint
 * \f[
 *    C\:X = D
 * \f]
 * to any subsequently computed slution of the equations being built in the
 * pis structure.
 *
 * \sa gan_pseudoinv_solve().
 */
Gan_Bool
 gan_pseudoinv_set_constraint ( Gan_PseudoInverseStruct *pis,
                                Gan_Matrix *C, Gan_Matrix *D )
{
   gan_err_test_bool ( C->cols == pis->SaaT.size &&
                       D->cols == pis->SabT.cols && C->rows == D->rows,
                       "gan_pseudoinv_set_constraint", GAN_ERROR_INCOMPATIBLE,
                       "" );

   if ( pis->C == NULL )
   {
      gan_err_test_bool ( pis->D == NULL,
                          "gan_pseudoinv_set_constraint",
                          GAN_ERROR_INCOMPATIBLE, "" );

      /* copy matrices into pseudo-inverse structure */
      if ( (pis->C = gan_mat_copy_s ( C )) == NULL ||
           (pis->D = gan_mat_copy_s ( D )) == NULL )
      {
         gan_err_register ( "gan_pseudoinv_set_constraint", GAN_ERROR_FAILURE,
                            "" );
         return GAN_FALSE;
      }
   }
   else
   {
      /* constraints already exist. Add new constraints to existing ones */
      Gan_Matrix *Ctmp, *Dtmp;

      /* make copies of existing constraint matrices */
      if ( (Ctmp = gan_mat_copy_s ( pis->C )) == NULL ||
           (Dtmp = gan_mat_copy_s ( pis->D )) == NULL )
      {
         gan_err_register ( "gan_pseudoinv_set_constraint", GAN_ERROR_FAILURE,
                            "" );
         return GAN_FALSE;
      }

      /* adjust size of constraint matrices */
      if ( gan_mat_set_dims ( pis->C, Ctmp->rows+C->rows, C->cols ) == NULL ||
           gan_mat_set_dims ( pis->D, Dtmp->rows+D->rows, D->cols ) == NULL )
      {
         gan_err_register ( "gan_pseudoinv_set_constraint", GAN_ERROR_FAILURE,
                            "" );
         return GAN_FALSE;
      }

      /* insert original constraints in constraint matrices */
      if ( gan_mat_insert ( pis->C, 0, 0, Ctmp, 0, 0, Ctmp->rows, Ctmp->cols )
           == NULL ||
           gan_mat_insert ( pis->D, 0, 0, Dtmp, 0, 0, Dtmp->rows, Dtmp->cols )
           == NULL )
      {
         gan_err_register ( "gan_pseudoinv_set_constraint", GAN_ERROR_FAILURE,
                            "" );
         return GAN_FALSE;
      }

      /* insert new constraints in constraint matrices */
      if ( gan_mat_insert ( pis->C, Ctmp->rows, 0, C, 0, 0, C->rows, C->cols )
           == NULL ||
           gan_mat_insert ( pis->D, Dtmp->rows, 0, D, 0, 0, D->rows, D->cols )
           == NULL )
      {
         gan_err_register ( "gan_pseudoinv_set_constraint", GAN_ERROR_FAILURE,
                            "" );
         return GAN_FALSE;
      }

      /* free copies of old constraints */
      gan_mat_free_va ( Dtmp, Ctmp, NULL );
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Sets the size of the null space in a system of linear equations.
 * \param pis A pointer to the structure containing the equations
 * \param null_space The dimension of the null space
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Indicates that the solution of an accumulated system of linear equations
 * has a null-space with the given dimension.
 *
 * \sa gan_pseudoinv_solve().
 */
Gan_Bool
 gan_pseudoinv_set_null_space ( Gan_PseudoInverseStruct *pis, int null_space )
{
   /* check that the size of the null_space is consistent */
   gan_err_test_bool ( null_space >= 0 && null_space < (int)pis->SaaT.size,
                       "gan_pseudoinv_set_null_space", GAN_ERROR_INCOMPATIBLE,
                       "" );

   /* set null-space */
   pis->null_space = null_space;

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Sets the threshold on singular value ratios.
 * \param pis A pointer to the structure containing the equations
 * \param ratio_thres The threshold
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Sets a threshold so that when the Moore-Penrose pseudo-inverse is
 * calculated, the singular values are compared with the highest singular
 * values. If they are less than \a ratio_thres times the highest singular
 * value, the low singular values are effectively set to infinity for the
 * purpose of inverse.
 *
 * The default value for \a ratio_thres is zero, eliminating its effect.
 *
 * \sa gan_pseudoinv_solve().
 */
Gan_Bool
 gan_pseudoinv_set_ratio_thres ( Gan_PseudoInverseStruct *pis,
                                 double ratio_thres )
                               
{
   /* set ratio threshold */
   pis->ratio_thres = ratio_thres;

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Sets the number of allowed low singular values.
 * \param pis A pointer to the structure containing the equations
 * \param low_values_allowed The number of low singular values allowed
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Sets the number of small singular values allowed when solving for the
 * pseudo-inverse solution of a set of equations, where "small" is in the
 * sense of the singular value ratio threshold between the tested value and
 * the largest singular value, compared with the value set by
 * gan_pseudoinv_set_ratio_thres(). Values within the allowed number threshold
 * defined here are set to zero on inverting the system matrix.
 *
 * The default value for this parameter is zero, effectively nullifying its
 * effect. If it is set to a non-zero value then
 * gan_pseudoinv_set_ratio_thres() should also be used to set the singular
 * value ratio threshold.
 *
 * \sa gan_pseudoinv_set_ratio_thres().
 */
Gan_Bool
 gan_pseudoinv_set_low_values_allowed ( Gan_PseudoInverseStruct *pis,
                                        int low_values_allowed )
{
   /* set allowed number of small singular values */
   pis->low_values_allowed = low_values_allowed;

   /* success */
   return GAN_TRUE;
}

/* find pseudo-inverse solution to unconstrained linear least squares */
static Gan_Bool pseudoinv_solve_no_constraint ( Gan_PseudoInverseStruct *pis,
                                                int *error_code )
{
   /* consistency checks */
   gan_err_test_bool ( pis->D == NULL, "pseudoinv_solve_no_constraint",
                       GAN_ERROR_FAILURE, "" );
   gan_err_test_bool ( pis->low_values_allowed == 0 ||
                       pis->ratio_thres != 0.0F,
                       "pseudoinv_solve_no_constraint",
                       GAN_ERROR_FAILURE, "" );
   gan_err_test_bool ( pis->low_values_allowed + pis->null_space
                       < pis->SaaT.size, "pseudoinv_solve_no_constraint",
                       GAN_ERROR_FAILURE, "" );
   if ( pis->null_space == 0 && pis->ratio_thres == 0.0 )
   {
      /* compute Cholesky factorisation of matrix A^T * A */
      if ( pis->SaaT.funcs->cholesky ( &pis->SaaT, &pis->L, error_code )
           == NULL )
      {
         if ( error_code == NULL )
            gan_err_register ( "pseudoinv_solve_no_constraint",
                               GAN_ERROR_FAILURE, "" );
         else
            *error_code = GAN_ERROR_FAILURE;

         return GAN_FALSE;
      }

      /* compute solution X by left-multiplying A*B^T by (A*A^T)^-1 in two
         stages */
      if ( gan_squmatI_rmult_q ( &pis->L, &pis->SabT, &pis->X ) == NULL ||
           gan_squmatIT_rmult_i ( &pis->L, &pis->X ) == NULL )
      {
         if ( error_code == NULL )
            gan_err_register ( "pseudoinv_solve_no_constraint",
                               GAN_ERROR_FAILURE, "" );
         else
            *error_code = GAN_ERROR_FAILURE;

         return GAN_FALSE;
      }
   }
   else
   {
      /* eigensystem decomposition matrices */
      Gan_SquMatrix *SaaT, *W;
      Gan_Matrix *workm;
      int size = (int)pis->SaaT.size, i;

      /* allocate matrices for eigenvalues and eigenvectors */
      if ( pis->U == NULL )
         pis->U = gan_mat_alloc ( size, size );
      else
         pis->U = gan_mat_set_dims ( pis->U, size, size );

      W = gan_diagmat_alloc ( size );
      workm = gan_mat_alloc ( 3*size, 1 );
      SaaT = gan_squmat_copy_s ( &pis->SaaT );

      if ( pis->U == NULL || W == NULL || workm == NULL || SaaT == NULL )
      {
         gan_err_register ( "pseudoinv_solve_no_constraint",
                            GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      /* compute eigenvalues & aigenvectors */
      if ( !gan_symmat_eigen ( SaaT, W, pis->U, GAN_TRUE,
                               workm->data, workm->data_size ) )
      {
         gan_err_register ( "pseudoinv_solve_no_constraint",
                            GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      /* zero off singular values marked as part of null-space */
      for ( i = (int)pis->null_space-1; i >= 0; i-- )
         ssel ( W, i, i, 0.0 );

      /* check ratio of smallest non-zero singular value to largest */
      if ( sgel ( W, pis->null_space+pis->low_values_allowed,
                     pis->null_space+pis->low_values_allowed )
           < pis->ratio_thres*sgel(W,size-1,size-1) )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "pseudoinv_solve_no_constraint",
                               GAN_ERROR_TOO_SMALL, "" );
         }
         else
            *error_code = GAN_ERROR_TOO_SMALL;

         gan_mat_free ( workm );
         gan_squmat_free_va ( SaaT, W, NULL );
         return GAN_FALSE;
      }

      /* check off singular values in range of allowed singular values */
      for ( i = 0; i < pis->low_values_allowed; i++ )
         if ( sgel ( W, pis->null_space+i, pis->null_space+i )
              < pis->ratio_thres*sgel(W,size-1,size-1) )
            ssel ( W, pis->null_space+i, pis->null_space+i, 0.0 );
         else
            /* other singular values must be bigger, so abort the loop */
            break;

      /* compute pseudo-inverse of matrix by inverting singular values
         not part of null-space */
      for ( i += pis->null_space; i < size; i++ )
      {
         if ( sgel(W,i,i) == 0.0 )
         {
            gan_mat_free ( workm );
            gan_squmat_free_va ( SaaT, W, NULL );
            if ( error_code == NULL )
               gan_err_register ( "pseudoinv_solve_no_constraint",
                                  GAN_ERROR_DIVISION_BY_ZERO, "" );
            else
               *error_code = GAN_ERROR_DIVISION_BY_ZERO;

            return GAN_FALSE;
         }

         ssel ( W, i, i, 1.0/sgel(W,i,i) );
      }

      /* compute special solution V*W^-1*U^T*A^T*B into solution field
         of structure */
      gan_symmat_lrmult_q ( W, pis->U, workm, SaaT );
      gan_squmat_rmult_q ( SaaT, &pis->SabT, &pis->X );

      /* free temporary matrices */
      gan_mat_free ( workm );
      gan_squmat_free_va ( SaaT, W, NULL );
   }

   /* system now solved */
   pis->solved = GAN_TRUE;

   /* success */
   return GAN_TRUE;
}

/* find pseudo-inverse solution to constrained linear least squares, using
 * the method described in Golub & Van Load, 2nd edition, pages 585-6.
 */
static Gan_Bool pseudoinv_solve_constraint ( Gan_PseudoInverseStruct *pis,
                                             int *error_code )
{
   /* matrices used for QR factorisation */
   Gan_Matrix *Q, *Y, *Qtop, *Qbot, *AAQ, *Z, *QY, *QAAQY;
   Gan_SquMatrix *L, *QAAQ;

   gan_err_test_bool ( pis->null_space == 0 && pis->ratio_thres == 0.0 &&
                       pis->low_values_allowed == 0,
                       "pseudoinv_solve_constraint", GAN_ERROR_FAILURE, "" );
   assert ( pis->D != NULL );
   gan_err_test_bool ( pis->C->cols == pis->SaaT.size &&
                       pis->D->cols == pis->SabT.cols &&
                       pis->C->rows == pis->D->rows,
                       "pseudoinv_solve_constraint", GAN_ERROR_INCOMPATIBLE,
                       "" );

   /* compute LQ factorisation of C */
   Q = gan_mat_alloc ( pis->C->cols, pis->C->cols );
   L = gan_ltmat_alloc ( pis->C->rows );
   if ( Q == NULL && L == NULL )
   {
      gan_err_register ( "pseudoinv_solve_constraint", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   if ( !gan_mat_lq ( pis->C, L, Q, NULL, 0 ) )
   {
      gan_err_register ( "pseudoinv_solve_constraint", GAN_ERROR_FAILURE, "" );
      gan_squmat_free(L);
      gan_mat_free(Q);
      return GAN_FALSE;
   }

   /* solve for Y such that L*Y = D */
   Y = gan_squmatI_rmult_s ( L, pis->D );

   /* split the Q matrix into bottom and top parts */
   Qtop = gan_mat_extract_s ( Q, 0, 0, pis->C->rows, pis->C->cols );
   Qbot = gan_mat_extract_s ( Q, pis->C->rows, 0,
                              pis->C->cols-pis->C->rows, pis->C->cols );

   /* test for errors */
   if ( Y == NULL || Qtop == NULL || Qbot == NULL )
   {
      gan_err_register ( "pseudoinv_solve_constraint", GAN_ERROR_FAILURE, "" );
      gan_squmat_free(L);
      gan_mat_free(Q);
      return GAN_FALSE;
   }

   /* compute Qbot*SaaT*Qbot^T */
   AAQ = gan_mat_alloc(pis->SaaT.size, Qbot->rows);
   QAAQ = gan_symmat_lrmult_s ( &pis->SaaT, Qbot, AAQ );

   /* test for errors */
   if ( AAQ == NULL || QAAQ == NULL )
   {
      gan_err_register ( "pseudoinv_solve_constraint", GAN_ERROR_FAILURE, "" );
      gan_mat_free_va ( Qbot, Qtop, Y, NULL );
      gan_squmat_free(L);
      gan_mat_free(Q);
      return GAN_FALSE;
   }

   /* compute Cholesky factorisation of Qbot*SaaT*Qbot^T */
   if ( gan_squmat_cholesky_i ( QAAQ ) == NULL )
   {
      gan_err_register ( "pseudoinv_solve_constraint", GAN_ERROR_FAILURE, "" );
      gan_squmat_free ( QAAQ );
      gan_mat_free ( AAQ );
      gan_mat_free_va ( Qbot, Qtop, Y, NULL );
      gan_squmat_free(L);
      gan_mat_free(Q);
      return GAN_FALSE;
   }

   /* compute Qbot*SabT and Qtop^T*Y, then compute Qbot*SaaT*Qtop^T*Y and
      subtract it from Qbot*SabT */
   Z = gan_mat_rmult_s ( Qbot, &pis->SabT );
   QY = gan_matT_rmult_s ( Qtop, Y );
   if ( QY != NULL )
   {
      QAAQY = gan_matT_rmult_s ( AAQ, QY );
      if ( QAAQY != NULL )
         gan_mat_decrement ( Z, QAAQY );
   }

   /* test for errors */
   if ( Z == NULL || QY == NULL || QAAQY == NULL )
   {
      gan_err_register ( "pseudoinv_solve_constraint", GAN_ERROR_FAILURE, "" );
      gan_squmat_free ( QAAQ );
      gan_mat_free ( AAQ );
      gan_mat_free_va ( Qbot, Qtop, Y, NULL );
      gan_squmat_free(L);
      gan_mat_free(Q);
      return GAN_FALSE;
   }

   gan_mat_free ( QAAQY );

   /* left-multiply Z by (Qbot*SaaT*Qbot^T)^-1 */
   Z = gan_squmatI_rmult_i ( QAAQ, Z );
   if ( Z != NULL )
      Z = gan_squmatIT_rmult_i ( QAAQ, Z );

   /* test for errors */
   if ( Z == NULL )
   {
      gan_err_register ( "pseudoinv_solve_constraint", GAN_ERROR_FAILURE, "" );
      gan_squmat_free ( QAAQ );
      gan_mat_free ( AAQ );
      gan_mat_free_va ( Qbot, Qtop, Y, NULL );
      gan_squmat_free(L);
      gan_mat_free(Q);
      return GAN_FALSE;
   }

   /* we have computed Y and Z; now build solution X = Qtop^T*Z + Qbot^T*Y */
   gan_matT_rmult_q ( Qbot, Z, &pis->X );
   gan_mat_increment ( &pis->X, QY );

   /* system now solved */
   pis->solved = GAN_TRUE;

   /* free temporary matrices */
   gan_mat_free_va ( QY, Z, NULL );
   gan_squmat_free ( QAAQ );
   gan_mat_free ( AAQ );
   gan_mat_free_va ( Qbot, Qtop, Y, NULL );

   /* free LQ factorisation matrices */
   gan_squmat_free(L);
   gan_mat_free(Q);

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Solve an accumulated system of linear equations.
 * \param pis A pointer to the structure containing the equations
 * \param error_code A pointer to be filled with an error code or \c NULL
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Solves the set of equations
 * \f[
 *   A\:X = B
 * \f]
 * for matrix \f$ X \f$. If there are any constraints \f$ C\:X = D \f$ they are
 * incorporated.
 *
 * If an error occurs, the error handler is invoked unless \a error_code is
 * not \c NULL, in which case the error code is passed back in the
 * \a error_code pointer. In either case #GAN_FALSE is returned to indicate
 * the error.
 *
 * \sa gan_pseudoinv_alloc(), gan_pseudoinv_increment().
 */
Gan_Bool
 gan_pseudoinv_solve ( Gan_PseudoInverseStruct *pis, int *error_code )
{
   Gan_Bool result;

   /* check whether constraints have been imposed */
   if ( pis->C == NULL )
      result = pseudoinv_solve_no_constraint ( pis, error_code );
   else
      result = pseudoinv_solve_constraint ( pis, error_code );

   if ( !result && error_code == NULL )
      gan_err_register ( "gan_pseudoinv_solve", GAN_ERROR_FAILURE, "" );

   return result;
}

/**
 * \brief Return solution of an accumulated system of linear equations.
 * \param pis A pointer to the structure containing the equations
 * \return Pointer to the solution matrix, or \c NULL on failure.
 *
 * Returns the solution computed by gan_pseudoinv_solve(). The returned matrix
 * is returned directly from the \a pis structure, so should not be freed by
 * the calling program.
 *
 * \sa gan_pseudoinv_solve().
 */
Gan_Matrix *
 gan_pseudoinv_get_solution ( Gan_PseudoInverseStruct *pis )
{
   gan_err_test_ptr ( pis->solved, "gan_pseudoinv_get_solution",
                      GAN_ERROR_FAILURE, "" );
   return &pis->X;
}

/**
 * \brief Return a null vector from the solution space.
 * \param pis A pointer to the structure containing the equations
 * \return The null-space vector, or \c NULL on failure.
 *
 * Returns a null vector from the solution space of the given system of linear
 * equations, on which The returned matrix is returned directly from the pis
 * structure, so should not be freed by the calling program.
 *
 * \sa gan_pseudoinv_solve().
 */
Gan_Vector *
 gan_pseudoinv_get_null_vector ( Gan_PseudoInverseStruct *pis )
{
   gan_err_test_ptr ( pis->solved, "gan_pseudoinv_get_null_vector",
                      GAN_ERROR_FAILURE, "" );
   gan_err_test_ptr ( pis->null_space > 0 &&
                      pis->null_space < (int)pis->SaaT.size,
                      "gan_pseudoinv_get_null_vector", GAN_ERROR_FAILURE, "" );

   /* allocate/set null vector */
   if ( pis->null_vector == NULL )
      pis->null_vector = gan_vec_alloc ( pis->SaaT.size );
   else
      gan_vec_set_size ( pis->null_vector, pis->SaaT.size );

   /* retrieve null vector from previously computed solution */
   gan_vec_insert_mat ( pis->null_vector, 0,
                        pis->U, 0, pis->null_space-1, pis->SaaT.size );

   /* one less null vector to retrieve */
   pis->null_space--;

   return ( pis->null_vector );
}

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */
