#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for 
# research purposes only, provided that (1) the above copyright notice and 
# the following four paragraphs appear on all copies of this software, and 
# (2) that source code to any modifications to this software be made 
# publicly available under terms no more restrictive than those in this 
# License Agreement. Use of this software constitutes acceptance of these 
# terms and conditions.
# 
# 3D Slicer Software has not been reviewed or approved by the Food and 
# Drug Administration, and is for non-clinical, IRB-approved Research Use 
# Only.  In no event shall data or images generated through the use of 3D 
# Slicer Software be used in the provision of patient care.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
# OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
# NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
#
#===============================================================================
# FILE:        DTMRISave.tcl
# PROCEDURES:  
#   DTMRISaveInit
#   DTMRISaveBuildGUI
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC DTMRISaveInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRISaveInit {} {

    global DTMRI

}



#-------------------------------------------------------------------------------
# .PROC DTMRISaveBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRISaveBuildGUI {} {

    global DTMRI Tensor Module Gui Volume

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Save
    #-------------------------------------------

    #-------------------------------------------
    # Save frame
    #-------------------------------------------
    set fSave $Module(DTMRI,fSave)
    set f $fSave
    
    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fActive -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    
    foreach frame "Top Middle Bottom" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    $f.fTop configure  -relief groove -bd 3 
    $f.fMiddle configure  -relief groove -bd 3 
    $f.fBottom configure  -relief groove -bd 3 

    #-------------------------------------------
    # Save->Active frame
    #-------------------------------------------
    set f $fSave.fActive

    # menu to select active DTMRI
    DevAddSelectButton  Tensor $f Active "Active DTMRI:" Pack \
    "Active DTMRI" 20 BLA 
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Tensor(mbActiveList) $f.mbActive
    lappend Tensor(mActiveList) $f.mbActive.m

    #-------------------------------------------
    # Save->Top frame
    #-------------------------------------------
    set f $fSave.fTop

    DevAddButton $f.bSave "Save Tensors" {DTMRIWriteStructuredPoints $DTMRI(devel,fileName)}
    pack $f.bSave -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd $f.bSave "Save tensor data (Active DTMRI) to vtk file format."

    #-------------------------------------------
    # Save->Middle frame
    #-------------------------------------------
    set f $fSave.fMiddle
    frame $f.fButton  -bg $Gui(activeWorkspace)
    pack $f.fButton -side top -padx $Gui(pad) -pady $Gui(pad)

    set f $fSave.fMiddle.fButton
    DevAddButton $f.bApply "Save tract points" \
        {DTMRISaveStreamlinesAsIJKPoints}
    TooltipAdd $f.bApply "Save text file(s) with the tract paths.\n This does not save vtk models."

    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad) 


    #-------------------------------------------
    # Save->Bottom frame
    #-------------------------------------------
    set f $fSave.fBottom

    foreach frame "Entries" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady 1 -fill x
    }

    #-------------------------------------------
    # Save->Bottom->Entries frame
    #-------------------------------------------
    set f $fSave.fBottom.fEntries
    foreach frame "Info1 Apply1 Info2 Apply2" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------
    # Save->Bottom->Entries->Info1 frame
    #-------------------------------------------
    set f $fSave.fBottom.fEntries.fInfo1
    DevAddLabel $f.l "Save the tracts you have created."
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Save->Bottom->Entries->Apply1 frame
    #-------------------------------------------
    set f $fSave.fBottom.fEntries.fApply1
    DevAddButton $f.bApply "Save tracts in model file(s)" \
        {puts "Saving streamlines"; DTMRISaveStreamlinesAsModel}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $f.bApply "Save tracts to vtk file(s).\nEach color of tract will become a separate model.\n Choose the initial part of the filename, and models\nwill be saved as filename_0.vtk, filename_1.vtk, etc.\nThen you can load the models into slicer\n(they must be re-added to the mrml tree)."

    #-------------------------------------------
    # Save->Bottom->Entries->Info2 frame
    #-------------------------------------------
    set f $fSave.fBottom.fEntries.fInfo2
    DevAddLabel $f.l "Save currently visible tracts as a polyline.\n Useful for further processing on tracts.\nONLY DEVELOPERS"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Save->Bottom->Entries->Apply2 frame
    #-------------------------------------------
    set f $fSave.fBottom.fEntries.fApply2
    DevAddButton $f.bApply "Save tracts in vtk file" \
        {puts "Saving streamlines"; DTMRISaveStreamlinesAsPolyLines "" tracts}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $f.bApply "Save visible tracts to vtk file as a set of polylines."


}
