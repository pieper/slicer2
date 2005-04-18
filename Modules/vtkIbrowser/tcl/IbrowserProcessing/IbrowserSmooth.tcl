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
# FILE:        IbrowserSmooth.tcl
# PROCEDURES:  
#   IbrowserCancelSmoothSequence
#==========================================================================auto=

proc IbrowserBuildSmoothGUI { f master } {
    global Gui

    #--- This GUI configures a Gaussian filter kernel
    #--- to be applied to a selected sequence and
    #--- drives the creation of a new filtered sequence.
    #--- set a global variable for the frame so we can raise it.
    set ::Ibrowser(fProcessSmooth) $f
    #--- default values for full-width-half-maximum of gaussian in mm.
    set ::Ibrowser(Process,Smooth,rl_FWHM) 4.0
    set ::Ibrowser(Process,Smooth,is_FWHM) 4.0
    set ::Ibrowser(Process,Smooth,ap_FWHM) 4.0
    
    frame $f.fSpace -bg $::Gui(activeWorkspace) -bd 2 
    eval { label $f.fSpace.lSpace -text "       " } $Gui(WLA)
    pack $f.fSpace -side top 
    pack $f.fSpace.lSpace -side top -pady 4 -padx 20 -anchor w -fill x

    #--- menu button and pulldown menu that specify the interval to be processed.
    frame $f.fSelectInterval -bg $::Gui(activeWorkspace) -bd 2
    pack $f.fSelectInterval -side top -anchor w -fill x
    eval { label $f.fSelectInterval.lText -text "interval to process:" } $Gui(WLA)    
    eval { menubutton $f.fSelectInterval.mbIntervals -text "none" -width 18 -relief raised \
               -height 1 -menu $f.fSelectInterval.mbIntervals.m -bg $::Gui(activeWorkspace) \
               -indicatoron 1 } $Gui(WBA)
    eval { menu $f.fSelectInterval.mbIntervals.m } $Gui(WMA)
    foreach i $::Ibrowser(idList) {
        $f.mbVolumes.m add command -label $::Ibrowser($i,name) \
            -command "IbrowserSetActiveInterval $i"
    }

    set ::Ibrowser(Process,Smooth,mbIntervals) $f.fSelectInterval.mbIntervals
    set ::Ibrowser(Process,Smooth,mIntervals) $f.fSelectInterval.mbIntervals.m
    pack $f.fSelectInterval.lText -pady 2 -padx 2 -anchor w
    pack $f.fSelectInterval.mbIntervals -pady 2 -padx 2 -anchor w
    
    frame $f.fConfiguration -bg $Gui(activeWorkspace) -bd 2 -relief groove
    pack $f.fConfiguration -side top -anchor w -padx 2 -pady 5 -fill x
    eval { label $f.fConfiguration.lText -text "Gaussian kernel:" } $Gui(WLA)
    eval { label $f.fConfiguration.lSpace -text "        " } $Gui(WLA)
    eval { label $f.fConfiguration.lRLFWHM -text "R/L: FWHM (mm)" } $Gui(WLA)
    TooltipAdd $f.fConfiguration.lRLFWHM "FWHM is the R/L axis at which the full width of the Gaussian filter's kernel is half the central peak value. \n The standard deviation (sigma) is computed as FWHM/sqrt(8*ln(2)), \n and the point at which the filter's value is clamped to zero is set at 3*sigma."
    eval { entry $f.fConfiguration.eRLFWHM -width 8 \
               -textvariable ::Ibrowser(Process,Smooth,rl_FWHM) } $Gui(WEA)
    eval { label $f.fConfiguration.lISFWHM -text "I/S: FWHM (mm)" } $Gui(WLA)
    TooltipAdd $f.fConfiguration.lISFWHM "FWHM is the value along the S/I axis at which the full width of the Gaussian filter's kernel is half the central peak value. \n The standard deviation (sigma) is computed as FWHM/sqrt(8*ln(2)), \n and the point at which the filter's value is clamped to zero is set at 3*sigma."
    eval { entry $f.fConfiguration.eISFWHM -width 8 \
               -textvariable ::Ibrowser(Process,Smooth,is_FWHM) } $Gui(WEA)
    eval { label $f.fConfiguration.lAPFWHM -text "A/P: FWHM (mm)" } $Gui(WLA)
    TooltipAdd $f.fConfiguration.lAPFWHM "FWHM is the value along the A/P axis at which the full width of the Gaussian filter's kernel is half the central peak value. \n The standard deviation (sigma) is computed as FWHM/sqrt(8*ln(2)), \n and the point at which the filter's value is clamped to zero is set at 3*sigma."
    eval { entry $f.fConfiguration.eAPFWHM -width 8 \
               -textvariable ::Ibrowser(Process,Smooth,ap_FWHM) } $Gui(WEA)

    grid $f.fConfiguration.lText -row 0 -columnspan 3  -pady 6
    grid $f.fConfiguration.lRLFWHM -row 1 -column 1 -sticky w -padx 2 -pady 2
    grid $f.fConfiguration.eRLFWHM -row 1 -column 2 -sticky w -padx 2 -pady 2
    grid $f.fConfiguration.lAPFWHM -row 2 -column 1 -sticky w -padx 2 -pady 2
    grid $f.fConfiguration.eAPFWHM -row 2 -column 2 -sticky w -padx 2 -pady 2
    grid $f.fConfiguration.lISFWHM -row 3 -column 1 -sticky w -padx 2 -pady 2
    grid $f.fConfiguration.eISFWHM -row 3 -column 2 -sticky w -padx 2 -pady 2

    frame $f.fApply -bg $Gui(activeWorkspace) -bd 2 -relief groove 
    pack $f.fApply -side top -anchor w -padx 2 -pady 5 -fill x
    DevAddLabel $f.fApply.lSpace "         "
    DevAddButton $f.fApply.bApply "apply" "IbrowserSmoothSequence" 8
    DevAddButton $f.fApply.bCancel "cancel" "IbrowserCancelSmoothSequence " 8
    grid $f.fApply.lSpace $f.fApply.bApply $f.fApply.bCancel -sticky e -padx 2 -pady 5
    
    place $f -in $master -relwidth 1.0 -relheight 1.0 -y 0
}


#-------------------------------------------------------------------------------
# .PROC IbrowserCancelSmoothSequence
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserCancelSmoothSequence { } {

    #--- revert to default values.
    set ::Ibrowser(Process,Smooth,rl_FWHM) 4.0
    set ::Ibrowser(Process,Smooth,is_FWHM) 4.0
}



#--- Slices a volume along a selected axis
#--- and assembles new volumes from slices
#--- along interval axis.
proc IbrowserSmoothSequence { } {
    global Volume

    set rl_fwhm $::Ibrowser(Process,Smooth,rl_FWHM)
    set is_fwhm $::Ibrowser(Process,Smooth,is_FWHM)
    #--- arbitrary acceptible mm range for filter FWHM
    set max 100.0
    set min 0.1
    #--- check for valid filter values.
    if { $rl_fwhm < $min } {
        DevErrorWindow "Value for R/L FWHM must be greater than $min mm."
        return
    } elseif { $rl_fwhm > $max } {
        DevErrorWindow "Value for R/L FWHM must be less than $max mm."
        return
    } 
    if { $is_fwhm < $min } {
        DevErrorWindow "Value for I/S FWHM must be greater than $min"
        return
    } elseif { $is_fwhm > $max } {
        DevErrorWindow "Value for I/S FWHM must be less than $max mm."
        return
    } 
    #--- if no interval is selected...
    if { $::Ibrowser(activeInterval) == $::Ibrowser(none,intervalID) } {
        DevErrorWindow "Please select an interval to smooth."
        return        
    }
    
    IbrowserRaiseProgressBar

    #--- get the destination interval started
    set dstID $::Ibrowser(uniqueNum)
    set ::Ibrowser(loadVol,name) [format "imageData-%d" $dstID]
    set dstName $::Ibrowser(loadVol,name)
    set ::Ibrowser($dstID,name) $dstName
    set ::Ibrowser($dstName,intervalID) $dstID

    #--- get info about the source interval
    set srcID $::Ibrowser(activeInterval)
    set targetName $::Ibrowser($srcID,name)
    set firstVolID $::Ibrowser($srcID,firstMRMLid)
    set lastVolID $::Ibrowser($srcID,lastMRMLid)
    set srcName $::Ibrowser($srcID,name)
    
    #--- how many volumes will we need?
    set numVols $::Ibrowser($srcID,numDrops)

    #--- create $numVols new Volume nodes and volumes.
    for {set i 0} { $i < $numVols } { incr i } {
        set node [MainMrmlAddNode Volume ]
        set nodeID [$node GetID ]
        MainVolumesCreate $nodeID
        lappend newnodeList  $node 
    }

    #--- create 2D gaussian filter
    vtkImageGaussianSmooth gaussian
    gaussian SetDimensionality 2

    #--- for each new data volume
    for { set n 0 } { $n < $numVols } { incr n } {
        set dstnode [ lindex $newnodeList $n ]
        set dstnodeID [ $dstnode GetID ]

        #--- set the source volume
        set vid $firstVolID
        gaussian SetInput  [ ::Volume($vid,vol) GetOutput ]
        set imdata [ gaussian GetOutput ]
        $imdata Update

        set dim [ $imdata GetDimensions ]
        set dimx [ lindex $dim 0 ]
        set dimy [ lindex $dim 1 ]
        set dimz [ lindex $dim 2 ]
        set spc [ $imdata GetSpacing ]
        set pixwid [ lindex $spc 0 ]
        set pixhit [ lindex $spc 1 ]
        set sliceThickness [ lindex $spc 2 ]
        set sliceSpacing 0
        set zSpacing [ expr $sliceThickness + $sliceSpacing ]
        set ext [ $imdata GetWholeExtent ]
        set xstart [expr 1 + [lindex $ext 0]]
        set xstop [expr 1 + [lindex $ext 1]]            
        set ystart [expr 1 + [lindex $ext 2]]
        set ystop [expr 1 + [lindex $ext 3]]            
        set zstart [expr 1 + [lindex $ext 4]]
        set zstop [expr 1 + [lindex $ext 5]] 
        #puts "configuring MrmlVolumeNode $n: "
        #puts "....image dimensions: $dimx $dimy $dimz"
        #puts "....pixwid = $pixwid; pixhit = $pixhit; pixdepth = $zSpacing"
        #puts "....image extent = $xstart $xstop; $ystart $ystop; $zstart $zstop"

        #--- configure filter:
        #--- sigma = FWHM / sqrt (8*ln(2))
        #--- radius = 3*sigma (effective cutoff point for filter)
        #--- should radius be set to some higher num of std.deviations?
        #--- must configure filter in pixel units, so also convert from mm...
        set pix2mmRL [ expr 1.0 / $pixwid ]
        set pix2mmIS [ expr 1.0 / $pixhit ]
        set rl_sigma [ expr $rl_fwhm * $pix2mmRL / 2.355  ]
        set is_sigma [ expr $is_fwhm * $pix2mmIS / 2.355 ]
        set rl_radius [ expr $rl_sigma * $pix2mmRL * 3.0 ] 
        set is_radius [ expr $is_sigma * $pix2mmIS * 3.0  ]
        puts "$rl_sigma $is_sigma $rl_radius $is_radius"
        gaussian SetStandardDeviations $rl_sigma $is_sigma
        gaussian SetRadiusFactors $rl_radius $is_radius
    
        #--- configure node
        $dstnode SetName ${dstName}_${n}
        $dstnode SetLabelMap [ ::Volume($firstVolID,node) GetLabelMap ]
        eval $dstnode SetSpacing $pixwid $pixhit $zSpacing
        $dstnode SetTilt 0
        $dstnode SetNumScalars [ $imdata GetNumberOfScalarComponents ]
        $dstnode SetScanOrder [ ::Volume($firstVolID,node) GetScanOrder ]
        $dstnode SetLittleEndian 1
        $dstnode SetImageRange 1 [ expr $dimz  ]
        $dstnode SetDimensions $dimx $dimy
        $dstnode ComputeRasToIjkFromScanOrder [ ::Volume($firstVolID,node) GetScanOrder ]
        $dstnode SetLUTName [ ::Volume($firstVolID,node) GetLUTName ]
        $dstnode SetAutoWindowLevel [ ::Volume($firstVolID,node) GetAutoWindowLevel ]
        $dstnode SetWindow [ ::Volume($firstVolID,node) GetWindow ]
        $dstnode SetLevel [ ::Volume($firstVolID,node) GetLevel ]
        $dstnode SetApplyThreshold [ ::Volume($firstVolID,node) GetApplyThreshold ]
        $dstnode SetAutoThreshold [ ::Volume($firstVolID,node) GetAutoThreshold ]
        $dstnode SetUpperThreshold [ ::Volume($firstVolID,node) GetUpperThreshold ]
        $dstnode SetLowerThreshold [ ::Volume($firstVolID,node) GetLowerThreshold ]
        $dstnode SetFrequencyPhaseSwap [ ::Volume($firstVolID,node) GetFrequencyPhaseSwap ]
        $dstnode SetInterpolate [ ::Volume($firstVolID,node) GetInterpolate ]
        ::Volume($dstnodeID,vol) SetImageData  $imdata

        set ::Ibrowser($dstID,$n,MRMLid) $dstnodeID
    }

    gaussian Delete
    #--- set first and last MRML ids in the interval
    #--- and create a new interval to hold the volumes
    set ::Ibrowser($dstID,firstMRMLid) [ [ lindex $newnodeList 0 ] GetID ]
    set ::Ibrowser($dstID,lastMRMLid) [ [ lindex $newnodeList end ] GetID ]
    set spanmax [ expr $numVols - 1 ]
    IbrowserMakeNewInterval $dstName $::IbrowserController(Info,Ival,imageIvalType) \
    0.0 $spanmax $numVols
    
    IbrowserEndProgressFeedback
    set tt "New interval $dstName contains gaussian smoothed version of $srcName (R/L: sigma=$rl_sigma radius=$rl_sigma) (IS: sigma=$is_sigma radius=$is_sigma)."
    IbrowserSayThis $tt 0
    IbrowserLowerProgressBar    
    set ::Ibrowser(Process,reassembleChoice) ""
}


