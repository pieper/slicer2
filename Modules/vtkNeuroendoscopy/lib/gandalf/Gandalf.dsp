# Microsoft Developer Studio Project File - Name="Gandalf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Gandalf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Gandalf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Gandalf.mak" CFG="Gandalf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Gandalf - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Gandalf - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Gandalf - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(libpng)" /I "$(zlib)" /I "$(DEVDIR)" /I "..\3rd_party\\" /D "GAN_THREAD_SAFE" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "WIN32" /D "_AFX_NOFORCE_LIBS" /FR /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Gandalf - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "GanDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "$(libpng)" /I "$(zlib)" /I "$(DEVDIR)" /I "..\3rd_party\\" /D "GAN_THREAD_SAFE" /D "_MBCS" /D "_LIB" /D "WINSTANDALONE" /D "WIN32" /D "_AFX_NOFORCE_LIBS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"GanDebug\Gandalf.lib"

!ENDIF 

# Begin Target

# Name "Gandalf - Win32 Release"
# Name "Gandalf - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "image"

# PROP Default_Filter "c"
# Begin Group "io"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\image\io\cineon_io.c
# End Source File
# Begin Source File

SOURCE=.\image\io\dpx_io.c
# End Source File
# Begin Source File

SOURCE=.\image\io\image_io.c
# End Source File
# Begin Source File

SOURCE=.\image\io\jpeg_io.c
# End Source File
# Begin Source File

SOURCE=.\image\io\movie.c
# End Source File
# Begin Source File

SOURCE=.\image\io\pbm_io.c
# End Source File
# Begin Source File

SOURCE=.\image\io\pgm_io.c
# End Source File
# Begin Source File

SOURCE=.\image\io\png_io.c
# End Source File
# Begin Source File

SOURCE=.\image\io\ppm_io.c
# End Source File
# Begin Source File

SOURCE=.\image\io\sgi_io.c
# End Source File
# Begin Source File

SOURCE=.\image\io\targa_io.c
# End Source File
# Begin Source File

SOURCE=.\image\io\tiff_io.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\image\image_bit.c
# End Source File
# Begin Source File

SOURCE=.\image\image_channel.c
# End Source File
# Begin Source File

SOURCE=.\image\image_compare.c
# End Source File
# Begin Source File

SOURCE=.\image\image_composite.c
# End Source File
# Begin Source File

SOURCE=.\image\image_defs.c
# End Source File
# Begin Source File

SOURCE=.\image\image_display.c
# End Source File
# Begin Source File

SOURCE=.\image\image_extract.c
# End Source File
# Begin Source File

SOURCE=.\image\image_flip.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_double.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_float.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_int.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_short.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_uchar.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_uint.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_uint10.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_uint12.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_ushort.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gla_double.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gla_float.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gla_int.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gla_short.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gla_uchar.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gla_uint.c
# End Source File
# Begin Source File

SOURCE=.\image\image_gla_ushort.c
# End Source File
# Begin Source File

SOURCE=.\image\image_invert.c
# End Source File
# Begin Source File

SOURCE=.\image\image_pointer.c
# End Source File
# Begin Source File

SOURCE=.\image\image_pyramid.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgb_double.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgb_float.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgb_int.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgb_short.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgb_uchar.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgb_uint.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgb_ushort.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_double.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_float.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_int.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_short.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_uchar.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_uint.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_uint12.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_ushort.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgbas_uint10.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgbx_uint10.c
# End Source File
# Begin Source File

SOURCE=.\image\image_rgbx_uint8.c
# End Source File
# Begin Source File

SOURCE=.\image\image_scale.c
# End Source File
# Begin Source File

SOURCE=.\image\image_vfield2D_double.c
# End Source File
# Begin Source File

SOURCE=.\image\image_vfield2D_float.c
# End Source File
# Begin Source File

SOURCE=.\image\image_vfield2D_int.c
# End Source File
# Begin Source File

SOURCE=.\image\image_vfield2D_short.c
# End Source File
# Begin Source File

SOURCE=.\image\image_vfield3D_double.c
# End Source File
# Begin Source File

SOURCE=.\image\image_vfield3D_float.c
# End Source File
# Begin Source File

SOURCE=.\image\image_vfield3D_int.c
# End Source File
# Begin Source File

SOURCE=.\image\image_vfield3D_short.c
# End Source File
# Begin Source File

SOURCE=.\image\image_yuv422_uint8.c
# End Source File
# Begin Source File

SOURCE=.\image\image_yuva444_uint8.c
# End Source File
# Begin Source File

SOURCE=.\image\image_yuvx444_uint8.c
# End Source File
# Begin Source File

SOURCE=.\image\pixel.c
# End Source File
# End Group
# Begin Group "linalg"

# PROP Default_Filter "c"
# Begin Group "matrix"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\linalg\matrix\eigen.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\eigenf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\eigval.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\eigvalf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\house.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\housef.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\housev.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\housevf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\ldumat.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\ldumatf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\ldvmat.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\ldvmatf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\qrbdv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\qrbdvf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\qreval.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\qrevalf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\qrevec.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\qrevecf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\svduv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\svduvf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\trnm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\trnmf.c
# End Source File
# End Group
# Begin Group "cblas"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\linalg\cblas\daxpy.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dcopy.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\ddot.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dgemm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dgemv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dger.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dnrm2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dscal.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dspmv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dspr.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dspr2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dswap.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dtpmv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dtpsv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dtrmv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dtrsm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\idamax.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\isamax.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\saxpy.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\scopy.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\sdot.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\sgemm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\sgemv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\sger.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\snrm2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\sscal.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\sspmv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\sspr.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\sspr2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\sswap.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\stpmv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\stpsv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\strmv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\strsm.c
# End Source File
# End Group
# Begin Group "clapack"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\linalg\clapack\dgelq2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dgeqr2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dgesv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dgetf2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dgetrf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dgetri.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dgetrs.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dlapy2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dlarf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dlarfg.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dlaswp.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dorg2l.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dorg2r.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dorgl2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dpptrf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dpptri.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dtptri.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dtrti2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\ilaenv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\sgelq2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\sgeqr2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\sgesv.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\sgetf2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\sgetrf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\sgetri.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\sgetrs.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\slapy2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\slarf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\slarfg.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\slaswp.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\sorg2l.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\sorg2r.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\sorgl2.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\spptrf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\spptri.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\stptri.c
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\strti2.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\linalg\2vector.c
# End Source File
# Begin Source File

SOURCE=.\linalg\2vector_norm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\2vectorf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\2x2matrix.c
# End Source File
# Begin Source File

SOURCE=.\linalg\2x2matrix_eigsym.c
# End Source File
# Begin Source File

SOURCE=.\linalg\2x2matrix_norm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\2x2matrixf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\2x3matrix.c
# End Source File
# Begin Source File

SOURCE=.\linalg\2x3matrix_norm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\2x3matrixf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\2x4matrix.c
# End Source File
# Begin Source File

SOURCE=.\linalg\2x4matrix_norm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\2x4matrixf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\3vector.c
# End Source File
# Begin Source File

SOURCE=.\linalg\3vector_norm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\3vectorf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3matrix.c
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3matrix_eigen.c
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3matrix_eigsym.c
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3matrix_norm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3matrix_svd.c
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3matrixf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\3x4matrix.c
# End Source File
# Begin Source File

SOURCE=.\linalg\3x4matrix_norm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\3x4matrixf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\4vector.c
# End Source File
# Begin Source File

SOURCE=.\linalg\4vector_norm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\4vectorf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4matrix.c
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4matrix_eigsym.c
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4matrix_norm.c
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4matrix_svd.c
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4matrixf.c
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_diagonal.c
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_gen.c
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_lq.c
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_qr.c
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_scaledI.c
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_square.c
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_svd.c
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_symmetric.c
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_triangular.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_diagonal.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_gen.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_lq.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_qr.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_scaledI.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_square.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_svd.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_symmetric.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_triangular.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matvec_blas.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matvec_clapack.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matvecf_blas.c
# End Source File
# Begin Source File

SOURCE=.\linalg\matvecf_clapack.c
# End Source File
# Begin Source File

SOURCE=.\linalg\pseudo_inverse.c
# End Source File
# Begin Source File

SOURCE=.\linalg\pseudo_inversef.c
# End Source File
# Begin Source File

SOURCE=.\linalg\symmat_eigen.c
# End Source File
# Begin Source File

SOURCE=.\linalg\vec_gen.c
# End Source File
# Begin Source File

SOURCE=.\linalg\vecf_gen.c
# End Source File
# End Group
# Begin Group "vision"

# PROP Default_Filter "c"
# Begin Source File

SOURCE=.\vision\affine33_fit.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_affine.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_convert.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_cubic_Bspline.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_cubic_Bspline_inv.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_equi_solid_angle.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_equidistant.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_linear.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_radial_dist1.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_radial_dist1_inv.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_radial_dist2.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_radial_dist3.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_sine_law.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_stereographic.c
# End Source File
# Begin Source File

SOURCE=.\vision\camera_xy_dist4.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_affine.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_cubic_Bspline.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_cubic_Bspline_inv.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_equi_solid_angle.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_equidistant.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_linear.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_radial_dist1.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_radial_dist1_inv.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_radial_dist2.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_radial_dist3.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_sine_law.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_stereographic.c
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_xy_dist4.c
# End Source File
# Begin Source File

SOURCE=.\vision\canny_edge.c
# End Source File
# Begin Source File

SOURCE=.\vision\convolve1D.c
# End Source File
# Begin Source File

SOURCE=.\vision\corner_disp.c
# End Source File
# Begin Source File

SOURCE=.\vision\corner_feature.c
# End Source File
# Begin Source File

SOURCE=.\vision\edge_disp.c
# End Source File
# Begin Source File

SOURCE=.\vision\edge_feature.c
# End Source File
# Begin Source File

SOURCE=.\vision\essential.c
# End Source File
# Begin Source File

SOURCE=.\vision\euclid3D.c
# End Source File
# Begin Source File

SOURCE=.\vision\fast_hough_transform.c
# End Source File
# Begin Source File

SOURCE=.\vision\fundamental.c
# End Source File
# Begin Source File

SOURCE=.\vision\harris_corner.c
# End Source File
# Begin Source File

SOURCE=.\vision\homog33_fit.c
# End Source File
# Begin Source File

SOURCE=.\vision\homog34_fit.c
# End Source File
# Begin Source File

SOURCE=.\vision\lev_marq.c
# End Source File
# Begin Source File

SOURCE=.\vision\line_disp.c
# End Source File
# Begin Source File

SOURCE=.\vision\line_feature.c
# End Source File
# Begin Source File

SOURCE=.\vision\local_feature.c
# End Source File
# Begin Source File

SOURCE=.\vision\mask1D.c
# End Source File
# Begin Source File

SOURCE=.\vision\modified_fht2D.c
# End Source File
# Begin Source File

SOURCE=.\vision\orthog_line.c
# End Source File
# Begin Source File

SOURCE=.\vision\rotate3D.c
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\common\allocate.c
# End Source File
# Begin Source File

SOURCE=.\common\array.c
# End Source File
# Begin Source File

SOURCE=.\common\bit_array.c
# End Source File
# Begin Source File

SOURCE=.\common\compare.c
# End Source File
# Begin Source File

SOURCE=.\common\endian_io.c
# End Source File
# Begin Source File

SOURCE=.\common\gan_err.c
# End Source File
# Begin Source File

SOURCE=.\common\gan_err_trace.c
# End Source File
# Begin Source File

SOURCE=.\common\linked_list.c
# End Source File
# Begin Source File

SOURCE=.\common\memory_stack.c
# End Source File
# Begin Source File

SOURCE=.\common\misc_defs.c
# End Source File
# Begin Source File

SOURCE=.\common\misc_error.c
# End Source File
# Begin Source File

SOURCE=.\common\numerics.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "image_h"

# PROP Default_Filter "h"
# Begin Group "io_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\image\io\cineon_io.h
# End Source File
# Begin Source File

SOURCE=.\image\io\dpx_io.h
# End Source File
# Begin Source File

SOURCE=.\image\io\image_io.h
# End Source File
# Begin Source File

SOURCE=.\image\io\io.h
# End Source File
# Begin Source File

SOURCE=.\image\io\jpeg_io.h
# End Source File
# Begin Source File

SOURCE=.\image\io\movie.h
# End Source File
# Begin Source File

SOURCE=.\image\io\pbm_io.h
# End Source File
# Begin Source File

SOURCE=.\image\io\pgm_io.h
# End Source File
# Begin Source File

SOURCE=.\image\io\png_io.h
# End Source File
# Begin Source File

SOURCE=.\image\io\ppm_io.h
# End Source File
# Begin Source File

SOURCE=.\image\io\sgi_io.h
# End Source File
# Begin Source File

SOURCE=.\image\io\targa_io.h
# End Source File
# Begin Source File

SOURCE=.\image\io\tiff_io.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\image\image_bit.h
# End Source File
# Begin Source File

SOURCE=.\image\image_channel.h
# End Source File
# Begin Source File

SOURCE=.\image\image_colour.h
# End Source File
# Begin Source File

SOURCE=.\image\image_compare.h
# End Source File
# Begin Source File

SOURCE=.\image\image_composite.h
# End Source File
# Begin Source File

SOURCE=.\image\image_defs.h
# End Source File
# Begin Source File

SOURCE=.\image\image_double.h
# End Source File
# Begin Source File

SOURCE=.\image\image_extract.h
# End Source File
# Begin Source File

SOURCE=.\image\image_flip.h
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_uchar.h
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_uint10.h
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_uint12.h
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_uint16.h
# End Source File
# Begin Source File

SOURCE=.\image\image_gl_uint8.h
# End Source File
# Begin Source File

SOURCE=.\image\image_gla_double.h
# End Source File
# Begin Source File

SOURCE=.\image\image_gla_int.h
# End Source File
# Begin Source File

SOURCE=.\image\image_gla_uchar.h
# End Source File
# Begin Source File

SOURCE=.\image\image_gla_uint.h
# End Source File
# Begin Source File

SOURCE=.\image\image_grey.h
# End Source File
# Begin Source File

SOURCE=.\image\image_int.h
# End Source File
# Begin Source File

SOURCE=.\image\image_invert.h
# End Source File
# Begin Source File

SOURCE=.\image\image_pointer.h
# End Source File
# Begin Source File

SOURCE=.\image\image_pyramid.h
# End Source File
# Begin Source File

SOURCE=.\image\image_rgb_double.h
# End Source File
# Begin Source File

SOURCE=.\image\image_rgb_int.h
# End Source File
# Begin Source File

SOURCE=.\image\image_rgb_uchar.h
# End Source File
# Begin Source File

SOURCE=.\image\image_rgb_uint.h
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_double.h
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_int.h
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_uchar.h
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_uint.h
# End Source File
# Begin Source File

SOURCE=.\image\image_rgba_uint12.h
# End Source File
# Begin Source File

SOURCE=.\image\image_rgbx_uint10.h
# End Source File
# Begin Source File

SOURCE=.\image\image_rgbx_uint8.h
# End Source File
# Begin Source File

SOURCE=.\image\image_scale.h
# End Source File
# Begin Source File

SOURCE=.\image\image_uint.h
# End Source File
# Begin Source File

SOURCE=.\image\image_vfield2D_double.h
# End Source File
# Begin Source File

SOURCE=.\image\image_vfield2D_int.h
# End Source File
# Begin Source File

SOURCE=.\image\image_vfield3D_double.h
# End Source File
# Begin Source File

SOURCE=.\image\image_vfield3D_int.h
# End Source File
# Begin Source File

SOURCE=.\image\image_yuv422_uint8.h
# End Source File
# Begin Source File

SOURCE=.\image\image_yuva444_uint8.h
# End Source File
# Begin Source File

SOURCE=.\image\image_yuvx444_uint8.h
# End Source File
# Begin Source File

SOURCE=.\image\io.h
# End Source File
# Begin Source File

SOURCE=.\image\pixel.h
# End Source File
# End Group
# Begin Group "linalg_h"

# PROP Default_Filter "h"
# Begin Group "matrix_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\linalg\matrix\complex.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix\matutl.h
# End Source File
# End Group
# Begin Group "cblas_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\linalg\cblas\cblas_defs.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\daxpy.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dcopy.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\ddot.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dgemm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dgemv.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dger.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dnrm2.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dscal.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dspmv.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dspr.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dspr2.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dswap.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dtpmv.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dtpsv.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dtrmv.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\dtrsm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\extern.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\idamax.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas\strsm.h
# End Source File
# End Group
# Begin Group "lapack_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\linalg\clapack\dgelq2.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dgelqf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dgeqr2.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dgeqrf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dgetf2.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dgetrf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dgetri.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dlapy2.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dlarf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dlarfg.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dorg2l.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dorg2r.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dorgl2.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dorglq.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dorgqr.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dpptrf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dpptri.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dtptri.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dtrti2.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\dtrtri.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack\extern.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\linalg\2vector.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2vector_norm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2vectorf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x2ltmat.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x2ltmatT.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x2mat.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x2matrix.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x2matrix_eigsym.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x2matrix_norm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x2matrixf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x2symmat.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x3mat.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x3matrix.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x3matrix_norm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x3matrixf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x4mat.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x4matrix.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x4matrix_norm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\2x4matrixf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3vector.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3vector_norm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3vectorf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3ltmat.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3ltmatT.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3mat.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3matrix.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3matrix_eigen.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3matrix_eigsym.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3matrix_norm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3matrix_svd.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3matrixf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x3symmat.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x4mat.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x4matrix.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x4matrix_norm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\3x4matrixf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\4vector.h
# End Source File
# Begin Source File

SOURCE=.\linalg\4vector_norm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\4vectorf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4ltmat.h
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4ltmatT.h
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4mat.h
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4matrix.h
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4matrix_eigsym.h
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4matrix_norm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4matrix_svd.h
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4matrixf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\4x4symmat.h
# End Source File
# Begin Source File

SOURCE=.\linalg\cblas.h
# End Source File
# Begin Source File

SOURCE=.\linalg\clapack.h
# End Source File
# Begin Source File

SOURCE=.\linalg\linalg_defs.h
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_defs.h
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_diagonal.h
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_gen.h
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_lq.h
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_norm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_orthog_diag.h
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_qr.h
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_scaledI.h
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_square.h
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_svd.h
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_symmetric.h
# End Source File
# Begin Source File

SOURCE=.\linalg\mat_triangular.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_diagonal.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_gen.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_scaledI.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_square.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_symmetric.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matf_triangular.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matrix.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matrixf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matvec_blas.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matvec_clapack.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matvecf_blas.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matvecf_clapack.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matvecf_macros.h
# End Source File
# Begin Source File

SOURCE=.\linalg\matvecf_macros_noc.h
# End Source File
# Begin Source File

SOURCE=.\linalg\pseudo_inverse.h
# End Source File
# Begin Source File

SOURCE=.\linalg\repeat1.h
# End Source File
# Begin Source File

SOURCE=.\linalg\repeat2.h
# End Source File
# Begin Source File

SOURCE=.\linalg\repeat22.h
# End Source File
# Begin Source File

SOURCE=.\linalg\repeat23.h
# End Source File
# Begin Source File

SOURCE=.\linalg\repeat24.h
# End Source File
# Begin Source File

SOURCE=.\linalg\repeat3.h
# End Source File
# Begin Source File

SOURCE=.\linalg\repeat33.h
# End Source File
# Begin Source File

SOURCE=.\linalg\repeat34.h
# End Source File
# Begin Source File

SOURCE=.\linalg\repeat4.h
# End Source File
# Begin Source File

SOURCE=.\linalg\repeat44.h
# End Source File
# Begin Source File

SOURCE=.\linalg\squmatrixf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\symmat_eigen.h
# End Source File
# Begin Source File

SOURCE=.\linalg\symmatrixf.h
# End Source File
# Begin Source File

SOURCE=.\linalg\vec_gen.h
# End Source File
# Begin Source File

SOURCE=.\linalg\vec_norm.h
# End Source File
# Begin Source File

SOURCE=.\linalg\vecf_gen.h
# End Source File
# Begin Source File

SOURCE=.\linalg\vectorf.h
# End Source File
# End Group
# Begin Group "vision_h"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\vision\affine33_fit.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_affine.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_cubic_Bspline.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_cubic_Bspline_inv.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_defs.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_equi_solid_angle.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_linear.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_radial_dist1.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_radial_dist1_inv.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_radial_dist2.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_radial_dist3.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_sine_law.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_stereographic.h
# End Source File
# Begin Source File

SOURCE=.\vision\camera_xy_dist4.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_affine.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_cubic_Bspline.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_cubic_Bspline_inv.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_equi_solid_angle.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_equidistant.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_linear.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_radial_dist1.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_radial_dist1_inv.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_radial_dist2.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_radial_dist3.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_sine_law.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_stereographic.h
# End Source File
# Begin Source File

SOURCE=.\vision\cameraf_xy_dist4.h
# End Source File
# Begin Source File

SOURCE=.\vision\canny_edge.h
# End Source File
# Begin Source File

SOURCE=.\vision\convolve1D.h
# End Source File
# Begin Source File

SOURCE=.\vision\corner_disp.h
# End Source File
# Begin Source File

SOURCE=.\vision\corner_feature.h
# End Source File
# Begin Source File

SOURCE=.\vision\edge_disp.h
# End Source File
# Begin Source File

SOURCE=.\vision\edge_feature.h
# End Source File
# Begin Source File

SOURCE=.\vision\essential.h
# End Source File
# Begin Source File

SOURCE=.\vision\euclid3D.h
# End Source File
# Begin Source File

SOURCE=.\vision\fast_hough_transform.h
# End Source File
# Begin Source File

SOURCE=.\vision\fundamental.h
# End Source File
# Begin Source File

SOURCE=.\vision\harris_corner.h
# End Source File
# Begin Source File

SOURCE=.\vision\homog33_fit.h
# End Source File
# Begin Source File

SOURCE=.\vision\homog34_fit.h
# End Source File
# Begin Source File

SOURCE=.\vision\lev_marq.h
# End Source File
# Begin Source File

SOURCE=.\vision\line_disp.h
# End Source File
# Begin Source File

SOURCE=.\vision\line_feature.h
# End Source File
# Begin Source File

SOURCE=.\vision\local_feature.h
# End Source File
# Begin Source File

SOURCE=.\vision\mask1D.h
# End Source File
# Begin Source File

SOURCE=.\vision\modified_fht2D.h
# End Source File
# Begin Source File

SOURCE=.\vision\orthog_line.h
# End Source File
# Begin Source File

SOURCE=.\vision\rotate3D.h
# End Source File
# End Group
# Begin Group "common_h"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\common\allocate.h
# End Source File
# Begin Source File

SOURCE=.\common\array.h
# End Source File
# Begin Source File

SOURCE=.\common\bit_array.h
# End Source File
# Begin Source File

SOURCE=.\common\comp.h
# End Source File
# Begin Source File

SOURCE=.\common\compare.h
# End Source File
# Begin Source File

SOURCE=.\common\complex.h
# End Source File
# Begin Source File

SOURCE=.\common\endian_io.h
# End Source File
# Begin Source File

SOURCE=.\common\gan_err.h
# End Source File
# Begin Source File

SOURCE=.\common\gan_err_trace.h
# End Source File
# Begin Source File

SOURCE=.\common\linked_list.h
# End Source File
# Begin Source File

SOURCE=.\common\memory_stack.h
# End Source File
# Begin Source File

SOURCE=.\common\misc_defs.h
# End Source File
# Begin Source File

SOURCE=.\common\misc_error.h
# End Source File
# Begin Source File

SOURCE=.\common\numerics.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\image.h
# End Source File
# Begin Source File

SOURCE=.\linalg.h
# End Source File
# Begin Source File

SOURCE=.\vision.h
# End Source File
# End Group
# End Target
# End Project
