#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
# FILE:        EMSegmentAlgorithm.tcl
# PROCEDURES:  
#   EMSegmentSetVtkSuperClassSetting
#   EMSegmentAlgorithmStart
#==========================================================================auto=
#=auto==========================================================================
# (c) Copyright 2002 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
proc CutOutRegion {ThreshInstance MathInstance ResultVolume ProbVolume CutOffProb volDataType flag} {
   # 1. Define cut out area 
    $ThreshInstance SetInput [$ProbVolume GetOutput] 
    if {$flag} {$ThreshInstance ThresholdByUpper $CutOffProb
    } else {$ThreshInstance ThresholdBetween $CutOffProb $CutOffProb}
    $ThreshInstance SetInValue 1.0
    $ThreshInstance SetOutValue 0.0
    $ThreshInstance SetOutputScalarTypeTo$volDataType 
    $ThreshInstance Update
    # 2. Cut out region from normal image
    $MathInstance SetOperationToMultiply
    $MathInstance SetInput 1 [$ResultVolume GetOutput]
    $MathInstance SetInput 0 [$ThreshInstance GetOutput]
    $MathInstance Update
}

# Extracts the Gauss curve from the given histogram. The histogram is defined by the probability map (ROI) and Grey value image 
# [llength $MRIVolumeList] = 1 => results in a 1D Histogram;  [llength $MRIVolumeList] = 2 => results in a 2D Histogram  
# result will be returned in MathImage(Gauss,Mean,x), MathImage(Gauss,Covariance,y,x),  MathImage(Gauss,Sum),   MathImage(Gauss,LogGaussFlag)
# MathImage(Gauss,CutOffAbsolut), MathImage(Gauss,CutOffPercent), MathImage(Gauss,MaxProb),  MathImage(Gauss,GreyMin,x), and MathImage(Gauss,GreyMax,x)
proc GaussCurveCalculation {CutOffProbability LogGaussFlag MRIVolumeList ProbVolume} {
    # Kilian Debug Code : something is not girhty - take the same volume for the first two images 
    # also add to code so you can cut of the by hhest grey value so you  measure skin  
    # 1. Figure out how many voxels are in $MathImage(mathValue,1) %
    global MathImage

   # Initialize values 
    set NumInputChannel [llength $MRIVolumeList] 
    for {set y 1} {$y <= $NumInputChannel} {incr y} {
    if {$LogGaussFlag} {set MathImage(Gauss,Mean,$y) 0.0 
        } else {set MathImage(Gauss,Mean,$y) 0}
        set MathImage(Gauss,GreyMin,$y) -1
        set MathImage(Gauss,GreyMax,$y) -1
    for {set x 1} {$x <= $NumInputChannel} {incr x} {
          set MathImage(Gauss,Covariance,$y,$x) 0.0
    }
    }
    set MathImage(Gauss,Sum)  0
    set MathImage(Gauss,LogGaussFlag) $LogGaussFlag
    set MathImage(Gauss,CutOffAbsolut) 0 
    set MathImage(Gauss,CutOffPercent) 0.0
    set MathImage(Gauss,MaxProb) 0


    vtkImageAccumulate MathImg
    MathImg SetInput [$ProbVolume GetOutput]
    MathImg Update
    set Min [lindex [MathImg GetMin] 0]
    set MathImage(Gauss,MaxProb) [lindex [MathImg GetMax] 0] 
    if {$Min == 0} {incr Min}
    if {($Min <0) && $MathImage(Gauss,LogGaussFlag)} {
    puts "Probability Volume $ProbVolume has negative values (ValueRange $Min $MathImage(Gauss,MaxProb)), which is not possible for log gaussian ! Probably little endian set incorrectly"
    MathImg Delete
    exit 1
    }
    set index  [expr $MathImage(Gauss,MaxProb) - $Min]
    MathImg SetComponentExtent 0 $index 0 0 0 0
    MathImg SetComponentOrigin $Min 0.0 0.0 
    MathImg Update 
    
    set data   [MathImg GetOutput]
    set ROIVoxel 0   
    for {set i 0} {$i <= $index} {incr i} { incr ROIVoxel [expr int([$data GetScalarComponentAsFloat $i 0 0 0])] }
    if  {$ROIVoxel == 0} {
        MathImg   Delete
    return
    }
    set CutOffVoxel [expr $ROIVoxel*(1.0 - $CutOffProbability)]
    # Add instructions so if border is to high you can set a flag so that the the highest probability will be stilll sampled  
    for {set i  $index} {$i > -1} {incr i -1} {
    incr MathImage(Gauss,Sum) [expr int ([$data GetScalarComponentAsFloat $i 0 0 0])]
        # puts "$i [expr int ([$data GetScalarComponentAsFloat $i 0 0 0])]"
    if {$MathImage(Gauss,Sum) > $CutOffVoxel} { 
        if {$i == $index } {
        puts "Warning: CutOffProbabiliyt ($CutOffProbability) is set to low ! No samples could be taken => Redefine it !"
        set CutOffVoxel $MathImage(Gauss,Sum)
        } else {
        set MathImage(Gauss,CutOffAbsolut) [expr $i+$Min +1] 
        set MathImage(Gauss,Sum) [expr $MathImage(Gauss,Sum) - int([$data GetScalarComponentAsFloat $i 0 0 0])]
        break
        }
    }
    }
    # If it went through all of it you have to set it to $min !
    if  {$MathImage(Gauss,CutOffAbsolut) == 0 } {set MathImage(Gauss,CutOffAbsolut) $Min}
    set MathImage(Gauss,CutOffPercent) [expr 100 - int(double($MathImage(Gauss,Sum))/double($ROIVoxel)*1000)/10.0]
    if {$MathImage(Gauss,Sum) == 0} { 
    MathImg   Delete
    return
    }

    vtkImageThreshold threshold
    vtkImageMathematics MathMulti
    # Calculate the mean for each image
    for {set i 1} {$i <= $NumInputChannel} {incr i} {
    CutOutRegion threshold MathMulti [lindex $MRIVolumeList [expr $i-1]] $ProbVolume $MathImage(Gauss,CutOffAbsolut) $MathImage(volDataType) 1
    
    # puts "Multiplying"
    # Now value To it so we can differnetiate between real 0 and not
    vtkImageMathematics MathAdd($i)
    MathAdd($i) SetOperationToAdd
    MathAdd($i) SetInput 1 [MathMulti GetOutput]
    MathAdd($i) SetInput 0 [threshold GetOutput]
    MathAdd($i) Update
    # if {$i ==-1} {
    #    set data [MathAdd($i) GetOutput]
    #    for {set x 0} {$x < 256} {incr x} {
    #    for {set y 0} {$y < 256} {incr y} {
    #        if {([$data GetScalarComponentAsFloat $x $y 0 0] > 103) && ([$data GetScalarComponentAsFloat $x $y 0 0] < 107)} {
    #        puts "Jey Hey $x $y [$data GetScalarComponentAsFloat $x $y 0 0]"
    #        }
    #    }
    #    }
    # }
    # 3. Generate Histogram in 1D
    MathImg SetInput [MathAdd($i) GetOutput]   
    MathImg Update            
    set min($i)    [lindex [MathImg GetMin] 0]
    set max($i)    [lindex [MathImg GetMax] 0] 
    if {$min($i) == 0} {incr min($i)
        } else { 
        if { $min($i) < 0} {
                # Please note: only the input volume is corrupt, because the probility volume was checked before !
                # (MathMulti is the cut out form from the input volume i) 
        puts "Error: INPUT VOLUME $i has values below 0! Probably the LittleEndian is set wrong!" 
                set ErrorVolume  [lindex $MRIVolumeList [expr $i-1]]
        puts "       Run ./mathImage -fc his -pr [$ErrorVolume GetFilePrefix] -le [expr [$ErrorVolume GetDataByteOrder] ? yes : no]"
        exit 1
        } 
    } 
    set Index($i)  [expr $max($i) - $min($i)]
    
    MathImg SetComponentExtent 0 $Index($i) 0 0 0 0
    MathImg SetComponentOrigin $min($i) 0.0 0.0 
    MathImg Update
    # Calculate the mean for every image 
    set data   [MathImg GetOutput]
    set MinBorder($i) $max($i)
    set MaxBorder($i) $min($i)
    set Xindex  $Index($i)
    # If you get an error message here 
    for {set x $max($i)} {$x >= $min($i)} {incr x -1} {
        set temp [$data GetScalarComponentAsFloat $Xindex 0 0 0]
        # if {($x > 103) && ($x < 107)} {
        #        puts "Du $x [$data GetScalarComponentAsFloat $Xindex 0 0 0]"
        #        }
        incr Xindex -1
        if {$temp} {
        set MinBorder($i) $x
        if {$x > $MaxBorder($i)} {set MaxBorder($i) $x }
        if {$MathImage(Gauss,LogGaussFlag)} {
            set MathImage(Gauss,Mean,$i) [expr $MathImage(Gauss,Mean,$i) + log($x) * double($temp)]
        } else {
            # puts "$MathImage(Gauss,Mean,$i) expr ($x+1) * int($temp)"
            incr MathImage(Gauss,Mean,$i) [expr ($x+1) * int($temp)]}
        }
    }
    set MathImage(Gauss,Mean,$i) [expr double($MathImage(Gauss,Mean,$i))/ double($MathImage(Gauss,Sum))]
    # Calculate Variance for the two images
    set Xindex [expr $MinBorder($i) - $min($i)]
        set MathImage(Gauss,GreyMin,$i) $MinBorder($i)
        set MathImage(Gauss,GreyMax,$i) $MaxBorder($i)

    for {set x $MinBorder($i)} {$x <= $MaxBorder($i)} {incr x} {
        if {$MathImage(Gauss,LogGaussFlag)} { set  temp [expr log($x) - $MathImage(Gauss,Mean,$i)]
        } else {set  temp [expr $x - 1 - $MathImage(Gauss,Mean,$i)]}
        set MathImage(Gauss,Covariance,$i,$i) [expr $MathImage(Gauss,Covariance,$i,$i) + $temp*$temp * [$data GetScalarComponentAsFloat $Xindex 0 0 0]]
        incr Xindex
    }
    set min($i) $MinBorder($i)
    if {$MinBorder($i) <  $MaxBorder($i)} {
        set max($i)  $MaxBorder($i)
        set Index($i)  [expr $max($i) - $min($i)]
    }
    }
    if {($MathImage(Gauss,Sum) > 1) && ($NumInputChannel > 1)} {
    vtkImageAppendComponents imageAppend
    imageAppend AddInput [MathAdd(1) GetOutput]
    imageAppend AddInput [MathAdd(2) GetOutput]
    imageAppend Update
    MathImg SetInput [imageAppend GetOutput]
    MathImg SetComponentExtent 0 $Index(1) 0 $Index(2) 0 0
    MathImg SetComponentOrigin $min(1) $min(2) 0.0 
    MathImg Update
    set data   [MathImg GetOutput]
    
    # You can cut of images to make them the same length
    # vtkImageClip clip
    # clip SetInput [imageAppend GetOutput]
    # clip SetOutputWholeExtent 0 255 0 255 20 22
    # Now figure out the variance 
    # 4. Calculate Covariance 
    
    # Covariance = (Sum(Sample(x,i) - mean(x))*(Sample(y,i) - mean(y)))/(n-1)

    set Yindex [expr $MinBorder(1) - $min(1)]
    for {set y $min(1)} {$y <= $max(1)} {incr y} {
        if {$MathImage(Gauss,LogGaussFlag)} { set Ytemp [expr log($y) - $MathImage(Gauss,Mean,1)]
        } else {set Ytemp [expr $y - 1 - $MathImage(Gauss,Mean,1)]}
        set Xindex [expr $min(2) - $min(2)]
        for {set x $min(2)} {$x <= $max(2)} {incr x} {
        if {$MathImage(Gauss,LogGaussFlag)} { set  Xtemp [expr log($x) - $MathImage(Gauss,Mean,2)]
            } else {set  Xtemp [expr $x - 1 - $MathImage(Gauss,Mean,2)]}
        set MathImage(Gauss,Covariance,1,2) [expr $MathImage(Gauss,Covariance,1,2) + $Xtemp*$Ytemp * [$data GetScalarComponentAsFloat $Yindex $Xindex 0 0]]
        # if {[$data GetScalarComponentAsFloat $Yindex $Xindex 0 0]} {
            # puts " $Yindex $Xindex [$data GetScalarComponentAsFloat $Yindex $Xindex 0 0]"
            # }
            incr Xindex
        }
        incr Yindex
        }
        set MathImage(Gauss,Covariance,2,1) $MathImage(Gauss,Covariance,1,2)
        for {set y 1} {$y < 3} {incr y} {
        for {set x 1} {$x < 3} {incr x} {
            set MathImage(Gauss,Covariance,$y,$x) [expr $MathImage(Gauss,Covariance,$y,$x) / double($MathImage(Gauss,Sum) - 1)]
        }
        }
    } else { 
        if {$NumInputChannel > 1} {
                 set MathImage(Gauss,Covariance,1,2) 1.0
         set MathImage(Gauss,Covariance,2,1) $MathImage(Gauss,Covariance,1,2)
        } else {
        if { $MathImage(Gauss,Sum) > 1 } { set MathImage(Gauss,Covariance,1,1)  [expr $MathImage(Gauss,Covariance,1,1)/ double($MathImage(Gauss,Sum) - 1)] } 
        }
    }
    # Clean Up
    if {$NumInputChannel > 1} {
        imageAppend Delete
        MathAdd(2) Delete
    }
    MathAdd(1) Delete
    threshold Delete
    MathMulti Delete
    MathImg Delete
}
