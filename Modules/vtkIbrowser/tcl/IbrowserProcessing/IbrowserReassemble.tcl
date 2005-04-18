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
# FILE:        IbrowserReassemble.tcl
# PROCEDURES:  
#   IbrowserCancelReassembleSequence
#   IbrowserValidReassembleAxis
#==========================================================================auto=

proc IbrowserBuildReassembleGUI { f master } {
global Gui

    #--- This GUI allows a new set of volumes to be created from
    #--- slices from volumes in the interval.
    #--- Set a global variable for frame so we can raise it.
    set ::Ibrowser(fProcessReassemble) $f
    
    frame $f.fSpace -bg $::Gui(activeWorkspace) -bd 2 
    eval { label $f.fSpace.lSpace -text "       " } $Gui(WLA)
    pack $f.fSpace -side top 
    pack $f.fSpace.lSpace -side top -pady 4 -padx 20 -anchor w -fill x

    #--- frame to specify the interval to be processed.
    frame $f.fSelectInterval -bg $::Gui(activeWorkspace) -bd 2
    pack $f.fSelectInterval -side top -anchor w -fill x
    eval { label $f.fSelectInterval.lText -text "interval to process:" } $Gui(WLA)    
    eval { menubutton $f.fSelectInterval.mbIntervals -text "none" -width 18 -relief raised \
               -height 1 -menu $f.fSelectInterval.mbIntervals.m -bg $::Gui(activeWorkspace) \
               -indicatoron 1 } $Gui(WBA)
    eval { menu $f.fSelectInterval.mbIntervals.m } $Gui(WMA)
    foreach i $::Ibrowser(idList) {
        puts "adding $::Ibrowser($i,name)"
        $f.mbVolumes.m add command -label $::Ibrowser($i,name) \
            -command "IbrowserSetActiveInterval $i"
    }

    set ::Ibrowser(Process,Reassemble,mbIntervals) $f.fSelectInterval.mbIntervals
    set ::Ibrowser(Process,Reassemble,mIntervals) $f.fSelectInterval.mbIntervals.m
    pack $f.fSelectInterval.lText -pady 2 -padx 2 -anchor w
    pack $f.fSelectInterval.mbIntervals -pady 2 -padx 2 -anchor w

    frame $f.fConfiguration -bg $Gui(activeWorkspace) -bd 2 -relief groove
    pack $f.fConfiguration -side top -anchor w -padx 2 -pady 5 -fill x
    DevAddLabel $f.fConfiguration.lText "Select axis to vary along interval:"
    eval { radiobutton $f.fConfiguration.r1 -width 20 -text {R/L} \
               -variable ::Ibrowser(Process,reassembleAxis) -value "RL" \
               -relief flat -offrelief flat -overrelief raised \
               -command "" -selectcolor white } $Gui(WEA)
    eval { radiobutton $f.fConfiguration.r2 -width 20 -text {A/P} \
               -variable ::Ibrowser(Process,reassembleAxis) -value "AP" \
               -relief flat -offrelief flat -overrelief raised \
               -command "" -selectcolor white } $Gui(WEA)
    eval { radiobutton $f.fConfiguration.r3 -width 20 -text {S/I} \
               -variable ::Ibrowser(Process,reassembleAxis) -value "SI" \
               -relief flat -offrelief flat -overrelief raised \
               -command "" -selectcolor white } $Gui(WEA)
    
    pack $f.fConfiguration.lText -side top -pady 4 -padx 20 -anchor w
    pack $f.fConfiguration.r1 $f.fConfiguration.r2 $f.fConfiguration.r3 -side top -pady 5 -padx 20 -anchor w

    DevAddButton $f.fConfiguration.bApply "apply" "IbrowserReassembleSequence" 10
    TooltipAdd $f.fConfiguration.bApply "Reassembles volumes and populates a new interval with them."
    DevAddButton $f.fConfiguration.bCancel "cancel" "IbrowserCancelReassembleSequence" 10

    pack $f.fConfiguration.bApply $f.fConfiguration.bCancel -side top -anchor w -padx 20 -pady 5
    place $f -in $master -relwidth 1.0 -relheight 1.0 -y 0
    
}


#-------------------------------------------------------------------------------
# .PROC IbrowserCancelReassembleSequence
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserCancelReassembleSequence { } {
    set ::Ibrowser(Process,reassembleAxis) ""
}



#-------------------------------------------------------------------------------
# .PROC IbrowserValidReassembleAxis
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserValidReassembleAxis { } {
    
    set c $::Ibrowser(Process,reassembleAxis)
    if { $c == "RL" || $c == "AP" || $c == "SI" } {
        return 1
    } else {
        return 0
    }
}


#--- Slices a volume along a selected axis
#--- and assembles new volumes from slices
#--- along interval axis.
proc IbrowserReassembleSequence { } {
global Volume

    if { [ IbrowserValidReassembleAxis ] } {
        IbrowserRaiseProgressBar
        puts "The axis to vary along interval is: $::Ibrowser(Process,reassembleAxis)"

        #--- get the destination interval started
        set dstID $::Ibrowser(uniqueNum)
        set ::Ibrowser(loadVol,name) [format "multiVol%d" $dstID]
        set dstName $::Ibrowser(loadVol,name)
        set ::Ibrowser($dstID,name) $dstName
        set ::Ibrowser($dstName,intervalID) $dstID
        
        #--- get info about the source interval
        set srcID $::Ibrowser(activeInterval)
        set firstVolID $::Ibrowser($srcID,firstMRMLid)
        set lastVolID $::Ibrowser($srcID,lastMRMLid)
        set srcName $::Ibrowser($srcID,name)

        #--- what are the dimensions of the first image volume
        set DimList [ [ ::Volume($firstVolID,vol) GetOutput ] GetDimensions ]
        set rangeList [ ::Volume($firstVolID,node) GetImageRange ]
        set xmin 0
        set ymin 0
        set zmin 0
        set xmax [expr [ lindex $DimList 0 ] - 1 ]
        set ymax [ expr [ lindex $DimList 1 ] - 1 ]
        set zmax [ expr [ lindex $DimList 2 ] - 1 ]
        puts "input volumes are [expr $xmax+1] x [expr $ymax+1] x [expr $zmax+1]"

        
        #--- Error check: see if all volumes in the src interval
        #--- have the same dimension; otherwise this will fail.
        #--- Assuming dimensions are returned in vtk space.
        for { set vid $firstVolID } { $vid <= $lastVolID} { incr vid } {
            set VDimList [ [::Volume($vid,vol) GetOutput ] GetDimensions ]
            set Vi [ expr [ lindex $VDimList 0 ] - 1 ]
            set Vj [ expr [ lindex $VDimList 1 ] - 1 ]
            set Vk [ expr [lindex $VDimList 2 ] - 1 ]
            if { $Vi != $xmax ||  $Vj != $ymax || $Vk != $zmax } {
                DevErrorWindow "All volumes in selected interval must have the same dimension."
                return
            }
        }
        puts "all volumes have the same dimension."
        
        #--- create $numVols new Volume nodes and volumes.
        #--- get the corresponding axis in Vtk space for this vol.
        #--- ok, assuming all volumes have the same dimension...
        set vid $firstVolID
        set newvec [ IbrowserGetRasToVtkAxis \
                         $::Ibrowser(Process,reassembleAxis) \
                         ::Volume($vid,node) ]
        #--- unpack the vector into x, y and z
        foreach { x y z } $newvec { }

        if { ($x == 1) || ($x == -1) } {
            #---vary axis 0 along interval
            set appendAxis 0
            set numVols [ expr $xmax + 1 ]
        } elseif { ($y == 1) || ( $y == -1) } {
            #---vary axis 1 along interval
            set appendAxis 1
            set numVols [ expr $ymax + 1 ]
        } elseif { ($z == 1) || ($z == -1) } {
            #---vary axis 2 along interval
            set appendAxis 2
            set numVols [ expr $zmax + 1 ]
        }
        puts "VTK vector is: $newvec; appendAxis is: $appendAxis"
        puts "creating $numVols new nodes..."
        for {set i 0} { $i < $numVols } { incr i } {
            set node [MainMrmlAddNode Volume ]
            set nodeID [$node GetID ]
            MainVolumesCreate $nodeID
            lappend newnodeList  $node 
        }
        puts "...created $numVols new nodes"

        #--- Here's what we're going to do:
        #--- for each [of n = 0 to (numVols-1)] new data volume,
        #--- extract  nth slices from all m src data vols
        #--- append them to create nth new data volume.
        vtkExtractVOI sliceExtractor
        sliceExtractor SetSampleRate 1 1 1
        
        #--- for each new data volume
        for { set n 0 } { $n < $numVols } { incr n } {
            set dstnode [ lindex $newnodeList $n ]
            set dstnodeID [ $dstnode GetID ]

            #--- create the image appender
            vtkImageAppend sliceAppender
            sliceAppender SetAppendAxis $appendAxis
            
            #--- set the source volume
            set vid $firstVolID
            sliceExtractor SetInput  [ ::Volume($vid,vol) GetOutput ]
            #--- consider each volume in the sequence...
            for { set m 0 } { $m < $::Ibrowser($srcID,numDrops) } { incr m } {
                vtkImageData vol
                #--- extract the nth slice
                if { $appendAxis == 0 } {
                    sliceExtractor SetVOI $n $n $ymin $ymax $zmin $zmax
                    puts "setting VOI to be $n $n $ymin $ymax $zmin $zmax"
                    sliceExtractor Update
                    set d [ sliceExtractor GetOutput ]
                    vol DeepCopy $d
                    vol SetExtent $m $m $ymin $ymax $zmin $zmax                    
                } elseif { $appendAxis == 1 } {
                    sliceExtractor SetVOI $xmin $xmax $n $n $zmin $zmax
                    puts "setting VOI to be $xmin $xmax $n $n $zmin $zmax"
                    sliceExtractor Update
                    set d [ sliceExtractor GetOutput ]
                    vol DeepCopy $d
                    vol SetExtent $xmin $xmax $m $m $zmin $zmax                    
                } elseif { $appendAxis == 2 } {
                    sliceExtractor SetVOI $xmin $xmax $ymin $ymax $n $n
                    puts "setting VOI to be $xmin $xmax $ymin $ymax $n $n"
                    sliceExtractor Update
                    set d [ sliceExtractor GetOutput ]
                    vol DeepCopy $d
                    vol SetExtent $xmin $xmax $ymin $ymax $m $m                   
                } else {
                    DevErrorWindow "Invalid axis for reassembling volumes."
                }
                sliceAppender AddInput vol
                vol Delete
                incr vid
            }

            #--- grab the new image data
            set imdata [ sliceAppender GetOutput ]
            $imdata Update

            #--- get info to configure the MRML node.
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
            #puts "....pixwid = $pixwid; pixhit = $pixhit; pixdepth = $zSpacing"
            #puts "....image extent = $xstart $xstop; $ystart $ystop; $zstart $zstop"
            #puts "....image dimensions: $dimx $dimy $dimz"

            
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

            sliceAppender Delete
        }

        sliceExtractor Delete
        #--- set first and last MRML ids in the interval
        #--- and create a new interval to hold the volumes
        set ::Ibrowser($dstID,firstMRMLid) [ [ lindex $newnodeList 0 ] GetID ]
        set ::Ibrowser($dstID,lastMRMLid) [ [ lindex $newnodeList end ] GetID ]
        set spanmax [ expr $numVols - 1 ]
        IbrowserMakeNewInterval $dstName $::IbrowserController(Info,Ival,imageIvalType) \
            0.0 $spanmax $numVols

        #--- text the user
        set tt "Created new interval $dstName from volumes."
        IbrowserSayThis $tt 0

        IbrowserEndProgressFeedback
        set tt "New interval $dstName contains reassembled volumes of $srcName."
        IbrowserSayThis $tt 0
        IbrowserLowerProgressBar    
        set ::Ibrowser(Process,reassembleAxis) ""
    }
}


