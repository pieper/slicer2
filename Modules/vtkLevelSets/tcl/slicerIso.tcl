#   ==================================================
#   Module : vtkLevelSets
#   Authors: Karl Krissian
#   Email  : karl@bwh.harvard.edu
#
#   This module implements a Active Contour evolution
#   for segmentation of 2D and 3D images.
#   It implements a 'codimension 2' levelsets as an
#   option for the smoothing term.
#   It comes with a Tcl/Tk interface for the '3D Slicer'.
#   ==================================================
#   Copyright (C) 2003  LMI, Laboratory of Mathematics in Imaging, 
#   Brigham and Women's Hospital, Boston MA USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License as published by the Free Software Foundation; either
#   version 2.1 of the License, or (at your option) any later version.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#   ================================================== 
#   The full GNU Lesser General Public License file is in vtkLevelSets/LesserGPL_license.txt




#
# Info on display
#
# files tcl-main/MainSlices.tcl  MainRender.tcl
#
#
# slice windows: vtkXOpenGLRenderWindow
# sl0win, sl1win, sl2win
#
# sl0Mapper  vtkOpenGLImageMapper
# sl0Actor   vtkActor2D
# sl0Imager  vtkOpenGLRenderer
# sl0Win     vtkXOpenGLRenderWindow
#
#
# Slice visible in the 3D window?
# Slice(id,visibility)
#
# Slice(id,planeSource)
# Slice(id,texture)
# Slice(id,planeMapper)
# Slice(id,planeActor)
#
#
# slice zoom
# Slice(id,zoom) 
# Slicer SetZoomNew 0 1
# Slicer Update
#

# in MainSlices.tcl
#
# in MainSlicesBuildVTK{}
#
# "    foreach s $Slice(idList) {
#        #Build VTK objects
#        MainSlicesBuildVTKForSliceActor $s
#  
#        #Set input from vtkMrmlSlicer Slicer object
#        Slice($s,texture)      SetInput [Slicer GetOutput $s]
#        Slice($s,outlineActor) SetUserMatrix [Slicer GetReformatMatrix $s]
#        Slice($s,planeActor)   SetUserMatrix [Slicer GetReformatMatrix $s]
#    
#        # add to the scene
#        MainAddActor Slice($s,outlineActor)
#        MainAddActor Slice($s,planeActor)
#
#        # Clip
#        MainSlicesSetClipState $s
#    }"
#
#
#

#
# slice position
# Slice($s,offset)
#

# Slicer: vtkMrmlSlicer
#
# SetNoneVolume
# SetBackVolume
# SetForeVolume
# SetLavelVolume
#
# SetFieldOfView
# SetForeFade
# SetLabelIndirectLUT
# SetForeOpacity
#
#
# Interesting: SetFirstFilter, SetLastFilter
#
# Creating the slices:
# BuildUpper, BuildLower
#


#======================================================================
proc SGetImage {volname} {
#    ---------
  global Volume

  foreach vid $Volume(idList) {
      if {[Volume($vid,node) GetName] == $volname} {
      return [Volume($vid,vol) GetOutput]
      }
  }
  puts "no volume with this id \n"
  return -1
}

#======================================================================
proc SGetImageId {volname} {
#    -----------
  global Volume

  foreach vid $Volume(idList) {
      if {[Volume($vid,node) GetName] == $volname} {
      return $vid;
      }
  }
  puts "no volume with this id \n"
  return -1
}

#======================================================================
proc SGetSurface {modelname} {
#    -----------
  global Model

  foreach vid $Model(idList) {
      if {[Model($vid,node) GetName] == $modelname} {
      return $Model($vid,polyData)
      }
  }
  puts "no surface with this id \n"
  return -1
}

#======================================================================
proc SGetSurfaceId {modelname} {
#    -------------
  global Model

  foreach vid $Model(idList) {
      if {[Model($vid,node) GetName] == $modelname} {
      return $vid;
      }
  }
  puts "no surface with this id \n"
  return -1
}


#======================================================================
proc SAddMrmlImage {imname extension } {
#    -------------
  global Volume

  set imid [SGetImageId $imname]
  set resname [append imname $extension]
  set newvol [DevCreateNewCopiedVolume $imid ""  $resname ]
  set node [Volume($newvol,vol) GetMrmlNode]
  Mrml(dataTree) RemoveItem $node 
  set nodeBefore [Volume($imid,vol) GetMrmlNode]
  Mrml(dataTree) InsertAfterItem $nodeBefore $node
  MainUpdateMRML

  return $newvol
}


#======================================================================
proc SSaveImage {imname filename } {
#    ----------
  global Volume

}


#======================================================================
proc DisplayMatrix {mat} {
#    -------------
    set e00 [$mat GetElement 0 0]
    set e01 [$mat GetElement 0 1]
    set e02 [$mat GetElement 0 2]
    set e03 [$mat GetElement 0 3]

    set e10 [$mat GetElement 1 0]
    set e11 [$mat GetElement 1 1]
    set e12 [$mat GetElement 1 2]
    set e13 [$mat GetElement 1 3]

    set e20 [$mat GetElement 2 0]
    set e21 [$mat GetElement 2 1]
    set e22 [$mat GetElement 2 2]
    set e23 [$mat GetElement 2 3]

    set e30 [$mat GetElement 3 0]
    set e31 [$mat GetElement 3 1]
    set e32 [$mat GetElement 3 2]
    set e33 [$mat GetElement 3 3]

  puts "$e00 $e01 $e02 $e03"
  puts "$e10 $e11 $e12 $e13"
  puts "$e20 $e21 $e22 $e23"
  puts "$e30 $e31 $e32 $e33"
  
}


#======================================================================
proc SInvertIntensity {imname sd} {
#    ----------------

  global Volume


  vtkImageMathematics vtk_immath
  vtk_immath SetInput1 [SGetImage $imname]
  vtk_immath SetOperationToMultiplyByK
  vtk_immath SetConstantK -1

  vtk_immath Update

  set newvol [SAddMrmlImage $imname "_invert"]

  Volume($newvol,vol) SetImageData [vtk_immath GetOutput]
  MainVolumesUpdate $newvol

  vtk_immath Delete

  return [append $imname "_invert"]

}

#======================================================================
proc SSmooth {imname sd} {
#    -------
  global Volume


  vtkImageGaussianSmooth smooth
  smooth SetNumberOfThreads 8
  smooth SetInput [SGetImage $imname]
  smooth SetDimensionality 3
  smooth SetStandardDeviations $sd $sd $sd
  smooth SetProgressMethod {set pro [smooth GetProgress]; puts "Completed $pro"; flush stdout}
  smooth Update

  set newvol [SAddMrmlImage $imname "_smooth"]

  Volume($newvol,vol) SetImageData [smooth GetOutput]
  MainVolumesUpdate $newvol

  smooth Delete

  return [append $imname "_smooth"]

}

#======================================================================
proc SUpSample {imname fx fy fz} {
#    ---------

  global Volume

  # vtk resampling
  vtkImageResample magnify
  magnify SetDimensionality 3
  magnify SetInput [SGetImage $imname]
  magnify SetAxisMagnificationFactor 0 ${fx}
  magnify SetAxisMagnificationFactor 1 ${fy}
  magnify SetAxisMagnificationFactor 2 ${fz}
  magnify ReleaseDataFlagOff
  magnify Update

  set newvol [SAddMrmlImage $imname "_upsampled"]
  puts "newvol = ${newvol}"
  Volume($newvol,vol) SetImageData [magnify GetOutput]
  magnify     Delete
  set imid [SGetImageId $imname]

  # Set  new dimensions
  set dim [Volume($imid,node) GetDimensions]
  Volume($newvol,node) SetDimensions [expr round([lindex $dim 0]*${fx})]  [expr round([lindex $dim 1]*${fy})]
  # Set  new range
  set range   [Volume($imid,node) GetImageRange]
  Volume($newvol,node) SetImageRange [expr round([lindex $range 0]*${fz})] [expr round([lindex $range 1]*${fz})]

  # Set new spacing
  set spacing [Volume($imid,node) GetSpacing]
  Volume($newvol,node) SetSpacing [expr 1.0*[lindex ${spacing} 0]/${fx}] [expr 1.0*[lindex ${spacing} 1]/${fy}] [expr 1.0*[lindex ${spacing} 2]/${fz}]

  puts "Spacing: \n"
  puts [Volume($newvol,node) GetSpacing]

  MainUpdateMRML
  MainVolumesUpdate $newvol

  # update matrices
  Volume($newvol,node) ComputeRasToIjkFromScanOrder [Volume($imid,node) GetScanOrder]


  # Set the RasToWld matrix
  # Ras2ToWld = Ras2ToIjk2 x Ijk2ToIjk1 x Ijk1ToRas1 x Ras1ToWld
  puts "Set the RasToWld matrix\n"
  set ras1wld1 [Volume($imid,node)   GetRasToWld]

  # It's weird ... : I need to call SetRasToWld in order to update RasToIjk !!!
  Volume($newvol,node) SetRasToWld $ras1wld1

  set ras2ijk2 [Volume($newvol,node) GetRasToIjk]

  vtkMatrix4x4 ijk2ijk1
  ijk2ijk1 Identity
  ijk2ijk1 SetElement 0 0 [expr 1.0/${fx}]
  ijk2ijk1 SetElement 1 1 [expr 1.0/${fy}]
  ijk2ijk1 SetElement 2 2 [expr 1.0/${fz}]

  vtkMatrix4x4 ijk1ras1 
  ijk1ras1 DeepCopy [Volume($imid,node) GetRasToIjk]
  ijk1ras1 Invert

  vtkMatrix4x4 ras2wld2 
  ras2wld2 Identity
  ras2wld2 Multiply4x4 ijk2ijk1  $ras2ijk2  ras2wld2
  ras2wld2 Multiply4x4 ijk1ras1  ras2wld2   ras2wld2
  ras2wld2 Multiply4x4 $ras1wld1 ras2wld2   ras2wld2

  Volume($newvol,node) SetRasToWld ras2wld2
  
  MainVolumesUpdate $newvol

  ijk2ijk1    Delete
  ijk1ras1    Delete
  ras2wld2    Delete

  return [append $imname "_upsampled"]

}


#======================================================================
proc SAniso {imname sd th it} {
#    ------
  global Volume

  vtkAnisoGaussSeidel aniso
  aniso SetInput [SGetImage $imname]
  aniso Setmode 3
  aniso Setsigma $sd 
  aniso Setk $th
  aniso SetNumberOfIterations $it
#  aniso SetProgressMethod {set pro [aniso GetProgress]; puts "Completed $pro"; flush stdout}
  aniso Update

  set newvol [SAddMrmlImage $imname "_aniso"]

  Volume($newvol,vol) SetImageData [aniso GetOutput]
  MainVolumesUpdate $newvol

  aniso Delete

  return [append $imname "_aniso"]

}


#======================================================================
proc SSubVol {imname x1 x2 y1 y2 z1 z2 } {
#    -------
  global Volume

  set  I [SGetImage $imname]

  vtkImageData sI 
  sI SetScalarType [$I GetScalarType]
  sI SetNumberOfScalarComponents [$I GetNumberOfScalarComponents]
  set spacing [$I GetSpacing]
  sI SetSpacing [lindex $spacing 0] [lindex $spacing 1] [lindex $spacing 2]
  sI SetDimensions [expr $x2-$x1+1] [expr $y2-$y1+1] [expr $z2-$z1+1]

  sI CopyAndCastFrom $I $x1 $x2 $y1 $y2 $z1 $z2

  set newvol [SAddMrmlImage $imname "_subvol"]

  Volume($newvol,vol) SetImageData [sI]
  MainVolumesUpdate $newvol

  return [append $imname "_subvol"]
}


#-------------------------------------------------------------------------------
# .PROC SModelMakerCreate
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SModelMakerCreate { volname name threshold smooth decimate} {
    global Model ModelMaker Label

    set ModelMaker(name)     $name
    set ModelMaker(smooth)   $smooth
    set ModelMaker(decimate) $decimate

    if {[ValidateName $ModelMaker(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }

    # Validate smooth
    if {[ValidateInt $ModelMaker(smooth)] == 0} {
        tk_messageBox -message "The number of smoothing iterations must be an integer."
        return
    }

    # Validate decimate
    if {[ValidateInt $ModelMaker(decimate)] == 0} {
        tk_messageBox -message "The number of decimate iterations must be an integer."
        return
    }

    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $ModelMaker(name)
    $n SetColor $Label(name)

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    MainModelsCreate $m

    # Registration
    #
    # Here needs ModelMaker(idVolume) !!!
    #
    set ModelMaker(idVolume) [SGetImageId $volname]
    set v $ModelMaker(idVolume)

    Model($m,node) SetRasToWld [Volume($v,node) GetRasToWld]

    if {[SModelMakerMarch $m $v $threshold $ModelMaker(decimate) $ModelMaker(smooth)] != 0} {
        MainModelsDelete $m
        $ModelMaker(bCreate) config -state normal
        return
    }
    $ModelMaker(msg) config -text "\
Marching cubes: $ModelMaker(t,mcubes) sec.\n\
Decimate: $ModelMaker(t,decimator) sec.\n\
Smooth: $ModelMaker(t,smoother) sec.\n\
$ModelMaker(n,mcubes) polygons reduced to $ModelMaker(n,decimator)."

    MainUpdateMRML
    MainModelsSetActive $m
    $ModelMaker(bCreate) config -state normal
    set name [Model($m,node) GetName]
    tk_messageBox -message "The model '$name' has been created."
}



#-------------------------------------------------------------------------------
# .PROC SModelMakerMarch
# 
# 
# .END
#-------------------------------------------------------------------------------
proc SModelMakerMarch {m v threshold decimateIterations smoothIterations} {
    global Model ModelMaker Gui Label Module
    
#    if {$ModelMaker(marching) == 1} {
#        puts "already marching"
#        return -1
#    }

#    set ModelMaker(marching) 1
    set name [Model($m,node) GetName]

    # Marching cubes cannot run on data of dimension less than 3
    set dim [[Volume($v,vol) GetOutput] GetExtent]
    if {[lindex $dim 0] == [lindex $dim 1] ||
        [lindex $dim 2] == [lindex $dim 3] ||
        [lindex $dim 4] == [lindex $dim 5]} {
        puts "extent=$dim"
        tk_messageBox -message "The volume '[Volume($v,node) GetName]' is not 3D"
        set ModelMaker(marching) 0
        return -1
    }

    set spacing [[Volume($v,vol) GetOutput] GetSpacing]
    set origin  [[Volume($v,vol) GetOutput] GetOrigin]
    # The spacing is accounted for in the rasToVtk transform, 
    # so we have to remove it here, or mcubes will use it.
    [Volume($v,vol) GetOutput] SetSpacing 1 1 1
    [Volume($v,vol) GetOutput] SetOrigin 0 0 0
    
    # Read orientation matrix and permute the images if necessary.
    vtkTransform rot
    set matrixList [Volume($v,node) GetRasToVtkMatrix]
    for {set row 0} { $row < 4 } {incr row} {
        for {set col 0} {$col < 4} {incr col} {
            [rot GetMatrix] SetElement $row $col \
                [lindex $matrixList [expr $row*4+$col]]
        }
    }
    [rot GetMatrix] Invert


    #
    # Marching Cubes
    #
    set p mcubes
    vtkMarchingCubes $p
#    $p SetInput [to GetOutput]
    $p SetInput [Volume($v,vol) GetOutput]
    $p SetValue 0 $threshold
    $p ComputeScalarsOff
    $p ComputeGradientsOff
    $p ComputeNormalsOff
    [$p GetOutput] ReleaseDataFlagOn
    set Gui(progressText) "Marching $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    set ModelMaker(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
    set ModelMaker(n,$p) [[$p GetOutput] GetNumberOfPolys]

    # If there are no polygons, then the smoother gets mad, so stop.
    if {$ModelMaker(n,$p) == 0} {
        tk_messageBox -message "No polygons can be created."
        mcubes SetInput ""
        rot Delete
        mcubes Delete
        set ModelMaker(marching) 0
        eval [Volume($v,vol) GetOutput] SetSpacing $spacing
        eval [Volume($v,vol) GetOutput] SetOrigin $origin
        return -1
    }

    #
    # Decimation
    #
    set p decimator
    vtkDecimate $p
    $p SetInput [mcubes GetOutput]
    $p SetInitialFeatureAngle 60
    $p SetMaximumIterations $decimateIterations
    $p SetMaximumSubIterations 0
    $p PreserveEdgesOn
    $p SetMaximumError 1
    $p SetTargetReduction 1
    $p SetInitialError .0002
    $p SetErrorIncrement .0002
    [$p GetOutput] ReleaseDataFlagOn
    set Gui(progressText) "Decimating $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    set ModelMaker(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
    set ModelMaker(n,$p) [[$p GetOutput] GetNumberOfPolys]
    
    vtkReverseSense reverser

    # Do normals need reversing?
    set mm [rot GetMatrix] 
    if {[$mm Determinant] < 0} {
#      
# History: In a note to Samson Timoner, Dave Gering wrote:
# With some scan orders (AP PA LR RL IS SI), the normals need to be reversed
# for proper surface rendering. I meant to one day validate that this was
# happening correctly, but I never got around to making a model from every
# type of scan order. The popup was to aid my testing, and it certainly
# shouldn't still be in there!!
#
#        tk_messageBox -message Reverse
        set p reverser
        $p SetInput [decimator GetOutput]
        $p ReverseNormalsOn
        [$p GetOutput] ReleaseDataFlagOn
        set Gui(progressText) "Reversing $name"
        $p SetStartMethod     MainStartProgress
        $p SetProgressMethod "MainShowProgress $p"
        $p SetEndMethod       MainEndProgress
    }


#    if { }
    #
    # Smoothing
    #
    vtkSmoothPolyDataFilter smoother
    smoother SetInput [$p GetOutput]
    set p smoother
    $p SetNumberOfIterations $smoothIterations
    # This next line massively rounds corners
    $p SetRelaxationFactor 0.33
    $p SetFeatureAngle 60
    $p FeatureEdgeSmoothingOff
    $p BoundarySmoothingOff
    $p SetConvergence 0
    [$p GetOutput] ReleaseDataFlagOn
    set Gui(progressText) "Smoothing $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    set ModelMaker(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
    set ModelMaker(n,$p) [[$p GetOutput] GetNumberOfPolys]
    set ModelMaker($m,nPolys) $ModelMaker(n,$p)

    #
    # Transformation
    #
    set p transformer
    vtkTransformPolyDataFilter $p
    $p SetInput [smoother GetOutput]
    $p SetTransform rot
    set Gui(progressText) "Transforming $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    [$p GetOutput] ReleaseDataFlagOn

    #
    # Normals
    #
    set p normals
    vtkPolyDataNormals $p
    $p SetInput [transformer GetOutput]
    $p SetFeatureAngle 60
    set Gui(progressText) "Normals $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    [$p GetOutput] ReleaseDataFlagOn

    #
    # Stripping
    #
    set p stripper
    vtkStripper $p
    $p SetInput [normals GetOutput]
    set Gui(progressText) "Stripping $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    stripper SetOutput ""

    SCleanMarch

    # Restore spacing
    eval [Volume($v,vol) GetOutput] SetSpacing $spacing
    eval [Volume($v,vol) GetOutput] SetOrigin $origin

    return 0
}


#----------------------------------------------------------------------
proc SCleanMarch {} {
    foreach p "mcubes decimator reverser transformer smoother normals stripper" {
         $p SetInput ""
         $p Delete
    }
    rot Delete
    set ModelMaker(marching) 0
}

#----------------------------------------------------------------------
proc SModel_IJK_2_RAS { volID modelname newmodelname } {
#
# Not tested yet ...
#
    global Model ModelMaker Label Module

    set ModelMaker(name)     $newmodelname
    if {[ValidateName $ModelMaker(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }
    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $ModelMaker(name)
    $n SetColor $Label(name)

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    MainModelsCreate $m

    # Registration
    #
    # Here needs ModelMaker(idVolume) !!!
    #
    set ModelMaker(idVolume) $volID
    set v $ModelMaker(idVolume)

    Model($m,node) SetRasToWld [Volume($v,node) GetRasToWld]

    # Here process the conversion
    # 1. get the transformation
    # Read orientation matrix and permute the images if necessary.
    vtkTransform rot
    set matrixList [Volume($v,node) GetRasToVtkMatrix]
    for {set row 0} { $row < 4 } {incr row} {
        for {set col 0} {$col < 4} {incr col} {
            [rot GetMatrix] SetElement $row $col \
                [lindex $matrixList [expr $row*4+$col]]
        }
    }
    [rot GetMatrix] Invert
    
    # 2. process Transformation
    set p transformer
    vtkTransformPolyDataFilter $p
    $p SetInput [SGetSurface $modelname]
    $p SetTransform rot
    [$p GetOutput] ReleaseDataFlagOn

    vtkReverseSense reverser
    # Do normals need reversing?
    set mm [rot GetMatrix] 
    if {[$mm Determinant] < 0} {
        reverser SetInput [$p GetOutput]
        set p reverser
        $p ReverseNormalsOn
        [$p GetOutput] ReleaseDataFlagOn
    }

    # Normals
    vtkPolyDataNormals normals
    normals SetInput [$p GetOutput]
    set p normals
    $p SetFeatureAngle 60
    [$p GetOutput] ReleaseDataFlagOn

    # Stripping
    set p stripper
    vtkStripper $p
    $p SetInput [normals GetOutput]
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    stripper SetOutput ""

    foreach p "transformer reverser normals stripper" {
         $p SetInput ""
         $p Delete
    }
    rot Delete

    MainUpdateMRML
    MainModelsSetActive $m
    $ModelMaker(bCreate) config -state normal
    set name [Model($m,node) GetName]
    tk_messageBox -message "The model '$newmodelname' has been created."

} 
# SModel_IJK_2_RAS



#----------------------------------------------------------------------
proc SModelLine_IJK_2_RAS { volID modelname newmodelname } {
#
# Not tested yet ...
#
    global Model ModelMaker Label Module

    set ModelMaker(name)     $newmodelname
    if {[ValidateName $ModelMaker(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }
    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $ModelMaker(name)
    $n SetColor $Label(name)

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    MainModelsCreate $m

    # Registration
    #
    # Here needs ModelMaker(idVolume) !!!
    #
    set ModelMaker(idVolume) $volID
    set v $ModelMaker(idVolume)

    Model($m,node) SetRasToWld [Volume($v,node) GetRasToWld]

    # Here process the conversion
    # 1. get the transformation
    # Read orientation matrix and permute the images if necessary.
    vtkTransform rot
    set matrixList [Volume($v,node) GetRasToVtkMatrix]
    for {set row 0} { $row < 4 } {incr row} {
        for {set col 0} {$col < 4} {incr col} {
            [rot GetMatrix] SetElement $row $col \
                [lindex $matrixList [expr $row*4+$col]]
        }
    }
    [rot GetMatrix] Invert
    
    # 2. process Transformation
    set p transformer
    vtkTransformPolyDataFilter $p
    $p SetInput [SGetSurface $modelname]
    $p SetTransform rot
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }

    transformer SetOutput ""
    transformer SetInput ""
    transformer Delete
    rot Delete

    MainUpdateMRML
    MainModelsSetActive $m
    $ModelMaker(bCreate) config -state normal
    set name [Model($m,node) GetName]
    tk_messageBox -message "The model '$newmodelname' has been created."

} 
# SModelLine_IJK_2_RAS



#----------------------------------------------------------------------
proc SModel_RAS_2_IJK { volID modelname newmodelname } {
#
# Not tested yet ...
#
    global Model ModelMaker Label Module

    set ModelMaker(name)     $newmodelname
    if {[ValidateName $ModelMaker(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }
    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $ModelMaker(name)
    $n SetColor $Label(name)

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    MainModelsCreate $m

    # Registration
    #
    # Here needs ModelMaker(idVolume) !!!
    #
    set ModelMaker(idVolume) $volID
    set v $ModelMaker(idVolume)

    Model($m,node) SetRasToWld [Volume($v,node) GetRasToWld]

    # Here process the conversion
    # 1. get the transformation
    # Read orientation matrix and permute the images if necessary.
    vtkTransform rot
    set matrixList [Volume($v,node) GetRasToVtkMatrix]
    for {set row 0} { $row < 4 } {incr row} {
        for {set col 0} {$col < 4} {incr col} {
            [rot GetMatrix] SetElement $row $col \
                [lindex $matrixList [expr $row*4+$col]]
        }
    }
#    [rot GetMatrix] Invert
    
    # 2. process Transformation
    set p transformer
    vtkTransformPolyDataFilter $p
    $p SetInput [SGetSurface $modelname]
    $p SetTransform rot
    [$p GetOutput] ReleaseDataFlagOn

    vtkReverseSense reverser
    # Do normals need reversing?
    set mm [rot GetMatrix] 
    if {[$mm Determinant] < 0} {
        reverser SetInput [$p GetOutput]
        set p reverser
        $p ReverseNormalsOn
        [$p GetOutput] ReleaseDataFlagOn
    }

    # Normals
    vtkPolyDataNormals normals
    normals SetInput [$p GetOutput]
    set p normals
    $p SetFeatureAngle 60
    [$p GetOutput] ReleaseDataFlagOn

    # Stripping
    set p stripper
    vtkStripper $p
    $p SetInput [normals GetOutput]
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    stripper SetOutput ""

    foreach p "transformer reverser normals stripper" {
         $p SetInput ""
         $p Delete
    }
    rot Delete

    MainUpdateMRML
    MainModelsSetActive $m
    $ModelMaker(bCreate) config -state normal
    set name [Model($m,node) GetName]
    tk_messageBox -message "The model '$newmodelname' has been created."

} 
# RAS_2_IJK


#----------------------------------------------------------------------
proc SModel_Scale { modelname newmodelname sx sy sz} {
#
# Not tested yet ...
#
    global Model ModelMaker Label Module

    set ModelMaker(name)     $newmodelname
    if {[ValidateName $ModelMaker(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }
    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $ModelMaker(name)
    $n SetColor $Label(name)

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    MainModelsCreate $m

    # Here process the conversion
    # 1. get the transformation
    # Read orientation matrix and permute the images if necessary.
    vtkTransform Tscale
    Tscale Identity
    Tscale Scale $sx $sy $sz
    
    # 2. process Transformation
    set p transformer
    vtkTransformPolyDataFilter $p
    $p SetInput [SGetSurface $modelname]
    $p SetTransform Tscale
    [$p GetOutput] ReleaseDataFlagOn

    vtkReverseSense reverser
    # Do normals need reversing?
    set mm [Tscale GetMatrix] 
    if {[$mm Determinant] < 0} {
        reverser SetInput [$p GetOutput]
        set p reverser
        $p ReverseNormalsOn
        [$p GetOutput] ReleaseDataFlagOn
    }

    # Normals
    vtkPolyDataNormals normals
    normals SetInput [$p GetOutput]
    set p normals
    $p SetFeatureAngle 60
    [$p GetOutput] ReleaseDataFlagOn

    # Stripping
    set p stripper
    vtkStripper $p
    $p SetInput [normals GetOutput]
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    stripper SetOutput ""

    foreach p "transformer reverser normals stripper" {
         $p SetInput ""
         $p Delete
    }
    Tscale Delete

    MainUpdateMRML
    MainModelsSetActive $m
    $ModelMaker(bCreate) config -state normal
    set name [Model($m,node) GetName]
    tk_messageBox -message "The model '$newmodelname' has been created."

} 
# SModel_Scale

#----------------------------------------------------------------------
proc SModelLine_Scale { modelname newmodelname sx sy sz} {
#
# Not tested yet ...
#
    global Model ModelMaker Label Module

    set ModelMaker(name)     $newmodelname
    if {[ValidateName $ModelMaker(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }
    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $ModelMaker(name)
    $n SetColor $Label(name)

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    MainModelsCreate $m

    # Here process the conversion
    # 1. get the transformation
    # Read orientation matrix and permute the images if necessary.
    vtkTransform Tscale
    Tscale Identity
    Tscale Scale $sx $sy $sz
    
    # 2. process Transformation
    set p transformer
    vtkTransformPolyDataFilter $p
    $p SetInput [SGetSurface $modelname]
    $p SetTransform Tscale
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }

    transformer SetOutput ""
    transformer SetInput ""
    transformer Delete
    Tscale Delete

    MainUpdateMRML
    MainModelsSetActive $m
    $ModelMaker(bCreate) config -state normal
    set name [Model($m,node) GetName]
    tk_messageBox -message "The model '$newmodelname' has been created."

} 
# SModelLine_Scale

#----------------------------------------------------------------------
proc SModelLine_Smooth { modelname newmodelname length } {
#
# Not tested yet ...
#
    global Model ModelMaker Label Module

    set ModelMaker(name)     $newmodelname
    if {[ValidateName $ModelMaker(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }
    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $ModelMaker(name)
    $n SetColor $Label(name)

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    MainModelsCreate $m

    # Here process the conversion
    # 1. get the transformation
    # Read orientation matrix and permute the images if necessary.
    vtkSplineFilter Tsmooth
    Tsmooth   SetInput [SGetSurface $modelname]
    Tsmooth SetSubdivideToLength
    Tsmooth SetLength $length
    [Tsmooth  GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [Tsmooth GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }

    Tsmooth SetOutput ""
    Tsmooth SetInput ""
    Tsmooth Delete

    MainUpdateMRML
    MainModelsSetActive $m
    $ModelMaker(bCreate) config -state normal
    set name [Model($m,node) GetName]
    tk_messageBox -message "The model '$newmodelname' has been created."

} 
# SModelLine_Smooth

#----------------------------------------------------------------------
proc SModelLine_FlipY { modelname newmodelname ty} {
#
# Not tested yet ...
#
    global Model ModelMaker Label Module

    set ModelMaker(name)     $newmodelname
    if {[ValidateName $ModelMaker(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }
    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $ModelMaker(name)
    $n SetColor $Label(name)

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    MainModelsCreate $m

    # Here process the conversion
    # 1. get the transformation
    # Read orientation matrix and permute the images if necessary.
    vtkTransform Tflip
    Tflip Identity
    Tflip Scale     0 -1  0
    Tflip Translate 0 [expr $ty -1] 0
    
    # 2. process Transformation
    set p transformer
    vtkTransformPolyDataFilter $p
    $p SetInput [SGetSurface $modelname]
    $p SetTransform Tflip
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }

    transformer SetOutput ""
    transformer SetInput ""
    transformer Delete
    Tflip Delete

    MainUpdateMRML
    MainModelsSetActive $m
    $ModelMaker(bCreate) config -state normal
    set name [Model($m,node) GetName]
    tk_messageBox -message "The model '$newmodelname' has been created."

} 
# SModelLine_Scale


#----------------------------------------------------------------------
proc SModel_Translation { modelname newmodelname tx ty tz} {
#
# Not tested yet ...
#
    global Model ModelMaker Label Module

    set ModelMaker(name)     $newmodelname
    if {[ValidateName $ModelMaker(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }
    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $ModelMaker(name)
    $n SetColor $Label(name)

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    MainModelsCreate $m

    # Here process the conversion
    # 1. get the transformation
    # Read orientation matrix and permute the images if necessary.
    vtkTransform Tscale
    Tscale Identity
    Tscale Translate $tx $ty $tz
    
    # 2. process Transformation
    set p transformer
    vtkTransformPolyDataFilter $p
    $p SetInput [SGetSurface $modelname]
    $p SetTransform Tscale
    [$p GetOutput] ReleaseDataFlagOn

    vtkReverseSense reverser
    # Do normals need reversing?
    set mm [Tscale GetMatrix] 
    if {[$mm Determinant] < 0} {
        reverser SetInput [$p GetOutput]
        set p reverser
        $p ReverseNormalsOn
        [$p GetOutput] ReleaseDataFlagOn
    }

    # Normals
    vtkPolyDataNormals normals
    normals SetInput [$p GetOutput]
    set p normals
    $p SetFeatureAngle 60
    [$p GetOutput] ReleaseDataFlagOn

    # Stripping
    set p stripper
    vtkStripper $p
    $p SetInput [normals GetOutput]
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    stripper SetOutput ""

    foreach p "transformer reverser normals stripper" {
         $p SetInput ""
         $p Delete
    }
    Tscale Delete

    MainUpdateMRML
    MainModelsSetActive $m
    $ModelMaker(bCreate) config -state normal
    set name [Model($m,node) GetName]
    tk_messageBox -message "The model '$newmodelname' has been created."

} 
# SModel_Translation


#----------------------------------------------------------------------
proc SModelLine_Translation { modelname newmodelname tx ty tz} {
#
# Not tested yet ...
#
    global Model ModelMaker Label Module

    set ModelMaker(name)     $newmodelname
    if {[ValidateName $ModelMaker(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }
    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $ModelMaker(name)
    $n SetColor $Label(name)

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    MainModelsCreate $m

    # Here process the conversion
    # 1. get the transformation
    # Read orientation matrix and permute the images if necessary.
    vtkTransform Tscale
    Tscale Identity
    Tscale Translate $tx $ty $tz
    
    # 2. process Transformation
    set p transformer
    vtkTransformPolyDataFilter $p
    $p SetInput [SGetSurface $modelname]
    $p SetTransform Tscale
    [$p GetOutput] ReleaseDataFlagOff


    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    $p SetOutput ""

    foreach p "transformer" {
         $p SetInput ""
         $p Delete
    }
    Tscale Delete

    MainUpdateMRML
    MainModelsSetActive $m
    $ModelMaker(bCreate) config -state normal
    set name [Model($m,node) GetName]
    tk_messageBox -message "The model '$newmodelname' has been created."

} 
# SModelLine_Translation


#----------------------------------------------------------------------
proc SModel_Reverse { modelname newmodelname } {
#
# Not tested yet ...
#
    global Model ModelMaker Label Module

    set ModelMaker(name)     $newmodelname
    if {[ValidateName $ModelMaker(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }
    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $ModelMaker(name)
    $n SetColor $Label(name)

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    MainModelsCreate $m

    
    vtkReverseSense reverser
    # Do normals need reversing?
    reverser SetInput [SGetSurface $modelname]
    set p reverser
    $p ReverseNormalsOn
    [$p GetOutput] ReleaseDataFlagOn

#    # Normals
#    vtkPolyDataNormals normals
#    normals SetInput [$p GetOutput]
#    set p normals
#    $p SetFeatureAngle 60
#    [$p GetOutput] ReleaseDataFlagOn

    # Stripping
    vtkStripper stripper
    stripper SetInput [$p GetOutput]
    set p stripper
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    stripper SetOutput ""

#    normals SetInput ""
#    normals Delete
    foreach p "reverser  stripper" {
         $p SetInput ""
         $p Delete
    }


    MainUpdateMRML
    MainModelsSetActive $m
    $ModelMaker(bCreate) config -state normal
    set name [Model($m,node) GetName]
    tk_messageBox -message "The model '$newmodelname' has been created."

} 
# SModel_Reverse




proc SMainCC { surfname} {

  set sid [SGetSurfaceId surfname]

  vtkPolyDataConnectivityFilter connect
  connect SetInput [Model($sid,node) GetOutput]
  connect SetExtractionModeToLargestRegion

  connect SetInput ""
  connect Delete
}

# SModel_RAS_2_IJK 3 LS-Model2-50 LS-Model2-50-IJK
# SModel_Scale LS-Model2-50-IJK LS-Model2-50-IJK2 1 1 0.5
# SModel_IJK_2_RAS 1 LS-Model2-50-IJK2 LS-Model2-50-IJK2-RAS
