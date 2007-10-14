/**
 * File:          $RCSfile: mask2D.c,v $
 * Module:        Constructing 2D Gaussian convolution masks
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:20 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2004 <a href="http://frav.escet.urjc.es">FRAV (Face Recognition & Artificial Vision Group)</a>
 */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GN
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include <math.h>
#include <gandalf/vision/mask2D.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/array.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup Convolution
 * \{
 */

Gan_Mask2D *
 gan_mask2D_form_gen ( Gan_Mask2D *mask,
                       Gan_Mask2DFormat format, 
                       Gan_Matrix *data, 
               unsigned int rows,
               unsigned int cols)
{
   gan_heap_push();

   /* make sure mask size is odd */
   gan_err_test_ptr ( (rows % 2) == 1, "gan_mask2D_form_gen",
                      GAN_ERROR_INCOMPATIBLE, "mask rows size must be odd" );

   gan_err_test_ptr ( (cols % 2) == 1, "gan_mask2D_form_gen",
                      GAN_ERROR_INCOMPATIBLE, "mask cols size must be odd" );


   /* create mask structure if necessary */
   if ( mask == NULL )
   {
      mask = gan_malloc_object(Gan_Mask2D);
      mask->alloc = GAN_TRUE;
   }
   else
      mask->alloc = GAN_FALSE;

   /* allocate mask data if necessary */
   if ( data == NULL )
   {
      switch ( format )
      {
         case GAN_MASK2D_SYMMETRIC:
            mask->size_alloc = (1+rows/2)*(1+cols/2);
        mask->data = gan_mat_alloc (1+rows/2,1+cols/2);
            break;

         case GAN_MASK2D_ANTISYMMETRIC:
            mask->size_alloc = (rows/2)*(cols/2);
        mask->data = gan_mat_alloc (rows/2,cols/2);
            break;
           
         case GAN_MASK2D_GENERIC:
            mask->size_alloc = rows*cols;
        mask->data = gan_mat_alloc (rows,cols);
            break;

         default:
            gan_err_flush_trace();
            gan_err_register ( "gan_mask2D_form_gen",
                               GAN_ERROR_ILLEGAL_TYPE, "" );
            return NULL;
      }

      mask->data_alloc = GAN_TRUE;
   }
   else
   {
      switch ( format )
      {
         case GAN_MASK2D_SYMMETRIC:
        gan_err_test_ptr ( cols==data->cols*2-1, 
                   "gan_mask2D_form_gen",
                   GAN_ERROR_INCOMPATIBLE, 
         "2D symmetric mask width incompatible with data array width" );
        gan_err_test_ptr ( rows==data->rows*2-1, 
                   "gan_mask2D_form_gen",
                   GAN_ERROR_INCOMPATIBLE, 
         "2D symmetric mask height incompatible with data array height" );
            break;

         case GAN_MASK2D_ANTISYMMETRIC:
        gan_err_test_ptr ( cols==data->cols*2+1, 
                   "gan_mask2D_form_gen",
                GAN_ERROR_INCOMPATIBLE, 
      "2D antisymmetric mask width incompatible with data array width" );
        gan_err_test_ptr ( rows==data->rows*2+1, 
                   "gan_mask2D_form_gen",
                   GAN_ERROR_INCOMPATIBLE, 
      "2D antisymmetric mask height incompatible with data array height" );
            break;
           
         case GAN_MASK2D_GENERIC:
        gan_err_test_ptr ( cols==data->cols, 
                   "gan_mask2D_form_gen",
                   GAN_ERROR_INCOMPATIBLE, 
         "2D generic mask width incompatible with data array width" );
        gan_err_test_ptr ( rows==data->rows, 
                   "gan_mask2D_form_gen",
                   GAN_ERROR_INCOMPATIBLE, 
         "2D generic mask height incompatible with data array height" );
        break;
         default:
            gan_err_flush_trace();
            gan_err_register ( "gan_mask2D_form_gen",
                               GAN_ERROR_ILLEGAL_TYPE, "" );
            return NULL;
      }
      mask->data=gan_mat_copy_s( data); 
      mask->data_alloc = GAN_FALSE;
   }

   /* set mask format and size */
   mask->format = format;
   mask->rows   = rows;
   mask->cols   = cols;

   gan_heap_pop();
   return mask;
}

Gan_Bool 
  gan_mask2D_copy_q ( Gan_Mask2D *source, Gan_Mask2D *dest ) 
{
   switch ( source->format )
   {
      case GAN_MASK2D_SYMMETRIC:
      
         gan_err_test_bool ( dest->size_alloc >= source->rows*source->cols/4+
                 source->rows/2+source->cols/2+1,
                 "gan_mask2D_copy_q", GAN_ERROR_INCOMPATIBLE,
                 "convolution masks" );
     dest->data = gan_mat_copy_q(source->data,dest->data);
     break;

      case GAN_MASK2D_ANTISYMMETRIC:
     gan_err_test_bool ( dest->size_alloc >= source->rows*source->cols/4,
                 "gan_mask2D_copy_q", GAN_ERROR_INCOMPATIBLE,
                 "convolution masks" );
     dest->data = gan_mat_copy_q(source->data,dest->data);
     break;

      case GAN_MASK2D_GENERIC:
    gan_err_test_bool ( dest->size_alloc >= source->rows*source->cols,
                "gan_mask2D_copy_q", GAN_ERROR_INCOMPATIBLE,
                "convolution masks" );
    dest->data = gan_mat_copy_q(source->data,dest->data);

   }

   dest->format = source->format;
   dest->rows   = source->rows;
   dest->cols   = source->cols;
   return GAN_TRUE;
}

Gan_Bool
 gan_mask2D_free ( Gan_Mask2D *mask )
{
   if ( mask->data_alloc )
      free ( mask->data );

   if ( mask->alloc ) free ( mask );
   return GAN_TRUE;
}


Gan_Mask2D* gan_mask2D_gen_from_Gan_Image (Gan_Image *img) 
{
   Gan_Mask2D *mask2D;
   Gan_Matrix *mat;
   int i,j;
   Gan_Pixel pixel;

   gan_err_test_bool ( img!=NULL && img->format==GAN_GREY_LEVEL_IMAGE,
                       "gan_mask2D_gen_from_Gan_Image", 
               GAN_ERROR_INCOMPATIBLE,
                       "convolution image must be grey-level" );

   gan_err_test_bool (img->height % 2 == 1,
              "gan_mask2D_gen_from_Gan_Image",
              GAN_ERROR_FAILURE,
              "convolution image must have odd height");
    
   gan_err_test_bool (img->width % 2 == 1,
              "gan_mask2D_gen_from_Gan_Image",
              GAN_ERROR_FAILURE,
              "convolution image must have odd width");

   mat = gan_mat_alloc(img->height,img->width);
   if(mat == NULL)
   {
      gan_err_register("gan_mask2D_gen_from_Gan_Image",
                       GAN_ERROR_FAILURE, "");
      return NULL;
   }

   for (i=0; i<img->height; i++) 
   {
      for (j=0; j<img->width; j++) 
      {
         pixel = gan_image_get_pix(img,i,j);
         gan_mat_set_el(mat,i,j,((double)pixel.data.gl.uc) /
            (img->height*img->width*255));
      }
   }    
   
   mask2D = gan_mask2D_form_gen ( NULL,
                                  GAN_MASK2D_GENERIC,
                                  mat,
                                  img->height,
                                  img->width);

   gan_mat_free(mat);
   return mask2D;
}

/**
 * \}
 */

/**
 * \}
 */
