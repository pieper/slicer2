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
proc EMSegmentAlgorithmStart { } {
   global EMSegment Volume 
   if {$EMSegment(SegmentMode) > 0} {
       # EMLocalSegmentation: Multiple Input Images
       vtkImageEMLocalSegmenter EMSegment(vtkEMSegment) 
       # How many input images do you have
       EMSegment(vtkEMSegment) SetNumInputImages $EMSegment(NumInputChannel) 
       EMSegment(vtkEMSegment) SetNumClasses $EMSegment(NumClasses)  
       for {set i 1} { $i<= $EMSegment(NumClasses)} {incr i} {
          EMSegment(vtkEMSegment) SetTissueProbability $EMSegment(Cattrib,$i,Prob) $i
       } 
       EMSegment(vtkEMSegment) SetNumberOfTrainingSamples $EMSegment(NumberOfTrainingSamples)
       # Transefer probability map
       set NumInputImagesSet 0
       set i 0
       while {$i< $EMSegment(NumClasses)} {
           incr i
           if {$EMSegment(Cattrib,$i,ProbabilityData) != $Volume(idNone)} {
              EMSegment(vtkEMSegment) SetProbDataLocal 1 $i
              EMSegment(vtkEMSegment) SetInputIndex $NumInputImagesSet [Volume($EMSegment(Cattrib,$i,ProbabilityData),vol) GetOutput]
              incr NumInputImagesSet
           } else {
              EMSegment(vtkEMSegment) SetProbDataLocal 0 $i 
           }
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
       EMSegment(vtkEMSegment) SetBiasPrint $EMSegment(BiasPrint)
       if {$EMSegment(BiasPrint)} {EMSegment(vtkEMSegment) SetBiasRootFileName $EMSegment(BiasRootFileName)}
   } else {
       # EM Specific Information - Simple EM Algorithm
       vtkImageEMSegmenter EMSegment(vtkEMSegment)    
       EMSegment(vtkEMSegment) SetNumClasses      $EMSegment(NumClasses)  

       EMSegment(vtkEMSegment) SetImgTestNo       $EMSegment(ImgTestNo)
       EMSegment(vtkEMSegment) SetImgTestDivision $EMSegment(ImgTestDivision)
       EMSegment(vtkEMSegment) SetImgTestPixel    $EMSegment(ImgTestPixel)
       for {set i 1} { $i<= $EMSegment(NumClasses)} {incr i} {
           EMSegment(vtkEMSegment) SetProbability  $EMSegment(Cattrib,$i,Prob) $i
       }
       # Transfer image information
       set NumInputImagesSet 1
       EMSegment(vtkEMSegment) SetInput [Volume([lindex $EMSegment(SelVolList,VolumeList) 0],vol) GetOutput]
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

   for {set i 1} { $i<= $EMSegment(NumClasses)} {incr i} {
       set yindex 1
       for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
           if {$EMSegment(SegmentMode)} {
              EMSegment(vtkEMSegment) SetLogMu $EMSegment(Cattrib,$i,LogMean,$y) $i $yindex
              for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {
                 EMSegment(vtkEMSegment) SetLogCovariance $EMSegment(Cattrib,$i,LogCovariance,$y,$x) $i $yindex [expr $x+1]
              }
              incr yindex
              } else {
                 EMSegment(vtkEMSegment) SetMu $EMSegment(Cattrib,$i,LogMean,$y) $i
                 EMSegment(vtkEMSegment) SetSigma $EMSegment(Cattrib,$i,LogCovariance,$y,$y) $i 
             }
       }
       EMSegment(vtkEMSegment) SetLabel   $EMSegment(Cattrib,$i,Label) $i 
       # Reads in the value for each class individually
       for {set j 1} { $j<= $EMSegment(NumClasses)} {incr j} {
       for {set k 0} { $k< 6} {incr k} {
           EMSegment(vtkEMSegment) SetMarkovMatrix $EMSegment(CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) [expr $k+1] $j $i
       }
       }
   }
   return $NumInputImagesSet
}
