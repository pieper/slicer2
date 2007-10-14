/*
 * File:          $RCSfile: comp_noc.c,v $
 * Module:        Numerical comparison (to be included)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:11:44 $
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

/* definitions of functions for comparing values of a specific type */

/**
 * \brief Return maximum of two values.
 */
GAN_COMP_TYPE
 GAN_MAX2 ( GAN_COMP_TYPE a, GAN_COMP_TYPE b )
{
   return gan_max2(a,b);
}

/**
 * \brief Return minimum of two values.
 */
GAN_COMP_TYPE
 GAN_MIN2 ( GAN_COMP_TYPE a, GAN_COMP_TYPE b )
{
   return gan_min2(a,b);
}

/**
 * \brief Return maximum of three values.
 */
GAN_COMP_TYPE
 GAN_MAX3 ( GAN_COMP_TYPE a, GAN_COMP_TYPE b, GAN_COMP_TYPE c )
{
   return gan_max3(a,b,c);
}

/**
 * \brief Return minimum of three values.
 */
GAN_COMP_TYPE
 GAN_MIN3 ( GAN_COMP_TYPE a, GAN_COMP_TYPE b, GAN_COMP_TYPE c )
{
   return gan_min3(a,b,c);
}

/**
 * \brief Return maximum of four values.
 */
GAN_COMP_TYPE
 GAN_MAX4 ( GAN_COMP_TYPE a, GAN_COMP_TYPE b,
            GAN_COMP_TYPE c, GAN_COMP_TYPE d )
{
   return gan_max4(a,b,c,d);
}

/**
 * \brief Return minimum of four values.
 */
GAN_COMP_TYPE
 GAN_MIN4 ( GAN_COMP_TYPE a, GAN_COMP_TYPE b,
            GAN_COMP_TYPE c, GAN_COMP_TYPE d )
{
   return gan_min4(a,b,c,d);
}

/**
 * \brief Return maximum of five values.
 */
GAN_COMP_TYPE
 GAN_MAX5 ( GAN_COMP_TYPE a, GAN_COMP_TYPE b, GAN_COMP_TYPE c,
            GAN_COMP_TYPE d, GAN_COMP_TYPE e )
{
   return gan_max5(a,b,c,d,e);
}

/**
 * \brief Return minimum of five values.
 */
GAN_COMP_TYPE
 GAN_MIN5 ( GAN_COMP_TYPE a, GAN_COMP_TYPE b, GAN_COMP_TYPE c,
            GAN_COMP_TYPE d, GAN_COMP_TYPE e )
{
   return gan_min5(a,b,c,d,e);
}

/**
 * \brief Return maximum of six values.
 */
GAN_COMP_TYPE
 GAN_MAX6 ( GAN_COMP_TYPE a, GAN_COMP_TYPE b, GAN_COMP_TYPE c,
            GAN_COMP_TYPE d, GAN_COMP_TYPE e, GAN_COMP_TYPE f )
{
   return gan_max6(a,b,c,d,e,f);
}

/**
 * \brief Return minimum of six values.
 */
GAN_COMP_TYPE
 GAN_MIN6 ( GAN_COMP_TYPE a, GAN_COMP_TYPE b, GAN_COMP_TYPE c,
            GAN_COMP_TYPE d, GAN_COMP_TYPE e, GAN_COMP_TYPE f )
{
   return gan_min6(a,b,c,d,e,f);
}

#undef GAN_COMP_TYPE
#undef GAN_MAX2
#undef GAN_MIN2
#undef GAN_MAX3
#undef GAN_MIN3
#undef GAN_MAX4
#undef GAN_MIN4
#undef GAN_MAX5
#undef GAN_MIN5
#undef GAN_MAX6
#undef GAN_MIN6
