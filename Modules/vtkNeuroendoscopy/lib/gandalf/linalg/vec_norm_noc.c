/*
 * File:          $RCSfile: vec_norm_noc.c,v $
 * Module:        Fixed size vector projective normalisation
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:24 $
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

/* normalises an array of vectors b */
Gan_Bool
 GAN_VEC_NORMALISE ( GAN_VECTYPE *b, int n, double term_threshold,
                     int max_iterations, GAN_SQUMATTYPE *Lp )
{
   GAN_SQUMATTYPE S, Sp, Si, bbT, L;
   GAN_VECTYPE    Sib;
   double         norm;
   int i, it;

   /* starting point for S matrix is always identity */
   S = GAN_SYMMAT_IDENT_S();

   /* compute S=L*L^T using iterative algorithm */
   for ( it = max_iterations-1; it >= 0; it-- )
   {
      /* compute S^-1 */
      GAN_SYMMAT_INVERT_Q ( &S, &Si );

      /* compute Sp = sum of (b_i*b_i^T) / (b_i^T*S^-1*b_i) */
      (void)GAN_SYMMAT_ZERO_Q ( &Sp );
      for ( i = n-1; i >= 0; i-- )
      {
         /* compute bbT = b*b^T */
         (void)GAN_VEC_OUTER_SYM_Q ( &b[i], &bbT );

         /* compute Sib = S^-1*b */
         (void)GAN_SYMMAT_MULTV_Q ( &Si, &b[i], &Sib );

         /* compute bTSib = b^T*S^-1*b */
         norm = GAN_VEC_DOT_S ( &b[i], &Sib );
         if(norm == 0.0)
         {
            gan_err_flush_trace();
            gan_err_register ( GAN_VEC_NORM_FNAME, GAN_ERROR_DIVISION_BY_ZERO, "" );
            return GAN_FALSE;
         }

         /* scale using inverse of trace of bTSib */
         (void)GAN_SYMMAT_DIVIDE_I ( &bbT, norm );

         /* increment sum by adjusting Sp */
         (void)GAN_SYMMAT_INCREMENT ( &Sp, &bbT );
      }

      /* scale new S (Sp) by its trace to normalise it */
      norm = GAN_SYMMAT_TRACE_S(&Sp);
      if(norm == 0.0)
      {
         gan_err_flush_trace();
         gan_err_register ( GAN_VEC_NORM_FNAME, GAN_ERROR_DIVISION_BY_ZERO, "" );
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
      gan_err_register ( GAN_VEC_NORM_FNAME, GAN_ERROR_NO_CONVERGENCE, "" );
      return GAN_FALSE;
   }

   /* use Cholesky factorisation to compute L where L*L^T = S */
   if ( GAN_SYMMAT_CHOLESKY_Q ( &S, &L ) == NULL )
   {
      gan_err_register ( GAN_VEC_NORM_FNAME, GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* multiply each vector by L^-1 and scale to unit length */
   for ( i = n-1; i >= 0; i-- )
   {
      b[i] = GAN_LTMATI_MULTV_S ( &L, &b[i] );
      b[i] = GAN_VEC_DIVIDE_S ( &b[i], sqrt(GAN_VEC_SQRLEN_S(&b[i])) );
   }

   /* store L if requested */
   if ( Lp != NULL ) *Lp = L;

   /* return with success */
   return GAN_TRUE;
}
