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
# FILE:        EMSegmentAlgorithm.tcl
# PROCEDURES:  
#   EMSegmentAlgorithmStart
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
# .PROC EMSegmentAlgorithmStart
# Starts the EM Algorithm 
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
      if {$EMSegment(IntensityAvgClass) == $i} {
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
 

       set NumInputImagesSet [EMSegmentSetVtkSuperClassSetting 0 0]
 
       # Transfer image information
       foreach v $EMSegment(SelVolList,VolumeList) {       
          EMSegment(vtkEMSegment) SetInputIndex $NumInputImagesSet [Volume($v,vol) GetOutput]
          incr NumInputImagesSet
       }
       # Transfer Bias Print out Information
       EMSegment(vtkEMSegment) SetBiasPrint $EMSegment(BiasPrint)
       if {$EMSegment(BiasPrint)} {EMSegment(vtkEMSegment) SetBiasRootFileName $EMSegment(BiasRootFileName)}
   }

   # This is just here for memory -> that's how the structure was before introducing hirachy
   if {$EMSegment(SegmentMode) < 0} {
       # EMLocalSegmentation: Multiple Input Images
       vtkImageEMLocalSegmenter EMStart 
       # How many input images do you have
       EMStart SetNumInputImages $EMSegment(NumInputChannel)  
       EMStart SetNumberOfTrainingSamples $EMSegment(NumberOfTrainingSamples)

       # Transefer probability map
       set i 0
       while {$i< $EMSegment(NumClasses)} {
          incr i
          # Kilian Changed here from standard
          if {$EMSegment(Cattrib,$i,ProbabilityData) != 0} {
             EMStart SetProbDataLocal 1 $i
             EMStart SetInputIndex $NumInputImagesSet [Volume($EMSegment(Cattrib,$i,ProbabilityData),vol) GetOutput]
             incr NumInputImagesSet
          } else {
            EMStart SetProbDataLocal 0 $i 
          }
       }
       # Transfer image information
       foreach v $EMSegment(SelVolList,VolumeList) {
         EMStart SetInputIndex $NumInputImagesSet [Volume($v,vol) GetOutput]
         incr NumInputImagesSet
       }
       set index 1
       EMStart SetIntensityAvgClass $EMSegment(IntensityAvgClass)
       foreach v $EMSegment(SelVolList,VolumeList) {       
         EMStart SetIntensityAvgValuePreDef $EMSegment(IntensityAvgValue,$v) $index
         incr index
       } 
       # Transfer Bias Print out Information
       EMStart SetBiasPrint 0
   }
   if {$EMSegment(SegmentMode) == 0}  {
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

