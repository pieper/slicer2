/*
 * File:          $RCSfile: mat_norm_noc.c,v $
 * Module:        Fixed size matrix projective normalisation
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:23 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Not to be compiled separately
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

/* normalises an array of matrices B */
Gan_Bool
 GAN_MAT_NORMALISE ( GAN_MATTYPE *B, int n, double term_threshold,
                     int max_iterations, GAN_SQUMATTYPE *Lp )
{
   GAN_SQUMATTYPE S, Sp, Si, BTB, L;
   GAN_SQUMATTYPEP BSiBT;
   GAN_MATTYPE BSi;
   int i, it;
   double norm;

   /* starting point for S matrix is always identity */
   S = GAN_SYMMAT_IDENT_S();

   /* compute S=L*L^T using iterative algorithm */
   for ( it = max_iterations-1; it >= 0; it-- )
   {
      /* compute S^-1 */
      GAN_SYMMAT_INVERT_Q ( &S, &Si );

      /* compute Sp = sum of (B_i^T*B_i) / ||B_i*S^-1*B_i^T!!_F where _F
         denotes the Frobenius norm of a matrix */
      (void)GAN_SYMMAT_ZERO_Q ( &Sp );
      for ( i = n-1; i >= 0; i-- )
      {
         /* compute BTB = B_i^T*B_i */
         (void)GAN_MAT_SLMULTT_Q ( &B[i], &BTB );

         /* compute BSiBT = B_i*S^-1*B_i^T */
         (void)GAN_SYMMAT_LRMULTM_Q ( &Si, &B[i], &BSi, &BSiBT );

         /* scale BTB using inverse of trace of BSiBT */
         norm = GAN_SYMMATP_TRACE_S(&BSiBT);
         if(norm == 0.0)
         {
            gan_err_flush_trace();
            gan_err_register ( GAN_MAT_NORM_FNAME, GAN_ERROR_DIVISION_BY_ZERO, "" );
            return GAN_FALSE;
         }

         GAN_SYMMAT_DIVIDE_S ( &BTB, norm );

         /* increment sum by adjusting Sp */
         (void)GAN_SYMMAT_INCREMENT ( &Sp, &BTB );
      }

      /* scale new S (Sp) by its trace to normalise it */
      norm = GAN_SYMMAT_TRACE_S(&Sp);
      if(norm == 0.0)
      {
         gan_err_flush_trace();
         gan_err_register ( GAN_MAT_NORM_FNAME, GAN_ERROR_DIVISION_BY_ZERO, "" );
         return GAN_FALSE;
      }

      Sp = GAN_SYMMAT_DIVIDE_S ( &Sp, norm );

      /* break out of loop if the Frobenius norm of the difference between
         the old S and new S (Sp) is smaller than a threshold */
      (void)GAN_SYMMAT_SUB_Q ( &S, &Sp, &Si );
      if ( GAN_SYMMAT_SUMSQR_Q(&Si) < term_threshold )
         break;

      /* replace S by new solution Sp */
      S = Sp;
   }
   
   /* if all the iterations were completed, the algorithm didn't converge,
      so return with failure */
   if ( it < 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( GAN_MAT_NORM_FNAME, GAN_ERROR_NO_CONVERGENCE, "" );
      return GAN_FALSE;
   }

   /* use Cholesky factorisation to compute L where L*L^T = S */
   if ( GAN_SYMMAT_CHOLESKY_Q ( &S, &L ) == NULL )
   {
      gan_err_register ( GAN_MAT_NORM_FNAME, GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* right-multiply each matrix by L^-T and scale to unit Frobenius norm */
   for ( i = n-1; i >= 0; i-- )
   {
      B[i] = GAN_MAT_RMULTLIT_S ( &B[i], &L );
      B[i] = GAN_MAT_DIVIDE_S ( &B[i], GAN_MAT_FNORM_S(&B[i]) );
   }

   /* store L if requested */
   if ( Lp != NULL ) *Lp = L;

   return GAN_TRUE;
}

#ifdef GAN_MATT_NORMALISE

/* normalises an array of transposes of matrices B */
Gan_Bool
 GAN_MATT_NORMALISE ( GAN_MATTYPE *B, int n, double term_threshold,
                      int max_iterations, GAN_SQUMATTYPE *Lp )
{
   Gan_Bool result;
   int i;

   /* transpose each matrix in-place */
   for ( i = n-1; i >= 0; i-- )
      GAN_MAT_TPOSE_I ( &B[i] );

   /* apply normalisation */
   result = GAN_MAT_NORMALISE ( B, n, term_threshold, max_iterations, Lp );

   /* undo matrix transposition */
   for ( i = n-1; i >= 0; i-- )
      GAN_MAT_TPOSE_I ( &B[i] );

   return result;
}

#endif /* #ifdef GAN_MATT_NORMALISE */
