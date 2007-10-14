/**
 * File:          $RCSfile: convolve2D_test.c,v $
 * Module:        Vision package test program
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:17 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2004 <a href="http://frav.escet.urjc.es">FRAV (Face Recognition & Artificial Vision Group)</a>
 * Notes:         Tests the 2D convolution module.
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

#include <stdlib.h>
#include <stdio.h>

#include <gandalf/TestFramework/cUnit.h>
#include <gandalf/vision/convolve2D_test.h>
#include <gandalf/vision/mask2D.h>
#include <gandalf/vision/convolve2D.h>

static Gan_Bool setup_test(void)
{
   printf("\nSetup for convolve2D_test completed.\n\n");
   return GAN_TRUE;
}

static Gan_Bool teardown_test(void)
{
   printf("\nTeardown for convolve2D_test completed.\n\n");
   return GAN_TRUE;
}

/**
 * Auxiliar function that prints the contents of a 2D small mask.
 * @param mask The 2D mask to print.
 */
void printMask (Gan_Mask2D *mask) 
{
   int i,j;
   printf("\nConvolution mask:\n");
   for (i=0; i<mask->rows; i++) 
   {
      printf("[ ");
      for (j=0; j<mask->cols; j++) 
      {
         printf("%f \t",gan_mat_get_el(mask->data,i,j));
      }
      printf(" ]\n");
   }
}

/* Tests all the 2D convolution functions */
static Gan_Bool run_test(void)
{  
   // Generation of a 3x3 convolution mask.
   Gan_Matrix *mat = gan_mat_alloc(3,3);
   mat = gan_mat_fill_zero_q(mat,3,3);
   gan_mat_set_el(mat,0,0,1./9.);
   gan_mat_set_el(mat,1,0,1./9.);
   gan_mat_set_el(mat,2,0,1./9.);
   gan_mat_set_el(mat,0,1,1./9.);
   gan_mat_set_el(mat,1,1,1./9.);
   gan_mat_set_el(mat,2,1,1./9.);
   gan_mat_set_el(mat,0,2,1./9.);
   gan_mat_set_el(mat,1,2,1./9.);
   gan_mat_set_el(mat,2,2,1./9.);

   Gan_Mask2D *mask;
   mask = gan_mask2D_alloc_data (GAN_MASK2D_GENERIC,mat,3,3);

   /////////////////////////////////////
   // 4x4 Grey level unsigned char image
   /////////////////////////////////////
   printf("==============================================================\n");
   printf("Testing 2D convolution with an unsigned char grey level image.\n");
   printf("==============================================================\n");
   Gan_Image* img_gl_uc = gan_image_alloc_gl_uc(4,4);
   Gan_Pixel Pixel;

   Pixel.format = GAN_GREY_LEVEL_IMAGE;
   Pixel.type = GAN_UCHAR;

   // Value insertion
   int i,j;
   int v = 0;
   printf("\nImage to convolve:\n");
   for (i=0; i<img_gl_uc->height; i++) 
   {
      printf("[ ");
      for (j=0; j<img_gl_uc->width; j++) 
      {
         Pixel.data.gl.uc = v*10;
         gan_image_set_pix (img_gl_uc,i,j,&Pixel);
     printf("%d \t",Pixel.data.gl.uc);
         v++;
      }
      printf(" ]\n");
   }

   // Mask
   printMask (mask);

   // Convolution
   Gan_Image *conv_gl_uc = NULL;
   conv_gl_uc = gan_image_convolve2D_s (img_gl_uc,GAN_ALL_CHANNELS,mask);
 
   // Result is output to screen
   printf("\nResult of the convolution:\n");
   for (i=0; i<conv_gl_uc->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_gl_uc->width; j++) 
      {
         printf("%d \t",gan_image_get_pix (conv_gl_uc,i,j).data.gl.uc);
      }
      printf(" ]\n");
   } 


   /////////////////////////////////////
   // 4x4 Grey level int image
   /////////////////////////////////////
   printf("\n\n==============================================================\n");
   printf("Testing 2D convolution with an int grey level image.\n");
   printf("==============================================================\n");
 
   Gan_Image* img_gl_i = gan_image_alloc_gl_i(4,4);
   Pixel.type = GAN_INT;

   // Value insertion
   v = 0;
   printf("\nImage to convolve:\n");
   for (i=0; i<img_gl_i->height; i++) 
   {
      printf("[ ");
      for (j=0; j<img_gl_i->width; j++) 
      {
         Pixel.data.gl.i = v*10;
         gan_image_set_pix (img_gl_i,i,j,&Pixel);
     printf("%d \t",Pixel.data.gl.i);
         v++;
      }
      printf(" ]\n");
   }

   // Mask
   printMask(mask);

   // Convolution
   Gan_Image *conv_gl_i = NULL;
   conv_gl_i = gan_image_convolve2D_s (img_gl_i,GAN_ALL_CHANNELS,mask);
 
   // Result is output to screen
   printf("\nResult of the convolution:\n");
   for (i=0; i<conv_gl_i->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_gl_i->width; j++) 
      {
         printf("%d \t",gan_image_get_pix (conv_gl_i,i,j).data.gl.i);
      }
      printf("]\n");
   } 

   /////////////////////////////////////
   // 4x4 Grey level double image
   /////////////////////////////////////
   printf("\n\n==============================================================\n");
   printf("Testing 2D convolution with a double grey level image.\n");
   printf("==============================================================\n");
 
   Gan_Image* img_gl_d = gan_image_alloc_gl_d(4,4);
   Pixel.type = GAN_DOUBLE;

   // Value insertion
   v = 0;
   printf("\nImage to convolve:\n");
   for (i=0; i<img_gl_d->height; i++) 
   {
      printf("[ ");
      for (j=0; j<img_gl_d->width; j++) 
      {
         Pixel.data.gl.d = v*10.;
         gan_image_set_pix (img_gl_d,i,j,&Pixel);
     printf("%f \t",Pixel.data.gl.d);
         v++;
      }
      printf(" ]\n");
   }

   // Mask
   printMask(mask);

   // Convolution
   Gan_Image *conv_gl_d = NULL;
   conv_gl_d = gan_image_convolve2D_s (img_gl_d,GAN_ALL_CHANNELS,mask);
 
   // Result is output to screen
   printf("\nResult of the convolution:\n");
   for (i=0; i<conv_gl_d->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_gl_d->width; j++) 
      {
         printf("%f \t",gan_image_get_pix (conv_gl_d,i,j).data.gl.d);
      }
      printf(" ]\n");
   } 

   /////////////////////////////////////
   // 4x4 Grey level float image
   /////////////////////////////////////
   printf("\n\n==============================================================\n");
   printf("Testing 2D convolution with a float grey level image.\n");
   printf("==============================================================\n");
 
   Gan_Image* img_gl_f = gan_image_alloc_gl_f(4,4);
   Pixel.type = GAN_FLOAT;

   // Value insertion
   v = 0;
   printf("\nImage to convolve:\n");
   for (i=0; i<img_gl_f->height; i++) 
   {
      printf("[ ");
      for (j=0; j<img_gl_f->width; j++) 
      {
         Pixel.data.gl.f = v*10.;
         gan_image_set_pix (img_gl_f,i,j,&Pixel);
     printf("%f \t",Pixel.data.gl.f);
         v++;
      }
      printf(" ]\n");
   }

   // Mask
   printMask(mask);

   // Convolution
   Gan_Image *conv_gl_f = NULL;
   conv_gl_f = gan_image_convolve2D_s (img_gl_f,GAN_ALL_CHANNELS,mask);
 
   // Result is output to screen
   printf("\nResult of the convolution:\n");
   for (i=0; i<conv_gl_f->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_gl_f->width; j++) 
      {
         printf("%f \t",gan_image_get_pix (conv_gl_f,i,j).data.gl.f);
      }
      printf(" ]\n");
   } 

   /////////////////////////////////////
   // 4x4 Grey level unsigned int image
   /////////////////////////////////////
   printf("\n\n==============================================================\n");
   printf("Testing 2D convolution with an unsigned int grey level image.\n");
   printf("==============================================================\n");
 
   Gan_Image* img_gl_ui = gan_image_alloc_gl_ui(4,4);
   Pixel.type = GAN_UINT;

   // Value insertion
   v = 0;
   printf("\nImage to convolve:\n");
   for (i=0; i<img_gl_ui->height; i++) 
   {
      printf("[ ");
      for (j=0; j<img_gl_ui->width; j++) 
      {
         Pixel.data.gl.ui = v*10;
         gan_image_set_pix (img_gl_ui,i,j,&Pixel);
     printf("%d \t",Pixel.data.gl.ui);
         v++;
      }
      printf(" ]\n");
   }

   // Mask
   printMask(mask);

   // Convolution
   Gan_Image *conv_gl_ui = NULL;
   conv_gl_ui = gan_image_convolve2D_s (img_gl_ui,GAN_ALL_CHANNELS,mask);
 
   // Result is output to screen
   printf("\nResult of the convolution:\n");
   for (i=0; i<conv_gl_ui->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_gl_ui->width; j++) 
      {
         printf("%d \t",gan_image_get_pix (conv_gl_ui,i,j).data.gl.ui);
      }
      printf(" ]\n");
   } 

   /////////////////////////////////////
   // 4x4 RGB level unsigned char image
   /////////////////////////////////////
   printf("\n\n==============================================================\n");
   printf("Testing 2D convolution with an unsigned char RGB level image.\n");
   printf("==============================================================\n");
   Gan_Image* img_rgb_uc = gan_image_alloc_rgb_uc(4,4);

   Pixel.format = GAN_RGB_COLOUR_IMAGE;
   Pixel.type = GAN_UCHAR;

   // Value insertion
   v = 0;
   printf("\nImage to convolve:\n");
   for (i=0; i<img_rgb_uc->height; i++) 
   {
      printf("[ ");
      for (j=0; j<img_rgb_uc->width; j++) 
      {
         Pixel.data.rgb.uc.R = v*10;
         Pixel.data.rgb.uc.G = v*10+20;
         Pixel.data.rgb.uc.B = v*10+40;
         gan_image_set_pix (img_rgb_uc,i,j,&Pixel);
     printf("(%d,%d,%d) \t",Pixel.data.rgb.uc.R,
        Pixel.data.rgb.uc.G,Pixel.data.rgb.uc.B);
         v++;
      }
      printf(" ]\n");
   }

   // Mask
   printMask(mask);

   // RGB Convolution
   Gan_Image *conv_rgb_uc = NULL;
   conv_rgb_uc = gan_image_convolve2D_s (img_rgb_uc,GAN_ALL_CHANNELS,mask);
 
   // Result is output to screen
   printf("\nResult of the RGB convolution:\n");
   for (i=0; i<conv_rgb_uc->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_rgb_uc->width; j++) 
      {
         printf("(%d,%d,%d) \t",
        gan_image_get_pix (conv_rgb_uc,i,j).data.rgb.uc.R,
        gan_image_get_pix (conv_rgb_uc,i,j).data.rgb.uc.G,
        gan_image_get_pix (conv_rgb_uc,i,j).data.rgb.uc.B);
      }
      printf(" ]\n");
   } 

   // Grey level Convolution: only RED
   conv_gl_uc = gan_image_convolve2D_s (img_rgb_uc,GAN_RED_CHANNEL,mask);
 
   // Result is output to screen
   printf("\nResult of the only-red convolution:\n");
   for (i=0; i<conv_gl_uc->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_gl_uc->width; j++) 
      {
         printf("%d \t", gan_image_get_pix (conv_gl_uc,i,j).data.gl.uc);
      }
      printf(" ]\n");
   }

   // Grey level Convolution: only GREEN
   conv_gl_uc = gan_image_convolve2D_s (img_rgb_uc,GAN_GREEN_CHANNEL,mask);
 
   // Result is output to screen
   printf("\nResult of the only-green convolution:\n");
   for (i=0; i<conv_gl_uc->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_gl_uc->width; j++) 
      {
         printf("%d \t", gan_image_get_pix (conv_gl_uc,i,j).data.gl.uc);
      }
      printf(" ]\n");
   }

   // Grey level Convolution: only BLUE
   conv_gl_uc = gan_image_convolve2D_s (img_rgb_uc,GAN_BLUE_CHANNEL,mask);
 
   // Result is output to screen
   printf("\nResult of the only-blue convolution:\n");
   for (i=0; i<conv_gl_uc->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_gl_uc->width; j++) 
      {
         printf("%d \t", gan_image_get_pix (conv_gl_uc,i,j).data.gl.uc);
      }
      printf(" ]\n");
   }


   /////////////////////////////////////
   // 4x4 RGB level double image
   /////////////////////////////////////
   printf("\n\n==============================================================\n");
   printf("Testing 2D convolution with a double RGB level image.\n");
   printf("==============================================================\n");
   Gan_Image* img_rgb_d = gan_image_alloc_rgb_d(4,4);

   Pixel.format = GAN_RGB_COLOUR_IMAGE;
   Pixel.type = GAN_DOUBLE;

   // Value insertion
   v = 0;
   printf("\nImage to convolve:\n");
   for (i=0; i<img_rgb_d->height; i++) 
   {
      printf("[");
      for (j=0; j<img_rgb_d->width; j++) 
      {
         Pixel.data.rgb.d.R = v*10.;
         Pixel.data.rgb.d.G = v*10.+20.;
         Pixel.data.rgb.d.B = v*10.+40.;
         gan_image_set_pix (img_rgb_d,i,j,&Pixel);
     printf("(%3.1f,%3.1f,%3.1f) ",Pixel.data.rgb.d.R,
        Pixel.data.rgb.d.G,Pixel.data.rgb.d.B);
         v++;
      }
      printf("]\n");
   }

   // Mask
   printMask(mask);

   // RGB Convolution
   Gan_Image *conv_rgb_d = NULL;
   conv_rgb_d = gan_image_convolve2D_s (img_rgb_d,GAN_ALL_CHANNELS,mask);
 
   // Result is output to screen
   printf("\nResult of the RGB convolution:\n");
   for (i=0; i<conv_rgb_d->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_rgb_d->width; j++) 
      {
         printf("(%f,%f,%f) ",
        gan_image_get_pix (conv_rgb_d,i,j).data.rgb.d.R,
        gan_image_get_pix (conv_rgb_d,i,j).data.rgb.d.G,
        gan_image_get_pix (conv_rgb_d,i,j).data.rgb.d.B);
      }
      printf(" ]\n");
   } 

   // Grey level Convolution: only RED
   conv_gl_d = gan_image_convolve2D_s (img_rgb_d,GAN_RED_CHANNEL,mask);
 
   // Result is output to screen
   printf("\nResult of the only-red convolution:\n");
   for (i=0; i<conv_gl_d->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_gl_d->width; j++) 
      {
         printf("%f ", gan_image_get_pix (conv_gl_d,i,j).data.gl.d);
      }
      printf(" ]\n");
   }

   // Grey level Convolution: only GREEN
   conv_gl_d = gan_image_convolve2D_s (img_rgb_d,GAN_GREEN_CHANNEL,mask);
 
   // Result is output to screen
   printf("\nResult of the only-green convolution:\n");
   for (i=0; i<conv_gl_d->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_gl_d->width; j++) 
      {
         printf("%f ", gan_image_get_pix (conv_gl_d,i,j).data.gl.d);
      }
      printf(" ]\n");
   }

   // Grey level Convolution: only BLUE
   conv_gl_d = gan_image_convolve2D_s (img_rgb_d,GAN_BLUE_CHANNEL,mask);
 
   // Result is output to screen
   printf("\nResult of the only-blue convolution:\n");
   for (i=0; i<conv_gl_d->height; i++) 
   {
      printf("[ ");
      for (j=0; j<conv_gl_d->width; j++) 
      {
         printf("%f ", gan_image_get_pix (conv_gl_d,i,j).data.gl.d);
      }
      printf(" ]\n");
   }

   // Freeing memory
   gan_image_free(conv_gl_uc);   
   gan_image_free(conv_gl_i);   
   gan_image_free(conv_gl_d);   
   gan_image_free(conv_gl_f);   
   gan_image_free(conv_gl_ui);   
   gan_image_free(conv_rgb_uc);   
   gan_image_free(conv_rgb_d);   
   gan_mask2D_free(mask);     
   gan_image_free(img_gl_uc);
   gan_image_free(img_gl_i);
   gan_image_free(img_gl_d);
   gan_image_free(img_gl_f);
   gan_image_free(img_gl_ui);
   gan_image_free(img_rgb_uc);
   gan_image_free(img_rgb_d);
   return GAN_TRUE;
}

#ifdef CONVOLVE2D_TEST_MAIN

int main ( int argc, char *argv[] )
{
   /* set default Gandalf error handler without trace handling */ 
   gan_err_set_reporter ( gan_err_default_reporter );
   gan_err_set_trace ( GAN_ERR_TRACE_OFF );

   setup_test();
   if ( run_test() )
      printf ( "Tests ran successfully!\n" );
   else
      printf ( "At least one test failed\n" );

      teardown_test();
      gan_heap_report(NULL);
      return 0;
   }

#else

   /* This function registers the unit tests to a cUnit_test_suite. */
   cUnit_test_suite *convolve2D_test_build_suite(void)
   {
      cUnit_test_suite *sp;

      /* Get a new test session */
      sp = cUnit_new_test_suite("convolve2D_test suite");
      
      cUnit_add_test(sp, "convolve2D_test", run_test);

      /* register a setup and teardown on the test suite 
     'convolve2D_test' */
      if (cUnit_register_setup(sp, setup_test) != GAN_TRUE)
         printf("Error while setting up test suite convolve2D_test");

      if (cUnit_register_teardown(sp, teardown_test) != GAN_TRUE)
         printf("Error while tearing down test suite convolve2D_test");

      return( sp );
   }

#endif /* #ifdef CONVOLVE2D_TEST_MAIN */


