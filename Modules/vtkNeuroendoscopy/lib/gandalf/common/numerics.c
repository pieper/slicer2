/**
 * File:          $RCSfile: numerics.c,v $
 * Module:        Numerical functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:11:45 $
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

#include <gandalf/common/numerics.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/misc_error.h>
#include <math.h>
#include <string.h>

/**
 * \addtogroup Common
 * \{
 */

/**
 * \defgroup CommonNumerics Numerical Routines
 * \{
 */

/**
 * \brief Real square function.
 * \param x Input real value
 * \return The square of \a x
 *
 * Real square function.
 *
 * \sa gan_sqr_i(), gan_cbrt().
 */
double
 gan_sqr_d ( double x )
{
   return gan_sqr(x);
}

/**
 * \brief Real square function (single precision).
 * \param x Input real value
 * \return The square of \a x
 *
 * Real square function
 *
 * \sa gan_sqr_d(), gan_cbrt().
 */
float
 gan_sqr_f ( float x )
{
   return gan_sqr(x);
}

/**
 * \brief Integer square function.
 * \param x Input integer value
 * \return The square of \a x
 *
 * Integer square function
 *
 * \sa gan_sqr_d(), gan_cbrt().
 */
int
 gan_sqr_i ( int x )
{
   return gan_sqr(x);
}

/**
 * \brief Unsigned integer square function.
 * \param x Input unsigned integer value
 * \return The square of \a x
 *
 * Unsigned integer square function.
 *
 * \sa gan_sqr_d(), gan_cbrt().
 */
unsigned
 gan_sqr_ui ( unsigned x )
{
   return gan_sqr(x);
}

/**
 * \brief Long integer square function.
 * \param x Input long integer value
 * \return The square of \a x
 *
 * Long integer square function
 *
 * \sa gan_sqr_i(), gan_cbrt().
 */
long
 gan_sqr_l ( long x )
{
   return gan_sqr(x);
}

/**
 * \brief Real cube root function.
 * \param x Input real value
 * \return The cube root of \a x.
 *
 * Real cube root function.
 */
double
 gan_cbrt ( double x )
{
   return ( (x>0.0) ? exp(log(x)/3.0) : ((x<0.0) ? -exp(log(-x)/3.0) : 0.0) );
}

/**
 * \brief Returns a sample of a normally distributed random variable.
 * \param mu The mean of the normal distribution
 * \param sigma The standard deviation of the Gaussian distribution
 * \return The sample taken from the normal distribution.
 *
 * Returns a sample from a normal distribution. Two values are computed,
 * but only one is returned.
 */
double
 gan_normal_sample ( double mu, double sigma )
{
   double r, x, y;

   do
   {
      /* get a random point inside the unit circle */
      x = 2.0*gan_random_01() - 1.0;
      y = 2.0*gan_random_01() - 1.0;
      r = x*x + y*y;
   }
   while ( r > 1.0 );

   /* discarded sample value is mu + y*sqrt(-2.0*log(r)/r)*sigma */
   return ( mu + x*sqrt(-2.0*log(r)/r)*sigma );
}

/**
 * \brief Find roots of a quadratic equation with real coefficients.
 * \param a The coeffient in \f$ x^2 \f$
 * \param b The coeffient in \f$ x \f$
 * \param c The scalar coeffient
 * \param x The roots of the equation
 * \return The number of roots, or -1 on error.
 *
 * Solve quadratic equation
 * \f[
 *   a\:x^2 + b\:x + c = 0
 * \f]
 * using method given in Numerical Recipes.
 */
int
 gan_solve_quadratic ( double a, double b, double c, Gan_Complex x[2] )
{
   double d;

   if ( a == 0.0 )
   {
      /* linear equation */
      if ( b == 0.0 )
      {
         if ( c == 0.0 )
         {
            /* equation is 0 == 0 */
            gan_err_flush_trace();
            gan_err_register ( "gan_solve_quadratic", GAN_ERROR_FAILURE, "" );
            return -1;
         }

         /* equation is c == 0 for non-zero c */
         return 0;
      }

      x[0].r = -c/b;
      x[0].i = 0.0;
      return 1;
   }

   if ( b == 0.0 )
   {
      /* simple case */
      if ( c <= 0.0 )
      {
         x[0].r = sqrt(-c/a);
         x[1].r = -x[0].r;
         x[0].i = x[1].i = 0.0;
      }
      else
      {
         x[0].r = x[1].r = 0.0;
         x[0].i = sqrt(c/a);
         x[1].i = -x[0].i;
      }

      return 2;
   }
         
   /* compute discriminant */
   d = b*b - 4.0*a*c;

   if ( d > 0.0 )
   {
      double q;

      /* positive discriminant: two real roots */
      q = (b > 0.0) ? -0.5*(b + sqrt(d)) : -0.5*(b - sqrt(d));
      x[0].r = q/a;
      x[1].r = c/q;
      x[0].i = x[1].i = 0.0;
   }
   else
   {
      double qr, qi, q2;

      /* negative discriminant: two complex roots */
      qr = -0.5*b;
      if ( b > 0.0 )
         qi = -0.5*sqrt(-d);
      else
         qi =  0.5*sqrt(-d);
         
      x[0].r = qr/a;
      x[0].i = qi/a;
      q2 = qr*qr + qi*qi;
      x[1].r = c*qr/q2;
      x[1].i = -c*qi/q2;
   }
   
   return 2;
}
   
/**
 * \brief Find roots of a cubic equation with real coefficients.
 * \param a The coeffient in \f$ x^3 \f$
 * \param b The coeffient in \f$ x^2 \f$
 * \param c The coeffient in \f$ x \f$
 * \param d The scalar coeffient
 * \param x The roots of the equation
 * \return The number of roots, or -1 on error.
 *
 * Solve cubic equation
 * \f[
 *    a\:x^3 + b\:x^2 + c\:x + d = 0
 * \f]
 * using method given in Numerical Recipes.
 */
int
 gan_solve_cubic ( double a, double b, double c, double d, Gan_Complex x[3] )
{
   double Q, Rp, Q3;

   if ( a == 0.0 )
   {
      Gan_Complex xq[2];
      int qn, i;

      /* solve quadratic */
      qn = gan_solve_quadratic ( b, c, d, xq );
      for ( i = qn-1; i >= 0; i-- )
      {
         x[i].r = xq[i].r;
         x[i].i = xq[i].i;
      }
      
      return qn;
   }

   if ( a != 1.0 )
   {
      b /= a;
      c /= a;
      d /= a;
   }

   Q = (b*b - 3.0*c)/9.0;
   Rp = (2.0*b*b*b - 9.0*b*c + 27.0*d)/54.0;
   Q3 = Q*Q*Q;
   if ( Rp*Rp < Q3 )
   {
      double t, Qr2m, b3 = b/3.0;

      /* real roots */
      t = acos(Rp/sqrt(Q3));
      Qr2m = -2.0*sqrt(Q);
      x[0].r = Qr2m*cos(t/3.0) - b3;
      x[1].r = Qr2m*cos((t + 2.0*M_PI)/3.0) - b3;
      x[2].r = Qr2m*cos((t - 2.0*M_PI)/3.0) - b3;

      /* set imaginary components to zero */
      x[0].i = x[1].i = x[2].i = 0.0;
   }
   else
   {
      /* one real and two complex roots */
      double A, B;

      A = (Rp > 0) ? -gan_cbrt(fabs(Rp) + sqrt(Rp*Rp - Q3))
                   :  gan_cbrt(fabs(Rp) + sqrt(Rp*Rp - Q3));
      B = (A == 0.0) ? 0.0 : Q/A;
      x[0].r = (A + B) - b/3.0;
      x[0].i = 0.0;
      x[1].r = -0.5*(A+B) - b/3.0;
      x[1].i = 0.5*M_SQRT3*(A - B);
      x[2].r = x[1].r;
      x[2].i = -x[1].i;
   }

   return 3;
}

#define ADDRESS(i) ((void *)CHADDR(i))
#define CHADDR(i) ((void *)(&(((char *)base)[(i)*size])))
#define SWAP(i,j) {memcpy(tmpbuf,CHADDR(i),size); memcpy(CHADDR(i),CHADDR(j),size); memcpy(CHADDR(j),tmpbuf,size);}

/**
 * \brief Return k'th highest element of an array
 */
void *
 gan_kth_highest ( void *base, size_t nmemb, size_t size,
                   unsigned int k,
                   int (*compar)(const void *, const void *) )
{
   unsigned low = 0, high = nmemb-1, middle, x, y;
   void *tmpbuf;

   if ( k >= nmemb || nmemb == 0 || size == 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_kth_highest", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      return NULL;
   }

   /* allocate buffer for swapping */
   tmpbuf = (void *)gan_malloc_array(char,size);
   if ( tmpbuf == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_kth_highest", GAN_ERROR_MALLOC_FAILED, "", size*sizeof(char) );
      return NULL;
   }

   for(;;)
   {
      /* if low and high element are the same, we're done */
      if ( low == high )
      {
         assert(low == k);
         free(tmpbuf);
         return ADDRESS(low);
      }

      /* if low and high are consecutive, choose one of them */
      if ( low == high-1 )
      {
         if ( compar(ADDRESS(low), ADDRESS(high)) > 0 )
            SWAP(low,high)

         assert(low == k || high == k);
         free(tmpbuf);
         return ADDRESS(k);
      }

      /* choose a comparison element as median of low, high and middle
         element of current sub-array */
      middle = low + ((high - low)/2);

      /* move median element to position low+1, and swap around low and high
         elements if necessary to maintain their order */
      if ( compar(ADDRESS(low), ADDRESS(high)) < 0 )
      {
         if ( compar(ADDRESS(high), ADDRESS(middle)) < 0 )
            /* high is the median of the three; swap with middle element */
            SWAP(high,middle)
         else if ( compar(ADDRESS(low), ADDRESS(middle)) > 0 )
            /* low is the median of the three */
            SWAP(low,middle)
      }
      else
      {
         /* first swap low and high to put them in order */
         SWAP(low,high)
         if ( compar(ADDRESS(high), ADDRESS(middle)) < 0 )
            /* high is the median of the three; swap with middle element */
            SWAP(high,middle)
         else if ( compar(ADDRESS(low), ADDRESS(middle)) > 0 )
            /* low is the median of the three */
            SWAP(low,middle)
      }

      /* move median element into position low+1 */
      if ( middle != low+1 )
      {
         SWAP(low+1,middle)
         middle = low+1;
      }

      /* initialise indices of lower and upper elements */
      x = middle;
      y = high;
      for(;;)
      {
         do x++; while ( compar(ADDRESS(x), ADDRESS(middle)) < 0 );
         do y--; while ( compar(ADDRESS(y), ADDRESS(middle)) > 0 );
         if (x>y) break; /* terminate when indices cross */
         SWAP(x,y)
      }

      /* place partitioning element into position */
      if ( y != middle )
         SWAP(middle, y)

      if (y == k)
      {
         free(tmpbuf);
         return ADDRESS(k);
      }

      if (y > k) high=y-1;
      if (y < k) low = (x==y+2) ? x-1 : x;
      assert(low <= k && high >= k);
   }

   /* shouldn't get here */
   free(tmpbuf);
   gan_err_flush_trace();
   gan_err_register ( "gan_kth_highest", GAN_ERROR_FAILURE, "" );
   return NULL;
}

#undef CHADDR
#undef ADDRESS
#undef SWAP

/**
 * \}
 */

/**
 * \}
 */
