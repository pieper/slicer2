
#-------------------------------------------------------------------------------
# .PROC EndoscopicEnter
# Called when this module is entered by the user.  

# effects: Pushes the event manager for this module and 
#          calls EndoscopicAddEndoscopicView. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicEnter {} {
    global Endoscopic View viewWin
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)

    # push initial bindings
    EndoscopicPushBindings
    
    # show the actors based on their visibility parameter
    foreach a $Endoscopic(actors) {
    EndoscopicUpdateVisibility $a
    }
    Render3D
    if {$Endoscopic(endoview,visibility) == 1} {
    EndoscopicAddEndoscopicView
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicExit
# Called when this module is exited by the user.  
#
# effects: Pops the event manager for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicExit {} {
    global Endoscopic
    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    if {$Endoscopic(endoview,hideOnExit) == 1} {
    # reset the slice driver
    EndoscopicSetSliceDriver User
    
    # set all endoscopic actors to be invisible, without changing their 
    # visibility parameters
    foreach a $Endoscopic(actors) {
        Endoscopic($a,actor) SetVisibility 0
        EndoscopicSetPickable $a 0
            
    }
    Render3D
    EndoscopicRemoveEndoscopicView
    }
    EndoscopicPopBindings
}




#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateEndoscopicViewVisibility
#  Makes the endoscopic view appear or disappear based on the variable Endoscopic(mainview,visibility) [note: there is a check to make sure that the endoscopic view cannot disappear if the main view is not visible)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateEndoscopicViewVisibility {} {
    global View viewWin Gui Endoscopic

    if {$Endoscopic(endoview,visibility) == 1 && $Endoscopic(mainview,visibility) == 1} {
    EndoscopicAddEndoscopicView
    } elseif {$Endoscopic(endoview,visibility) == 0 && $Endoscopic(mainview,visibility) == 1} {
    EndoscopicRemoveEndoscopicView
    } elseif {$Endoscopic(endoview,visibility) == 1 && $Endoscopic(mainview,visibility) == 0} {
    EndoscopicAddEndoscopicView
    EndoscopicRemoveMainView
    }
    Render3D
    # for the rest do nothing
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateMainViewVisibility
# Makes the main view appear or disappear based on the variable Endoscopic(mainview,visibility) [note: there is a check to make sure that the main view cannot disappear if the endoscopic view is not visible)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateMainViewVisibility {} {
    global View viewWin Gui Endoscopic

    if {$Endoscopic(mainview,visibility) == 1 && $Endoscopic(endoview,visibility) == 1} {
    EndoscopicAddMainView
    } elseif {$Endoscopic(mainview,visibility) == 0 && $Endoscopic(endoview,visibility) == 1} {
    EndoscopicRemoveMainView
     } elseif {$Endoscopic(mainview,visibility) == 1 && $Endoscopic(endoview,visibility) == 0} {
     EndoscopicAddMainView
     EndoscopicRemoveEndoscopicView
     }
    Render3D
    # for the rest do nothing
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicAddEndoscopicView
#  Add the endoscopic renderer to the right of the main view 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddEndoscopicView {} {
    global View viewWin Gui Endoscopic

    if {$Endoscopic(viewOn) == 0} {

    EndoscopicSetSliceDriver User
    # set the endoscopic actors' visibility according to their prior visibility
        
    EndoscopicUpdateVirtualEndoscope
        $viewWin AddRenderer endoscopicRen    
    viewRen SetViewport 0 0 .5 1
        endoscopicRen SetViewport .5 0 1 1
        MainViewerSetSecondViewOn
        MainViewerSetMode $View(mode) 
    set Endoscopic(viewOn) 1
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicAddMainView
#  Add the main view to the left of the endoscopic view
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddMainView {} {
    global View viewWin Gui Endoscopic

    $viewWin AddRenderer viewRen    
    viewRen SetViewport 0 0 .5 1
    endoscopicRen SetViewport .5 0 1 1
    MainViewerSetSecondViewOn
    set Endoscopic(viewOn) 1
    MainViewerSetMode $View(mode) 
    
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicAddEndoscopicViewRemoveMainView
#  Makes the main view invisible/the endoscopic view visible
#  (so switch between the 2 views)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddEndoscopicViewRemoveMainView {} {
    global View viewWin Gui Endoscopic
    
    if {$Endoscopic(viewOn) == 0} {

    EndoscopicSetSliceDriver Camera
    # set the endoscopic actors' visibility according to their prior visibility
    
    foreach a $Endoscopic(actors) {
        EndoscopicUpdateVisibility $a
    }
    Render3D
    EndoscopicUpdateVirtualEndoscope
        $viewWin AddRenderer endoscopicRen
    $viewWin RemoveRenderer viewRen
        endoscopicRen SetViewport 0 0 1 1
        MainViewerSetSecondViewOn
    set Endoscopic(viewOn) 1
        MainViewerSetMode $View(mode) 
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicRemoveEndoscopicView
#  Remove the endoscopic view
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicRemoveEndoscopicView {} {
    global Endoscopic View viewWin Gui

    if { $Endoscopic(viewOn) == 1} {


        $viewWin RemoveRenderer endoscopicRen    
    viewRen SetViewport 0 0 1 1
        MainViewerSetSecondViewOff
    set Endoscopic(viewOn) 0
        MainViewerSetMode $View(mode) 
    }
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicRemoveMainView
#  Remove the main view
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicRemoveMainView {} {
    global Endoscopic View viewWin viewRen endoscopicRen
    
    $viewWin RemoveRenderer viewRen    
    endoscopicRen SetViewport 0 0 1 1
    MainViewerSetSecondViewOn
    set Endoscopic(viewOn) 1
    MainViewerSetMode $View(mode) 

}

#-------------------------------------------------------------------------------
# .PROC EndoscopicAddMainViewRemoveEndoscopicView
#  Makes the main view visible/the endoscopic view invisible
#  (so switch between the 2 views)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddMainViewRemoveEndoscopicView {} {
    global Endoscopic View viewWin Gui

    if { $Endoscopic(viewOn) == 1} {

    # reset the slice driver
    EndoscopicSetSliceDriver User

    # set all endoscopic actors to be invisible, without changing their visibility 
    # parameters
    foreach a $Endoscopic(actors) {
        Endoscopic($a,actor) SetVisibility 0
        EndoscopicSetPickable $a 0
            
    }
    Render3D
    $viewWin AddRenderer viewRen
        $viewWin RemoveRenderer endoscopicRen    
    viewRen SetViewport 0 0 1 1
        MainViewerSetSecondViewOff
    set Endoscopic(viewOn) 0
        MainViewerSetMode $View(mode) 
    }
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicInit
#  The "Init" procedure is called automatically by the slicer.  
#  <br>
#
#  effects: * It puts information about the module into a global array called 
#             Module. <br>
#           * It adds a renderer called View(endCam) to the global array 
#             View and adds the new renderer to the list of renderers in 
#             Module(renderers) <br> 
#           * It also initializes module-level variables (in the global array 
#             Endoscopic and Path) <br>
#             The global array Endoscopic contains information about the 
#             7 actors created in this module (the list is saved in Endoscopic(actors): <br>
#        regular actors: <br>
#               cam: the endoscopic camera <br>
#               fp:  the focal point of cam <br>
#               gyro: the csys to move the cam actor
#        3D glyphs:       <br>
#               cPath: the camera path <br>
#               cLand: the landmarks on the camera Path <br>
#               vector: the view vector at every point on the spline <br>
#               fPath: the focal point path <br>
#               fLand: the landmarks on the focal point Path <br>
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicInit {} {
    global Endoscopic Module Model Path Advanced View Gui 
    

    set m Endoscopic
    set Module($m,row1List) "Help Display Camera Path"
    set Module($m,row1Name) "{Help} {Display} {Camera} {Path}"
    set Module($m,row1,tab) Camera    
    set Module($m,row2List) "Advanced"
    set Module($m,row2Name) "{Advanced}"
    set Module($m,row2,tab) Advanced

    set Module($m,depend) ""
    
    # Define Procedures
    #------------------------------------
    
    set Module($m,procVTK) EndoscopicBuildVTK
    set Module($m,procGUI) EndoscopicBuildGUI
    set Module($m,procCameraMotion) EndoscopicCameraMotionFromUser
    set Module($m,procXformMotion) EndoscopicGyroMotion
    set Module($m,procEnter) EndoscopicEnter
    set Module($m,procExit) EndoscopicExit
    set Module($m,procMRML) EndoscopicUpdateMRML
    
    # create a second renderer here in Init so that it is added to the list 
    # of Renderers before MainActorAdd is called anywhere
    # That way any actor added to viewRen (the MainView's renderer) is also 
    # added to endoscopicRen and will appear on the second window once we decide 
    # to show it
    
    vtkRenderer endoscopicRen
    lappend Module(Renderers) endoscopicRen    
    eval endoscopicRen SetBackground $View(bgColor)

    vtkLight endLight
    vtkLight endLight2
    endoscopicRen AddLight endLight
    endoscopicRen AddLight endLight2
    set View(endCam) [endoscopicRen GetActiveCamera]
    # initial settings. 
    # These parameters are then set in EndoscopicUpdateVirtualEndoscope
    $View(endCam) SetPosition 0 0 0
    $View(endCam) SetFocalPoint 0 30 0
    $View(endCam) SetViewUp 0 0 1
    $View(endCam) ComputeViewPlaneNormal        
    set View(endoscopicClippingRange) ".01 1000"
    eval $View(endCam) SetClippingRange $View(endoscopicClippingRange)
    
    ### create bindings
    EndoscopicCreateBindings
    
    # Initialize module-level variables
    #------------------------------------
    
    set Endoscopic(count) 0
    set Endoscopic(eventManager)  ""
    set Endoscopic(actors) ""

    # Camera variables
    # don't change these default values, change Endoscopic(cam,size) instead
    
    set Endoscopic(cam,name) "Camera"
    set Endoscopic(cam,visibility) 1    
    set Endoscopic(cam,size) 1
    set Endoscopic(cam,boxlength) 30
    set Endoscopic(cam,boxheight) 20
    set Endoscopic(cam,boxwidth)  10
    set Endoscopic(cam,x) 0
    set Endoscopic(cam,y) 0
    set Endoscopic(cam,z) 0
    set Endoscopic(cam,xRotation) 0
    set Endoscopic(cam,yRotation) 0
    set Endoscopic(cam,zRotation) 0
    set Endoscopic(cam,FPdistance) 30
    set Endoscopic(cam,viewAngle) 90
    set Endoscopic(cam,AngleStr) 90
    set Endoscopic(cam,viewUpX) 0 
    set Endoscopic(cam,viewUpY) 0 
    set Endoscopic(cam,viewUpZ) 1
    set Endoscopic(cam,viewPlaneNormalX) 0
    set Endoscopic(cam,viewPlaneNormalY) 1
    set Endoscopic(cam,viewPlaneNormalZ) 0
    set Endoscopic(cam,driver) 0
    set Endoscopic(cam,PathNode) ""
    set Endoscopic(cam,EndPathNode) ""
    set Endoscopic(cam,xStr) 0
    set Endoscopic(cam,yStr) 0
    set Endoscopic(cam,zStr) 0
    set Endoscopic(cam,xStr,old) 0
    set Endoscopic(cam,yStr,old) 0
    set Endoscopic(cam,zStr,old) 0
    set Endoscopic(cam,rxStr) 0
    set Endoscopic(cam,ryStr) 0
    set Endoscopic(cam,rzStr) 0
    set Endoscopic(cam,rxStr,old) 0
    set Endoscopic(cam,ryStr,old) 0
    set Endoscopic(cam,rzStr,old) 0

    set Endoscopic(sliderx) ""
    set Endoscopic(slidery) ""
    set Endoscopic(sliderz) ""
    set Endoscopic(Box,name) "Camera Box"
    set Endoscopic(Box,color) "1 .4 .5" 

    set Endoscopic(Lens,name) "Camera Lens"
    set Endoscopic(Lens,color) ".4 .2 .6" 

    set Endoscopic(fp,name) "Focal Point"    
    set Endoscopic(fp,visibility) 1    
    set Endoscopic(fp,size) 4
    set Endoscopic(fp,color) ".2 .6 .8"
    set Endoscopic(fp,x) 0
    set Endoscopic(fp,y) 30
    set Endoscopic(fp,z) 0
    set Endoscopic(fp,driver) 0

    set Endoscopic(intersection,driver) 0    
    set Endoscopic(intersection,x) 0    
    set Endoscopic(intersection,y) 0    
    set Endoscopic(intersection,z) 0    
    # if it is absolute, the camera will move along the 
    #  RA/IS/LR axis
    # if it is relative, the camera will move along its
    #  own axis 
    set Endoscopic(cam,axis) relative
    
    # Endoscopic variables
    

    set Endoscopic(cPath,name) "Camera Path"
    set Endoscopic(cPath,size) .5 
    set Endoscopic(cPath,visibility) 1    
    set Endoscopic(cPath,color) ".4 .2 .6" 

    set Endoscopic(cLand,name) "Camera Landmarks"
    set Endoscopic(cLand,size) 1 
    set Endoscopic(cLand,visibility) 1    
    set Endoscopic(cLand,color) ".4 .2 .6"
    set Endoscopic(cLand,selectedID) 0

    set Endoscopic(fPath,name) "Focal Path"
    set Endoscopic(fPath,size) .45 
    set Endoscopic(fPath,visibility) 0    
    set Endoscopic(fPath,color) ".2 .6 .8"

    set Endoscopic(fLand,name) "Focal Landmarks"
    set Endoscopic(fLand,size) 1 
    set Endoscopic(fLand,visibility) 0    
    set Endoscopic(fLand,color) ".2 .6 .8"
    

    set Endoscopic(vector,name) "Vectors"
    set Endoscopic(vector,size) 5 
    set Endoscopic(vector,visibility) 1    
    set Endoscopic(vector,color) ".2 .6 .8"
    set Endoscopic(vector,selectedID) 0

    set Endoscopic(gyro,name) "3D Gyro"
    set Endoscopic(gyro,size) 100 
    set Endoscopic(gyro,visibility) 1
    set Endoscopic(gyro,Xactor,color) "1.0 0.4 0.4"
    set Endoscopic(gyro,Yactor,color) "0.4 1.0 0.4"
    set Endoscopic(gyro,Zactor,color) "0.4 0.4 1.0"
    set Endoscopic(gyro,Xactor,selectedColor) "1.0 0 0"
    set Endoscopic(gyro,Yactor,selectedColor) "0 1.0 0"
    set Endoscopic(gyro,Zactor,selectedColor) "0 0 1"
    set Endoscopic(gyro,use) 1
    
    #Advanced variables
    set Endoscopic(ModelsVisibilityInside) 1
    set Endoscopic(SlicesVisibility) 1
    set Endoscopic(collision) 0
    set Endoscopic(collDistLabel) ""
    set Endoscopic(collMenu) ""

    # Path variable
    set Endoscopic(path,flyDirection) "Forward"
    set Endoscopic(path,speed) 1
    set Endoscopic(path,random) 0
    set Endoscopic(path,first) 1
    set Endoscopic(path,i) 0
    set Endoscopic(path,stepStr) 0
    set Endoscopic(path,exists) 0
    set Endoscopic(path,numLandmarks) 0
    set Endoscopic(path,stop) 0
    set Endoscopic(path,vtkNodeRead) 0
    
    set Endoscopic(path,interpolationStr) 1
    # set Colors
    set Endoscopic(path,cColor) [MakeColor "0 204 255"]
    set Endoscopic(path,sColor) [MakeColor "204 255 255"]
    set Endoscopic(path,eColor) [MakeColor "255 204 204"]
    set Endoscopic(path,rColor) [MakeColor "204 153 153"]
    
    set LastX 0
    
     # viewers 
    set Endoscopic(mainview,visibility) 1
    set Endoscopic(endoview,visibility) 1
    set Endoscopic(endoview,hideOnExit) 0
    set Endoscopic(viewOn) 0
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicBuildVTK
#  Creates the vtk objects for this module
#  
#  effects: calls EndoscopicCreateFocalPoint, 
#           EndoscopicCreateCamera, EndoscopicCreateLandmarks and 
#           EndoscopicCreatePath, EndoscopicCreateGyro, EndoscopicCreateVector   
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicBuildVTK {} {
    global Endoscopic Model Csys
    

    vtkCellPicker Endoscopic(picker)
    Endoscopic(picker) SetTolerance 0.001

    # create the 3D mouse
    CsysCreate Endoscopic gyro -1 -1 -1
    lappend Endoscopic(actors) gyro 
    
    # create the focal point actor
    EndoscopicCreateFocalPoint
    lappend Endoscopic(actors) fp 

    # create the camera actor (needs to be created after the gyro
    # since it uses the gyro's matrix as its user matrix
    EndoscopicCreateCamera
    lappend Endoscopic(actors) cam

    #update the virtual camera
    EndoscopicUpdateVirtualEndoscope
    
    # create the Landmarks actors
    EndoscopicCreateLandmarks
    lappend Endoscopic(actors) cLand
    lappend Endoscopic(actors) fLand

    #create the Path actors 
    EndoscopicCreatePath    
    lappend Endoscopic(actors) cPath
    lappend Endoscopic(actors) fPath

    #create the Vector actor
    EndoscopicCreateVector
    lappend Endoscopic(actors) vector

    # create the voxelisation actor
    vtkDataSetMapper Endoscopic(voxelModeller,mapper)
    vtkActor Endoscopic(voxelModeller,actor)

    # create the implicit modeller
    vtkImplicitModeller Endoscopic(implicitModeller)
    vtkContourFilter Endoscopic(implicitModeller,contourFilter)
    vtkPolyDataMapper Endoscopic(implicitModeller,mapper)
    vtkActor Endoscopic(implicitModeller,actor)

    # add the camera, fp, gyro actors only to viewRen, not endoscopicRen
    # set their visibility to 0 until we enter the module
    viewRen AddActor Endoscopic(cam,actor)
    Endoscopic(cam,actor) SetVisibility 0
    EndoscopicSetPickable cam 0
    viewRen AddActor Endoscopic(fp,actor)
    Endoscopic(fp,actor) SetVisibility 0
    EndoscopicSetPickable fp 0
    viewRen AddActor Endoscopic(gyro,actor)
    Endoscopic(gyro,actor) SetVisibility 0
    EndoscopicSetPickable gyro 0

    viewRen AddActor Endoscopic(cLand,actor)
    Endoscopic(cLand,actor) SetVisibility 0
    viewRen AddActor Endoscopic(fLand,actor)
    Endoscopic(fLand,actor) SetVisibility 0
    viewRen AddActor Endoscopic(cPath,actor)
    Endoscopic(cPath,actor) SetVisibility 0
    viewRen AddActor Endoscopic(fPath,actor)
    Endoscopic(fPath,actor) SetVisibility 0
    viewRen AddActor Endoscopic(vector,actor)
    Endoscopic(vector,actor) SetVisibility 0

    viewRen AddActor Endoscopic(voxelModeller,actor)
    viewRen AddActor Endoscopic(implicitModeller,actor)

    vtkMath Endoscopic(path,vtkmath)
    
    
}

#############################################################################
#
#     PART 1: create vtk actors and parameters 
#
#############################################################################


#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateCamera
#  Create the Camera vtk actor
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicCreateCamera {} {
    global Endoscopic
    
    vtkCubeSource camCube
    vtkTransform CubeXform        
    vtkCubeSource camCube2
    vtkTransform CubeXform2        
    vtkCubeSource camCube3
    vtkTransform CubeXform3        
    vtkCylinderSource camCyl
    vtkTransform CylXform        
    vtkCylinderSource camCyl2
    vtkTransform CylXform2        
    
    EndoscopicCameraParams
    
    # make the actor (camera)
    
    vtkTransformPolyDataFilter CylXformFilter
    CylXformFilter SetInput [camCyl GetOutput]
    CylXformFilter SetTransform CylXform
    
    vtkTransformPolyDataFilter CylXformFilter2
    CylXformFilter2 SetInput [camCyl2 GetOutput]
    CylXformFilter2 SetTransform CylXform2
    
    vtkTransformPolyDataFilter CubeXformFilter
    CubeXformFilter SetInput [camCube GetOutput]
    CubeXformFilter SetTransform CubeXform
    
    vtkTransformPolyDataFilter CubeXformFilter2
    CubeXformFilter2 SetInput [camCube2 GetOutput]
    CubeXformFilter2 SetTransform CubeXform2
    
    vtkTransformPolyDataFilter CubeXformFilter3
    CubeXformFilter3 SetInput [camCube3 GetOutput]
    CubeXformFilter3 SetTransform CubeXform3
    
    vtkPolyDataMapper BoxMapper
    BoxMapper SetInput [CubeXformFilter GetOutput]
    
    vtkPolyDataMapper BoxMapper2
    BoxMapper2 SetInput [CubeXformFilter2 GetOutput]
    
    vtkPolyDataMapper BoxMapper3
    BoxMapper3 SetInput [CubeXformFilter3 GetOutput]
    
    vtkPolyDataMapper LensMapper
    LensMapper SetInput [CylXformFilter GetOutput]
    
    vtkPolyDataMapper LensMapper2
    LensMapper2 SetInput [CylXformFilter2 GetOutput]
    
    vtkActor Endoscopic(Box,actor)
    Endoscopic(Box,actor) SetMapper BoxMapper
    eval [Endoscopic(Box,actor) GetProperty] SetColor $Endoscopic(Box,color)
    Endoscopic(Box,actor) PickableOff
    
    vtkActor Endoscopic(Box2,actor)
    Endoscopic(Box2,actor) SetMapper BoxMapper2
    eval [Endoscopic(Box2,actor) GetProperty] SetColor $Endoscopic(Box,color) 
    Endoscopic(Box2,actor) PickableOff
    
    vtkActor Endoscopic(Box3,actor)
    Endoscopic(Box3,actor) SetMapper BoxMapper3
    eval [Endoscopic(Box3,actor) GetProperty] SetColor 0 0 0 
    Endoscopic(Box3,actor) PickableOff
    
    vtkActor Endoscopic(Lens,actor)
    Endoscopic(Lens,actor) SetMapper LensMapper
    eval [Endoscopic(Lens,actor) GetProperty] SetColor $Endoscopic(Lens,color) 
    Endoscopic(Lens,actor) PickableOff
    
    vtkActor Endoscopic(Lens2,actor)
    Endoscopic(Lens2,actor) SetMapper LensMapper2
    [Endoscopic(Lens2,actor) GetProperty] SetColor 0 0 0
    Endoscopic(Lens2,actor) PickableOff
    
    set Endoscopic(cam,actor) [vtkAssembly Endoscopic(cam,actor)]
    Endoscopic(cam,actor) AddPart Endoscopic(Box,actor)
    Endoscopic(cam,actor) AddPart Endoscopic(Box2,actor)
    Endoscopic(cam,actor) AddPart Endoscopic(Box3,actor)
    Endoscopic(cam,actor) AddPart Endoscopic(Lens,actor)
    Endoscopic(cam,actor) AddPart Endoscopic(Lens2,actor)
    Endoscopic(cam,actor) PickableOn

    # set the user matrix of the camera and focal point to be the matrix of
    # the gyro
    # the full matrix of the cam and fp is a concatenation of their matrix and
    # their user matrix
    Endoscopic(cam,actor) SetUserMatrix [Endoscopic(gyro,actor) GetMatrix]
    Endoscopic(fp,actor) SetUserMatrix [Endoscopic(gyro,actor) GetMatrix]
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicCameraParams
# effects: Set the size parameters for the camera and the focal point
# .ARGS int size (optional), 30 by default
# .END
#-------------------------------------------------------------------------------
proc EndoscopicCameraParams {{size -1}} {
    global Endoscopic
    
    if { $size == -1 } { 
    set Endoscopic(cam,size) 1
    } else {
    set Endoscopic(cam,size) $size
    }
    # set parameters for cube (body) geometry and transform
    set Endoscopic(cam,boxlength) [expr $Endoscopic(cam,size) * 30]
    set Endoscopic(cam,boxheight) [expr $Endoscopic(cam,size) * 20]
    set Endoscopic(cam,boxwidth)  [expr $Endoscopic(cam,size) * 10]
    
    set unit [expr $Endoscopic(cam,size)]
    
    camCube SetXLength $Endoscopic(cam,boxlength)
    camCube SetYLength $Endoscopic(cam,boxwidth)
    camCube SetZLength $Endoscopic(cam,boxheight)
    
    CubeXform Identity
    #CubeXform RotateZ 90
    
    camCube2 SetXLength [expr $Endoscopic(cam,boxlength) /3] 
    camCube2 SetYLength $Endoscopic(cam,boxwidth)
    camCube2 SetZLength [expr $Endoscopic(cam,boxheight) /3]
    
    CubeXform2 Identity
    CubeXform2 Translate 0 0 [expr {$Endoscopic(cam,boxheight) /2}]
    
    camCube3 SetXLength [expr $Endoscopic(cam,boxlength) /6] 
    camCube3 SetYLength [expr $Endoscopic(cam,boxwidth) +$unit]
    camCube3 SetZLength [expr $Endoscopic(cam,boxheight) /6]
    
    CubeXform3 Identity
    CubeXform3 Translate 0 0 [expr {$Endoscopic(cam,boxheight) /2} + $unit]
    
    # set parameters for cylinder (lens) geometry and transform
    camCyl SetRadius [expr $Endoscopic(cam,boxlength) / 3.5]
    camCyl SetHeight [expr $Endoscopic(cam,boxwidth) * 1.5]
    camCyl SetResolution 8
    
    CylXform Identity
    CylXform Translate 0 [expr $Endoscopic(cam,boxwidth) / 2] 0
    
    camCyl2 SetRadius [expr $Endoscopic(cam,boxlength) / 5]
    camCyl2 SetHeight [expr $Endoscopic(cam,boxwidth) * 1.5]
    camCyl2 SetResolution 8
    
    CylXform2 Identity
    CylXform2 Translate 0 [expr {$Endoscopic(cam,boxwidth) / 2} + $unit] 0
    
    #also, set the size of the focal Point actor
    set Endoscopic(fp,size) [expr $Endoscopic(cam,size) * 4]
    Endoscopic(fp,source) SetRadius $Endoscopic(fp,size)
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateFocalPoint
#  Create the vtk FocalPoint actor
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicCreateFocalPoint {} {
    
    global Endoscopic

    vtkSphereSource Endoscopic(fp,source)
    Endoscopic(fp,source) SetRadius $Endoscopic(fp,size)
    vtkPolyDataMapper Endoscopic(fp,mapper)
    Endoscopic(fp,mapper) SetInput [Endoscopic(fp,source) GetOutput]
    vtkActor Endoscopic(fp,actor)
    Endoscopic(fp,actor) SetMapper Endoscopic(fp,mapper)
    eval [Endoscopic(fp,actor) GetProperty] SetColor $Endoscopic(fp,color)
    set Endoscopic(fp,distance) [expr $Endoscopic(cam,boxwidth) *3] 
    Endoscopic(fp,actor) SetPosition 0 $Endoscopic(fp,distance) 0
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateLandmarks
#  Create the vtk camera Landmarks and focalPoint Landmarks actors
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicCreateLandmarks {} {
    global Endoscopic 
    
    
    foreach l "cLand fLand" {
    
    vtkPoints    Endoscopic($l,keyPoints)  
    vtkPoints    Endoscopic($l,graphicalInterpolatedPoints)
    vtkPoints    Endoscopic($l,allInterpolatedPoints)

    Endoscopic($l,keyPoints)  SetDataTypeToFloat
    Endoscopic($l,graphicalInterpolatedPoints) SetDataTypeToFloat
    Endoscopic($l,allInterpolatedPoints) SetDataTypeToFloat

    vtkPolyData       Endoscopic($l,polyData)       
    vtkSphereSource   Endoscopic($l,source)        
    vtkGlyph3D        Endoscopic($l,glyph)        
    vtkPolyDataMapper Endoscopic($l,mapper)
    vtkActor          Endoscopic($l,actor)
    vtkCardinalSpline Endoscopic($l,aSplineX)
    vtkCardinalSpline Endoscopic($l,aSplineY)
    vtkCardinalSpline Endoscopic($l,aSplineZ)
    vtkScalars        Endoscopic($l,scalars)

    Endoscopic($l,polyData)   SetPoints Endoscopic($l,keyPoints)
    Endoscopic($l,source)     SetRadius $Endoscopic($l,size)
    Endoscopic($l,source)     SetPhiResolution 10
    Endoscopic($l,source)     SetThetaResolution 10
    [Endoscopic($l,polyData) GetPointData] SetScalars Endoscopic($l,scalars)

    Endoscopic($l,glyph)      SetInput Endoscopic($l,polyData)
    Endoscopic($l,glyph)      SetSource [Endoscopic($l,source) GetOutput]
    Endoscopic($l,glyph) SetColorModeToColorByScalar    
    Endoscopic($l,glyph) SetScaleModeToDataScalingOff    
    Endoscopic($l,mapper)     SetInput [Endoscopic($l,glyph) GetOutput]
    Endoscopic($l,actor)      SetMapper Endoscopic($l,mapper)
    
    eval [Endoscopic($l,actor) GetProperty] SetDiffuseColor $Endoscopic($l,color)
    [Endoscopic($l,actor) GetProperty] SetSpecular .3
    [Endoscopic($l,actor) GetProperty] SetSpecularPower 30
    
    }
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicCreatePath
#  Create the vtk camera Path and focalPoint Path actors
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicCreatePath {} {
         global Endoscopic


    foreach p "c f" {
    vtkPolyData         Endoscopic(${p}Path,polyData)
    vtkCellArray        Endoscopic(${p}Path,lines)    
    vtkTubeFilter       Endoscopic(${p}Path,source)
    vtkPolyDataMapper   Endoscopic(${p}Path,mapper)    
    vtkActor            Endoscopic(${p}Path,actor)
    
    # set the input data
    Endoscopic(${p}Path,polyData)     SetLines  Endoscopic(${p}Path,lines)
    Endoscopic(${p}Path,polyData)     SetPoints Endoscopic(${p}Land,graphicalInterpolatedPoints)
    
    # set the tube

    Endoscopic(${p}Path,source)       SetNumberOfSides 8
    Endoscopic(${p}Path,source)       SetInput Endoscopic(${p}Path,polyData)
    Endoscopic(${p}Path,source)       SetRadius $Endoscopic(${p}Path,size)
    Endoscopic(${p}Path,mapper)       SetInput [Endoscopic(${p}Path,source) GetOutput]
    Endoscopic(${p}Path,actor)        SetMapper Endoscopic(${p}Path,mapper)
    
    eval  [Endoscopic(${p}Path,actor) GetProperty] SetDiffuseColor $Endoscopic(${p}Path,color)
    [Endoscopic(${p}Path,actor) GetProperty] SetSpecular .3
    [Endoscopic(${p}Path,actor) GetProperty] SetSpecularPower 30
    
    }    

}

#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateVector
#  Create the vtk vector actor
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicCreateVector {} {

    global Endoscopic

    # create the vectors 
    vtkCylinderSource tube
    vtkConeSource arrow
    vtkTransform tubeXform
    vtkTransform arrowXform
    vtkTransformPolyDataFilter tubeXformFilter
    tubeXformFilter SetInput [tube GetOutput]
    tubeXformFilter SetTransform tubeXform
    vtkTransformPolyDataFilter arrowXformFilter
    arrowXformFilter SetInput [arrow GetOutput]
    arrowXformFilter SetTransform arrowXform
    vtkAppendPolyData Endoscopic(vector,source)
    Endoscopic(vector,source) AddInput [tubeXformFilter GetOutput]
    Endoscopic(vector,source) AddInput [arrowXformFilter GetOutput]
    
    EndoscopicVectorParams 10
    
    vtkPolyData         Endoscopic(vector,polyData)
    vtkVectors          Endoscopic(vector,vectors)
    vtkGlyph3D          Endoscopic(vector,glyph)
    vtkPolyDataMapper   Endoscopic(vector,mapper)
    vtkActor            Endoscopic(vector,actor)
    vtkScalars          Endoscopic(vector,scalars)
    
    Endoscopic(vector,polyData) SetPoints Endoscopic(cLand,keyPoints)
    #    Endoscopic(vector,polyData) SetPoints Endoscopic(cLand,graphicalInterpolatedPoints)
    # set the vector glyphs
    [Endoscopic(vector,polyData) GetPointData] SetVectors Endoscopic(vector,vectors)
    [Endoscopic(vector,polyData) GetPointData] SetScalars Endoscopic(vector,scalars)
    
    Endoscopic(vector,glyph) SetInput Endoscopic(vector,polyData)
    Endoscopic(vector,glyph) SetSource [Endoscopic(vector,source) GetOutput]
    Endoscopic(vector,glyph) SetVectorModeToUseVector
    Endoscopic(vector,glyph) SetColorModeToColorByScalar
    Endoscopic(vector,glyph) SetScaleModeToDataScalingOff

    
    Endoscopic(vector,mapper) SetInput [Endoscopic(vector,glyph) GetOutput]
    Endoscopic(vector,actor) SetMapper Endoscopic(vector,mapper)
    
    Endoscopic(vector,actor) PickableOn

}


#-------------------------------------------------------------------------------
# .PROC EndoscopicVectorParams
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicVectorParams { {axislen -1} {axisrad -1} {conelen -1} } {
    global Endoscopic
    if { $axislen == -1 } { set axislen 10 }
    if { $axisrad == -1 } { set axisrad [expr $axislen*0.05] }
    if { $conelen == -1 } { set conelen [expr $axislen*0.2]}
    set axislen [expr $axislen-$conelen]
    
    set Endoscopic(vector,size) $axislen
    
    # set parameters for cylinder geometry and transform
    tube SetRadius $axisrad
    tube SetHeight $axislen
    tube SetCenter 0 [expr -0.5*$axislen] 0
    tube SetResolution 8
    tubeXform Identity
    tubeXform RotateZ 90
    
    # set parameters for cone geometry and transform
    arrow SetRadius [expr $axisrad * 2.5]
    arrow SetHeight $conelen
    arrow SetResolution 8
    arrowXform Identity
    arrowXform Translate $axislen 0 0
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateVisibility
# 
#  This procedure updates the current visibility of actor a (if specified)
#  and then set that actor to its current visibility 
# 
# .ARGS
#  a  name of the actor Endoscopic($a,actor)
#  visibility (optional) 0 or 1 
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateVisibility {a {visibility ""}} {
    
    global Endoscopic
    if {$visibility != ""} {
    # TODO: check that visibility is a number
    set Endoscopic($a,visibility) $visibility
    }
    Endoscopic($a,actor) SetVisibility $Endoscopic($a,visibility)
    if {$a == "cam"} {
    set Endoscopic(gyro,visibility) $Endoscopic($a,visibility)
    Endoscopic(gyro,actor) SetVisibility $Endoscopic(gyro,visibility)
    }
    EndoscopicSetPickable $a $Endoscopic($a,visibility)
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicSetPickable
# 
#  This procedure sets the pickability of actor a to the value specified
# 
# .ARGS
#  a  name of the actor Endoscopic($a,actor)
#  pickability 0 or 1 
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetPickable {a pickability} {
    
    global Endoscopic
    if {$pickability == 0} {
    set p Off
    } elseif {$pickability == 1} {
    set p On
    }
    if {$a == "gyro"} {
    foreach w "X Y Z" {
        Endoscopic($a,${w}actor) Pickable$p
    }
    } else {
    Endoscopic($a,actor) Pickable$p 
    }
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateSize
#
# This procedure updates the size of actor a according to Endoscopic(a,size)
#
# .ARGS
#  a  name of the actor Endoscopic($a,actor)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetSize {a} {
    global Advanced Endoscopic Path
    
    if { $a == "cam"} {
    set Endoscopic(fp,distance) [expr $Endoscopic(cam,size) * 30]
    EndoscopicCameraParams $Endoscopic($a,size)
    EndoscopicSetCameraPosition
    } elseif { $a == "vector" } {
    EndoscopicVectorParams $Endoscopic($a,size)
    } elseif { $Endoscopic(path,exists) == 1 } {
    # a is cPath,fPath,cLand or fLand => set their radius
    #set Endoscopic($a,size) Endoscopic($a,sizeStr)
    Endoscopic($a,source) SetRadius $Endoscopic($a,size)
    }
    Render3D 
}

##############################################################################
#
#       PART 2: Build the Gui
#
##############################################################################

proc EndoscopicPopBindings {} {
    global Ev
    EvDeactivateBindingSet EndoscopicEvents
    EvDeactivateBindingSet Slice0Events
    EvDeactivateBindingSet Slice1Events
    EvDeactivateBindingSet Slice2Events
}

proc EndoscopicPushBindings {} {
    global Ev
    # push onto the event stack a new event manager that deals with
    # events when the Endoscopic module is active
    EvActivateBindingSet EndoscopicEvents
    EvActivateBindingSet Slice0Events
    EvActivateBindingSet Slice1Events
    EvActivateBindingSet Slice2Events
}

proc EndoscopicCreateBindings {} {
    global Gui Ev
    
    # Creates events sets we'll  need for this module
    
    # Main events set: EndoMouseEvents
    # The entry point is the mouse click/mouse release that selects/unselects
    # => Based on whether an actor is selected or not, we either interact 
    # regularly with the screen (virtual camera moves) or we have a 
    # selected an actor. If the selected actor is the gyro, then the 
    # mouse motion moves that gyro axis. If the selected actor is a 
    # landmark or a vector, then the landmark or vector is selected 
    # and the endoscope moves there.

    # sub events set: EndoscopicTkMotion

    EvDeclareEventHandler EndoMouseEvents <Any-ButtonPress> {EndoscopicSelectActor %W %x %y}
    
    EvDeclareEventHandler EndoMouseEvents <Any-ButtonRelease> {EndoscopicUnselectActor %W %x %y}

    # create the event manager for the ability to add landmarks with
    # the key-press e on 2D slices
    EvDeclareEventHandler EndoKeySlicesEvents <KeyPress-l> { if { [SelectPick2D %W %x %y] != 0 } { eval EndoscopicAddLandmarkNoDirectionSpecified $Select(xyz); EndoscopicBuildInterpolatedPath; Render3D }}
    
    EvDeclareEventHandler EndoKeySlicesEvents <KeyPress-c> { if { [SelectPick2D %W %x %y] != 0 } { eval EndoscopicSetWorldPosition $Select(xyz);EndoscopicBuildInterpolatedPath;Render3D }} 
    
    EvDeclareEventHandler EndoKeyMainViewEvents <KeyPress-l> { if { [SelectPick Endoscopic(picker) %W %x %y] != 0 } { eval EndoscopicAddLandmarkNoDirectionSpecified $Select(xyz); EndoscopicBuildInterpolatedPath; Render3D }}
    
    EvDeclareEventHandler EndoKeyMainViewEvents <KeyPress-c> { if { [SelectPick Endoscopic(picker) %W %x %y] != 0 } { eval EndoscopicSetWorldPosition $Select(xyz);EndoscopicBuildInterpolatedPath;Render3D }}   

    # endoscopic events for 3d view
    EvAddWidgetToBindingSet EndoscopicEvents $Gui(fViewWin) {{EndoMouseEvents} {EndoKeyMainViewEvents} {tkRegularEvents}}

    # endoscopic events for slice windows (in addition to already existing events)
    EvAddWidgetToBindingSet Slice0Events $Gui(fSl0Win) {EndoKeySlicesEvents}
    EvAddWidgetToBindingSet Slice1Events $Gui(fSl1Win) {EndoKeySlicesEvents}
    EvAddWidgetToBindingSet Slice2Events $Gui(fSl2Win) {EndoKeySlicesEvents}

    # other events set we'll need:
    EvAddWidgetToBindingSet EndoscopicTkMotion $Gui(fViewWin) {{tkMotionEvents} {tkRegularEvents} {EndoMouseEvents} {EndoKeyMainViewEvents}}
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicBuildGUI
# Create the Graphical User Interface.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicBuildGUI {} {
    global Gui Module Model View Advanced Endoscopic Path
    
    
        set LposTexts "{L<->R} {P<->A} {I<->S}"
        set RposTexts "{Pitch  } {Roll  } {Yaw  }"
        set posAxi "x y z"

        set dirTexts "{X} {Y} {Z}"
        set dirAxi "rx ry rz"

        # first add the Endoscopic label to the View menu in the main Gui
    $Gui(mView) add separator
    $Gui(mView) add command -label "Add Endoscopic View" -command "EndoscopicAddEndoscopicView; Render3D"
    $Gui(mView) add command -label "Remove Endoscopic View" -command "EndoscopicRemoveEndoscopicView; Render3D"

    # A frame has already been constructed automatically for each tab.
    # A frame named "Stuff" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(Endoscopic,fStuff)
    # This is a useful comment block that makes reading this easy for all:
        #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Advanced
    #   Top
    #     Vis
    #     Title
    #     Pos
    #   Mid
    #     Vis
    #     Title
    #     Pos
    #   Bot
    #     Vis
    #     Title
    #     Pos
    # Camera    
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    set help "
    This module allows you to position an endoscopic camera in the scene and 
    view through the camera's lens in the second window.
    You can also create a fly-through trajectory and save it in the MRML file by selecting File -> 'Save Scene As'.

    <P>Tab content descriptions:<BR>
    <UL>
    <LI><B>Display</B> 
    <BR> This Tab permits you to set display parameters.
    <BR> 
    <UL>
    <LI><B>Camera</B> 
    <BR>This tab contains controls to navigate the camera and create landmarks.
    <P><B>To move and rotate the camera</B>:
    <BR>- select absolute mode (to move along the world's axis) or relative mode (to move along the camera's axis)
    <BR>- to move the camera with precision, use the sliders or type a world coordinate in the text box, next to the sliders. For the orientation of the camera, the number displayed is the angle of rotation in degrees.
    <BR>- to move the camera intuitively, use the 3D Gyro (read instructions on the tab). 
    <BR>- you can also move the camera by moving the mouse/pressing mouse buttons in the endoscopic view, just like you control the main view.
    <P>
    <BR><B>To create a path</B>:
    <BR><B>To add a landmark</B>:
    <BR> You have 2 options: you can pick a landmark position on a 2D slice or in the 3D world:
    <BR> * <B>IN 3D</B>
    <BR>- Position the camera where desired
    <BR>- Click the button <I>Add</I>: this will add a landmark (a sphere) at the current camera position and another landmark at<BR> the current focal point position 
    <BR>- Two paths will automatically be created: one that links all the camera landmarks and another one that links all the focal point landmarks
    <BR> * <B>IN 2D</B>
    <BR> - You need to have greyscale images loaded along with the model, otherwise this option won't work
    <BR> - position the mouse in the slice window of your choice until the cross hair is on top of the region where you want to add a landmark
    <BR> - press the <B>c</B> key
    <BR> - by default, both a camera and focal point landmark will be added at this position
    <BR><B>To select a landmark</B>:
    <BR> - Click on the landmark position in the scrollable text area, the line will be highlighted in red and the camera will jump to that landmark in the 3D view.
    <BR> - or Point the mouse at a landmark in the 3D view and press 'p'.
    <BR><B> To update a landmark position/orientation</B>:
    <BR> Select a landmark as described above, then reposition and reorient the camera then press the 'Update' button.
    <BR><B>To delete a camera landmark</B>:
    <BR>- Select the landmark a described above 
    <BR>- Click the <I>Delete</I> button
    <BR><B> To Delete the whole path</B>:
    <BR>- Click the <I>Delete All</I> button
    <BR><B> To save a path </B>
    <BR>- Go to File -> Save Scene As
    <BR>- Next time you open that scene, the path will appear as well.
    <BR><B> To Fly-Through on the path</B>:  
    <BR>- Go to the Path Tab
    <P>
    <LI><B>Path</B>
    <BR>This Tab contains controls to fly-through on the path
    <BR><B>Command buttons</B>: 
    <BR>- <I>Fly-Through</I>: moves the camera along the path and orient its focal point along the focal point path
    <BR><I>- Reset</I>: stops the motion of the camera and repositions it at the beginning of the path
    <BR><I>- Stop</I>: stops the motion of the camera
    <BR><B>Command sliders</B>:
    <BR><I>- Frame</I>: positions the camera on any part of the path
    <BR><I>- Speed</I>: controls the speed of the camera's motion along the path (the number corresponds to the number of frames to skip)
    <BR><I>- No of Interpolated Points </I>: this will recompute the path with approximately the number of interpolated points per mm specified in between key points (landmarks).
    <BR><B>To drive the slices</B>:
    <BR> Select which driver you want: 
    <BR> - <B>User</B>: the user selects the slice position and orientation with the user interface
    <BR> - <B>Camera</B>: the slices in the orientation 'InPlane', 'Perp', 'InPlane90' are reformatted to intersect at the camera position and be oriented along the 3 axes of the camera.
    <BR> - <B>Focal Point</B>: same as above with the focal point instead of the camera.
    <BR> - <B>Intersection</B>: the intersection is the point on the surface of the model in the center of the endoscopic screen. It is the intersection between a 'ray' shot from the camera straight ahead and any triangulated surface that is first intersected.
    <BR><B> Random Path </B>
    <BR><I>- Compute Path</I>: creates a new random path at every click
    <BR><I>- Delete Path</I>: deletes the random path (or any path previously created)
    <BR>
    <BR> The functionality of creating/deleting a random path is there for you to play around with difference fly-through options without having to create a path. If you feel like having a bit of fun, click on <I> Show Path </I> and then set the <I> RollerCoaster</I> option before doing a fly-through. <B>Enjoy the ride! </B>
    
    <P>
    <LI><B>Advanced</B>
    <BR>This Tab allows you to change color and size parameters for the camera, focal point, landmarks and path. You can also change the virtual camera's lens angle for a wider view.
    "
    regsub -all "\n" $help { } help
    MainHelpApplyTags Endoscopic $help
        MainHelpBuildGUI Endoscopic

    #-------------------------------------------
    # Display frame
    #-------------------------------------------
    set fDisplay $Module(Endoscopic,fDisplay)
    set f $fDisplay
        
    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    frame $f.fMain -bg $Gui(activeWorkspace) -relief groove -bd 2 
    frame $f.fEndo -bg $Gui(activeWorkspace) -relief groove -bd 2 
    frame $f.fTitle2 -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns $f.fMain $f.fEndo $f.fTitle2 -side top -pady 5 -fill x
    
    
    eval {label $f.fTitle.lTitle -text "
    If your screen allows, you can change 
    the width of the view screen to have 
    a better Endoscopic view:"} $Gui(WLA)
    pack $f.fTitle.lTitle -side left -padx $Gui(pad) -pady 0
    
    eval {label $f.fBtns.lW -text "Width:"} $Gui(WLA)
    eval {entry $f.fBtns.eWidth -width 5 -textvariable View(viewerWidth)} $Gui(WEA)
    bind $f.fBtns.eWidth  <Return> {MainViewerSetMode}
    eval {menubutton $f.fMBtns -text "choose" -menu $f.fMBtns.fMenu} $Gui(WMBA)
    TooltipAdd $f.fMBtns "Choose from the following sizes"
    eval {menu $f.fMBtns.fMenu} $Gui(WMA)
    $f.fMBtns.fMenu add command -label 1000 -command {set View(viewerWidth) 1000; MainViewerSetMode}
    $f.fMBtns.fMenu add command -label 768 -command {set View(viewerWidth) 768; MainViewerSetMode}
    
    grid $f.fBtns.lW $f.fBtns.eWidth $f.fMBtns -padx $Gui(pad)

    set f $fDisplay.fMain
    eval {label $f.fexpl -text "Main View parameter:"} $Gui(WLA)
     eval {checkbutton $f.cMainView -variable Endoscopic(mainview,visibility) -text "Show Main View" -command "EndoscopicUpdateMainViewVisibility" -indicatoron 0} $Gui(WCA)    

    pack $f.fexpl $f.cMainView -pady 2
    

    set f $fDisplay.fEndo
        eval {label $f.fexpl -text "Endoscopic View parameters:"} $Gui(WLA)
    
    eval {checkbutton $f.cEndoView -variable Endoscopic(endoview,visibility) -text "Show Endoscopic View" -command "EndoscopicUpdateEndoscopicViewVisibility" -indicatoron 0} $Gui(WCA)    
    
    
    eval {checkbutton $f.cslices -variable Endoscopic(SlicesVisibility) -text "Show 2D Slices" -command "EndoscopicSetSlicesVisibility" -indicatoron 0} $Gui(WCA)
    pack $f.fexpl -side top -pady 2
    pack $f.cEndoView  -side top -padx $Gui(pad) -pady 0
    pack $f.cslices -side top -pady 5

    set f $fDisplay

    eval {checkbutton $f.cExitEndoView -variable Endoscopic(endoview,hideOnExit) -text "Hide Endoscopic View on Exit" -indicatoron 0} $Gui(WCA)   

    set text "
    To start, go to the Camera tab 
    If you need help, go to the Help tab"

    
    eval {label $f.fTitle2.lTitle -text $text} $Gui(WLA)
    pack $f.cExitEndoView -side top -padx $Gui(pad) -pady 7
    
    #-------------------------------------------
    # Camera frame
    #-------------------------------------------
    set fCamera $Module(Endoscopic,fCamera)
    set f $fCamera
    
    frame $f.fTop -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fMid -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fBot -bg $Gui(activeWorkspace) -relief groove -bd 2
    pack $f.fTop $f.fMid $f.fBot -side top -pady 0.5 -padx 0 -fill x
    
    
    #-------------------------------------------
    # Camera->Top frame
    #-------------------------------------------
    set f $fCamera.fTop
    
    frame $f.fGyro   -bg $Gui(activeWorkspace)
    frame $f.fGyro2   -bg $Gui(activeWorkspace)
    frame $f.fInfo     -bg $Gui(activeWorkspace)
    frame $f.fInfo2     -bg $Gui(activeWorkspace)
    pack  $f.fGyro $f.fGyro2 $f.fInfo $f.fInfo2 \
        -side top 
    
    
    #-------------------------------------------
    # Camera->Top->Title frame
    #-------------------------------------------
    
    set f $fCamera.fTop.fGyro
    
    eval {checkbutton $f.cgyro -variable Endoscopic(gyro,use) -text "Use the 3D Gyro" -command "EndoscopicUseGyro; Render3D" -indicatoron 0 } $Gui(WBA)

    set Endoscopic(gyro,image,absolute) [image create photo -file \
        [ExpandPath [file join gui endoabsolute.gif]]]
    
    set Endoscopic(gyro,image,relative) [image create photo -file \
        [ExpandPath [file join gui endorelative.gif]]]

    
    set Endoscopic(imageButton) $fCamera.fTop.fGyro.cb
    
    eval {checkbutton $Endoscopic(imageButton)  -image $Endoscopic(gyro,image,relative) -variable Endoscopic(cam,navigation,relative) -width 70 -indicatoron 0 -height 70} $Gui(WBA)
 
    eval {button $f.bhow -text "How ?"} $Gui(WBA)
    TooltipAdd $f.bhow "Select an axis by clicking on it with the mouse.
Translate the axis by pressing the left mouse button and moving the mouse.
Rotate the axis by pressing the right mouse button and moving the mouse."

    pack $f.cb  $f.cgyro $f.bhow -side left -pady 3 -padx 2

    set f $fCamera.fTop.fGyro2
    eval {label $f.lgyroOr -text "Orientation"} $Gui(WTA)
    foreach item "absolute relative" {    
    eval {radiobutton $f.f$item -variable Endoscopic(cam,axis) -text $item -value $item -command "$Endoscopic(imageButton) configure -image $Endoscopic(gyro,image,${item}); EndoscopicSetGyroOrientation; EndoscopicSetCameraAxis $item; Render3D"} $Gui(WBA)
    pack $f.lgyroOr $f.f$item -side left -padx 2 -pady 2
    }
    
    #-------------------------------------------
    # Camera->Mid frame
    #-------------------------------------------
    set f $fCamera.fMid

    frame $f.fPosTitle   -bg $Gui(activeWorkspace)
    frame $f.fPos   -bg $Gui(activeWorkspace)
    pack $f.fPosTitle $f.fPos -side top
    frame $f.fRotTitle     -bg $Gui(activeWorkspace)
    frame $f.fRot     -bg $Gui(activeWorkspace)
    frame $f.fZoom    -bg $Gui(activeWorkspace)
    pack $f.fRotTitle $f.fRot -side top 
    #    pack $f.fZoom -side top


    set f $fCamera.fMid.fPosTitle
    
    eval {label $f.l -height 1 -text "Camera Position"} $Gui(WTA)
    eval {button $f.r \
        -text "reset" -width 10 -command "EndoscopicResetCameraPosition; Render3D"} $Gui(WBA)            
    pack $f.l $f.r -padx 1 -pady 1 -side left
    
    set f $fCamera.fMid.fPos

    #Position Sliders
    foreach slider $posAxi Ltext $LposTexts Rtext $RposTexts orient "horizontal horizontal vertical" {
    
    EndoscopicCreateLabelAndSlider $f l$slider 0 "$Ltext" $slider $orient -400 400 110 Endoscopic(cam,${slider}Str) "EndoscopicSetCameraPosition $slider" 5 0
    set Endoscopic(slider$slider) $f.s$slider
    set Endoscopic(label$slider) $f.l$slider
    }

    

    #-------------------------------------------
    # Camera->Mid->Title frame
    #-------------------------------------------

        set f $fCamera.fMid.fRotTitle

    eval {label $f.l -height 1 -text "Camera Orientation"} $Gui(WTA)
    eval {button $f.r \
        -text "reset" -width 10 -command "EndoscopicResetCameraDirection; Render3D"} $Gui(WBA)            
    grid $f.l $f.r -padx 1 -pady 1

    #-------------------------------------------
    # Camera->Mid->Dir frame
    #-------------------------------------------
    set f $fCamera.fMid.fRot

    # Rotation Sliders
    foreach slider $dirAxi Rtext $RposTexts orient "horizontal horizontal vertical" {

        EndoscopicCreateLabelAndSlider $f l$slider 0 "$Rtext" $slider $orient -360 360 110 Endoscopic(cam,${slider}Str) "EndoscopicSetCameraDirection $slider" 5 0
        set Endoscopic(slider$slider) $f.s$slider
        set Endoscopic(label$slider) $f.l$slider
    }

    #-------------------------------------------
    # Camera->Mid->Zoom frame
    #-------------------------------------------
    set f $fCamera.fMid.fZoom

    # Position Sliders

    eval {label $f.l2 -height 2 -text "Camera Zoom"} $Gui(WTA)
    grid $f.l2
    
        EndoscopicCreateLabelAndSlider $f lzoom 0 "In<->Out" $slider horizontal 1 500 110 Endoscopic(cam,zoomStr) "EndoscopicSetCameraZoom" 5 30


    #-------------------------------------------
    # Camera->Bot frame
    #-------------------------------------------

    #-------------------------------------------
    # Camera->Bot->Rec frame
    #-------------------------------------------

    EndoscopicBuildLandmarkScroll $fCamera.fBot

    #-------------------------------------------
    # Path frame
    #-------------------------------------------
    set fPath $Module(Endoscopic,fPath)
    set f $fPath

    foreach frame "Top TMid BMid Bot BBot" {
        frame $f.f$frame -bg $Gui(activeWorkspace) -relief groove -bd 2
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Path->Top frame
    #-------------------------------------------
    set f $fPath.fTop
    
    frame $f.fFly     -bg $Gui(activeWorkspace) 
    frame $f.fStep     -bg $Gui(activeWorkspace)

    pack  $f.fFly $f.fStep\
        -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Path->Top->Fly frame
    #-------------------------------------------
    set f $fPath.fTop.fFly

    # Compute path
    eval {button $f.fPath \
        -text "Fly-Thru"  -width 8 -command "EndoscopicViewPath; Render3D"} $Gui(WMBA) {-bg $Endoscopic(path,sColor)}   

    eval {button $f.fReset \
        -text "Reset"  -width 8  -command "EndoscopicResetPath; Render3D"} $Gui(WMBA) {-bg $Endoscopic(path,sColor)}   

    eval {button $f.fStop \
        -text "Stop" -width 6 -command "EndoscopicStopPath; Render3D"} $Gui(WMBA) {-bg $Endoscopic(path,sColor)}   

    #foreach value "Forward Backward" width "9 11" {
    #    eval {radiobutton $f.r$value -width $width \
    #        -text "$value" -value "$value" -variable Endoscopic(flyDirection)\
    #        -indicatoron 0 -command "EndoscopicSetFlyDirection $value; Render3D"} $Gui(WMBA) {-bg $Endoscopic(path,sColor)} {-selectcolor $Endoscopic(path,cColor)}  
    #}       
    #$f.rForward select

    grid $f.fPath $f.fReset $f.fStop -padx $Gui(pad) -pady $Gui(pad)
    #grid $f.rForward $f.rBackward -padx $Gui(pad) -pady $Gui(pad)
    
    #-------------------------------------------
    # Path->Top->Step frame
    #-------------------------------------------
    set f $fPath.fTop.fStep

    # Position Sliders

    eval {label $f.lstep -text "Frame:"} $Gui(WLA)

    eval {entry $f.estep \
        -textvariable Endoscopic(path,stepStr) -width 4} $Gui(WEA) {-bg $Endoscopic(path,sColor)}
    bind $f.estep <Return> \
        "EndoscopicSetPathFrame; Render3D"
    
    eval {scale $f.sstep -from 0 -to 400 -length 100 \
            -variable Endoscopic(path,stepStr) \
        -command "EndoscopicSetPathFrame; Render3D"\
            -resolution 1} $Gui(WSA) {-troughcolor $Endoscopic(path,sColor)}

    set Endoscopic(path,stepScale) $f.sstep
    
    # default value
    $f.sstep set 0
    
    # Grid
    grid $f.lstep $f.estep $f.sstep -padx $Gui(pad) -pady $Gui(pad)
        grid $f.lstep -sticky e

    #-------------------------------------------
    # Path->TMid frame
    #-------------------------------------------
    set f $fPath.fTMid
    frame $f.fSpeed     -bg $Gui(activeWorkspace)
    frame $f.fInterp     -bg $Gui(activeWorkspace)
    
    pack  $f.fSpeed $f.fInterp -side top -padx $Gui(pad) -pady 0

    #-------------------------------------------
    # Path->TMid->Speed frame
    #-------------------------------------------
    set f $fPath.fTMid.fSpeed

    # Position Sliders

    eval {label $f.lspeed -text "Speed:"} $Gui(WLA)

    eval {entry $f.espeed \
        -textvariable Endoscopic(path,speedStr) -width 4} $Gui(WEA) {-bg $Endoscopic(path,sColor)}
    bind $f.espeed <Return> \
        "EndoscopicSetSpeed; Render3D"
    
    eval {scale $f.sspeed -from 1 -to 20 -length 100 \
            -variable Endoscopic(path,speedStr) \
            -command "EndoscopicSetSpeed; Render3D"\
            -resolution 1} $Gui(WSA) {-troughcolor $Endoscopic(path,sColor)}

    set Endoscopic(path,speedScale) $f.sspeed
    
    # default color values for the lens sliders
    $f.sspeed set 1

# Grid
    grid $f.lspeed $f.espeed $f.sspeed 
        grid $f.lspeed -sticky e


    set f $fPath.fTMid.fInterp
    
    eval {label $f.linterp -text "You can change the number of \n interpolated points per mm: \n (more points will decrease the \n fly speed)"} $Gui(WLA)
    eval {entry $f.einterp \
        -textvariable Endoscopic(path,interpolationStr) -width 4} $Gui(WEA) {-bg $Endoscopic(path,sColor)}
    bind $f.einterp <Return> \
        "EndoscopicBuildInterpolatedPath; Render3D"
    
    pack $f.linterp $f.einterp -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Path->BMid frame
    #-------------------------------------------
    set f $fPath.fBMid
    frame $f.fSli     -bg $Gui(activeWorkspace)
    
    pack  $f.fSli -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Path->Bot->Sli frame
    #-------------------------------------------
    set f $fPath.fBMid.fSli

    # set the camera or focal point to be the slice driver
    
    eval {label $f.lTitle -text "The Camera or Focal Point can Drive \n the Slices"} $Gui(WTA)
    grid $f.lTitle -padx 1 -pady 1 -columnspan 2

    eval {label $f.lDriver -text "Driver:"} $Gui(WTA)

        eval {menubutton $f.mbDriver -text "User" \
            -menu $f.mbDriver.m -width 12 -relief raised \
            -bd 2} $Gui(WMBA) {-bg $Endoscopic(path,eColor)}
        set Endoscopic(mDriver) $f.mbDriver.m
        set Endoscopic(mbDriver) $f.mbDriver
        eval {menu $f.mbDriver.m} $Gui(WMA) {-bg $Endoscopic(path,eColor)}
        foreach item "User Camera FocalPoint Intersection" {
            $Endoscopic(mDriver) add command -label $item \
                -command "EndoscopicSetSliceDriver $item"
        }

            grid $f.lDriver $f.mbDriver -padx $Gui(pad) -pady $Gui(pad)
    #-------------------------------------------
    # Path->Bot frame
    #-------------------------------------------
    set f $fPath.fBot
    frame $f.fVis     -bg $Gui(activeWorkspace)
    
    pack  $f.fVis -side top -padx $Gui(pad) -pady $Gui(pad)


    #-------------------------------------------
    # Path->Bot->Vis frame
    #-------------------------------------------
    set f $fPath.fBot.fVis

    # Compute path
        eval {label $f.lTitle -text "You can create a Random Path to \n play with Fly-Thru options:"} $Gui(WTA)
        grid $f.lTitle -padx 1 -pady 1 -columnspan 2

    eval {button $f.cRandPath \
        -text "New random path" -width 16 -command "EndoscopicComputeRandomPath; Render3D"} $Gui(WBA) {-bg $Endoscopic(path,rColor)}           

    eval {button $f.dRandPath \
        -text "Delete path" -width 14 -command "EndoscopicDeletePath; Render3D"} $Gui(WBA) {-bg $Endoscopic(path,rColor)}           
    
    eval {checkbutton $f.rPath \
        -text "RollerCoaster" -variable Endoscopic(path,rollerCoaster) -width 12 -indicatoron 0 -command "Render3D"} $Gui(WBA) {-bg $Endoscopic(path,rColor)}             
    
    eval {checkbutton $f.sPath \
        -text "Show Path" -variable Endoscopic(path,showPath) -width 12 -indicatoron 0 -command "EndoscopicShowPath; Render3D"} $Gui(WBA) {-bg $Endoscopic(path,rColor)}             
    grid $f.cRandPath $f.dRandPath -padx 0 -pady $Gui(pad)
    #grid $f.rPath $f.sPath -padx 0 -pady $Gui(pad)
    grid $f.sPath -padx 0 -pady $Gui(pad)

    #EndoscopicBuildLandmarkScroll $fPath.fBBot
    #-------------------------------------------
    # Advanced frame
    #-------------------------------------------
    set fAdvanced $Module(Endoscopic,fAdvanced)
    set f $fAdvanced

    frame $f.fTop -bg $Gui(activeWorkspace) 
    frame $f.fMid -bg $Gui(activeWorkspace) 
        pack $f.fTop $f.fMid -side top -pady 0 -padx 0 -fill x

    #-------------------------------------------
    # Advanced->Top frame
    #-------------------------------------------
    set f $fAdvanced.fTop

    frame $f.fVis     -bg $Gui(activeWorkspace)
    frame $f.fTitle   -bg $Gui(activeWorkspace)
    frame $f.fPos     -bg $Gui(activeWorkspace)
    pack $f.fVis $f.fTitle $f.fPos \
        -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Advanced->Top->Vis frame
    #-------------------------------------------
    set f $fAdvanced.fTop.fVis

    eval {label $f.lV -text Visibility} $Gui(WLA)
    eval {label $f.lO -text Color} $Gui(WLA)
        eval {label $f.lS -text "  Size"} $Gui(WLA)
    
    grid $f.lV $f.lO $f.lS -pady 2 -padx 2

    # Done in EndoscopicCreateAdvancedGUI

        EndoscopicCreateAdvancedGUI $f cam   visible   noColor size
        EndoscopicCreateAdvancedGUI $f Lens  notvisible color
        EndoscopicCreateAdvancedGUI $f Box   notvisible color
        EndoscopicCreateAdvancedGUI $f fp    visible    color
        EndoscopicCreateAdvancedGUI $f cLand visible    color size
        EndoscopicCreateAdvancedGUI $f cPath visible    color size
        EndoscopicCreateAdvancedGUI $f vector visible   color size
    EndoscopicCreateAdvancedGUI $f fLand visible    color size
        EndoscopicCreateAdvancedGUI $f fPath visible    color size
    
    #-------------------------------------------
    # Advanced->Mid frame
    #-------------------------------------------
    set f $fAdvanced.fMid

        frame $f.fAngle    -bg $Gui(activeWorkspace)
        frame $f.fTitle     -bg $Gui(activeWorkspace)
        frame $f.fToggle     -bg $Gui(activeWorkspace)
    frame $f.fVis     -bg $Gui(activeWorkspace)
    
    pack $f.fAngle $f.fToggle $f.fVis  -side top -padx 1 -pady 1
    
    #-------------------------------------------
    # Advanced->Mid->Angle frame
    #-------------------------------------------
    set f $fAdvanced.fMid.fAngle

        EndoscopicCreateLabelAndSlider $f l2 2 "Lens Angle" "Angle" horizontal 0 360 110 Endoscopic(cam,AngleStr) "EndoscopicSetCameraViewAngle" 5 90


    #-------------------------------------------
    # Advanced->Mid->Vis frame
    #-------------------------------------------
    
        set f $fAdvanced.fMid.fVis
    
    EndoscopicCreateCheckButton $f.cmodels Endoscopic(ModelsVisibilityInside) "Show Inside Models"  "EndoscopicSetModelsVisibilityInside"

    
    pack $f.cmodels

        #-------------------------------------------
    # Advanced->Mid->Toggle frame
    #-------------------------------------------

    set f $fAdvanced.fMid.fToggle
    


    eval {label $f.l -height 2 -text "Collision Detection:"} $Gui(WTA)
    eval {menubutton $f.fMBtns -text "off" -menu $f.fMBtns.fMenu} $Gui(WMBA)  
    eval {menu $f.fMBtns.fMenu} $Gui(WMA) 
    
    $f.fMBtns.fMenu add command -label "off" -command {EndoscopicSetCollision 0;}
    $f.fMBtns.fMenu add command -label "on" -command {EndoscopicSetCollision 1}
    
    eval {label $f.l2 -height 2 -text "distance: 0"} $Gui(WTA)
    set Endoscopic(collMenu) $f.fMBtns
    set Endoscopic(collDistLabel) $f.l2
    grid $f.l $f.fMBtns $f.l2 -padx 1 -pady 1
    
    
    }
    

    proc EndoscopicBuildLandmarkScroll {f } {
    global Endoscopic Gui

    
    set f $f.fShow
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx $Gui(pad) -pady $Gui(pad)

    eval {label $f.lTitle -text "Path Creation"} $Gui(WTA)
    eval {button $f.bhow -text "How Do I create a path?"} $Gui(WBA)
    TooltipAdd $f.bhow "A path is like a rail that guides the virtual endoscope during the fly-through.
A path is a smooth connection between landmarks defined by the user. 

To create a landmark, there are 2 options:

* place the endoscope in the desired position and orientation and click the 'Add' button. A little sphere is created at the position of the camera and a 
vector shows the direction of view.

* pick a point in the 3D View or on any of the slice windows with the mouse and press the key 'l'. A landmark is added at that position with a default 
direction of view.

To update a landmark:

* pick the landmark (sphere or vector) in the 3D screen by clicking on it with the mouse. The endoscope jumps to that landmark and positions itself along 
the vector of view. Re-orient or move the endoscope as needed. Click the 'Update' button to apply the new information to that landmark.

To Delete a landmark:

* you can delete a landmark by picking it in the 3D screen or by selecting it in the scrolltext. Then click the 'Delete' button. "

    set Endoscopic(path,fLandmarkList) [ScrolledListbox $f.list 1 1 -height 10 -width 20 -selectforeground red]
    bind $Endoscopic(path,fLandmarkList) <ButtonRelease-1> {EndoscopicLandmarkSelected}

    
    frame $f.fBtns -bg $Gui(activeWorkspace)
    eval {button $f.fBtns.bAdd -text "Add" -width 4 \
        -command "EndoscopicAddLandmarkDirectionSpecified; EndoscopicBuildInterpolatedPath; Render3D"} $Gui(WBA)
    TooltipAdd $f.fBtns.bAdd "To add a Landmark: place the Camera where desired and click Add"
    eval {button $f.fBtns.bDelete -text "Delete" -width 7 \
        -command "EndoscopicDeleteLandmark; Render3D"} $Gui(WBA)
    TooltipAdd $f.fBtns.bDelete "To delete a Landmark: select a Landmark by clicking on it with the mouse and click Delete"
    eval {button $f.fBtns.bMove -text "Update" -width 7 \
        -command "EndoscopicUpdateLandmark; Render3D"} $Gui(WBA)
    TooltipAdd $f.fBtns.bMove "Select a Landmark, move/re-orient the endoscope to update the position and orientation of the landmark, then click Update"
    eval {button $f.fBtns.bDeletePath -text "Delete Path" -width 10 \
        -command "EndoscopicDeletePath; Render3D"} $Gui(WBA)
    TooltipAdd $f.fBtns.bDeletePath "Deletes the whole Path"
    pack $f.fBtns.bAdd $f.fBtns.bMove $f.fBtns.bDelete $f.fBtns.bDeletePath -side top -padx $Gui(pad) -pady $Gui(pad)
    pack $f.lTitle $f.bhow -side top -pady 0
    pack $f.list $f.fBtns -side left -pady $Gui(pad)
    }
#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateLabelAndSlider
# 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
    proc EndoscopicCreateLabelAndSlider {f labelName labelHeight labelText sliderName orient from to length variable commandString entryWidth defaultSliderValue} {

    global Gui Model
    eval {label $f.$labelName -height $labelHeight -text $labelText} $Gui(WTA)

    # Sliders        
    eval {scale $f.s$sliderName -from $from -to $to -length $length \
        -variable $variable -orient vertical\
        -command "$commandString; Render3D" \
        -resolution .1} $Gui(WSA)

    eval {entry $f.e$sliderName \
        -textvariable $variable -width $entryWidth} $Gui(WEA)
    bind $f.e$sliderName <Return> \
        "$commandString; Render3D"
    #bind $f.e$sliderName <FocusOut> \
    #    "$commandString; Render3D"
    
    # Grid
    grid $f.$labelName $f.e$sliderName $f.s$sliderName -padx 0 -pady 0
    
    # default value for the slider
    $f.s$sliderName set $defaultSliderValue
    }

    
#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateCheckButton
# 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
    proc EndoscopicCreateCheckButton {ButtonName VariableName Message Command {Indicatoron 0} {Width 0} } {
    global Gui
    if {$Width == 0 } {
        set Width [expr [string length $Message]]
    }
    eval  {checkbutton $ButtonName -variable $VariableName -text \
    $Message -width $Width -indicatoron $Indicatoron -command $Command } $Gui(WCA)
} 


#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateAdvancedGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicCreateAdvancedGUI {f a {vis ""} {col ""} {size ""}} {
    global Gui Endoscopic Color Advanced

    
    # Name / Visible only if vis is not empty
    if {$vis == "visible"} {
    eval {checkbutton $f.c$a \
        -text $Endoscopic($a,name) -wraplength 65 -variable Endoscopic($a,visibility) \
        -width 11 -indicatoron 0\
        -command "EndoscopicUpdateVisibility $a; Render3D"} $Gui(WCA)
    } else {
    eval {label $f.c$a -text $Endoscopic($a,name)} $Gui(WLA)
    }

    # Change colors
    if {$col == "color"} {
    eval {button $f.b$a \
        -width 1 -command "EndoscopicSetActive $a $f.b$a; ShowColors EndoscopicPopupCallback; Render3D" \
        -background [MakeColorNormalized $Endoscopic($a,color)]}
    } else {
    eval {label $f.b$a -text " " } $Gui(WLA)
    }
    
    if {$size == "size"} {
    
        eval {entry $f.e$a -textvariable Endoscopic($a,size) -width 3} $Gui(WEA)
        bind $f.e$a  <Return> "EndoscopicSetSize $a"
        eval {scale $f.s$a -from 0.0 -to 10.0 -length 70 \
        -variable Endoscopic($a,size) \
        -command "EndoscopicSetSize $a; Render3D" \
        -resolution 0.01} $Gui(WSA) {-sliderlength 10 }    
    grid $f.c$a $f.b$a $f.e$a $f.s$a -pady 0 -padx 0        
    } else {
    grid $f.c$a $f.b$a -pady 0 -padx 0        
    }
}

    
#-------------------------------------------------------------------------------
# .PROC EndoscopicSetActive
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetActive {a b} {

    global Advanced

    set Advanced(ActiveActor) $a
    set Advanced(ActiveButton) $b 
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicPopupCallback
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicPopupCallback {} {
    global Label Endoscopic Color Gui Advanced

        set name $Label(name) 

        set a $Advanced(ActiveActor)

    # Use second color by default
    set color [lindex $Color(idList) 1]

    foreach c $Color(idList) {
        if {[Color($c,node) GetName] == $name} {
            set color $c
        }
    }

    [Endoscopic($a,actor) GetProperty] SetAmbient    [Color($color,node) GetAmbient]
    [Endoscopic($a,actor) GetProperty] SetDiffuse    [Color($color,node) GetDiffuse]
    [Endoscopic($a,actor) GetProperty] SetSpecular   [Color($color,node) GetSpecular]
    [Endoscopic($a,actor) GetProperty] SetSpecularPower [Color($color,node) GetPower]
    eval [Endoscopic($a,actor) GetProperty] SetColor    [Color($color,node) GetDiffuseColor]

    set Endoscopic($a,color) [Color($color,node) GetDiffuseColor]

    # change the color of the button on the Advanced GUI
    $Advanced(ActiveButton) configure -background [MakeColorNormalized [[Endoscopic($a,actor) GetProperty] GetColor]]

    if {$Advanced(ActiveActor) == "Box"} {
        
        [Endoscopic(Box2,actor) GetProperty] SetAmbient    [Color($color,node) GetAmbient]
        [Endoscopic(Box2,actor) GetProperty] SetDiffuse    [Color($color,node) GetDiffuse]
        [Endoscopic(Box2,actor) GetProperty] SetSpecular   [Color($color,node) GetSpecular]
        [Endoscopic(Box2,actor) GetProperty] SetSpecularPower [Color($color,node) GetPower]
        eval [Endoscopic(Box2,actor) GetProperty] SetColor    [Color($color,node) GetDiffuseColor]

    }
    Render3D
}


##############################################################################
#
#        PART 3: Selection of actors through key/mouse
#
#############################################################################

proc EndoscopicUseGyro {} {
    global Endoscopic
        if { $Endoscopic(gyro,use) == 1} {
    foreach XX "X Y Z" {
        [Endoscopic(gyro,${XX}actor) GetProperty] SetOpacity 1
    }
    EndoscopicSetPickable gyro 1
    } else {
    foreach XX "X Y Z" {
        [Endoscopic(gyro,${XX}actor) GetProperty] SetOpacity 0
    }
    EndoscopicSetPickable gyro 0
    }
    Render3D

}
proc EndoscopicUnselectActor {widget x y} {

    global Endoscopic 

    # get the path of actors selected by the picker

    set assemblyPath [Endoscopic(picker) GetPath]
    # if nothing was picked, then pop the tk interaction events from the stack
    if { $assemblyPath == "" } {
    # deactivate the tk motion bindings
    EvDeactivateBindingSet EndoscopicTkMotion
    EndMotion $widget $x $y
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicSelectActor
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSelectActor {widget x y} {

    global Endoscopic Ev

    if { [SelectPick Endoscopic(picker) $widget $x $y] != 0 } {
    
    # get the path of actors selected by the picker
    set assemblyPath [Endoscopic(picker) GetPath]
    $assemblyPath InitTraversal
    set assemblyNode [$assemblyPath GetLastNode]
    set actor [$assemblyNode GetProp]
    
    if { [$actor GetProperty] == [Endoscopic(gyro,Xactor) GetProperty] } {
        
        eval [Endoscopic(gyro,Xactor) GetProperty] SetColor $Endoscopic(gyro,Xactor,selectedColor) 
        eval [Endoscopic(gyro,Yactor) GetProperty] SetColor $Endoscopic(gyro,Yactor,color) 
        eval [Endoscopic(gyro,Zactor) GetProperty] SetColor $Endoscopic(gyro,Zactor,color) 
        XformAxisStart Endoscopic gyro $widget 0 $x $y
        
    } elseif { [$actor GetProperty] == [Endoscopic(gyro,Yactor) GetProperty] } {
        eval [Endoscopic(gyro,Xactor) GetProperty] SetColor $Endoscopic(gyro,Xactor,color) 
        eval [Endoscopic(gyro,Yactor) GetProperty] SetColor $Endoscopic(gyro,Yactor,selectedColor) 
        eval [Endoscopic(gyro,Zactor) GetProperty] SetColor $Endoscopic(gyro,Zactor,color) 
        XformAxisStart Endoscopic gyro $widget 1 $x $y
        
    } elseif { [$actor GetProperty]  == [Endoscopic(gyro,Zactor) GetProperty] } {
        eval [Endoscopic(gyro,Xactor) GetProperty] SetColor $Endoscopic(gyro,Xactor,color) 
        eval [Endoscopic(gyro,Yactor) GetProperty] SetColor $Endoscopic(gyro,Yactor,color) 
        eval [Endoscopic(gyro,Zactor) GetProperty] SetColor $Endoscopic(gyro,Zactor,selectedColor)
        XformAxisStart Endoscopic gyro $widget 2 $x $y
        
    } elseif { [$actor GetProperty] == [Endoscopic(vector,actor) GetProperty] } {
        set numCells [[Endoscopic(vector,source) GetOutput] GetNumberOfCells]
        
        set cid [Endoscopic(picker) GetCellId]
        # see which vector we have selected
        set id [expr $cid/$numCells]
        EndoscopicVectorSelected $id
        
    } elseif { [$actor GetProperty] == [Endoscopic(cLand,actor) GetProperty] } {
        set numCells [[Endoscopic(cLand,source) GetOutput] GetNumberOfCells]
        
        set cid [Endoscopic(picker) GetCellId]
        # see which landmark we have selected
        set id [expr $cid/$numCells]
        EndoscopicLandmarkSelected $id
    } else {
        # if none of the above is picked, 
        # then interact with the virtual camera, as usual
        # activate the tk motion bindings
        EvActivateBindingSet EndoscopicTkMotion
        StartMotion $widget $x $y
    }
    Render3D
    } else {
        # if nothing is picked, then interact with the virtual camera, as usual
    # activate the tk motion bindings
    
    EvActivateBindingSet EndoscopicTkMotion
    StartMotion $widget $x $y
    }
}
#-------------------------------------------------------------------------------
# .PROC EndoscopicVectorSelected
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicVectorSelected {id} {

    global Endoscopic

    Endoscopic(vector,scalars) SetScalar $Endoscopic(vector,selectedID) 0.5
    Endoscopic(vector,scalars) SetScalar $id 0
    Endoscopic(vector,polyData) Modified
    set Endoscopic(vector,selectedID) $id    
  #  EndoscopicMoveGyroToVector $id
    EndoscopicMoveGyroToLandmark $id
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicLandmarkSelected
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicLandmarkSelected {{id ""}} {

    global Endoscopic

    if {$id == ""} {
    # this was called when the user selected a landmark in the text box,
    # so find out the id
    set id [$Endoscopic(path,fLandmarkList) curselection]
    }
    
    Endoscopic(cLand,scalars) SetScalar $Endoscopic(cLand,selectedID) 0.2 
    Endoscopic(cLand,scalars) SetScalar $id 0 
    Endoscopic(cLand,polyData) Modified
    set Endoscopic(cLand,selectedID) $id    
    EndoscopicUpdateSelectionLandmarkList $id
    EndoscopicMoveGyroToLandmark  $id
    Render3D
}


#############################################################################
#
#      PART 4 : Endoscope movement
#
#############################################################################

proc EndoscopicGyroMotion {actor} {
    
    global Endoscopic
    if {$actor == "Endoscopic(gyro,actor)"} {
    # get the position of the gyro and set the sliders
    set cam_mat [Endoscopic(cam,actor) GetMatrix]   
    set Endoscopic(cam,xStr,old) [$cam_mat GetElement 0 3] 
    set Endoscopic(cam,yStr,old) [$cam_mat GetElement 1 3] 
    set Endoscopic(cam,zStr,old) [$cam_mat GetElement 2 3] 
    set Endoscopic(cam,xStr) [$cam_mat GetElement 0 3] 
    set Endoscopic(cam,yStr) [$cam_mat GetElement 1 3] 
    set Endoscopic(cam,zStr) [$cam_mat GetElement 2 3] 

    # get the orientation of the gyro and set the sliders 
    set or [Endoscopic(gyro,actor) GetOrientation]
    set Endoscopic(cam,rxStr,old) [lindex $or 0]
    set Endoscopic(cam,ryStr,old) [lindex $or 1]
    set Endoscopic(cam,rzStr,old) [lindex $or 2]
    set Endoscopic(cam,rxStr) [lindex $or 0]
    set Endoscopic(cam,ryStr) [lindex $or 1]
    set Endoscopic(cam,rzStr) [lindex $or 2]

    EndoscopicUpdateVirtualEndoscope
    EndoscopicCheckDriver 
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicSetGyroOrientation
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetGyroOrientation {} {
    global Endoscopic
    if {$Endoscopic(cam,axis) == "relative"} {
        vtkTransform tmp
    tmp SetMatrix [Endoscopic(cam,actor) GetMatrix] 
    eval Endoscopic(gyro,actor) SetOrientation [tmp GetOrientation]
    tmp Delete
    Endoscopic(cam,actor) SetOrientation 0 0 0
    
    } elseif {$Endoscopic(cam,axis) == "absolute"} {
    set or [Endoscopic(gyro,actor) GetOrientation]
    Endoscopic(gyro,actor) SetOrientation 0 0 0
    eval Endoscopic(cam,actor) SetOrientation $or
    }       
}
    
#-------------------------------------------------------------------------------
# .PROC EndoscopicSetWorldPosition
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetWorldPosition {x y z} {
    global Endoscopic


    # reset the sliders
    set Endoscopic(cam,xStr) $x
    set Endoscopic(cam,yStr) $y
    set Endoscopic(cam,zStr) $z
    set Endoscopic(cam,xStr,old) $x
    set Endoscopic(cam,yStr,old) $y
    set Endoscopic(cam,zStr,old) $z
    Endoscopic(gyro,actor) SetPosition $x $y $z
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetWorldOrientation
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetWorldOrientation {rx ry rz} {
    global Endoscopic


    # reset the sliders
    set Endoscopic(cam,rxStr) $rx
    set Endoscopic(cam,ryStr) $ry
    set Endoscopic(cam,rzStr) $rz
    set Endoscopic(cam,rxStr,old) $rx
    set Endoscopic(cam,ryStr,old) $ry
    set Endoscopic(cam,rzStr,old) $rz
    Endoscopic(gyro,actor) SetOrientation $rx $ry $rz
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicSetCameraPosition
#  This is called when the position sliders are updated. We use the values
#  stored in the slider variables to update the position of the endoscope 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetCameraPosition {{value ""}} {
    global Endoscopic View Endoscopic
    
    set collision 0
    
    # get the View plane of the virtual camera because we want to move 
    # in and out along that plane
    set l [$View(endCam) GetViewPlaneNormal]
    set IO(x) [expr -[lindex $l 0]]
    set IO(y) [expr -[lindex $l 1]] 
    set IO(z) [expr -[lindex $l 2]]
    
    
    # get the View up of the virtual camera because we want to move up
    # and down along that plane (and reverse it)
    set l [$View(endCam) GetViewUp]
    set Up(x) [lindex $l 0]
    set Up(y) [lindex $l 1]
    set Up(z) [lindex $l 2]
    
    
    # cross Up and IO to get the vector LR (to slide left and right)
    # LR = Up x IO

    #Cross LR Up IO 
    Cross LR IO Up 
    Normalize LR
    

    
    # if we want to go along the camera's own axis (Relative mode)
    # Endoscopic(cam,XXStr) is set by the slider
    # Endoscopic(cam,XXStr,old) is saved at the end of this proc
    
    # for the next time 
    if { $Endoscopic(cam,axis) == "relative" } {
    
    # this matrix tells us the current position of the cam actor
    set cam_mat [Endoscopic(cam,actor) GetMatrix]   
    # stepXX is the amount to move along axis XX
    set stepX [expr $Endoscopic(cam,xStr,old) - $Endoscopic(cam,xStr)]
    set stepY [expr $Endoscopic(cam,yStr,old) - $Endoscopic(cam,yStr)]
    set stepZ [expr $Endoscopic(cam,zStr,old) - $Endoscopic(cam,zStr)]
    
    set Endoscopic(cam,x) [expr [$cam_mat GetElement 0 3] + \
        $stepX*$LR(x) + $stepY * $IO(x) + $stepZ * $Up(x)] 
    set Endoscopic(cam,y) [expr  [$cam_mat GetElement 1 3] + \
        $stepX * $LR(y) + $stepY * $IO(y) + $stepZ * $Up(y)] 
    set Endoscopic(cam,z) [expr  [$cam_mat GetElement 2 3] + \
        $stepX * $LR(z) +  $stepY * $IO(z) +  $stepZ * $Up(z)] 
    
    } elseif { $Endoscopic(cam,axis) == "absolute" } {
    set Endoscopic(cam,x) $Endoscopic(cam,xStr)
    set Endoscopic(cam,y) $Endoscopic(cam,yStr)
    set Endoscopic(cam,z) $Endoscopic(cam,zStr)
    }
    
    # store current slider
    set Endoscopic(cam,xStr,old) $Endoscopic(cam,xStr)
    set Endoscopic(cam,yStr,old) $Endoscopic(cam,yStr)
    set Endoscopic(cam,zStr,old) $Endoscopic(cam,zStr)

    # set position of actor gyro (that will in turn set the position
    # of the camera and fp actor since their user matrix is linked to
    # the matrix of the gyro
    Endoscopic(gyro,actor) SetPosition $Endoscopic(cam,x) $Endoscopic(cam,y) $Endoscopic(cam,z)
    
    #################################
    # should not be needed anymore
    #Endoscopic(cam,actor) SetPosition $Endoscopic(cam,x) $Endoscopic(cam,y) $Endoscopic(cam,z)
    #Endoscopic(fp,actor) SetPosition $Endoscopic(fp,x) $Endoscopic(fp,y) $Endoscopic(fp,z)
    ##################################
    # set position of virtual camera
    EndoscopicUpdateVirtualEndoscope

    #*******************************************************************
    #
    # STEP 3: if the user decided to have the camera drive the slice, 
    #         then do it!
    #
    #*******************************************************************
    EndoscopicCheckDriver

    Render3D
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicResetCameraPosition
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicResetCameraPosition {} {
    global Endoscopic

    EndoscopicSetWorldPosition 0 0 0
    # in case the camera's model matrix is not the identity
    Endoscopic(cam,actor) SetPosition 0 0 0
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicSetCameraDirection
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetCameraDirection {{value ""}} {
    global Endoscopic View Model

    if {$Endoscopic(cam,axis) == "absolute"} {
    
    #Endoscopic(gyro,actor) SetOrientation $Endoscopic(cam,rxStr) $Endoscopic(cam,ryStr) $Endoscopic(cam,rzStr) 
    if {$value == "rx"} {
        set temprx [expr $Endoscopic(cam,rxStr) - $Endoscopic(cam,rxStr,old)]
        set Endoscopic(cam,rxStr,old) $Endoscopic(cam,rxStr)
        Endoscopic(gyro,actor) RotateWXYZ $temprx 1 0 0
    } elseif {$value == "ry"} {
        set tempry [expr $Endoscopic(cam,ryStr) - $Endoscopic(cam,ryStr,old)]
        set Endoscopic(cam,ryStr,old) $Endoscopic(cam,ryStr)
        Endoscopic(gyro,actor) RotateWXYZ $tempry 0 1 0
        #$View(endCam) Roll $tempry
    } elseif {$value == "rz"} {
        set temprz [expr $Endoscopic(cam,rzStr) - $Endoscopic(cam,rzStr,old)]
        set Endoscopic(cam,rzStr,old) $Endoscopic(cam,rzStr)
        Endoscopic(gyro,actor) RotateWXYZ $temprz 0 0 1
    }
    #eval Endoscopic(gyro,actor) SetOrientation [Endoscopic(gyro,actor) GetOrientation]        
    
    } elseif {$Endoscopic(cam,axis) == "relative"} {
    if {$value == "rx"} {
        set temprx [expr $Endoscopic(cam,rxStr) - $Endoscopic(cam,rxStr,old)]
        set Endoscopic(cam,rxStr,old) $Endoscopic(cam,rxStr)
        Endoscopic(gyro,actor) RotateX $temprx
    } elseif {$value == "ry"} {
        set tempry [expr $Endoscopic(cam,ryStr) - $Endoscopic(cam,ryStr,old)]
        set Endoscopic(cam,ryStr,old) $Endoscopic(cam,ryStr)
        Endoscopic(gyro,actor) RotateY $tempry
    } elseif {$value == "rz"} {
        set temprz [expr $Endoscopic(cam,rzStr) - $Endoscopic(cam,rzStr,old)]
        set Endoscopic(cam,rzStr,old) $Endoscopic(cam,rzStr)
        Endoscopic(gyro,actor) RotateZ $temprz
    }
    }
    
    #*******************************************************************
    #
    # if the user decided to have the camera drive the slice, 
    #         then do it!
    #
    #*******************************************************************
    
    EndoscopicCheckDriver   

    EndoscopicUpdateVirtualEndoscope
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicResetCameraDirection
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicResetCameraDirection {} {
    global Endoscopic 

    # we reset the rotation around the absolute y axis
    set Endoscopic(cam,yRotation) 0
    set Endoscopic(cam,viewUpX) 0
    set Endoscopic(cam,viewUpY) 0
    set Endoscopic(cam,viewUpZ) 1

    EndoscopicSetWorldOrientation 0 0 0 
    # in case the camera's model matrix is not the identity
    Endoscopic(cam,actor) SetOrientation 0 0 0
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateActorFromVirtualEndoscope
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateActorFromVirtualEndoscope {} {
    global Endoscopic View Path Model
        
    #*********************************************************************
    #
    # STEP 1: set the gyro matrix's orientation based on the virtual
    #         camera's matrix
    #         
    #*********************************************************************
    
    # this doesn't work because the virtual camera is rotated -90 degrees on 
    # the y axis originally, so the overall matrix is not correct (off by 90 degrees)
    #eval Endoscopic(gyro,actor) SetOrientation [$View(endCam) GetOrientation]
    #eval Endoscopic(gyro,actor) SetPosition [$View(endCam) GetPosition]

    # so build the matrix ourselves instead, that way we are sure about it
    vtkMatrix4x4 matrix
   
    # Uy = ViewPlaneNormal
    set Uy(x) $Endoscopic(cam,viewPlaneNormalX)
    set Uy(y) $Endoscopic(cam,viewPlaneNormalY)
    set Uy(z) $Endoscopic(cam,viewPlaneNormalZ)
    # Uz = ViewUp
    set Uz(x) $Endoscopic(cam,viewUpX)
    set Uz(y) $Endoscopic(cam,viewUpY)
    set Uz(z) $Endoscopic(cam,viewUpZ)
    # Ux = Uy x Uz
    set Ux(x) [expr $Uy(y)*$Uz(z) - $Uz(y)*$Uy(z)]
    set Ux(y) [expr $Uz(x)*$Uy(z) - $Uy(x)*$Uz(z)]
    set Ux(z) [expr $Uy(x)*$Uz(y) - $Uz(x)*$Uy(y)]
    
    #Ux
    matrix SetElement 0 0 $Ux(x)
    matrix SetElement 1 0 $Ux(y)
    matrix SetElement 2 0 $Ux(z)
    matrix SetElement 3 0 0
    # Uy
    matrix SetElement 0 1 $Uy(x)
    matrix SetElement 1 1 $Uy(y)
    matrix SetElement 2 1 $Uy(z)
    matrix SetElement 3 1 0
    # Uz
    matrix SetElement 0 2 $Uz(x)
    matrix SetElement 1 2 $Uz(y)
    matrix SetElement 2 2 $Uz(z)
    matrix SetElement 3 2 0
    # Right column (position)
    matrix SetElement 0 3 0
    matrix SetElement 1 3 0
    matrix SetElement 2 3 0
    matrix SetElement 3 3 1

    vtkTransform transform
    transform SetMatrix matrix
    set orientation [transform GetOrientation]
    set position [$View(endCam) GetPosition]
    eval Endoscopic(gyro,actor) SetOrientation $orientation
    eval Endoscopic(gyro,actor) SetPosition $position
    transform Delete
    matrix Delete

    # set the sliders
    set Endoscopic(cam,xStr) [lindex $position 0]
    set Endoscopic(cam,yStr) [lindex $position 1]
    set Endoscopic(cam,zStr) [lindex $position 2]
    set Endoscopic(cam,xStr,old) [lindex $position 0]
    set Endoscopic(cam,yStr,old) [lindex $position 1]
    set Endoscopic(cam,zStr,old) [lindex $position 2]

    set Endoscopic(cam,rxStr) [lindex $orientation 0]
    set Endoscopic(cam,ryStr) [lindex $orientation 1]
    set Endoscopic(cam,rzStr) [lindex $orientation 2]
    set Endoscopic(cam,rxStr,old) [lindex $orientation 0]
    set Endoscopic(cam,ryStr,old) [lindex $orientation 1]
    set Endoscopic(cam,rzStr,old) [lindex $orientation 2]
    
    #*******************************************************************
    #
    # STEP 2: if the user decided to have the camera drive the slice, 
    #         then do it!
    #
    #*******************************************************************

    EndoscopicCheckDriver

}

#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateVirtualEndoscope
#       Updates the virtual camera's position, orientation and view angle
#       Calls EndoscopicLightFollowsEndoCamera
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateVirtualEndoscope {{coordList ""}} {
    global Endoscopic Model View Path

    
    if {$coordList != "" && [llength $coordList] == 6} {
    # COORDLIST IS NOT EMPTY IF WE WANT TO SET THE VIRTUAL CAMERA ONLY
    # BASED ON INFORMATION ABOUT THE POSITION OF THE ACTOR CAMERA AND
    # ACTOR FOCAL POINT
    # we only have information about the position of the camera and the 
    # focal point. Extrapolate the additional information from that 
    $View(endCam) SetPosition [lindex $coordList 0] [lindex $coordList 1] [lindex $coordList 2]
    $View(endCam) SetFocalPoint [lindex $coordList 3] [lindex $coordList 4] [lindex $coordList 5] 
    # use prior information to prevent the View from flipping at undefined
    # boundary points (i.e when the viewUp and the viewPlaneNormal are 
    # parallel, OrthogonalizeViewUp sometimes produces a viewUp that 
    # flips direction 
    $View(endCam) SetViewUp $Endoscopic(cam,viewUpX) $Endoscopic(cam,viewUpY) $Endoscopic(cam,viewUpZ)    
    
    } elseif {$coordList == ""} {
    # COORDLIST IS EMPTY IF WE JUST WANT THE VIRTUAL CAMERA TO MIMICK
    # THE CURRENT ACTOR CAMERA
    # we want the virtual camera to be in the same position/orientation 
    # than the endoscope and we have all the information we need
    # so set the position, focal point, and view up (the z unit vector of 
    # the camera actor's orientation [the 3rd column of its world matrix])
    set cam_mat [Endoscopic(cam,actor) GetMatrix]   
    $View(endCam) SetPosition [$cam_mat GetElement 0 3] [$cam_mat GetElement 1 3] [$cam_mat GetElement 2 3]     
    set fp_mat [Endoscopic(fp,actor) GetMatrix]
    $View(endCam) SetFocalPoint [$fp_mat GetElement 0 3] [$fp_mat GetElement 1 3] [$fp_mat GetElement 2 3] 
    $View(endCam) SetViewUp [$cam_mat GetElement 0 2] [$cam_mat GetElement 1 2] [$cam_mat GetElement 2 2]         
    
    }
    $View(endCam) ComputeViewPlaneNormal        
    $View(endCam) OrthogonalizeViewUp
    # save the current view Up
    set l [$View(endCam) GetViewUp]
    set Endoscopic(cam,viewUpX) [expr [lindex $l 0]]
    set Endoscopic(cam,viewUpY) [expr [lindex $l 1]] 
    set Endoscopic(cam,viewUpZ) [expr [lindex $l 2]]

    # save the current view Plane
    set l [$View(endCam) GetViewPlaneNormal]
    set Endoscopic(cam,viewPlaneNormalX) [expr -[lindex $l 0]]
    set Endoscopic(cam,viewPlaneNormalY) [expr -[lindex $l 1]] 
    set Endoscopic(cam,viewPlaneNormalZ) [expr -[lindex $l 2]]
    
    EndoscopicSetCameraViewAngle
    eval $View(endCam) SetClippingRange $View(endoscopicClippingRange)    
    EndoscopicLightFollowEndoCamera
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicLightFollowEndoCamera
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicLightFollowEndoCamera {} {
    global View
    
    # 3D Viewer
    
    set endoLights [endoscopicRen GetLights]
    
    $endoLights InitTraversal
    set endoCurrentLight [$endoLights GetNextItem]
    if {$endoCurrentLight != ""} {
    eval $endoCurrentLight SetPosition [$View(endCam) GetPosition]
    eval $endoCurrentLight SetIntensity 1
    eval $endoCurrentLight SetFocalPoint [$View(endCam) GetFocalPoint]
    }
    set endoCurrentLight [$endoLights GetNextItem]
    if {$endoCurrentLight != ""} {
    eval $endoCurrentLight SetFocalPoint [$View(endCam) GetPosition]
    eval $endoCurrentLight SetIntensity 1
    eval $endoCurrentLight SetConeAngle 180    
    eval $endoCurrentLight SetPosition [$View(endCam) GetFocalPoint]
    }

}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetCameraZoom
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetCameraZoom {} {
    global Endoscopic Model View
    
    # get the View plane of the virtual camera because we want to move 
    # in and out along that plane
    set l [$View(endCam) GetViewPlaneNormal]
    set IO(x) [expr -[lindex $l 0]]
    set IO(y) [expr -[lindex $l 1]] 
    set IO(z) [expr -[lindex $l 2]]

    set Endoscopic(fp,distance) $Endoscopic(cam,zoomStr)

    # now move the fp a percentage of its distance to the camera
    set Endoscopic(cam,x) [expr $Endoscopic(fp,x) - $IO(x) * $Endoscopic(fp,distance)]
    set Endoscopic(cam,y) [expr $Endoscopic(fp,y) - $IO(y) * $Endoscopic(fp,distance)]
    set Endoscopic(cam,z) [expr $Endoscopic(fp,z) - $IO(z) * $Endoscopic(fp,distance)]
    
    Endoscopic(cam,actor) SetPosition $Endoscopic(cam,x) $Endoscopic(cam,y) $Endoscopic(cam,z)
    Endoscopic(gyro,actor) SetPosition $Endoscopic(cam,x) $Endoscopic(cam,y) $Endoscopic(cam,z)
    EndoscopicUpdateVirtualEndoscope
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetCameraViewAngle
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetCameraViewAngle {} {
    global Endoscopic Model View

    set Endoscopic(cam,viewAngle) $Endoscopic(cam,AngleStr)
    $View(endCam) SetViewAngle $Endoscopic(cam,viewAngle)
    
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetCameraAxis
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetCameraAxis {{axis ""}} {
    global Endoscopic Model
    
    if {$axis != ""} {
    if {$axis == "absolute" || $axis == "relative"} {
        set Endoscopic(cam,axis) $axis
        
        # Change button text
        #$Endoscopic(axis) config -text $axis

        
        # update the actual camera position for the slider

        if {$axis == "relative"} {
        $Endoscopic(labelx) configure -text "Left/Right"
        $Endoscopic(labely) configure -text "Forw/Back"
        $Endoscopic(labelz) configure -text "Up/Down"        

        } elseif {$axis == "absolute"} {
        $Endoscopic(labelx) configure -text "L<->R "
        $Endoscopic(labely) configure -text "P<->A "
        $Endoscopic(labelz) configure -text "I<->S "
        }
        set l [$Endoscopic(gyro,actor) GetPosition]
        set Endoscopic(cam,xStr) [expr [lindex $l 0]]
        set Endoscopic(cam,yStr) [expr [lindex $l 1]]
        set Endoscopic(cam,zStr) [expr [lindex $l 2]]
        
        set Endoscopic(cam,xStr,old) [expr [lindex $l 0]]
        set Endoscopic(cam,yStr,old) [expr [lindex $l 1]]
        set Endoscopic(cam,zStr,old) [expr [lindex $l 2]]
        
        $Endoscopic(sliderx) set $Endoscopic(cam,xStr)
        $Endoscopic(slidery) set $Endoscopic(cam,yStr)
        $Endoscopic(sliderz) set $Endoscopic(cam,zStr)
        
        
        vtkTransform tmp
        tmp SetMatrix [$Endoscopic(gyro,actor) GetMatrix] 
        set l [tmp GetOrientation]
        tmp Delete
        set Endoscopic(cam,rxStr) [expr [lindex $l 0]]
        set Endoscopic(cam,ryStr) [expr [lindex $l 1]]
        set Endoscopic(cam,rzStr) [expr [lindex $l 2]]
        
        set Endoscopic(cam,rxStr,old) [expr [lindex $l 0]]
        set Endoscopic(cam,ryStr,old) [expr [lindex $l 1]]
        set Endoscopic(cam,rzStr,old) [expr [lindex $l 2]]
        
        $Endoscopic(sliderrx) set $Endoscopic(cam,rxStr)
        $Endoscopic(sliderry) set $Endoscopic(cam,ryStr)
        $Endoscopic(sliderrz) set $Endoscopic(cam,rzStr)
    
    } else {
        return
    }   
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicCameraMotionFromUser
#
# called whenever the active camera is moved. This routine syncs the position of
# the graphical endoscopic camera with the virtual endoscopic camera
# (i.e if the user changes the view of the endoscopic window with the mouse,
#  we want to change the position of the graphical camera)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicCameraMotionFromUser {} {
    
    global View Endoscopic
    global CurrentCamera 
    
    if {$CurrentCamera == $View(endCam)} {
    EndoscopicUpdateActorFromVirtualEndoscope 
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicSetCollision
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetCollision {value} {
    global Endoscopic

    set Endoscopic(collision) $value
    if { $value == 0 } {
    $Endoscopic(collMenu) config -text "off"
    } else {
    $Endoscopic(collMenu) config -text "on"
    }
}


############################################################################
#
#       PART 5: Vector Operation
#
###########################################################################

#proc EndoscopicMoveGyroToVector {id} {
#    
#    global Endoscopic 
#    set xyz [Endoscopic(cLand,graphicalInterpolatedPoints) GetPoint $id]
#    set Endoscopic(cam,xStr) [lindex $xyz 0]
#    set Endoscopic(cam,yStr) [lindex $xyz 1]
#    set Endoscopic(cam,zStr) [lindex $xyz 2]
#    EndoscopicSetCameraPosition
#}

#-------------------------------------------------------------------------------
# .PROC EndoscopicMoveGyroToLandmark
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicMoveGyroToLandmark {id} {
    
    global Endoscopic 
    set xyz [Endoscopic(cLand,keyPoints) GetPoint $id]
    set rxyz [Endoscopic(fLand,keyPoints) GetPoint $id]
    
    EndoscopicUpdateVirtualEndoscope "[lindex $xyz 0] [lindex $xyz 1] [lindex $xyz 2] [lindex $rxyz 0] [lindex $rxyz 1] [lindex $rxyz 2]" 
    EndoscopicUpdateActorFromVirtualEndoscope
}

#proc EndoscopicRotateVector {} {
#    global Endoscopic 
    
    # change the vector direction based on the current camera orientation

#   set rxyz [Endoscopic(cam,actor) GetOrientation]
#    Endoscopic(vector,vectors) SetVector $Endoscopic(vector,selectedID) [lindex $rxyz 0] [lindex $rxyz 1] [lindex $rxyz 2] 
#    Endoscopic(vector,polyData) Modified
#    Render3D
#}

#proc EndoscopicRotateVector {} {
#
#    global CurrentCamera 
#    global LastX 
#    global RendererFound
#    global View Module Endoscopic
#
#    set axis $Endoscopic(cam,movementAxis)
#    
#    if { $axis != "" } {
#    
#    if { ! $RendererFound } { return }
#    if {[info exists Module(Endoscopic,procEnter)] == 1} {
#        
#        set Endoscopic(cam,r${axis}Str) [expr $tmp + ($LastX - $x)]
#        EndoscopicSetCameraDirection "r${axis}"
#        set list [Endoscopic(cam,actor) GetOrientation]
#        Endoscopic(gyro,actor) SetOrientation [lindex $list 0] [lindex $list 1] [lindex $list 2]
#        Render3D
#        set LastX $x
#        }
#    } else {
#    tk_messageBox -message "No axis selected. Please select an axis with the mouse and press the key 's' "
#    }
#}



#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateVectors
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateVectors {} {
        
    global Endoscopic
    set numPoints [Endoscopic(cLand,keyPoints) GetNumberOfPoints]
    for {set i 0} {$i < $numPoints} {incr i} {
    set cp [Endoscopic(cLand,keyPoints) GetPoint $i]
    set fp [Endoscopic(fLand,keyPoints) GetPoint $i]
    
    set cpX [lindex $cp 0]
    set cpY [lindex $cp 1]
    set cpZ [lindex $cp 2]
    
    set fpX [lindex $fp 0]
    set fpY [lindex $fp 1]
    set fpZ [lindex $fp 2]
    
    Endoscopic(vector,vectors) InsertVector $i [expr $fpX - $cpX] [expr $fpY - $cpY] [expr $fpZ - $cpZ]
    
    Endoscopic(vector,scalars) InsertScalar $i .5 
    Endoscopic(vector,polyData) Modified
    }    
}

#############################################################################
#
#     PART 6: Landmark Operations
#
#############################################################################

#-------------------------------------------------------------------------------
# .PROC EndoscopicAddLandmarkNoDirectionSpecified
#
# this procedure is called when the user adds a landmark at position i 
# on a slice or on a model and we don't know yet what direction of view we 
# should save along with the landmark. There are 2 cases:
#  i = 1 => the direction vector is [0 1 0]
#  i > 1 => The direction vector is tangential to the curve 
# [(position of landmark i - 1) - (position of last interpolated point on the path]
# 
# The user can then change the direction vector interactively through the 
# user interface.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddLandmarkNoDirectionSpecified {x y z} {

    global Endoscopic Model Path


    set numLandmarks [Endoscopic(cLand,keyPoints) GetNumberOfPoints]
    set i $numLandmarks

    #if {$numLandmarks == 1} {
#    
#    viewRen AddActor Endoscopic(cLand,actor)
#    viewRen AddActor Endoscopic(fLand,actor)
#    }

    set Endoscopic(cLand,$i,x) [expr $x]
    set Endoscopic(cLand,$i,y) [expr $y]
    set Endoscopic(cLand,$i,z) [expr $z]
    # this is now the direction of the vector
    # if i = 0, give it the default direction 0 1 0
    # else if i > 0, give it the tangential direction
    if { $i == 0 } {
    set Endoscopic(fLand,$i,x) [expr $x]
    set Endoscopic(fLand,$i,y) [expr $y + 1]
    set Endoscopic(fLand,$i,z) [expr $z]
    } else {
    
    set prev [expr $i - 1]
    set prevX $Endoscopic(fLand,$prev,x)
    set prevY $Endoscopic(fLand,$prev,y)
    set prevZ $Endoscopic(fLand,$prev,z)
    
    set d(x) [expr ($x - $prevX)]
    set d(y) [expr ($y - $prevY)]
    set d(z) [expr ($z - $prevZ)]
    
    Normalize d
    
    set Endoscopic(fLand,$i,x) [expr $x + $d(x)]
    set Endoscopic(fLand,$i,y) [expr $y + $d(y)]
    set Endoscopic(fLand,$i,z) [expr $z + $d(z)]
    }
      
    # add the new landmark position at the end of the scrollable list
    $Endoscopic(path,fLandmarkList) insert end "$Endoscopic(cLand,$i,x) $Endoscopic(cLand,$i,y) $Endoscopic(cLand,$i,z)"
    
    # update the Mrml tree
    EndoscopicAddToEndOfMRMLTree $i $Endoscopic(cLand,$i,x) $Endoscopic(cLand,$i,y) $Endoscopic(cLand,$i,z) $Endoscopic(fLand,$i,x) $Endoscopic(fLand,$i,y) $Endoscopic(fLand,$i,z)
    MainUpdateMRML

    # update vtk
    foreach m "c f" {
    Endoscopic(${m}Land,aSplineX) AddPoint $i $Endoscopic(${m}Land,$i,x)
    Endoscopic(${m}Land,aSplineY) AddPoint $i $Endoscopic(${m}Land,$i,y)
    Endoscopic(${m}Land,aSplineZ) AddPoint $i $Endoscopic(${m}Land,$i,z)
    Endoscopic(${m}Land,keyPoints) InsertPoint $i $Endoscopic(${m}Land,$i,x) $Endoscopic(${m}Land,$i,y) $Endoscopic(${m}Land,$i,z)
    Endoscopic(${m}Land,scalars) InsertScalar $i 0.2
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicAddLandmarkDirectionSpecified
#
# This procedure is called when we want to add a landmark at position i and 
# we know that the direction of view to save along with the landmark is the
# current view direction of the endoscope
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddLandmarkDirectionSpecified {} {

    global Endoscopic Model Path View Point EndPath Landmark

    set numLandmarks [Endoscopic(cLand,keyPoints) GetNumberOfPoints]
    set i $numLandmarks
    
#    if {$numLandmarks == 1} {
#    viewRen AddActor Endoscopic(cLand,actor)
#    viewRen AddActor Endoscopic(fLand,actor)
#    }

    # cLand and fLand are set to the camera and focal point position by 
    # default, or explicitely in EndoscopicSetLandmark

    set Endoscopic(cLand,$i,x) $Endoscopic(cam,x)
    set Endoscopic(cLand,$i,y) $Endoscopic(cam,y)
    set Endoscopic(cLand,$i,z) $Endoscopic(cam,z)
    set Endoscopic(fLand,$i,x) $Endoscopic(fp,x)
    set Endoscopic(fLand,$i,y) $Endoscopic(fp,y)
    set Endoscopic(fLand,$i,z) $Endoscopic(fp,z)

    # add the new landmark position at the end of the scrollable list
    $Endoscopic(path,fLandmarkList) insert end "$Endoscopic(cLand,$i,x) $Endoscopic(cLand,$i,y) $Endoscopic(cLand,$i,z)"
    #incr Endoscopic(path,numLandmarks)
    
    # update the Mrml tree
    EndoscopicAddToEndOfMRMLTree $i $Endoscopic(cLand,$i,x) $Endoscopic(cLand,$i,y) $Endoscopic(cLand,$i,z) $Endoscopic(fLand,$i,x) $Endoscopic(fLand,$i,y) $Endoscopic(fLand,$i,z)
    MainUpdateMRML

    # update vtk
    foreach m "c f" {
    Endoscopic(${m}Land,aSplineX) AddPoint $i $Endoscopic(${m}Land,$i,x)
    Endoscopic(${m}Land,aSplineY) AddPoint $i $Endoscopic(${m}Land,$i,y)
    Endoscopic(${m}Land,aSplineZ) AddPoint $i $Endoscopic(${m}Land,$i,z)
    Endoscopic(${m}Land,keyPoints) InsertPoint $i $Endoscopic(${m}Land,$i,x) $Endoscopic(${m}Land,$i,y) $Endoscopic(${m}Land,$i,z)
    Endoscopic(${m}Land,scalars) InsertScalar $i 0.2
    }
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateLandmark
#
# This procedure is called when we want to update the current selected  landmark to the current position and orientation of the camera actor
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateLandmark {} {

    global Endoscopic Model Path View Point EndPath Landmark
    
    set numLandmarks [Endoscopic(cLand,keyPoints) GetNumberOfPoints]
    if { $numLandmarks < 0} {
    puts "we have a problem!!! numLandmarks < 0"
    }
    set i $Endoscopic(cLand,selectedID)
    
    # cLand is set to the current gyro position

    set l [Endoscopic(cam,actor) GetPosition]
    set Endoscopic(cLand,$i,x) [lindex $l 0]
    set Endoscopic(cLand,$i,y) [lindex $l 1]
    set Endoscopic(cLand,$i,z) [lindex $l 2]

    set l [Endoscopic(fp,actor) GetPosition]
    set Endoscopic(fLand,$i,x) [lindex $l 0]
    set Endoscopic(fLand,$i,y) [lindex $l 1]
    set Endoscopic(fLand,$i,z) [lindex $l 2]

    # update the new landmark position at in the scrollable list
    $Endoscopic(path,fLandmarkList) delete $i 
    $Endoscopic(path,fLandmarkList) insert $i "$Endoscopic(cLand,$i,x) $Endoscopic(cLand,$i,y) $Endoscopic(cLand,$i,z)" 
    EndoscopicUpdateSelectionLandmarkList $i

    # update the Mrml tree
    EndoscopicUpdateInMRMLTree $i $Endoscopic(cLand,$i,x) $Endoscopic(cLand,$i,y) $Endoscopic(cLand,$i,z) $Endoscopic(fLand,$i,x) $Endoscopic(fLand,$i,y) $Endoscopic(fLand,$i,z)
    MainUpdateMRML

    foreach m {c f} {
    
    # we want to update item d
    Endoscopic(${m}Land,aSplineX) RemovePoint $i 
    Endoscopic(${m}Land,aSplineY) RemovePoint $i 
    Endoscopic(${m}Land,aSplineZ) RemovePoint $i 

    Endoscopic(${m}Land,aSplineX) AddPoint $i $Endoscopic(${m}Land,$i,x)
    Endoscopic(${m}Land,aSplineY) AddPoint $i $Endoscopic(${m}Land,$i,y)
    Endoscopic(${m}Land,aSplineZ) AddPoint $i $Endoscopic(${m}Land,$i,z)
    
    Endoscopic(${m}Land,keyPoints) SetPoint $i $Endoscopic(${m}Land,$i,x) $Endoscopic(${m}Land,$i,y) $Endoscopic(${m}Land,$i,z)
    Endoscopic(${m}Land,scalars) SetScalar $i 0.2
    
    }

    EndoscopicBuildInterpolatedPath 
    Render3D
}
    

    

#-------------------------------------------------------------------------------
# .PROC EndoscopicDeleteLandmark
#
# 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicDeleteLandmark {} {
    global Endoscopic Model View Path EndPath Landmark Point View

    # delete the curSelection and shift the indices of the points

    set numLandmarks [Endoscopic(cLand,keyPoints) GetNumberOfPoints]
    set d [expr [$Endoscopic(path,fLandmarkList) curselection]]
    if { $d == "" } {
    tk_messageBox -message "You have not selected a landmark from the list"
    
    } else {
    if {$numLandmarks <= 1} {
        EndoscopicDeletePath
    } else {
        set numLandmarks [expr $numLandmarks - 1]
        $Endoscopic(path,fLandmarkList) delete $d
        
        MainMrmlDeleteNode Landmark [$Endoscopic(cam,$d,LandmarkNode) GetID]
        
        for {set i $d} {$i<$numLandmarks} {incr i 1} {
        
        set Endoscopic(cLand,$i,x) $Endoscopic(cLand,[expr $i + 1],x)
        set Endoscopic(cLand,$i,y) $Endoscopic(cLand,[expr $i + 1],y)
        set Endoscopic(cLand,$i,z) $Endoscopic(cLand,[expr $i + 1],z)
        
        set Endoscopic(fLand,$i,x) $Endoscopic(fLand,[expr $i + 1],x)
        set Endoscopic(fLand,$i,y) $Endoscopic(fLand,[expr $i + 1],y)
        set Endoscopic(fLand,$i,z) $Endoscopic(fLand,[expr $i + 1],z)
        
        set Endoscopic(cam,$i,LandmarkNode) $Endoscopic(cam,[expr $i + 1],LandmarkNode)
        $Endoscopic(cam,$i,LandmarkNode) SetPathPosition $i
        }
        
        
        MainUpdateMRML
        
        foreach m {c f} {

        # we want to remove item d, so rebuild the whole point set
        Endoscopic(${m}Land,keyPoints) Reset
        set numberOfkeyPoints $numLandmarks

        # clear the splines
        Endoscopic(${m}Land,aSplineX) RemoveAllPoints
        Endoscopic(${m}Land,aSplineY) RemoveAllPoints
        Endoscopic(${m}Land,aSplineZ) RemoveAllPoints
        
        for {set i 0} {$i<$numberOfkeyPoints} {incr i 1} {
            Endoscopic(${m}Land,aSplineX) AddPoint $i $Endoscopic(${m}Land,$i,x)
            Endoscopic(${m}Land,aSplineY) AddPoint $i $Endoscopic(${m}Land,$i,y)
            Endoscopic(${m}Land,aSplineZ) AddPoint $i $Endoscopic(${m}Land,$i,z)
            Endoscopic(${m}Land,keyPoints) InsertPoint $i $Endoscopic(${m}Land,$i,x) $Endoscopic(${m}Land,$i,y) $Endoscopic(${m}Land,$i,z)
            Endoscopic(${m}Land,scalars) InsertScalar $i 0.2
        } 
        
        }
        EndoscopicBuildInterpolatedPath 
    }
    }
}



#############################################################################
#
#    PART 7: Path Operation
#
############################################################################


#-------------------------------------------------------------------------------
# .PROC EndoscopicBuildInterpolatedPath
#
# This procedure creates a new path model by:
# creating a path containing all landmarks 
#    from i = 0 to i = # of points added with EndoscopicAddLandmark*
#
# It is much faster to create a path by iteratively calling 
# EndoscopicAddLandmark* for each new landmark -> EndoscopicBuildInterpolatedPath
# 
# then by iteratively calling
# (EndoscopicAddLandmark* -> EndoscopicBuildInterpolatedPath) for each new landmark
#
# but the advantage of the latter is that the user can see the path being
# created iteratively (the former is used when loading mrml paths).
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicBuildInterpolatedPath {} {
    
    global Endoscopic Model Path View Point EndPath Landmark
    
    
    set numLandmarks [Endoscopic(cLand,keyPoints) GetNumberOfPoints]
    set i $numLandmarks

    # only build the path if there are at least 2 landmarks
    if { $numLandmarks > 1 } {

    foreach m {c f} {
    # reset the vtk objects

        Endoscopic(${m}Land,graphicalInterpolatedPoints) Reset
        Endoscopic(${m}Land,allInterpolatedPoints) Reset
        Endoscopic(${m}Path,lines) Reset 
        Endoscopic(${m}Path,polyData) Modified
        
        #evaluate the fist point of the spline (0)    
        Endoscopic(${m}Land,graphicalInterpolatedPoints) InsertPoint 0 [Endoscopic(${m}Land,aSplineX) Evaluate 0] [Endoscopic(${m}Land,aSplineY) Evaluate 0] [Endoscopic(${m}Land,aSplineZ) Evaluate 0]

        Endoscopic(${m}Land,allInterpolatedPoints) InsertPoint 0 [Endoscopic(${m}Land,aSplineX) Evaluate 0] [Endoscopic(${m}Land,aSplineY) Evaluate 0] [Endoscopic(${m}Land,aSplineZ) Evaluate 0]

    }
    
    
    set numberOfkeyPoints [Endoscopic(cLand,keyPoints) GetNumberOfPoints]
    
    # now build the rest of the spline
    for {set i 0} {$i< [expr $numberOfkeyPoints - 1]} {incr i 1} {
        
        set pci [Endoscopic(cLand,keyPoints) GetPoint $i]
        set pcni [Endoscopic(cLand,keyPoints) GetPoint [expr $i+1]]
        set pfi [Endoscopic(fLand,keyPoints) GetPoint $i]
        set pfni [Endoscopic(fLand,keyPoints) GetPoint [expr $i+1]]

        # calculate the distance di between key point i and i+1 for both 
        # the camera path and fp path, keep the highest one

        set Endoscopic(cLand,$i,distanceToNextPoint) [eval EndoscopicDistanceBetweenTwoPoints $pci $pcni]
        set Endoscopic(fLand,$i,distanceToNextPoint) [eval EndoscopicDistanceBetweenTwoPoints $pfi $pfni]

        #puts "c $dci f $dfi"

        if {$Endoscopic(cLand,$i,distanceToNextPoint) >= $Endoscopic(fLand,$i,distanceToNextPoint)} {
        set di $Endoscopic(cLand,$i,distanceToNextPoint)
        } else {
        set di $Endoscopic(fLand,$i,distanceToNextPoint)
        }
        
        # take into account the interpolation factor
        
        set di [expr $di * $Endoscopic(path,interpolationStr)]
        
        # evaluate the spline di times start after i, finish at i+1
        for {set j 1} {$j<=$di} {incr j 1} {

        set t [expr $i + ($j/$di)]

        foreach m "c f" {
            # add the points for the graphical lines
            if {$Endoscopic(${m}Land,$i,distanceToNextPoint) !=0} {
            set numPoints [Endoscopic(${m}Land,graphicalInterpolatedPoints) GetNumberOfPoints]
            Endoscopic(${m}Land,graphicalInterpolatedPoints) InsertPoint [expr $numPoints] [Endoscopic(${m}Land,aSplineX) Evaluate $t] [Endoscopic(${m}Land,aSplineY) Evaluate $t] [Endoscopic(${m}Land,aSplineZ) Evaluate $t]
            }
            
            # add the points for the landmark record
            set numPoints [Endoscopic(${m}Land,allInterpolatedPoints) GetNumberOfPoints]
            Endoscopic(${m}Land,allInterpolatedPoints) InsertPoint [expr $numPoints] [Endoscopic(${m}Land,aSplineX) Evaluate $t] [Endoscopic(${m}Land,aSplineY) Evaluate $t] [Endoscopic(${m}Land,aSplineZ) Evaluate $t]
        }
        }
    }
    
    set numberOfOutputPoints [Endoscopic(cLand,allInterpolatedPoints) GetNumberOfPoints]
    set Endoscopic(path,exists) 1

    # since that is where the camera is

    set Endoscopic(path,stepStr) [expr $numberOfOutputPoints - 1]
    
    $Endoscopic(path,stepScale) config -to [expr $numberOfOutputPoints - 1]
    

    # add cell data
    foreach m "c f" {
        set numberOfOutputPoints [Endoscopic(${m}Land,graphicalInterpolatedPoints) GetNumberOfPoints]
        Endoscopic(${m}Path,lines) InsertNextCell $numberOfOutputPoints
        for {set i 0} {$i< $numberOfOutputPoints} {incr i 1} {
        Endoscopic(${m}Path,lines) InsertCellPoint $i
        }
    }

    Endoscopic(fLand,polyData) Modified
    Endoscopic(fLand,polyData) Modified
    # now update the vectors
    EndoscopicUpdateVectors
    }
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicDeletePath
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicDeletePath {} {
    global Endoscopic Path Point Model Landmark EndPath


    set numLandmarks [Endoscopic(cLand,keyPoints) GetNumberOfPoints]
    
    if { $Endoscopic(path,exists) == 1 } {
    
    for {set d 0} {$d<$numLandmarks} {incr d 1} {
        MainMrmlDeleteNode Landmark [$Endoscopic(cam,$d,LandmarkNode) GetID]       
    }
    
    if { $Endoscopic(cam,PathNode) != ""} {
        MainMrmlDeleteNode Path [$Endoscopic(cam,PathNode) GetID]
        set Endoscopic(cam,PathNode) ""
    }
    if { $Endoscopic(cam,EndPathNode) != ""} {
        MainMrmlDeleteNode EndPath [$Endoscopic(cam,EndPathNode) GetID]
        set Endoscopic(cam,EndPathNode) ""
    }
    
    MainUpdateMRML

    EndoscopicResetPathVariables
    }
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicResetPathVariables
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicResetPathVariables {} {
    global Endoscopic View
    
    # reset the pointers to the path nodes
    set Endoscopic(cam,PathNode) ""
    set Endoscopic(cam,EndPathNode) ""
    
    # clear the scroll text
    $Endoscopic(path,fLandmarkList) delete 0 end
    
    # reset the path variables
    set Endoscopic(path,exists) 0
    set Endoscopic(path,showPath) 0
    set Endoscopic(path,random) 0
    
    # get rid of path actors
    #viewRen RemoveActor Endoscopic(cLand,actor)
    #viewRen RemoveActor Endoscopic(fLand,actor)
    #viewRen RemoveActor Endoscopic(cPath,actor)
    #viewRen RemoveActor Endoscopic(fPath,actor)    
    #viewRen RemoveActor Endoscopic(vector,actor)
    
    foreach m {c f} {
    Endoscopic(${m}Land,aSplineX) RemoveAllPoints
    Endoscopic(${m}Land,aSplineY) RemoveAllPoints
    Endoscopic(${m}Land,aSplineZ) RemoveAllPoints
    Endoscopic(${m}Land,keyPoints) Reset
    Endoscopic(${m}Land,graphicalInterpolatedPoints) Reset
    Endoscopic(${m}Land,allInterpolatedPoints) Reset
    Endoscopic(${m}Path,lines) Reset 
    Endoscopic(${m}Land,polyData) Modified
    Endoscopic(${m}Path,polyData) Modified
    Endoscopic(vector,polyData) Modified
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicComputeRandomPath
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicComputeRandomPath {} {
    global Endoscopic View Path Model Landmark EndPath Path
       
    if { $Endoscopic(path,exists) == 1} {
    EndoscopicDeletePath
    }
    
    for {set i 0} {$i<20} {incr i 1} {
    set x  [expr [Endoscopic(path,vtkmath) Random -1 1] * 100]
    set y  [expr [Endoscopic(path,vtkmath) Random -1 1] * 100]
    set z  [expr [Endoscopic(path,vtkmath) Random -1 1] * 100]
    EndoscopicAddLandmarkNoDirectionSpecified $x $y $z
    
    }
    set Endoscopic(path,random) 1   
    
    EndoscopicBuildInterpolatedPath
    
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicShowPath
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicShowPath {} {
    global Path Endoscopic

    if {$Endoscopic(path,exists) == 1} {
    if {$Endoscopic(path,showPath) == 1} {
        foreach m {c f} {
        endoscopicRen AddActor Endoscopic(${m}Land,actor)
        endoscopicRen AddActor Endoscopic(${m}Path,actor)
        }
    } else {
        foreach m {c f} {
        endoscopicRen RemoveActor Endoscopic(${m}Land,actor)
        endoscopicRen RemoveActor Endoscopic(${m}Path,actor)
        }
    }
    }

}

#-------------------------------------------------------------------------------
# .PROC EndoscopicViewPath
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicViewPath {} {
    global Endoscopic Model View Path Module
    
    set numPoints [Endoscopic(cLand,allInterpolatedPoints) GetNumberOfPoints]
    if {$Endoscopic(path,random) == 1} {
    set var 4
    } else {
    set var 0
    }
    set u 0
    

    for {set Endoscopic(path,i) $Endoscopic(path,stepStr)} {$Endoscopic(path,i)< $numPoints - $var} {incr Endoscopic(path,i) $Endoscopic(path,speed)} { 
    if {$Endoscopic(path,stop) == "0"} {
        set Endoscopic(path,stepStr) $Endoscopic(path,i)
        EndoscopicSetPathFrame
            update            
        Render3D    
    } else {    
        EndoscopicResetStopPath
        
        break
    }    
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicStopPath
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicStopPath {} {
    global Path Endoscopic
        
    set Endoscopic(path,stop) 1
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicResetStopPath
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicResetStopPath {} {
    global Endoscopic
        
    set Endoscopic(path,stop) 0
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicResetPath
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicResetPath {} {
    global Endoscopic Path 
        
    set Endoscopic(cam,viewUpX) 0
    set Endoscopic(cam,viewUpY) 0
    set Endoscopic(cam,viewUpZ) 1
    EndoscopicStopPath
    
    set Endoscopic(path,stepStr) 0
    EndoscopicSetPathFrame
    EndoscopicResetStopPath
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicSetPathFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetPathFrame {} {
    global Endoscopic Model View Path
    
    if { $Endoscopic(path,exists) == 0 } {
    # do nothing
    }
    if { $Endoscopic(path,exists) == 1 } {
    
    if { $Endoscopic(path,flyDirection) == "Forward" } {
        
        set Endoscopic(path,i) $Endoscopic(path,stepStr)
        set l [Endoscopic(cLand,allInterpolatedPoints) GetPoint $Endoscopic(path,i)] 
        set l2 [Endoscopic(fLand,allInterpolatedPoints) GetPoint $Endoscopic(path,i)]

        EndoscopicUpdateVirtualEndoscope "[lindex $l 0] [lindex $l 1] [lindex $l 2] [lindex $l2 0] [lindex $l2 1] [lindex $l2 2]"
        EndoscopicUpdateActorFromVirtualEndoscope 
        
    } elseif { $Endoscopic(path,flyDirection) == "Backward" } {
        set Endoscopic(path,i) $Endoscopic(path,stepStr)
        set l [Endoscopic(cLand,allInterpolatedPoints) GetPoint $Endoscopic(path,i)]
        set l2 [Endoscopic(fLand,allInterpolatedPoints) GetPoint $Endoscopic(path,i)]
        EndoscopicUpdateVirtualEndoscope "[lindex $l 0] [lindex $l 1] [lindex $l 2] [lindex $l2 0] [lindex $l2 1] [lindex $l2 2]"
        EndoscopicUpdateActorFromVirtualEndoscope 
    }
    }
}
    


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetFlyDirection
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
#proc EndoscopicSetFlyDirection {{dir ""}} {
#    global Endoscopic Path
#    
#    if {$dir != ""} {
#    if {$dir == "Forward" || $dir == "Backward"} {
#        set Endoscopic(path,flyDirection) $dir
#    } else {
#        return
#    }   
#    }
#}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetSpeed
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetSpeed {} {
    global Endoscopic Path
    
    set Endoscopic(path,speed) $Endoscopic(path,speedStr)
}


#############################################################################
#
#     PART 8:  Slice driver operations
#
#############################################################################


#-------------------------------------------------------------------------------
# .PROC EndoscopicCheckDriver
# This procedure is called once the position of the endoscope is updated. It checks to see if there is a driver for the slices and calls EndoscopicReformatSlices with the right argument to update the position of the slices.

# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicCheckDriver {} {

global Endoscopic


    if { $Endoscopic(fp,driver) == 1 } {
    set fp_mat [Endoscopic(fp,actor) GetMatrix]
    set x [$fp_mat GetElement 0 3]
    set y [$fp_mat GetElement 1 3]
    set z [$fp_mat GetElement 2 3]
    EndoscopicReformatSlices $x $y $z
    } elseif { $Endoscopic(cam,driver) == 1 } {
    set cam_mat [Endoscopic(cam,actor) GetMatrix]
    set x [$cam_mat GetElement 0 3]
    set y [$cam_mat GetElement 1 3]
    set z [$cam_mat GetElement 2 3]
    EndoscopicReformatSlices $x $y $z
    } elseif { $Endoscopic(intersection,driver) == 1 } {
    # get the intersection
    set l [endoscopicRen GetCenter]
    set l0 [expr [lindex $l 0]]
    set l1 [expr [lindex $l 1]]
    set l2 [expr [lindex $l 2]]
    set p [Endoscopic(picker) Pick $l0 $l1 $l2 endoscopicRen]
    if { $p == 1} {
        set selPt [Endoscopic(picker) GetPickPosition]
        set x [expr [lindex $selPt 0]]
        set y [expr [lindex $selPt 1]]
        set z [expr [lindex $selPt 2]]
        EndoscopicReformatSlices $x $y $z
    }
    
    }
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicReformatSlices
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicReformatSlices {x y z} {
    global Endoscopic View Slice

    # Force recomputation of the reformat matrix
    Slicer SetDirectNTP \
        $Endoscopic(cam,viewUpX) $Endoscopic(cam,viewUpY) $Endoscopic(cam,viewUpZ) \
            $Endoscopic(cam,viewPlaneNormalX) $Endoscopic(cam,viewPlaneNormalY) $Endoscopic(cam,viewPlaneNormalZ)  \
        $x $y $z
    RenderSlices
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicSetSliceDriver
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetSliceDriver {name} {
    global Endoscopic Model View Slice


    # Change button text
    $Endoscopic(mbDriver) config -text $name
    
    
    if { $name == "User" } {
    foreach s $Slice(idList) {
        Slicer SetDriver $s 0
    }
    set Endoscopic(fp,driver) 0
    set Endoscopic(cam,driver) 0
    set Endoscopic(intersection,driver) 0
    } else {
    

    foreach s $Slice(idList) {
        Slicer SetDriver $s 1
    }
    if { $name == "Camera"} {
        set m cam
        set Endoscopic(fp,driver) 0
        set Endoscopic(cam,driver) 1
        set Endoscopic(intersection,driver) 0
    } elseif { $name == "FocalPoint"} {
        set m fp 
        set Endoscopic(fp,driver) 1
        set Endoscopic(cam,driver) 0
        set Endoscopic(intersection,driver) 0
    } elseif { $name == "Intersection"} {
        set m intersection 
        set Endoscopic(fp,driver) 0
        set Endoscopic(cam,driver) 0
        set Endoscopic(intersection,driver) 1
    }
    
    MainSlicesSetOrientAll "Orthogonal"
    EndoscopicCheckDriver
    Render3D
    }    
}


#############################################################################
#
#     PART 9: MRML operations
#
#############################################################################


#-------------------------------------------------------------------------------
# .PROC EndoscopicAddToEndOfMRMLTree id cx cy cz fx fy fz
# creates the Path and EndPath nodes if they don't already exist
# creates a Landmark node at the position id with camera position = cx,cy,cz
# and focalPoint position = fx, fy, fz 
# MainUpdateMrml is not called in this procedure because if a path is created
# automatically, we only want to update the Mrml file once with the new path
# So the developper is responsible for calling MainUpdateMrml once it is 
# appropriate to write the new nodes to the Mrml file
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddToEndOfMRMLTree {id cx cy cz fx fy fz} {
    global Endoscopic Landmark Path EndPath
    
    # do if there is none
    if { $Endoscopic(cam,PathNode) == "" && $Endoscopic(cam,EndPathNode) == "" } {
    set Endoscopic(cam,PathNode) [MainMrmlAddNode Path]  
    set Endoscopic(cam,EndPathNode) [MainMrmlAddNode EndPath]  
    }
    

    set Endoscopic(cam,$id,LandmarkNode) [MainMrmlInsertBeforeNode $Endoscopic(cam,EndPathNode) Landmark]
    set pid [$Endoscopic(cam,$id,LandmarkNode) GetID]
    Landmark($pid,node) SetXYZ $cx $cy $cz
    Landmark($pid,node) SetFXYZ $fx $fy $fz
    Landmark($pid,node) SetPathPosition $id     

}


#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateInMRMLTree id cx cy cz fx fy fz
# updates the Landmark node at the position id with camera position = cx,cy,cz
# and focalPoint position = fx, fy, fz 
#
# MainUpdateMrml is not called in this procedure because if a path is updated
# automatically, we only want to update the Mrml file once with the new path
# So the developper is responsible for calling MainUpdateMrml once it is 
# appropriate to write the new nodes to the Mrml file
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateInMRMLTree {id cx cy cz fx fy fz} {
    global Endoscopic Landmark Path EndPath
    
    # do if there is none
    set pid [$Endoscopic(cam,$id,LandmarkNode) GetID]
    Landmark($pid,node) SetXYZ $cx $cy $cz
    Landmark($pid,node) SetFXYZ $fx $fy $fz
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateMRML
#
# Update the Fiducials
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateMRML {} {
    global Mrml Path Endoscopic

    # Endoscopic(FileMode) is set to 1 when we are reading a path from a file
    set Endoscopic(FileMode) 0
    set Endoscopic(path,vtkNodeRead) 0
    
    Mrml(dataTree) ComputeTransforms
    Mrml(dataTree) InitTraversal
    set item [Mrml(dataTree) GetNextItem]
    while { $item != "" } {

    if { [$item GetClassName] == "vtkMrmlPathNode" } {
        set Endoscopic(path,vtkNodeRead) 1
        set Endoscopic(FileMode) 1
        # if the Path is the same than the one we are working on,
        # don't update anything
        set pid [$item GetID]
        if { $Endoscopic(cam,PathNode) != "" && $pid == [$Endoscopic(cam,PathNode) GetID]} {
        set Endoscopic(FileMode) 0
        break
        }   
        # if the path is not the same, reset all the path variables
        EndoscopicResetPathVariables

        # if there is no pathNode yet and we are reading one, set
        # the Endoscopic(cam,PathNode) variable
        if { $Endoscopic(cam,PathNode) == ""} {
        set Endoscopic(cam,PathNode) $item 
        #check that a path doesn't exist on the screen
        if {[Endoscopic(cLand,keyPoints) GetNumberOfPoints] != 0} {
            puts "we are about to create a new path, but one exists on the screen"
        }
        }
    }
    if { [$item GetClassName] == "vtkMrmlLandmarkNode" } {
                
        set lid [$item GetID]
        set i [Landmark($lid,node) GetPathPosition]
        set l [Landmark($lid,node) GetXYZ]
        #puts "in updateMrml    set Endoscopic(cLand,$i,x) [lindex $l 0]"
        set Endoscopic(cLand,$i,x) [lindex $l 0]
        set Endoscopic(cLand,$i,y) [lindex $l 1]
        set Endoscopic(cLand,$i,z) [lindex $l 2]
        set f [Landmark($lid,node) GetFXYZ]
        set Endoscopic(fLand,$i,x) [lindex $f 0]
        set Endoscopic(fLand,$i,y) [lindex $f 1]
        set Endoscopic(fLand,$i,z) [lindex $f 2]
        set Endoscopic(cam,$i,LandmarkNode) $item
        # update the scrollable list
        $Endoscopic(path,fLandmarkList) insert end "$l"
        #set Endoscopic(path,numLandmarks) [expr $i + 1]
        # update vtk
        foreach m "c f" {
        Endoscopic(${m}Land,aSplineX) AddPoint $i $Endoscopic(${m}Land,$i,x)
        Endoscopic(${m}Land,aSplineY) AddPoint $i $Endoscopic(${m}Land,$i,y)
        Endoscopic(${m}Land,aSplineZ) AddPoint $i $Endoscopic(${m}Land,$i,z)
        Endoscopic(${m}Land,keyPoints) InsertPoint $i $Endoscopic(${m}Land,$i,x) $Endoscopic(${m}Land,$i,y) $Endoscopic(${m}Land,$i,z)
        Endoscopic(${m}Land,scalars) InsertScalar $i 0.2
        }
        
        # FIXME find a more elegant solution ??
        if {$Endoscopic(fLand,$i,x) == $Endoscopic(cLand,$i,x) &&
        $Endoscopic(fLand,$i,y) == $Endoscopic(cLand,$i,y) &&
        $Endoscopic(fLand,$i,z) == $Endoscopic(cLand,$i,z) } {
        set Endoscopic(fLand,$i,y) [expr $Endoscopic(fLand,$i,y) + 1]
        }
    }
    
    if { [$item GetClassName] == "vtkMrmlEndPathNode" } {
        # if there is no EndPathNode yet and we are reaading one, set
        # the Endoscopic(cam,PathNode) variable
        if { $Endoscopic(cam,EndPathNode) == ""} {
        set Endoscopic(cam,EndPathNode) $item  
        }
    }
    
    set item [Mrml(dataTree) GetNextItem]
    }
    

    # if we haven't encountered a path node (ie the user closed the file and
    # an UpdateMrml was made), then delete the current path
    # (we don't call EndoscopicDeletePath, because the procedure also deletes
    # the Mrml nodes and in this case, the Mrml nodes are already deleted)
    
    if {$Endoscopic(path,vtkNodeRead) == 0} {
    EndoscopicResetPathVariables
    } elseif {$Endoscopic(FileMode) == 1} {
    # create the path and reset the variable for the next MrmlUpdate
    EndoscopicBuildInterpolatedPath    
    }
}

############################################################################
#
#  PART 10: Helper functions     
#
############################################################################

#-------------------------------------------------------------------------------
# .PROC EndoscopicDistanceBetweenTwoPoints
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicDistanceBetweenTwoPoints {p1x p1y p1z p2x p2y p2z} {

    return [expr sqrt((($p2x - $p1x) * ($p2x - $p1x)) + (($p2y - $p1y) * ($p2y - $p1y)) + (($p2z - $p1z) * ($p2z - $p1z)))]
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateSelectionLandmarkList
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateSelectionLandmarkList {id} {
    
    global Endoscopic
    set sel [$Endoscopic(path,fLandmarkList) curselection] 
    if {$sel != ""} {
    $Endoscopic(path,fLandmarkList) selection clear $sel $sel
    }
    $Endoscopic(path,fLandmarkList) selection set $id $id
    $Endoscopic(path,fLandmarkList) see $id
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicSetModelsVisibilityInside
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetModelsVisibilityInside {} {
    global View Model Endoscopic

    if { $Endoscopic(ModelsVisibilityInside) == 0 } {
    set value 1
    } else {
    set value 0
    }
    
    foreach m $Model(idList) {
    MainModelsSetCulling $m $value
    }

}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetSlicesVisibility
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetSlicesVisibility {} {
    global View Endoscopic

    if { $Endoscopic(SlicesVisibility) == 0 } {
    foreach s "0 1 2" {
        endoscopicRen RemoveActor Slice($s,outlineActor)
        endoscopicRen RemoveActor Slice($s,planeActor)
    }
    } else {
    foreach s "0 1 2" {
        endoscopicRen AddActor Slice($s,outlineActor)
        endoscopicRen AddActor Slice($s,planeActor)
    }
    }
    Render3D
}

