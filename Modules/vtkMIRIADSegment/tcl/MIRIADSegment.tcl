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
# FILE:        MIRIADSegment.tcl
# PROCEDURES:  
#   MIRIADSegmentInit
#   MIRIADSegmentBuildGUI
#   MIRIADSegmentEnter
#   MIRIADSegmentExit
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
# This module support BIRN MIRIAD project segmentations
# To find it when you run the Slicer, click on More->MIRIADSegment.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentInit {} {
    global MIRIADSegment Module Volume Model

    set m MIRIADSegment
    
    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "Perform Deidentification and Upload."
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
    set Module($m,row1List) "Help MIRIADSegment"
    set Module($m,row1Name) "{Help} {MIRIADSegment}"
    set Module($m,row1,tab) MIRIADSegment



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
    #   set Module($m,procVTK) MIRIADSegmentBuildVTK
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
    set Module($m,procGUI) MIRIADSegmentBuildGUI
    set Module($m,procEnter) MIRIADSegmentEnter
    set Module($m,procExit) MIRIADSegmentExit

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
        {$Revision: 1.9 $} {$Date: 2003/11/05 03:32:47 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set MIRIADSegment(archive)  "gpop.bwh.harvard.edu:/nas/nas0/pieper/data/MIRIAD/Project_0002"
    set MIRIADSegment(subject_dir)  ""

    set tmpdirs {/state/partition1/pieper /usr/tmp /tmp}
    foreach tmpdir $tmpdirs {
        if { [file exists $tmpdir] } {
            set MIRIADSegment(tmpdir) $tmpdir
            break
        }
    }

    if { [file exists /usr/bin/rsync] } {
        set MIRIADSegment(rsync) /usr/bin/rsync
    } else {
        set MIRIADSegment(rsync) $::env(HOME)/birn/bin/rsync
    }

    if { [file exists $::env(HOME)/birn/data/atlas] } {
        set ::MIRIADSegment(splatlas) $::env(HOME)/birn/data/atlas
    } else {
        set ::MIRIADSegment(splatlas) $::env(HOME)/data/atlas
    }
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
# .PROC MIRIADSegmentBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentBuildGUI {} {
    global Gui MIRIADSegment Module Volume Model
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "LDMM" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(MIRIADSegment,fLDMM)
    
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
    The MIRIADSegment Module is used to invoke the EMSegment algoritm on MIRIAD data.
    <BR>
    <LI><B>CREDIT:</B> Steve Pieper, Neil Weisenfeld, Kilian Pohl and the Morphometry BIRN
    <BR>
    <LI><B>CREDIT:</B> See www.nbirn.net for BIRN details.
    <P>
    Description by tab:  This module has scripts that are primarily used in batch mode.
    <BR>
    <UL>
    <LI><B>MIRIADSegment:</B> 
    <BR>
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags MIRIADSegment $help
    MainHelpBuildGUI MIRIADSegment
    
# DDD1
    #-------------------------------------------
    # Deface frame
    #-------------------------------------------
    set fDeface $Module(MIRIADSegment,fMIRIADSegment)
    set f $fDeface
    # Frames
    frame $f.fActive -bg $Gui(backdrop) -relief sunken -bd 2 -height 20
    frame $f.fRange  -bg $Gui(activeWorkspace) -relief flat -bd 3

    pack $f.fActive -side top -pady $Gui(pad) -padx $Gui(pad)
    pack $f.fRange  -side top -pady $Gui(pad) -padx $Gui(pad) -fill x



    #-------------------------------------------
    # Deface->Active frame
    #-------------------------------------------
    set f $fDeface.fActive

    eval {label $f.lActive -text "Active Volume: "} $Gui(BLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
        -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbActive
    lappend Volume(mActiveList)  $f.mbActive.m

    #-------------------------------------------
    # Deface->Range frame
    #-------------------------------------------
    set f $fDeface.fRange

    eval {button $f.select -text "Load Atlas" -width 20 -command "MIRIADSegmentLoadSPLAtlas"} $Gui(WBA)
    
    pack $f.select -pady $Gui(pad) -side top -fill y -expand 1

# DDD2 
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentEnter
# Called when this module is entered by the user.  Place holder.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentEnter {} {
    global MIRIADSegment
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentExit
# Called when this module is exited by the user.   Place holder.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentExit {} {
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentProcessStudy
# 
# Main entry point for the Module
# Read the dicom data and the atlas for a subject, runs the segmentation and saves results
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentProcessStudy { {BIRNID "000397921927"} {visit 001} } {

    set ::MIRIADSegment(version) 1

    MIRIADSegmentLoadStudy $BIRNID $visit 

    MIRIADSegmentSetEMParameters
    MIRIADSegmentRunEM

    MIRIADSegmentSaveResults 
    
    puts "MIRIADSegment Finished"
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentLoadStudy
# Read the dicom data and the atlas for a subject, runs the segmentation and saves results
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentLoadStudy { {BIRNID "000397921927"} {visit 001} {atlas "spl"} } {

    MainFileClose 

    #
    # first, make the local directory for the data
    #
    set ::MIRIADSegment(subject_dir) $::MIRIADSegment(tmpdir)/$BIRNID/Visit_$visit/Study_0001
    file mkdir $::MIRIADSegment(subject_dir)
    set ::MIRIADSegment(archive_dir) $::MIRIADSegment(archive)/${BIRNID}/Visit_$visit/Study_0001

    #
    # then, bring over the data with rsync and load it up
    #
    puts "rsyncing..." ; update
    exec $::MIRIADSegment(rsync) -rz --rsh=ssh $::MIRIADSegment(archive_dir)/ $::MIRIADSegment(subject_dir)/

    puts "loading raw..." ; update
    MIRIADSegmentLoadDukeStudy $::MIRIADSegment(subject_dir)/RawData/001.ser

    #
    # either read the existing warped atlas, or create it
    #
    puts "loading atlas..." ; update
    if { $atlas == "loni" } {
        MIRIADSegmentLoadLONIWarpedAtlas $::MIRIADSegment(subject_dir)/DerivedData/LONI/mri/atlases/bwh_prob/air_252p
    } else {
        if { [MIRIADSegmentLoadSPLWarpedAtlas] } {
            puts "creating atlas..." ; update
            MIRIADSegmentLoadSPLAtlas $::MIRIADSegment(splatlas) 
            MIRIADSegmentCreateSPLWarpedAtlas 
        }
    }
    puts "done" ; update

}


#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentSaveResults
# Save the EM results
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentSaveResults { } {

    set SEGid [MIRIADSegmentGetVolumeByName "EMSegResult1"]
    set resultdir $::MIRIADSegment(subject_dir)/DerivedData/SPL/EM-$::MIRIADSegment(version)
    file mkdir $resultdir
    MainVolumesWrite $SEGid $resultdir/EMSegResult

    set ::Mrml(dir) $resultdir 
    set ::File(filePrefix) AtlasAndSegmentation
    MainFileSaveAsApply

    puts "saving results to archive..." ; update
    exec $::MIRIADSegment(rsync) -rz --rsh=ssh $::MIRIADSegment(subject_dir)/ $::MIRIADSegment(archive_dir)/

}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentLoadDukeStudy 
# Reads the bwh probability atlas
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentLoadDukeStudy { {dir "choose"} } {

    if { $dir == "choose"} {
        set initialdir /home/pieper/data/duke-data/neil/MIRIAD/subjects/000397921927/Visit_001/Study_0001/RawData/001.ser
        set dir [tk_chooseDirectory -initialdir $initialdir]
        if { $dir == "" } {
            return
        }
    }

    MIRIADSegmentDeleteVolumeByName "T2"
    MIRIADSegmentDeleteVolumeByName "PD"

    set T2id [DICOMLoadStudy $dir *\[02468\].dcm]
    set PDid [DICOMLoadStudy $dir *\[13579\].dcm]

    Volume($T2id,node) SetName "T2"
    Volume($PDid,node) SetName "PD"

    MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentLoadAtlas 
# Reads the bwh probability atlas
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentLoadSPLAtlas { {dir "choose"} } {

    if { $dir == "choose"} {
        set dir [tk_chooseDirectory]
        if { $dir == "" } {
            return
        }
    }

    set vols {
        case2/spgr/case2.001
        case2/t2w/case2.001
        sumbackground/I.001
        sumcsf/I.001
        sumgreymatter/I.001
        sumwhitematter/I.001
    }
        
    foreach vol $vols {
        set name atlas-[file tail [file dir $vol]]
        regsub -all "\\." $name "" name
        MIRIADSegmentDeleteVolumeByName $name

        MainVolumesSetActive "NEW"
        set ::Volume(name) $name
        set ::Volume(desc) "SPL Atlas $vol"
        set ::Volume(firstFile) $dir/$vol
        set ::Volume(lastNum) 124
        set ::Volume(isDICOM) 0
        set ::Volume(width) 256
        set ::Volume(height) 256
        set ::Volume(pixelWidth) .9375
        set ::Volume(pixelHeight) .9375
        set ::Volume(sliceThickness) 1.5
        set ::Volume(sliceSpacing) 0
        set ::Volume(gantryDetectorTilt) 0
        set ::Volume(numScalars) 1
        set ::Volume(readHeaders) 0
        set ::Volume(labelMap) 0
        set ::Volume(scanOrder) "PA"
        set ::Volume(scalarType) "Short"
        VolumesPropsApply 
    }
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentCreateSPLWarpedAtlas 
# Make a bwh probability atlas as warped by vtkAG
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentCreateSPLWarpedAtlas {} {

    # 
    # require that duke data and bwh atlas are already loaded
    #
    set ::AG(InputVolTarget) [MIRIADSegmentGetVolumeByName "T2"]
    set ::AG(InputVolTarget2) [MIRIADSegmentGetVolumeByName "PD"]
    set ::AG(InputVolSource) [MIRIADSegmentGetVolumeByName "atlas-t2w"]
    set ::AG(InputVolSource2) [MIRIADSegmentGetVolumeByName "atlas-spgr"]
    # special flag to Create New output volume
    set ::AG(ResultVol) -5
    set ::AG(ResultVol2) -5

    #
    # perform the registration
    #
    RunAG

    #
    # apply the transform to each of the atlas volumes and save them
    #
    foreach vol [MIRIADSegmentGetVolumesByNamePattern atlas-sum*] {
        AGTransformOneVolume $vol $::AG(InputVolTarget)
    }
    
    # save the atlas and the warped image data
    set vols [MIRIADSegmentGetVolumesByNamePattern AGResult*]
    set vols [concat $vols [MIRIADSegmentGetVolumesByNamePattern resample_atlas*]]
    foreach vol $vols {
        set name [Volume($vol,node) GetName]
        set resultdir $::MIRIADSegment(subject_dir)/DerivedData/SPL/mri/atlases/bwh_prob/AG
        file mkdir $resultdir
        MainVolumesWrite $vol $resultdir/$name
    }
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentLoadSPLWarpedAtlas 
# Reads the bwh probability atlas as warped by SPL
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentLoadSPLWarpedAtlas { } {

    # TODO - don't load the atlas now - just create a new one
    return -1


    set resultsdir $::MIRIADSegment(subject_dir)/DerivedData/SPL/mri/atlases/bwh_prob/AG
    if { ![file exists $resultsdir] } {
        return -1
    }

    set all_vols {
        sumbackground.xml sumcsf.xml sumforeground.xml 
        sumgraymatter_amygdala.xml sumgraymatter_hippocampus.xml 
        sumgraymatter_parrahipp_normed.xml sumgraymatter_stg_normed.xml 
        sumgraymatter_substr.xml sumgreymatter_all.xml sumgreymatter.xml 
        sumlamygdala.xml sumlamygdala_normed.xml sumlAnterInsulaCortex.xml 
        sumlhippocampus.xml sumlhippocampus_normed.xml sumlInferiorTG.xml 
        sumlMiddleTG.xml sumlparrahipp.xml sumlparrahipp_normed.xml 
        sumlPostInsulaCortex.xml sumlstg.xml sumlstg_normed.xml 
        sumlTempLobe.xml sumlThalamus.xml sumramygdala.xml 
        sumramygdala_normed.xml sumrAnterInsulaCortex.xml 
        sumrhippocampus.xml sumrhippocampus_normed.xml 
        sumrInferiorTG.xml sumrMiddleTG.xml sumrparrahipp.xml 
        sumrparrahipp_normed.xml sumrPostInsulaCortex.xml sumrstg.xml 
        sumrstg_normed.xml sumrTempLobe.xml sumrThalamus.xml 
        sumwhitematter.xml
    }
    set four_vols {
        sumbackground.xml sumcsf.xml 
        sumwhitematter.xml sumgreymatter.xml 
    }
        
    foreach vol $four_vols {
        MainMrmlImport $resultsdir/$vol
    }
    RenderAll
    return 0
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentLoadLONIWarpedAtlas 
# Reads the bwh probability atlas as warped by LONI
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentLoadLONIWarpedAtlas { {dir "choose"} } {

    if { $dir == "choose"} {
        set initialdir /home/pieper/data/duke-data/neil/MIRIAD/subjects/000397921927/Visit_001/Study_0001/DerivedData/LONI/mri/atlases/bwh_prob/air_252p
        set dir [tk_chooseDirectory -initialdir $initialdir]
        if { $dir == "" } {
            return
        }
    }
    set all_vols {
        sumbackground.hdr sumcsf.hdr sumforeground.hdr 
        sumgraymatter_amygdala.hdr sumgraymatter_hippocampus.hdr 
        sumgraymatter_parrahipp_normed.hdr sumgraymatter_stg_normed.hdr 
        sumgraymatter_substr.hdr sumgreymatter_all.hdr sumgreymatter.hdr 
        sumlamygdala.hdr sumlamygdala_normed.hdr sumlAnterInsulaCortex.hdr 
        sumlhippocampus.hdr sumlhippocampus_normed.hdr sumlInferiorTG.hdr 
        sumlMiddleTG.hdr sumlparrahipp.hdr sumlparrahipp_normed.hdr 
        sumlPostInsulaCortex.hdr sumlstg.hdr sumlstg_normed.hdr 
        sumlTempLobe.hdr sumlThalamus.hdr sumramygdala.hdr 
        sumramygdala_normed.hdr sumrAnterInsulaCortex.hdr 
        sumrhippocampus.hdr sumrhippocampus_normed.hdr 
        sumrInferiorTG.hdr sumrMiddleTG.hdr sumrparrahipp.hdr 
        sumrparrahipp_normed.hdr sumrPostInsulaCortex.hdr sumrstg.hdr 
        sumrstg_normed.hdr sumrTempLobe.hdr sumrThalamus.hdr 
        sumwhitematter.hdr
    }
    set four_vols {
        sumbackground.hdr sumcsf.hdr 
        sumwhitematter.hdr sumgreymatter.hdr 
    }
        
    foreach vol $four_vols {
        set name resample_atlas-[file root $vol]
        MIRIADSegmentDeleteVolumeByName $name
        MainVolumesSetActive "NEW"
        set ::Volume(VolAnalyze,FileName) $dir/$vol
        set ::Volume(name) $name
        set i [VolAnalyzeApply]
        MIRIADSegmentNormalizeImage $i 82
    }
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentSetEMParameters
# Define the parameters for the segmentation
# - this method interacts a bit with the GUI -- this ensures that 
# all the right variables get set by the callbacks
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentSetEMParameters {} {

    set ::EMSegment(SegmentMode) 1
    set ::EMSegment(DebugVolume) 1

    #
    # pick the PD and T2 volumes as the seg channels
    #
    $::EMSegment(fAllVolList) selection clear 0 end
    for {set i 0} {$i < [$::EMSegment(fAllVolList) size]} {incr i} {
        if { [$::EMSegment(fAllVolList) get $i] == "PD" } {
            $::EMSegment(fAllVolList) selection set $i
            set ::EMSegment(AllVolList,ActiveID) $i
            EMSegmentTransfereVolume All
            break
        }
    }
    $::EMSegment(fAllVolList) selection clear 0 end
    for {set i 0} {$i < [$::EMSegment(fAllVolList) size]} {incr i} {
        if { [$::EMSegment(fAllVolList) get $i] == "T2" } {
            $::EMSegment(fAllVolList) selection set $i
            set ::EMSegment(AllVolList,ActiveID) $i
            EMSegmentTransfereVolume All
            break
        }
    }

    #
    # set the global parameters
    #
    set ::EMSegment(EMiteration) 20
    set ::EMSegment(MFAiteration) 10

    set ::EMSegment(NumClassesNew) 4
    EMSegmentCreateDeleteClasses 1 1

    set ::EMSegment(Cattrib,1,Prob) .05
    set ::EMSegment(Cattrib,2,Prob) .20
    set ::EMSegment(Cattrib,3,Prob) .50
    set ::EMSegment(Cattrib,4,Prob) .25
    EMSegmentSumGlobalUpdate

    #
    # set the per-class parameters: class, atlas vol, mean, and covariance
    #
    set classes "1 2 3 4" 
    set probvols "resample_atlas-sumbackground resample_atlas-sumwhitematter resample_atlas-sumcsf resample_atlas-sumgreymatter"
    set logmeans {
        {0.5711 0.4534} {6.3364 5.0624} {4.5678 4.2802} {6.3836 5.1253}
    }
    set logcovs {
        {2.915 1.9455 1.9455 1.9985} 
        {0.0049 -0.0019 -0.0019 0.0711} 
        {7.6805 5.0207 5.0207 7.3293} 
        {0.0026 0.004 0.004 0.0562}
    }
    foreach class $classes probvol $probvols lmean $logmeans lcov $logcovs {
        EMSegmentChangeClass $class
        set ::EMSegment(ProbVolumeSelect) [MIRIADSegmentGetVolumeByName $probvol]       
        EMSegmentProbVolumeSelectNode \
            Volume [MIRIADSegmentGetVolumeByName $probvol] \
            EMSegment EM-ProbVolumeSelect ProbVolumeSelect

        set index 0
        for {set y 0} {$y < $::EMSegment(NumInputChannel)} {incr y} {
            set ::EMSegment(Cattrib,$class,LogMean,$y) [lindex $lmean $y]
            for {set x 0} {$x < $::EMSegment(NumInputChannel)} {incr x} {
                set ::EMSegment(Cattrib,$class,LogCovariance,$y,$x)  [lindex $lcov $index]
                incr index
            }
        }
    }
}


if {0} {
    #
    # some left over stuff - just for reference
    #
    set logmeans {
        "0.571072875212 0.453423712788 " 
        "6.33643656174 5.06243056865 " 
        "4.56782393077 4.28015667061 " 
        "6.50700533879 5.52666528996 " 
    }
    set logcovs {
        "2.91504731506 1.94549455788 1.94549455788 1.99850648931"
        "0.00493500976813 -0.00186271455602 -0.00186271455602 0.0711103673701 "
        "7.68049171392 5.02067293354 5.02067293354 7.32928998269 "
        "0.289870642343 0.281761623486 0.281761623486 0.506977903929 "
    }
    set ::EMSegment(Cattrib,1,Prob) .05
    set ::EMSegment(Cattrib,2,Prob) .25
    set ::EMSegment(Cattrib,3,Prob) .60
    set ::EMSegment(Cattrib,4,Prob) .10
    
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentRunEM
# Run the EM algorithm on the loaded data
# - mimic user actions
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentRunEM {} {

    #set ::EMSegment(StartSlice) 29
    #set ::EMSegment(EndSlice) 31

    set ::EMSegment(Cattrib,1,Prob) .25
    set ::EMSegment(Cattrib,2,Prob) .25
    set ::EMSegment(Cattrib,3,Prob) .25
    set ::EMSegment(Cattrib,4,Prob) .25
    EMSegmentSumGlobalUpdate

    EMSegmentExecute "EM" "Run" "do_not_save"
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentSamplesFromSegmentation
# Use a segmentation volume to define the samples for the EM starting point
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentSamplesFromSegmentation {class SEGid label} {

    set T2id [MIRIADSegmentGetVolumeByName "T2"]
    set PDid [MIRIADSegmentGetVolumeByName "PD"]
    set T2image [Volume($T2id,vol) GetOutput]
    set PDimage [Volume($PDid,vol) GetOutput]
    set SEGimage [Volume($SEGid,vol) GetOutput]

    set ::EMSegment(Cattrib,$class,$T2id,Sample) ""
    set ::EMSegment(Cattrib,$class,$PDid,Sample) ""

    set dims [[Volume($T2id,vol) GetOutput] GetDimensions]
    set xsize [lindex $dims 0]
    set ysize [lindex $dims 1]
    set zsize [lindex $dims 2]

    for {set z 0} {$z < $zsize} {incr z} {
        set z 30
        for {set y 0} {$y < $ysize} {incr y} {
            set y 128
            for {set x 0} {$x < $xsize} {incr x} {
                if { $label == [$SEGimage GetScalarComponentAsFloat $x $y $z 0] } {
                    set sample [$T2image GetScalarComponentAsFloat $x $y $z 0]
                    lappend ::EMSegment(Cattrib,$class,$T2id,Sample) [list $x $y $z $sample]
                    set sample [$PDimage GetScalarComponentAsFloat $x $y $z 0]
                    lappend ::EMSegment(Cattrib,$class,$PDid,Sample) [list $x $y $z $sample]
                }
            }
            return
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentClassPDFFromSegmentation
# Recalculate class Mean and Covariance from segmentations
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentClassPDFFromSegmentation {} {


    MIRIADSegmentSamplesFromSegmentation  1 [MIRIADSegmentGetVolumeByName WorkingB] 1
    MIRIADSegmentSamplesFromSegmentation  2 [MIRIADSegmentGetVolumeByName WorkingW] 2
    MIRIADSegmentSamplesFromSegmentation  3 [MIRIADSegmentGetVolumeByName WorkingC] 3
    MIRIADSegmentSamplesFromSegmentation  4 [MIRIADSegmentGetVolumeByName WorkingG] 4


    set ::EMSegment(UseSamples) 1
    EMSegmentCalculateClassMeanCovariance 
    set ::EMSegment(UseSamples) 0

    set classes "1 2 3 4" 
    set logmeans ""
    set logcovs ""
    foreach class $classes {
        set lmean ""
        set lcov ""
        for {set y 0} {$y < $::EMSegment(NumInputChannel)} {incr y} {
            lappend lmean $::EMSegment(Cattrib,$class,LogMean,$y) 
            for {set x 0} {$x < $::EMSegment(NumInputChannel)} {incr x} {
                lappend lcov $::EMSegment(Cattrib,$class,LogCovariance,$y,$x)
            }
        }
        lappend logmeans $lmean
        lappend logcovs $lcov
    }
    puts "set logmeans $logmeans"
    puts "set logcovs $logcovs"
}


#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentGetVolumeByName 
# returns the id of first match for a name
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentGetVolumeByName {name} {

    set nvols [Mrml(dataTree) GetNumberOfVolumes]
    for {set vv 0} {$vv < $nvols} {incr vv} {
        set n [Mrml(dataTree) GetNthVolume $vv]
        if { $name == [$n GetName] } {
            return [DataGetIdFromNode $n]
        }
    }
    return -1
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentGetVolumesByNamePattern
# returns a list of IDs for a given pattern
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentGetVolumesByNamePattern {pattern} {

    set ids ""
    set nvols [Mrml(dataTree) GetNumberOfVolumes]
    for {set vv 0} {$vv < $nvols} {incr vv} {
        set n [Mrml(dataTree) GetNthVolume $vv]
        if { [string match $pattern [$n GetName]] } {
            lappend ids [DataGetIdFromNode $n]
        }
    }
    return $ids
}

#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentDeleteVolumeByName 
# clean up volumes before reloading them
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentDeleteVolumeByName {name} {

    set nvols [Mrml(dataTree) GetNumberOfVolumes]
    for {set vv 0} {$vv < $nvols} {incr vv} {
        set n [Mrml(dataTree) GetNthVolume $vv]
        if { $name == [$n GetName] } {
            set id [DataGetIdFromNode $n]
            global Volume
            MainMrmlDeleteNode Volume $id
            break
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC MIRIADSegmentNormalizeImage 
# Rescale the image data to the correct atlas range
# (assumes that the max value actually occurs in the image somewhere)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRIADSegmentNormalizeImage {volid MaxValue} { 
    catch "Accu Delete"
    catch "NormImg Delete"
    vtkImageAccumulate Accu
    Accu SetInput [Volume($volid,vol) GetOutput]
    Accu Update
    set MaxImage [lindex [Accu GetMax] 0]
    puts "Normalize [Volume($volid,node) GetName]: max Volume Value = $MaxImage, max Predefined Value = $MaxValue"  
    if {$MaxImage ==  $MaxValue} {
        return
    }

    # 1. Normailze Image
    vtkImageMathematics NormImg
    NormImg SetInput1 [Volume($volid,vol) GetOutput]
    NormImg SetOperationToMultiplyByK
    set value [expr $MaxValue / double($MaxImage)]
    # 1. find out last digit
    set i 0
    while {[expr (double(int($value / pow(10.0,$i)))) > 0] } {
        incr i
    }
    # 2. Add to value 
    # - have to do it otherwise rounding error produce value MaxValue -1
    set i [expr -1*($::tcl_precision - $i)]
    while {[expr $value*$MaxImage] <=  $MaxValue} {
        set value [expr $value + pow(10.0,$i)]
    }
    NormImg SetConstantK $value
    NormImg SetInput1 [Volume($volid,vol) GetOutput]
    NormImg Update  
    set imdata [[NormImg GetOutput] NewInstance]
    $imdata DeepCopy [NormImg GetOutput]
    Volume($volid,vol) SetImageData $imdata

    NormImg Delete
    Accu Delete
}

