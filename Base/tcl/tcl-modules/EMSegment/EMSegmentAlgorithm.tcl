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

#-------------------------------------------------------------------------------
# .PROC  EMSegmentSetVtkSuperClassSetting
# Setting up everything for the super classes  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetVtkSuperClassSetting {SuperClass NumInputImagesSet } {
  global EMSegment Volume
  # Reads in the value for each class individually
  EMSegment(vtkEMSegment) SetNumClasses [llength $EMSegment(Cattrib,$SuperClass,ClassList)]
  set x 0

  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
      set y 0
      foreach j $EMSegment(Cattrib,$SuperClass,ClassList) {
      for {set k 0} { $k < 6} {incr k} {
         EMSegment(vtkEMSegment) SetMarkovMatrix $EMSegment(Cattrib,$SuperClass,CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) $k $y $x
      }
      incr y
      }
      incr x
  }
  
  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
    if {$EMSegment(Cattrib,$i,IsSuperClass)} {
      EMSegment(vtkEMSegment) CreateNextSuperClass 
    } else {
      EMSegment(vtkEMSegment) CreateNextClass
      EMSegment(vtkEMSegment) SetLabel             $EMSegment(Cattrib,$i,Label) 
      EMSegment(vtkEMSegment) SetShapeParameter    $EMSegment(Cattrib,$i,ShapeParameter)
      if {$EMSegment(Cattrib,$i,ProbabilityData) != $Volume(idNone)} {
          EMSegment(vtkEMSegment) SetProbDataLocal 1 
          EMSegment(vtkEMSegment) SetInputIndex $NumInputImagesSet [Volume($EMSegment(Cattrib,$i,ProbabilityData),vol) GetOutput]
          incr NumInputImagesSet
      } else {
          EMSegment(vtkEMSegment) SetProbDataLocal 0 
      }
      for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
          if {$EMSegment(SegmentMode)} {
             EMSegment(vtkEMSegment) SetLogMu $EMSegment(Cattrib,$i,LogMean,$y) $y
             for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {
               EMSegment(vtkEMSegment) SetLogCovariance $EMSegment(Cattrib,$i,LogCovariance,$y,$x) $y $x
             }
          } else {
             EMSegment(vtkEMSegment) SetMu $EMSegment(Cattrib,$i,LogMean,[expr $y+1]) $i
             EMSegment(vtkEMSegment) SetSigma $EMSegment(Cattrib,$i,LogCovariance,$y,$y) $i 
          }
      }
      if {$EMSegment(IntensityAvgClass) == $EMSegment(Cattrib,$i,Label)} {
          # Transfere Intensity correction filter stuff
          set index 0
          EMSegment(vtkEMSegment) SetIntensityAvgClass
          foreach v $EMSegment(SelVolList,VolumeList) {       
             EMSegment(vtkEMSegment) SetIntensityAvgValuePreDef $EMSegment(IntensityAvgValue,$v) $index
             incr index
          } 
      }
    }
    # puts "blubber $i $EMSegment(Cattrib,$i,Prob)"
    EMSegment(vtkEMSegment) SetTissueProbability $EMSegment(Cattrib,$i,Prob)
    # puts "$i is superclass $EMSegment(Cattrib,$i,IsSuperClass)"
    if {$EMSegment(Cattrib,$i,IsSuperClass)} {
      set NumInputImagesSet [EMSegmentSetVtkSuperClassSetting $i $NumInputImagesSet]
      # just to go back to super class => go up a level 
      EMSegment(vtkEMSegment) MoveToNextClass
    }

   }
   return $NumInputImagesSet
}

proc EMSegmentSetVtkClassSettingOld {} {
  global EMSegment Volume

  EMSegment(vtkEMSegment) SetNumClasses [llength $EMSegment(Cattrib,0,ClassList)]
  set NumInputImagesSet 0
  # Set Class info
  set xindex 1
  foreach i $EMSegment(Cattrib,0,ClassList) { 
    EMSegment(vtkEMSegment) SetLabel   $EMSegment(Cattrib,$i,Label) $xindex
 
    # Probability paramters
    EMSegment(vtkEMSegment) SetTissueProbability $EMSegment(Cattrib,$i,Prob) $xindex
    if {$EMSegment(Cattrib,$i,ProbabilityData) != $Volume(idNone)} {
       EMSegment(vtkEMSegment) SetProbDataLocal 1 $xindex
       EMSegment(vtkEMSegment) SetInputIndex $NumInputImagesSet [Volume($EMSegment(Cattrib,$i,ProbabilityData),vol) GetOutput]
       incr NumInputImagesSet
    } else {
       EMSegment(vtkEMSegment) SetProbDataLocal 0 $xindex 
    }

    # Intensity Paramters
    set yindex 1
    for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
    EMSegment(vtkEMSegment) SetLogMu $EMSegment(Cattrib,$i,LogMean,$y) $xindex $yindex
    for {set z 0} {$z < $EMSegment(NumInputChannel)} {incr z} {
        EMSegment(vtkEMSegment) SetLogCovariance $EMSegment(Cattrib,$i,LogCovariance,$y,$z) $xindex $yindex [expr $z +1]
    }
        incr yindex
    }

    # MRF Parameters
    set yindex 1
    foreach j $EMSegment(Cattrib,0,ClassList) { 
       for {set k 0} { $k< 6} {incr k} {
          EMSegment(vtkEMSegment) SetMarkovMatrix $EMSegment(Cattrib,0,CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) [expr $k+1] $yindex $xindex
       }
       incr yindex
    }
    incr xindex
  }
  # Transfer image information
  foreach v $EMSegment(SelVolList,VolumeList) {       
      EMSegment(vtkEMSegment) SetInputIndex $NumInputImagesSet [Volume($v,vol) GetOutput]
      incr NumInputImagesSet
  }
  # Transfere Intensity correction filter stuff
  set index 1
  EMSegment(vtkEMSegment) SetIntensityAvgClass $EMSegment(IntensityAvgClass)
  foreach v $EMSegment(SelVolList,VolumeList) {       
     EMSegment(vtkEMSegment) SetIntensityAvgValuePreDef $EMSegment(IntensityAvgValue,$v) $index
     incr index
  } 
  # Transfer Bias Print out Information
  if {$EMSegment(BiasPrint)} {EMSegment(vtkEMSegment) SetBiasRootFileName $EMSegment(BiasRootFileName)}
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentAlgorithmStart
# Sets up the segmentation algorithm
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentAlgorithmStart { } {
   global EMSegment Volume 
   set NumInputImagesSet 0
   if {$EMSegment(SegmentMode) > 0} {
       # EMLocalSegmentation: Multiple Input Images
       vtkImageEMLocalSegmenter EMSegment(vtkEMSegment) 
       # How many input images do you have
       EMSegment(vtkEMSegment) SetNumInputImages $EMSegment(NumInputChannel) 
       EMSegment(vtkEMSegment) SetNumberOfTrainingSamples $EMSegment(NumberOfTrainingSamples)
       EMSegment(vtkEMSegment) SetBiasPrint $EMSegment(BiasPrint)

       if {$EMSegment(SegmentMode) > 1} {
          set NumInputImagesSet [EMSegmentSetVtkSuperClassSetting 0 0]
 
          # Transfer image information
          foreach v $EMSegment(SelVolList,VolumeList) {       
            EMSegment(vtkEMSegment) SetInputIndex $NumInputImagesSet [Volume($v,vol) GetOutput]
            incr NumInputImagesSet
          }
          # Transfer Bias Print out Information
          EMSegment(vtkEMSegment) SetPrintIntermediateDir $EMSegment(PrintIntermediateDir)
          EMSegment(vtkEMSegment) SetNumEMShapeIter  $EMSegment(EMShapeIter)  

      # This is for debuging purposes so extra volumes can be loaded into the segmentation process 
      if {$EMSegment(DebugVolume)} {
          set index 1 
          set foundindex 0
          while {$foundindex > -1} {
          set foundindex [lsearch -exact $EMSegment(VolumeNameList)  EMDEBUG${index}] 
          if {$foundindex > -1} {
              EMSegment(vtkEMSegment) SetInputIndex $NumInputImagesSet [Volume([lindex $Volume(idList) $foundindex],vol) GetOutput]
              incr NumInputImagesSet
              incr index
          }
          }
      }
       } else {
          EMSegmentSetVtkClassSettingOld
       }
   } else {
       # EM Specific Information - Simple EM Algorithm
       vtkImageEMSegmenter EMSegment(vtkEMSegment)    
       EMSegment(vtkEMSegment) SetNumClasses     [llength $EMSegment(Cattrib,0,ClassList)]
       EMSegment(vtkEMSegment) SetImgTestNo       -1 
       EMSegment(vtkEMSegment) SetImgTestDivision 0
       EMSegment(vtkEMSegment) SetImgTestPixel    0
       # Transfer image information
       EMSegment(vtkEMSegment) SetInput [Volume([lindex $EMSegment(SelVolList,VolumeList) 0],vol) GetOutput]
       set xindex 1 
       foreach i $EMSegment(Cattrib,0,ClassList) {
          EMSegment(vtkEMSegment) SetProbability  $EMSegment(Cattrib,$i,Prob)              $xindex
          EMSegment(vtkEMSegment) SetMu           $EMSegment(Cattrib,$i,LogMean,0)         $xindex
          EMSegment(vtkEMSegment) SetSigma        $EMSegment(Cattrib,$i,LogCovariance,0,0) $xindex 
          EMSegment(vtkEMSegment) SetLabel        $EMSegment(Cattrib,$i,Label)             $xindex
          # Reads in the value for each class individually
          set yindex 1
          foreach j $EMSegment(Cattrib,0,ClassList) { 
             for {set k 0} { $k< 6} {incr k} {
               EMSegment(vtkEMSegment) SetMarkovMatrix $EMSegment(Cattrib,0,CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) [expr $k+1] $yindex $xindex
             }
             incr yindex
          }
          incr xindex
       }
   }

   #----------------------------------------------------------------------------
   # Transfering General Information
   #----------------------------------------------------------------------------
   EMSegment(vtkEMSegment) SetNumIter         $EMSegment(EMiteration)  
   EMSegment(vtkEMSegment) SetNumRegIter      $EMSegment(MFAiteration) 
   EMSegment(vtkEMSegment) SetAlpha           $EMSegment(Alpha) 

   EMSegment(vtkEMSegment) SetSmoothingWidth  $EMSegment(SmWidth)    
   EMSegment(vtkEMSegment) SetSmoothingSigma  $EMSegment(SmSigma)      

   EMSegment(vtkEMSegment) SetStartSlice      $EMSegment(StartSlice)
   EMSegment(vtkEMSegment) SetEndSlice        $EMSegment(EndSlice)

   EMSegment(vtkEMSegment) SetPrintIntermediateResults  $EMSegment(PrintIntermediateResults) 
   EMSegment(vtkEMSegment) SetPrintIntermediateSlice  $EMSegment(PrintIntermediateSlice) 
   EMSegment(vtkEMSegment) SetPrintIntermediateFrequency  $EMSegment(PrintIntermediateFrequency) 

   return $NumInputImagesSet
}

#-------------------------------------------------------------------------------
# .PROC  EMSegmentSuperClassChildren 
# Finds out the all children, grandchildren and ... of a super class
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSuperClassChildren {SuperClass} {
    global EMSegment
    if {$EMSegment(Cattrib,$SuperClass,IsSuperClass) == 0} {
    return     $EMSegment(Cattrib,$SuperClass,Label)
    }
    set result ""
    foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
    if {$EMSegment(Cattrib,$i,IsSuperClass)} {
           # it is defined as SetType<TYPE> <ID>  
       set result "$result [EMSegmentSuperClassChildren $i]" 
    } else {
        lappend result $EMSegment(Cattrib,$i,Label)  
    }
     } 
     return $result
 }

#-------------------------------------------------------------------------------
# .PROC EMSegmentTrainCIMField
# Traines the CIM Field with a given Image
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentTrainCIMField {} {
    global EMSegment Volume
    # Transferring Information
    foreach i $EMSegment(GlobalSuperClassList) {
       vtkImageEMMarkov EMCIM    
       # EM Specific Information
       set NumClasses [llength $EMSegment(Cattrib,$i,ClassList)]
       EMCIM SetNumClasses     $NumClasses  
       EMCIM SetStartSlice     $EMSegment(StartSlice)
       EMCIM SetEndSlice       $EMSegment(EndSlice)

        # Kilian : Get rid of those 
        EMCIM SetImgTestNo       -1 
        EMCIM SetImgTestDivision  0 
        EMCIM SetImgTestPixel     0 
        set index 0
        foreach j $EMSegment(Cattrib,$i,ClassList) {
        set LabelList [EMSegmentSuperClassChildren $j]
        EMCIM SetLabelNumber $index [llength $LabelList]
        foreach l $LabelList {
        EMCIM SetLabel $index $l
        }
      incr index
    }

    # Transfer image information
    EMCIM SetInput [Volume($Volume(activeID),vol) GetOutput]
    set data [EMCIM GetOutput]
    # This Command calls the Thread Execute function
    $data Update
    set xindex 0 
    foreach x $EMSegment(Cattrib,$i,ClassList) {
        set EMSegment(Cattrib,$x,Prob) [EMCIM GetProbability $xindex]
        set yindex 0
        # EMCIM traines the matrix (y=t, x=t-1) just the other way EMSegment (y=t-1, x=t) needs it - Sorry !
        foreach y $EMSegment(Cattrib,$i,ClassList) {
        for {set z 0} {$z < 6} {incr z} {
            # Error made in x,y coordinates in EMSegment - I checked everything - it workes in XML and CIM Display in correct order - so do not worry - it is just a little bit strange - but strange things happen
            set temp [$data GetScalarComponentAsFloat $yindex $xindex  $z 0]
            set EMSegment(Cattrib,$i,CIMMatrix,$x,$y,[lindex $EMSegment(CIMList) $z]) [expr round($temp*100000)/100000.0]        
        }
        incr yindex
        }
        incr xindex
    }
    # Delete instance
    EMCIM Delete
    }
    # Jump to edit field 
    # EMSegmentExecuteCIM Edit
    # Move Button to Edit
    # set EMSegment(TabbedFrame,$EMSegment(Ma-tabCIM),tab) Edit
}
