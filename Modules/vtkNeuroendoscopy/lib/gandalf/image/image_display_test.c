/**
 * File:          $RCSfile: image_display_test.c,v $
 * Module:        Vision package test program
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:19 $
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

#include <gandalf/TestFramework/cUnit.h>
#include <gandalf/image/image_display_test.h>

#include <gandalf/image/io/image_io.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/image/image_extract.h>
#include <gandalf/image/image_convert.h>
#include <gandalf/linalg/3x3matrix.h>

#ifdef WIN32
        #include <windows.h>
#endif

#ifdef IMAGE_DISPLAY_TEST_MAIN
#include <gandalf/image/image_display.h>
#include <gandalf/image/openGL_texture.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

static int window_id;

/* display colours */
static Gan_RGBPixel_f blue = {0.0F, 0.0F, 1.0F}, green = {0.0F, 1.0F, 0.0F},
                      yellow = {1.0F, 1.0F, 0.0F}, red = {1.0F, 0.0F, 0.0F};

#if 1
static Gan_Bool bUseTextures = GAN_TRUE;
#else
static Gan_Bool bUseTextures = GAN_FALSE;
#endif

#define SHOW_RGBA8       0
#define SHOW_GREY8       1
#define SHOW_RGBX10      2
#define SHOW_RGBAS10     3
#define SHOW_RGBA12      4
#define SHOW_GREY10_6    5
#define SHOW_GREY12_4    6
#define SHOW_YUVX444     7
#define SHOW_YUVA444     8
#define SHOW_YUV422      9
#define QUIT            99

/* defines currently displayed type of image */
static int mode = SHOW_RGBA8;
#endif /* #ifdef IMAGE_DISPLAY_TEST_MAIN */

/* grey-level and RGB versions of image */
static Gan_Image *img_rgba8=NULL, *img_grey8=NULL, *img_rgbx10=NULL, *img_rgbas10=NULL, *img_rgba12=NULL, *img_grey10_6=NULL, *img_grey12_4=NULL;
static Gan_Image *img_yuvx444=NULL, *img_yuva444=NULL, *img_yuv422=NULL;
static Gan_OpenGLTextureStruct rgba8_textures, grey8_textures, rgbx10_textures, rgbas10_textures, rgba12_textures, grey10_6_textures, grey12_4_textures;
static Gan_OpenGLTextureStruct yuvx444_textures, yuva444_textures, yuv422_textures;

#ifdef IMAGE_DISPLAY_TEST_MAIN

/* Free up resources and exit */
static void cleanup(void)
{
   glutDestroyWindow ( window_id );
   gan_image_free(img_rgba8);
   gan_free_openGL_textures_for_image(&rgba8_textures);
   if(img_grey8 != NULL)
   {
      gan_image_free(img_grey8);
      gan_free_openGL_textures_for_image(&grey8_textures);
   }
   
   if(img_rgbx10 != NULL)
   {
      gan_image_free(img_rgbx10);
      gan_free_openGL_textures_for_image(&rgbx10_textures);
   }

   if(img_rgbas10 != NULL)
   {
      gan_image_free(img_rgbas10);
      gan_free_openGL_textures_for_image(&rgbas10_textures);
   }

   if(img_rgba12 != NULL)
   {
      gan_image_free(img_rgba12);
      gan_free_openGL_textures_for_image(&rgba12_textures);
   }

   if(img_grey10_6 != NULL)
   {
      gan_image_free(img_grey10_6);
      gan_free_openGL_textures_for_image(&grey10_6_textures);
   }

   if(img_grey12_4 != NULL)
   {
      gan_image_free(img_grey12_4);
      gan_free_openGL_textures_for_image(&grey12_4_textures);
   }

   if(img_yuvx444 != NULL)
   {
      gan_image_free(img_yuvx444);
      gan_free_openGL_textures_for_image(&yuvx444_textures);
   }

   if(img_yuva444 != NULL)
   {
      gan_image_free(img_yuva444);
      gan_free_openGL_textures_for_image(&yuva444_textures);
   }

   if(img_yuv422 != NULL)
   {
      gan_image_free(img_yuv422);
      gan_free_openGL_textures_for_image(&yuv422_textures);
   }
}

/* offsets and scaling of image in window */
static Gan_Vector2 v2ImageOffset={0.0,0.0};
static double dImageScale=1.0;

/* size of display window */
static unsigned int uiWindowHeight, uiWindowWidth;

static void vReshapeMainWindow ( int iWindowWidth, int iWindowHeight )
{
   glViewport ( 0, 0, iWindowWidth, iWindowHeight );

   /* set coordinate frame for graphics in window */
   glMatrixMode ( GL_PROJECTION );
   glLoadIdentity();

   gluOrtho2D ( -v2ImageOffset.x, -v2ImageOffset.x+(double)iWindowWidth/dImageScale,
                -v2ImageOffset.y+(double)iWindowHeight/dImageScale, -v2ImageOffset.y );

   glMatrixMode ( GL_MODELVIEW );
   glLoadIdentity();

   /* update window dimensions */
   uiWindowHeight = (unsigned int)iWindowHeight;
   uiWindowWidth  = (unsigned int)iWindowWidth;
}

/* Displays the image on the screen */
static void display_image(void)
{
   /* restore window settings */
   vReshapeMainWindow ( (int)uiWindowWidth, (int)uiWindowHeight );

   if(bUseTextures)
   {
      /* clear window to red */
      glClearColor(1.0F, 0.0F, 0.0F, 1.0F);
      glClear(GL_COLOR_BUFFER_BIT);
   }

   glColor4f(1.0F, 0.0F, 0.0F, 1.0F);

   /* display image */
   glRasterPos2i ( 0, 0 );
   switch ( mode )
   {
      case SHOW_RGBA8:
        fprintf ( stderr, "displaying 8-bit RGBA image\n" );
        if(bUseTextures)
           gan_render_openGL_textures_for_image(&rgba8_textures);
        else
           gan_image_display ( img_rgba8 );

        break;
        
      case SHOW_GREY8:
        fprintf ( stderr, "displaying 8-bit grey-level image\n" );
        if(img_grey8 == NULL)
        {
           img_grey8 = gan_image_convert_s ( img_rgba8, GAN_GREY_LEVEL_IMAGE, GAN_UINT8 );
           if(img_grey8 == NULL)
           {
              fprintf ( stderr, "image conversion failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }

           if(bUseTextures && !gan_build_openGL_textures_for_image ( img_grey8, &grey8_textures ))
           {
              fprintf ( stderr, "texture generation failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
        }

        if(bUseTextures)
           gan_render_openGL_textures_for_image(&grey8_textures);
        else
           gan_image_display ( img_grey8 );

        break;
        
      case SHOW_RGBX10:
        fprintf ( stderr, "displaying 10-bit image with 2-bit padding\n" );
        if(img_rgbx10 == NULL)
        {
           img_rgbx10 = gan_image_convert_s ( img_rgba8, GAN_RGBX, GAN_UINT10 );
           if(img_rgbx10 == NULL)
           {
              fprintf ( stderr, "image conversion failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
#if 1
           /* save image */
           img_rgbx10->format = GAN_RGB_COLOUR_ALPHA_IMAGE;
           img_rgbx10->type = GAN_UINT8;
           gan_image_write("/store/tmp/rgbx10.tif", GAN_TIFF_FORMAT, img_rgbx10, NULL);
           img_rgbx10->format = GAN_RGBX;
           img_rgbx10->type = GAN_UINT10;
#endif
           if(bUseTextures && !gan_build_openGL_textures_for_image ( img_rgbx10, &rgbx10_textures ))
           {
              fprintf ( stderr, "texture generation failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
        }

        if(bUseTextures)
        {
           /* clear window and display image */
           glClear(GL_COLOR_BUFFER_BIT);
           glColor3f(0.0F,0.0F,0.0F);
           gan_render_openGL_textures_for_image(&rgbx10_textures);
        }
        else
           gan_image_display ( img_rgbx10 );

        break;
        
      case SHOW_RGBAS10:
        fprintf ( stderr, "displaying 10-bit image with 2-bit padding\n" );
        if(img_rgbas10 == NULL)
        {
           img_rgbas10 = gan_image_convert_s ( img_rgba8, GAN_RGBAS, GAN_UINT10 );
           if(img_rgbas10 == NULL)
           {
              fprintf ( stderr, "image conversion failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
#if 1
           /* save image */
           img_rgbas10->format = GAN_RGB_COLOUR_ALPHA_IMAGE;
           img_rgbas10->type = GAN_UINT8;
           gan_image_write("/store/tmp/rgbas10.tif", GAN_TIFF_FORMAT, img_rgbas10, NULL);
           img_rgbas10->format = GAN_RGBAS;
           img_rgbas10->type = GAN_UINT10;
#endif
           if(bUseTextures && !gan_build_openGL_textures_for_image ( img_rgbas10, &rgbas10_textures ))
           {
              fprintf ( stderr, "texture generation failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
        }

        if(bUseTextures)
           gan_render_openGL_textures_for_image(&rgbas10_textures);
        else
           gan_image_display ( img_rgbas10 );

        break;
        
      case SHOW_RGBA12:
        fprintf ( stderr, "displaying 12-bit RGBA image\n" );
        if(img_rgba12 == NULL)
        {
           img_rgba12 = gan_image_convert_s ( img_rgba8, GAN_RGB_COLOUR_ALPHA_IMAGE, GAN_UINT12 );
           if(img_rgba12 == NULL)
           {
              fprintf ( stderr, "image conversion failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
#if 1
           /* save image */
           img_rgba12->type = GAN_UINT16;
           gan_image_write("/store/tmp/rgba12.tif", GAN_TIFF_FORMAT, img_rgba12, NULL);
           img_rgba12->type = GAN_UINT12;
#endif
           if(bUseTextures && !gan_build_openGL_textures_for_image ( img_rgba12, &rgba12_textures ))
           {
              fprintf ( stderr, "texture generation failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
        }

        if(bUseTextures)
           gan_render_openGL_textures_for_image(&rgba12_textures);
        else
           gan_image_display ( img_rgba12 );

        break;
        
      case SHOW_GREY10_6:
        fprintf ( stderr, "displaying 10-bit grey-level image with 6-bit padding\n" );
        if(img_grey10_6 == NULL)
        {
           img_grey10_6 = gan_image_convert_s ( img_rgba8, GAN_GREY_LEVEL_IMAGE, GAN_UINT10 );
           if(img_grey10_6 == NULL)
           {
              fprintf ( stderr, "image conversion failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }

           if(bUseTextures && !gan_build_openGL_textures_for_image ( img_grey10_6, &grey10_6_textures ))
           {
              fprintf ( stderr, "texture generation failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
        }

        if(bUseTextures)
           gan_render_openGL_textures_for_image(&grey10_6_textures);
        else
           gan_image_display ( img_grey10_6 );

        break;
        
      case SHOW_GREY12_4:
        fprintf ( stderr, "displaying 12-bit grey-level image with 4-bit padding\n" );
        if(img_grey12_4 == NULL)
        {
           img_grey12_4 = gan_image_convert_s ( img_rgba8, GAN_GREY_LEVEL_IMAGE, GAN_UINT12 );
           if(img_grey12_4 == NULL)
           {
              fprintf ( stderr, "image conversion failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
        
           if(bUseTextures && !gan_build_openGL_textures_for_image ( img_grey12_4, &grey12_4_textures ))
           {
              fprintf ( stderr, "texture generation failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
        }

        if(bUseTextures)
           gan_render_openGL_textures_for_image(&grey12_4_textures);
        else
           gan_image_display ( img_grey12_4 );

        break;
        
      case SHOW_YUVX444:
        fprintf ( stderr, "displaying 8-bit padded YUV-444\n" );
        if(img_yuvx444 == NULL)
        {
           img_yuvx444 = gan_image_convert_s ( img_rgba8, GAN_YUVX444, GAN_UINT8 );
           if(img_yuvx444 == NULL)
           {
              fprintf ( stderr, "image conversion failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
           
#if 1
           /* save image */
           img_yuvx444->format = GAN_RGB_COLOUR_ALPHA_IMAGE;
           gan_image_write("/store/tmp/yuvx444.tif", GAN_TIFF_FORMAT, img_yuvx444, NULL);
           img_yuvx444->format = GAN_YUVX444;
#endif
#if 1
           {
              Gan_Image* pgiTmp;
              pgiTmp = gan_image_convert_s(img_yuvx444, GAN_RGB_COLOUR_IMAGE, GAN_UINT8);
              gan_image_write("/tmp/yuvx444.tif", GAN_TIFF_FORMAT, pgiTmp, NULL);
              gan_image_free(pgiTmp);
           }
#endif
           if(bUseTextures && !gan_build_openGL_textures_for_image ( img_yuvx444, &yuvx444_textures ))
           {
              fprintf ( stderr, "texture generation failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
        }
        
        if(bUseTextures)
           gan_render_openGL_textures_for_image(&yuvx444_textures);
        else
           gan_image_display ( img_yuvx444 );

        break;
        
      case SHOW_YUVA444:
        fprintf ( stderr, "displaying 8-bit YUVA-444\n" );
        if(img_yuva444 == NULL)
        {
           img_yuva444 = gan_image_convert_s ( img_rgba8, GAN_YUVA444, GAN_UINT8 );
           if(img_yuva444 == NULL)
           {
              fprintf ( stderr, "image conversion failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
           
#if 1
           /* save image */
           img_yuva444->format = GAN_RGB_COLOUR_ALPHA_IMAGE;
           gan_image_write("/store/tmp/yuva444.tif", GAN_TIFF_FORMAT, img_yuva444, NULL);
           img_yuva444->format = GAN_YUVA444;
#endif
        
           if(bUseTextures && !gan_build_openGL_textures_for_image ( img_yuva444, &yuva444_textures ))
           {
              fprintf ( stderr, "texture generation failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
        }
        
        if(bUseTextures)
           gan_render_openGL_textures_for_image(&yuva444_textures);
        else
           gan_image_display ( img_yuva444 );

        break;
        
      case SHOW_YUV422:
        fprintf ( stderr, "displaying 8-bit YUV-422\n" );
        if(img_yuv422 == NULL)
        {
           img_yuv422 = gan_image_convert_s ( img_rgba8, GAN_YUV422, GAN_UINT8 );
           if(img_yuv422 == NULL)
           {
              fprintf ( stderr, "image conversion failed\n" );
              cleanup();
              exit(EXIT_FAILURE);
           }
#if 1
           /* save image */
           img_yuv422->format = GAN_GREY_LEVEL_ALPHA_IMAGE;
           img_yuv422->type = GAN_UINT8;
           gan_image_write("/store/tmp/yuv422.tif", GAN_TIFF_FORMAT, img_yuv422, NULL);
           img_yuv422->format = GAN_YUV422;
           img_yuv422->type = GAN_UINT8;
#endif
#if 1
           {
              Gan_Image* pgiTmp;
              pgiTmp = gan_image_convert_s(img_yuv422, GAN_RGB_COLOUR_IMAGE, GAN_UINT8);
              gan_image_write("/tmp/yuv422.tif", GAN_TIFF_FORMAT, pgiTmp, NULL);
              gan_image_free(pgiTmp);
           }
#endif
        
           if(bUseTextures)
           {
              if(!gan_build_openGL_textures_for_image ( img_yuv422, &yuv422_textures ))
              {
                 fprintf ( stderr, "texture generation failed\n" );
                 cleanup();
                 exit(EXIT_FAILURE);
              }
           }
        }
        
        if(bUseTextures)
           gan_render_openGL_textures_for_image(&yuv422_textures);
        else
           gan_image_display ( img_yuv422 );

        break;
        
      case QUIT:
        cleanup();
        exit(EXIT_SUCCESS);
        break;
        
      default:
        fprintf ( stderr, "illegal menu entry %d\n", mode );
        cleanup();
        exit(EXIT_FAILURE);
        break;
   }

   glutSwapBuffers();
   glFlush();
}

/* Determines and processes the option the user has selected from the menu */
static void ModeMenu ( int entry )
{
   switch ( entry )
   {
      case SHOW_RGBA8:
        fprintf ( stderr, "displaying 8-bit RGBA image\n" );
        break;
        
      case SHOW_GREY8:
        fprintf ( stderr, "displaying 8-bit grey-level image\n" );
        break;
        
      case SHOW_RGBX10:
        fprintf ( stderr, "displaying 10-bit RGB image with 2-bit padding\n" );
        break;
        
      case SHOW_RGBAS10:
        fprintf ( stderr, "displaying 10-bit RGBA image with small alpha channel\n" );
        break;
        
      case SHOW_RGBA12:
        fprintf ( stderr, "displaying 12-bit RGBA image\n" );
        break;
        
      case SHOW_GREY10_6:
        fprintf ( stderr, "displaying 10-bit grey-level image with 6-bit padding\n" );
        break;
        
      case SHOW_GREY12_4:
        fprintf ( stderr, "displaying 12-bit grey-level image with 4-bit padding\n" );
        break;
        
      case SHOW_YUVX444:
        fprintf ( stderr, "displaying 8-bit padded YUV-444 image\n" );
        break;
        
      case SHOW_YUVA444:
        fprintf ( stderr, "displaying 8-bit YUVA-444 image\n" );
        break;
        
      case SHOW_YUV422:
        fprintf ( stderr, "displaying 8-bit YUV422 image\n" );
        break;
        
      case QUIT:
        cleanup();
        exit(EXIT_SUCCESS);
        break;
        
      default:
        cleanup();
        fprintf ( stderr, "illegal menu entry %d\n", entry );
        exit(EXIT_FAILURE);
        break;
   }
   
   /* store latest display mode for redisplay events */
   mode = entry;
   
   /* display image and features */
   display_image();
}

#endif /* #ifdef IMAGE_DISPLAY_TEST_MAIN */

static Gan_Bool setup_test(void)
{
   /* set default Gandalf error handler without trace handling */
   printf("\nSetup for image_display_test completed!\n\n");
   return GAN_TRUE;
}

static Gan_Bool teardown_test(void)
{
   printf("\nTeardown for image_display_test completed!\n\n");
   return GAN_TRUE;
}

/* Tests the vision functions */
static Gan_Bool run_test(void)
{
   {
      /* compute YUV inverse coefficients */
      Gan_Matrix33 M, Minv;
      Gan_Vector3 v;

      (void)gan_mat33_fill_q(&M,
                              2104.0/8192.0, 4130.0/8192.0, 802.0/8192.0,
                             -1214.0/8192.0, -2384.0/8192.0, 3598.0/8192.0,
                             3598.0/8192.0, -3013.0/8192.0, -585.0/8192.0);
      Minv = gan_mat33_invert_s(&M);
      gan_mat33_print(&Minv, "YUV --> RGB", 1, "%g");
      (void)gan_vec3_fill_q(&v,
                            (4096.0+131072.0)/8192.0,
                            (4096.0+1048576.0)/8192.0,
                            (4096.0+1048576.0)/8192.0);
      v = gan_mat33_multv3_s(&Minv, &v);
      gan_vec3_print(&v, "YUV --> RGB", 1, "%g");
   }
      
#if 1
   char *image_file = acBuildPathName(TEST_INPUT_PATH,"MonetLogo.png");
   Gan_ImageFileFormat file_format = GAN_PNG_FORMAT;
#else
   char *image_file = "/store/ndm/Backup/orl.333/examples/example_images/AlphaImages/alpha_image.0000.png";
   Gan_ImageFileFormat file_format = GAN_PNG_FORMAT;
#endif

   /* read image from file */
   img_rgba8 = gan_image_read ( image_file, file_format, NULL, NULL, NULL );
   cu_assert(img_rgba8 != NULL);

#ifdef IMAGE_DISPLAY_TEST_MAIN
   
   {
      int argc=1;
      char *argv[1] = {"\0"};
    
      glutInit(&argc,argv);
      glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
   }

   /* set black as the background colour */
   glClearColor ( 0.0F, 0.0F, 0.0F, 0.0F );

   if ( !gan_display_new_window ( img_rgba8->height, img_rgba8->width, 1.0, image_file, 0, 0, &window_id ) )
   {
      fprintf ( stderr, "cannot open create OpenGL window\n" );
      return GAN_FALSE;
   }

   /* set up display window with same dimensions as warped image */
   uiWindowHeight = img_rgba8->height;
   uiWindowWidth  = img_rgba8->width;

   if(!gan_build_openGL_textures_for_image ( img_rgba8, &rgba8_textures ))
   {
      fprintf ( stderr, "texture generation failed\n" );
      cleanup();
      exit(EXIT_FAILURE);
   }

   glutDisplayFunc ( display_image );
   glutReshapeFunc ( vReshapeMainWindow );

   /* Setup the menu, which is available by right clicking on the image */
   glutCreateMenu( ModeMenu );
   glutAddMenuEntry ( "Show 8-bit RGBA",                  SHOW_RGBA8 );
   glutAddMenuEntry ( "Show 8-bit Grey-level",            SHOW_GREY8 );
   glutAddMenuEntry ( "Show padded 10-bit RGB",           SHOW_RGBX10 );
   glutAddMenuEntry ( "Show 10-bit RGB with 2-bit alpha", SHOW_RGBAS10 );
   glutAddMenuEntry ( "Show 12-bit RGBA",                 SHOW_RGBA12 );
   glutAddMenuEntry ( "Show padded 10-bit grey-level",    SHOW_GREY10_6 );
   glutAddMenuEntry ( "Show padded 12-bit grey-level",    SHOW_GREY12_4 );
   glutAddMenuEntry ( "Show 8-bit padded YUV-444",        SHOW_YUVX444 );
   glutAddMenuEntry ( "Show 8-bit YUVA-444",              SHOW_YUVA444 );
   glutAddMenuEntry ( "Show 8-bit YUV-422",               SHOW_YUV422 );
   glutAddMenuEntry ( "Quit",                             QUIT );
   glutAttachMenu(GLUT_RIGHT_BUTTON);

   /* enter event handling loop */
   printf ( "Right-click on the image to bring up the feature display menu\n");
   
   /* transfer control of flow to OpenGL event handler */
   glutMainLoop();

   /* shouldn't get here */
   return GAN_FALSE;
#else
   /* Free up resources */
   gan_image_free(img_rgba8);

   /* success */
   return GAN_TRUE;
#endif /* #ifdef IMAGE_DISPLAY_TEST_MAIN */   
}

#ifdef IMAGE_DISPLAY_TEST_MAIN

int main ( int argc, char *argv[] )
{
   /* turn on error tracing */
   gan_err_set_trace(GAN_ERR_TRACE_ON);

   setup_test();
   if ( run_test() )
      printf ( "Tests ran successfully!\n" );
   else
   {
      printf ( "At least one test failed\n" );
      printf ( "Gandalf errors:\n" );
      gan_err_default_reporter();
   }

   teardown_test();
   gan_heap_report(NULL);
   return 0;
}

#else

/* This function registers the unit tests to a cUnit_test_suite. */
cUnit_test_suite *image_display_test_build_suite(void)
{
   cUnit_test_suite *sp;

   /* Get a new test session */
   sp = cUnit_new_test_suite("image_display_test suite");

   cUnit_add_test(sp, "image_display_test", run_test);

   /* register a setup and teardown on the test suite 'image_display_test' */
   if (cUnit_register_setup(sp, setup_test) != GAN_TRUE)
      printf("Error while setting up test suite image_display_test");

   if (cUnit_register_teardown(sp, teardown_test) != GAN_TRUE)
      printf("Error while tearing down test suite image_display_test");

   return( sp );
}

#endif /* #ifdef IMAGE_DISPLAY_TEST_MAIN */
