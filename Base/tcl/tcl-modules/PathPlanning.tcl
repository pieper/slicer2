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
# FILE:        PathPlanning.tcl
# PROCEDURES:  
#   PathPlanningInit
#   PathPlanningBuildVTK
#   PathPlanningSetPlaneOffset
#   PathPlanningSetVoxelSize
#   PathPlanningSetMaximumDistance
#   PathPlanningSetSource
#   PathPlanningSetSink
#   PathPlanningVisualizeMaps
#   PathPlanningExtractCenterline
#   PathPlanningMakeMrmlVolume
#==========================================================================auto=
    


#-------------------------------------------------------------------------------
# .PROC PathPlanningInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PathPlanningInit {} {
    global PathPlanning Module Model Path Advanced View Gui 
    

    set m PathPlanning

    set Module($m,depend) ""
    set Module($m,author) "Delphine Nain, MIT, delfin@ai.mit.edu"
    set Module($m,category) [lindex $Module(categories) 1]
    set Module($m,overview) "Path planning algorithms for use with Endoscopic module"

    
    # Define Procedures
    #------------------------------------
    
    set Module($m,procVTK) PathPlanningBuildVTK

    set PathPlanning(source) 0
    set PathPlanning(sink) 0
    set PathPlanning(source,coord) "0 0 0"
    set PathPlanning(sink,coord) "0 0 0"
    set PathPlanning(evalShortestPath) 1
    set PathPlanning(lastActiveModel) ""

    set PathPlanning(useExponential) 1
    set PathPlanning(useSquared) 0
    set PathPlanning(useLinear) 0
}


#-------------------------------------------------------------------------------
# .PROC PathPlanningBuildVTK
#  Creates the vtk objects for this module
#  
#  effects: calls PathPlanningCreateFocalPoint, 
#           PathPlanningCreateCamera, PathPlanningCreateLandmarks and 
#           PathPlanningCreatePath   
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PathPlanningBuildVTK {} {
    global PathPlanning Model
    
    # plane actor stuff
    vtkMatrix4x4 PathPlanning(plane,matrix)
    vtkMatrixToLinearTransform PathPlanning(plane,transform)
    vtkLookupTable PathPlanning(table)
    vtkTexture PathPlanning(plane,texture)
    vtkPlaneSource PathPlanning(plane,source)
    vtkTextureMapToPlane PathPlanning(plane,textureMapper)
    vtkDataSetMapper PathPlanning(plane,mapper)
    vtkActor PathPlanning(plane,actor)

    vtkTriangleFilter PathPlanning(triangleFilter) 
    # labelmap 0=outside, 1=boundary, 2=inside

    vtkDataSetToLabelMap PathPlanning(labelMapFilter)
   
    vtkImageEuclideanDistance PathPlanning(dist)
    
    vtkImageDijkstra PathPlanning(dijkstra)

    vtkTransformPolyDataFilter PathPlanning(shrinkFilter)
    # the coolest actor of them all!
    vtkScalarBarActor scalarBar
}


###########################################################
#                                                         # 
#                  SET PARAMETERS                         #
#                                                         #
###########################################################


#-------------------------------------------------------------------------------
# .PROC PathPlanningSetPlaneOffset
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PathPlanningSetPlaneOffset {{arg ""}} {
    global PathPlanning Endoscopic
    
    if { $arg != ""} {
        set Endoscopic(planeoffset) $arg
    }
    vtkTransform tmp
    tmp Translate 0 0 $Endoscopic(planeoffset)
    tmp GetMatrix PathPlanning(plane,matrix)
    Render3D
    tmp Delete
    
}

#-------------------------------------------------------------------------------
# .PROC PathPlanningSetVoxelSize
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PathPlanningSetVoxelSize {} {
    global PathPlanning
}


#-------------------------------------------------------------------------------
# .PROC PathPlanningSetMaximumDistance
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PathPlanningSetMaximumDistance {} {
    global PathPlanning
}

#-------------------------------------------------------------------------------
# .PROC PathPlanningSetSource
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PathPlanningSetSource {fid pid} {
    global PathPlanning Fiducials

    set PathPlanning(source,coord) [FiducialsGetPointCoordinates $pid]
    #later find closest point in the inside of the volume to 

}

#-------------------------------------------------------------------------------
# .PROC PathPlanningSetSink
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PathPlanningSetSink {fid pid} {
    global PathPlanning Fiducials
    
    set PathPlanning(sink,coord) [FiducialsGetPointCoordinates $pid]
    #later find closest point in the inside of the volume to 
    
}


###########################################################
#                                                         # 
#   LABELMAP AND DISTANCE MAP VIZUALISATION               #
#                                                         #
###########################################################

#-------------------------------------------------------------------------------
# .PROC PathPlanningVisualizeMaps
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PathPlanningVisualizeMaps {which} {

global PathPlanning

    
    # get the parameters
    #PathPlanning($which) Update
    set origin [[PathPlanning($which) GetOutput] GetOrigin]

    set origin0 [lindex $origin 0]
    set origin1 [lindex $origin 1]
    set origin2 [lindex $origin 2]
    
    set dimensions [[PathPlanning($which) GetOutput] GetDimensions]
    set dim0 [lindex $dimensions 0]
    set dim1 [lindex $dimensions 1]
    set dim2 [lindex $dimensions 2]
    
    set r_ext0 [expr ($dim0 - 1) * $PathPlanning(voxelSize)]
    set r_ext1 [expr ($dim1 - 1) * $PathPlanning(voxelSize)]
    set r_ext2 [expr ($dim2 - 1) * $PathPlanning(voxelSize)]
    
    
    # draw the planes
    # transform shared by reslice filter and texture mapped plane actor
    
    PathPlanning(plane,transform) SetInput PathPlanning(plane,matrix)
    
    #slice extraction filter
    vtkImageReslice PathPlanning(reslice)    
    PathPlanning(reslice) SetInput [PathPlanning($which) GetOutput]
    PathPlanning(reslice) SetResliceTransform PathPlanning(plane,transform)
    PathPlanning(reslice) InterpolateOff
    PathPlanning(reslice) SetOutputSpacing $PathPlanning(voxelSize) $PathPlanning(voxelSize) $PathPlanning(voxelSize)
    PathPlanning(reslice) SetOutputOrigin [expr $origin0] [expr $origin1] $origin2 
    PathPlanning(reslice) SetOutputExtent 0 $dim0 0 $dim1 0 0
    PathPlanning(reslice) Update
    
    #set the table range to match the scalar range
    
    set range [[PathPlanning($which) GetOutput] GetScalarRange]
    #puts "range $range input [PathPlanning($which) GetOutput] "
    PathPlanning(table) SetTableRange [lindex $range 0] [lindex $range 1]
    PathPlanning(table) Build
    
    scalarBar SetLookupTable PathPlanning(table)
    [scalarBar GetPositionCoordinate] SetCoordinateSystemToNormalizedViewport
    [scalarBar GetPositionCoordinate] SetValue 0.1 0.01
    scalarBar SetOrientationToHorizontal
    scalarBar SetWidth 0.8
    scalarBar SetHeight 0.17
    viewRen AddActor scalarBar
    
    # texture from reslice filter
    PathPlanning(plane,texture) SetInput [PathPlanning(reslice) GetOutput]
    PathPlanning(plane,texture) SetLookupTable PathPlanning(table)
    PathPlanning(plane,texture) InterpolateOn
    
    # need a plane to texture map onto
    PathPlanning(plane,source) SetXResolution 1
    PathPlanning(plane,source) SetYResolution 1
    PathPlanning(plane,source) SetOrigin [expr $origin0] [expr $origin1] [expr $origin2]
    PathPlanning(plane,source) SetPoint1 [expr $origin0 + $r_ext0] [expr $origin1 ] [expr $origin2]
    PathPlanning(plane,source) SetPoint2 [expr $origin0] [expr $origin1 + $r_ext1] [expr $origin2]
    
    # generate texture coordinates
    PathPlanning(plane,textureMapper) SetInput [PathPlanning(plane,source) GetOutput]
    
    # mapper for the textured plane
    PathPlanning(plane,mapper) SetInput [PathPlanning(plane,textureMapper) GetOutput]
    
    # put everything together (note that the same transform
    # is used for slice extraction and actor positioning)    
    PathPlanning(plane,actor) SetMapper PathPlanning(plane,mapper)
    PathPlanning(plane,actor) SetTexture PathPlanning(plane,texture)
    PathPlanning(plane,actor) SetUserMatrix PathPlanning(plane,matrix)
    PathPlanning(plane,actor) SetOrigin 0.0 0.0 0.0
    viewRen AddActor PathPlanning(plane,actor)
    PathPlanning(reslice) Delete
}


############################################################################
#s
#       Central Path Planning algorithm
#
############################################################################


# not used anymore
proc PathPlanningShrink {arg} {
    
    global Model PathPlanning
    set m $Model(activeID)
    
    if {$m != ""} {
    
        # scale
        set scale $arg
        vtkTransform shrinkTransform
        shrinkTransform Scale $scale $scale $scale
        
        PathPlanning(shrinkFilter) SetInput $Model($m,polyData) 
        PathPlanning(shrinkFilter) SetTransform shrinkTransform
        Model($m,mapper,viewRen) SetInput [PathPlanning(shrinkFilter) GetOutput]
        Render3D
        shrinkTransform Delete
        set PathPlanning(shrink) 1
    }
}


#-------------------------------------------------------------------------------
# .PROC PathPlanningExtractCenterline
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PathPlanningExtractCenterline {} {
    global Model PathPlanning Fiducials

    set PathPlanning(shrink) 0
    # set the steps 
    set voxelize 1
    set dist 1
    set io 0
    set dijkstra 1
    #puts "do dijkstra: $dijkstra"

    set m $Model(activeID)
    
    if {$m != ""} {
        if {$PathPlanning(shrink) } {
            puts "setting shrunk data"
            PathPlanning(triangleFilter) SetInput [PathPlanning(shrinkFilter) GetOutput]
        } else {
            PathPlanning(triangleFilter) SetInput $Model($m,polyData) 
        }
        
        PathPlanning(triangleFilter) Update
        
        
        
        ###########################################################
        #                                                         # 
        #                    VOXELIZE                             #
        #                                                         #
        ###########################################################
        
        if { $voxelize} {

            PathPlanning(labelMapFilter) Delete
            vtkDataSetToLabelMap PathPlanning(labelMapFilter)
            #PathPlanning(labelMapFilter) SetStartMethod       MainStartProgress
            #PathPlanning(labelMapFilter) SetProgressMethod   "MainShowProgress PathPlanning(labelMapFilter)"
            #PathPlanning(labelMapFilter) SetEndMethod         MainEndProgress
            PathPlanning(labelMapFilter) SetUseBoundaryVoxels 1
            PathPlanning(labelMapFilter) SetInput [PathPlanning(triangleFilter) GetOutput]
            PathPlanning(labelMapFilter) SetOutputSpacing $PathPlanning(voxelSize) $PathPlanning(voxelSize) $PathPlanning(voxelSize)
            PathPlanning(labelMapFilter) Update
           
            #PathPlanningMakeMrmlVolume PathPlanning(labelMapFilter)
            
        }
        
            
        ###########################################################
        #                                                         # 
        #                    DISTANCE TRANSFORM                   #
        #                                                         #
        ###########################################################

        if {$dist == 1} {
            # do the distance transform
            
            # we  have to create a new DT everytime otherwise it doesn't
            # get updated right when we try to switch its input (it keeps a 
            # record of the old input Whole extent and complains when the new
            # input's extents don't match that)

            if { $m != $PathPlanning(lastActiveModel)} {
                PathPlanning(dist) Delete
                vtkImageEuclideanDistance PathPlanning(dist)
            }
            if {$voxelize} {
                PathPlanning(dist) SetInput [PathPlanning(labelMapFilter) GetOutput]
            } else {
                PathPlanning(dist) SetInput [Volume(1,vol) GetOutput]
            }

        #PathPlanning(dist) SetStartMethod       MainStartProgress
            #PathPlanning(dist) SetProgressMethod   "MainShowProgress PathPlanning(dist)"
            #PathPlanning(dist) SetEndMethod         MainEndProgress
            PathPlanning(dist) SetMaximumDistance $PathPlanning(dist,maxDistance)
            PathPlanning(dist) SetInitialize 1
            PathPlanning(dist) SetConsiderAnisotropy 0
            #PathPlanning(dist) Update
        }
        
        #PathPlanningMakeMrmlVolume PathPlanning(dist)


        ###########################################################
        #                                                         # 
        #                    I/O STUFF (save Vol/Dist)            #
        #                                                         #
        ###########################################################

        if {$io == 1} {
            
            vtkImageWriter PathPlanning(iwriter)    
            puts "writing volume [Model($m,node) GetName]Vol.vtk"
            PathPlanning(iwriter) SetFileDimensionality 3
            PathPlanning(iwriter) SetFilePrefix [Model($m,node) GetName]Vol
            PathPlanning(iwriter) SetInput [PathPlanning(labelMapFilter) GetOutput]
            PathPlanning(iwriter) Write

            vtkImageWriter PathPlanning(iwriter2)    
            puts "writing volume [Model($m,node) GetName]Dist.vtk"
            PathPlanning(iwriter2) SetFileDimensionality 3
            PathPlanning(iwriter2) SetFilePrefix [Model($m,node) GetName]Dist
            PathPlanning(iwriter2) SetInput [PathPlanning(dist) GetOutput]
            PathPlanning(iwriter2) Write
          
            PathPlanning(iwriter) Delete
            PathPlanning(iwriter2) Delete
        }
        
        
        ###########################################################
        #                                                         # 
        #                   DIJKSTRA                              #
        #                                                         #
        ###########################################################
        

        if {$dijkstra == 1} {

            # delete it so the filter re-executes no matter what

            PathPlanning(dijkstra) Delete
            vtkImageDijkstra PathPlanning(dijkstra)
            
        # DO NOT UNCOMMENT UNTIL vtkImageDijkstra has appropriate
        # UpdateProgress calls...

        #PathPlanning(dijkstra) SetStartMethod       MainStartProgress
            #PathPlanning(dijkstra) SetProgressMethod   "MainShowProgress PathPlanning(dijkstra)"
            #PathPlanning(dijkstra) SetEndMethod         MainEndProgress

            PathPlanning(dijkstra) SetUseInverseExponentialDistance $PathPlanning(useExponential) 
            PathPlanning(dijkstra) SetUseInverseSquaredDistance $PathPlanning(useSquared) 
            PathPlanning(dijkstra) SetUseInverseDistance $PathPlanning(useLinear) 
            # try Dijkstra's
            # find IDs (closest point to coords)
            PathPlanning(dist) Update
            if {$voxelize} {
            
                set PathPlanning(source) [[PathPlanning(dist) GetOutput] FindPoint [lindex $PathPlanning(source,coord) 0] [lindex $PathPlanning(source,coord) 1] [lindex $PathPlanning(source,coord) 2]]
                set PathPlanning(sink) [[PathPlanning(dist) GetOutput] FindPoint [lindex $PathPlanning(sink,coord) 0] [lindex $PathPlanning(sink,coord) 1] [lindex $PathPlanning(sink,coord) 2]]
            
            } else {
                set dim [[PathPlanning(dist) GetOutput] GetDimensions]
                set sourceIJK [[Volume(1,node) GetRasToIjk] MultiplyPoint [lindex $PathPlanning(source,coord) 0] [lindex $PathPlanning(source,coord) 1] [lindex $PathPlanning(source,coord) 2] 1]
                set PathPlanning(source) [expr int([lindex $sourceIJK 0] + ([lindex $sourceIJK 1] * [lindex $dim 0]) + ([lindex $sourceIJK 2] * [lindex $dim 0] * [lindex $dim 1]))]
                puts "source ID $PathPlanning(source)"
                set sinkIJK [[Volume(1,node) GetRasToIjk] MultiplyPoint [lindex $PathPlanning(sink,coord) 0] [lindex $PathPlanning(sink,coord) 1] [lindex $PathPlanning(sink,coord) 2] 1]
                set PathPlanning(sink) [expr int([lindex $sinkIJK 0] + ([lindex $sinkIJK 1] * [lindex $dim 0]) + ([lindex $sinkIJK 2] * [lindex $dim 0] * [lindex $dim 1]))]

                puts "sink ID $PathPlanning(sink)"
            }  
            
            
            if { $PathPlanning(source) == -1 } {
                tk_messageBox -message "The source fiducial is not in the model selected"
            } elseif { $PathPlanning(sink) == -1 } {
                tk_messageBox -message "The sink fiducial is not in the model selected"
            } else {

                PathPlanning(dijkstra) SetSourceID $PathPlanning(source)
                PathPlanning(dijkstra) SetSinkID $PathPlanning(sink)
                PathPlanning(dijkstra) SetBoundaryScalars [PathPlanning(labelMapFilter) GetBoundaryScalars]

                PathPlanning(dijkstra) SetInput [PathPlanning(dist) GetOutput]
                PathPlanning(dijkstra) Update
                PathPlanning(dijkstra) InitTraversePath
                
                if {[PathPlanning(dijkstra) GetNumberOfPathNodes] > 1} {
                    set list [EndoscopicGetAvailableListName [Model($m,node) GetName]]
                    set id [FiducialsCreateFiducialsList "endoscopic" $list]
                    #puts "Fiducials($id,node) SetTextSize 0.0"
                    Fiducials($id,node) SetTextSize 0.0
                    MainUpdateMRML

                    set node [PathPlanning(dijkstra) GetNextPathNode]
                    set coord [[PathPlanning(dist) GetOutput] GetPoint $node]
                    set next_node [PathPlanning(dijkstra) GetNextPathNode]
                    set next_coord [[PathPlanning(dist) GetOutput] GetPoint $next_node]
                    
                    while { $next_node != -1} {
                        EndoscopicAddLandmarkDirectionSpecified "[lindex $coord 0] [lindex $coord 1] [lindex $coord 2] [lindex $next_coord 0] [lindex $next_coord 1] [lindex $next_coord 2]" $list 
                        
                        set node $next_node
                        set coord $next_coord
                        # sampling for smoothing
                        for {set j 0} {$j < 5} {incr j} {
                            set next_node [PathPlanning(dijkstra) GetNextPathNode]
                        }
                        set next_coord [[PathPlanning(dist) GetOutput] GetPoint $next_node]
                    }
                    # add the last point 
                    EndoscopicAddLandmarkNoDirectionSpecified [lindex $coord 0] [lindex $coord 1] [lindex $coord 2] $list 
                    MainUpdateMRML
                    Render3D
                }
            }
        }
    }
    set PathPlanning(lastActiveModel) $m
}


#-------------------------------------------------------------------------------
# .PROC PathPlanningMakeMrmlVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PathPlanningMakeMrmlVolume {source} {

    # Create the vtkMrmlVolumeNode
    set newvol [MainMrmlAddNode Volume]

    # Delphine: we need to set up the matrices in the node somehow
    set origin [[$source GetOutput] GetOrigin]
    set dim [[$source GetOutput] GetDimensions]
    
    
    # Set the Description and Name
    #$newvol SetDescription "PathPlanning volume"
    #$newvol SetName "PathPlanning"
    #set result [$newvol ComputeRasToIjkFromCorners \
    #    0 0 0 \
    #    [lindex $origin 0] [lindex $origin 1] [lindex $origin 2] \
    #    [lindex $dim 0] [lindex $origin 1] [lindex $origin 2] \
    #    [lindex $dim 0] [lindex $dim 1] [lindex $origin 2] \
    #    0 0 0 \
    #    [lindex $origin 0] [lindex $origin 1] [lindex $dim 2]]

    #puts "$result ras [$newvol GetRasToIjkMatrix] Print"
    # Create the vtkMrmlDataVolume
    

    set v [$newvol GetID]
    MainVolumesCreate $v

    # not for distance vols
    $newvol SetLabelMap    1
    Volume($v,vol) UseLabelIndirectLUTOn

    #return $n

    #puts "created volume $v"

    # debug
    #puts [[$source GetOutput] Print]

    Volume($v,vol) SetImageData [$source GetOutput]
    
    # This updates all the buttons to say that the
    # Volume List has changed.
    MainUpdateMRML

    # update slicer stuff
    MainVolumesUpdate $v

    # display this volume so the user knows something happened
    MainSlicesSetVolumeAll Back $v
    RenderAll    
}
