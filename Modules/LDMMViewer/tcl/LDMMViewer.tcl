#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        LDMMViewer.tcl
# PROCEDURES:  
#   LDMMViewerInit
#   LDMMViewerBuildGUI
#   LDMMViewerEnter
#   LDMMViewerExit
#   LDMMViewerCount
#   LDMMViewerShowFile
#   LDMMViewerBindingCallback
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
# This module views Large Deformation Metric Mapping (LDMM)
# files generated by JHU.
# To find it when you run the Slicer, click on More->LDMMViewer.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC LDMMViewerInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerInit {} {
    global LDMMViewer Module Volume Model

    set m LDMMViewer
    
    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "View Large Deformation Metric Mapping files."
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Steve Pieper, SPL, pieper@bwh.harvard.edu"

    # Define Tabs
    #------------------------------------
    # Description:
    #   Each module is given a button on the Slicer's main menu.
    #   When that button is pressed a row of tabs appear, and there is a panel
    #   on the user interface for each tab.  If all the tabs do not fit on one
    #   row, then the last tab is automatically created to say "More", and 
    #   clicking it reveals a second row of tabs.
    #
    #   Define your tabs here as shown below.  The options are:
    #   
    #   row1List = list of ID's for tabs. (ID's must be unique single words)
    #   row1Name = list of Names for tabs. (Names appear on the user interface
    #              and can be non-unique with multiple words.)
    #   row1,tab = ID of initial tab
    #   row2List = an optional second row of tabs if the first row is too small
    #   row2Name = like row1
    #   row2,tab = like row1 
    #
    set Module($m,row1List) "Help LDMM"
    set Module($m,row1Name) "{Help} {LDMM}"
    set Module($m,row1,tab) LDMM



    # Define Procedures
    #------------------------------------
    # Description:
    #   The Slicer sources all *.tcl files, and then it calls the Init
    #   functions of each module, followed by the VTK functions, and finally
    #   the GUI functions. A MRML function is called whenever the MRML tree
    #   changes due to the creation/deletion of nodes.
    #   
    #   While the Init procedure is required for each module, the other 
    #   procedures are optional.  If they exist, then their name (which
    #   can be anything) is registered with a line like this:
    #
    #   set Module($m,procVTK) LDMMViewerBuildVTK
    #
    #   All the options are:
    #
    #   procGUI   = Build the graphical user interface
    #   procVTK   = Construct VTK objects
    #   procMRML  = Update after the MRML tree changes due to the creation
    #               of deletion of nodes.
    #   procEnter = Called when the user enters this module by clicking
    #               its button on the main menu
    #   procExit  = Called when the user leaves this module by clicking
    #               another modules button
    #   procCameraMotion = Called right before the camera of the active 
    #                      renderer is about to move 
    #   procStorePresets  = Called when the user holds down one of the Presets
    #               buttons.
    #   procRecallPresets  = Called when the user clicks one of the Presets buttons
    #               
    #   Note: if you use presets, make sure to give a preset defaults
    #   string in your init function, of the form: 
    #   set Module($m,presets) "key1='val1' key2='val2' ..."
    #   
    set Module($m,procGUI) LDMMViewerBuildGUI
    set Module($m,procEnter) LDMMViewerEnter
    set Module($m,procExit) LDMMViewerExit

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
    set Module($m,depend) ""

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.4 $} {$Date: 2003/09/08 13:28:11 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set LDMMViewer(dir)  ""
    set LDMMViewer(time)  0
    set LDMMViewer(increment)  1 ;# for StepFrame in play mode

}


# NAMING CONVENTION:
#-------------------------------------------------------------------------------
#
# Use the following starting letters for names:
# t  = toplevel
# f  = frame
# mb = menubutton
# m  = menu
# b  = button
# l  = label
# s  = slider
# i  = image
# c  = checkbox
# r  = radiobutton
# e  = entry
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC LDMMViewerBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerBuildGUI {} {
    global Gui LDMMViewer Module Volume Model
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "LDMM" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(LDMMViewer,fLDMM)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # LDMM
    #   Top
    #   Middle
    #   Bottom
    #     FileLabel
    #     CountDemo
    #     TextBox
    #-------------------------------------------
    
    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The LDMMViewer expects data formatted in .vtk files as generated by LDMM.
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>LDMM:</B> Select the base directory that contains the velocity_fields and deformed_template directories
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags LDMMViewer $help
    MainHelpBuildGUI LDMMViewer
    
    #-------------------------------------------
    # LDMM frame
    #-------------------------------------------
    set fLDMM $Module(LDMMViewer,fLDMM)
    set f $fLDMM
    
    foreach frame "Top Middle Bottom" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # LDMM->Top frame
    #-------------------------------------------
    set f $fLDMM.fTop
    
    # Add menus that list models and volumes
    DevAddButton $f.browse "Pick Directory..." LDMMViewerSetDirectory 

    pack $f.browse
    
    #-------------------------------------------
    # LDMM->Middle frame
    #-------------------------------------------
    set f $fLDMM.fMiddle
    DevAddButton $f.build "Make Models" LDMMViewerMakeModels 
    DevAddButton $f.showvectors "Show Vectors" LDMMViewerShowVectors 
    DevAddButton $f.hidevectors "Hide Vectors" LDMMViewerHideVectors 
    pack $f.build $f.showvectors $f.hidevectors

    #-------------------------------------------
    # LDMM->Bottom frame
    #-------------------------------------------
    set f $fLDMM.fBottom

    DevAddLabel $f.label "Time"
    eval {scale $f.time -from 0 -to 9 \
            -length 220 -resolution 1 \
            -command LDMMViewerSetTime } \
            $::Gui(WSA) {-sliderlength 22}
    set ::LDMMViewer(timescale) $f.time

    if { ![catch "package require iSlicer"] } {
        istask $f.play -taskcommand LDMMViewerStepFrame
        pack $f.play
    }

    DevAddButton $f.movie "Movie" LDMMViewerMovie 
    pack $f.label $f.time $f.movie
}

#-------------------------------------------------------------------------------
# .PROC LDMMViewerEnter
# Called when this module is entered by the user.  Place holder.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerEnter {} {
    global LDMMViewer
}

#-------------------------------------------------------------------------------
# .PROC LDMMViewerExit
# Called when this module is exited by the user.   Place holder.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerExit {} {
}

#-------------------------------------------------------------------------------
# .PROC LDMMViewerSetDirectory
#
# set the LDMM source directory and read the image data
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerSetDirectory { {dir ""} } {
    
    if { $dir == "" } {
        set dir [tk_chooseDirectory]
    }
    if { ![file isdirectory $dir/deformed_template] } {
        DevErrorWindow "$dir doesn't appear to be an LDMM directory"
        return
    }
    set ::LDMMViewer(dir) $dir

    LDMMViewerLoadVolumes
}


#-------------------------------------------------------------------------------
# .PROC LDMMViewerLoadStructuredPoints 
# Read a .vtk file
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerLoadStructuredPoints { vtkfile } {

    catch "spr Delete"
    vtkStructuredPointsReader spr
    spr SetFileName $vtkfile
    spr Update
    set id [spr GetOutput]

    # add a mrml node
    set n [MainMrmlAddNode Volume]
    set i [$n GetID]
    MainVolumesCreate $i

    # set the name and description of the volume
    $n SetName [file tail $vtkfile]
    $n SetDescription "$vtkfile"

    eval Volume($i,node) SetSpacing [$id GetSpacing]
    Volume($i,node) SetScanOrder PA
    Volume($i,node) SetNumScalars [$id GetNumberOfScalarComponents]
    set ext [$id GetWholeExtent]
    Volume($i,node) SetImageRange [lindex $ext 4] [lindex $ext 5]
    Volume($i,node) SetScalarType [$id GetScalarType]
    Volume($i,node) SetDimensions [lindex [$id GetDimensions] 0] [lindex [$id GetDimensions] 1]
    Volume($i,node) ComputeRasToIjkFromScanOrder [::Volume($i,node) GetScanOrder]

    MainUpdateMRML

    Volume($i,vol) SetImageData $id

    Slicer SetOffset 0 0
    MainSlicesSetVolumeAll Back $i
    MainVolumesSetActive $i
    set ::Volume(autoThreshold) 1
    MainVolumesSetParam "ApplyThreshold" 1
    MainVolumesSetParam "LowerThreshold" 128
    MainVolumesSetParam "UpperThreshold" 255
    RenderAll

    spr Delete

    return $i
}

#-------------------------------------------------------------------------------
# .PROC LDMMViewerLoadVolumes 
# Read a series of .vtk structured points files
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerLoadVolumes { } {

    set files [lsort -dictionary [glob $::LDMMViewer(dir)/deformed_template/*.vtk]]
    set t 0
    foreach f $files { 
        puts "reading $f"
        set ::LDMMViewer($t,f) $f
        set ::LDMMViewer($t,id) [LDMMViewerLoadStructuredPoints $f]
        set ::LDMMViewer(lastindex) $t
        incr t
    }
    set ::View(fov) 50
    MainViewSetFov
}

#-------------------------------------------------------------------------------
# .PROC LDMMViewerMakeModels 
# Create models from the volume files
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerMakeModels { } {
    for {set t 0} {$t <= $::LDMMViewer(lastindex)} {incr t} {
        EditorSetOriginal $::LDMMViewer($t,id)
        Tab Editor row1 Effects
        EditorSetEffect EdThreshold
        set ::Ed(EdThreshold,lower) 128
        set ::Ed(EdThreshold,upper) 255
        EdThresholdUpdate 
        EdThresholdApply
        Tab Editor row1 Effects
        EditorMakeModel 
        set ::ModelMaker(name) [file tail [file root $::LDMMViewer($t,f)]]
        set ::LDMMViewer($t,m) [ModelMakerCreate]
        update
    }
    MainSlicesSetOrientAll AxiSagCor
    MainSlicesSetVisibilityAll 0
    update
}

#-------------------------------------------------------------------------------
# .PROC LDMMViewerShowVectors 
#
# create vector glyphs from ldmm files
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerShowVectors {} {

    LDMMViewerHideVectors 

    set ::LDMMViewer(vectorfiles) [lsort -dictionary [glob $::LDMMViewer(dir)/velocity_fields/*]]

    foreach f $::LDMMViewer(vectorfiles) { 
        puts "reading $f"
        catch "LDMMspr_$f Delete"
        vtkStructuredPointsReader LDMMspr_$f
        LDMMspr_$f SetFileName $f
        LDMMspr_$f Update
        [LDMMspr_$f GetOutput] SetOrigin -32 -32 -32
    }

    set f0 [lindex $::LDMMViewer(vectorfiles) 0]
    vtkExtractVOI LDMMevoi
    LDMMevoi SetSampleRate 4 4 4
    LDMMevoi SetInput [LDMMspr_$f0 GetOutput]

    vtkGlyph3D LDMMg3d
    LDMMg3d SetInput [LDMMevoi GetOutput]
    LDMMg3d SetColorModeToColorByVector
    LDMMg3d SetScaleModeToScaleByVector
    LDMMg3d SetScaleFactor 2

    vtkPolyDataMapper LDMMgmapper
    LDMMgmapper SetInput [LDMMg3d GetOutput]

    vtkActor LDMMgactor
    LDMMgactor SetMapper LDMMgmapper

    viewRen AddActor LDMMgactor
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC LDMMViewerHideVectors 
#
# remove vector glyphs from scene
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerHideVectors {} {

    catch "viewRen RemoveActor LDMMgactor"
    if { [info exists ::LDMMViewer(vectorfiles)] } { 
        foreach f $::LDMMViewer(vectorfiles) { 
            catch "LDMMspr_$f Delete"
        }
    }
    catch "LDMMspr Delete"
    catch "LDMMevoi Delete"
    catch "LDMMg3d Delete"
    catch "LDMMgmapper Delete"
    catch "LDMMgactor Delete"
    Render3D
}



#-------------------------------------------------------------------------------
# .PROC LDMMViewerStepFrame
#
#  adjust the frame according to current increment and handle boundaries
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerStepFrame { } {

    set first 0
    set last 9
    set inc $::LDMMViewer(increment) 
    set t $::LDMMViewer(time) 

    set t [expr $t + $inc]

    if {$t > $last} {
        set t $last
        set ::LDMMViewer(increment) -1
    } 
    if {$t < $first} {
        set t $first 
        set ::LDMMViewer(increment) 1
    } 
    LDMMViewerSetTime $t
}



#-------------------------------------------------------------------------------
# .PROC LDMMViewerSetTime
#
#  Show the models and volume for the current time
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerSetTime { {t ""} } {

    # if time hasn't been changed, return
    if { $t != "" } {
        if { $t == $::LDMMViewer(time) } {
            return
        }
    }
    if { $t == "" } {
        set t $::LDMMViewer(time)
    } else {
        set ::LDMMViewer(time) $t
    }
    $::LDMMViewer(timescale) set $t

    if { ![info exists ::LDMMViewer(lastindex)] } {
        # no data loaded yet
        return
    }

    # turn off all models
    for {set tt 0} {$tt <= $::LDMMViewer(lastindex)} {incr tt} {
        if { [info exists ::LDMMViewer($tt,m)] } {
            MainModelsSetVisibility $::LDMMViewer($tt,m) 0
        }
    }
    # then turn on the one for the current time
    if { [info exists ::LDMMViewer($t,m)] } {
        MainModelsSetVisibility $::LDMMViewer($t,m) 1
    }

    # set the background volume to this time step
    MainSlicesSetVolumeAll Back $::LDMMViewer($t,id)

    # set the vector input 
    if { [info commands LDMMevoi] != "" } {
        set f [lindex $::LDMMViewer(vectorfiles) $t]
        LDMMevoi SetInput [LDMMspr_$f GetOutput]
    }

    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC LDMMViewerRenderNTimes 
#
# to stretch out scenes in the movie or add pauses
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerRenderNTimes {N} {
    for {set n 0} {$n < $N} {incr n} {
        RenderAll
    }
}

#-------------------------------------------------------------------------------
# .PROC LDMMViewerMovie 
#
# make an animated series of views
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerMovie {} {
    
    set directions "R L A P S I"
    set xList  "10 65 55 17 40 40"
    set yList  "36 23 42 13 10 65"
    set dir(R,x) 10; set dir(R,y) 36
    set dir(L,x) 65; set dir(L,y) 23
    set dir(A,x) 55; set dir(A,y) 42
    set dir(P,x) 17; set dir(P,y) 13
    set dir(S,x) 40; set dir(S,y) 10
    set dir(I,x) 40; set dir(I,y) 65

    
    LDMMViewerRenderNTimes 5
    foreach view {A R P L I S A} {
        MainViewNavReset [set dir($view,x)] [set dir($view,y)] click
        LDMMViewerRenderNTimes 3
        for {set tt 0} {$tt <= $::LDMMViewer(lastindex)} {incr tt} {
            LDMMViewerSetTime $tt
            update
        }
        LDMMViewerRenderNTimes 5
        for {set tt $::LDMMViewer(lastindex)} {$tt >= 0} {incr tt -1} {
            LDMMViewerSetTime $tt
            update
        }
        LDMMViewerRenderNTimes 3
    }
    LDMMViewerRenderNTimes 5
    MainViewNavReset $dir(A,x) $dir(A,y) click
    LDMMViewerSetTime 0
    LDMMViewerRenderNTimes 5
}

#-------------------------------------------------------------------------------
# .PROC LDMMViewerBatchRender 
#
# save a movie of ldmm views to an mpeg file.  Meant to be used with the
# --exec command line flag but could be typed to the console
# .END
#-------------------------------------------------------------------------------
proc LDMMViewerBatchRender { dir } {

    if { ![info exists ::env(IMAGEMAGICK_CONVERT)] } {
        set paths { 
            /usr/X11R6/bin/convert 
            "c:/Program Files/ImageMagick-5.5.7-Q16/convert.exe"
        }
        foreach path $paths {
            if { [file exists $path] } {
                if { $::tcl_platform(platform) == "windows" } {
                    set ::env(IMAGEMAGICK_CONVERT) [file attributes $path -shortname]
                } else {
                    set ::env(IMAGEMAGICK_CONVERT) $path
                }
            }
        }
    }
    if { ![info exists ::env(IMAGEMAGICK_CONVERT)] } {
        puts stderr "Set environment variable IMAGEMAGIC_CONVERT to the convert command"
        exit
    }

    LDMMViewerSetDirectory $dir
    LDMMViewerShowVectors
    LDMMViewerMakeModels

    Tab LDMMViewer

    raise .tViewer; update
    MainViewerSetMode 3D ; update 

    set ::Save(imageSaveMode) Movie
    set ::Save(imageDirectory) $::LDMMViewer(dir)/movies/frames
    if { ![file exists $::Save(imageDirectory)] } {
        file mkdir $::Save(imageDirectory)
    }
    set ::Save(imageFrameCounter) 0

    LDMMViewerMovie

    set ::Save(imageSaveMode) "Single view"

    puts "Encoding MPEG Movie..."; update
    set files [glob $::Save(imageDirectory)/*.png]
    set fp [open $::Save(imageDirectory)/files "w"]
    puts $fp $files
    close $fp

    set ret [catch "exec $::env(IMAGEMAGICK_CONVERT) @$::Save(imageDirectory)/files $::LDMMViewer(dir)/movies/preview.mpg" res]
    puts "Output from convert:\n$res"
    file delete -force $::Save(imageDirectory)

    puts "finished"
}

