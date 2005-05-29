
proc DTMRISaveInit {} {

    global DTMRI

}


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
