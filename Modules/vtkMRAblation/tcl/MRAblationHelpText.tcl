#=auto==========================================================================
#   Portions (c) Copyright 2006 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: MRAblationHelpText.tcl,v $
#   Date:      $Date: 2006/03/23 18:47:59 $
#   Version:   $Revision: 1.1.2.5 $
# 
#===============================================================================
# FILE:        MRAblationHelpText.tcl
# PROCEDURES:  
#   MRAblationHelpSpecifyImages
#   MRAblationHelpSpecifyParameters
#==========================================================================auto=

proc MRAblationGetHelpWinID { } {
    if {! [info exists ::MRAblation(newID) ]} {
        set ::MRAblation(newID) 0
    }
    incr ::MRAblation(newID)
    return $::MRAblation(newID)
}


#-------------------------------------------------------------------------------
# .PROC MRAblationHelpSpecifyImages
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationHelpSpecifyImages { } {
    #--- Compute 
    #--- compute thermal volumes 
    set i [ MRAblationGetHelpWinID ]
    set txt "<H3>Specifying images for thermal volume computation</H3>

<P> Thermal volumes are computed from the input images at different timepoints.
<BR>
<P> <B>Cold images</B>: In general, images in timepoint 1 are cold images. However, you can choose cold images from any available timepoints. 
<BR> <B>Hot images</B>: You may pick up any timepoint (except that for cold images) for hot images or just choose \"all\" (if any) on the list. If a specific timepoint is chosen, only one thermal volume will be computed. If \"all\" is selected, multiple thermal volumes will be generated, one for each hot timepoint.
<BR> <B>Magnitude images</B>: Select your magnitude images. 
<BR> <B>Real images</B>: Select your real images. 
<BR> <B>Imaginary images</B>: Select your imaginary images. 

<P> If button <B>One volume per timepoint</B> is checked, timepoint 1 will be automatically chosen for cold images and \"all\" selected for hot images if at least 3 phases are available within the experiment." 

    DevCreateTextPopup infowin$i "MRAblation information" 100 100 18 $txt
}

#-------------------------------------------------------------------------------
# .PROC MRAblationHelpSpecifyExperiment
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationHelpSpecifyExperiment { } {
    #--- Compute 
    #--- compute thermal volumes 
    set i [ MRAblationGetHelpWinID ]
    set txt "<H3>Specifying the experiment</H3>

<P> <B>TE</B>: Time to excitation, a pulse sequence parameter, measured in milliseconds (msec). Input TE here in seconds. 
<BR> <B>w0</B>: Operating frequency of maganet.
<BR> <B>TC</B>: Conversion coefficient, in units of ppm/C, for phase shift in ppm to temperature. It is usually named <I>alpha</I>. 
<P> Recommended values for laser therapy in the brain at 0.5T:
<BR>        TE = 0.020
<BR>        w0 = 21.3
<BR>        TC = 0.01076"

    DevCreateTextPopup infowin$i "MRAblation information" 100 100 18 $txt
}
