/**
 * File:          $RCSfile: image_display.c,v $
 * Module:        Display images using Open GL
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:18 $
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

#include <math.h>
#include <gandalf/image/image_display.h>
#include <gandalf/image/image_convert.h>
#include <gandalf/image/image_gl_uchar.h>
#include <gandalf/image/image_gl_ushort.h>
#include <gandalf/image/image_gl_uint.h>
#include <gandalf/image/image_gl_float.h>
#include <gandalf/image/image_gl_uint10.h>
#include <gandalf/image/image_gl_uint12.h>
#include <gandalf/image/image_rgb_uchar.h>
#include <gandalf/image/image_rgb_ushort.h>
#include <gandalf/image/image_rgb_uint.h>
#include <gandalf/image/image_rgb_float.h>
#include <gandalf/image/image_rgba_uchar.h>
#include <gandalf/image/image_rgba_ushort.h>
#include <gandalf/image/image_rgba_uint.h>
#include <gandalf/image/image_rgba_float.h>
#include <gandalf/image/image_rgbx_uint10.h>
#include <gandalf/common/linked_list.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/allocate.h>

#ifdef HAVE_GLUT

#ifdef WIN32
        #include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#ifdef HAVE_OPENGL_EXT
#include <GL/glext.h>
#endif
#endif



/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \defgroup ImageDisplay Display Images using OpenGL
 * \{
 */

/* display binary images */
static Gan_Bool
 gan_image_display_b ( const Gan_Image *img )
{
   switch ( img->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
      {
         Gan_Image *icopy;

         icopy = gan_image_convert_s ( img, GAN_GREY_LEVEL_IMAGE, GAN_UCHAR );
         glDrawPixels ( icopy->width, icopy->height,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE,
                        gan_image_get_pixptr_gl_uc ( icopy, 0, 0 ) );
         gan_image_free ( icopy );
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "gan_image_display_b", GAN_ERROR_NOT_IMPLEMENTED,
                            "" );
        return GAN_FALSE;
        break;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Displays an image using OpenGL.
 * \param img The image to display
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Displays an image on the screen using the OpenGL function \c glDrawPixels().
 * Assumes that the graphics window has already been set up in a way similar to
 * the procedure in gan_image_display_new_window().
 *
 * \sa gan_image_display_new_window().
 */
Gan_Bool
 gan_image_display ( const Gan_Image *img )
{
   Gan_Bool copied = GAN_FALSE;

   /* handle Boolean and float images separately */
   if ( img->type == GAN_BOOL )
      return gan_image_display_b ( img );

   /* copy the image data if the stride is not equal to the width, because
      OpenGL cannot handle this case */
   if ( img->stride != gan_image_min_stride ( img->format, img->type,
                                              img->width, 0 ) )
   {
      img = gan_image_copy_s ( img );
      copied = GAN_TRUE;
   }

   switch ( img->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             glDrawPixels ( img->width, img->height, GL_LUMINANCE, GL_UNSIGNED_BYTE, gan_image_get_pixptr_gl_uc ( img, 0, 0 ) );
             break;

           case GAN_USHORT:
             glDrawPixels ( img->width, img->height, GL_LUMINANCE, GL_UNSIGNED_SHORT, gan_image_get_pixptr_gl_us ( img, 0, 0 ) );
             break;

           case GAN_UINT:
             glDrawPixels ( img->width, img->height, GL_LUMINANCE, GL_UNSIGNED_INT, gan_image_get_pixptr_gl_ui ( img, 0, 0 ) );
             break;

           case GAN_FLOAT:
             glDrawPixels ( img->width, img->height, GL_LUMINANCE, GL_FLOAT, gan_image_get_pixptr_gl_f ( img, 0, 0 ) );
             break;

           case GAN_UINT10:
             glDrawPixels ( img->width, img->height, GL_LUMINANCE, GL_UNSIGNED_SHORT, (unsigned short *)gan_image_get_pixptr_gl_ui10 ( img, 0, 0 ) );
             break;

           case GAN_UINT12:
             glDrawPixels ( img->width, img->height, GL_LUMINANCE, GL_UNSIGNED_SHORT, (unsigned short *)gan_image_get_pixptr_gl_ui12 ( img, 0, 0 ) );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_display", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return GAN_FALSE;
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             glDrawPixels ( img->width, img->height, GL_RGB, GL_UNSIGNED_BYTE, gan_image_get_pixptr_rgb_uc ( img, 0, 0 ) );
             break;

           case GAN_USHORT:
             glDrawPixels ( img->width, img->height, GL_RGB, GL_UNSIGNED_SHORT, gan_image_get_pixptr_rgb_us ( img, 0, 0 ) );
             break;

           case GAN_UINT:
             glDrawPixels ( img->width, img->height, GL_RGB, GL_UNSIGNED_INT, gan_image_get_pixptr_rgb_ui ( img, 0, 0 ) );
             break;

           case GAN_FLOAT:
             glDrawPixels ( img->width, img->height, GL_RGB, GL_FLOAT, gan_image_get_pixptr_rgb_f ( img, 0, 0 ) );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register (  "gan_image_display", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return GAN_FALSE;
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             glDrawPixels ( img->width, img->height, GL_RGBA, GL_UNSIGNED_BYTE, gan_image_get_pixptr_rgba_uc ( img, 0, 0 ) );
             break;

           case GAN_USHORT:
             glDrawPixels ( img->width, img->height, GL_RGBA, GL_UNSIGNED_SHORT, gan_image_get_pixptr_rgba_us ( img, 0, 0 ) );
             break;

           case GAN_UINT:
             glDrawPixels ( img->width, img->height, GL_RGBA, GL_UNSIGNED_INT, gan_image_get_pixptr_rgba_ui ( img, 0, 0 ) );
             break;

           case GAN_FLOAT:
             glDrawPixels ( img->width, img->height, GL_RGBA, GL_FLOAT, gan_image_get_pixptr_rgba_f ( img, 0, 0 ) );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register (  "gan_image_display", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return GAN_FALSE;
             break;
        }
        break;

      case GAN_RGBX:
        switch ( img->type )
        {
#ifdef GL_UNSIGNED_INT_10_10_10_2
           case GAN_UINT10:
             glDrawPixels ( img->width, img->height, GL_RGB10, GL_UNSIGNED_INT_10_10_10_2, gan_image_get_pixptr_rgbx_ui10 ( img, 0, 0 ) );
             break;
#endif

           default:
             gan_err_flush_trace();
             gan_err_register (  "gan_image_display", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return GAN_FALSE;
             break;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "gan_image_display", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return GAN_FALSE;
        break;
   }

   if ( copied ) gan_image_free((Gan_Image*)img);

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Creates an OpenGL display window.
 * \param height The height images/graphics to be shown in the window
 * \param width The width of images/graphics to be shown in the window
 * \param zoom Zoom factor for image/graphics in the window
 * \param name A name for the window
 * \param offset_r Vertical offset of window from corner of screen
 * \param offset_c Horizontal offset of window from corner of screen
 * \param window_id Pointer to the OpenGL display window ID
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Creates a new OpenGL window, with the given dimensions, \a zoom factor,
 * \a name and screen offsets. Upon success, the ID of the new OpenGL window
 * is passed back in the \a window_id pointer.
 *
 * \sa gan_display_new_window_array().
 */
Gan_Bool
 gan_display_new_window ( int height, int width, double zoom,
                          char *name, int offset_r, int offset_c,
                          int *window_id )
{
   int window_width, window_height;
   double zoom_x, zoom_y;

   if ( width <= 0 || height <= 0 || zoom == 0.0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_display_new_window", GAN_ERROR_ILLEGAL_ARGUMENT,
                         "" );
      return GAN_FALSE;
   }

   /* compute dimensions of window */
   window_width  = (int) (zoom*(double)width  + 0.5);
   window_height = (int) (zoom*(double)height + 0.5);

   /* compute zoom factors to make graphics overlay the image precisely */
   zoom_x = (double)window_width/(double)width;
   zoom_y = (double)window_height/(double)height;

   /* create display window */
   glutInitWindowSize ( window_width, window_height );
   glutInitWindowPosition ( offset_c, offset_r );
   if ( window_id == NULL ) glutCreateWindow ( name );
   else *window_id = glutCreateWindow ( name );

   /* set some standard defaults */
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
   glClearColor ( 0.0F, 0.0F, 0.0F, 0.0F );
   glViewport ( 0, 0, window_width, window_height );

   /* set coordinate frame for graphics in window */
   glMatrixMode ( GL_PROJECTION );
   glLoadIdentity();

   gluOrtho2D ( 0, width, height, 0 );

   glMatrixMode ( GL_MODELVIEW );
   glLoadIdentity();

   /* set zoom factors for image display */
   glPixelZoom ( zoom_x, -zoom_y );

   /* set origin for drawing images as the top-left corner of the window */
   glRasterPos2i ( 0, 0 );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Creates an OpenGL display window with subwindows.
 * \param rows Number of subwindows in window vertically
 * \param cols Number of subwindows in window horizontally
 * \param height The height images/graphics to be shown in the window
 * \param width The width of images/graphics to be shown in the window
 * \param zoom Zoom factor for image/graphics in the window
 * \param name A name for the window
 * \param offset_r Vertical offset of window from corner of screen
 * \param offset_c Horizontal offset of window from corner of screen
 * \param window_id Pointer to the main OpenGL display window ID
 * \param subwindow_id Pointer to the array of OpenGL subwindow IDs
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Creates a new OpenGL window with the given \a name and screen offsets, and
 * containing subwindows, each with the given dimensions and \a zoom factor,
 * in an array of \a rows by \a cols subwindows. Upon success, the ID of the
 * main OpenGL window created is passed back in the window_id pointer, and an
 * array of \a rows by \a cols OpenGL subwindow IDs is passed back in
 * \a subwindow_id. \a (*subwindow_id)[0], \a (*subwindow_id)[cols-1],
 * \a (*subwindow_id)[rows*(cols-1)] and \a (*subwindow_id)[rows*cols-1]
 * are the IDs for the top-left, top-right, bottom-left and bottom-right
 * subwindows respectively. The \a subwindow_id array is dynamically allocated
 * using \c malloc().
 *
 * \sa gan_display_new_window().
 */
Gan_Bool
 gan_display_new_window_array ( int rows, int cols,
                                int height, int width, double zoom,
                                char *name, int offset_r, int offset_c,
                                int *window_id, int **subwindow_id )
{
   int window_width, window_height, r, c, w;
   double zoom_x, zoom_y;

   if ( rows <= 0 || cols <= 0 || width <= 0 || height <= 0 || zoom == 0.0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_display_new_window_array",
                         GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      return GAN_FALSE;
   }

   /* compute dimensions of window */
   window_width  = (int) (zoom*(double)width  + 0.5);
   window_height = (int) (zoom*(double)height + 0.5);

   /* compute zoom factors to make graphics overlay the image precisely */
   zoom_x = (double)window_width/(double)width;
   zoom_y = (double)window_height/(double)height;

   /* create display window */
   glutInitWindowSize ( cols*window_width, rows*window_height );
   glutInitWindowPosition ( offset_c, offset_r );
   *window_id = glutCreateWindow ( name );

   /* set some standard defaults */
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
   glClearColor ( 0.0F, 0.0F, 0.0F, 0.0F );
   glViewport ( 0, 0, window_width, window_height );

   /* set coordinate frame for graphics in window */
   glMatrixMode ( GL_PROJECTION );
   glLoadIdentity();

   gluOrtho2D ( 0, cols*width, rows*height, 0 );

   glMatrixMode ( GL_MODELVIEW );
   glLoadIdentity();

   /* set zoom factors for image display */
   glPixelZoom ( zoom_x, -zoom_y );

   *subwindow_id = gan_malloc_array ( int, rows*cols );
   if ( *subwindow_id == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_display_new_window_array", GAN_ERROR_MALLOC_FAILED, "", rows*cols*sizeof(int) );
      return GAN_FALSE;
   }

   for ( r = w = 0; r < rows; r++ )
      for ( c = 0; c < cols; c++, w++ )
      {
         /* create display sub-window */
         (*subwindow_id)[w] = glutCreateSubWindow ( *window_id, 
                                                    c*window_width,
                                                    r*window_height,
                                                    window_width,
                                                    window_height );

         /* set some standard defaults */
         glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
         glClearColor ( 0.0F, 0.0F, 0.0F, 0.0F );
         glViewport ( 0, 0, window_width, window_height );

         /* set coordinate frame for graphics in window */
         glMatrixMode ( GL_PROJECTION );
         glLoadIdentity();

         gluOrtho2D ( 0, width, height, 0 );

         glMatrixMode ( GL_MODELVIEW );
         glLoadIdentity();

         /* set zoom factors for image display */
         glPixelZoom ( zoom_x, -zoom_y );

         /* set origin for drawing images as the top-left corner of the
            window */
         glRasterPos2i ( 0, 0 );
      }

   /* success */
   return GAN_TRUE;
}

Gan_List *list=NULL;

typedef struct
{
   int window_id;
   Gan_Image *img;
} DisplayAssoc;
  
static void display_image(void)
{
   int i, window_id = glutGetWindow();
   DisplayAssoc *assoc=NULL;

   if ( list == NULL ) return;

   /* search for image corresponding to window */
   gan_list_goto_head(list);
   for ( i = gan_list_get_size(list)-1; i >= 0; i-- )
   {
      assoc = gan_list_get_next(list, DisplayAssoc);
      if ( assoc->window_id == window_id ) break;
   }

   assert ( i >= 0 );

   /* display image */
   glRasterPos2i ( 0, 0 );
   gan_image_display ( assoc->img );
   glFlush();
}

#define QUIT 99

static void display_assoc_free ( DisplayAssoc *assoc )
{
   glutDestroyWindow ( assoc->window_id );
   gan_image_free ( assoc->img );
   free ( assoc );
}

static void ModeMenu ( int entry )
{
   switch ( entry )
   {
      case QUIT:
      {
         int i, window_id = glutGetWindow();
         DisplayAssoc *assoc;

         /* search for image corresponding to window */
         gan_list_goto_head(list);
         for ( i = gan_list_get_size(list)-1; i >= 0; i-- )
         {
            assoc = gan_list_get_next(list, DisplayAssoc);
            if ( assoc->window_id == window_id ) break;
         }

         assert ( i >= 0 );

         gan_list_goto_pos ( list, gan_list_get_pos(list)-1 );
         gan_list_delete_next ( list, (void (*)(void *))display_assoc_free );
      }
      break;

      default:
        fprintf ( stderr, "illegal menu entry %d\n", entry );
        exit(EXIT_FAILURE);
        break;
   }
}

/**
 * \brief Frees image display stuff.
 * \return No value.
 *
 * Call this function to free stored display images created by calls to
 * gan_image_display_new_window(), just prior to exiting the program.
 *
 * \sa gan_image_display_new_window().
 */
void
 gan_image_display_free_windows(void)
{
   gan_list_free ( list, (void (*)(void *))display_assoc_free );
}

/**
 * \brief Displays an image using OpenGL.
 * \param img The image to display
 * \param zoom Zoom factor
 * \param name A name for the window
 * \param offset_r Vertical offset of window from corner of screen
 * \param offset_c Horizontal offset of window from corner of screen
 * \param window_idp Pointer to the OpenGL display window ID
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Creates a new OpenGL window, with the given screen offset and zoom factor,
 * and displays the provided image in the window. The image is copied, so this
 * is not memory-efficient. Upon success, the ID of the new OpenGL window is
 * passed back in the \a window_id pointer.
 *
 * \sa gan_image_display().
 */
Gan_Bool
 gan_image_display_new_window ( const Gan_Image *img, double zoom, char *name,
                                int offset_r, int offset_c, int *window_idp )
                                   
{
   DisplayAssoc *assoc;
   int window_id;

   /* create display window */
   if ( !gan_display_new_window ( img->height, img->width, zoom, name,
                                  offset_r, offset_c, &window_id ) )
   {
      gan_err_register  ( "gan_image_display_new_window", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* set callback function for image display */
   glutDisplayFunc ( display_image );

   glutCreateMenu( ModeMenu );
   glutAddMenuEntry ( "Quit", QUIT );
   glutAttachMenu(GLUT_RIGHT_BUTTON);

   /* add image and window to list */
   img = gan_image_copy_s(img);
   if ( img == NULL )
   {
      gan_err_register  ( "gan_image_display_new_window", GAN_ERROR_FAILURE,
                          "" );
      return GAN_FALSE;
   }

   if ( list == NULL )
   {
      list = gan_list_new();
      if ( list == NULL )
      {
         gan_err_register  ( "gan_image_display_new_window", GAN_ERROR_FAILURE,
                             "" );
         return GAN_FALSE;
      }
   }
   
   assoc = gan_malloc_object(DisplayAssoc);
   if ( assoc == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number  ( "gan_image_display_new_window", GAN_ERROR_MALLOC_FAILED, "", sizeof(*assoc) );
      return GAN_FALSE;
   }

   assoc->window_id = window_id;
   assoc->img       = (Gan_Image*)img;
   gan_list_insert_first ( list, assoc );

   /* success */
   glRasterPos2i ( 0, 0 );
   gan_image_display ( assoc->img );
   glFlush();
   if ( window_idp != NULL ) *window_idp = window_id;
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

#endif /* #ifdef HAVE_GLUT */
