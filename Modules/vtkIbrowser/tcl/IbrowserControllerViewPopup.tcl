#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: IbrowserControllerViewPopup.tcl,v $
#   Date:      $Date: 2005/12/20 22:56:00 $
#   Version:   $Revision: 1.2.8.1 $
# 
#===============================================================================
# FILE:        IbrowserControllerViewPopup.tcl
# PROCEDURES:  
#   IbrowserSetupViewPopupImages
#   IbrowserMakeViewMenu
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC IbrowserSetupViewPopupImages
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserSetupViewPopupImages { } {
    global PACKAGE_DIR_VTKIbrowser
    
    set tmpstr $PACKAGE_DIR_VTKIbrowser
    set tmpstr [string trimright $tmpstr "/vtkIbrowser" ]
    set tmpstr [string trimright $tmpstr "/Tcl" ]
    set tmpstr [string trimright $tmpstr "Wrapping" ]
    set modulePath [format "%s%s" $tmpstr "tcl/"]

    set ::IbrowserController(Images,Menu,sliceOneVolLO) \
        [ image creat photo -file ${modulePath}iconPix/20x20/gifs/controls/singleVolSliceLO.gif ]
    set ::IbrowserController(Images,Menu,sliceOneVolHI) \
        [ image creat photo -file ${modulePath}iconPix/20x20/gifs/controls/singleVolSliceHI.gif ]
    set ::IbrowserController(Images,Menu,sliceMultiVolLO) \
        [ image creat photo -file ${modulePath}iconPix/20x20/gifs/controls/multiVolSliceLO.gif ]
    set ::IbrowserController(Images,Menu,sliceMultiVolHI) \
        [ image creat photo -file ${modulePath}iconPix/20x20/gifs/controls/multiVolSliceHI.gif ]
    set ::IbrowserController(Images,Menu,MultiVolVoxLO) \
        [ image creat photo -file ${modulePath}iconPix/20x20/gifs/controls/multiVolVoxelLO.gif ]
    set ::IbrowserController(Images,Menu,MultiVolVoxHI) \
        [ image creat photo -file ${modulePath}iconPix/20x20/gifs/controls/multiVolVoxelHI.gif ]

}

#-------------------------------------------------------------------------------
# .PROC IbrowserMakeViewMenu
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMakeViewMenu { fr } {

    #pack all labels in the same frame that Animation Menu is in.
    #named .fibAnimControl
    label $fr.lviewMultiVox -background white \
        -image $::IbrowserController(Images,Menu,MultiVolVoxLO) -relief flat
    bind $fr.lviewMultiVox  <Enter> {
        %W config -image $::IbrowserController(Images,Menu,MultiVolVoxHI) }
    bind $fr.lviewMultiVox <Leave> {
        %W config -image $::IbrowserController(Images,Menu,MultiVolVoxLO) }    

    label $fr.lviewMatrixOneVol -background white \
        -image $::IbrowserController(Images,Menu,sliceOneVolLO) -relief flat
    bind $fr.lviewMatrixOneVol <Enter> {
        %W config -image $::IbrowserController(Images,Menu,sliceOneVolHI) }
    bind $fr.lviewMatrixOneVol <Leave> {
        %W config -image $::IbrowserController(Images,Menu,sliceOneVolLO) }    

    label $fr.lviewMatrixMultiVol -background white \
        -image $::IbrowserController(Images,Menu,sliceMultiVolLO) -relief flat
    bind $fr.lviewMatrixMultiVol <Enter> {
        %W config -image $::IbrowserController(Images,Menu,sliceMultiVolHI) }
    bind $fr.lviewMatrixMultiVol <Leave> {
        %W config -image $::IbrowserController(Images,Menu,sliceMultiVolLO) }    

    pack $fr.lviewMultiVox -side right
    pack $fr.lviewMatrixOneVol -side right
    pack $fr.lviewMatrixMultiVol -side right
}

