/**
 * File:          $RCSfile: openGL_texture.c,v $
 * Module:        Open GL texture handling code
 * Part of:       Gandalf Library
 *
<<<<<<< openGL_texture.c
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:27 $
 * Author:        $Author: ruetz $
=======
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:27 $
 * Author:        $Author: ruetz $
>>>>>>> 1.33.2.6
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

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageDisplay
 * \{
 */

#define GL_GLEXT_PROTOTYPES 1
#define GLX_GLXEXT_PROTOTYPES 1

#include <gandalf/image/openGL_texture.h>
#include <gandalf/image/image_extract.h>
#include <gandalf/common/compare.h>
#include <gandalf/image/image_rgbx_uint10.h>
#include <gandalf/image/io/image_io.h>
#include <string.h>
#ifdef HAVE_OPENGL

#define LARGEST_TEXTURE_TILE_SIZE 256
#define BUFFER_OFFSET(i)  ((char *)NULL + (i))
#if defined(HAVE_OPENGL_EXT) && defined(GL_ARB_shader_objects) && defined(GL_ARB_multitexture)

#if defined(_WIN32)
#  include <GL/glext.h>
#  define gan_texGetProcAddress(name) wglGetProcAddress((LPCSTR)name)
#else
#  if defined(__APPLE__)
#    define gan_texGetProcAddress(name) NSGLGetProcAddress(name)
#  else
#    if defined(__sgi) || defined(__sun)
#      define gan_texGetProcAddress(name) dlGetProcAddress(name)
#    else /* __linux */
#      include <GL/glx.h>
#      define gan_texGetProcAddress(name) (*glXGetProcAddressARB)(name)
#    endif
#  endif
#endif

#ifdef __APPLE__
// NSGLGetProcAddress has to be written by us, see QA1188
#include <mach-o/dyld.h>
static void *NSGLGetProcAddress(const char *name)
{
   NSSymbol symbol;
   char *symbolName;
   // Prepend a '_' for the Unix C symbol mangling convention
   symbolName = malloc (strlen (name) + 2);
   strcpy(symbolName + 1, name);
   symbolName[0] = '_';
   symbol = NULL;
   if (NSIsSymbolNameDefined (symbolName))
      symbol = NSLookupAndBindSymbol (symbolName);
   free (symbolName);
   return symbol ? NSAddressOfSymbol (symbol) : NULL;
}
#endif

static struct _Gan_GLExt_Function_Pointers sGLExtFuncPtrs = {    0, 0, 0, 0, 0,
                                                                0, 0, 0, 0, 0,
                                                                0, 0, 0, 0, 0,
                                                                0, 0, 0, 0, 0, 
                                                                0, 0, 0, 0, 0,
                                                                0, 0, 0, 0, 0,
                                                                0, 0};

/**
 * \brief Initialise OpenGL extension function pointer structure
 */
void gan_initialise_glext_func_ptrs(Gan_GLExt_Function_Pointers* pExtFnPtrs)
{
   pExtFnPtrs->fCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)gan_texGetProcAddress((const GLubyte*)"glCompileShaderARB");
   pExtFnPtrs->fUniform1iARB = (PFNGLUNIFORM1IARBPROC)gan_texGetProcAddress((const GLubyte*)"glUniform1iARB");
   pExtFnPtrs->fGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)gan_texGetProcAddress((const GLubyte*)"glGetUniformLocationARB");
   pExtFnPtrs->fActiveTexture = (PFNGLACTIVETEXTUREPROC)gan_texGetProcAddress((const GLubyte*)"glActiveTexture");
   pExtFnPtrs->fUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)gan_texGetProcAddress((const GLubyte*)"glUseProgramObjectARB");
   pExtFnPtrs->fLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)gan_texGetProcAddress((const GLubyte*)"glLinkProgramARB");
   pExtFnPtrs->fAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)gan_texGetProcAddress((const GLubyte*)"glAttachObjectARB");
   pExtFnPtrs->fGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)gan_texGetProcAddress((const GLubyte*)"glGetInfoLogARB");
   pExtFnPtrs->fGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)gan_texGetProcAddress((const GLubyte*)"glGetObjectParameterivARB");
   pExtFnPtrs->fShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)gan_texGetProcAddress((const GLubyte*)"glShaderSourceARB");
   pExtFnPtrs->fCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)gan_texGetProcAddress((const GLubyte*)"glCreateShaderObjectARB");
   pExtFnPtrs->fCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)gan_texGetProcAddress((const GLubyte*)"glCreateProgramObjectARB");
   pExtFnPtrs->fUniform3fARB = (PFNGLUNIFORM3FARBPROC)gan_texGetProcAddress((const GLubyte*)"glUniform3fARB");
   pExtFnPtrs->fUniform4fARB = (PFNGLUNIFORM4FARBPROC)gan_texGetProcAddress((const GLubyte*)"glUniform4fARB");
   //stct for pbo
   pExtFnPtrs->fGenBuffersARB = (PFNGLGENBUFFERSARBPROC)gan_texGetProcAddress((const GLubyte*)"glGenBuffersARB");
   pExtFnPtrs->fBindBufferARB = (PFNGLBINDBUFFERARBPROC)gan_texGetProcAddress((const GLubyte*)"glBindBufferARB");
   pExtFnPtrs->fBufferDataARB = (PFNGLBUFFERDATAARBPROC)gan_texGetProcAddress((const GLubyte*)"glBufferDataARB");
   pExtFnPtrs->fBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)gan_texGetProcAddress((const GLubyte*)"glBufferSubDataARB");
   pExtFnPtrs->fDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)gan_texGetProcAddress((const GLubyte*)"glDeleteBuffersARB");
   pExtFnPtrs->fMapBufferARB = (PFNGLMAPBUFFERARBPROC)gan_texGetProcAddress((const GLubyte*)"glMapBufferARB");
   pExtFnPtrs->fUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)gan_texGetProcAddress((const GLubyte*)"glUnmapBufferARB");
   //stct for fbo
   pExtFnPtrs->fGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)gan_texGetProcAddress((const GLubyte*)"glGenFramebuffersEXT");
   pExtFnPtrs->fBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)gan_texGetProcAddress((const GLubyte*)"glBindFramebufferEXT");
   pExtFnPtrs->fCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)gan_texGetProcAddress((const GLubyte*)"glCheckFramebufferStatusEXT");
   pExtFnPtrs->fFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)gan_texGetProcAddress((const GLubyte*)"glFramebufferTexture2DEXT");
   pExtFnPtrs->fDrawBuffersARB =  (PFNGLDRAWBUFFERSARBPROC)gan_texGetProcAddress((const GLubyte*)"glDrawBuffersARB");

   //Render Buffer 
   pExtFnPtrs->fGenRenderbuffersEXT =  (PFNGLGENRENDERBUFFERSEXTPROC)gan_texGetProcAddress((const GLubyte*)"glGenRenderbuffersEXT");
   pExtFnPtrs->fBindRenderbufferEXT =  (PFNGLBINDRENDERBUFFEREXTPROC)gan_texGetProcAddress((const GLubyte*)"glBindRenderbufferEXT");
   pExtFnPtrs->fRenderbufferStorageEXT =  (PFNGLRENDERBUFFERSTORAGEEXTPROC)gan_texGetProcAddress((const GLubyte*)"glRenderbufferStorageEXT");
   pExtFnPtrs->fFramebufferRenderbufferEXT =  (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)gan_texGetProcAddress((const GLubyte*)"glFramebufferRenderbufferEXT");


                                 


   pExtFnPtrs->setup = 1;
      
   if (!(    pExtFnPtrs->fCompileShaderARB && pExtFnPtrs->fUniform1iARB          && pExtFnPtrs->fGetUniformLocationARB
          && pExtFnPtrs->fActiveTexture    && pExtFnPtrs->fUseProgramObjectARB   && pExtFnPtrs->fLinkProgramARB
          && pExtFnPtrs->fAttachObjectARB  && pExtFnPtrs->fGetInfoLogARB         && pExtFnPtrs->fGetObjectParameterivARB
          && pExtFnPtrs->fShaderSourceARB  && pExtFnPtrs->fCreateShaderObjectARB && pExtFnPtrs->fCreateProgramObjectARB ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_initialise_glext_func_ptrs()", GAN_ERROR_INCOMPATIBLE, "OpenGL extension not supported by hardware" );
      pExtFnPtrs->error = 1;
   }
   else
   {
      pExtFnPtrs->error = 0;
   }
}

/**
 * \brief Return status of OpenGL extension function pointer structure, non-zero is error
 */
int gan_check_glext_func_ptrs()
{
   if (!(sGLExtFuncPtrs.setup))
   {
      gan_initialise_glext_func_ptrs(&sGLExtFuncPtrs);
   }
   return sGLExtFuncPtrs.error;
}

#endif


/**
 * \brief Build a set of OpenGL texture tiles for an image
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 */
Gan_Bool
 gan_build_openGL_textures_for_image ( Gan_Image *pgiImage, Gan_OpenGLTextureStruct *pOGLTexStruct )
{
   /* OpenGL texture stuff */
   int iTile, iTileV, iTileH;
GLuint uiBufferID;
GLenum info;

#ifdef HAVE_OPENGL_EXT
   /* colour transformation parameters */
   Gan_Bool bApplyColourTransformation;
   GLdouble agldConversionMatrix[16];
   GLdouble agldOffset[4];

#if defined(GL_ARB_shader_objects) && defined(GL_ARB_multitexture)
   gan_check_glext_func_ptrs();
#endif
#endif

   /* determine tile size as greatest power of two fitting inside the image */
   pOGLTexStruct->iTextureTileSize = LARGEST_TEXTURE_TILE_SIZE;
   while(pOGLTexStruct->iTextureTileSize > (int)pgiImage->width || pOGLTexStruct->iTextureTileSize > (int)pgiImage->height)
      pOGLTexStruct->iTextureTileSize /= 2;
   
   pOGLTexStruct->iNumberOfTilesV = (pgiImage->height+pOGLTexStruct->iTextureTileSize-1)/pOGLTexStruct->iTextureTileSize;
   pOGLTexStruct->iNumberOfTilesH = (pgiImage->width+pOGLTexStruct->iTextureTileSize-1)/pOGLTexStruct->iTextureTileSize;
   pOGLTexStruct->iNumberOfTiles = pOGLTexStruct->iNumberOfTilesV*pOGLTexStruct->iNumberOfTilesH;
   /*
   pOGLTexStruct->iNumberOfTilesV =1;
   pOGLTexStruct->iNumberOfTilesH =1;
   pOGLTexStruct->iNumberOfTiles = 1;
   */
   pOGLTexStruct->av2iTilePosition = gan_malloc_array ( Gan_Vector2_i, pOGLTexStruct->iNumberOfTiles );
   if ( pOGLTexStruct->av2iTilePosition == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_build_openGL_textures_for_image", GAN_ERROR_MALLOC_FAILED, "", pOGLTexStruct->iNumberOfTiles*sizeof(Gan_Vector2_i) );
      return GAN_FALSE;
   }
   
   for ( iTileV = 0; iTileV < pOGLTexStruct->iNumberOfTilesV; iTileV++ )
      for ( iTileH = 0; iTileH < pOGLTexStruct->iNumberOfTilesH; iTileH++ )
      {
         iTile = iTileV*pOGLTexStruct->iNumberOfTilesH+iTileH;
         pOGLTexStruct->av2iTilePosition[iTile].x = iTileH*pOGLTexStruct->iTextureTileSize;
         pOGLTexStruct->av2iTilePosition[iTile].y = iTileV*pOGLTexStruct->iTextureTileSize;

         if(pOGLTexStruct->av2iTilePosition[iTile].x + pOGLTexStruct->iTextureTileSize > (int)pgiImage->width)
            pOGLTexStruct->av2iTilePosition[iTile].x = pgiImage->width-pOGLTexStruct->iTextureTileSize;

         if(pOGLTexStruct->av2iTilePosition[iTile].y + pOGLTexStruct->iTextureTileSize > (int)pgiImage->height)
            pOGLTexStruct->av2iTilePosition[iTile].y = pgiImage->height-pOGLTexStruct->iTextureTileSize;
      }

   /* create array of texture labels, or delete previous array */
   pOGLTexStruct->agluiTexture = gan_malloc_array(GLuint, pOGLTexStruct->iNumberOfTiles);
   if ( pOGLTexStruct->agluiTexture == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_build_openGL_textures_for_image", GAN_ERROR_MALLOC_FAILED, "", pOGLTexStruct->iNumberOfTiles*sizeof(GLuint) );
      return GAN_FALSE;
   }

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);    
   glPixelStorei(GL_UNPACK_ROW_LENGTH , pgiImage->width);

#ifdef HAVE_OPENGL_EXT
   /* initialise extension stuff */
#ifdef GL_ARB_shader_objects
   pOGLTexStruct->PHandle = 0;
#endif
   bApplyColourTransformation = GAN_FALSE;
#endif /* #ifdef HAVE_OPENGL_EXT */

   pOGLTexStruct->bEnableBlending = GAN_FALSE;
   pOGLTexStruct->eTextureMode = GL_TEXTURE_2D;

#ifdef HAVE_OPENGL_EXT

   /* special parameters for particular formats */
   switch(pgiImage->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
        break;
              
      case GAN_RGB_COLOUR_IMAGE:
        break;
              
      case GAN_RGBX:
        break;
              
      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        pOGLTexStruct->bEnableBlending = GAN_TRUE;
        break;
              
      case GAN_RGBAS:
        pOGLTexStruct->bEnableBlending = GAN_TRUE;
        break;
              
      case GAN_YUV422:
#ifdef GL_ARB_shader_objects
      {
         GLhandleARB FSHandle;
         int i;
         char *s;

         const char *FProgram=
         "uniform sampler2D YUVtex;\n"
         "\n"
         "void main()\n"
         "{\n"
         "   vec2 leftPixel, rightPixel;\n"
         "   vec4 leftSample, rightSample;\n"
         "   bool bOdd;\n"
         "   vec3 yuv, rgb, evenColour, oddColour;\n"
         "   vec3 YUVOffsets;\n"
         "   mat3 YUVCoeff;\n"
         "\n"
         "   YUVCoeff = mat3(1.164384, 0.0, 1.596027,\n"
         "                   1.164384, -0.391762, -0.812968,\n"
         "                   1.164384, 2.017232,  0.0);\n"
         "\n"
         "   YUVOffsets = vec3(-0.874202, 0.531668, -1.085631);\n"
         "\n"
         "   leftPixel.y = gl_TexCoord[0].y;\n"
         "   rightPixel.y = gl_TexCoord[0].y;\n"
         "\n"
         "   bOdd = bool(floor(fract(gl_TexCoord[0].x * 0.5) * 2.0));\n"
         "\n"
         "   leftPixel.x = floor(gl_TexCoord[0].x) - float(bOdd) + 0.5;\n"
         "   rightPixel.x = leftPixel.x + 1.0;\n"
         "\n"
         "   leftSample = texture2D(YUVtex, leftPixel);\n"
         "   rightSample = texture2D(YUVtex, rightPixel);\n"
         "\n"
         "   evenColour.r = leftSample.a;\n"
         "   evenColour.g = leftSample.r;\n"
         "   evenColour.b = rightSample.r;\n"
         "\n"
         "   oddColour.r = rightSample.a;\n"
         "   oddColour.g = leftSample.r;\n"
         "   oddColour.b = rightSample.r;\n"
         "\n"
         "   yuv = oddColour * vec3(bOdd) + evenColour * vec3(!bOdd);\n"
         "\n"
         "   rgb = (yuv * YUVCoeff) + YUVOffsets;\n"
         "\n"
         "   gl_FragColor = vec4(rgb, 1.0);\n"
         "}\n";

         /* Set up program objects. */
         pOGLTexStruct->PHandle=(*sGLExtFuncPtrs.fCreateProgramObjectARB)();
         FSHandle=(*sGLExtFuncPtrs.fCreateShaderObjectARB)(GL_FRAGMENT_SHADER_ARB);

         /* Compile the shader. */
         (*sGLExtFuncPtrs.fShaderSourceARB)(FSHandle,1,&FProgram,NULL);
         (*sGLExtFuncPtrs.fCompileShaderARB)(FSHandle);

         /* Print the compilation log. */
         (*sGLExtFuncPtrs.fGetObjectParameterivARB)(FSHandle,GL_OBJECT_COMPILE_STATUS_ARB,&i);
         s=malloc(32768);
         (*sGLExtFuncPtrs.fGetInfoLogARB)(FSHandle,32768,NULL,s);
         printf("Compile Log: %s\n", s);
         free(s);

         /* Create a complete program object. */
         (*sGLExtFuncPtrs.fAttachObjectARB)(pOGLTexStruct->PHandle, FSHandle);
         (*sGLExtFuncPtrs.fLinkProgramARB)(pOGLTexStruct->PHandle);

         /* And print the link log. */
         s=malloc(32768);
         (*sGLExtFuncPtrs.fGetInfoLogARB)(pOGLTexStruct->PHandle, 32768, NULL, s);
         printf("Link Log: %s\n", s);
         free(s);

         /* Finally, use the program. */
         (*sGLExtFuncPtrs.fUseProgramObjectARB)(pOGLTexStruct->PHandle);

         /* Select texture unit 0 as the active unit and bind the YUV texture. */
         (*sGLExtFuncPtrs.fActiveTexture)(GL_TEXTURE0);
         i=(*sGLExtFuncPtrs.fGetUniformLocationARB)(pOGLTexStruct->PHandle,"YUVtex");
         (*sGLExtFuncPtrs.fUniform1iARB)(i,0);  /* Bind Ytex to texture unit 0 */

         pOGLTexStruct->eTextureMode = GL_TEXTURE_2D;
      }
#else
      gan_err_flush_trace();
      gan_err_register ( "gan_build_openGL_textures_for_image", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
      return GAN_FALSE;
      
#endif /* #ifdef GL_ARB_shader_objects */
      break;

      case GAN_YUVA444:
      case GAN_YUVX444:
      {
         /* specify colourspace conversion */
         agldConversionMatrix[0] = 255.0/219.0;
         agldConversionMatrix[1] = 255.0/219.0;
         agldConversionMatrix[2] = 255.0/219.0;
         agldConversionMatrix[3] = 0.0;
         agldConversionMatrix[4] = 0.0;
         agldConversionMatrix[5] = -255.0*0.114*(1.0-0.114)/(0.587*224.0*0.5);
         agldConversionMatrix[6] = 255.0*(1.0-0.114)/(224.0*0.5);
         agldConversionMatrix[7] = 0.0;
         agldConversionMatrix[8] = 255.0*(1.0-0.299)/(224.0*0.5);
         agldConversionMatrix[9] = -255.0*0.299*(1.0-0.299)/(0.587*224.0*0.5);
         agldConversionMatrix[10] = 0.0;
         agldConversionMatrix[11] = 0.0;
         agldConversionMatrix[12] = 0.0;
         agldConversionMatrix[13] = 0.0;
         agldConversionMatrix[14] = 0.0;
         agldConversionMatrix[15] = 1.0;
         agldOffset[0] = -16.0/219.0                                             - 128.0*(1.0-0.299)/(224.0*0.5);
         agldOffset[1] = -16.0/219.0 + 128.0*0.114*(1.0-0.114)/(0.587*224.0*0.5) + 128.0*0.299*(1.0-0.299)/(0.587*224.0*0.5);
         agldOffset[2] = -16.0/219.0 - 128.0*(1.0-0.114)/(224.0*0.5);
         if(pgiImage->format == GAN_YUVA444)
         {
            agldOffset[3] = 0.0;
            pOGLTexStruct->bEnableBlending = GAN_TRUE;
         }
         else
            agldOffset[3] = 1.0;
#if 0
         printf("%f %f %f\n", agldConversionMatrix[0], agldConversionMatrix[4], agldConversionMatrix[8]);
         printf("%f %f %f\n", agldConversionMatrix[1], agldConversionMatrix[5], agldConversionMatrix[9]);
         printf("%f %f %f\n", agldConversionMatrix[2], agldConversionMatrix[6], agldConversionMatrix[10]);
         printf("Offsets %f %f %f %f\n", agldOffset[0], agldOffset[1], agldOffset[2], agldOffset[3]);
#endif
         bApplyColourTransformation = GAN_TRUE;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_build_openGL_textures_for_image", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return GAN_FALSE;
   }
   
#endif /* #ifdef HAVE_OPENGL_EXT */

   /* generate OpenGL texture */
   glEnable(GL_TEXTURE_2D);
   glGenTextures ( pOGLTexStruct->iNumberOfTiles, pOGLTexStruct->agluiTexture );

#ifdef HAVE_OPENGL_EXT
   if(bApplyColourTransformation)
   {
      /* specify colourspace conversion */
      glMatrixMode(GL_COLOR);
      glLoadMatrixd(agldConversionMatrix);
      glPixelTransferf(GL_POST_COLOR_MATRIX_RED_BIAS,   agldOffset[0]);
      glPixelTransferf(GL_POST_COLOR_MATRIX_GREEN_BIAS, agldOffset[1]);
      glPixelTransferf(GL_POST_COLOR_MATRIX_BLUE_BIAS,  agldOffset[2]);
      glPixelTransferf(GL_POST_COLOR_MATRIX_ALPHA_BIAS, agldOffset[3]);
   }
#endif /* #ifdef HAVE_OPENGL_EXT */

   glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

   for ( iTileV = 0; iTileV < pOGLTexStruct->iNumberOfTilesV; iTileV++ )
      for ( iTileH = 0; iTileH < pOGLTexStruct->iNumberOfTilesH; iTileH++ )
      {
         iTile = iTileV*pOGLTexStruct->iNumberOfTilesH+iTileH;

         glPixelStorei(GL_UNPACK_SKIP_ROWS,   pOGLTexStruct->av2iTilePosition[iTile].y);
         glPixelStorei(GL_UNPACK_SKIP_PIXELS, pOGLTexStruct->av2iTilePosition[iTile].x);

         glBindTexture(pOGLTexStruct->eTextureMode, pOGLTexStruct->agluiTexture[iTile]);
#if 0
         glTexParameteri ( pOGLTexStruct->eTextureMode, GL_TEXTURE_WRAP_S, GL_REPEAT );
         glTexParameteri ( pOGLTexStruct->eTextureMode, GL_TEXTURE_WRAP_T, GL_REPEAT );
#endif
         glTexParameteri ( pOGLTexStruct->eTextureMode, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
         glTexParameteri ( pOGLTexStruct->eTextureMode, GL_TEXTURE_MIN_FILTER, GL_NEAREST );


         switch ( pgiImage->format )
         {
            case GAN_GREY_LEVEL_IMAGE:
              switch ( pgiImage->type )
              {
                 case GAN_UINT8:
#if defined(GL_ARB_pixel_buffer_object)
                   
                   (*sGLExtFuncPtrs.fGenBuffersARB)(1,&uiBufferID);
                   (*sGLExtFuncPtrs.fBindBufferARB)(GL_PIXEL_UNPACK_BUFFER_ARB,uiBufferID);
                   (*sGLExtFuncPtrs.fBufferDataARB)(GL_PIXEL_UNPACK_BUFFER_ARB,pOGLTexStruct->iTextureTileSize*pOGLTexStruct->iTextureTileSize*8,pgiImage->pix_data_ptr,GL_STREAM_DRAW);
                   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB,pOGLTexStruct->iTextureTileSize,pOGLTexStruct->iTextureTileSize,
                                   0, GL_LUMINANCE, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0) );
                   info = glGetError();
#else
                   glTexImage2D ( pOGLTexStruct->eTextureMode, 0, GL_RGB, pOGLTexStruct->iTextureTileSize, pOGLTexStruct->iTextureTileSize,
                                  0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pgiImage->pix_data_ptr );
#endif/*defined(GL_ARB_pixel_buffer_object)*/
                   break;

                 case GAN_UINT10:
                 case GAN_UINT12:
                 case GAN_UINT16:
                   glTexImage2D ( pOGLTexStruct->eTextureMode, 0, GL_RGB, pOGLTexStruct->iTextureTileSize, pOGLTexStruct->iTextureTileSize,
                                  0, GL_LUMINANCE, GL_UNSIGNED_SHORT, pgiImage->pix_data_ptr );
                   break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_build_openGL_textures_for_image", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                   return GAN_FALSE;
              }
              break;
              
            case GAN_RGB_COLOUR_IMAGE:
              switch ( pgiImage->type )
              {
                 case GAN_UINT8:
                   glTexImage2D ( pOGLTexStruct->eTextureMode, 0, GL_RGB, pOGLTexStruct->iTextureTileSize, pOGLTexStruct->iTextureTileSize,
                                  0, GL_RGB, GL_UNSIGNED_BYTE, pgiImage->pix_data_ptr );
                   break;

                 case GAN_UINT16:
                   glTexImage2D ( pOGLTexStruct->eTextureMode, 0, GL_RGB, pOGLTexStruct->iTextureTileSize, pOGLTexStruct->iTextureTileSize,
                                  0, GL_RGB, GL_UNSIGNED_SHORT, pgiImage->pix_data_ptr );
                   break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_build_openGL_textures_for_image", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                   return GAN_FALSE;
              }
              break;
#ifdef HAVE_OPENGL_EXT              
            case GAN_RGBX:
              switch ( pgiImage->type )
              {
                 case GAN_UINT10:
                   glTexImage2D ( pOGLTexStruct->eTextureMode, 0, GL_RGB10, pOGLTexStruct->iTextureTileSize, pOGLTexStruct->iTextureTileSize,
                                  0, GL_RGBA, GL_UNSIGNED_INT_10_10_10_2, pgiImage->pix_data_ptr );
                   break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_build_openGL_textures_for_image", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                   return GAN_FALSE;
              }
              break;
#endif              
            case GAN_RGB_COLOUR_ALPHA_IMAGE:
              pOGLTexStruct->bEnableBlending = GAN_TRUE;
              switch ( pgiImage->type )
              {
                 case GAN_UINT8:
                   glTexImage2D ( pOGLTexStruct->eTextureMode, 0, GL_RGBA, pOGLTexStruct->iTextureTileSize, pOGLTexStruct->iTextureTileSize,
                                  0, GL_RGBA, GL_UNSIGNED_BYTE, pgiImage->pix_data_ptr );
                   break;

                 case GAN_UINT12:
                   glTexImage2D ( pOGLTexStruct->eTextureMode, 0, GL_RGBA, pOGLTexStruct->iTextureTileSize, pOGLTexStruct->iTextureTileSize,
                                  0, GL_RGBA12, GL_UNSIGNED_SHORT, pgiImage->pix_data_ptr );
                   break;

                 case GAN_UINT16:
                   glTexImage2D ( pOGLTexStruct->eTextureMode, 0, GL_RGBA, pOGLTexStruct->iTextureTileSize, pOGLTexStruct->iTextureTileSize,
                                  0, GL_RGBA, GL_UNSIGNED_SHORT, pgiImage->pix_data_ptr );
                   break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_build_openGL_textures_for_image", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                   return GAN_FALSE;
              }
              break;
#ifdef HAVE_OPENGL_EXT              
            case GAN_RGBAS:
              pOGLTexStruct->bEnableBlending = GAN_TRUE;
              switch ( pgiImage->type )
              {
                 case GAN_UINT10:
                   glTexImage2D ( pOGLTexStruct->eTextureMode, 0, GL_RGB10_A2, pOGLTexStruct->iTextureTileSize, pOGLTexStruct->iTextureTileSize,
                                  0, GL_RGBA, GL_UNSIGNED_INT_10_10_10_2, pgiImage->pix_data_ptr );
                   break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_build_openGL_textures_for_image", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                   return GAN_FALSE;
              }
              break;
#endif              
            case GAN_YUVA444:
              pOGLTexStruct->bEnableBlending = GAN_TRUE;

              /* no break here */

            case GAN_YUVX444:
              switch ( pgiImage->type )
              {
                 case GAN_UINT8:
                   glTexImage2D ( pOGLTexStruct->eTextureMode, 0, GL_RGBA, pOGLTexStruct->iTextureTileSize, pOGLTexStruct->iTextureTileSize,
                                  0, GL_RGBA, GL_UNSIGNED_BYTE, pgiImage->pix_data_ptr );
                   break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_build_openGL_textures_for_image", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                   return GAN_FALSE;
              }
              break;
              
            case GAN_YUV422:
              switch ( pgiImage->type )
              {
                 case GAN_UINT8:
                   glTexImage2D ( pOGLTexStruct->eTextureMode, 0, 2, pOGLTexStruct->iTextureTileSize, pOGLTexStruct->iTextureTileSize,
                                  0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, pgiImage->pix_data_ptr );
                   break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_build_openGL_textures_for_image", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                   return GAN_FALSE;
              }
              break;
              
            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_build_openGL_textures_for_image", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
              return GAN_FALSE;
         }
      }

   glPopClientAttrib();

#ifdef HAVE_OPENGL_EXT
   if(bApplyColourTransformation)
   {
      /* specify colourspace conversion */
      glMatrixMode(GL_COLOR);
      glLoadIdentity();
      glPixelTransferf(GL_POST_COLOR_MATRIX_RED_BIAS,   0.0);
      glPixelTransferf(GL_POST_COLOR_MATRIX_GREEN_BIAS, 0.0);
      glPixelTransferf(GL_POST_COLOR_MATRIX_BLUE_BIAS,  0.0);
      glPixelTransferf(GL_POST_COLOR_MATRIX_ALPHA_BIAS, 0.0F);
   }
#endif /* #ifdef HAVE_OPENGL_EXT */

   glDisable(GL_TEXTURE_2D);

   return GAN_TRUE;
}

/**
 * \brief Free OpenGL texture tiles for an image
 */
void
 gan_free_openGL_textures_for_image ( Gan_OpenGLTextureStruct *pOGLTexStruct )
{
   glDeleteTextures ( pOGLTexStruct->iNumberOfTiles, pOGLTexStruct->agluiTexture );
   free(pOGLTexStruct->agluiTexture);
   free(pOGLTexStruct->av2iTilePosition);
}

/**
 * \brief Render OpenGL texture tiles for an image
 */
Gan_Bool
 gan_render_openGL_textures_for_image ( Gan_OpenGLTextureStruct *pOGLTexStruct )
{
   int iTile;

#if defined(HAVE_OPENGL_EXT) && defined(GL_ARB_shader_objects) && defined(GL_ARB_multitexture)
   gan_check_glext_func_ptrs();
#endif

   /* render OpenGL textures */
   glEnable(pOGLTexStruct->eTextureMode);
   glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

#if defined(HAVE_OPENGL_EXT) && defined(GL_ARB_shader_objects)
   if(pOGLTexStruct->PHandle != 0)
      (*sGLExtFuncPtrs.fUseProgramObjectARB)(pOGLTexStruct->PHandle);
#endif /* #if defined(HAVE_OPENGL_EXT) && defined(GL_ARB_shader_objects) */

   if(pOGLTexStruct->bEnableBlending)
   {
      glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      glEnable(GL_BLEND);
   }

   for ( iTile = 0; iTile < pOGLTexStruct->iNumberOfTiles; iTile++ )
   {

      glBindTexture ( pOGLTexStruct->eTextureMode, pOGLTexStruct->agluiTexture[iTile] );

      /* set texture coordinates */
      switch(pOGLTexStruct->eTextureMode)
      {
         case GL_TEXTURE_2D:
           glBegin(GL_QUADS);

           /* top-left */
           glTexCoord2d ( 0.0, 0.0 );
           glVertex2d ( (double)pOGLTexStruct->av2iTilePosition[iTile].x,
                        (double)pOGLTexStruct->av2iTilePosition[iTile].y );

           /* bottom-left */
           glTexCoord2d ( 0.0, 1.0 );
           glVertex2d ( (double)pOGLTexStruct->av2iTilePosition[iTile].x,
                   (double)pOGLTexStruct->av2iTilePosition[iTile].y+(double)pOGLTexStruct->iTextureTileSize );

           /* bottom-right */
           glTexCoord2d ( 1.0, 1.0 );
           glVertex2d ( (double)pOGLTexStruct->av2iTilePosition[iTile].x+(double)pOGLTexStruct->iTextureTileSize,
                        (double)pOGLTexStruct->av2iTilePosition[iTile].y+(double)pOGLTexStruct->iTextureTileSize );

           /* top-right */
           glTexCoord2d ( 1.0, 0.0 );
           glVertex2d ( (double)pOGLTexStruct->av2iTilePosition[iTile].x+(double)pOGLTexStruct->iTextureTileSize,
                   (double)pOGLTexStruct->av2iTilePosition[iTile].y );
           
           glEnd();
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_render_openGL_textures_for_image", GAN_ERROR_ILLEGAL_TYPE, "" );
           return GAN_FALSE;
      }
   }

   
#ifdef HAVE_OPENGL_EXT
#ifdef GL_ARB_shader_objects
   if(pOGLTexStruct->PHandle != 0)
      (*sGLExtFuncPtrs.fUseProgramObjectARB)(0);
#endif
#endif /* #ifdef HAVE_OPENGL_EXT */

#if defined(GL_ARB_pixel_buffer_object)
   glDisable(GL_TEXTURE_2D);
#else
   glDisable(pOGLTexStruct->eTextureMode);
#endif

   if(pOGLTexStruct->bEnableBlending)
      glDisable(GL_BLEND);

   return GAN_TRUE;
}

#if defined(GL_ARB_pixel_buffer_object)
Gan_Bool
 gan_readback_openGL_image_from_framebuffer ( Gan_OpenGLTextureStruct *pOGLTexStruct, Gan_Image *pgiImage  )
{
   int iTile;
   GLenum info;
    int pixsize;
    void* pixel;
    Gan_Image* pgiOutputImage;
    Gan_Image* pgiTemp;
    GLuint* uiBufferID;
#if defined(HAVE_OPENGL_EXT) && defined(GL_ARB_shader_objects) && defined(GL_ARB_multitexture)
   gan_check_glext_func_ptrs();
#endif

   /* render OpenGL textures */
   glEnable(pOGLTexStruct->eTextureMode);
   glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

#if defined(HAVE_OPENGL_EXT) && defined(GL_ARB_shader_objects)
   if(pOGLTexStruct->PHandle != 0)
      (*sGLExtFuncPtrs.fUseProgramObjectARB)(pOGLTexStruct->PHandle);

   printf("sizeof(GLuint)=%d\n", (int)sizeof(GLuint));
  
   (*sGLExtFuncPtrs.fGenBuffersARB)(6,uiBufferID);
#endif /* #if defined(HAVE_OPENGL_EXT) && defined(GL_ARB_shader_objects) */
      
   for ( iTile = 0; iTile < pOGLTexStruct->iNumberOfTiles; iTile++ )
   {
      
      (*sGLExtFuncPtrs.fBindBufferARB)(GL_PIXEL_PACK_BUFFER_ARB,uiBufferID[0]);
     
      pixsize = 8* gan_image_pixel_size(pgiImage->format,pgiImage->type);
      
      (*sGLExtFuncPtrs.fBufferDataARB)(GL_PIXEL_PACK_BUFFER_ARB,pOGLTexStruct->iTextureTileSize*pOGLTexStruct->iTextureTileSize*pixsize,NULL,GL_STATIC_READ);

      glReadPixels(pOGLTexStruct->av2iTilePosition[iTile].x,pOGLTexStruct->av2iTilePosition[iTile].y,
                   pOGLTexStruct->iTextureTileSize,pOGLTexStruct->iTextureTileSize,GL_LUMINANCE,GL_UNSIGNED_BYTE,BUFFER_OFFSET(0));
      pixel = (*sGLExtFuncPtrs.fMapBufferARB)(GL_PIXEL_PACK_BUFFER_ARB,GL_READ_WRITE_ARB);
      pgiOutputImage = gan_image_alloc_data (pgiImage->format, pgiImage->type,pOGLTexStruct->iTextureTileSize,pOGLTexStruct->iTextureTileSize, pixel,pOGLTexStruct->iTextureTileSize*pOGLTexStruct->iTextureTileSize*pixsize , NULL,0);
      pgiTemp = gan_image_extract_s (pgiImage,pOGLTexStruct->av2iTilePosition[iTile].y,pOGLTexStruct->av2iTilePosition[iTile].x,pOGLTexStruct->iTextureTileSize,pOGLTexStruct->iTextureTileSize, pgiImage->format, pgiImage->type,GAN_FALSE);

      pgiTemp = gan_image_extract_q (pgiOutputImage,0,0,pOGLTexStruct->iTextureTileSize,pOGLTexStruct->iTextureTileSize, pgiImage->format, pgiImage->type, GAN_TRUE, pgiTemp);
      gan_image_free(pgiOutputImage);
      gan_image_free(pgiTemp);
      
     
      info = glGetError();
      if (!(*sGLExtFuncPtrs.fUnmapBufferARB)(GL_PIXEL_PACK_BUFFER_ARB))
         return GAN_FALSE;
      

      
      
     

    
      /*
    
           glVertex2d ( (double)pOGLTexStruct->av2iTilePosition[iTile].x,
                        (double)pOGLTexStruct->av2iTilePosition[iTile].y );
      */

      
   }
   //   gan_image_write("/tmp/tmp.png",GAN_PNG_FORMAT,pgiImage,0);
   return GAN_TRUE;    

}
#endif

/* #if defined(GL_ARB_pixel_buffer_object) */
/* GLsizeiptr OffsetLimitImage2D(__GLpixelStoreMode* pixelStoreMode, */
/*                                      GLsizei width, GLsizei height, */
/*                                      GLenum format,GLenum type, */
/*                                      const GLvoid* userdata) */
/* { */
/*    const GLint line_length = pixelStoreMode->lineLength; */
/*    const GLint image_height =pixelStoreMode->imageHeight; */
/*    const GLint alignment = pixelStoreMode->alignment; */
/*    const GLint skip_pixels = pixelStoreMode->skipPixels; */
/*    const GLint skip_lines = pixelStoreMode->skipLines; */

/*    GLsizeiptr offsetLimit = (GLsizeiptr) userdata; */

/*    GLint rowsize; */
/*    GLint padding; */
/*    GLint imagesize; */

/*    assert(width>0); */
/*    assert(height>0); */
/*    assert(depth =1); */

/*    assert(line_length>=0); */
/*    assert(image_hieght>=0); */

/*    assert(skip_pixels>=0); */
/*    assert(skip_lines>=0); */

/*    assert((alignment ==1) || */
/*           (alignment ==2) || */
/*           (alignment ==4) || */
/*           (alignment ==8)); */
/*    const GLint components = glElementsPerGroup(format,type); */
/*    const GLint element_size = glBytesPerElement(type); */
/*    const GLint group_size = element_size * components; */

/*    if (0 == (line_length | image_height | skip_pixels | */
/*              skip_lines |skip_pixels)) */
/*    { */
/*       rowsize = width * group_size; */
/*       padding = rowsize & (alignment -1); */
/*       if (padding) */
/*       { */
/*          rowsize += alignment - padding; */
/*       } */
/*       imagesize = height * rowsize ; */
/*       offsetLimit += imagesize; */
/*    } */
/*    else */
/*    { */
/*       const GLint groups_per_line = (line_length >0)? line_length : width; */
/*       const GLint rows_per_image = (image+height >0)? image_height : height; */

/*       rowsize = groups_per_line * group_size; */
/*       padding = rowsize & (alignment -1); */
/*       if (padding) */
/*       { */
/*          rowsize += alignment - padding; */
         
/*       } */
/*       imagesize= rows_per_image * rowsize; */

/*       offsetLimit += imagesize ; */
/*       offsetLimit += rowsize * (skip_lines +height - 1); */
/*       offsetLimit += group_size * (skip_pixels + width); */
/*    } */
/*    return offsetLimit; */
/* } */
/* #endif */

#endif /* #ifdef HAVE_OPENGL */

   
   
   
   
   
   
   
   
/**
 * \}
 */

/**
 * \}
 */
