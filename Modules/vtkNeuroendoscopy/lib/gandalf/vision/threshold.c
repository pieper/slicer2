/**
 * File:          $RCSfile: threshold.c,v $
 * Module:        threshold detection
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:20 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * History:      
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

#include "threshold.h"
#include <gandalf/image/image_defs.c>
#include <math.h>
#include <float.h>
#include "bright_feature.h"

//#include <gandalf/image/image_gl_ushort.h>


 Gan_brightFeatureMap *
  gan_find_bright ( Gan_Image *strength,unsigned short thres, Gan_Matrix23_f *A,
                    Gan_Camera *camera,int status,
                    Gan_LocalFeatureMapParams *lpms,
                    Gan_brightFeatureMap *bright_map )
{
   int  i,j;
  unsigned short temp;
   int size2 = strength->width;
   int size1 = strength->height;
   /* allocate bright map if necessary */
   if ( bright_map == NULL )
   {
      bright_map = gan_bright_feature_map_alloc(0);
      if ( bright_map == NULL )
      {
         gan_err_register ( "find_bright", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }

   /* set bright map attributes and clear bright map */
   if ( !gan_bright_feature_map_clear ( bright_map,
                                        strength->height,
                                        strength->width, A, camera,
                                        lpms ) )
   {
      gan_err_register ( "find_bright", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* return without any bright if the region is too small */
   if ( strength->width < 2 || strength->height < 2 )
   {
      gan_bright_feature_map_postprocess ( bright_map );
      return bright_map;
   }

 
   

   for ( i = 0 ; i< size1 ; i++ )//on the first size of picture
      for (j = 0; j< size2 ; j++ )//on second size
      {
         // temp = gan_image_get_pixptr_gl_f(strength,i,j);
         
         temp =gan_pixel_uc_to_us(gan_image_get_pix_gl_uc(strength , i, j ));

         if ( temp>thres)//checking if upper than threshold
         {
             /* add bright to bright map */
            if (gan_bright_feature_add ( bright_map ,
                                         i,j,(float) i,(float) j, status)
                 == NULL )


                 /* postprocess feature map, building index array into local feature map */
                  gan_bright_feature_map_postprocess ( bright_map );
            
      
         
         }
      }
   
         

   /* success */
   return bright_map;
}





   

