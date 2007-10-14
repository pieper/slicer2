/**
 * File:          $RCSfile: movie.c,v $
 * Module:        Image movie definitions and functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:29 $
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

#include <string.h>
#include <gandalf/image/io/movie.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/allocate.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageIO
 * \{
 */

/**
 * \brief Create new movie structure for image sequence I/O.
 * \param directory The directory in which the image sequence is stored
 * \param basename The base name of the sequence
 * \param no_digits Number of digits in number field of file name
 * \param suffix String at end of each file name
 * \param first Number of first file in the sequence
 * \param no_images Number of images in the sequence
 * \param file_format File format of the image files
 * \return Pointer to successfully created structure, or \c NULL on failure.
 *
 * Builds and returns a pointer to a new structure containing the specification
 * of an image sequence.
 *
 * \sa gan_movie_free().
 */
Gan_MovieStruct *
 gan_movie_new ( const char *directory,
                 const char *basename, int no_digits,
                 const char *suffix, int first, int no_images,
                 Gan_ImageFileFormat file_format )
{
   Gan_MovieStruct *movie;

   /* allocate movie structure */
   movie = gan_malloc_object(Gan_MovieStruct);
   if ( movie == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_movie_new", GAN_ERROR_MALLOC_FAILED, "movie structure" );
      return NULL;
   }

   /* allocate and fill strings inside movie structure */
   movie->directory = gan_malloc_array ( char, strlen(directory)+1 );
   movie->basename  = gan_malloc_array ( char, strlen(basename)+1 );
   movie->suffix    = gan_malloc_array ( char, strlen(suffix)+1 );
   if ( movie->directory == NULL || movie->basename == NULL ||
        movie->suffix == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_movie_new", GAN_ERROR_MALLOC_FAILED, "movie structure" );
      return NULL;
   }

   strcpy ( movie->directory, directory );
   strcpy ( movie->basename,  basename  );
   strcpy ( movie->suffix,    suffix    );

   /* copy other fields */
   movie->no_digits   = no_digits;
   movie->first       = first;
   movie->no_images   = no_images;
   movie->file_format = file_format;

   /* set defaults for other parameters */
   movie->step = 1;
   movie->crop_xlow = movie->crop_xhigh =
   movie->crop_ylow = movie->crop_yhigh = 0;

   /* success */
   return movie;
}

/**
 * \brief Sets step attribute of movie structure.
 * \param movie Pointer to movie structure
 * \param step Change in number between each frame of the sequence
 * \return No value.
 *
 * Sets step attribute of movie structure created by gan_movie_new(),
 * i.e. the number change between adjacent frames in the sequence.
 * The default is one.
 *
 * \sa gan_movie_new().
 */
void
 gan_movie_set_step ( Gan_MovieStruct *movie, int step )
{
   movie->step = step;
}

/**
 * \brief Sets crop window attribute of movie structure.
 * \param movie Pointer to movie structure
 * \param crop_xlow Low x-value of crop region
 * \param crop_xhigh High x-value of crop region
 * \param crop_ylow Low y-value of crop region
 * \param crop_yhigh High y-value of crop region
 * \return No value.
 *
 * Sets crop window attribute of movie structure created by gan_movie_new().
 *
 * \sa gan_movie_new().
 */
void
 gan_movie_set_crop_window ( Gan_MovieStruct *movie,
                             int crop_xlow, int crop_xhigh,
                             int crop_ylow, int crop_yhigh )
{
   movie->crop_xlow  = crop_xlow;
   movie->crop_xhigh = crop_xhigh;
   movie->crop_ylow  = crop_ylow;
   movie->crop_yhigh = crop_yhigh;
}

/**
 * \brief Frees a movie structure.
 * \param movie Pointer to the movie structure to free
 * \return No value.
 *
 * Frees a structure containing the specification of an image sequence.
 *
 * \sa gan_movie_new().
 */
void
 gan_movie_free ( Gan_MovieStruct *movie )
{
   gan_free_va ( movie->suffix, movie->basename, movie->directory, NULL );
   free ( movie );
}

/* returns the length in digits of the given decimal number */
static int decimal_length ( int num )
{
   int no_digits = 1;

   if ( num < 0 )
   {
      /* add one for the negative sign */
      no_digits++;
      num = -num;
   }

   while ( num >= 10 )
   {
      no_digits++;
      num /= 10;
   }

   return no_digits;
}

/**
 * \brief Builds the full name of an image in a movie.
 * \param movie Pointer to a movie structure
 * \param number The number of the image in the sequence
 * \param string A string to be filled with the file name or NULL
 * \param slen The length of the provided string (zero if string is NULL)
 * \return The filled file name as a string, or \c NULL on failure.
 *
 * Builds and returns the full name of a specific image in a \a movie.
 * If \a string is \c NULL, the string is malloc()'d and returned;
 * otherwise the provided string is filled and returned. If the string is
 * not long enough, \c NULL is returned.
 *
 * \a slen defines the maximum length of the string, including the null
 * terminator character, i.e. the total size of the string array.
 *
 * \sa gan_movie_image_read().
 */
char *
 gan_movie_image_name ( Gan_MovieStruct *movie, int number,
                        char *string, unsigned slen )
{
   char fmt[100];
   int length;

   if ( number < 0 || number >= movie->no_images )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_movie_image_name", GAN_ERROR_OUTSIDE_RANGE, "image number" );
      return NULL;
   }

   /* convert number in sequence to file number */
   number = movie->first+number*movie->step;

   /* compute length of string and fill format string for final sprintf */
   length = strlen(movie->directory) + 1 + strlen(movie->basename);
   if ( movie->no_digits == 0 )
   {
      length += decimal_length(number);
      strcpy ( fmt, "%s/%s%d%s" );
   }
   else
   {
      if ( decimal_length(number) > movie->no_digits )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_movie_image_name", GAN_ERROR_TOO_LARGE, "image number string" );
         return NULL;
      }

      length += movie->no_digits;
      sprintf ( fmt, "%%s/%%s%%0%dd%%s", movie->no_digits );
   }

   /* add length of suffix and null string terminator */
   length += strlen(movie->suffix) + 1;

   if ( string == NULL )
   {
      string = gan_malloc_array ( char, length );
      if ( string == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_movie_image_name", GAN_ERROR_MALLOC_FAILED, "image file name string" );
         return NULL;
      }
   }
   else
      if ( length > (int)slen )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_movie_image_name", GAN_ERROR_TOO_SMALL, "image file name string" );
         return NULL;
      }

   sprintf ( string, fmt, movie->directory, movie->basename, number, movie->suffix );
   return string;
}

/**
 * \brief Reads an image from an image sequence.
 * \param movie Pointer to a movie structure
 * \param number The number of the image in the sequence
 * \param image The image structure to read the image data into or NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads a specific image file from an image \a movie into the provided image
 * structure. If \a image is \c NULL a new image is dynamically allocated;
 * otherwise the already allocated image structure is reused.
 *
 * \sa gan_movie_image_write().
 */
Gan_Image *
 gan_movie_image_read ( Gan_MovieStruct *movie, int number, Gan_Image *image )
{
   char *filename;

   /* build file name */
   filename = gan_movie_image_name ( movie, number, NULL, 0 );
   if ( filename == NULL )
   {
      gan_err_register ( "gan_movie_image_read", GAN_ERROR_FAILURE, "building file name" );
      return NULL;
   }

   /* read file */
   image = gan_image_read ( filename, movie->file_format, image, NULL, NULL );
   if ( image == NULL )
   {
      gan_err_register ( "gan_movie_image_read", GAN_ERROR_FAILURE, filename );
      return NULL;
   }

   /* free file name string */
   free ( filename );

   /* success */
   return image;
}

/**
 * \brief Writes an image to an image sequence.
 * \param movie Pointer to a movie structure
 * \param number The number of the image in the sequence
 * \param image The image structure to write to the file
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes an image structure into a specific image file from an image sequence.
 *
 * \sa gan_movie_image_read().
 */
Gan_Bool
 gan_movie_image_write ( Gan_MovieStruct *movie, int number, Gan_Image *image )
{
   char *filename;

   /* build file name */
   filename = gan_movie_image_name ( movie, number, NULL, 0 );
   if ( filename == NULL )
   {
      gan_err_register ( "gan_movie_image_write", GAN_ERROR_FAILURE, "building file name" );
      return GAN_FALSE;
   }

   /* write file */
   if ( !gan_image_write ( filename, movie->file_format, image, NULL ))
   {
      gan_err_register ( "gan_movie_image_write", GAN_ERROR_FAILURE, filename );
      return GAN_FALSE;
   }

   /* free file name string */
   free ( filename );

   /* success */
   return GAN_TRUE;
}
