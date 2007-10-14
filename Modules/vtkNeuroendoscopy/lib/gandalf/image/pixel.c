/**
 * File:          $RCSfile: pixel.c,v $
 * Module:        Image pixel definitions and functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:28 $
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

#include <gandalf/image/pixel.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageAccessPixel
 * \{
 */

float lookup_table_8bit[256] = 
{0.0F, 0.00392156886F, 0.00784313772F, 0.011764707F, 0.0156862754F, 0.0196078438F, 0.0235294122F, 0.0274509806F, 
   0.0313725509F, 0.0352941193F, 0.0392156877F, 0.0431372561F, 0.0470588244F, 0.0509803928F, 0.0549019612F, 0.0588235296F,
   0.0627451017F, 0.0666666701F, 0.0705882385F, 0.0745098069F, 0.0784313753F, 0.0823529437F, 0.0862745121F, 0.0901960805F, 
   0.0941176489F, 0.0980392173F, 0.101960786F, 0.105882354F, 0.109803922F, 0.113725491F, 0.117647059F, 0.121568628F, 
   0.125490203F, 0.129411772F, 0.13333334F, 0.137254909F, 0.141176477F, 0.145098045F, 0.149019614F, 0.152941182F, 
   0.156862751F, 0.160784319F, 0.164705887F, 0.168627456F, 0.172549024F, 0.176470593F, 0.180392161F, 0.184313729F, 
   0.188235298F, 0.192156866F, 0.196078435F, 0.200000003F, 0.203921571F, 0.20784314F, 0.211764708F, 0.215686277F, 
   0.219607845F, 0.223529413F, 0.227450982F, 0.23137255F, 0.235294119F, 0.239215687F, 0.243137255F, 0.247058824F,
   0.250980407F, 0.254901975F, 0.258823544F, 0.262745112F, 0.266666681F, 0.270588249F, 0.274509817F, 0.278431386F, 
   0.282352954F, 0.286274523F, 0.290196091F, 0.294117659F, 0.298039228F, 0.301960796F, 0.305882365F, 0.309803933F, 
   0.313725501F, 0.31764707F, 0.321568638F, 0.325490206F, 0.329411775F, 0.333333343F, 0.337254912F, 0.34117648F, 
   0.345098048F, 0.349019617F, 0.352941185F, 0.356862754F, 0.360784322F, 0.36470589F, 0.368627459F, 0.372549027F, 
   0.376470596F, 0.380392164F, 0.384313732F, 0.388235301F, 0.392156869F, 0.396078438F, 0.400000006F, 0.403921574F, 
   0.407843143F, 0.411764711F, 0.41568628F, 0.419607848F, 0.423529416F, 0.427450985F, 0.431372553F, 0.435294122F, 
   0.43921569F, 0.443137258F, 0.447058827F, 0.450980395F, 0.454901963F, 0.458823532F, 0.4627451F, 0.466666669F, 
   0.470588237F, 0.474509805F, 0.478431374F, 0.482352942F, 0.486274511F, 0.490196079F, 0.494117647F, 0.498039216F,
   0.501960814F, 0.505882382F, 0.509803951F, 0.513725519F, 0.517647088F, 0.521568656F, 0.525490224F, 0.529411793F, 
   0.533333361F, 0.53725493F, 0.541176498F, 0.545098066F, 0.549019635F, 0.552941203F, 0.556862772F, 0.56078434F, 
   0.564705908F, 0.568627477F, 0.572549045F, 0.576470613F, 0.580392182F, 0.58431375F, 0.588235319F, 0.592156887F, 
   0.596078455F, 0.600000024F, 0.603921592F, 0.607843161F, 0.611764729F, 0.615686297F, 0.619607866F, 0.623529434F, 
   0.627451003F, 0.631372571F, 0.635294139F, 0.639215708F, 0.643137276F, 0.647058845F, 0.650980413F, 0.654901981F, 
   0.65882355F, 0.662745118F, 0.666666687F, 0.670588255F, 0.674509823F, 0.678431392F, 0.68235296F, 0.686274529F, 
   0.690196097F, 0.694117665F, 0.698039234F, 0.701960802F, 0.70588237F, 0.709803939F, 0.713725507F, 0.717647076F, 
   0.721568644F, 0.725490212F, 0.729411781F, 0.733333349F, 0.737254918F, 0.741176486F, 0.745098054F, 0.749019623F,
   0.752941191F, 0.75686276F, 0.760784328F, 0.764705896F, 0.768627465F, 0.772549033F, 0.776470602F, 0.78039217F, 
   0.784313738F, 0.788235307F, 0.792156875F, 0.796078444F, 0.800000012F, 0.80392158F, 0.807843149F, 0.811764717F, 
   0.815686285F, 0.819607854F, 0.823529422F, 0.827450991F, 0.831372559F, 0.835294127F, 0.839215696F, 0.843137264F, 
   0.847058833F, 0.850980401F, 0.854901969F, 0.858823538F, 0.862745106F, 0.866666675F, 0.870588243F, 0.874509811F, 
   0.87843138F, 0.882352948F, 0.886274517F, 0.890196085F, 0.894117653F, 0.898039222F, 0.90196079F, 0.905882359F, 
   0.909803927F, 0.913725495F, 0.917647064F, 0.921568632F, 0.925490201F, 0.929411769F, 0.933333337F, 0.937254906F, 
   0.941176474F, 0.945098042F, 0.949019611F, 0.952941179F, 0.956862748F, 0.960784316F, 0.964705884F, 0.968627453F, 
   0.972549021F, 0.97647059F, 0.980392158F, 0.984313726F, 0.988235295F, 0.992156863F, 0.996078432F, 1.0F };

/**
 * \brief Convert RGB \c unsigned \c char pixel to grey-level \c unsigned \c char pixel.
 */
unsigned char
 gan_pixel_rgbuc_to_yuc ( const Gan_RGBPixel_uc *rgb )
{
   return ((unsigned char) ((79*((unsigned) rgb->R) + 156*((unsigned) rgb->G) + 21*((unsigned) rgb->B) + 128)/256));
}

/**
 * \brief Convert RGB \c unsigned \c char pixel to grey-level \c unsigned \c short pixel.
 */
unsigned short
 gan_pixel_rgbuc_to_yus ( const Gan_RGBPixel_uc *rgb )
{
#if (SIZEOF_SHORT-SIZEOF_CHAR == 1)
   return (79*((unsigned short) rgb->R) + 156*((unsigned short) rgb->G) + 21*((unsigned short) rgb->B));
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert RGB \c unsigned \c char pixel to grey-level \c unsigned \c int pixel.
 */
unsigned int
 gan_pixel_rgbuc_to_yui ( const Gan_RGBPixel_uc *rgb )
{
#if (SIZEOF_INT-SIZEOF_CHAR == 3)
   return (((79u*(unsigned)rgb->R + 156u*(unsigned)rgb->G +
             21u*(unsigned)rgb->B)) << 16u);
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert RGB \c unsigned \c short pixel to grey-level \c unsigned \c char pixel.
 */
unsigned char
 gan_pixel_rgbus_to_yuc ( const Gan_RGBPixel_us *rgb )
{
#if (SIZEOF_INT > SIZEOF_SHORT) && (SIZEOF_SHORT-SIZEOF_CHAR == 1) && (SIZEOF_SHORT==2)
   return ((unsigned char) ((79u*(unsigned)rgb->R + 156u*(unsigned)rgb->G + 21u*(unsigned)rgb->B + 32768) >> 16));
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert RGB \c unsigned \c short pixel to grey-level \c unsigned \c short pixel.
 */
unsigned short
 gan_pixel_rgbus_to_yus ( const Gan_RGBPixel_us *rgb )
{
#if (SIZEOF_SHORT==2) && (SIZEOF_INT - SIZEOF_SHORT >= 2)
   return ((unsigned short) ((79u*(unsigned)rgb->R + 156u*(unsigned)rgb->G + 21u*(unsigned)rgb->B + 128u) >> 8u));
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert RGB \c float pixel to grey-level \c float pixel.
 */
float
 gan_pixel_rgbui_to_yf ( const Gan_RGBPixel_ui *rgb )
{
   return (0.30859375F*gan_pixel_ui_to_f(rgb->R) +
           0.609375F*gan_pixel_ui_to_f(rgb->G) +
           0.08203125F*gan_pixel_ui_to_f(rgb->B));
}

/**
 * \brief Convert RGB \c float pixel to grey-level \c float pixel.
 */
float
 gan_pixel_rgbf_to_yf ( const Gan_RGBPixel_f *rgb )
{
   return (0.30859375F*rgb->R + 0.609375F*rgb->G + 0.08203125F*rgb->B);
}

/**
 * \brief Convert RGB \c double pixel to grey-level \c double pixel.
 */
double
 gan_pixel_rgbd_to_yd ( const Gan_RGBPixel_d *rgb )
{
   return (0.30859375*rgb->R + 0.609375*rgb->G + 0.08203125*rgb->B);
}

/**
 * \brief Convert RGBA \c unsigned \c char pixel to grey-level \c unsigned \c char pixel.
 */
unsigned char
 gan_pixel_rgbauc_to_yuc ( const Gan_RGBAPixel_uc *rgba )
{
   return ((unsigned char) ((79u*(unsigned)rgba->R + 156u*(unsigned)rgba->G + 21u*(unsigned)rgba->B + 128) >> 8));
}

/**
 * \brief Convert RGBA \c unsigned \c char pixel to grey-level \c unsigned \c short pixel.
 */
unsigned short
 gan_pixel_rgbauc_to_yus ( const Gan_RGBAPixel_uc *rgba )
{
#if (SIZEOF_SHORT-SIZEOF_CHAR == 1)
   return (79*(unsigned short)rgba->R + 156*(unsigned short)rgba->G + 21*(unsigned short)rgba->B);
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert RGBA \c unsigned \c char pixel to grey-level \c unsigned \c int pixel.
 */
unsigned int
 gan_pixel_rgbauc_to_yui ( const Gan_RGBAPixel_uc *rgba )
{
#if (SIZEOF_INT-SIZEOF_CHAR == 3)
   return 0x10000*(79*((unsigned)rgba->R) + 156*((unsigned)rgba->G) + 21*((unsigned)rgba->B));
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert RGBA \c unsigned \c short pixel to grey-level \c unsigned \c char pixel.
 */
unsigned char
 gan_pixel_rgbaus_to_yuc ( const Gan_RGBAPixel_us *rgba )
{
#if (SIZEOF_INT > SIZEOF_SHORT) && (SIZEOF_SHORT-SIZEOF_CHAR == 1) && (SIZEOF_SHORT==2)
   return ((unsigned char) ((79u*(unsigned)rgba->R + 156u*(unsigned)rgba->G + 21u*(unsigned)rgba->B + 32768u) >> 16));
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert RGBA \c unsigned \c short pixel to grey-level \c unsigned \c short pixel.
 */
unsigned short
 gan_pixel_rgbaus_to_yus ( const Gan_RGBAPixel_us *rgba )
{
#if (SIZEOF_INT > SIZEOF_SHORT)
   return ((unsigned short) ((79u*(unsigned)rgba->R + 156u*(unsigned)rgba->G + 21u*(unsigned)rgba->B + 128u) >> 8));
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert RGBA \c float pixel to grey-level \c float pixel.
 */
float
 gan_pixel_rgbaui_to_yf ( const Gan_RGBAPixel_ui *rgba )
{
   return (0.30859375F*gan_pixel_ui_to_f(rgba->R) +
           0.609375F*gan_pixel_ui_to_f(rgba->G) +
           0.08203125F*gan_pixel_ui_to_f(rgba->B));
}

/**
 * \brief Convert RGBA \c float pixel to grey-level \c float pixel.
 */
float
 gan_pixel_rgbaf_to_yf ( const Gan_RGBAPixel_f *rgba )
{
   return (0.30859375F*rgba->R + 0.609375F*rgba->G + 0.08203125F*rgba->B);
}

/**
 * \brief Convert RGBA \c double pixel to grey-level \c double pixel.
 */
double
 gan_pixel_rgbad_to_yd ( const Gan_RGBAPixel_d *rgba )
{
   return (0.30859375*rgba->R + 0.609375*rgba->G + 0.08203125*rgba->B);
}

/**
 * \brief Convert BGR \c unsigned \c char pixel to grey-level \c unsigned \c char pixel.
 */
unsigned char
 gan_pixel_bgruc_to_yuc ( const Gan_BGRPixel_uc *bgr )
{
   return ((unsigned char) ((21*((unsigned) bgr->R) + 156*((unsigned) bgr->G) + 79*((unsigned) bgr->B) + 128)/256));
}

/**
 * \brief Convert BGR \c unsigned \c char pixel to grey-level \c unsigned \c short pixel.
 */
unsigned short
 gan_pixel_bgruc_to_yus ( const Gan_BGRPixel_uc *bgr )
{
#if (SIZEOF_SHORT-SIZEOF_CHAR == 1)
   return (21*((unsigned short) bgr->R) + 156*((unsigned short) bgr->G) + 79*((unsigned short) bgr->B));
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert BGR \c unsigned \c char pixel to grey-level \c unsigned \c int pixel.
 */
unsigned int
 gan_pixel_bgruc_to_yui ( const Gan_BGRPixel_uc *bgr )
{
#if (SIZEOF_INT-SIZEOF_CHAR == 3)
   return (((21u*(unsigned)bgr->R + 156u*(unsigned)bgr->G + 79u*(unsigned)bgr->B)) << 16u);
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert BGR \c unsigned \c short pixel to grey-level \c unsigned \c char pixel.
 */
unsigned char
 gan_pixel_bgrus_to_yuc ( const Gan_BGRPixel_us *bgr )
{
#if (SIZEOF_INT > SIZEOF_SHORT) && (SIZEOF_SHORT-SIZEOF_CHAR == 1) && (SIZEOF_SHORT==2)
   return ((unsigned char) ((21u*(unsigned)bgr->R + 156u*(unsigned)bgr->G + 79u*(unsigned)bgr->B + 32768) >> 16));
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert BGR \c unsigned \c short pixel to grey-level \c unsigned \c short pixel.
 */
unsigned short
 gan_pixel_bgrus_to_yus ( const Gan_BGRPixel_us *bgr )
{
#if (SIZEOF_SHORT==2) && (SIZEOF_INT - SIZEOF_SHORT >= 2)
   return ((unsigned short) ((21u*(unsigned)bgr->R + 156u*(unsigned)bgr->G + 79u*(unsigned)bgr->B + 128u) >> 8u));
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert BGR \c float pixel to grey-level \c float pixel.
 */
float
 gan_pixel_bgrui_to_yf ( const Gan_BGRPixel_ui *bgr )
{
   return (0.08203125F*gan_pixel_ui_to_f(bgr->R) +
           0.609375F*gan_pixel_ui_to_f(bgr->G) +
           0.30859375F*gan_pixel_ui_to_f(bgr->B));
}

/**
 * \brief Convert BGR \c float pixel to grey-level \c float pixel.
 */
float
 gan_pixel_bgrf_to_yf ( const Gan_BGRPixel_f *bgr )
{
   return (0.08203125F*bgr->R + 0.609375F*bgr->G + 0.30859375F*bgr->B);
}

/**
 * \brief Convert BGR \c double pixel to grey-level \c double pixel.
 */
double
 gan_pixel_bgrd_to_yd ( const Gan_BGRPixel_d *bgr )
{
   return (0.08203125*bgr->R + 0.609375*bgr->G + 0.30859375*bgr->B);
}

/**
 * \brief Convert BGRA \c unsigned \c char pixel to grey-level \c unsigned \c char pixel.
 */
unsigned char
 gan_pixel_bgrauc_to_yuc ( const Gan_BGRAPixel_uc *bgra )
{
   return ((unsigned char) ((21u*(unsigned)bgra->R + 156u*(unsigned)bgra->G + 79u*(unsigned)bgra->B + 128) >> 8));
}

/**
 * \brief Convert BGRA \c unsigned \c char pixel to grey-level \c unsigned \c short pixel.
 */
unsigned short
 gan_pixel_bgrauc_to_yus ( const Gan_BGRAPixel_uc *bgra )
{
#if (SIZEOF_SHORT-SIZEOF_CHAR == 1)
   return (21*(unsigned short)bgra->R + 156*(unsigned short)bgra->G + 79*(unsigned short)bgra->B);
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert BGRA \c unsigned \c char pixel to grey-level \c unsigned \c int pixel.
 */
unsigned int
 gan_pixel_bgrauc_to_yui ( const Gan_BGRAPixel_uc *bgra )
{
#if (SIZEOF_INT-SIZEOF_CHAR == 3)
   return 0x10000*(21*((unsigned)bgra->R) + 156*((unsigned)bgra->G) + 79*((unsigned)bgra->B));
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert BGRA \c unsigned \c short pixel to grey-level \c unsigned \c char pixel.
 */
unsigned char
 gan_pixel_bgraus_to_yuc ( const Gan_BGRAPixel_us *bgra )
{
#if (SIZEOF_INT > SIZEOF_SHORT) && (SIZEOF_SHORT-SIZEOF_CHAR == 1) && (SIZEOF_SHORT==2)
   return ((unsigned char) ((21u*(unsigned)bgra->R + 156u*(unsigned)bgra->G + 79u*(unsigned)bgra->B + 32768u) >> 16));
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert BGRA \c unsigned \c short pixel to grey-level \c unsigned \c short pixel.
 */
unsigned short
 gan_pixel_bgraus_to_yus ( const Gan_BGRAPixel_us *bgra )
{
#if (SIZEOF_INT > SIZEOF_SHORT)
   return ((unsigned short) ((21u*(unsigned)bgra->R + 156u*(unsigned)bgra->G + 79u*(unsigned)bgra->B + 128u) >> 8));
#else
#error "Illegal sizes"
#endif
}

/**
 * \brief Convert BGRA \c float pixel to grey-level \c float pixel.
 */
float
 gan_pixel_bgraui_to_yf ( const Gan_BGRAPixel_ui *bgra )
{
   return (0.08203125F*gan_pixel_ui_to_f(bgra->R) +
           0.609375F*gan_pixel_ui_to_f(bgra->G) +
           0.30859375F*gan_pixel_ui_to_f(bgra->B));
}

/**
 * \brief Convert BGRA \c float pixel to grey-level \c float pixel.
 */
float
 gan_pixel_bgraf_to_yf ( const Gan_BGRAPixel_f *bgra )
{
   return (0.08203125F*bgra->R + 0.609375F*bgra->G + 0.30859375F*bgra->B);
}

/**
 * \brief Convert BGRA \c double pixel to grey-level \c double pixel.
 */
double
 gan_pixel_bgrad_to_yd ( const Gan_BGRAPixel_d *bgra )
{
   return (0.08203125*bgra->R + 0.609375*bgra->G + 0.30859375*bgra->B);
}

static Gan_Bool
 convert_pixel_gl_gl ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_GREY_LEVEL_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_gl_gl", GAN_ERROR_ILLEGAL_ARGUMENT,
                            "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_d_to_uc(inpix->data.gl.d);
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_d_to_us(inpix->data.gl.d);
             break;

           case GAN_INT:
             outpix->data.gl.i = gan_pixel_d_to_i(inpix->data.gl.d);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_d_to_ui(inpix->data.gl.d);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = (float)inpix->data.gl.d;
             break;

           case GAN_BOOL:
             outpix->data.gl.b = (inpix->data.gl.d == 0.0)
                                 ? GAN_FALSE : GAN_TRUE;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_f_to_uc(inpix->data.gl.f);
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_f_to_us(inpix->data.gl.f);
             break;

           case GAN_INT:
             outpix->data.gl.i = gan_pixel_f_to_i(inpix->data.gl.f);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_f_to_ui(inpix->data.gl.f);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = (double)inpix->data.gl.f;
             break;

           case GAN_BOOL:
             outpix->data.gl.b = (inpix->data.gl.f == 0.0F)
                                 ? GAN_FALSE : GAN_TRUE;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_gl",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_uc_to_us(inpix->data.gl.uc);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_uc_to_ui(inpix->data.gl.uc);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_uc_to_d(inpix->data.gl.uc);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_uc_to_f(inpix->data.gl.uc);
             break;

           case GAN_BOOL:
             outpix->data.gl.b = (inpix->data.gl.uc == 0)
                                 ? GAN_FALSE : GAN_TRUE;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_gl",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_us_to_uc(inpix->data.gl.us);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_us_to_ui(inpix->data.gl.us);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_us_to_d(inpix->data.gl.us);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_us_to_f(inpix->data.gl.us);
             break;

           case GAN_BOOL:
             outpix->data.gl.b = (inpix->data.gl.us == 0)
                                 ? GAN_FALSE : GAN_TRUE;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_gl",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_ui_to_uc(inpix->data.gl.ui);
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_ui_to_us(inpix->data.gl.ui);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_ui_to_d(inpix->data.gl.ui);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_ui_to_f(inpix->data.gl.ui);
             break;

           case GAN_BOOL:
             outpix->data.gl.b = (inpix->data.gl.ui == 0)
                                 ? GAN_FALSE : GAN_TRUE;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_gl",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_INT:
        switch ( outpix->type )
        {
           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_i_to_d(inpix->data.gl.i);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_i_to_f(inpix->data.gl.i);
             break;

           case GAN_BOOL:
             outpix->data.gl.b = (inpix->data.gl.i == 0)
                                 ? GAN_FALSE : GAN_TRUE;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_gl",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_BOOL:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = inpix->data.gl.b ? UCHAR_MAX : 0;
             break;

           case GAN_USHORT:
             outpix->data.gl.us = inpix->data.gl.b ? USHRT_MAX : 0;
             break;

           case GAN_UINT:
             outpix->data.gl.ui = inpix->data.gl.b ? UINT_MAX : 0;
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = inpix->data.gl.b ? 1.0 : 0.0;
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = inpix->data.gl.b ? 1.0F : 0.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_gl",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_gl_gl", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_gl_gla ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_GREY_LEVEL_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_gl_gla",
                            GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif
   
   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_d_to_uc(inpix->data.gl.d);
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_d_to_us(inpix->data.gl.d);
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_d_to_ui(inpix->data.gl.d);
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = (float)inpix->data.gl.d;
             outpix->data.gla.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = inpix->data.gl.d;
             outpix->data.gla.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_f_to_uc(inpix->data.gl.f);
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_f_to_us(inpix->data.gl.f);
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_f_to_ui(inpix->data.gl.f);
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = inpix->data.gl.f;
             outpix->data.gla.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = (double)inpix->data.gl.f;
             outpix->data.gla.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_gla",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = inpix->data.gl.uc;
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_uc_to_us(inpix->data.gl.uc);
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_uc_to_ui(inpix->data.gl.uc);
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_uc_to_d(inpix->data.gl.uc);
             outpix->data.gla.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_uc_to_f(inpix->data.gl.uc);
             outpix->data.gla.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_gla",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_us_to_uc(inpix->data.gl.us);
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = inpix->data.gl.us;
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_us_to_ui(inpix->data.gl.us);
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_us_to_d(inpix->data.gl.us);
             outpix->data.gla.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_us_to_f(inpix->data.gl.us);
             outpix->data.gla.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_gla",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_ui_to_uc(inpix->data.gl.ui);
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_ui_to_us(inpix->data.gl.ui);
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = inpix->data.gl.ui;
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_ui_to_d(inpix->data.gl.ui);
             outpix->data.gla.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_ui_to_f(inpix->data.gl.ui);
             outpix->data.gla.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_gla",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_gl_gla", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_gl_rgb ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_GREY_LEVEL_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_gl_rgb", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = outpix->data.rgb.uc.G = outpix->data.rgb.uc.B = gan_pixel_d_to_uc(inpix->data.gl.d);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = outpix->data.rgb.us.G = outpix->data.rgb.us.B = gan_pixel_d_to_us(inpix->data.gl.d);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = outpix->data.rgb.ui.G = outpix->data.rgb.ui.B = gan_pixel_d_to_ui(inpix->data.gl.d);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = outpix->data.rgb.f.G = outpix->data.rgb.f.B = (float)inpix->data.gl.d;
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = outpix->data.rgb.d.G = outpix->data.rgb.d.B = inpix->data.gl.d;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = outpix->data.rgb.uc.G = outpix->data.rgb.uc.B = gan_pixel_f_to_uc(inpix->data.gl.f);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = outpix->data.rgb.us.G = outpix->data.rgb.us.B = gan_pixel_f_to_us(inpix->data.gl.f);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = outpix->data.rgb.ui.G = outpix->data.rgb.ui.B = gan_pixel_f_to_ui(inpix->data.gl.f);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = outpix->data.rgb.f.G = outpix->data.rgb.f.B = inpix->data.gl.f;
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = outpix->data.rgb.d.G = outpix->data.rgb.d.B = (double)inpix->data.gl.f;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = outpix->data.rgb.uc.G = outpix->data.rgb.uc.B = inpix->data.gl.uc;
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = outpix->data.rgb.us.G = outpix->data.rgb.us.B = gan_pixel_uc_to_us(inpix->data.gl.uc);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = outpix->data.rgb.ui.G = outpix->data.rgb.ui.B = gan_pixel_uc_to_ui(inpix->data.gl.uc);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = outpix->data.rgb.f.G = outpix->data.rgb.f.B = gan_pixel_uc_to_f(inpix->data.gl.uc);
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = outpix->data.rgb.d.G = outpix->data.rgb.d.B = gan_pixel_uc_to_d(inpix->data.gl.uc);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = outpix->data.rgb.uc.G = outpix->data.rgb.uc.B = gan_pixel_us_to_uc(inpix->data.gl.us);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = outpix->data.rgb.us.G = outpix->data.rgb.us.B = inpix->data.gl.us;
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = outpix->data.rgb.ui.G = outpix->data.rgb.ui.B = gan_pixel_us_to_ui(inpix->data.gl.us);
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = outpix->data.rgb.d.G = outpix->data.rgb.d.B = gan_pixel_us_to_d(inpix->data.gl.us);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = outpix->data.rgb.f.G = outpix->data.rgb.f.B = gan_pixel_us_to_f(inpix->data.gl.us);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = outpix->data.rgb.uc.G = outpix->data.rgb.uc.B = gan_pixel_ui_to_uc(inpix->data.gl.ui);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = outpix->data.rgb.us.G = outpix->data.rgb.us.B = gan_pixel_ui_to_us(inpix->data.gl.ui);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = outpix->data.rgb.ui.G = outpix->data.rgb.ui.B = inpix->data.gl.ui;
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = outpix->data.rgb.d.G = outpix->data.rgb.d.B = gan_pixel_ui_to_d(inpix->data.gl.ui);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = outpix->data.rgb.f.G = outpix->data.rgb.f.B = gan_pixel_ui_to_f(inpix->data.gl.ui);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_gl_rgb", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_gl_rgba ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_GREY_LEVEL_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_gl_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif
   
   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = outpix->data.rgba.uc.G = outpix->data.rgba.uc.B = gan_pixel_d_to_uc(inpix->data.gl.d);
             outpix->data.rgba.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = outpix->data.rgba.us.G = outpix->data.rgba.us.B = gan_pixel_d_to_us(inpix->data.gl.d);
             outpix->data.rgba.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = outpix->data.rgba.ui.G = outpix->data.rgba.ui.B = gan_pixel_d_to_ui(inpix->data.gl.d);
             outpix->data.rgba.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = outpix->data.rgba.f.G = outpix->data.rgba.f.B = (float)inpix->data.gl.d;
             outpix->data.rgba.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = outpix->data.rgba.d.G = outpix->data.rgba.d.B = inpix->data.gl.d;
             outpix->data.rgba.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = outpix->data.rgba.uc.G = outpix->data.rgba.uc.B = gan_pixel_f_to_uc(inpix->data.gl.f);
             outpix->data.rgba.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = outpix->data.rgba.us.G = outpix->data.rgba.us.B = gan_pixel_f_to_us(inpix->data.gl.f);
             outpix->data.rgba.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = outpix->data.rgba.ui.G = outpix->data.rgba.ui.B = gan_pixel_f_to_ui(inpix->data.gl.f);
             outpix->data.rgba.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = outpix->data.rgba.f.G = outpix->data.rgba.f.B = inpix->data.gl.f;
             outpix->data.rgba.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = outpix->data.rgba.d.G = outpix->data.rgba.d.B = (double)inpix->data.gl.f;
             outpix->data.rgba.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = outpix->data.rgba.uc.G = outpix->data.rgba.uc.B = inpix->data.gl.uc;
             outpix->data.rgba.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = outpix->data.rgba.us.G = outpix->data.rgba.us.B = gan_pixel_uc_to_us(inpix->data.gl.uc);
             outpix->data.rgba.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = outpix->data.rgba.ui.G = outpix->data.rgba.ui.B = gan_pixel_uc_to_ui(inpix->data.gl.uc);
             outpix->data.rgba.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = outpix->data.rgba.d.G = outpix->data.rgba.d.B = gan_pixel_uc_to_d(inpix->data.gl.uc);
             outpix->data.rgba.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = outpix->data.rgba.f.G = outpix->data.rgba.f.B = gan_pixel_uc_to_f(inpix->data.gl.uc);
             outpix->data.rgba.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = outpix->data.rgba.uc.G = outpix->data.rgba.uc.B = gan_pixel_us_to_uc(inpix->data.gl.us);
             outpix->data.rgba.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = outpix->data.rgba.us.G = outpix->data.rgba.us.B = inpix->data.gl.us;
             outpix->data.rgba.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = outpix->data.rgba.ui.G = outpix->data.rgba.ui.B = gan_pixel_us_to_ui(inpix->data.gl.us);
             outpix->data.rgba.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = outpix->data.rgba.d.G = outpix->data.rgba.d.B = gan_pixel_us_to_d(inpix->data.gl.us);
             outpix->data.rgba.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = outpix->data.rgba.f.G = outpix->data.rgba.f.B = gan_pixel_us_to_f(inpix->data.gl.us);
             outpix->data.rgba.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = outpix->data.rgba.uc.G = outpix->data.rgba.uc.B = gan_pixel_ui_to_uc(inpix->data.gl.ui);
             outpix->data.rgba.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = outpix->data.rgba.us.G = outpix->data.rgba.us.B = gan_pixel_ui_to_us(inpix->data.gl.ui);
             outpix->data.rgba.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = outpix->data.rgba.ui.G = outpix->data.rgba.ui.B = inpix->data.gl.ui;
             outpix->data.rgba.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = outpix->data.rgba.d.G = outpix->data.rgba.d.B = gan_pixel_ui_to_d(inpix->data.gl.ui);
             outpix->data.rgba.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = outpix->data.rgba.f.G = outpix->data.rgba.f.B = gan_pixel_ui_to_f(inpix->data.gl.ui);
             outpix->data.rgba.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_gl_rgba", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_gl_bgr ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_GREY_LEVEL_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_gl_bgr", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = outpix->data.bgr.uc.G = outpix->data.bgr.uc.B = gan_pixel_d_to_uc(inpix->data.gl.d);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = outpix->data.bgr.us.G = outpix->data.bgr.us.B = gan_pixel_d_to_us(inpix->data.gl.d);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = outpix->data.bgr.ui.G = outpix->data.bgr.ui.B = gan_pixel_d_to_ui(inpix->data.gl.d);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = outpix->data.bgr.f.G = outpix->data.bgr.f.B = (float)inpix->data.gl.d;
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = outpix->data.bgr.d.G = outpix->data.bgr.d.B = inpix->data.gl.d;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = outpix->data.bgr.uc.G = outpix->data.bgr.uc.B = gan_pixel_f_to_uc(inpix->data.gl.f);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = outpix->data.bgr.us.G = outpix->data.bgr.us.B = gan_pixel_f_to_us(inpix->data.gl.f);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = outpix->data.bgr.ui.G = outpix->data.bgr.ui.B = gan_pixel_f_to_ui(inpix->data.gl.f);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = outpix->data.bgr.f.G = outpix->data.bgr.f.B = inpix->data.gl.f;
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = outpix->data.bgr.d.G = outpix->data.bgr.d.B = (double)inpix->data.gl.f;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = outpix->data.bgr.uc.G = outpix->data.bgr.uc.B = inpix->data.gl.uc;
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = outpix->data.bgr.us.G = outpix->data.bgr.us.B = gan_pixel_uc_to_us(inpix->data.gl.uc);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = outpix->data.bgr.ui.G = outpix->data.bgr.ui.B = gan_pixel_uc_to_ui(inpix->data.gl.uc);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = outpix->data.bgr.f.G = outpix->data.bgr.f.B = gan_pixel_uc_to_f(inpix->data.gl.uc);
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = outpix->data.bgr.d.G = outpix->data.bgr.d.B = gan_pixel_uc_to_d(inpix->data.gl.uc);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = outpix->data.bgr.uc.G = outpix->data.bgr.uc.B = gan_pixel_us_to_uc(inpix->data.gl.us);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = outpix->data.bgr.us.G = outpix->data.bgr.us.B = inpix->data.gl.us;
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = outpix->data.bgr.ui.G = outpix->data.bgr.ui.B = gan_pixel_us_to_ui(inpix->data.gl.us);
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = outpix->data.bgr.d.G = outpix->data.bgr.d.B = gan_pixel_us_to_d(inpix->data.gl.us);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = outpix->data.bgr.f.G = outpix->data.bgr.f.B = gan_pixel_us_to_f(inpix->data.gl.us);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = outpix->data.bgr.uc.G = outpix->data.bgr.uc.B = gan_pixel_ui_to_uc(inpix->data.gl.ui);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = outpix->data.bgr.us.G = outpix->data.bgr.us.B = gan_pixel_ui_to_us(inpix->data.gl.ui);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = outpix->data.bgr.ui.G = outpix->data.bgr.ui.B = inpix->data.gl.ui;
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = outpix->data.bgr.d.G = outpix->data.bgr.d.B = gan_pixel_ui_to_d(inpix->data.gl.ui);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = outpix->data.bgr.f.G = outpix->data.bgr.f.B = gan_pixel_ui_to_f(inpix->data.gl.ui);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_gl_bgr", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_gl_bgra ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_GREY_LEVEL_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_gl_bgra", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif
   
   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = outpix->data.bgra.uc.G = outpix->data.bgra.uc.B = gan_pixel_d_to_uc(inpix->data.gl.d);
             outpix->data.bgra.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = outpix->data.bgra.us.G = outpix->data.bgra.us.B = gan_pixel_d_to_us(inpix->data.gl.d);
             outpix->data.bgra.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = outpix->data.bgra.ui.G = outpix->data.bgra.ui.B = gan_pixel_d_to_ui(inpix->data.gl.d);
             outpix->data.bgra.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = outpix->data.bgra.f.G = outpix->data.bgra.f.B = (float)inpix->data.gl.d;
             outpix->data.bgra.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = outpix->data.bgra.d.G = outpix->data.bgra.d.B = inpix->data.gl.d;
             outpix->data.bgra.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = outpix->data.bgra.uc.G = outpix->data.bgra.uc.B = gan_pixel_f_to_uc(inpix->data.gl.f);
             outpix->data.bgra.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = outpix->data.bgra.us.G = outpix->data.bgra.us.B = gan_pixel_f_to_us(inpix->data.gl.f);
             outpix->data.bgra.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = outpix->data.bgra.ui.G = outpix->data.bgra.ui.B = gan_pixel_f_to_ui(inpix->data.gl.f);
             outpix->data.bgra.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = outpix->data.bgra.f.G = outpix->data.bgra.f.B = inpix->data.gl.f;
             outpix->data.bgra.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = outpix->data.bgra.d.G = outpix->data.bgra.d.B = (double)inpix->data.gl.f;
             outpix->data.bgra.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = outpix->data.bgra.uc.G = outpix->data.bgra.uc.B = inpix->data.gl.uc;
             outpix->data.bgra.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = outpix->data.bgra.us.G = outpix->data.bgra.us.B = gan_pixel_uc_to_us(inpix->data.gl.uc);
             outpix->data.bgra.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = outpix->data.bgra.ui.G = outpix->data.bgra.ui.B = gan_pixel_uc_to_ui(inpix->data.gl.uc);
             outpix->data.bgra.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = outpix->data.bgra.d.G = outpix->data.bgra.d.B = gan_pixel_uc_to_d(inpix->data.gl.uc);
             outpix->data.bgra.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = outpix->data.bgra.f.G = outpix->data.bgra.f.B = gan_pixel_uc_to_f(inpix->data.gl.uc);
             outpix->data.bgra.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = outpix->data.bgra.uc.G = outpix->data.bgra.uc.B = gan_pixel_us_to_uc(inpix->data.gl.us);
             outpix->data.bgra.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = outpix->data.bgra.us.G = outpix->data.bgra.us.B = inpix->data.gl.us;
             outpix->data.bgra.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = outpix->data.bgra.ui.G = outpix->data.bgra.ui.B = gan_pixel_us_to_ui(inpix->data.gl.us);
             outpix->data.bgra.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = outpix->data.bgra.d.G = outpix->data.bgra.d.B = gan_pixel_us_to_d(inpix->data.gl.us);
             outpix->data.bgra.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = outpix->data.bgra.f.G = outpix->data.bgra.f.B = gan_pixel_us_to_f(inpix->data.gl.us);
             outpix->data.bgra.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = outpix->data.bgra.uc.G = outpix->data.bgra.uc.B = gan_pixel_ui_to_uc(inpix->data.gl.ui);
             outpix->data.bgra.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = outpix->data.bgra.us.G = outpix->data.bgra.us.B = gan_pixel_ui_to_us(inpix->data.gl.ui);
             outpix->data.bgra.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = outpix->data.bgra.ui.G = outpix->data.bgra.ui.B = inpix->data.gl.ui;
             outpix->data.bgra.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = outpix->data.bgra.d.G = outpix->data.bgra.d.B = gan_pixel_ui_to_d(inpix->data.gl.ui);
             outpix->data.bgra.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = outpix->data.bgra.f.G = outpix->data.bgra.f.B = gan_pixel_ui_to_f(inpix->data.gl.ui);
             outpix->data.bgra.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gl_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_gl_bgra", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/* convert grey level with alpha channel to grey level */
static Gan_Bool
 convert_pixel_gla_gl ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_GREY_LEVEL_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_gla_gl", GAN_ERROR_ILLEGAL_ARGUMENT,
                            "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_d_to_uc(inpix->data.gla.d.I);
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_d_to_us(inpix->data.gla.d.I);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_d_to_ui(inpix->data.gla.d.I);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = (float)inpix->data.gla.d.I;
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = inpix->data.gla.d.I;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_gl",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_f_to_uc(inpix->data.gla.f.I);
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_f_to_us(inpix->data.gla.f.I);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_f_to_ui(inpix->data.gla.f.I);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = inpix->data.gla.f.I;
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = (double)inpix->data.gla.f.I;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_gl",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = inpix->data.gla.uc.I;
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_uc_to_us(inpix->data.gla.uc.I);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_uc_to_ui(inpix->data.gla.uc.I);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_uc_to_d(inpix->data.gla.uc.I);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_uc_to_f(inpix->data.gla.uc.I);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_gl",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_us_to_uc(inpix->data.gla.us.I);
             break;

           case GAN_USHORT:
             outpix->data.gl.us = inpix->data.gla.us.I;
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_us_to_ui(inpix->data.gla.us.I);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_us_to_d(inpix->data.gla.us.I);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_us_to_f(inpix->data.gla.us.I);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_gl",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_ui_to_uc(inpix->data.gla.ui.I);
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_ui_to_us(inpix->data.gla.ui.I);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = inpix->data.gla.ui.I;
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_ui_to_d(inpix->data.gla.ui.I);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_ui_to_f(inpix->data.gla.ui.I);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_gl",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_gla_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_gla_gla ( Gan_Pixel *inpix, Gan_Pixel *outpix,
                         int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_GREY_LEVEL_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_gla_gla",
                            GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_d_to_uc(inpix->data.gla.d.I);
             outpix->data.gla.uc.A = gan_pixel_d_to_uc(inpix->data.gla.d.A);
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_d_to_us(inpix->data.gla.d.I);
             outpix->data.gla.us.A = gan_pixel_d_to_us(inpix->data.gla.d.A);
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_d_to_ui(inpix->data.gla.d.I);
             outpix->data.gla.ui.A = gan_pixel_d_to_ui(inpix->data.gla.d.A);
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = (float)inpix->data.gla.d.I;
             outpix->data.gla.f.A = (float)inpix->data.gla.d.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_gla",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_f_to_uc(inpix->data.gla.f.I);
             outpix->data.gla.uc.A = gan_pixel_f_to_uc(inpix->data.gla.f.A);
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_f_to_us(inpix->data.gla.f.I);
             outpix->data.gla.us.A = gan_pixel_f_to_us(inpix->data.gla.f.A);
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_f_to_ui(inpix->data.gla.f.I);
             outpix->data.gla.ui.A = gan_pixel_f_to_ui(inpix->data.gla.f.A);
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = (double)inpix->data.gla.f.I;
             outpix->data.gla.d.A = (double)inpix->data.gla.f.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_gla",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_uc_to_us(inpix->data.gla.uc.I);
             outpix->data.gla.us.A = gan_pixel_uc_to_us(inpix->data.gla.uc.A);
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_uc_to_ui(inpix->data.gla.uc.I);
             outpix->data.gla.ui.A = gan_pixel_uc_to_ui(inpix->data.gla.uc.A);
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_uc_to_f(inpix->data.gla.uc.I);
             outpix->data.gla.f.A = gan_pixel_uc_to_f(inpix->data.gla.uc.A);
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_uc_to_d(inpix->data.gla.uc.I);
             outpix->data.gla.d.A = gan_pixel_uc_to_d(inpix->data.gla.uc.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_gla",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_us_to_uc(inpix->data.gla.us.I);
             outpix->data.gla.uc.A = gan_pixel_us_to_uc(inpix->data.gla.us.A);
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_us_to_ui(inpix->data.gla.us.I);
             outpix->data.gla.ui.A = gan_pixel_us_to_ui(inpix->data.gla.us.A);
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_us_to_d(inpix->data.gla.us.I);
             outpix->data.gla.d.A = gan_pixel_us_to_d(inpix->data.gla.us.A);
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_us_to_f(inpix->data.gla.us.I);
             outpix->data.gla.f.A = gan_pixel_us_to_f(inpix->data.gla.us.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_gla",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_ui_to_uc(inpix->data.gla.ui.I);
             outpix->data.gla.uc.A = gan_pixel_ui_to_uc(inpix->data.gla.ui.A);
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_ui_to_us(inpix->data.gla.ui.I);
             outpix->data.gla.us.A = gan_pixel_ui_to_us(inpix->data.gla.ui.A);
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_ui_to_f(inpix->data.gla.ui.I);
             outpix->data.gla.f.A = gan_pixel_ui_to_f(inpix->data.gla.ui.A);
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_ui_to_d(inpix->data.gla.ui.I);
             outpix->data.gla.d.A = gan_pixel_ui_to_d(inpix->data.gla.ui.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_gla",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      default:
        if ( error_code == NULL )
        {
           gan_err_flush_trace();
           gan_err_register ( "convert_pixel_gla_gla",
                              GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        }
        else
           *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_gla_rgb ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_GREY_LEVEL_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_gla_rgb", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = outpix->data.rgb.uc.G = outpix->data.rgb.uc.B = gan_pixel_d_to_uc(inpix->data.gla.d.I);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = outpix->data.rgb.us.G = outpix->data.rgb.us.B = gan_pixel_d_to_us(inpix->data.gla.d.I);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = outpix->data.rgb.ui.G = outpix->data.rgb.ui.B = gan_pixel_d_to_ui(inpix->data.gla.d.I);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = outpix->data.rgb.f.G = outpix->data.rgb.f.B = (float)inpix->data.gla.d.I;
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = outpix->data.rgb.d.G = outpix->data.rgb.d.B = inpix->data.gla.d.I;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = outpix->data.rgb.uc.G = outpix->data.rgb.uc.B = gan_pixel_f_to_uc(inpix->data.gla.f.I);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = outpix->data.rgb.us.G = outpix->data.rgb.us.B = gan_pixel_f_to_us(inpix->data.gla.f.I);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = outpix->data.rgb.ui.G = outpix->data.rgb.ui.B = gan_pixel_f_to_ui(inpix->data.gla.f.I);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = outpix->data.rgb.f.G = outpix->data.rgb.f.B = inpix->data.gla.f.I;
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = outpix->data.rgb.d.G = outpix->data.rgb.d.B = (double)inpix->data.gla.f.I;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = outpix->data.rgb.uc.G = outpix->data.rgb.uc.B = inpix->data.gla.uc.I;
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = outpix->data.rgb.us.G = outpix->data.rgb.us.B = gan_pixel_uc_to_us(inpix->data.gla.uc.I);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = outpix->data.rgb.ui.G = outpix->data.rgb.ui.B = gan_pixel_uc_to_ui(inpix->data.gla.uc.I);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = outpix->data.rgb.f.G = outpix->data.rgb.f.B = gan_pixel_uc_to_f(inpix->data.gla.uc.I);
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = outpix->data.rgb.d.G = outpix->data.rgb.d.B = gan_pixel_uc_to_d(inpix->data.gla.uc.I);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = outpix->data.rgb.uc.G = outpix->data.rgb.uc.B = gan_pixel_us_to_uc(inpix->data.gla.us.I);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = outpix->data.rgb.us.G = outpix->data.rgb.us.B = inpix->data.gla.us.I;
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = outpix->data.rgb.ui.G = outpix->data.rgb.ui.B = gan_pixel_us_to_ui(inpix->data.gla.us.I);
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = outpix->data.rgb.d.G = outpix->data.rgb.d.B = gan_pixel_us_to_d(inpix->data.gla.us.I);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = outpix->data.rgb.f.G = outpix->data.rgb.f.B = gan_pixel_us_to_f(inpix->data.gla.us.I);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = outpix->data.rgb.uc.G = outpix->data.rgb.uc.B = gan_pixel_ui_to_uc(inpix->data.gla.ui.I);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = outpix->data.rgb.us.G = outpix->data.rgb.us.B = gan_pixel_ui_to_us(inpix->data.gla.ui.I);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = outpix->data.rgb.ui.G = outpix->data.rgb.ui.B = inpix->data.gla.ui.I;
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = outpix->data.rgb.d.G = outpix->data.rgb.d.B = gan_pixel_ui_to_d(inpix->data.gla.ui.I);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = outpix->data.rgb.f.G = outpix->data.rgb.f.B = gan_pixel_ui_to_f(inpix->data.gla.ui.I);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_gla_rgb", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_gla_rgba ( Gan_Pixel *inpix, Gan_Pixel *outpix,
                           int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_GREY_LEVEL_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_gla_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = outpix->data.rgba.uc.G = 
             outpix->data.rgba.uc.B = gan_pixel_d_to_uc(inpix->data.gla.d.I);
             outpix->data.rgba.uc.A = gan_pixel_d_to_uc(inpix->data.gla.d.A);
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = outpix->data.rgba.us.G =
             outpix->data.rgba.us.B = gan_pixel_d_to_us(inpix->data.gla.d.I);
             outpix->data.rgba.us.A = gan_pixel_d_to_us(inpix->data.gla.d.A);
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = outpix->data.rgba.ui.G = 
             outpix->data.rgba.ui.B = gan_pixel_d_to_ui(inpix->data.gla.d.I);
             outpix->data.rgba.ui.A = gan_pixel_d_to_ui(inpix->data.gla.d.A);
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = outpix->data.rgba.f.G = 
             outpix->data.rgba.f.B = (float)inpix->data.gla.d.I;
             outpix->data.rgba.f.A = (float)inpix->data.gla.d.A;
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = outpix->data.rgba.d.G = 
             outpix->data.rgba.d.B = inpix->data.gla.d.I;
             outpix->data.rgba.d.A = inpix->data.gla.d.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = outpix->data.rgba.uc.G = 
             outpix->data.rgba.uc.B = gan_pixel_f_to_uc(inpix->data.gla.f.I);
             outpix->data.rgba.uc.A = gan_pixel_f_to_uc(inpix->data.gla.f.A);
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = outpix->data.rgba.us.G =
             outpix->data.rgba.us.B = gan_pixel_f_to_us(inpix->data.gla.f.I);
             outpix->data.rgba.us.A = gan_pixel_f_to_us(inpix->data.gla.f.A);
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = outpix->data.rgba.ui.G = 
             outpix->data.rgba.ui.B = gan_pixel_f_to_ui(inpix->data.gla.f.I);
             outpix->data.rgba.ui.A = gan_pixel_f_to_ui(inpix->data.gla.f.A);
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = outpix->data.rgba.f.G =
             outpix->data.rgba.f.B = inpix->data.gla.f.I;
             outpix->data.rgba.f.A = inpix->data.gla.f.A;
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = outpix->data.rgba.d.G =
             outpix->data.rgba.d.B = (double)inpix->data.gla.f.I;
             outpix->data.rgba.d.A = (double)inpix->data.gla.f.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = outpix->data.rgba.uc.G =
             outpix->data.rgba.uc.B = inpix->data.gla.uc.I;
             outpix->data.rgba.uc.A = inpix->data.gla.uc.A;
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = outpix->data.rgba.us.G =
             outpix->data.rgba.us.B = gan_pixel_uc_to_us(inpix->data.gla.uc.I);
             outpix->data.rgba.us.A = gan_pixel_uc_to_us(inpix->data.gla.uc.A);
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = outpix->data.rgba.ui.G =
             outpix->data.rgba.ui.B = gan_pixel_uc_to_ui(inpix->data.gla.uc.I);
             outpix->data.rgba.ui.A = gan_pixel_uc_to_ui(inpix->data.gla.uc.A);
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = outpix->data.rgba.f.G =
             outpix->data.rgba.f.B = gan_pixel_uc_to_f(inpix->data.gla.uc.I);
             outpix->data.rgba.f.A = gan_pixel_uc_to_f(inpix->data.gla.uc.A);
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = outpix->data.rgba.d.G =
             outpix->data.rgba.d.B = gan_pixel_uc_to_d(inpix->data.gla.uc.I);
             outpix->data.rgba.d.A = gan_pixel_uc_to_d(inpix->data.gla.uc.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = outpix->data.rgba.uc.G =
             outpix->data.rgba.uc.B = gan_pixel_us_to_uc(inpix->data.gla.us.I);
             outpix->data.rgba.uc.A = gan_pixel_us_to_uc(inpix->data.gla.us.A);
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = outpix->data.rgba.us.G =
             outpix->data.rgba.us.B = inpix->data.gla.us.I;
             outpix->data.rgba.us.A = inpix->data.gla.us.A;
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = outpix->data.rgba.ui.G =
             outpix->data.rgba.ui.B = gan_pixel_us_to_ui(inpix->data.gla.us.I);
             outpix->data.rgba.ui.A =gan_pixel_us_to_ui(inpix->data.gla.us.A);
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = outpix->data.rgba.d.G =
             outpix->data.rgba.d.B = gan_pixel_us_to_d(inpix->data.gla.us.I);
             outpix->data.rgba.d.A = gan_pixel_us_to_d(inpix->data.gla.us.A);
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = outpix->data.rgba.f.G =
             outpix->data.rgba.f.B = gan_pixel_us_to_f(inpix->data.gla.us.I);
             outpix->data.rgba.f.A = gan_pixel_us_to_f(inpix->data.gla.us.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = outpix->data.rgba.uc.G =
             outpix->data.rgba.uc.B = gan_pixel_ui_to_uc(inpix->data.gla.ui.I);
             outpix->data.rgba.uc.A = gan_pixel_ui_to_uc(inpix->data.gla.ui.A);
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = outpix->data.rgba.us.G =
             outpix->data.rgba.us.B = gan_pixel_ui_to_us(inpix->data.gla.ui.I);
             outpix->data.rgba.us.A = gan_pixel_ui_to_us(inpix->data.gla.ui.A);
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = outpix->data.rgba.ui.G =
             outpix->data.rgba.ui.B = inpix->data.gla.ui.I;
             outpix->data.rgba.ui.A = inpix->data.gla.ui.A;
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = outpix->data.rgba.f.G =
             outpix->data.rgba.f.B = gan_pixel_ui_to_f(inpix->data.gla.ui.I);
             outpix->data.rgba.f.A = gan_pixel_ui_to_f(inpix->data.gla.ui.A);
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = outpix->data.rgba.d.G =
             outpix->data.rgba.d.B = gan_pixel_ui_to_d(inpix->data.gla.ui.I);
             outpix->data.rgba.d.A = gan_pixel_ui_to_d(inpix->data.gla.ui.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      default:
        if ( error_code == NULL )
        {
           gan_err_flush_trace();
           gan_err_register ( "convert_pixel_rgba_rgba", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        }
        else
           *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/* convert from RGB to grey-level */
static Gan_Bool
 convert_pixel_rgb_gl ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_RGB_COLOUR_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_rgb_gl", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_rgbuc_to_yuc(&inpix->data.rgb.uc);
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_rgbuc_to_yus(&inpix->data.rgb.uc);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_rgbuc_to_yui(&inpix->data.rgb.uc);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_us_to_f(gan_pixel_rgbuc_to_yus(&inpix->data.rgb.uc) );
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_us_to_d(gan_pixel_rgbuc_to_yus(&inpix->data.rgb.uc) );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_rgbus_to_yuc(&inpix->data.rgb.us);
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_rgbus_to_yus(&inpix->data.rgb.us);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_us_to_ui(gan_pixel_rgbus_to_yus(&inpix->data.rgb.us));
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_us_to_f(gan_pixel_rgbus_to_yus(&inpix->data.rgb.us) );
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_us_to_d(gan_pixel_rgbus_to_yus(&inpix->data.rgb.us) );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_f_to_uc(gan_pixel_rgbui_to_yf(&inpix->data.rgb.ui) );
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_f_to_us(gan_pixel_rgbui_to_yf(&inpix->data.rgb.ui) );
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_f_to_ui(gan_pixel_rgbui_to_yf(&inpix->data.rgb.ui) );
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_rgbui_to_yf(&inpix->data.rgb.ui);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = (double)gan_pixel_rgbui_to_yf(&inpix->data.rgb.ui);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_f_to_uc(gan_pixel_rgbf_to_yf(&inpix->data.rgb.f) );
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_f_to_us(gan_pixel_rgbf_to_yf(&inpix->data.rgb.f) );
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_f_to_ui(gan_pixel_rgbf_to_yf(&inpix->data.rgb.f) );
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_rgbf_to_yf(&inpix->data.rgb.f);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = (double)gan_pixel_rgbf_to_yf(&inpix->data.rgb.f);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_d_to_uc(gan_pixel_rgbd_to_yd(&inpix->data.rgb.d) );
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_d_to_us(gan_pixel_rgbd_to_yd(&inpix->data.rgb.d) );
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_d_to_ui(gan_pixel_rgbd_to_yd(&inpix->data.rgb.d) );
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = (float)
                                 gan_pixel_rgbd_to_yd(&inpix->data.rgb.d);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_rgbd_to_yd(&inpix->data.rgb.d);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_rgb_gl", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_rgb_gla ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_RGB_COLOUR_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_rgb_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_rgbuc_to_yuc(&inpix->data.rgb.uc);
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_rgbuc_to_yus(&inpix->data.rgb.uc);
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_rgbuc_to_yui(&inpix->data.rgb.uc);
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_us_to_f(gan_pixel_rgbuc_to_yus(&inpix->data.rgb.uc) );
             outpix->data.gla.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_us_to_d(gan_pixel_rgbuc_to_yus(&inpix->data.rgb.uc) );
             outpix->data.gla.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_rgbus_to_yuc(&inpix->data.rgb.us);
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_rgbus_to_yus(&inpix->data.rgb.us);
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_us_to_ui(gan_pixel_rgbus_to_yus(&inpix->data.rgb.us));
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_us_to_f(gan_pixel_rgbus_to_yus(&inpix->data.rgb.us) );
             outpix->data.gla.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_us_to_d(gan_pixel_rgbus_to_yus(&inpix->data.rgb.us) );
             outpix->data.gla.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_f_to_uc(gan_pixel_rgbui_to_yf(&inpix->data.rgb.ui) );
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_f_to_us(gan_pixel_rgbui_to_yf(&inpix->data.rgb.ui) );
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_f_to_ui(gan_pixel_rgbui_to_yf(&inpix->data.rgb.ui) );
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_rgbui_to_yf(&inpix->data.rgb.ui);
             outpix->data.gla.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = (double)gan_pixel_rgbui_to_yf(&inpix->data.rgb.ui);
             outpix->data.gla.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_f_to_uc(gan_pixel_rgbf_to_yf(&inpix->data.rgb.f) );
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_f_to_us(gan_pixel_rgbf_to_yf(&inpix->data.rgb.f) );
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_f_to_ui(gan_pixel_rgbf_to_yf(&inpix->data.rgb.f) );
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_rgbf_to_yf(&inpix->data.rgb.f);
             outpix->data.gla.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = (double)gan_pixel_rgbf_to_yf(&inpix->data.rgb.f);
             outpix->data.gla.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_d_to_uc(gan_pixel_rgbd_to_yd(&inpix->data.rgb.d) );
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_d_to_us(gan_pixel_rgbd_to_yd(&inpix->data.rgb.d) );
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_d_to_ui(gan_pixel_rgbd_to_yd(&inpix->data.rgb.d) );
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = (float)gan_pixel_rgbd_to_yd(&inpix->data.rgb.d);
             outpix->data.gla.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_rgbd_to_yd(&inpix->data.rgb.d);
             outpix->data.gla.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_rgb_gla", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_rgb_rgb ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_RGB_COLOUR_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_rgb_rgb", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = gan_pixel_d_to_uc(inpix->data.rgb.d.R);
             outpix->data.rgb.uc.G = gan_pixel_d_to_uc(inpix->data.rgb.d.G);
             outpix->data.rgb.uc.B = gan_pixel_d_to_uc(inpix->data.rgb.d.B);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = gan_pixel_d_to_us(inpix->data.rgb.d.R);
             outpix->data.rgb.us.G = gan_pixel_d_to_us(inpix->data.rgb.d.G);
             outpix->data.rgb.us.B = gan_pixel_d_to_us(inpix->data.rgb.d.B);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = gan_pixel_d_to_ui(inpix->data.rgb.d.R);
             outpix->data.rgb.ui.G = gan_pixel_d_to_ui(inpix->data.rgb.d.G);
             outpix->data.rgb.ui.B = gan_pixel_d_to_ui(inpix->data.rgb.d.B);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = (float)inpix->data.rgb.d.R;
             outpix->data.rgb.f.G = (float)inpix->data.rgb.d.G;
             outpix->data.rgb.f.B = (float)inpix->data.rgb.d.B;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = gan_pixel_f_to_uc(inpix->data.rgb.f.R);
             outpix->data.rgb.uc.G = gan_pixel_f_to_uc(inpix->data.rgb.f.G);
             outpix->data.rgb.uc.B = gan_pixel_f_to_uc(inpix->data.rgb.f.B);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = gan_pixel_f_to_us(inpix->data.rgb.f.R);
             outpix->data.rgb.us.G = gan_pixel_f_to_us(inpix->data.rgb.f.G);
             outpix->data.rgb.us.B = gan_pixel_f_to_us(inpix->data.rgb.f.B);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = gan_pixel_f_to_ui(inpix->data.rgb.f.R);
             outpix->data.rgb.ui.G = gan_pixel_f_to_ui(inpix->data.rgb.f.G);
             outpix->data.rgb.ui.B = gan_pixel_f_to_ui(inpix->data.rgb.f.B);
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = (double)inpix->data.rgb.f.R;
             outpix->data.rgb.d.G = (double)inpix->data.rgb.f.G;
             outpix->data.rgb.d.B = (double)inpix->data.rgb.f.B;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_USHORT:
             outpix->data.rgb.us.R = gan_pixel_uc_to_us(inpix->data.rgb.uc.R);
             outpix->data.rgb.us.G = gan_pixel_uc_to_us(inpix->data.rgb.uc.G);
             outpix->data.rgb.us.B = gan_pixel_uc_to_us(inpix->data.rgb.uc.B);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = gan_pixel_uc_to_ui(inpix->data.rgb.uc.R);
             outpix->data.rgb.ui.G = gan_pixel_uc_to_ui(inpix->data.rgb.uc.G);
             outpix->data.rgb.ui.B = gan_pixel_uc_to_ui(inpix->data.rgb.uc.B);
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = gan_pixel_uc_to_d(inpix->data.rgb.uc.R);
             outpix->data.rgb.d.G = gan_pixel_uc_to_d(inpix->data.rgb.uc.G);
             outpix->data.rgb.d.B = gan_pixel_uc_to_d(inpix->data.rgb.uc.B);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = gan_pixel_uc_to_f(inpix->data.rgb.uc.R);
             outpix->data.rgb.f.G = gan_pixel_uc_to_f(inpix->data.rgb.uc.G);
             outpix->data.rgb.f.B = gan_pixel_uc_to_f(inpix->data.rgb.uc.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = gan_pixel_us_to_uc(inpix->data.rgb.us.R);
             outpix->data.rgb.uc.G = gan_pixel_us_to_uc(inpix->data.rgb.us.G);
             outpix->data.rgb.uc.B = gan_pixel_us_to_uc(inpix->data.rgb.us.B);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = gan_pixel_us_to_ui(inpix->data.rgb.us.R);
             outpix->data.rgb.ui.G = gan_pixel_us_to_ui(inpix->data.rgb.us.G);
             outpix->data.rgb.ui.B = gan_pixel_us_to_ui(inpix->data.rgb.us.B);
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = gan_pixel_us_to_d(inpix->data.rgb.us.R);
             outpix->data.rgb.d.G = gan_pixel_us_to_d(inpix->data.rgb.us.G);
             outpix->data.rgb.d.B = gan_pixel_us_to_d(inpix->data.rgb.us.B);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = gan_pixel_us_to_f(inpix->data.rgb.us.R);
             outpix->data.rgb.f.G = gan_pixel_us_to_f(inpix->data.rgb.us.G);
             outpix->data.rgb.f.B = gan_pixel_us_to_f(inpix->data.rgb.us.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_INT:
        switch ( outpix->type )
        {
           case GAN_DOUBLE:
             outpix->data.rgb.d.R = gan_pixel_i_to_d(inpix->data.rgb.i.R);
             outpix->data.rgb.d.G = gan_pixel_i_to_d(inpix->data.rgb.i.G);
             outpix->data.rgb.d.B = gan_pixel_i_to_d(inpix->data.rgb.i.B);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = gan_pixel_i_to_f(inpix->data.rgb.i.R);
             outpix->data.rgb.f.G = gan_pixel_i_to_f(inpix->data.rgb.i.G);
             outpix->data.rgb.f.B = gan_pixel_i_to_f(inpix->data.rgb.i.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = gan_pixel_ui_to_uc(inpix->data.rgb.ui.R);
             outpix->data.rgb.uc.G = gan_pixel_ui_to_uc(inpix->data.rgb.ui.G);
             outpix->data.rgb.uc.B = gan_pixel_ui_to_uc(inpix->data.rgb.ui.B);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = gan_pixel_ui_to_us(inpix->data.rgb.ui.R);
             outpix->data.rgb.us.G = gan_pixel_ui_to_us(inpix->data.rgb.ui.G);
             outpix->data.rgb.us.B = gan_pixel_ui_to_us(inpix->data.rgb.ui.B);
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = gan_pixel_ui_to_d(inpix->data.rgb.ui.R);
             outpix->data.rgb.d.G = gan_pixel_ui_to_d(inpix->data.rgb.ui.G);
             outpix->data.rgb.d.B = gan_pixel_ui_to_d(inpix->data.rgb.ui.B);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = gan_pixel_ui_to_f(inpix->data.rgb.ui.R);
             outpix->data.rgb.f.G = gan_pixel_ui_to_f(inpix->data.rgb.ui.G);
             outpix->data.rgb.f.B = gan_pixel_ui_to_f(inpix->data.rgb.ui.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_rgb_rgb", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_rgb_rgba ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_RGB_COLOUR_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_rgb_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = gan_pixel_d_to_uc(inpix->data.rgb.d.R);
             outpix->data.rgba.uc.G = gan_pixel_d_to_uc(inpix->data.rgb.d.G);
             outpix->data.rgba.uc.B = gan_pixel_d_to_uc(inpix->data.rgb.d.B);
             outpix->data.rgba.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = gan_pixel_d_to_us(inpix->data.rgb.d.R);
             outpix->data.rgba.us.G = gan_pixel_d_to_us(inpix->data.rgb.d.G);
             outpix->data.rgba.us.B = gan_pixel_d_to_us(inpix->data.rgb.d.B);
             outpix->data.rgba.us.A= USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = gan_pixel_d_to_ui(inpix->data.rgb.d.R);
             outpix->data.rgba.ui.G = gan_pixel_d_to_ui(inpix->data.rgb.d.G);
             outpix->data.rgba.ui.B = gan_pixel_d_to_ui(inpix->data.rgb.d.B);
             outpix->data.rgba.ui.A= UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = (float)inpix->data.rgb.d.R;
             outpix->data.rgba.f.G = (float)inpix->data.rgb.d.G;
             outpix->data.rgba.f.B = (float)inpix->data.rgb.d.B;
             outpix->data.rgba.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = inpix->data.rgb.d.R;
             outpix->data.rgba.d.G = inpix->data.rgb.d.G;
             outpix->data.rgba.d.B = inpix->data.rgb.d.B;
             outpix->data.rgba.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = gan_pixel_f_to_uc(inpix->data.rgb.f.R);
             outpix->data.rgba.uc.G = gan_pixel_f_to_uc(inpix->data.rgb.f.G);
             outpix->data.rgba.uc.B = gan_pixel_f_to_uc(inpix->data.rgb.f.B);
             outpix->data.rgba.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = gan_pixel_f_to_us(inpix->data.rgb.f.R);
             outpix->data.rgba.us.G = gan_pixel_f_to_us(inpix->data.rgb.f.G);
             outpix->data.rgba.us.B = gan_pixel_f_to_us(inpix->data.rgb.f.B);
             outpix->data.rgba.us.A= USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = gan_pixel_f_to_ui(inpix->data.rgb.f.R);
             outpix->data.rgba.ui.G = gan_pixel_f_to_ui(inpix->data.rgb.f.G);
             outpix->data.rgba.ui.B = gan_pixel_f_to_ui(inpix->data.rgb.f.B);
             outpix->data.rgba.ui.A= UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = (double)inpix->data.rgb.f.R;
             outpix->data.rgba.d.G = (double)inpix->data.rgb.f.G;
             outpix->data.rgba.d.B = (double)inpix->data.rgb.f.B;
             outpix->data.rgba.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = inpix->data.rgb.f.R;
             outpix->data.rgba.f.G = inpix->data.rgb.f.G;
             outpix->data.rgba.f.B = inpix->data.rgb.f.B;
             outpix->data.rgba.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = inpix->data.rgb.uc.R;
             outpix->data.rgba.uc.G = inpix->data.rgb.uc.G;
             outpix->data.rgba.uc.B = inpix->data.rgb.uc.B;
             outpix->data.rgba.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = gan_pixel_uc_to_us(inpix->data.rgb.uc.R);
             outpix->data.rgba.us.G = gan_pixel_uc_to_us(inpix->data.rgb.uc.G);
             outpix->data.rgba.us.B = gan_pixel_uc_to_us(inpix->data.rgb.uc.B);
             outpix->data.rgba.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = gan_pixel_uc_to_ui(inpix->data.rgb.uc.R);
             outpix->data.rgba.ui.G = gan_pixel_uc_to_ui(inpix->data.rgb.uc.G);
             outpix->data.rgba.ui.B = gan_pixel_uc_to_ui(inpix->data.rgb.uc.B);
             outpix->data.rgba.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = gan_pixel_uc_to_d(inpix->data.rgb.uc.R);
             outpix->data.rgba.d.G = gan_pixel_uc_to_d(inpix->data.rgb.uc.G);
             outpix->data.rgba.d.B = gan_pixel_uc_to_d(inpix->data.rgb.uc.B);
             outpix->data.rgba.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = gan_pixel_uc_to_f(inpix->data.rgb.uc.R);
             outpix->data.rgba.f.G = gan_pixel_uc_to_f(inpix->data.rgb.uc.G);
             outpix->data.rgba.f.B = gan_pixel_uc_to_f(inpix->data.rgb.uc.B);
             outpix->data.rgba.f.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = gan_pixel_us_to_uc(inpix->data.rgb.us.R);
             outpix->data.rgba.uc.G = gan_pixel_us_to_uc(inpix->data.rgb.us.G);
             outpix->data.rgba.uc.B = gan_pixel_us_to_uc(inpix->data.rgb.us.B);
             outpix->data.rgba.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = inpix->data.rgb.us.R;
             outpix->data.rgba.us.G = inpix->data.rgb.us.G;
             outpix->data.rgba.us.B = inpix->data.rgb.us.B;
             outpix->data.rgba.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = gan_pixel_us_to_ui(inpix->data.rgb.us.R);
             outpix->data.rgba.ui.G = gan_pixel_us_to_ui(inpix->data.rgb.us.G);
             outpix->data.rgba.ui.B = gan_pixel_us_to_ui(inpix->data.rgb.us.B);
             outpix->data.rgba.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = gan_pixel_us_to_d(inpix->data.rgb.us.R);
             outpix->data.rgba.d.G = gan_pixel_us_to_d(inpix->data.rgb.us.G);
             outpix->data.rgba.d.B = gan_pixel_us_to_d(inpix->data.rgb.us.B);
             outpix->data.rgba.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = gan_pixel_us_to_f(inpix->data.rgb.us.R);
             outpix->data.rgba.f.G = gan_pixel_us_to_f(inpix->data.rgb.us.G);
             outpix->data.rgba.f.B = gan_pixel_us_to_f(inpix->data.rgb.us.B);
             outpix->data.rgba.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = gan_pixel_ui_to_uc(inpix->data.rgb.ui.R);
             outpix->data.rgba.uc.G = gan_pixel_ui_to_uc(inpix->data.rgb.ui.G);
             outpix->data.rgba.uc.B = gan_pixel_ui_to_uc(inpix->data.rgb.ui.B);
             outpix->data.rgba.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = gan_pixel_ui_to_us(inpix->data.rgb.ui.R);
             outpix->data.rgba.us.G = gan_pixel_ui_to_us(inpix->data.rgb.ui.G);
             outpix->data.rgba.us.B = gan_pixel_ui_to_us(inpix->data.rgb.ui.B);
             outpix->data.rgba.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = inpix->data.rgb.ui.R;
             outpix->data.rgba.ui.G = inpix->data.rgb.ui.G;
             outpix->data.rgba.ui.B = inpix->data.rgb.ui.B;
             outpix->data.rgba.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = gan_pixel_ui_to_d(inpix->data.rgb.ui.R);
             outpix->data.rgba.d.G = gan_pixel_ui_to_d(inpix->data.rgb.ui.G);
             outpix->data.rgba.d.B = gan_pixel_ui_to_d(inpix->data.rgb.ui.B);
             outpix->data.rgba.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = gan_pixel_ui_to_f(inpix->data.rgb.ui.R);
             outpix->data.rgba.f.G = gan_pixel_ui_to_f(inpix->data.rgb.ui.G);
             outpix->data.rgba.f.B = gan_pixel_ui_to_f(inpix->data.rgb.ui.B);
             outpix->data.rgba.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_rgb_rgba", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_rgb_vfield2D ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_RGB_COLOUR_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_rgb_vfield2D", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_FLOAT:
             outpix->data.vfield2D.f.x = inpix->data.rgb.f.R;
             outpix->data.vfield2D.f.y = inpix->data.rgb.f.G;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgb_vfield2D", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_rgb_vfield2D", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/* convert from RGBA to grey-level with */
static Gan_Bool
 convert_pixel_rgba_gl ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code)
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_RGB_COLOUR_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_rgba_gl",
                            GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc =gan_pixel_rgbauc_to_yuc(&inpix->data.rgba.uc);
             break;

           case GAN_USHORT:
             outpix->data.gl.us =gan_pixel_rgbauc_to_yus(&inpix->data.rgba.uc);
             break;

           case GAN_UINT:
             outpix->data.gl.ui =gan_pixel_rgbauc_to_yui(&inpix->data.rgba.uc);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_us_to_f(gan_pixel_rgbauc_to_yus(&inpix->data.rgba.uc) );
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_us_to_d(gan_pixel_rgbauc_to_yus(&inpix->data.rgba.uc) );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc =gan_pixel_rgbaus_to_yuc(&inpix->data.rgba.us);
             break;

           case GAN_USHORT:
             outpix->data.gl.us =gan_pixel_rgbaus_to_yus(&inpix->data.rgba.us);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_us_to_ui(gan_pixel_rgbaus_to_yus(&inpix->data.rgba.us));
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_us_to_f(gan_pixel_rgbaus_to_yus(&inpix->data.rgba.us) );
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_us_to_d(gan_pixel_rgbaus_to_yus(&inpix->data.rgba.us) );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_f_to_uc(gan_pixel_rgbaui_to_yf(&inpix->data.rgba.ui) );
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_f_to_us(gan_pixel_rgbaui_to_yf(&inpix->data.rgba.ui) );
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_f_to_ui(gan_pixel_rgbaui_to_yf(&inpix->data.rgba.ui) );
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_rgbaui_to_yf(&inpix->data.rgba.ui);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = (double)gan_pixel_rgbaui_to_yf(&inpix->data.rgba.ui);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_f_to_uc(gan_pixel_rgbaf_to_yf(&inpix->data.rgba.f) );
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_f_to_us(gan_pixel_rgbaf_to_yf(&inpix->data.rgba.f) );
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_f_to_ui(gan_pixel_rgbaf_to_yf(&inpix->data.rgba.f) );
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_rgbaf_to_yf(&inpix->data.rgba.f);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = (double)gan_pixel_rgbaf_to_yf(&inpix->data.rgba.f);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_d_to_uc(gan_pixel_rgbad_to_yd(&inpix->data.rgba.d) );
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_d_to_us(gan_pixel_rgbad_to_yd(&inpix->data.rgba.d) );
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_d_to_ui(gan_pixel_rgbad_to_yd(&inpix->data.rgba.d) );
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = (float)gan_pixel_rgbad_to_yd(&inpix->data.rgba.d);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_rgbad_to_yd(&inpix->data.rgba.d);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_rgba_gl", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/* convert from RGBA to grey-level with alpha channel */
static Gan_Bool
 convert_pixel_rgba_gla ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code)
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_RGB_COLOUR_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_rgba_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_rgbauc_to_yuc(&inpix->data.rgba.uc);
             outpix->data.gla.uc.A = inpix->data.rgba.uc.A;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_rgbauc_to_yus(&inpix->data.rgba.uc);
             outpix->data.gla.us.A = gan_pixel_uc_to_us(inpix->data.rgba.uc.A);
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_rgbauc_to_yui(&inpix->data.rgba.uc);
             outpix->data.gla.ui.A = gan_pixel_uc_to_ui(inpix->data.rgba.uc.A);
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_us_to_f (
                               gan_pixel_rgbauc_to_yus(&inpix->data.rgba.uc) );
             outpix->data.gla.f.A = gan_pixel_uc_to_f(inpix->data.rgba.uc.A);
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_us_to_d(gan_pixel_rgbauc_to_yus(&inpix->data.rgba.uc) );
             outpix->data.gla.d.A = gan_pixel_uc_to_d(inpix->data.rgba.uc.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_rgbaus_to_yuc(&inpix->data.rgba.us);
             outpix->data.gla.uc.A = gan_pixel_us_to_uc(inpix->data.rgba.us.A);
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_rgbaus_to_yus(&inpix->data.rgba.us);
             outpix->data.gla.us.A = inpix->data.rgba.us.A;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_us_to_ui(gan_pixel_rgbaus_to_yus(&inpix->data.rgba.us));
             outpix->data.gla.ui.A = gan_pixel_us_to_ui(inpix->data.rgba.us.A);
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_us_to_f(gan_pixel_rgbaus_to_yus(&inpix->data.rgba.us) );
             outpix->data.gla.f.A = gan_pixel_us_to_f(inpix->data.rgba.us.A);
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_us_to_d(gan_pixel_rgbaus_to_yus(&inpix->data.rgba.us) );
             outpix->data.gla.d.A = gan_pixel_us_to_d(inpix->data.rgba.us.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_f_to_uc(gan_pixel_rgbaui_to_yf(&inpix->data.rgba.ui) );
             outpix->data.gla.uc.A = gan_pixel_ui_to_uc(inpix->data.rgba.ui.A);
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_f_to_us(gan_pixel_rgbaui_to_yf(&inpix->data.rgba.ui) );
             outpix->data.gla.us.A = gan_pixel_ui_to_us(inpix->data.rgba.ui.A);
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_f_to_ui(gan_pixel_rgbaui_to_yf(&inpix->data.rgba.ui) );
             outpix->data.gla.ui.A = inpix->data.rgba.ui.A;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I =
                                  gan_pixel_rgbaui_to_yf(&inpix->data.rgba.ui);
             outpix->data.gla.f.A = gan_pixel_ui_to_f(inpix->data.rgba.ui.A);
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = (double)gan_pixel_rgbaui_to_yf(&inpix->data.rgba.ui);
             outpix->data.gla.d.A = gan_pixel_ui_to_d(inpix->data.rgba.ui.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_f_to_uc(gan_pixel_rgbaf_to_yf(&inpix->data.rgba.f) );
             outpix->data.gla.uc.A = gan_pixel_f_to_uc(inpix->data.rgba.f.A);
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_f_to_us(gan_pixel_rgbaf_to_yf(&inpix->data.rgba.f) );
             outpix->data.gla.us.A = gan_pixel_f_to_us(inpix->data.rgba.f.A);
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_f_to_ui(gan_pixel_rgbaf_to_yf(&inpix->data.rgba.f) );
             outpix->data.gla.ui.A = gan_pixel_f_to_ui(inpix->data.rgba.f.A);
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_rgbaf_to_yf(&inpix->data.rgba.f);
             outpix->data.gla.f.A = inpix->data.rgba.f.A;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = (double)gan_pixel_rgbaf_to_yf(&inpix->data.rgba.f);
             outpix->data.gla.d.A = (double)inpix->data.rgba.f.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_d_to_uc(gan_pixel_rgbad_to_yd(&inpix->data.rgba.d) );
             outpix->data.gla.uc.A = gan_pixel_d_to_uc(inpix->data.rgba.d.A);
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_d_to_us(gan_pixel_rgbad_to_yd(&inpix->data.rgba.d) );
             outpix->data.gla.us.A = gan_pixel_d_to_us(inpix->data.rgba.d.A);
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_d_to_ui(gan_pixel_rgbad_to_yd(&inpix->data.rgba.d) );
             outpix->data.gla.ui.A = gan_pixel_d_to_ui(inpix->data.rgba.d.A);
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = (float)gan_pixel_rgbad_to_yd(&inpix->data.rgba.d);
             outpix->data.gla.f.A = (float)inpix->data.rgba.d.A;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_rgbad_to_yd(&inpix->data.rgba.d);
             outpix->data.gla.d.A = inpix->data.rgba.d.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_rgba_gla", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_rgba_rgb ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_RGB_COLOUR_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_rgba_rgb", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = gan_pixel_d_to_uc(inpix->data.rgba.d.R);
             outpix->data.rgb.uc.G = gan_pixel_d_to_uc(inpix->data.rgba.d.G);
             outpix->data.rgb.uc.B = gan_pixel_d_to_uc(inpix->data.rgba.d.B);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = gan_pixel_d_to_us(inpix->data.rgba.d.R);
             outpix->data.rgb.us.G = gan_pixel_d_to_us(inpix->data.rgba.d.G);
             outpix->data.rgb.us.B = gan_pixel_d_to_us(inpix->data.rgba.d.B);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = gan_pixel_d_to_ui(inpix->data.rgba.d.R);
             outpix->data.rgb.ui.G = gan_pixel_d_to_ui(inpix->data.rgba.d.G);
             outpix->data.rgb.ui.B = gan_pixel_d_to_ui(inpix->data.rgba.d.B);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = (float)inpix->data.rgba.d.R;
             outpix->data.rgb.f.G = (float)inpix->data.rgba.d.G;
             outpix->data.rgb.f.B = (float)inpix->data.rgba.d.B;
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = inpix->data.rgba.d.R;
             outpix->data.rgb.d.G = inpix->data.rgba.d.G;
             outpix->data.rgb.d.B = inpix->data.rgba.d.B;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = gan_pixel_f_to_uc(inpix->data.rgba.f.R);
             outpix->data.rgb.uc.G = gan_pixel_f_to_uc(inpix->data.rgba.f.G);
             outpix->data.rgb.uc.B = gan_pixel_f_to_uc(inpix->data.rgba.f.B);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = gan_pixel_f_to_us(inpix->data.rgba.f.R);
             outpix->data.rgb.us.G = gan_pixel_f_to_us(inpix->data.rgba.f.G);
             outpix->data.rgb.us.B = gan_pixel_f_to_us(inpix->data.rgba.f.B);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = gan_pixel_f_to_ui(inpix->data.rgba.f.R);
             outpix->data.rgb.ui.G = gan_pixel_f_to_ui(inpix->data.rgba.f.G);
             outpix->data.rgb.ui.B = gan_pixel_f_to_ui(inpix->data.rgba.f.B);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = inpix->data.rgba.f.R;
             outpix->data.rgb.f.G = inpix->data.rgba.f.G;
             outpix->data.rgb.f.B = inpix->data.rgba.f.B;
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = (double)inpix->data.rgba.f.R;
             outpix->data.rgb.d.G = (double)inpix->data.rgba.f.G;
             outpix->data.rgb.d.B = (double)inpix->data.rgba.f.B;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = inpix->data.rgba.uc.R;
             outpix->data.rgb.uc.G = inpix->data.rgba.uc.G;
             outpix->data.rgb.uc.B = inpix->data.rgba.uc.B;
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = gan_pixel_uc_to_us(inpix->data.rgba.uc.R);
             outpix->data.rgb.us.G = gan_pixel_uc_to_us(inpix->data.rgba.uc.G);
             outpix->data.rgb.us.B = gan_pixel_uc_to_us(inpix->data.rgba.uc.B);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = gan_pixel_uc_to_ui(inpix->data.rgba.uc.R);
             outpix->data.rgb.ui.G = gan_pixel_uc_to_ui(inpix->data.rgba.uc.G);
             outpix->data.rgb.ui.B = gan_pixel_uc_to_ui(inpix->data.rgba.uc.B);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = gan_pixel_uc_to_f(inpix->data.rgba.uc.R);
             outpix->data.rgb.f.G = gan_pixel_uc_to_f(inpix->data.rgba.uc.G);
             outpix->data.rgb.f.B = gan_pixel_uc_to_f(inpix->data.rgba.uc.B);
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = gan_pixel_uc_to_d(inpix->data.rgba.uc.R);
             outpix->data.rgb.d.G = gan_pixel_uc_to_d(inpix->data.rgba.uc.G);
             outpix->data.rgb.d.B = gan_pixel_uc_to_d(inpix->data.rgba.uc.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = gan_pixel_us_to_uc(inpix->data.rgba.us.R);
             outpix->data.rgb.uc.G = gan_pixel_us_to_uc(inpix->data.rgba.us.G);
             outpix->data.rgb.uc.B = gan_pixel_us_to_uc(inpix->data.rgba.us.B);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = inpix->data.rgba.us.R;
             outpix->data.rgb.us.G = inpix->data.rgba.us.G;
             outpix->data.rgb.us.B = inpix->data.rgba.us.B;
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = gan_pixel_us_to_ui(inpix->data.rgba.us.R);
             outpix->data.rgb.ui.G = gan_pixel_us_to_ui(inpix->data.rgba.us.G);
             outpix->data.rgb.ui.B = gan_pixel_us_to_ui(inpix->data.rgba.us.B);
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = gan_pixel_us_to_f(inpix->data.rgba.us.R);
             outpix->data.rgb.f.G = gan_pixel_us_to_f(inpix->data.rgba.us.G);
             outpix->data.rgb.f.B = gan_pixel_us_to_f(inpix->data.rgba.us.B);
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = gan_pixel_us_to_d(inpix->data.rgba.us.R);
             outpix->data.rgb.d.G = gan_pixel_us_to_d(inpix->data.rgba.us.G);
             outpix->data.rgb.d.B = gan_pixel_us_to_d(inpix->data.rgba.us.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgb.uc.R = gan_pixel_ui_to_uc(inpix->data.rgba.ui.R);
             outpix->data.rgb.uc.G = gan_pixel_ui_to_uc(inpix->data.rgba.ui.G);
             outpix->data.rgb.uc.B = gan_pixel_ui_to_uc(inpix->data.rgba.ui.B);
             break;

           case GAN_USHORT:
             outpix->data.rgb.us.R = gan_pixel_ui_to_us(inpix->data.rgba.ui.R);
             outpix->data.rgb.us.G = gan_pixel_ui_to_us(inpix->data.rgba.ui.G);
             outpix->data.rgb.us.B = gan_pixel_ui_to_us(inpix->data.rgba.ui.B);
             break;

           case GAN_UINT:
             outpix->data.rgb.ui.R = inpix->data.rgba.ui.R;
             outpix->data.rgb.ui.G = inpix->data.rgba.ui.G;
             outpix->data.rgb.ui.B = inpix->data.rgba.ui.B;
             break;

           case GAN_FLOAT:
             outpix->data.rgb.f.R = gan_pixel_ui_to_f(inpix->data.rgba.ui.R);
             outpix->data.rgb.f.G = gan_pixel_ui_to_f(inpix->data.rgba.ui.G);
             outpix->data.rgb.f.B = gan_pixel_ui_to_f(inpix->data.rgba.ui.B);
             break;

           case GAN_DOUBLE:
             outpix->data.rgb.d.R = gan_pixel_ui_to_d(inpix->data.rgba.ui.R);
             outpix->data.rgb.d.G = gan_pixel_ui_to_d(inpix->data.rgba.ui.G);
             outpix->data.rgb.d.B = gan_pixel_ui_to_d(inpix->data.rgba.ui.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      default:
        if ( error_code == NULL )
        {
           gan_err_flush_trace();
           gan_err_register ( "convert_pixel_rgba_rgb", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        }
        else
           *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_rgba_rgba ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_RGB_COLOUR_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_rgba_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = gan_pixel_d_to_uc(inpix->data.rgba.d.R);
             outpix->data.rgba.uc.G = gan_pixel_d_to_uc(inpix->data.rgba.d.G);
             outpix->data.rgba.uc.B = gan_pixel_d_to_uc(inpix->data.rgba.d.B);
             outpix->data.rgba.uc.A = gan_pixel_d_to_uc(inpix->data.rgba.d.A);
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = gan_pixel_d_to_us(inpix->data.rgba.d.R);
             outpix->data.rgba.us.G = gan_pixel_d_to_us(inpix->data.rgba.d.G);
             outpix->data.rgba.us.B = gan_pixel_d_to_us(inpix->data.rgba.d.B);
             outpix->data.rgba.us.A = gan_pixel_d_to_us(inpix->data.rgba.d.A);
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = gan_pixel_d_to_ui(inpix->data.rgba.d.R);
             outpix->data.rgba.ui.G = gan_pixel_d_to_ui(inpix->data.rgba.d.G);
             outpix->data.rgba.ui.B = gan_pixel_d_to_ui(inpix->data.rgba.d.B);
             outpix->data.rgba.ui.A = gan_pixel_d_to_ui(inpix->data.rgba.d.A);
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = (float)inpix->data.rgba.d.R;
             outpix->data.rgba.f.G = (float)inpix->data.rgba.d.G;
             outpix->data.rgba.f.B = (float)inpix->data.rgba.d.B;
             outpix->data.rgba.f.A = (float)inpix->data.rgba.d.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R = gan_pixel_f_to_uc(inpix->data.rgba.f.R);
             outpix->data.rgba.uc.G = gan_pixel_f_to_uc(inpix->data.rgba.f.G);
             outpix->data.rgba.uc.B = gan_pixel_f_to_uc(inpix->data.rgba.f.B);
             outpix->data.rgba.uc.A = gan_pixel_f_to_uc(inpix->data.rgba.f.A);
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R = gan_pixel_f_to_us(inpix->data.rgba.f.R);
             outpix->data.rgba.us.G = gan_pixel_f_to_us(inpix->data.rgba.f.G);
             outpix->data.rgba.us.B = gan_pixel_f_to_us(inpix->data.rgba.f.B);
             outpix->data.rgba.us.A = gan_pixel_f_to_us(inpix->data.rgba.f.A);
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R = gan_pixel_f_to_ui(inpix->data.rgba.f.R);
             outpix->data.rgba.ui.G = gan_pixel_f_to_ui(inpix->data.rgba.f.G);
             outpix->data.rgba.ui.B = gan_pixel_f_to_ui(inpix->data.rgba.f.B);
             outpix->data.rgba.ui.A = gan_pixel_f_to_ui(inpix->data.rgba.f.A);
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = (double)inpix->data.rgba.f.R;
             outpix->data.rgba.d.G = (double)inpix->data.rgba.f.G;
             outpix->data.rgba.d.B = (double)inpix->data.rgba.f.B;
             outpix->data.rgba.d.A = (double)inpix->data.rgba.f.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_USHORT:
             outpix->data.rgba.us.R =gan_pixel_uc_to_us(inpix->data.rgba.uc.R);
             outpix->data.rgba.us.G =gan_pixel_uc_to_us(inpix->data.rgba.uc.G);
             outpix->data.rgba.us.B =gan_pixel_uc_to_us(inpix->data.rgba.uc.B);
             outpix->data.rgba.us.A =gan_pixel_uc_to_us(inpix->data.rgba.uc.A);
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R =gan_pixel_uc_to_ui(inpix->data.rgba.uc.R);
             outpix->data.rgba.ui.G =gan_pixel_uc_to_ui(inpix->data.rgba.uc.G);
             outpix->data.rgba.ui.B =gan_pixel_uc_to_ui(inpix->data.rgba.uc.B);
             outpix->data.rgba.ui.A =gan_pixel_uc_to_ui(inpix->data.rgba.uc.A);
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = gan_pixel_uc_to_f(inpix->data.rgba.uc.R);
             outpix->data.rgba.f.G = gan_pixel_uc_to_f(inpix->data.rgba.uc.G);
             outpix->data.rgba.f.B = gan_pixel_uc_to_f(inpix->data.rgba.uc.B);
             outpix->data.rgba.f.A = gan_pixel_uc_to_f(inpix->data.rgba.uc.A);
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = gan_pixel_uc_to_d(inpix->data.rgba.uc.R);
             outpix->data.rgba.d.G = gan_pixel_uc_to_d(inpix->data.rgba.uc.G);
             outpix->data.rgba.d.B = gan_pixel_uc_to_d(inpix->data.rgba.uc.B);
             outpix->data.rgba.d.A = gan_pixel_uc_to_d(inpix->data.rgba.uc.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R =gan_pixel_us_to_uc(inpix->data.rgba.us.R);
             outpix->data.rgba.uc.G =gan_pixel_us_to_uc(inpix->data.rgba.us.G);
             outpix->data.rgba.uc.B =gan_pixel_us_to_uc(inpix->data.rgba.us.B);
             outpix->data.rgba.uc.A =gan_pixel_us_to_uc(inpix->data.rgba.us.A);
             break;

           case GAN_UINT:
             outpix->data.rgba.ui.R =gan_pixel_us_to_ui(inpix->data.rgba.us.R);
             outpix->data.rgba.ui.G =gan_pixel_us_to_ui(inpix->data.rgba.us.G);
             outpix->data.rgba.ui.B =gan_pixel_us_to_ui(inpix->data.rgba.us.B);
             outpix->data.rgba.ui.A =gan_pixel_us_to_ui(inpix->data.rgba.us.A);
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = gan_pixel_us_to_d(inpix->data.rgba.us.R);
             outpix->data.rgba.d.G = gan_pixel_us_to_d(inpix->data.rgba.us.G);
             outpix->data.rgba.d.B = gan_pixel_us_to_d(inpix->data.rgba.us.B);
             outpix->data.rgba.d.A = gan_pixel_us_to_d(inpix->data.rgba.us.A);
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = gan_pixel_us_to_f(inpix->data.rgba.us.R);
             outpix->data.rgba.f.G = gan_pixel_us_to_f(inpix->data.rgba.us.G);
             outpix->data.rgba.f.B = gan_pixel_us_to_f(inpix->data.rgba.us.B);
             outpix->data.rgba.f.A = gan_pixel_us_to_f(inpix->data.rgba.us.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_INT:
        switch ( outpix->type )
        {
           case GAN_FLOAT:
             outpix->data.rgba.f.R = gan_pixel_i_to_f(inpix->data.rgba.i.R);
             outpix->data.rgba.f.G = gan_pixel_i_to_f(inpix->data.rgba.i.G);
             outpix->data.rgba.f.B = gan_pixel_i_to_f(inpix->data.rgba.i.B);
             outpix->data.rgba.f.A = gan_pixel_i_to_f(inpix->data.rgba.i.A);
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = gan_pixel_i_to_d(inpix->data.rgba.i.R);
             outpix->data.rgba.d.G = gan_pixel_i_to_d(inpix->data.rgba.i.G);
             outpix->data.rgba.d.B = gan_pixel_i_to_d(inpix->data.rgba.i.B);
             outpix->data.rgba.d.A = gan_pixel_i_to_d(inpix->data.rgba.i.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.rgba.uc.R =gan_pixel_ui_to_uc(inpix->data.rgba.ui.R);
             outpix->data.rgba.uc.G =gan_pixel_ui_to_uc(inpix->data.rgba.ui.G);
             outpix->data.rgba.uc.B =gan_pixel_ui_to_uc(inpix->data.rgba.ui.B);
             outpix->data.rgba.uc.A =gan_pixel_ui_to_uc(inpix->data.rgba.ui.A);
             break;

           case GAN_USHORT:
             outpix->data.rgba.us.R =gan_pixel_ui_to_us(inpix->data.rgba.ui.R);
             outpix->data.rgba.us.G =gan_pixel_ui_to_us(inpix->data.rgba.ui.G);
             outpix->data.rgba.us.B =gan_pixel_ui_to_us(inpix->data.rgba.ui.B);
             outpix->data.rgba.us.A =gan_pixel_ui_to_us(inpix->data.rgba.ui.A);
             break;

           case GAN_FLOAT:
             outpix->data.rgba.f.R = gan_pixel_ui_to_f(inpix->data.rgba.ui.R);
             outpix->data.rgba.f.G = gan_pixel_ui_to_f(inpix->data.rgba.ui.G);
             outpix->data.rgba.f.B = gan_pixel_ui_to_f(inpix->data.rgba.ui.B);
             outpix->data.rgba.f.A = gan_pixel_ui_to_f(inpix->data.rgba.ui.A);
             break;

           case GAN_DOUBLE:
             outpix->data.rgba.d.R = gan_pixel_ui_to_d(inpix->data.rgba.ui.R);
             outpix->data.rgba.d.G = gan_pixel_ui_to_d(inpix->data.rgba.ui.G);
             outpix->data.rgba.d.B = gan_pixel_ui_to_d(inpix->data.rgba.ui.B);
             outpix->data.rgba.d.A = gan_pixel_ui_to_d(inpix->data.rgba.ui.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_rgba_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      default:
        if ( error_code == NULL )
        {
           gan_err_flush_trace();
           gan_err_register ( "convert_pixel_rgba_rgba", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        }
        else
           *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_gla_bgr ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_GREY_LEVEL_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_gla_bgr", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = outpix->data.bgr.uc.G = outpix->data.bgr.uc.B = gan_pixel_d_to_uc(inpix->data.gla.d.I);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = outpix->data.bgr.us.G = outpix->data.bgr.us.B = gan_pixel_d_to_us(inpix->data.gla.d.I);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = outpix->data.bgr.ui.G = outpix->data.bgr.ui.B = gan_pixel_d_to_ui(inpix->data.gla.d.I);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = outpix->data.bgr.f.G = outpix->data.bgr.f.B = (float)inpix->data.gla.d.I;
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = outpix->data.bgr.d.G = outpix->data.bgr.d.B = inpix->data.gla.d.I;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = outpix->data.bgr.uc.G = outpix->data.bgr.uc.B = gan_pixel_f_to_uc(inpix->data.gla.f.I);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = outpix->data.bgr.us.G = outpix->data.bgr.us.B = gan_pixel_f_to_us(inpix->data.gla.f.I);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = outpix->data.bgr.ui.G = outpix->data.bgr.ui.B = gan_pixel_f_to_ui(inpix->data.gla.f.I);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = outpix->data.bgr.f.G = outpix->data.bgr.f.B = inpix->data.gla.f.I;
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = outpix->data.bgr.d.G = outpix->data.bgr.d.B = (double)inpix->data.gla.f.I;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = outpix->data.bgr.uc.G = outpix->data.bgr.uc.B = inpix->data.gla.uc.I;
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = outpix->data.bgr.us.G = outpix->data.bgr.us.B = gan_pixel_uc_to_us(inpix->data.gla.uc.I);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = outpix->data.bgr.ui.G = outpix->data.bgr.ui.B = gan_pixel_uc_to_ui(inpix->data.gla.uc.I);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = outpix->data.bgr.f.G = outpix->data.bgr.f.B = gan_pixel_uc_to_f(inpix->data.gla.uc.I);
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = outpix->data.bgr.d.G = outpix->data.bgr.d.B = gan_pixel_uc_to_d(inpix->data.gla.uc.I);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = outpix->data.bgr.uc.G = outpix->data.bgr.uc.B = gan_pixel_us_to_uc(inpix->data.gla.us.I);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = outpix->data.bgr.us.G = outpix->data.bgr.us.B = inpix->data.gla.us.I;
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = outpix->data.bgr.ui.G = outpix->data.bgr.ui.B = gan_pixel_us_to_ui(inpix->data.gla.us.I);
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = outpix->data.bgr.d.G = outpix->data.bgr.d.B = gan_pixel_us_to_d(inpix->data.gla.us.I);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = outpix->data.bgr.f.G = outpix->data.bgr.f.B = gan_pixel_us_to_f(inpix->data.gla.us.I);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = outpix->data.bgr.uc.G = outpix->data.bgr.uc.B = gan_pixel_ui_to_uc(inpix->data.gla.ui.I);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = outpix->data.bgr.us.G = outpix->data.bgr.us.B = gan_pixel_ui_to_us(inpix->data.gla.ui.I);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = outpix->data.bgr.ui.G = outpix->data.bgr.ui.B = inpix->data.gla.ui.I;
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = outpix->data.bgr.d.G = outpix->data.bgr.d.B = gan_pixel_ui_to_d(inpix->data.gla.ui.I);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = outpix->data.bgr.f.G = outpix->data.bgr.f.B = gan_pixel_ui_to_f(inpix->data.gla.ui.I);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_gla_bgr", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_gla_bgra ( Gan_Pixel *inpix, Gan_Pixel *outpix,
                           int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_GREY_LEVEL_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_gla_bgra", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = outpix->data.bgra.uc.G = 
             outpix->data.bgra.uc.B = gan_pixel_d_to_uc(inpix->data.gla.d.I);
             outpix->data.bgra.uc.A = gan_pixel_d_to_uc(inpix->data.gla.d.A);
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = outpix->data.bgra.us.G =
             outpix->data.bgra.us.B = gan_pixel_d_to_us(inpix->data.gla.d.I);
             outpix->data.bgra.us.A = gan_pixel_d_to_us(inpix->data.gla.d.A);
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = outpix->data.bgra.ui.G = 
             outpix->data.bgra.ui.B = gan_pixel_d_to_ui(inpix->data.gla.d.I);
             outpix->data.bgra.ui.A = gan_pixel_d_to_ui(inpix->data.gla.d.A);
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = outpix->data.bgra.f.G = 
             outpix->data.bgra.f.B = (float)inpix->data.gla.d.I;
             outpix->data.bgra.f.A = (float)inpix->data.gla.d.A;
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = outpix->data.bgra.d.G = 
             outpix->data.bgra.d.B = inpix->data.gla.d.I;
             outpix->data.bgra.d.A = inpix->data.gla.d.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = outpix->data.bgra.uc.G = 
             outpix->data.bgra.uc.B = gan_pixel_f_to_uc(inpix->data.gla.f.I);
             outpix->data.bgra.uc.A = gan_pixel_f_to_uc(inpix->data.gla.f.A);
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = outpix->data.bgra.us.G =
             outpix->data.bgra.us.B = gan_pixel_f_to_us(inpix->data.gla.f.I);
             outpix->data.bgra.us.A = gan_pixel_f_to_us(inpix->data.gla.f.A);
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = outpix->data.bgra.ui.G = 
             outpix->data.bgra.ui.B = gan_pixel_f_to_ui(inpix->data.gla.f.I);
             outpix->data.bgra.ui.A = gan_pixel_f_to_ui(inpix->data.gla.f.A);
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = outpix->data.bgra.f.G =
             outpix->data.bgra.f.B = inpix->data.gla.f.I;
             outpix->data.bgra.f.A = inpix->data.gla.f.A;
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = outpix->data.bgra.d.G =
             outpix->data.bgra.d.B = (double)inpix->data.gla.f.I;
             outpix->data.bgra.d.A = (double)inpix->data.gla.f.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = outpix->data.bgra.uc.G =
             outpix->data.bgra.uc.B = inpix->data.gla.uc.I;
             outpix->data.bgra.uc.A = inpix->data.gla.uc.A;
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = outpix->data.bgra.us.G =
             outpix->data.bgra.us.B = gan_pixel_uc_to_us(inpix->data.gla.uc.I);
             outpix->data.bgra.us.A = gan_pixel_uc_to_us(inpix->data.gla.uc.A);
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = outpix->data.bgra.ui.G =
             outpix->data.bgra.ui.B = gan_pixel_uc_to_ui(inpix->data.gla.uc.I);
             outpix->data.bgra.ui.A = gan_pixel_uc_to_ui(inpix->data.gla.uc.A);
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = outpix->data.bgra.f.G =
             outpix->data.bgra.f.B = gan_pixel_uc_to_f(inpix->data.gla.uc.I);
             outpix->data.bgra.f.A = gan_pixel_uc_to_f(inpix->data.gla.uc.A);
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = outpix->data.bgra.d.G =
             outpix->data.bgra.d.B = gan_pixel_uc_to_d(inpix->data.gla.uc.I);
             outpix->data.bgra.d.A = gan_pixel_uc_to_d(inpix->data.gla.uc.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = outpix->data.bgra.uc.G =
             outpix->data.bgra.uc.B = gan_pixel_us_to_uc(inpix->data.gla.us.I);
             outpix->data.bgra.uc.A = gan_pixel_us_to_uc(inpix->data.gla.us.A);
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = outpix->data.bgra.us.G =
             outpix->data.bgra.us.B = inpix->data.gla.us.I;
             outpix->data.bgra.us.A = inpix->data.gla.us.A;
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = outpix->data.bgra.ui.G =
             outpix->data.bgra.ui.B = gan_pixel_us_to_ui(inpix->data.gla.us.I);
             outpix->data.bgra.ui.A =gan_pixel_us_to_ui(inpix->data.gla.us.A);
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = outpix->data.bgra.d.G =
             outpix->data.bgra.d.B = gan_pixel_us_to_d(inpix->data.gla.us.I);
             outpix->data.bgra.d.A = gan_pixel_us_to_d(inpix->data.gla.us.A);
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = outpix->data.bgra.f.G =
             outpix->data.bgra.f.B = gan_pixel_us_to_f(inpix->data.gla.us.I);
             outpix->data.bgra.f.A = gan_pixel_us_to_f(inpix->data.gla.us.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_gla_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = outpix->data.bgra.uc.G =
             outpix->data.bgra.uc.B = gan_pixel_ui_to_uc(inpix->data.gla.ui.I);
             outpix->data.bgra.uc.A = gan_pixel_ui_to_uc(inpix->data.gla.ui.A);
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = outpix->data.bgra.us.G =
             outpix->data.bgra.us.B = gan_pixel_ui_to_us(inpix->data.gla.ui.I);
             outpix->data.bgra.us.A = gan_pixel_ui_to_us(inpix->data.gla.ui.A);
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = outpix->data.bgra.ui.G =
             outpix->data.bgra.ui.B = inpix->data.gla.ui.I;
             outpix->data.bgra.ui.A = inpix->data.gla.ui.A;
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = outpix->data.bgra.f.G =
             outpix->data.bgra.f.B = gan_pixel_ui_to_f(inpix->data.gla.ui.I);
             outpix->data.bgra.f.A = gan_pixel_ui_to_f(inpix->data.gla.ui.A);
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = outpix->data.bgra.d.G =
             outpix->data.bgra.d.B = gan_pixel_ui_to_d(inpix->data.gla.ui.I);
             outpix->data.bgra.d.A = gan_pixel_ui_to_d(inpix->data.gla.ui.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      default:
        if ( error_code == NULL )
        {
           gan_err_flush_trace();
           gan_err_register ( "convert_pixel_bgra_bgra", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        }
        else
           *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/* convert from BGR to grey-level */
static Gan_Bool
 convert_pixel_bgr_gl ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_BGR_COLOUR_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_bgr_gl", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_bgruc_to_yuc(&inpix->data.bgr.uc);
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_bgruc_to_yus(&inpix->data.bgr.uc);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_bgruc_to_yui(&inpix->data.bgr.uc);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_us_to_f(gan_pixel_bgruc_to_yus(&inpix->data.bgr.uc) );
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_us_to_d(gan_pixel_bgruc_to_yus(&inpix->data.bgr.uc) );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_bgrus_to_yuc(&inpix->data.bgr.us);
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_bgrus_to_yus(&inpix->data.bgr.us);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_us_to_ui(gan_pixel_bgrus_to_yus(&inpix->data.bgr.us));
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_us_to_f(gan_pixel_bgrus_to_yus(&inpix->data.bgr.us) );
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_us_to_d(gan_pixel_bgrus_to_yus(&inpix->data.bgr.us) );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_f_to_uc(gan_pixel_bgrui_to_yf(&inpix->data.bgr.ui) );
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_f_to_us(gan_pixel_bgrui_to_yf(&inpix->data.bgr.ui) );
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_f_to_ui(gan_pixel_bgrui_to_yf(&inpix->data.bgr.ui) );
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_bgrui_to_yf(&inpix->data.bgr.ui);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = (double)gan_pixel_bgrui_to_yf(&inpix->data.bgr.ui);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_f_to_uc(gan_pixel_bgrf_to_yf(&inpix->data.bgr.f) );
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_f_to_us(gan_pixel_bgrf_to_yf(&inpix->data.bgr.f) );
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_f_to_ui(gan_pixel_bgrf_to_yf(&inpix->data.bgr.f) );
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_bgrf_to_yf(&inpix->data.bgr.f);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = (double)gan_pixel_bgrf_to_yf(&inpix->data.bgr.f);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_d_to_uc(gan_pixel_bgrd_to_yd(&inpix->data.bgr.d) );
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_d_to_us(gan_pixel_bgrd_to_yd(&inpix->data.bgr.d) );
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_d_to_ui(gan_pixel_bgrd_to_yd(&inpix->data.bgr.d) );
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = (float)
                                 gan_pixel_bgrd_to_yd(&inpix->data.bgr.d);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_bgrd_to_yd(&inpix->data.bgr.d);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_bgr_gl", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_bgr_gla ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_BGR_COLOUR_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_bgr_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_bgruc_to_yuc(&inpix->data.bgr.uc);
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_bgruc_to_yus(&inpix->data.bgr.uc);
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_bgruc_to_yui(&inpix->data.bgr.uc);
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_us_to_f(gan_pixel_bgruc_to_yus(&inpix->data.bgr.uc) );
             outpix->data.gla.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_us_to_d(gan_pixel_bgruc_to_yus(&inpix->data.bgr.uc) );
             outpix->data.gla.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_bgrus_to_yuc(&inpix->data.bgr.us);
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_bgrus_to_yus(&inpix->data.bgr.us);
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_us_to_ui(gan_pixel_bgrus_to_yus(&inpix->data.bgr.us));
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_us_to_f(gan_pixel_bgrus_to_yus(&inpix->data.bgr.us) );
             outpix->data.gla.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_us_to_d(gan_pixel_bgrus_to_yus(&inpix->data.bgr.us) );
             outpix->data.gla.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_f_to_uc(gan_pixel_bgrui_to_yf(&inpix->data.bgr.ui) );
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_f_to_us(gan_pixel_bgrui_to_yf(&inpix->data.bgr.ui) );
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_f_to_ui(gan_pixel_bgrui_to_yf(&inpix->data.bgr.ui) );
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_bgrui_to_yf(&inpix->data.bgr.ui);
             outpix->data.gla.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = (double)gan_pixel_bgrui_to_yf(&inpix->data.bgr.ui);
             outpix->data.gla.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_f_to_uc(gan_pixel_bgrf_to_yf(&inpix->data.bgr.f) );
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_f_to_us(gan_pixel_bgrf_to_yf(&inpix->data.bgr.f) );
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_f_to_ui(gan_pixel_bgrf_to_yf(&inpix->data.bgr.f) );
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_bgrf_to_yf(&inpix->data.bgr.f);
             outpix->data.gla.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = (double)gan_pixel_bgrf_to_yf(&inpix->data.bgr.f);
             outpix->data.gla.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_d_to_uc(gan_pixel_bgrd_to_yd(&inpix->data.bgr.d) );
             outpix->data.gla.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_d_to_us(gan_pixel_bgrd_to_yd(&inpix->data.bgr.d) );
             outpix->data.gla.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_d_to_ui(gan_pixel_bgrd_to_yd(&inpix->data.bgr.d) );
             outpix->data.gla.ui.A = UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = (float)gan_pixel_bgrd_to_yd(&inpix->data.bgr.d);
             outpix->data.gla.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_bgrd_to_yd(&inpix->data.bgr.d);
             outpix->data.gla.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_bgr_gla", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_bgr_bgr ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_BGR_COLOUR_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_bgr_bgr", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = gan_pixel_d_to_uc(inpix->data.bgr.d.R);
             outpix->data.bgr.uc.G = gan_pixel_d_to_uc(inpix->data.bgr.d.G);
             outpix->data.bgr.uc.B = gan_pixel_d_to_uc(inpix->data.bgr.d.B);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = gan_pixel_d_to_us(inpix->data.bgr.d.R);
             outpix->data.bgr.us.G = gan_pixel_d_to_us(inpix->data.bgr.d.G);
             outpix->data.bgr.us.B = gan_pixel_d_to_us(inpix->data.bgr.d.B);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = gan_pixel_d_to_ui(inpix->data.bgr.d.R);
             outpix->data.bgr.ui.G = gan_pixel_d_to_ui(inpix->data.bgr.d.G);
             outpix->data.bgr.ui.B = gan_pixel_d_to_ui(inpix->data.bgr.d.B);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = (float)inpix->data.bgr.d.R;
             outpix->data.bgr.f.G = (float)inpix->data.bgr.d.G;
             outpix->data.bgr.f.B = (float)inpix->data.bgr.d.B;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = gan_pixel_f_to_uc(inpix->data.bgr.f.R);
             outpix->data.bgr.uc.G = gan_pixel_f_to_uc(inpix->data.bgr.f.G);
             outpix->data.bgr.uc.B = gan_pixel_f_to_uc(inpix->data.bgr.f.B);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = gan_pixel_f_to_us(inpix->data.bgr.f.R);
             outpix->data.bgr.us.G = gan_pixel_f_to_us(inpix->data.bgr.f.G);
             outpix->data.bgr.us.B = gan_pixel_f_to_us(inpix->data.bgr.f.B);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = gan_pixel_f_to_ui(inpix->data.bgr.f.R);
             outpix->data.bgr.ui.G = gan_pixel_f_to_ui(inpix->data.bgr.f.G);
             outpix->data.bgr.ui.B = gan_pixel_f_to_ui(inpix->data.bgr.f.B);
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = (double)inpix->data.bgr.f.R;
             outpix->data.bgr.d.G = (double)inpix->data.bgr.f.G;
             outpix->data.bgr.d.B = (double)inpix->data.bgr.f.B;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_USHORT:
             outpix->data.bgr.us.R = gan_pixel_uc_to_us(inpix->data.bgr.uc.R);
             outpix->data.bgr.us.G = gan_pixel_uc_to_us(inpix->data.bgr.uc.G);
             outpix->data.bgr.us.B = gan_pixel_uc_to_us(inpix->data.bgr.uc.B);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = gan_pixel_uc_to_ui(inpix->data.bgr.uc.R);
             outpix->data.bgr.ui.G = gan_pixel_uc_to_ui(inpix->data.bgr.uc.G);
             outpix->data.bgr.ui.B = gan_pixel_uc_to_ui(inpix->data.bgr.uc.B);
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = gan_pixel_uc_to_d(inpix->data.bgr.uc.R);
             outpix->data.bgr.d.G = gan_pixel_uc_to_d(inpix->data.bgr.uc.G);
             outpix->data.bgr.d.B = gan_pixel_uc_to_d(inpix->data.bgr.uc.B);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = gan_pixel_uc_to_f(inpix->data.bgr.uc.R);
             outpix->data.bgr.f.G = gan_pixel_uc_to_f(inpix->data.bgr.uc.G);
             outpix->data.bgr.f.B = gan_pixel_uc_to_f(inpix->data.bgr.uc.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = gan_pixel_us_to_uc(inpix->data.bgr.us.R);
             outpix->data.bgr.uc.G = gan_pixel_us_to_uc(inpix->data.bgr.us.G);
             outpix->data.bgr.uc.B = gan_pixel_us_to_uc(inpix->data.bgr.us.B);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = gan_pixel_us_to_ui(inpix->data.bgr.us.R);
             outpix->data.bgr.ui.G = gan_pixel_us_to_ui(inpix->data.bgr.us.G);
             outpix->data.bgr.ui.B = gan_pixel_us_to_ui(inpix->data.bgr.us.B);
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = gan_pixel_us_to_d(inpix->data.bgr.us.R);
             outpix->data.bgr.d.G = gan_pixel_us_to_d(inpix->data.bgr.us.G);
             outpix->data.bgr.d.B = gan_pixel_us_to_d(inpix->data.bgr.us.B);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = gan_pixel_us_to_f(inpix->data.bgr.us.R);
             outpix->data.bgr.f.G = gan_pixel_us_to_f(inpix->data.bgr.us.G);
             outpix->data.bgr.f.B = gan_pixel_us_to_f(inpix->data.bgr.us.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_INT:
        switch ( outpix->type )
        {
           case GAN_DOUBLE:
             outpix->data.bgr.d.R = gan_pixel_i_to_d(inpix->data.bgr.i.R);
             outpix->data.bgr.d.G = gan_pixel_i_to_d(inpix->data.bgr.i.G);
             outpix->data.bgr.d.B = gan_pixel_i_to_d(inpix->data.bgr.i.B);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = gan_pixel_i_to_f(inpix->data.bgr.i.R);
             outpix->data.bgr.f.G = gan_pixel_i_to_f(inpix->data.bgr.i.G);
             outpix->data.bgr.f.B = gan_pixel_i_to_f(inpix->data.bgr.i.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = gan_pixel_ui_to_uc(inpix->data.bgr.ui.R);
             outpix->data.bgr.uc.G = gan_pixel_ui_to_uc(inpix->data.bgr.ui.G);
             outpix->data.bgr.uc.B = gan_pixel_ui_to_uc(inpix->data.bgr.ui.B);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = gan_pixel_ui_to_us(inpix->data.bgr.ui.R);
             outpix->data.bgr.us.G = gan_pixel_ui_to_us(inpix->data.bgr.ui.G);
             outpix->data.bgr.us.B = gan_pixel_ui_to_us(inpix->data.bgr.ui.B);
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = gan_pixel_ui_to_d(inpix->data.bgr.ui.R);
             outpix->data.bgr.d.G = gan_pixel_ui_to_d(inpix->data.bgr.ui.G);
             outpix->data.bgr.d.B = gan_pixel_ui_to_d(inpix->data.bgr.ui.B);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = gan_pixel_ui_to_f(inpix->data.bgr.ui.R);
             outpix->data.bgr.f.G = gan_pixel_ui_to_f(inpix->data.bgr.ui.G);
             outpix->data.bgr.f.B = gan_pixel_ui_to_f(inpix->data.bgr.ui.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_bgr_bgr", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_bgr_bgra ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_BGR_COLOUR_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_bgr_bgra", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = gan_pixel_d_to_uc(inpix->data.bgr.d.R);
             outpix->data.bgra.uc.G = gan_pixel_d_to_uc(inpix->data.bgr.d.G);
             outpix->data.bgra.uc.B = gan_pixel_d_to_uc(inpix->data.bgr.d.B);
             outpix->data.bgra.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = gan_pixel_d_to_us(inpix->data.bgr.d.R);
             outpix->data.bgra.us.G = gan_pixel_d_to_us(inpix->data.bgr.d.G);
             outpix->data.bgra.us.B = gan_pixel_d_to_us(inpix->data.bgr.d.B);
             outpix->data.bgra.us.A= USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = gan_pixel_d_to_ui(inpix->data.bgr.d.R);
             outpix->data.bgra.ui.G = gan_pixel_d_to_ui(inpix->data.bgr.d.G);
             outpix->data.bgra.ui.B = gan_pixel_d_to_ui(inpix->data.bgr.d.B);
             outpix->data.bgra.ui.A= UINT_MAX;
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = (float)inpix->data.bgr.d.R;
             outpix->data.bgra.f.G = (float)inpix->data.bgr.d.G;
             outpix->data.bgra.f.B = (float)inpix->data.bgr.d.B;
             outpix->data.bgra.f.A = 1.0F;
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = inpix->data.bgr.d.R;
             outpix->data.bgra.d.G = inpix->data.bgr.d.G;
             outpix->data.bgra.d.B = inpix->data.bgr.d.B;
             outpix->data.bgra.d.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = gan_pixel_f_to_uc(inpix->data.bgr.f.R);
             outpix->data.bgra.uc.G = gan_pixel_f_to_uc(inpix->data.bgr.f.G);
             outpix->data.bgra.uc.B = gan_pixel_f_to_uc(inpix->data.bgr.f.B);
             outpix->data.bgra.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = gan_pixel_f_to_us(inpix->data.bgr.f.R);
             outpix->data.bgra.us.G = gan_pixel_f_to_us(inpix->data.bgr.f.G);
             outpix->data.bgra.us.B = gan_pixel_f_to_us(inpix->data.bgr.f.B);
             outpix->data.bgra.us.A= USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = gan_pixel_f_to_ui(inpix->data.bgr.f.R);
             outpix->data.bgra.ui.G = gan_pixel_f_to_ui(inpix->data.bgr.f.G);
             outpix->data.bgra.ui.B = gan_pixel_f_to_ui(inpix->data.bgr.f.B);
             outpix->data.bgra.ui.A= UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = (double)inpix->data.bgr.f.R;
             outpix->data.bgra.d.G = (double)inpix->data.bgr.f.G;
             outpix->data.bgra.d.B = (double)inpix->data.bgr.f.B;
             outpix->data.bgra.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = inpix->data.bgr.f.R;
             outpix->data.bgra.f.G = inpix->data.bgr.f.G;
             outpix->data.bgra.f.B = inpix->data.bgr.f.B;
             outpix->data.bgra.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = inpix->data.bgr.uc.R;
             outpix->data.bgra.uc.G = inpix->data.bgr.uc.G;
             outpix->data.bgra.uc.B = inpix->data.bgr.uc.B;
             outpix->data.bgra.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = gan_pixel_uc_to_us(inpix->data.bgr.uc.R);
             outpix->data.bgra.us.G = gan_pixel_uc_to_us(inpix->data.bgr.uc.G);
             outpix->data.bgra.us.B = gan_pixel_uc_to_us(inpix->data.bgr.uc.B);
             outpix->data.bgra.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = gan_pixel_uc_to_ui(inpix->data.bgr.uc.R);
             outpix->data.bgra.ui.G = gan_pixel_uc_to_ui(inpix->data.bgr.uc.G);
             outpix->data.bgra.ui.B = gan_pixel_uc_to_ui(inpix->data.bgr.uc.B);
             outpix->data.bgra.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = gan_pixel_uc_to_d(inpix->data.bgr.uc.R);
             outpix->data.bgra.d.G = gan_pixel_uc_to_d(inpix->data.bgr.uc.G);
             outpix->data.bgra.d.B = gan_pixel_uc_to_d(inpix->data.bgr.uc.B);
             outpix->data.bgra.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = gan_pixel_uc_to_f(inpix->data.bgr.uc.R);
             outpix->data.bgra.f.G = gan_pixel_uc_to_f(inpix->data.bgr.uc.G);
             outpix->data.bgra.f.B = gan_pixel_uc_to_f(inpix->data.bgr.uc.B);
             outpix->data.bgra.f.A = 1.0;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = gan_pixel_us_to_uc(inpix->data.bgr.us.R);
             outpix->data.bgra.uc.G = gan_pixel_us_to_uc(inpix->data.bgr.us.G);
             outpix->data.bgra.uc.B = gan_pixel_us_to_uc(inpix->data.bgr.us.B);
             outpix->data.bgra.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = inpix->data.bgr.us.R;
             outpix->data.bgra.us.G = inpix->data.bgr.us.G;
             outpix->data.bgra.us.B = inpix->data.bgr.us.B;
             outpix->data.bgra.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = gan_pixel_us_to_ui(inpix->data.bgr.us.R);
             outpix->data.bgra.ui.G = gan_pixel_us_to_ui(inpix->data.bgr.us.G);
             outpix->data.bgra.ui.B = gan_pixel_us_to_ui(inpix->data.bgr.us.B);
             outpix->data.bgra.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = gan_pixel_us_to_d(inpix->data.bgr.us.R);
             outpix->data.bgra.d.G = gan_pixel_us_to_d(inpix->data.bgr.us.G);
             outpix->data.bgra.d.B = gan_pixel_us_to_d(inpix->data.bgr.us.B);
             outpix->data.bgra.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = gan_pixel_us_to_f(inpix->data.bgr.us.R);
             outpix->data.bgra.f.G = gan_pixel_us_to_f(inpix->data.bgr.us.G);
             outpix->data.bgra.f.B = gan_pixel_us_to_f(inpix->data.bgr.us.B);
             outpix->data.bgra.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = gan_pixel_ui_to_uc(inpix->data.bgr.ui.R);
             outpix->data.bgra.uc.G = gan_pixel_ui_to_uc(inpix->data.bgr.ui.G);
             outpix->data.bgra.uc.B = gan_pixel_ui_to_uc(inpix->data.bgr.ui.B);
             outpix->data.bgra.uc.A = UCHAR_MAX;
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = gan_pixel_ui_to_us(inpix->data.bgr.ui.R);
             outpix->data.bgra.us.G = gan_pixel_ui_to_us(inpix->data.bgr.ui.G);
             outpix->data.bgra.us.B = gan_pixel_ui_to_us(inpix->data.bgr.ui.B);
             outpix->data.bgra.us.A = USHRT_MAX;
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = inpix->data.bgr.ui.R;
             outpix->data.bgra.ui.G = inpix->data.bgr.ui.G;
             outpix->data.bgra.ui.B = inpix->data.bgr.ui.B;
             outpix->data.bgra.ui.A = UINT_MAX;
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = gan_pixel_ui_to_d(inpix->data.bgr.ui.R);
             outpix->data.bgra.d.G = gan_pixel_ui_to_d(inpix->data.bgr.ui.G);
             outpix->data.bgra.d.B = gan_pixel_ui_to_d(inpix->data.bgr.ui.B);
             outpix->data.bgra.d.A = 1.0;
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = gan_pixel_ui_to_f(inpix->data.bgr.ui.R);
             outpix->data.bgra.f.G = gan_pixel_ui_to_f(inpix->data.bgr.ui.G);
             outpix->data.bgra.f.B = gan_pixel_ui_to_f(inpix->data.bgr.ui.B);
             outpix->data.bgra.f.A = 1.0F;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_bgr_bgra", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_bgr_vfield2D ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_BGR_COLOUR_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_bgr_vfield2D", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_FLOAT:
             outpix->data.vfield2D.f.x = inpix->data.bgr.f.R;
             outpix->data.vfield2D.f.y = inpix->data.bgr.f.G;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgr_vfield2D", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_bgr_vfield2D", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/* convert from BGRA to grey-level with */
static Gan_Bool
 convert_pixel_bgra_gl ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code)
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_BGR_COLOUR_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_bgra_gl",
                            GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc =gan_pixel_bgrauc_to_yuc(&inpix->data.bgra.uc);
             break;

           case GAN_USHORT:
             outpix->data.gl.us =gan_pixel_bgrauc_to_yus(&inpix->data.bgra.uc);
             break;

           case GAN_UINT:
             outpix->data.gl.ui =gan_pixel_bgrauc_to_yui(&inpix->data.bgra.uc);
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_us_to_f(gan_pixel_bgrauc_to_yus(&inpix->data.bgra.uc) );
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_us_to_d(gan_pixel_bgrauc_to_yus(&inpix->data.bgra.uc) );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc =gan_pixel_bgraus_to_yuc(&inpix->data.bgra.us);
             break;

           case GAN_USHORT:
             outpix->data.gl.us =gan_pixel_bgraus_to_yus(&inpix->data.bgra.us);
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_us_to_ui(gan_pixel_bgraus_to_yus(&inpix->data.bgra.us));
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_us_to_f(gan_pixel_bgraus_to_yus(&inpix->data.bgra.us) );
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_us_to_d(gan_pixel_bgraus_to_yus(&inpix->data.bgra.us) );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_f_to_uc(gan_pixel_bgraui_to_yf(&inpix->data.bgra.ui) );
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_f_to_us(gan_pixel_bgraui_to_yf(&inpix->data.bgra.ui) );
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_f_to_ui(gan_pixel_bgraui_to_yf(&inpix->data.bgra.ui) );
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_bgraui_to_yf(&inpix->data.bgra.ui);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = (double)gan_pixel_bgraui_to_yf(&inpix->data.bgra.ui);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_f_to_uc(gan_pixel_bgraf_to_yf(&inpix->data.bgra.f) );
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_f_to_us(gan_pixel_bgraf_to_yf(&inpix->data.bgra.f) );
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_f_to_ui(gan_pixel_bgraf_to_yf(&inpix->data.bgra.f) );
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = gan_pixel_bgraf_to_yf(&inpix->data.bgra.f);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = (double)gan_pixel_bgraf_to_yf(&inpix->data.bgra.f);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gl.uc = gan_pixel_d_to_uc(gan_pixel_bgrad_to_yd(&inpix->data.bgra.d) );
             break;

           case GAN_USHORT:
             outpix->data.gl.us = gan_pixel_d_to_us(gan_pixel_bgrad_to_yd(&inpix->data.bgra.d) );
             break;

           case GAN_UINT:
             outpix->data.gl.ui = gan_pixel_d_to_ui(gan_pixel_bgrad_to_yd(&inpix->data.bgra.d) );
             break;

           case GAN_FLOAT:
             outpix->data.gl.f = (float)gan_pixel_bgrad_to_yd(&inpix->data.bgra.d);
             break;

           case GAN_DOUBLE:
             outpix->data.gl.d = gan_pixel_bgrad_to_yd(&inpix->data.bgra.d);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_bgra_gl", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/* convert from BGRA to grey-level with alpha channel */
static Gan_Bool
 convert_pixel_bgra_gla ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code)
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_BGR_COLOUR_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_bgra_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_bgrauc_to_yuc(&inpix->data.bgra.uc);
             outpix->data.gla.uc.A = inpix->data.bgra.uc.A;
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_bgrauc_to_yus(&inpix->data.bgra.uc);
             outpix->data.gla.us.A = gan_pixel_uc_to_us(inpix->data.bgra.uc.A);
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_bgrauc_to_yui(&inpix->data.bgra.uc);
             outpix->data.gla.ui.A = gan_pixel_uc_to_ui(inpix->data.bgra.uc.A);
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_us_to_f (
                               gan_pixel_bgrauc_to_yus(&inpix->data.bgra.uc) );
             outpix->data.gla.f.A = gan_pixel_uc_to_f(inpix->data.bgra.uc.A);
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_us_to_d(gan_pixel_bgrauc_to_yus(&inpix->data.bgra.uc) );
             outpix->data.gla.d.A = gan_pixel_uc_to_d(inpix->data.bgra.uc.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_bgraus_to_yuc(&inpix->data.bgra.us);
             outpix->data.gla.uc.A = gan_pixel_us_to_uc(inpix->data.bgra.us.A);
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_bgraus_to_yus(&inpix->data.bgra.us);
             outpix->data.gla.us.A = inpix->data.bgra.us.A;
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_us_to_ui(gan_pixel_bgraus_to_yus(&inpix->data.bgra.us));
             outpix->data.gla.ui.A = gan_pixel_us_to_ui(inpix->data.bgra.us.A);
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_us_to_f(gan_pixel_bgraus_to_yus(&inpix->data.bgra.us) );
             outpix->data.gla.f.A = gan_pixel_us_to_f(inpix->data.bgra.us.A);
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_us_to_d(gan_pixel_bgraus_to_yus(&inpix->data.bgra.us) );
             outpix->data.gla.d.A = gan_pixel_us_to_d(inpix->data.bgra.us.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_f_to_uc(gan_pixel_bgraui_to_yf(&inpix->data.bgra.ui) );
             outpix->data.gla.uc.A = gan_pixel_ui_to_uc(inpix->data.bgra.ui.A);
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_f_to_us(gan_pixel_bgraui_to_yf(&inpix->data.bgra.ui) );
             outpix->data.gla.us.A = gan_pixel_ui_to_us(inpix->data.bgra.ui.A);
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_f_to_ui(gan_pixel_bgraui_to_yf(&inpix->data.bgra.ui) );
             outpix->data.gla.ui.A = inpix->data.bgra.ui.A;
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I =
                                  gan_pixel_bgraui_to_yf(&inpix->data.bgra.ui);
             outpix->data.gla.f.A = gan_pixel_ui_to_f(inpix->data.bgra.ui.A);
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = (double)gan_pixel_bgraui_to_yf(&inpix->data.bgra.ui);
             outpix->data.gla.d.A = gan_pixel_ui_to_d(inpix->data.bgra.ui.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_f_to_uc(gan_pixel_bgraf_to_yf(&inpix->data.bgra.f) );
             outpix->data.gla.uc.A = gan_pixel_f_to_uc(inpix->data.bgra.f.A);
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_f_to_us(gan_pixel_bgraf_to_yf(&inpix->data.bgra.f) );
             outpix->data.gla.us.A = gan_pixel_f_to_us(inpix->data.bgra.f.A);
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_f_to_ui(gan_pixel_bgraf_to_yf(&inpix->data.bgra.f) );
             outpix->data.gla.ui.A = gan_pixel_f_to_ui(inpix->data.bgra.f.A);
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = gan_pixel_bgraf_to_yf(&inpix->data.bgra.f);
             outpix->data.gla.f.A = inpix->data.bgra.f.A;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = (double)gan_pixel_bgraf_to_yf(&inpix->data.bgra.f);
             outpix->data.gla.d.A = (double)inpix->data.bgra.f.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.gla.uc.I = gan_pixel_d_to_uc(gan_pixel_bgrad_to_yd(&inpix->data.bgra.d) );
             outpix->data.gla.uc.A = gan_pixel_d_to_uc(inpix->data.bgra.d.A);
             break;

           case GAN_USHORT:
             outpix->data.gla.us.I = gan_pixel_d_to_us(gan_pixel_bgrad_to_yd(&inpix->data.bgra.d) );
             outpix->data.gla.us.A = gan_pixel_d_to_us(inpix->data.bgra.d.A);
             break;

           case GAN_UINT:
             outpix->data.gla.ui.I = gan_pixel_d_to_ui(gan_pixel_bgrad_to_yd(&inpix->data.bgra.d) );
             outpix->data.gla.ui.A = gan_pixel_d_to_ui(inpix->data.bgra.d.A);
             break;

           case GAN_FLOAT:
             outpix->data.gla.f.I = (float)gan_pixel_bgrad_to_yd(&inpix->data.bgra.d);
             outpix->data.gla.f.A = (float)inpix->data.bgra.d.A;
             break;

           case GAN_DOUBLE:
             outpix->data.gla.d.I = gan_pixel_bgrad_to_yd(&inpix->data.bgra.d);
             outpix->data.gla.d.A = inpix->data.bgra.d.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_gla", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_bgra_gla", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

          return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_bgra_bgr ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_BGR_COLOUR_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_bgra_bgr", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = gan_pixel_d_to_uc(inpix->data.bgra.d.R);
             outpix->data.bgr.uc.G = gan_pixel_d_to_uc(inpix->data.bgra.d.G);
             outpix->data.bgr.uc.B = gan_pixel_d_to_uc(inpix->data.bgra.d.B);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = gan_pixel_d_to_us(inpix->data.bgra.d.R);
             outpix->data.bgr.us.G = gan_pixel_d_to_us(inpix->data.bgra.d.G);
             outpix->data.bgr.us.B = gan_pixel_d_to_us(inpix->data.bgra.d.B);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = gan_pixel_d_to_ui(inpix->data.bgra.d.R);
             outpix->data.bgr.ui.G = gan_pixel_d_to_ui(inpix->data.bgra.d.G);
             outpix->data.bgr.ui.B = gan_pixel_d_to_ui(inpix->data.bgra.d.B);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = (float)inpix->data.bgra.d.R;
             outpix->data.bgr.f.G = (float)inpix->data.bgra.d.G;
             outpix->data.bgr.f.B = (float)inpix->data.bgra.d.B;
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = inpix->data.bgra.d.R;
             outpix->data.bgr.d.G = inpix->data.bgra.d.G;
             outpix->data.bgr.d.B = inpix->data.bgra.d.B;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = gan_pixel_f_to_uc(inpix->data.bgra.f.R);
             outpix->data.bgr.uc.G = gan_pixel_f_to_uc(inpix->data.bgra.f.G);
             outpix->data.bgr.uc.B = gan_pixel_f_to_uc(inpix->data.bgra.f.B);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = gan_pixel_f_to_us(inpix->data.bgra.f.R);
             outpix->data.bgr.us.G = gan_pixel_f_to_us(inpix->data.bgra.f.G);
             outpix->data.bgr.us.B = gan_pixel_f_to_us(inpix->data.bgra.f.B);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = gan_pixel_f_to_ui(inpix->data.bgra.f.R);
             outpix->data.bgr.ui.G = gan_pixel_f_to_ui(inpix->data.bgra.f.G);
             outpix->data.bgr.ui.B = gan_pixel_f_to_ui(inpix->data.bgra.f.B);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = inpix->data.bgra.f.R;
             outpix->data.bgr.f.G = inpix->data.bgra.f.G;
             outpix->data.bgr.f.B = inpix->data.bgra.f.B;
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = (double)inpix->data.bgra.f.R;
             outpix->data.bgr.d.G = (double)inpix->data.bgra.f.G;
             outpix->data.bgr.d.B = (double)inpix->data.bgra.f.B;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = inpix->data.bgra.uc.R;
             outpix->data.bgr.uc.G = inpix->data.bgra.uc.G;
             outpix->data.bgr.uc.B = inpix->data.bgra.uc.B;
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = gan_pixel_uc_to_us(inpix->data.bgra.uc.R);
             outpix->data.bgr.us.G = gan_pixel_uc_to_us(inpix->data.bgra.uc.G);
             outpix->data.bgr.us.B = gan_pixel_uc_to_us(inpix->data.bgra.uc.B);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = gan_pixel_uc_to_ui(inpix->data.bgra.uc.R);
             outpix->data.bgr.ui.G = gan_pixel_uc_to_ui(inpix->data.bgra.uc.G);
             outpix->data.bgr.ui.B = gan_pixel_uc_to_ui(inpix->data.bgra.uc.B);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = gan_pixel_uc_to_f(inpix->data.bgra.uc.R);
             outpix->data.bgr.f.G = gan_pixel_uc_to_f(inpix->data.bgra.uc.G);
             outpix->data.bgr.f.B = gan_pixel_uc_to_f(inpix->data.bgra.uc.B);
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = gan_pixel_uc_to_d(inpix->data.bgra.uc.R);
             outpix->data.bgr.d.G = gan_pixel_uc_to_d(inpix->data.bgra.uc.G);
             outpix->data.bgr.d.B = gan_pixel_uc_to_d(inpix->data.bgra.uc.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = gan_pixel_us_to_uc(inpix->data.bgra.us.R);
             outpix->data.bgr.uc.G = gan_pixel_us_to_uc(inpix->data.bgra.us.G);
             outpix->data.bgr.uc.B = gan_pixel_us_to_uc(inpix->data.bgra.us.B);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = inpix->data.bgra.us.R;
             outpix->data.bgr.us.G = inpix->data.bgra.us.G;
             outpix->data.bgr.us.B = inpix->data.bgra.us.B;
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = gan_pixel_us_to_ui(inpix->data.bgra.us.R);
             outpix->data.bgr.ui.G = gan_pixel_us_to_ui(inpix->data.bgra.us.G);
             outpix->data.bgr.ui.B = gan_pixel_us_to_ui(inpix->data.bgra.us.B);
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = gan_pixel_us_to_f(inpix->data.bgra.us.R);
             outpix->data.bgr.f.G = gan_pixel_us_to_f(inpix->data.bgra.us.G);
             outpix->data.bgr.f.B = gan_pixel_us_to_f(inpix->data.bgra.us.B);
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = gan_pixel_us_to_d(inpix->data.bgra.us.R);
             outpix->data.bgr.d.G = gan_pixel_us_to_d(inpix->data.bgra.us.G);
             outpix->data.bgr.d.B = gan_pixel_us_to_d(inpix->data.bgra.us.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgr.uc.R = gan_pixel_ui_to_uc(inpix->data.bgra.ui.R);
             outpix->data.bgr.uc.G = gan_pixel_ui_to_uc(inpix->data.bgra.ui.G);
             outpix->data.bgr.uc.B = gan_pixel_ui_to_uc(inpix->data.bgra.ui.B);
             break;

           case GAN_USHORT:
             outpix->data.bgr.us.R = gan_pixel_ui_to_us(inpix->data.bgra.ui.R);
             outpix->data.bgr.us.G = gan_pixel_ui_to_us(inpix->data.bgra.ui.G);
             outpix->data.bgr.us.B = gan_pixel_ui_to_us(inpix->data.bgra.ui.B);
             break;

           case GAN_UINT:
             outpix->data.bgr.ui.R = inpix->data.bgra.ui.R;
             outpix->data.bgr.ui.G = inpix->data.bgra.ui.G;
             outpix->data.bgr.ui.B = inpix->data.bgra.ui.B;
             break;

           case GAN_FLOAT:
             outpix->data.bgr.f.R = gan_pixel_ui_to_f(inpix->data.bgra.ui.R);
             outpix->data.bgr.f.G = gan_pixel_ui_to_f(inpix->data.bgra.ui.G);
             outpix->data.bgr.f.B = gan_pixel_ui_to_f(inpix->data.bgra.ui.B);
             break;

           case GAN_DOUBLE:
             outpix->data.bgr.d.R = gan_pixel_ui_to_d(inpix->data.bgra.ui.R);
             outpix->data.bgr.d.G = gan_pixel_ui_to_d(inpix->data.bgra.ui.G);
             outpix->data.bgr.d.B = gan_pixel_ui_to_d(inpix->data.bgra.ui.B);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      default:
        if ( error_code == NULL )
        {
           gan_err_flush_trace();
           gan_err_register ( "convert_pixel_bgra_bgr", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        }
        else
           *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_bgra_bgra ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_BGR_COLOUR_ALPHA_IMAGE )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_bgra_bgra", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = gan_pixel_d_to_uc(inpix->data.bgra.d.R);
             outpix->data.bgra.uc.G = gan_pixel_d_to_uc(inpix->data.bgra.d.G);
             outpix->data.bgra.uc.B = gan_pixel_d_to_uc(inpix->data.bgra.d.B);
             outpix->data.bgra.uc.A = gan_pixel_d_to_uc(inpix->data.bgra.d.A);
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = gan_pixel_d_to_us(inpix->data.bgra.d.R);
             outpix->data.bgra.us.G = gan_pixel_d_to_us(inpix->data.bgra.d.G);
             outpix->data.bgra.us.B = gan_pixel_d_to_us(inpix->data.bgra.d.B);
             outpix->data.bgra.us.A = gan_pixel_d_to_us(inpix->data.bgra.d.A);
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = gan_pixel_d_to_ui(inpix->data.bgra.d.R);
             outpix->data.bgra.ui.G = gan_pixel_d_to_ui(inpix->data.bgra.d.G);
             outpix->data.bgra.ui.B = gan_pixel_d_to_ui(inpix->data.bgra.d.B);
             outpix->data.bgra.ui.A = gan_pixel_d_to_ui(inpix->data.bgra.d.A);
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = (float)inpix->data.bgra.d.R;
             outpix->data.bgra.f.G = (float)inpix->data.bgra.d.G;
             outpix->data.bgra.f.B = (float)inpix->data.bgra.d.B;
             outpix->data.bgra.f.A = (float)inpix->data.bgra.d.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R = gan_pixel_f_to_uc(inpix->data.bgra.f.R);
             outpix->data.bgra.uc.G = gan_pixel_f_to_uc(inpix->data.bgra.f.G);
             outpix->data.bgra.uc.B = gan_pixel_f_to_uc(inpix->data.bgra.f.B);
             outpix->data.bgra.uc.A = gan_pixel_f_to_uc(inpix->data.bgra.f.A);
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R = gan_pixel_f_to_us(inpix->data.bgra.f.R);
             outpix->data.bgra.us.G = gan_pixel_f_to_us(inpix->data.bgra.f.G);
             outpix->data.bgra.us.B = gan_pixel_f_to_us(inpix->data.bgra.f.B);
             outpix->data.bgra.us.A = gan_pixel_f_to_us(inpix->data.bgra.f.A);
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R = gan_pixel_f_to_ui(inpix->data.bgra.f.R);
             outpix->data.bgra.ui.G = gan_pixel_f_to_ui(inpix->data.bgra.f.G);
             outpix->data.bgra.ui.B = gan_pixel_f_to_ui(inpix->data.bgra.f.B);
             outpix->data.bgra.ui.A = gan_pixel_f_to_ui(inpix->data.bgra.f.A);
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = (double)inpix->data.bgra.f.R;
             outpix->data.bgra.d.G = (double)inpix->data.bgra.f.G;
             outpix->data.bgra.d.B = (double)inpix->data.bgra.f.B;
             outpix->data.bgra.d.A = (double)inpix->data.bgra.f.A;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_UCHAR:
        switch ( outpix->type )
        {
           case GAN_USHORT:
             outpix->data.bgra.us.R =gan_pixel_uc_to_us(inpix->data.bgra.uc.R);
             outpix->data.bgra.us.G =gan_pixel_uc_to_us(inpix->data.bgra.uc.G);
             outpix->data.bgra.us.B =gan_pixel_uc_to_us(inpix->data.bgra.uc.B);
             outpix->data.bgra.us.A =gan_pixel_uc_to_us(inpix->data.bgra.uc.A);
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R =gan_pixel_uc_to_ui(inpix->data.bgra.uc.R);
             outpix->data.bgra.ui.G =gan_pixel_uc_to_ui(inpix->data.bgra.uc.G);
             outpix->data.bgra.ui.B =gan_pixel_uc_to_ui(inpix->data.bgra.uc.B);
             outpix->data.bgra.ui.A =gan_pixel_uc_to_ui(inpix->data.bgra.uc.A);
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = gan_pixel_uc_to_f(inpix->data.bgra.uc.R);
             outpix->data.bgra.f.G = gan_pixel_uc_to_f(inpix->data.bgra.uc.G);
             outpix->data.bgra.f.B = gan_pixel_uc_to_f(inpix->data.bgra.uc.B);
             outpix->data.bgra.f.A = gan_pixel_uc_to_f(inpix->data.bgra.uc.A);
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = gan_pixel_uc_to_d(inpix->data.bgra.uc.R);
             outpix->data.bgra.d.G = gan_pixel_uc_to_d(inpix->data.bgra.uc.G);
             outpix->data.bgra.d.B = gan_pixel_uc_to_d(inpix->data.bgra.uc.B);
             outpix->data.bgra.d.A = gan_pixel_uc_to_d(inpix->data.bgra.uc.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_USHORT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R =gan_pixel_us_to_uc(inpix->data.bgra.us.R);
             outpix->data.bgra.uc.G =gan_pixel_us_to_uc(inpix->data.bgra.us.G);
             outpix->data.bgra.uc.B =gan_pixel_us_to_uc(inpix->data.bgra.us.B);
             outpix->data.bgra.uc.A =gan_pixel_us_to_uc(inpix->data.bgra.us.A);
             break;

           case GAN_UINT:
             outpix->data.bgra.ui.R =gan_pixel_us_to_ui(inpix->data.bgra.us.R);
             outpix->data.bgra.ui.G =gan_pixel_us_to_ui(inpix->data.bgra.us.G);
             outpix->data.bgra.ui.B =gan_pixel_us_to_ui(inpix->data.bgra.us.B);
             outpix->data.bgra.ui.A =gan_pixel_us_to_ui(inpix->data.bgra.us.A);
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = gan_pixel_us_to_d(inpix->data.bgra.us.R);
             outpix->data.bgra.d.G = gan_pixel_us_to_d(inpix->data.bgra.us.G);
             outpix->data.bgra.d.B = gan_pixel_us_to_d(inpix->data.bgra.us.B);
             outpix->data.bgra.d.A = gan_pixel_us_to_d(inpix->data.bgra.us.A);
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = gan_pixel_us_to_f(inpix->data.bgra.us.R);
             outpix->data.bgra.f.G = gan_pixel_us_to_f(inpix->data.bgra.us.G);
             outpix->data.bgra.f.B = gan_pixel_us_to_f(inpix->data.bgra.us.B);
             outpix->data.bgra.f.A = gan_pixel_us_to_f(inpix->data.bgra.us.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_INT:
        switch ( outpix->type )
        {
           case GAN_FLOAT:
             outpix->data.bgra.f.R = gan_pixel_i_to_f(inpix->data.bgra.i.R);
             outpix->data.bgra.f.G = gan_pixel_i_to_f(inpix->data.bgra.i.G);
             outpix->data.bgra.f.B = gan_pixel_i_to_f(inpix->data.bgra.i.B);
             outpix->data.bgra.f.A = gan_pixel_i_to_f(inpix->data.bgra.i.A);
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = gan_pixel_i_to_d(inpix->data.bgra.i.R);
             outpix->data.bgra.d.G = gan_pixel_i_to_d(inpix->data.bgra.i.G);
             outpix->data.bgra.d.B = gan_pixel_i_to_d(inpix->data.bgra.i.B);
             outpix->data.bgra.d.A = gan_pixel_i_to_d(inpix->data.bgra.i.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_UINT:
        switch ( outpix->type )
        {
           case GAN_UCHAR:
             outpix->data.bgra.uc.R =gan_pixel_ui_to_uc(inpix->data.bgra.ui.R);
             outpix->data.bgra.uc.G =gan_pixel_ui_to_uc(inpix->data.bgra.ui.G);
             outpix->data.bgra.uc.B =gan_pixel_ui_to_uc(inpix->data.bgra.ui.B);
             outpix->data.bgra.uc.A =gan_pixel_ui_to_uc(inpix->data.bgra.ui.A);
             break;

           case GAN_USHORT:
             outpix->data.bgra.us.R =gan_pixel_ui_to_us(inpix->data.bgra.ui.R);
             outpix->data.bgra.us.G =gan_pixel_ui_to_us(inpix->data.bgra.ui.G);
             outpix->data.bgra.us.B =gan_pixel_ui_to_us(inpix->data.bgra.ui.B);
             outpix->data.bgra.us.A =gan_pixel_ui_to_us(inpix->data.bgra.ui.A);
             break;

           case GAN_FLOAT:
             outpix->data.bgra.f.R = gan_pixel_ui_to_f(inpix->data.bgra.ui.R);
             outpix->data.bgra.f.G = gan_pixel_ui_to_f(inpix->data.bgra.ui.G);
             outpix->data.bgra.f.B = gan_pixel_ui_to_f(inpix->data.bgra.ui.B);
             outpix->data.bgra.f.A = gan_pixel_ui_to_f(inpix->data.bgra.ui.A);
             break;

           case GAN_DOUBLE:
             outpix->data.bgra.d.R = gan_pixel_ui_to_d(inpix->data.bgra.ui.R);
             outpix->data.bgra.d.G = gan_pixel_ui_to_d(inpix->data.bgra.ui.G);
             outpix->data.bgra.d.B = gan_pixel_ui_to_d(inpix->data.bgra.ui.B);
             outpix->data.bgra.d.A = gan_pixel_ui_to_d(inpix->data.bgra.ui.A);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_bgra_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      default:
        if ( error_code == NULL )
        {
           gan_err_flush_trace();
           gan_err_register ( "convert_pixel_bgra_bgra", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        }
        else
           *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 convert_pixel_vfield2D_vfield2D ( Gan_Pixel *inpix, Gan_Pixel *outpix, int *error_code )
{
   /* consistency check */
#ifndef NDEBUG
   if ( inpix->format != GAN_VECTOR_FIELD_2D )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "convert_pixel_vfield2D_vfield2D",
                            GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      }
      else
         *error_code = GAN_ERROR_ILLEGAL_ARGUMENT;

      return GAN_FALSE;
   }
#endif

   switch ( inpix->type )
   {
      case GAN_DOUBLE:
        switch ( outpix->type )
        {
           case GAN_FLOAT:
             outpix->data.vfield2D.f.x = (float)inpix->data.vfield2D.d.x;
             outpix->data.vfield2D.f.y = (float)inpix->data.vfield2D.d.y;
             break;

           case GAN_SHORT:
             outpix->data.vfield2D.s.x =
                                    gan_pixel_d_to_s(inpix->data.vfield2D.d.x);
             outpix->data.vfield2D.s.y =
                                    gan_pixel_d_to_s(inpix->data.vfield2D.d.y);
             break;

           case GAN_INT:
             outpix->data.vfield2D.i.x =
                                    gan_pixel_d_to_i(inpix->data.vfield2D.d.x);
             outpix->data.vfield2D.i.y =
                                    gan_pixel_d_to_i(inpix->data.vfield2D.d.y);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_vfield2D_vfield2D",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_FLOAT:
        switch ( outpix->type )
        {
           case GAN_SHORT:
             outpix->data.vfield2D.s.x =
                                    gan_pixel_f_to_s(inpix->data.vfield2D.f.x);
             outpix->data.vfield2D.s.y =
                                    gan_pixel_f_to_s(inpix->data.vfield2D.f.y);
             break;

           case GAN_INT:
             outpix->data.vfield2D.i.x =
                                    gan_pixel_f_to_i(inpix->data.vfield2D.f.x);
             outpix->data.vfield2D.i.y =
                                    gan_pixel_f_to_i(inpix->data.vfield2D.f.y);
             break;

           case GAN_DOUBLE:
             outpix->data.vfield2D.d.x = (double)inpix->data.vfield2D.f.x;
             outpix->data.vfield2D.d.y = (double)inpix->data.vfield2D.f.y;
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_vfield2D_vfield2D",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;
        
      case GAN_SHORT:
        switch ( outpix->type )
        {
           case GAN_INT:
             outpix->data.vfield2D.i.x =
                                   gan_pixel_s_to_i(inpix->data.vfield2D.s.x);
             outpix->data.vfield2D.i.y =
                                   gan_pixel_s_to_i(inpix->data.vfield2D.s.y);
             break;

           case GAN_DOUBLE:
             outpix->data.vfield2D.d.x =
                                   gan_pixel_s_to_d(inpix->data.vfield2D.s.x);
             outpix->data.vfield2D.d.y =
                                   gan_pixel_s_to_d(inpix->data.vfield2D.s.y);
             break;

           case GAN_FLOAT:
             outpix->data.vfield2D.f.x =
                                   gan_pixel_s_to_f(inpix->data.vfield2D.s.x);
             outpix->data.vfield2D.f.y =
                                   gan_pixel_s_to_f(inpix->data.vfield2D.s.y);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_vfield2D_vfield2D",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_INT:
        switch ( outpix->type )
        {
           case GAN_SHORT:
             outpix->data.vfield2D.s.x =
                                    gan_pixel_i_to_s(inpix->data.vfield2D.i.x);
             outpix->data.vfield2D.s.y =
                                    gan_pixel_i_to_s(inpix->data.vfield2D.i.y);
             break;

           case GAN_DOUBLE:
             outpix->data.vfield2D.d.x =
                                    gan_pixel_i_to_d(inpix->data.vfield2D.i.x);
             outpix->data.vfield2D.d.y =
                                    gan_pixel_i_to_d(inpix->data.vfield2D.i.y);
             break;

           case GAN_FLOAT:
             outpix->data.vfield2D.f.x =
                                    gan_pixel_i_to_f(inpix->data.vfield2D.i.x);
             outpix->data.vfield2D.f.y =
                                    gan_pixel_i_to_f(inpix->data.vfield2D.i.y);
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "convert_pixel_vfield2D_vfield2D",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

        default:
          if ( error_code == NULL )
          {
             gan_err_flush_trace();
             gan_err_register ( "convert_pixel_vfield2D_vfield2D",
                                GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
          }
          else
             *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Convert a pixel to the given format and type.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Convert pixel \a inpix to the given \a format and \a type, writing the
 * result into pixel \a outpix. If \a error_code is not \c NULL, an error
 * condition is returned in the \a error_code pointer; otherwise \a error_code
 * is \c NULL and the Gandalf error handler gan_err_register() is invoked.
 * In either case #GAN_FALSE is returned.
 */
Gan_Bool
 gan_image_convert_pixel ( Gan_Pixel *inpix,
                           Gan_ImageFormat format, Gan_Type type,
                           Gan_Pixel *outpix, int *error_code )
{
   Gan_Pixel pixel;
   Gan_Bool result;

   /* if format & type are unchanged, return pixel unchanged */
   if ( inpix->format == format && inpix->type == type )
   {
      *outpix = *inpix;
      return GAN_TRUE;
   }
      
   /* set format and type of output image */
   pixel.format = format;
   pixel.type   = type;

   switch ( inpix->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( format )
        {
           case GAN_GREY_LEVEL_IMAGE:
             result = convert_pixel_gl_gl ( inpix, &pixel, error_code );
             break;

           case GAN_GREY_LEVEL_ALPHA_IMAGE:
             result = convert_pixel_gl_gla ( inpix, &pixel, error_code );
             break;

           case GAN_RGB_COLOUR_IMAGE:
             result = convert_pixel_gl_rgb ( inpix, &pixel, error_code );
             break;

           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             result = convert_pixel_gl_rgba ( inpix, &pixel, error_code );
             break;

           case GAN_BGR_COLOUR_IMAGE:
             result = convert_pixel_gl_bgr ( inpix, &pixel, error_code );
             break;

           case GAN_BGR_COLOUR_ALPHA_IMAGE:
             result = convert_pixel_gl_bgra ( inpix, &pixel, error_code );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "gan_image_convert_pixel", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( format )
        {
           case GAN_GREY_LEVEL_IMAGE:
             result = convert_pixel_gla_gl ( inpix, &pixel, error_code );
             break;

           case GAN_GREY_LEVEL_ALPHA_IMAGE:
             result = convert_pixel_gla_gla ( inpix, &pixel, error_code );
             break;

           case GAN_RGB_COLOUR_IMAGE:
             result = convert_pixel_gla_rgb ( inpix, &pixel, error_code );
             break;

           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             result = convert_pixel_gla_rgba ( inpix, &pixel, error_code );
             break;

           case GAN_BGR_COLOUR_IMAGE:
             result = convert_pixel_gla_bgr ( inpix, &pixel, error_code );
             break;

           case GAN_BGR_COLOUR_ALPHA_IMAGE:
             result = convert_pixel_gla_bgra ( inpix, &pixel, error_code );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "gan_image_convert_pixel", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( format )
        {
           case GAN_GREY_LEVEL_IMAGE:
             result = convert_pixel_rgb_gl ( inpix, &pixel, error_code );
             break;

           case GAN_GREY_LEVEL_ALPHA_IMAGE:
             result = convert_pixel_rgb_gla ( inpix, &pixel, error_code );
             break;

           case GAN_RGB_COLOUR_IMAGE:
             result = convert_pixel_rgb_rgb ( inpix, &pixel, error_code );
             break;

           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             result = convert_pixel_rgb_rgba ( inpix, &pixel, error_code );
             break;

           case GAN_VECTOR_FIELD_2D:
             result = convert_pixel_rgb_vfield2D ( inpix, &pixel, error_code );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "gan_image_convert_pixel", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( format )
        {
           case GAN_GREY_LEVEL_IMAGE:
             result = convert_pixel_rgba_gl ( inpix, &pixel, error_code );
             break;

           case GAN_GREY_LEVEL_ALPHA_IMAGE:
             result = convert_pixel_rgba_gla ( inpix, &pixel, error_code );
             break;

           case GAN_RGB_COLOUR_IMAGE:
             result = convert_pixel_rgba_rgb ( inpix, &pixel, error_code );
             break;

           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             result = convert_pixel_rgba_rgba ( inpix, &pixel, error_code );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "gan_image_convert_pixel", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( format )
        {
           case GAN_GREY_LEVEL_IMAGE:
             result = convert_pixel_bgr_gl ( inpix, &pixel, error_code );
             break;

           case GAN_GREY_LEVEL_ALPHA_IMAGE:
             result = convert_pixel_bgr_gla ( inpix, &pixel, error_code );
             break;

           case GAN_BGR_COLOUR_IMAGE:
             result = convert_pixel_bgr_bgr ( inpix, &pixel, error_code );
             break;

           case GAN_BGR_COLOUR_ALPHA_IMAGE:
             result = convert_pixel_bgr_bgra ( inpix, &pixel, error_code );
             break;

           case GAN_VECTOR_FIELD_2D:
             result = convert_pixel_bgr_vfield2D ( inpix, &pixel, error_code );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "gan_image_convert_pixel", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( format )
        {
           case GAN_GREY_LEVEL_IMAGE:
             result = convert_pixel_bgra_gl ( inpix, &pixel, error_code );
             break;

           case GAN_GREY_LEVEL_ALPHA_IMAGE:
             result = convert_pixel_bgra_gla ( inpix, &pixel, error_code );
             break;

           case GAN_BGR_COLOUR_IMAGE:
             result = convert_pixel_bgra_bgr ( inpix, &pixel, error_code );
             break;

           case GAN_BGR_COLOUR_ALPHA_IMAGE:
             result = convert_pixel_bgra_bgra ( inpix, &pixel, error_code );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "gan_image_convert_pixel",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( format )
        {
           case GAN_VECTOR_FIELD_2D:
             result = convert_pixel_vfield2D_vfield2D ( inpix, &pixel,
                                                        error_code );
             break;

           default:
             if ( error_code == NULL )
             {
                gan_err_flush_trace();
                gan_err_register ( "gan_image_convert_pixel",
                                   GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             }
             else
                *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;

             return GAN_FALSE;
        }
        break;

      default:
        if ( error_code == NULL )
        {
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convert_pixel",
                              GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        }
        else
           *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;

        return GAN_FALSE;
   }
   
   if ( !result )
   {
      if ( error_code == NULL )
         gan_err_register ( "gan_image_convert_pixel", GAN_ERROR_FAILURE, "" );

      return GAN_FALSE;
   }

   /* success */
   *outpix = pixel;
   return GAN_TRUE;
}

/**
 * \brief Return the scaling involved in converting a pixel from one type to another.
 */
double
 gan_image_convert_scale ( Gan_Type gtFromType, Gan_Type gtToType )
{
   /* check trivial case first */
   if ( gtFromType == gtToType ) return 1.0;

   switch ( gtFromType )
   {
      case GAN_UCHAR:
        switch ( gtToType )
        {
           case GAN_USHORT: return GAN_USHRT_MAXD/GAN_UCHAR_MAXD;
           case GAN_UINT:   return GAN_UINT_MAXD/GAN_UCHAR_MAXD;
           case GAN_FLOAT:
           case GAN_DOUBLE: return 1.0/GAN_UCHAR_MAXD;
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_convert_scale",
                                GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return DBL_MAX;
        }
        break;

      case GAN_USHORT:
        switch ( gtToType )
        {
           case GAN_UCHAR:  return GAN_UCHAR_MAXD/GAN_USHRT_MAXD;
           case GAN_UINT: return GAN_UINT_MAXD/GAN_USHRT_MAXD;
           case GAN_FLOAT:
           case GAN_DOUBLE: return 1.0/GAN_USHRT_MAXD;
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_convert_scale",
                                GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return DBL_MAX;
        }
        break;

      case GAN_UINT:
        switch ( gtToType )
        {
           case GAN_DOUBLE: return 1.0/GAN_UINT_MAXD;
           case GAN_UCHAR:  return GAN_UCHAR_MAXD/GAN_UINT_MAXD;
           case GAN_USHORT: return GAN_USHRT_MAXD/GAN_UINT_MAXD;
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_convert_scale",
                                GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return DBL_MAX;
        }
        break;

      case GAN_FLOAT:
        switch ( gtToType )
        {
           case GAN_UCHAR:  return GAN_UCHAR_MAXD;
           case GAN_UINT:   return GAN_UINT_MAXD;
           case GAN_DOUBLE: return 1.0;
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_convert_scale",
                                GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return DBL_MAX;
        }
        break;

      case GAN_DOUBLE:
        switch ( gtToType )
        {
           case GAN_UCHAR: return GAN_UCHAR_MAXD;
           case GAN_UINT:  return GAN_UINT_MAXD;
           case GAN_FLOAT: return 1.0;
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_convert_scale",
                                GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return DBL_MAX;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_convert_scale",
                           GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return DBL_MAX;
   }

   /* shouldn't get here */
   return DBL_MAX;
}

/**
 * \brief Offset pixel values.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Add each channel of \a pixel to the given \a offset factor, writing
 * the result into \a offset_pixel.
 */
Gan_Bool
 gan_image_offset_pixel_q ( Gan_Pixel *pixel, double offset,
                            Gan_Pixel *offset_pixel )
{
   Gan_Pixel new_pixel;

   if ( !gan_image_convert_pixel_q ( pixel, pixel->format, GAN_DOUBLE,
                                     &new_pixel ) )
   {
      gan_err_register ( "gan_image_offset_pixel_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   switch ( pixel->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        new_pixel.data.gl.d += offset;
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        new_pixel.data.gla.d.I += offset;
        break;

      case GAN_RGB_COLOUR_IMAGE:
        new_pixel.data.rgb.d.R += offset;
        new_pixel.data.rgb.d.G += offset;
        new_pixel.data.rgb.d.B += offset;
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        new_pixel.data.rgba.d.R += offset;
        new_pixel.data.rgba.d.G += offset;
        new_pixel.data.rgba.d.B += offset;
        break;

      case GAN_BGR_COLOUR_IMAGE:
        new_pixel.data.bgr.d.R += offset;
        new_pixel.data.bgr.d.G += offset;
        new_pixel.data.bgr.d.B += offset;
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        new_pixel.data.bgra.d.R += offset;
        new_pixel.data.bgra.d.G += offset;
        new_pixel.data.bgra.d.B += offset;
        break;

      case GAN_VECTOR_FIELD_2D:
        new_pixel.data.vfield2D.d.x += offset;
        new_pixel.data.vfield2D.d.y += offset;
        break;

      case GAN_VECTOR_FIELD_3D:
        new_pixel.data.vfield3D.d.x += offset;
        new_pixel.data.vfield3D.d.y += offset;
        new_pixel.data.vfield3D.d.z += offset;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_offset_pixel_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT,
                           "image format" );
        return GAN_FALSE;
   }

   if ( !gan_image_convert_pixel_q ( &new_pixel, pixel->format, pixel->type,
                                     offset_pixel) )
   {
      gan_err_register ( "gan_image_offset_pixel_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Scale pixel values.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Multiply each channel of \a pixel by the given \a scale factor, writing
 * the result into \a scaled_pixel.
 */
Gan_Bool
 gan_image_scale_pixel_q ( Gan_Pixel *pixel, double scale,
                           Gan_Pixel *scaled_pixel )
{
   Gan_Pixel new_pixel;

   if ( !gan_image_convert_pixel_q ( pixel, pixel->format, GAN_DOUBLE,
                                     &new_pixel ) )
   {
      gan_err_register ( "gan_image_scale_pixel_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   switch ( pixel->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        new_pixel.data.gl.d *= scale;
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        new_pixel.data.gla.d.I *= scale;
        break;

      case GAN_RGB_COLOUR_IMAGE:
        new_pixel.data.rgb.d.R *= scale;
        new_pixel.data.rgb.d.G *= scale;
        new_pixel.data.rgb.d.B *= scale;
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        new_pixel.data.rgba.d.R *= scale;
        new_pixel.data.rgba.d.G *= scale;
        new_pixel.data.rgba.d.B *= scale;
        break;

      case GAN_BGR_COLOUR_IMAGE:
        new_pixel.data.bgr.d.R *= scale;
        new_pixel.data.bgr.d.G *= scale;
        new_pixel.data.bgr.d.B *= scale;
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        new_pixel.data.bgra.d.R *= scale;
        new_pixel.data.bgra.d.G *= scale;
        new_pixel.data.bgra.d.B *= scale;
        break;

      case GAN_VECTOR_FIELD_2D:
        (void)gan_vec2_scale_i ( &new_pixel.data.vfield2D.d, scale );
        break;

      case GAN_VECTOR_FIELD_3D:
        (void)gan_vec3_scale_i ( &new_pixel.data.vfield3D.d, scale );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_scale_pixel_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT,
                           "image format" );
        return GAN_FALSE;
   }

   if ( !gan_image_convert_pixel_q ( &new_pixel, pixel->format, pixel->type,
                                     scaled_pixel) )
   {
      gan_err_register ( "gan_image_scale_pixel_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Divide pixel values by scalar.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Divide each channel of \a pixel by the given \a scale factor, writing
 * the result into \a scaled_pixel.
 */
Gan_Bool
 gan_image_divide_pixel_q ( Gan_Pixel *pixel, double scale,
                            Gan_Pixel *scaled_pixel )
{
   Gan_Pixel new_pixel;

   if ( scale == 0.0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_pixel_divide_q",
                         GAN_ERROR_DIVISION_BY_ZERO, "" );
      return GAN_FALSE;
   }

   if ( !gan_image_convert_pixel_q ( pixel, pixel->format, GAN_DOUBLE,
                                     &new_pixel ) )
   {
      gan_err_register ( "gan_image_divide_pixel_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   switch ( pixel->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        new_pixel.data.gl.d /= scale;
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        new_pixel.data.gla.d.I /= scale;
        break;

      case GAN_RGB_COLOUR_IMAGE:
        new_pixel.data.rgb.d.R /= scale;
        new_pixel.data.rgb.d.G /= scale;
        new_pixel.data.rgb.d.B /= scale;
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        new_pixel.data.rgba.d.R /= scale;
        new_pixel.data.rgba.d.G /= scale;
        new_pixel.data.rgba.d.B /= scale;
        break;

      case GAN_BGR_COLOUR_IMAGE:
        new_pixel.data.bgr.d.R /= scale;
        new_pixel.data.bgr.d.G /= scale;
        new_pixel.data.bgr.d.B /= scale;
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        new_pixel.data.bgra.d.R /= scale;
        new_pixel.data.bgra.d.G /= scale;
        new_pixel.data.bgra.d.B /= scale;
        break;

      case GAN_VECTOR_FIELD_2D:
        (void)gan_vec2_divide_i ( &new_pixel.data.vfield2D.d, scale );
        break;

      case GAN_VECTOR_FIELD_3D:
        (void)gan_vec3_divide_i ( &new_pixel.data.vfield3D.d, scale );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_divide_pixel_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT,
                           "image format" );
        return GAN_FALSE;
   }

   if ( !gan_image_convert_pixel_q ( &new_pixel, pixel->format, pixel->type,
                                     scaled_pixel) )
   {
      gan_err_register ( "gan_image_divide_pixel_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Fill a pixel with zero value(s).
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * You should set the format and type of the pixel before calling this
 * function.
 */
Gan_Bool
 gan_image_fill_zero_pixel ( Gan_Pixel *pixel )
{
   switch ( pixel->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
      switch ( pixel->type )
      {
         case GAN_UCHAR:   pixel->data.gl.uc = 0;    break;
         case GAN_SHORT:   pixel->data.gl.s  = 0;    break;
         case GAN_USHORT:  pixel->data.gl.us = 0;    break;
         case GAN_INT:     pixel->data.gl.i  = 0;    break;
         case GAN_UINT:    pixel->data.gl.ui = 0;    break;
         case GAN_FLOAT:   pixel->data.gl.f  = 0.0F; break;
         case GAN_DOUBLE:  pixel->data.gl.d  = 0.0;  break;
         case GAN_POINTER: pixel->data.gl.p  = NULL; break;
         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_fill_zero_pixel",
                              GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           return GAN_FALSE;
      }
      break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
      switch ( pixel->type )
      {
         case GAN_UCHAR:
           pixel->data.gla.uc.I = pixel->data.gla.uc.A = 0; break;

         case GAN_SHORT:
           pixel->data.gla.s.I = pixel->data.gla.s.A = 0; break;

         case GAN_USHORT:
           pixel->data.gla.us.I = pixel->data.gla.us.A = 0; break;

         case GAN_INT:
           pixel->data.gla.i.I = pixel->data.gla.i.A = 0; break;

         case GAN_UINT:
           pixel->data.gla.ui.I = pixel->data.gla.ui.A = 0; break;

         case GAN_FLOAT:
           pixel->data.gla.f.I= pixel->data.gla.f.A = 0.0F; break;

         case GAN_DOUBLE:
           pixel->data.gla.d.I = pixel->data.gla.d.A = 0.0; break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_fill_zero_pixel",
                              GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           return GAN_FALSE;
      }
      break;

      case GAN_RGB_COLOUR_IMAGE:
      switch ( pixel->type )
      {
         case GAN_UCHAR:
           pixel->data.rgb.uc.R = pixel->data.rgb.uc.G =
           pixel->data.rgb.uc.B = 0; break;

         case GAN_SHORT:
           pixel->data.rgb.s.R = pixel->data.rgb.s.G =
           pixel->data.rgb.s.B = 0; break;

         case GAN_USHORT:
           pixel->data.rgb.us.R = pixel->data.rgb.us.G =
           pixel->data.rgb.us.B = 0; break;

         case GAN_INT:
           pixel->data.rgb.i.R = pixel->data.rgb.i.G =
           pixel->data.rgb.i.B = 0; break;

         case GAN_UINT:
           pixel->data.rgb.ui.R = pixel->data.rgb.ui.G =
           pixel->data.rgb.ui.B = 0; break;

         case GAN_FLOAT:
           pixel->data.rgb.f.R = pixel->data.rgb.f.G =
           pixel->data.rgb.f.B = 0.0F; break;

         case GAN_DOUBLE:
           pixel->data.rgb.d.R = pixel->data.rgb.d.G =
           pixel->data.rgb.d.B = 0.0; break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_fill_zero_pixel",
                              GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           return GAN_FALSE;
      }
      break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
      switch ( pixel->type )
      {
         case GAN_UCHAR:
           pixel->data.rgba.uc.R = pixel->data.rgba.uc.G =
           pixel->data.rgba.uc.B = pixel->data.rgba.uc.A = 0; break;

         case GAN_SHORT:
           pixel->data.rgba.s.R = pixel->data.rgba.s.G =
           pixel->data.rgba.s.B = pixel->data.rgba.s.A = 0; break;

         case GAN_USHORT:
           pixel->data.rgba.us.R = pixel->data.rgba.us.G =
           pixel->data.rgba.us.B = pixel->data.rgba.us.A = 0; break;

         case GAN_INT:
           pixel->data.rgba.i.R = pixel->data.rgba.i.G =
           pixel->data.rgba.i.B = pixel->data.rgba.i.A = 0; break;

         case GAN_UINT:
           pixel->data.rgba.ui.R = pixel->data.rgba.ui.G =
           pixel->data.rgba.ui.B = pixel->data.rgba.ui.A = 0; break;

         case GAN_FLOAT:
           pixel->data.rgba.f.R = pixel->data.rgba.f.G =
           pixel->data.rgba.f.B = pixel->data.rgba.f.A = 0.0F; break;

         case GAN_DOUBLE:
           pixel->data.rgba.d.R = pixel->data.rgba.d.G =
           pixel->data.rgba.d.B = pixel->data.rgba.d.A = 0.0; break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_fill_zero_pixel",
                              GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           return GAN_FALSE;
      }
      break;

      case GAN_BGR_COLOUR_IMAGE:
      switch ( pixel->type )
      {
         case GAN_UCHAR:
           pixel->data.bgr.uc.R = pixel->data.bgr.uc.G =
           pixel->data.bgr.uc.B = 0; break;

         case GAN_SHORT:
           pixel->data.bgr.s.R = pixel->data.bgr.s.G =
           pixel->data.bgr.s.B = 0; break;

         case GAN_USHORT:
           pixel->data.bgr.us.R = pixel->data.bgr.us.G =
           pixel->data.bgr.us.B = 0; break;

         case GAN_INT:
           pixel->data.bgr.i.R = pixel->data.bgr.i.G =
           pixel->data.bgr.i.B = 0; break;

         case GAN_UINT:
           pixel->data.bgr.ui.R = pixel->data.bgr.ui.G =
           pixel->data.bgr.ui.B = 0; break;

         case GAN_FLOAT:
           pixel->data.bgr.f.R = pixel->data.bgr.f.G =
           pixel->data.bgr.f.B = 0.0F; break;

         case GAN_DOUBLE:
           pixel->data.bgr.d.R = pixel->data.bgr.d.G =
           pixel->data.bgr.d.B = 0.0; break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_fill_zero_pixel",
                              GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           return GAN_FALSE;
      }
      break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
      switch ( pixel->type )
      {
         case GAN_UCHAR:
           pixel->data.bgra.uc.R = pixel->data.bgra.uc.G =
           pixel->data.bgra.uc.B = pixel->data.bgra.uc.A = 0; break;

         case GAN_SHORT:
           pixel->data.bgra.s.R = pixel->data.bgra.s.G =
           pixel->data.bgra.s.B = pixel->data.bgra.s.A = 0; break;

         case GAN_USHORT:
           pixel->data.bgra.us.R = pixel->data.bgra.us.G =
           pixel->data.bgra.us.B = pixel->data.bgra.us.A = 0; break;

         case GAN_INT:
           pixel->data.bgra.i.R = pixel->data.bgra.i.G =
           pixel->data.bgra.i.B = pixel->data.bgra.i.A = 0; break;

         case GAN_UINT:
           pixel->data.bgra.ui.R = pixel->data.bgra.ui.G =
           pixel->data.bgra.ui.B = pixel->data.bgra.ui.A = 0; break;

         case GAN_FLOAT:
           pixel->data.bgra.f.R = pixel->data.bgra.f.G =
           pixel->data.bgra.f.B = pixel->data.bgra.f.A = 0.0F; break;

         case GAN_DOUBLE:
           pixel->data.bgra.d.R = pixel->data.bgra.d.G =
           pixel->data.bgra.d.B = pixel->data.bgra.d.A = 0.0; break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_fill_zero_pixel",
                              GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           return GAN_FALSE;
      }
      break;

      case GAN_VECTOR_FIELD_2D:
      switch ( pixel->type )
      {
         case GAN_FLOAT:
           (void)gan_vec2f_zero_q ( &pixel->data.vfield2D.f ); break;

         case GAN_DOUBLE:
           (void)gan_vec2_zero_q ( &pixel->data.vfield2D.d ); break;

         case GAN_SHORT:
           pixel->data.vfield2D.s.x = pixel->data.vfield2D.s.y = 0; break;

         case GAN_INT:
           pixel->data.vfield2D.i.x = pixel->data.vfield2D.i.y = 0; break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_fill_zero_pixel",
                              GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           return GAN_FALSE;
      }
      break;

      case GAN_VECTOR_FIELD_3D:
      switch ( pixel->type )
      {
         case GAN_FLOAT:
           (void)gan_vec3f_zero_q ( &pixel->data.vfield3D.f ); break;

         case GAN_DOUBLE:
           (void)gan_vec3_zero_q ( &pixel->data.vfield3D.d ); break;

         case GAN_SHORT:
           pixel->data.vfield3D.s.x = pixel->data.vfield3D.s.y =
           pixel->data.vfield3D.s.z = 0; break;

         case GAN_INT:
           pixel->data.vfield3D.i.x = pixel->data.vfield3D.i.y =
           pixel->data.vfield3D.i.z = 0; break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_fill_zero_pixel",
                              GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           return GAN_FALSE;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_fill_zero_pixel", GAN_ERROR_ILLEGAL_IMAGE_FORMAT,
                           "image format" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Generates a string identifier for an internal image format.
 * \param format Image format, e.g. #GAN_RGB_COLOUR_IMAGE
 * \return String identifier, e.g. "RGB colour", or "" for unknown format.
 */
const char *gan_image_format_string ( Gan_ImageFormat format )
{
   switch(format)
   {
      case GAN_GREY_LEVEL_IMAGE:       return "grey level";                    break;
      case GAN_GREY_LEVEL_ALPHA_IMAGE: return "grey level with alpha channel"; break;
      case GAN_RGB_COLOUR_IMAGE:       return "RGB";                           break;
      case GAN_RGB_COLOUR_ALPHA_IMAGE: return "RGBA";                          break;
      case GAN_BGR_COLOUR_IMAGE:       return "BGR";                           break;
      case GAN_BGR_COLOUR_ALPHA_IMAGE: return "BGRA";                          break;
      case GAN_VECTOR_FIELD_2D:        return "2D vector field";               break;
      case GAN_VECTOR_FIELD_3D:        return "3D vector field";               break;
      case GAN_RGBX:                   return "padded RGB";                    break;
      case GAN_RGBAS:                  return "RGB with small alpha channel";  break;
      case GAN_YUVX444:                return "padded YUV 4-4-4";              break;
      case GAN_YUVA444:                return "YUV with alpha channel";        break;
      case GAN_YUV422:                 return "YUV 4-2-2";                     break;
      default:
        break;
   }

   /* unknown format */
   gan_err_flush_trace();
   gan_err_register ( "gan_image_format_string", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
   return "";
}

/**
 * \brief Returns a pixel format with an alpha channel given an input format
 * \param format Image format, e.g. #GAN_RGB_COLOUR_IMAGE
 * \return Pixel format with alpha channel
 */
Gan_ImageFormat gan_image_format_with_alpha ( Gan_ImageFormat format )
{
   switch(format)
   {
      case GAN_GREY_LEVEL_IMAGE:       return GAN_GREY_LEVEL_ALPHA_IMAGE;  break;
      case GAN_GREY_LEVEL_ALPHA_IMAGE: return GAN_GREY_LEVEL_ALPHA_IMAGE;  break;
      case GAN_RGB_COLOUR_IMAGE:       return GAN_RGB_COLOUR_ALPHA_IMAGE;  break;
      case GAN_RGB_COLOUR_ALPHA_IMAGE: return GAN_RGB_COLOUR_ALPHA_IMAGE;  break;
      case GAN_BGR_COLOUR_IMAGE:       return GAN_BGR_COLOUR_ALPHA_IMAGE;  break;
      case GAN_BGR_COLOUR_ALPHA_IMAGE: return GAN_BGR_COLOUR_ALPHA_IMAGE;  break;
      case GAN_VECTOR_FIELD_2D:        break;
      case GAN_VECTOR_FIELD_3D:        break;
      case GAN_RGBX:                   return GAN_RGBAS;                   break;
      case GAN_RGBAS:                  return GAN_RGBAS;                   break;
      case GAN_YUVX444:                return GAN_YUVA444;                 break;
      case GAN_YUVA444:                return GAN_YUVA444;                 break;
      case GAN_YUV422:                 break;
      default: break;
   }

   /* unknown or inapplicable format */
   gan_err_flush_trace();
   gan_err_register ( "gan_image_format_with_alpha", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
   return GAN_UNDEFINED_IMAGE_FORMAT;
}

/**
 * \brief Returns a pixel format without an alpha channel given an input format
 * \param format Image format, e.g. #GAN_RGB_COLOUR_IMAGE
 * \return Pixel format without alpha channel
 */
Gan_ImageFormat gan_image_format_without_alpha ( Gan_ImageFormat format )
{
   switch(format)
   {
      case GAN_GREY_LEVEL_IMAGE:       return GAN_GREY_LEVEL_IMAGE;  break;
      case GAN_GREY_LEVEL_ALPHA_IMAGE: return GAN_GREY_LEVEL_IMAGE;  break;
      case GAN_RGB_COLOUR_IMAGE:       return GAN_RGB_COLOUR_IMAGE;  break;
      case GAN_RGB_COLOUR_ALPHA_IMAGE: return GAN_RGB_COLOUR_IMAGE;  break;
      case GAN_BGR_COLOUR_IMAGE:       return GAN_BGR_COLOUR_IMAGE;  break;
      case GAN_BGR_COLOUR_ALPHA_IMAGE: return GAN_BGR_COLOUR_IMAGE;  break;
      case GAN_VECTOR_FIELD_2D:        break;
      case GAN_VECTOR_FIELD_3D:        break;
      case GAN_RGBX:                   return GAN_RGBX;              break;
      case GAN_RGBAS:                  return GAN_RGBX;              break;
      case GAN_YUVX444:                return GAN_YUVX444;           break;
      case GAN_YUVA444:                return GAN_YUVX444;           break;
      case GAN_YUV422:                 break;
      default: break;
   }

   /* unknown or inapplicable format */
   gan_err_flush_trace();
   gan_err_register ( "gan_image_format_without_alpha", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
   return GAN_UNDEFINED_IMAGE_FORMAT;
}

/**
 * \brief Returns the number of bits per pixel in an image
 * \param format Image pixel format, e.g. #GAN_RGB_COLOUR_IMAGE
 * \param type Image pixel type, e.g. #GAN_UINT8
 * \return Number of bits per pixel
 */
GANDALF_API unsigned int gan_image_bits_per_pixel ( Gan_ImageFormat format, Gan_Type type )
{
   unsigned int uiBitsPerPixel;

   switch(type)
   {
      case GAN_CHAR:
      case GAN_UCHAR: uiBitsPerPixel = 8*sizeof(unsigned char); break;
      case GAN_SHORT:
      case GAN_USHORT: uiBitsPerPixel = 8*sizeof(unsigned short); break;
      case GAN_INT:
      case GAN_UINT: uiBitsPerPixel = 8*sizeof(unsigned int); break;
      case GAN_FLOAT: uiBitsPerPixel = 8*sizeof(float); break;
      case GAN_DOUBLE: uiBitsPerPixel = 8*sizeof(double); break;
      case GAN_BOOL: uiBitsPerPixel = 1; break;
      case GAN_UINT10: uiBitsPerPixel = 10; break;
      case GAN_UINT12: uiBitsPerPixel = 12; break;
      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_bits_per_pixel", GAN_ERROR_ILLEGAL_TYPE, "" );
        return UINT_MAX;
   }

   switch(format)
   {
      case GAN_GREY_LEVEL_IMAGE:       return   uiBitsPerPixel; break;
      case GAN_GREY_LEVEL_ALPHA_IMAGE: return 2*uiBitsPerPixel; break;
      case GAN_RGB_COLOUR_IMAGE:       return 3*uiBitsPerPixel; break;
      case GAN_RGB_COLOUR_ALPHA_IMAGE: return 4*uiBitsPerPixel; break;
      case GAN_BGR_COLOUR_IMAGE:       return 3*uiBitsPerPixel; break;
      case GAN_BGR_COLOUR_ALPHA_IMAGE: return 4*uiBitsPerPixel; break;
      case GAN_VECTOR_FIELD_2D:        return 2*uiBitsPerPixel; break;
      case GAN_VECTOR_FIELD_3D:        return 3*uiBitsPerPixel; break;
      case GAN_RGBX:
      case GAN_RGBAS:
      case GAN_YUVX444:
      case GAN_YUVA444:
      case GAN_YUV422:
        if(type == GAN_UINT10) return 32;
        break;

      default: break;
   }

   gan_err_flush_trace();
   gan_err_register ( "gan_image_bits_per_pixel", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
   return UINT_MAX;
}

/**
 * \}
 */

/**
 * \}
 */
