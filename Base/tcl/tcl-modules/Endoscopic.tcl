#=auto==========================================================================
# (c) Copyright 2001 Massachusetts Institute of Technology
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
# FILE:        Endoscopic.tcl
# PROCEDURES:  
#   EndoscopicEnter
#   EndoscopicExit
#   EndoscopicInit
#   EndoscopicBuildVTK
#   EndoscopicCreateCamera
#   EndoscopicSetParamCamera
#   EndoscopicCreateFocalPoint
#   EndoscopicCreateLandmarks
#   EndoscopicCreatePath
#   EndoscopicBuildGUI
#   EndoscopicCreateLabelAndSlider
#   EndoscopicCreateCheckbutton
#   EndoscopicAddEndoscopicView
#   EndoscopicRemoveEndoscopicView
#   EndoscopicCreateAdvancedGUI
#   EndoscopicSetActive
#   EndoscopicPopupCallback
#   EndoscopicSetVisibility
#   EndoscopicSetSize
#   EndoscopicSetCameraPosition
#   EndoscopicResetCameraPosition
#   EndoscopicResetCameraDirection
#   EndoscopicSetCameraDirection
#   EndoscopicSetFocalAndCameraPosition
#   EndoscopicUpdateCamera
#   EndoscopicSetCameraZoom
#   EndoscopicSetCameraViewAngle
#   EndoscopicAddLandmarkAtWorldPos
#   EndoscopicAddLandmark
#   EndoscopicDeleteLandmark
#   EndoscopicComputePath
#   EndoscopicDeletePath
#   EndoscopicComputeRandomPath
#   EndoscopicCreateNewPath
#   EndoscopicShowPath
#   EndoscopicViewPath
#   EndoscopicStopPath
#   EndoscopicResetStopPath
#   EndoscopicResetPath
#   EndoscopicSetPathFrame
#   EndoscopicAddToMrml
#   EndoscopicSetFlyDirection
#   EndoscopicSetSpeed
#   EndoscopicSetCameraAxis
#   EndoscopicLightFollowEndoCamera
#   EndoscopicSetModelsVisibilityInside
#   EndoscopicSetSliceDriver
#   EndoscopicSetSlicePosition
#   EndoscopicUpdateMRML
#   EndoscopicCameraMotionFromUser
#==========================================================================auto=

    
#-------------------------------------------------------------------------------
# .PROC EndoscopicEnter
# Called when this module is entered by the user.  

# effects: Pushes the event manager for this module and 
#          calls EndoscopicAddView. 
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
    pushEventManager $Endoscopic(eventManager)

	EndoscopicAddEndoscopicView
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
    global Model
    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    
    popEventManager
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicInit
#  The "Init" procedure is called automatically by the slicer.  
#  
#
#  effects: * It puts information about the module into a global array called 
#             Module. 
#           * It adds a renderer called View(endCam) to the global array 
#             View and adds the new renderer to the list of renderers in 
#             Module(renderers) 
#           * It also initializes module-level variables (in the global array 
#             Endoscopic and Path)
#             The global array Endoscopic contains information about the 
#             6 actors created in this module:
#
#               cam: the endoscopic camera 
#               fp:  the focal point of cam
#               cPath: the camera path
#               cLand: the landmarks on the camera Path
#               fPath: the focal point path
#               fLand: the landmarks on the focal point Path
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicInit {} {
    global Endoscopic Module Model Path Advanced View Gui
    

    
    set m Endoscopic
    set Module($m,row1List) "Help Init Camera Path Advanced"
    set Module($m,row1Name) "{Help} {Init} {Camera} {Path} {Advanced}"
    set Module($m,row1,tab) Init

    set Module($m,depend) ""
    
    # Define Procedures
    #------------------------------------
    
    set Module($m,procVTK) EndoscopicBuildVTK
    set Module($m,procGUI) EndoscopicBuildGUI
    set Module($m,procCameraMotion) EndoscopicCameraMotionFromUser
    set Module($m,procEnter) EndoscopicEnter
    set Module($m,procExit) EndoscopicExit
    set Module($m,procMRML) EndoscopicUpdateMRML
    
    # create a second renderer here in Init so that it is added to the list of
    # Renderers before MainActorAdd is called anywhere
    # That way any actor added to viewRen (the MainView's renderer) is also 
    # added to endRen and will appear on the second window once we decide 
    # to show it
    
    vtkRenderer endRen
    lappend Module(Renderers) endRen    
    eval endRen SetBackground $View(bgColor)

    vtkLight endLight
    vtkLight endLight2
    endRen AddLight endLight
    endRen AddLight endLight2
    set View(endCam) [endRen GetActiveCamera]
    $View(endCam) SetPosition 0 0 0
    $View(endCam) SetFocalPoint 0 30 0
    $View(endCam) SetViewUp 0 0 1
    $View(endCam) ComputeViewPlaneNormal        
    eval $View(endCam) SetClippingRange $View(endoscopicClippingRange)
    


    
        
    # Initialize module-level variables
    #------------------------------------
    
    
    set Endoscopic(count) 0
    set Endoscopic(eventManager)  ""
    
    # Camera variables
    
    set Endoscopic(cam,tempX) 0
    set Endoscopic(cam,tempY) 0
    set Endoscopic(cam,tempZ) 0
    
    # don't change these default values, change Endoscopic(cam,size) instead
    
    set Endoscopic(cam,name) "Camera"
    set Endoscopic(cam,visibility) 1    
    set Endoscopic(cam,size) 1
    set Endoscopic(cam,sizeStr) 1     
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
    
    # if it is absolute, the camera will move along the 
    #  RA/IS/LR axis
    # if it is relative, the camera will move along its
    #  own axis 
    set Endoscopic(cam,axis) absolute
    
    # Endoscopic variables
    

    set Endoscopic(cPath,name) "Camera Path"
    set Endoscopic(cPath,size) .5 
    set Endoscopic(cPath,sizeStr) .5     
    set Endoscopic(cPath,visibility) 1    
    set Endoscopic(cPath,color) ".4 .2 .6" 

    set Endoscopic(cLand,name) "Camera Landmarks"
    set Endoscopic(cLand,size) 1 
    set Endoscopic(cLand,sizeStr) 1 
    set Endoscopic(cLand,visibility) 1    
    set Endoscopic(cLand,color) ".4 .2 .6"

    set Endoscopic(fPath,name) "Focal Path"
    set Endoscopic(fPath,size) .45 
    set Endoscopic(fPath,visibility) 1    
    set Endoscopic(fPath,color) ".2 .6 .8"
    set Endoscopic(fPath,sizeStr) .5 

    set Endoscopic(fLand,name) "Focal Landmarks"
    set Endoscopic(fLand,size) 1 
    set Endoscopic(fLand,visibility) 1    
    set Endoscopic(fLand,color) ".2 .6 .8"
    set Endoscopic(fLand,sizeStr) 1 
    
    #Advanced variables
    set Endoscopic(ModelsVisibilityInside) 1
    set Endoscopic(positionLandmarkFromEventMgr) 0

    # Path variable
    set Path(flyDirection) "Forward"
    set Path(speed) 1
    set Path(random) 0
    set Path(first) 1
    set Path(i) 0
    set Path(stepStr) 0
    set Path(exists) 0
    set Path(landmarkExists) 0
    set Path(numLandmarks) 0
    set Path(stop) 0
    set Path(vtkNodeRead) 0

#    set Colors
     set Path(cColor) [MakeColor "0 204 255"]
     set Path(sColor) [MakeColor "204 255 255"]
     set Path(eColor) [MakeColor "255 204 204"]
     set Path(rColor) [MakeColor "204 153 153"]
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicBuildVTK
#  Creates the vtk objects for this module
#  
#  effects: calls EndoscopicCreateFocalPoint, 
#           EndoscopicCreateCamera, EndoscopicCreateLandmarks and 
#           EndoscopicCreatePath   
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicBuildVTK {} {
        global Endoscopic Model

        # create the focal point actor
        EndoscopicCreateFocalPoint

        # create the camera actor
        EndoscopicCreateCamera

        # create the Landmarks actors
        EndoscopicCreateLandmarks

        #create the Path actors 
	EndoscopicCreatePath    

        # add the CameraActor only to viewRen, not endRen
        viewRen AddActor Endoscopic(cam,actor)
	viewRen AddActor Endoscopic(fp,actor)

        vtkMath Path(math)


}

#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateCamera
#
# Create the Camera vtk actor
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
	    
	    EndoscopicSetParamCamera

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
	      Endoscopic(Box,actor) PickableOn

       	    vtkActor Endoscopic(Box2,actor)
	      Endoscopic(Box2,actor) SetMapper BoxMapper2
	      eval [Endoscopic(Box2,actor) GetProperty] SetColor $Endoscopic(Box,color) 
	      Endoscopic(Box2,actor) PickableOn

       	    vtkActor Endoscopic(Box3,actor)
	      Endoscopic(Box3,actor) SetMapper BoxMapper3
	      eval [Endoscopic(Box3,actor) GetProperty] SetColor 0 0 0 
	      Endoscopic(Box3,actor) PickableOn
	    
	    vtkActor Endoscopic(Lens,actor)
	      Endoscopic(Lens,actor) SetMapper LensMapper
	      eval [Endoscopic(Lens,actor) GetProperty] SetColor $Endoscopic(Lens,color) 
	      Endoscopic(Lens,actor) PickableOn

	    vtkActor Endoscopic(Lens2,actor)
	      Endoscopic(Lens2,actor) SetMapper LensMapper2
	      [Endoscopic(Lens2,actor) GetProperty] SetColor 0 0 0
	      Endoscopic(Lens2,actor) PickableOn

	    set Endoscopic(cam,actor) [vtkAssembly Endoscopic(cam,actor)]
	      Endoscopic(cam,actor) AddPart Endoscopic(Box,actor)
       	      Endoscopic(cam,actor) AddPart Endoscopic(Box2,actor)
	      Endoscopic(cam,actor) AddPart Endoscopic(Box3,actor)
	      Endoscopic(cam,actor) AddPart Endoscopic(Lens,actor)
	      Endoscopic(cam,actor) AddPart Endoscopic(Lens2,actor)
	      Endoscopic(cam,actor) PickableOff
	    
	    # set the camera invisible until the user enters the module (through EndoscopicAddView)
	    Endoscopic(cam,actor) SetVisibility 0

	    vtkMatrix4x4 Endoscopic(actor,matrix)
	    Endoscopic(cam,actor) SetUserMatrix Endoscopic(actor,matrix)
	}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetParamCamera
#
# effects: Set the size parameters for the camera and the focal point
# .ARGS
# .END
#-------------------------------------------------------------------------------

	proc EndoscopicSetParamCamera {} {
	    global Endoscopic

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
	    set Endoscopic(fp,size) [expr 4 * $Endoscopic(cam,size)]
	    Endoscopic(fp,source) SetRadius $Endoscopic(fp,size)
	    
	    
	    EndoscopicUpdateCamera
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
              # set invisible until the module is active
               Endoscopic(fp,actor) SetVisibility 0
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

	vtkFloatPoints    Endoscopic($l,inputPoints)  
	vtkFloatPoints    Endoscopic($l,points)
	vtkPolyData       Endoscopic($l,polyData)       
	vtkSphereSource   Endoscopic($l,source)        
	vtkGlyph3D        Endoscopic($l,glyph)        
	vtkPolyDataMapper Endoscopic($l,mapper)
	vtkActor          Endoscopic($l,actor)
	vtkCardinalSpline Endoscopic($l,aSplineX)
	vtkCardinalSpline Endoscopic($l,aSplineY)
	vtkCardinalSpline Endoscopic($l,aSplineZ)
	
	Endoscopic($l,polyData)   SetPoints Endoscopic($l,inputPoints)
	Endoscopic($l,source)     SetRadius $Endoscopic($l,size)
	Endoscopic($l,source)     SetPhiResolution 10
	Endoscopic($l,source)     SetThetaResolution 10
	
	Endoscopic($l,glyph)      SetInput Endoscopic($l,polyData)
	Endoscopic($l,glyph)      SetSource [Endoscopic($l,source) GetOutput]
	Endoscopic($l,mapper)     SetInput [Endoscopic($l,glyph) GetOutput]
	Endoscopic($l,actor)      SetMapper Endoscopic($l,mapper)
	
	eval [Endoscopic($l,actor) GetProperty] SetDiffuseColor $Endoscopic($l,color)
	[Endoscopic($l,actor) GetProperty] SetSpecular .3
	[Endoscopic($l,actor) GetProperty] SetSpecularPower 30
	Endoscopic($l,actor) SetVisibility 0
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

        foreach p "cPath fPath" {
	    vtkPolyData         Endoscopic($p,polyData)
	    vtkCellArray        Endoscopic($p,lines)    
	    vtkTubeFilter       Endoscopic($p,source)
	    vtkPolyDataMapper   Endoscopic($p,mapper)    
	    vtkActor            Endoscopic($p,actor)

	    Endoscopic($p,polyData)     SetLines  Endoscopic($p,lines)
	    Endoscopic($p,source)       SetNumberOfSides 8
	    Endoscopic($p,source)       SetInput Endoscopic($p,polyData)
	    Endoscopic($p,source)       SetRadius $Endoscopic($p,size)
	    Endoscopic($p,mapper)       SetInput [Endoscopic($p,source) GetOutput]
	    Endoscopic($p,actor)        SetMapper Endoscopic($p,mapper)

      eval  [Endoscopic($p,actor) GetProperty] SetDiffuseColor $Endoscopic($p,color)
            [Endoscopic($p,actor) GetProperty] SetSpecular .3
            [Endoscopic($p,actor) GetProperty] SetSpecularPower 30
	    Endoscopic($p,actor) SetVisibility 0
	}	
    	    Endoscopic(cPath,polyData)     SetPoints Endoscopic(cLand,points)
	    Endoscopic(fPath,polyData)     SetPoints Endoscopic(fLand,points)
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

    #bind the TKInteractor to new commands for this module
    set widget $Gui(fViewWin)
    #bind $widget <Control-B1-Motion> {EndoscopicPanObject %W %x %y; Render3D}
    #bind $widget <Control-B2-Motion> {EndoscopicObjectZoom %W %x %y}
    #bind $widget <Control-Motion> {EndoscopicObjectSelectLandmark %W %x %y}
    
    # create the event manager for the ability to add landmarks with
    # the key-press e on 2D slices
    
    lappend Endoscopic(eventManager) {$Gui(fSl0Win) <KeyPress-c> \
	    { if { [SelectPick2D %W %x %y] != 0 } \
	    { eval EndoscopicAddLandmarkAtWorldPos $Select(xyz); Render3D } } }
    lappend Endoscopic(eventManager) {$Gui(fSl1Win) <KeyPress-c> \
	    { if { [SelectPick2D %W %x %y] != 0 } \
	    { eval EndoscopicAddLandmarkAtWorldPos $Select(xyz);Render3D } } }
    lappend Endoscopic(eventManager) {$Gui(fSl2Win) <KeyPress-c> \
	    { if { [SelectPick2D %W %x %y] != 0 } \
	    { eval EndoscopicAddLandmarkAtWorldPos $Select(xyz);Render3D } } }
    
	
        set LposTexts "{L<->R } {P<->A } {I<->S }"
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
    <P>Tab content descriptions:<BR>
    <UL>
    <LI><B>Camera</B> 
<BR>This tab contains sliders to control the camera position, orientation and zoom.
<P><B>To move and rotate the camera</B>:
<BR>- select absolute mode (to move along the world's axis) or relative mode (to move along the camera's axis)
<BR>- to move the camera with precision, use the sliders or type a world coordinate in the text box, next to the sliders. For the orientation of the camera, the number displayed is the angle of rotation in degrees.
<BR>- to move the camera intuitively, use the mouse:
<BR> 1) to pan (move left/right and up/down): hold the <B>Shift</B> key and the <B>left</B> mouse button and move the mouse in the right screen (the Endoscopic screen)
<BR> 2) to Pitch/Yaw: hold the <B>Shift</B> key and the <B>right</B> mouse button and move the mouse in the right screen
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
<BR><B>To delete a camera landmark</B>:
<BR>- Select the landmark position in the box
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
<BR><I>- Forward</I>: during fly-through, the camera faces forward   
<BR><I>- Backward</I>: during fly-through, the camera faces backward (works only on random paths)  
<BR><B>Command sliders</B>:
<BR><I>- Frame</I>: positions the camera on any part of the path
<BR><I>- Speed</I>: controls the speed of the camera's motion along the path 
<BR><B>To drive the slices</B>:
<BR> Select which driver you want: User, Camera or Focal Point.
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
	# Init frame
	#-------------------------------------------
	set fInit $Module(Endoscopic,fInit)
	set f $fInit
	frame $f.fSize -bg $Gui(activeWorkspace) 
        pack $f.fSize -side top -pady 0 -padx 0 -fill x

	#-------------------------------------------
	# Init->Size Frame
	#-------------------------------------------
	set f $fInit.fSize
	
	frame $f.fTitle -bg $Gui(activeWorkspace)
	frame $f.fBtns -bg $Gui(activeWorkspace)
	frame $f.fTitle2 -bg $Gui(activeWorkspace)
   	pack $f.fTitle $f.fBtns $f.fTitle2 -side top -pady 5

        
        eval {label $f.fTitle.lTitle -text "
If your screen allows, you can change 
the width of the view screen to have 
a better Endoscopic view:"} $Gui(WLA)
        pack $f.fTitle.lTitle -side left -padx $Gui(pad) -pady 0

        eval {label $f.fBtns.lW -text "Width:"} $Gui(WLA)
	eval {entry $f.fBtns.eWidth -width 5 -textvariable View(viewerWidth)} $Gui(WEA)
        bind $f.fBtns.eWidth  <Return> {MainViewerSetMode}
        eval {menubutton $f.fMBtns -text "choose" -menu $f.fMBtns.fMenu} $Gui(WMBA)
	eval {menu $f.fMBtns.fMenu} $Gui(WMA)
	$f.fMBtns.fMenu add command -label 1000 -command {set View(viewerWidth) 1000; MainViewerSetMode}
	$f.fMBtns.fMenu add command -label 768 -command {set View(viewerWidth) 768; MainViewerSetMode}
	
	grid $f.fBtns.lW $f.fBtns.eWidth $f.fMBtns -padx $Gui(pad)

	set text "
To start, go to the Camera tab 
If you need help, go to the Help tab"

	eval {label $f.fTitle2.lTitle -text $text} $Gui(WLA)
        pack $f.fTitle2.lTitle -side left -padx $Gui(pad) -pady 0

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

       EndoscopicCreateCheckbutton $f "Path" "Show Inside Models" Endoscopic(ModelsVisibilityInside) 18 0 "EndoscopicSetModelsVisibilityInside"


        #-------------------------------------------
	# Advanced->Mid->Toggle frame
	#-------------------------------------------

        set f $fAdvanced.fMid.fToggle

	eval {label $f.l -height 2 -text "Cam Direction:"} $Gui(WTA)
	foreach value "relative absolute" width "9 8" {
	    eval {radiobutton $f.r$value -width $width \
		    -text "$value" -value "$value" -variable Endoscopic(flyDirection)\
		    -indicatoron 0 -command "EndoscopicSetCameraAxis $value; Render3D"} $Gui(WCA) 
	}       
	$f.rabsolute select

	grid $f.l $f.rabsolute $f.rrelative  -padx $Gui(pad) -pady $Gui(pad)
	

        #-------------------------------------------
	# Camera frame
	#-------------------------------------------
	set fCamera $Module(Endoscopic,fCamera)
	set f $fCamera

	frame $f.fTop -bg $Gui(activeWorkspace) -height 300
	frame $f.fMid -bg $Gui(activeWorkspace) -height 300
	frame $f.fBot -bg $Gui(activeWorkspace) -height 100  -relief groove -bd 2
	pack $f.fTop $f.fMid $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill x


	#-------------------------------------------
	# Camera->Top frame
	#-------------------------------------------
	set f $fCamera.fTop

	frame $f.fTitle   -bg $Gui(activeWorkspace)
	frame $f.fPos     -bg $Gui(activeWorkspace)
	pack  $f.fTitle $f.fPos\
		-side top 
	

	#-------------------------------------------
	# Camera->Top->Title frame
	#-------------------------------------------

        set f $fCamera.fTop.fTitle

	eval {label $f.l -height 1 -text "Camera Position"} $Gui(WTA)
	eval {button $f.r \
		-text "reset" -width 5 -command "EndoscopicResetCameraPosition; Render3D"} $Gui(WBA)            

        eval {menubutton $f.fMBtns -text "absolute" -menu $f.fMBtns.fMenu} $Gui(WMBA)  
	eval {menu $f.fMBtns.fMenu} $Gui(WMA) 

	$f.fMBtns.fMenu add command -label "absolute" -command {set Endoscopic(flyDirection) "absolute"; set LposTexts "{L<->R } {P<->A } {I<->S }"; EndoscopicSetCameraAxis "absolute"; Render3D}
	$f.fMBtns.fMenu add command -label "relative" -command {set Endoscopic(flyDirection) "relative"; set LposTexts "{Left/Right} {Forw/Back} {Up/Down}";EndoscopicSetCameraAxis "relative"; Render3D}
	
	set Endoscopic(axis) $f.fMBtns
	grid $f.l $f.r $f.fMBtns -padx 1 -pady 1




	

	#-------------------------------------------
	# Camera->Top->Pos frame
	#-------------------------------------------
	set f $fCamera.fTop.fPos


	# Position Sliders
	foreach slider $posAxi Ltext $LposTexts Rtext $RposTexts orient "horizontal horizontal vertical" {

	    EndoscopicCreateLabelAndSlider $f l$slider 0 "$Ltext" $slider $orient -180 180 110 Endoscopic(cam,${slider}Str) "EndoscopicSetCameraPosition $slider" 5 0

	}


	#-------------------------------------------
	# Camera->Mid frame
	#-------------------------------------------
	set f $fCamera.fMid

	frame $f.fTitle   -bg $Gui(activeWorkspace)
	frame $f.fPos     -bg $Gui(activeWorkspace)
	frame $f.fTitle2    -bg $Gui(activeWorkspace)
	frame $f.fZoom    -bg $Gui(activeWorkspace)
	pack  $f.fTitle $f.fPos $f.fTitle2 $f.fZoom -side top 

	#-------------------------------------------
	# Camera->Mid->Title frame
	#-------------------------------------------

        set f $fCamera.fMid.fTitle

	eval {label $f.l -height 1 -text "Camera Orientation"} $Gui(WTA)
	eval {button $f.r \
		-text "reset" -width 10 -command "EndoscopicResetCameraDirection; Render3D"} $Gui(WBA)            
	grid $f.l $f.r -padx 1 -pady 1

	#-------------------------------------------
	# Camera->Mid->Dir frame
	#-------------------------------------------
	set f $fCamera.fMid.fPos

	# Rotation Sliders
	foreach slider $dirAxi Rtext $RposTexts orient "horizontal horizontal vertical" {

	    EndoscopicCreateLabelAndSlider $f l$slider 0 "$Rtext" $slider $orient -180 180 110 Endoscopic(cam,${slider}Str) "EndoscopicSetCameraDirection $slider" 5 0
	
	}

	#-------------------------------------------
	# Camera->Mid->Title2 frame
	#-------------------------------------------

        set f $fCamera.fMid.fTitle2

	eval {label $f.l2 -height 2 -text "Camera Zoom"} $Gui(WTA)
	pack $f.l2

	#-------------------------------------------
	# Camera->Mid->Zoom frame
	#-------------------------------------------
	set f $fCamera.fMid.fZoom

	# Position Sliders
	
	    EndoscopicCreateLabelAndSlider $f lzoom 0 "In<->Out" $slider horizontal 1 500 110 Endoscopic(cam,zoomStr) "EndoscopicSetCameraZoom" 5 30


	#-------------------------------------------
	# Camera->Bot frame
	#-------------------------------------------
	set f $fCamera.fBot

	frame $f.fShow     -bg $Gui(activeWorkspace)
	pack  $f.fShow\
		-side top -padx $Gui(pad) -pady $Gui(pad)


	#-------------------------------------------
	# Camera->Bot->Rec frame
	#-------------------------------------------

	set f $fCamera.fBot.fShow

	eval {label $f.lTitle -text "Landmarks"} $Gui(WTA)

	set Path(fLandmarkList) [ScrolledListbox $f.list 1 1 -height 5 -width 15]
	#bind $Path(fLandmarkList) <ButtonRelease-1> {EndoscopicHighlightLandmark}

	
	frame $f.fBtns -bg $Gui(activeWorkspace)
	eval {button $f.fBtns.bAdd -text "Add" -width 4 \
		-command "EndoscopicAddLandmark; Render3D"} $Gui(WBA)
	eval {button $f.fBtns.bDelete -text "Delete" -width 7 \
		-command "EndoscopicDeleteLandmark; Render3D"} $Gui(WBA)
	eval {button $f.fBtns.bDeletePath -text "Delete Path" -width 10 \
		-command "EndoscopicDeletePath; Render3D"} $Gui(WBA)
	pack $f.fBtns.bAdd $f.fBtns.bDelete $f.fBtns.bDeletePath -side top -padx $Gui(pad) -pady $Gui(pad)
	
	pack $f.lTitle -side top -pady 2
	pack $f.list $f.fBtns -side left -pady $Gui(pad)


	#-------------------------------------------
	# Path frame
	#-------------------------------------------
	set fPath $Module(Endoscopic,fPath)
	set f $fPath

	foreach frame "Top TMid BMid Bot" {
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
		-text "Fly-Thru"  -width 8 -command "EndoscopicViewPath; Render3D"} $Gui(WMBA) {-bg $Path(sColor)}   

	eval {button $f.fReset \
		-text "Reset"  -width 8  -command "EndoscopicResetPath; Render3D"} $Gui(WMBA) {-bg $Path(sColor)}   

	eval {button $f.fStop \
		-text "Stop" -width 6 -command "EndoscopicStopPath; Render3D"} $Gui(WMBA) {-bg $Path(sColor)}   

	foreach value "Forward Backward" width "9 11" {
		eval {radiobutton $f.r$value -width $width \
			-text "$value" -value "$value" -variable Endoscopic(flyDirection)\
			-indicatoron 0 -command "EndoscopicSetFlyDirection $value; Render3D"} $Gui(WMBA) {-bg $Path(sColor)} {-selectcolor $Path(cColor)}  
	}       
	$f.rForward select

	grid $f.fPath $f.fReset $f.fStop -padx $Gui(pad) -pady $Gui(pad)
	grid $f.rForward $f.rBackward -padx $Gui(pad) -pady $Gui(pad)
	
	#-------------------------------------------
	# Path->Top->Step frame
	#-------------------------------------------
	set f $fPath.fTop.fStep

	# Position Sliders

	eval {label $f.lstep -text "Frame:"} $Gui(WLA)

	eval {entry $f.estep \
		-textvariable Path(stepStr) -width 4} $Gui(WEA) {-bg $Path(sColor)}
	bind $f.estep <Return> \
		"EndoscopicSetPathFrame; Render3D"
	bind $f.estep <FocusOut> \
		"EndoscopicSetPathFrame; Render3D"
	
	eval {scale $f.sstep -from 0 -to 400 -length 100 \
	    	-variable Path(stepStr) \
	    	-command "EndoscopicSetPathFrame; Render3D"\
	    	-resolution 1} $Gui(WSA) {-troughcolor $Path(sColor)}

	set Path(stepScale) $f.sstep
	
	# default color values for the lens sliders
	$f.sstep set 0
	
	# Grid
	grid $f.lstep $f.estep $f.sstep -padx $Gui(pad) -pady $Gui(pad)
    	grid $f.lstep -sticky e

	#-------------------------------------------
	# Path->TMid frame
	#-------------------------------------------
	set f $fPath.fTMid
	frame $f.fSpeed     -bg $Gui(activeWorkspace)
	
	pack  $f.fSpeed -side top -padx $Gui(pad) -pady $Gui(pad)

	#-------------------------------------------
	# Path->TMid->Speed frame
	#-------------------------------------------
	set f $fPath.fTMid.fSpeed

	# Position Sliders

	eval {label $f.lspeed -text "Speed:"} $Gui(WLA)

	eval {entry $f.espeed \
		-textvariable Path(speedStr) -width 4} $Gui(WEA) {-bg $Path(sColor)}
	bind $f.espeed <Return> \
		"EndoscopicSetSpeed; Render3D"
	bind $f.espeed <FocusOut> \
		"EndoscopicSetSpeed; Render3D"
	
	eval {scale $f.sspeed -from 1 -to 20 -length 100 \
	    	-variable Path(speedStr) \
	    	-command "EndoscopicSetSpeed; Render3D"\
	    	-resolution 1} $Gui(WSA) {-troughcolor $Path(sColor)}

	set Path(speedScale) $f.sspeed
	
	# default color values for the lens sliders
	$f.sspeed set 1
	
	# Grid
	grid $f.lspeed $f.espeed $f.sspeed -padx $Gui(pad) -pady $Gui(pad)
    	grid $f.lspeed -sticky e


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
			-bd 2} $Gui(WMBA) {-bg $Path(eColor)}
		set Endoscopic(mDriver) $f.mbDriver.m
		set Endoscopic(mbDriver) $f.mbDriver
		eval {menu $f.mbDriver.m} $Gui(WMA) {-bg $Path(eColor)}
		foreach item "User Camera FocalPoint" {
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
		-text "Compute path" -width 14 -command "EndoscopicComputeRandomPath; Render3D"} $Gui(WBA) {-bg $Path(rColor)}           

	eval {button $f.dRandPath \
		-text "Delete path" -width 14 -command "EndoscopicDeletePath; Render3D"} $Gui(WBA) {-bg $Path(rColor)}           
	
	eval {checkbutton $f.rPath \
		-text "RollerCoaster" -variable Path(rollerCoaster) -width 12 -indicatoron 0 -command "Render3D"} $Gui(WBA) {-bg $Path(rColor)}             
	
	eval {checkbutton $f.sPath \
		-text "Show Path" -variable Path(showPath) -width 12 -indicatoron 0 -command "EndoscopicShowPath; Render3D"} $Gui(WBA) {-bg $Path(rColor)}             
	grid $f.cRandPath $f.dRandPath -padx 0 -pady $Gui(pad)
	grid $f.rPath $f.sPath -padx 0 -pady $Gui(pad)
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
	bind $f.e$sliderName <FocusOut> \
		"$commandString; Render3D"
	
	# Grid
	grid $f.$labelName $f.e$sliderName $f.s$sliderName -padx 0 -pady 0
	
	# default value for the slider
	$f.s$sliderName set $defaultSliderValue
    }

    
#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateCheckbutton
# 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicCreateCheckbutton {f name text variable width on command} {
    global Advanced Gui
    eval {checkbutton $f.v$name \
	    -text $text -variable $variable -width $width -indicatoron $on -command "$command; Render3D"} $Gui(WCA)            
    
    pack $f.v$name -padx 2 -pady 2
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicAddEndoscopicView
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddEndoscopicView {} {
    global View viewWin Gui 

    if {$View(EndoscopicViewOn) == 0} {

	# set the endoscopic actors' visibility according to their prior visibility
	EndoscopicSetVisibility cam
	EndoscopicSetVisibility fp
	EndoscopicSetVisibility cPath
	EndoscopicSetVisibility fPath
	EndoscopicSetVisibility cLand
	EndoscopicSetVisibility fLand
	
	EndoscopicUpdateCamera
        $viewWin AddRenderer endRen	
	viewRen SetViewport 0 0 .5 1
        endRen SetViewport .5 0 1 1
        MainViewerSetEndoscopicViewOn
        MainViewerSetMode $View(mode) 
    }
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicRemoveEndoscopicView
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicRemoveEndoscopicView {} {
    global Endoscopic View viewWin Gui

    if { $View(EndoscopicViewOn) == 1} {

	# reset the slice driver
	EndoscopicSetSliceDriver User

	# set all endoscopic actors to be invisible, without changing their visibility 
	# parameters
	Endoscopic(cam,actor) SetVisibility 0
	Endoscopic(cLand,actor) SetVisibility 0
	Endoscopic(cPath,actor) SetVisibility 0
	Endoscopic(fp,actor) SetVisibility 0
	Endoscopic(fLand,actor) SetVisibility 0
	Endoscopic(fPath,actor) SetVisibility 0
	
        $viewWin RemoveRenderer endRen	
	viewRen SetViewport 0 0 1 1
        MainViewerSetEndoscopicViewOff
        MainViewerSetMode $View(mode) 
    }
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
		-command "EndoscopicSetVisibility $a; Render3D"} $Gui(WCA)
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
	
    	eval {entry $f.e$a -textvariable Endoscopic($a,sizeStr) -width 3} $Gui(WEA)
    	bind $f.e$a  <Return> "EndoscopicSetSize $a; Render3D"
    	bind $f.e$a  <FocusOut> "EndoscopicSetSize $a; Render3D"
    	eval {scale $f.s$a -from 0.0 -to 10.0 -length 70 \
		-variable Endoscopic($a,sizeStr) \
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
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetVisibility
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetVisibility {a} {

	global Endoscopic

    Endoscopic($a,actor) SetVisibility $Endoscopic($a,visibility)
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetSize
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetSize {a} {
    global Advanced Endoscopic Path
    
    set Endoscopic($a,size) $Endoscopic($a,sizeStr)

    if { $a == "cam"} {
	set Endoscopic(fp,distance) [expr 30 * $Endoscopic(cam,size)]
	EndoscopicSetParamCamera
	EndoscopicSetCameraPosition
    } elseif { $Path(exists) == 1 } {   
	Endoscopic($a,source) SetRadius $Endoscopic($a,size)
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicSetCameraPosition
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetCameraPosition {{value ""}} {
	global Endoscopic View Endoscopic

	if {[ValidateFloat $Endoscopic(cam,xStr)] == 0} {
		tk_messageBox -message "LR is not a floating point number."
		return
	}
	if {[ValidateFloat $Endoscopic(cam,yStr)] == 0} {
		tk_messageBox -message "PA is not a floating point number."
		return
	}
	if {[ValidateFloat $Endoscopic(cam,zStr)] == 0} {
		tk_messageBox -message "IS is not a floating point number."
		return
	}


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

	Cross LR Up IO 
	#Cross LR IO Up 
	Normalize LR
	Normalize Up
	Normalize IO
	
	# if we want to go along the camera's own axis (Relative mode)

	if { $Endoscopic(cam,axis) == "relative" } {

	    set stepX [expr $Endoscopic(cam,tempX) - $Endoscopic(cam,xStr)]
	    set stepY [expr $Endoscopic(cam,tempY) - $Endoscopic(cam,yStr)]
	    set stepZ [expr $Endoscopic(cam,tempZ) - $Endoscopic(cam,zStr)]

	    set Endoscopic(cam,x) [expr $Endoscopic(cam,x) + $stepX * $LR(x) \
		    + $stepY * $IO(x) + $stepZ * $Up(x)] 
	    set Endoscopic(cam,y) [expr $Endoscopic(cam,y) + $stepX * $LR(y) \
		    + $stepY * $IO(y) + $stepZ * $Up(y)] 
	    set Endoscopic(cam,z) [expr $Endoscopic(cam,z) + $stepX * $LR(z) \
		    +  $stepY * $IO(z) +  $stepZ * $Up(z)] 
	    
	# else if we want to go along the absolute RA, IS, LR axis

	} elseif { $Endoscopic(cam,axis) == "absolute" } {
	    set Endoscopic(cam,x) $Endoscopic(cam,xStr) 
	    set Endoscopic(cam,y) $Endoscopic(cam,yStr) 
	    set Endoscopic(cam,z) $Endoscopic(cam,zStr) 
	}

	# set the focal point 
	set Endoscopic(fp,x) [expr $Endoscopic(cam,x) + $IO(x) * $Endoscopic(fp,distance)]
	set Endoscopic(fp,y) [expr $Endoscopic(cam,y) + $IO(y) * $Endoscopic(fp,distance)]
	set Endoscopic(fp,z) [expr $Endoscopic(cam,z) + $IO(z) * $Endoscopic(fp,distance)]
	

	# store current x,y,z string
	set Endoscopic(cam,tempX) $Endoscopic(cam,xStr)
	set Endoscopic(cam,tempY) $Endoscopic(cam,yStr)
	set Endoscopic(cam,tempZ) $Endoscopic(cam,zStr)


	Endoscopic(cam,actor) SetPosition $Endoscopic(cam,x) $Endoscopic(cam,y) $Endoscopic(cam,z)
	Endoscopic(fp,actor) SetPosition $Endoscopic(fp,x) $Endoscopic(fp,y) $Endoscopic(fp,z)
	EndoscopicUpdateCamera

}


#-------------------------------------------------------------------------------
# .PROC EndoscopicResetCameraPosition
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicResetCameraPosition {} {
    global Endoscopic

    EndoscopicSetFocalAndCameraPosition 0 0 0 [expr $Endoscopic(fp,x) - $Endoscopic(cam,x)] [expr $Endoscopic(fp,y) - $Endoscopic(cam,y)] [expr $Endoscopic(fp,z) - $Endoscopic(cam,z)]
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
    set y [expr $Endoscopic(cam,y) + $Endoscopic(cam,FPdistance)]
    EndoscopicSetFocalAndCameraPosition $Endoscopic(cam,x) $Endoscopic(cam,y) $Endoscopic(cam,z) $Endoscopic(cam,x) $y $Endoscopic(cam,z)
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetCameraDirection
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetCameraDirection {{value ""}} {
	global Endoscopic View Model

	if {[ValidateFloat $Endoscopic(cam,rxStr)] == 0} {
		tk_messageBox -message "LR is not a floating point number."
		return
	}
	if {[ValidateFloat $Endoscopic(cam,ryStr)] == 0} {
		tk_messageBox -message "PA $Endoscopic(cam,ryStr) is not a floating point number."
		return
	}
	if {[ValidateFloat $Endoscopic(cam,rzStr)] == 0} {
		tk_messageBox -message "IS is not a floating point number."
		return
	}


	
	Endoscopic(cam,actor) SetOrientation $Endoscopic(cam,rxStr) $Endoscopic(cam,ryStr) $Endoscopic(cam,rzStr)
 
	# this is the current amount of rotation
	set Endoscopic(cam,xRotation) $Endoscopic(cam,rxStr) 
	set Endoscopic(cam,yRotation) $Endoscopic(cam,ryStr) 
	set Endoscopic(cam,zRotation) $Endoscopic(cam,rzStr) 


	# this is the current amount of rotation in rad
	set Endoscopic(cam,xRotationRad) [expr $Endoscopic(cam,xRotation) *3.14 / 180]
	set Endoscopic(cam,yRotationRad) [expr $Endoscopic(cam,yRotation) *3.14 / 180]
	set Endoscopic(cam,zRotationRad) [expr $Endoscopic(cam,zRotation) *3.14 / 180]

	if { $value == "rx" } {

	    # position in the yz plane (rotation by x) 
	    set Endoscopic(fp,x) [expr $Endoscopic(cam,x)]
	    set Endoscopic(fp,y) [expr $Endoscopic(cam,y) + $Endoscopic(fp,distance) * {cos($Endoscopic(cam,xRotationRad))}] 
	    set Endoscopic(fp,z) [expr $Endoscopic(cam,z) + $Endoscopic(fp,distance) * {sin($Endoscopic(cam,xRotationRad))}] 
	}

	if { $value == "rz" || $value == "rx"} {

	    # distance from camera in the xy plane 
	    set dz [expr { sqrt (($Endoscopic(fp,y) - $Endoscopic(cam,y)) * ($Endoscopic(fp,y) - $Endoscopic(cam,y)) + ($Endoscopic(fp,x) - $Endoscopic(cam,x)) * ($Endoscopic(fp,x) - $Endoscopic(cam,x)))}]
 
	    if { $Endoscopic(cam,xRotation) < -90 || $Endoscopic(cam,xRotation) > 90 } {
		set dz [expr -$dz]
	    }

	    # position in the xy plane (rotation by z)
	    set Endoscopic(fp,x) [expr $Endoscopic(cam,x) + $dz * {sin($Endoscopic(cam,zRotationRad))} * -1]
	    set Endoscopic(fp,y) [expr $Endoscopic(cam,y) + $dz * {cos($Endoscopic(cam,zRotationRad))}] 
	}
	
	if { $value == "ry" } {
	    set Endoscopic(cam,viewUpX) [expr sin($Endoscopic(cam,yRotationRad))]
	    set Endoscopic(cam,viewUpY) 0
	    set Endoscopic(cam,viewUpZ) [expr cos(-$Endoscopic(cam,yRotationRad))]    

	}	
	

	Endoscopic(fp,actor) SetPosition $Endoscopic(fp,x) $Endoscopic(fp,y) $Endoscopic(fp,z)
	$View(endCam) SetViewUp $Endoscopic(cam,viewUpX) $Endoscopic(cam,viewUpY) $Endoscopic(cam,viewUpZ)
	$View(endCam) SetFocalPoint $Endoscopic(fp,x) $Endoscopic(fp,y) $Endoscopic(fp,z)
	$View(endCam) SetPosition $Endoscopic(cam,x) $Endoscopic(cam,y) $Endoscopic(cam,z)
	$View(endCam) ComputeViewPlaneNormal        
    }
    

#-------------------------------------------------------------------------------
# .PROC EndoscopicSetFocalAndCameraPosition
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetFocalAndCameraPosition {x y z FPx FPy FPz} {
	global Endoscopic View Path Model

    
    # set the new x,y,z strings for the sliders
    set Endoscopic(cam,xStr) $x
    set Endoscopic(cam,yStr) $y
    set Endoscopic(cam,zStr) $z

    # store current x,y,z to calculate the step in relative mode
    set Endoscopic(cam,tempX) $x
    set Endoscopic(cam,tempY) $y
    set Endoscopic(cam,tempZ) $z

    
    #*******************************************************************
    #
    # STEP 0: set the new position of the camera and fp and then update
    #         the virtual camera first
    #
    #*******************************************************************
    
    set Endoscopic(cam,x) $x
    set Endoscopic(cam,y) $y
    set Endoscopic(cam,z) $z
    
    set Endoscopic(fp,x) $FPx
    set Endoscopic(fp,y) $FPy
    set Endoscopic(fp,z) $FPz

    EndoscopicUpdateCamera
    
    #*********************************************************************
    #
    # STEP 1: set the focal point actor's position
    #
    #*********************************************************************
    
    # we have to set the position of the actor, 
    # otherwise it stays the Identity
    Endoscopic(cam,actor) SetPosition $Endoscopic(cam,x) $Endoscopic(cam,y) $Endoscopic(cam,z)
    
    Endoscopic(fp,actor) SetPosition $Endoscopic(fp,x) $Endoscopic(fp,y) $Endoscopic(fp,z)
    
    #*********************************************************************
    #
    # STEP 2: set the camera actor's orientation based on the virtual
    #         camera's orientation
    #         then set the position
    #
    #*********************************************************************
    
    vtkMatrix4x4 matrix
    vtkTransform transform
    
    # first set the rotation matrix based on the Virtual Camera's 
    # coordinate axis (orthogonal unit vectors):

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
    # Bottom row
    matrix SetElement 0 3 0
    matrix SetElement 1 3 0
    matrix SetElement 2 3 0
    matrix SetElement 3 3 1
    
    transform Identity

    # Set the vtkTransform to PostMultiply so a concatenated matrix, C,
    # is multiplied by the existing matrix, M: C*M (not M*C)
    transform PostMultiply

    # STEP 2.1: translate the actor back to the origin
    set old [Endoscopic(cam,actor) GetPosition] 
    set oldx [expr -[lindex $old 0]]
    set oldy [expr -[lindex $old 1]]
    set oldz [expr -[lindex $old 2]]
    transform Translate [expr $oldx] [expr $oldy] [expr $oldz]

    # STEP 2.2: rotate the actor according to the rotation of the virtual 
    #         camera
    transform Concatenate matrix

    # STEP 2.3: translate the actor to its new position
    transform Translate [expr $Endoscopic(cam,x)] [expr $Endoscopic(cam,y)] [expr $Endoscopic(cam,z)]
     
    # STEP 2.4: set the user matrix
    #transform GetMatrix Endoscopic(actor,matrix)
    #Endoscopic(actor,matrix) Modified
    
    # STEP 2.5: set the actor's orientation (better than step 2.5, so we 
    # can call GetOrientation)

    set l [transform GetOrientation]
    set Endoscopic(cam,xRotation) [expr [lindex $l 0]]
    set Endoscopic(cam,yRotation) [expr [lindex $l 1]]
    set Endoscopic(cam,zRotation) [expr [lindex $l 2]]
    Endoscopic(cam,actor) SetOrientation $Endoscopic(cam,xRotation) $Endoscopic(cam,yRotation) $Endoscopic(cam,zRotation)
    
    # set the sliders
    set Endoscopic(cam,rxStr) $Endoscopic(cam,xRotation)
    
    # FIXME: floating point problem (happens when yRotation is < 1. e-06)
    set Endoscopic(cam,ryStr) $Endoscopic(cam,yRotation)
    set Endoscopic(cam,rzStr) $Endoscopic(cam,zRotation)
    
    matrix Delete
    transform Delete

    #*******************************************************************
    #
    # STEP 3: if the user decided to have the camera drive the slice, 
    #         then do it!
    #
    #*******************************************************************

    if { $Endoscopic(fp,driver) == 1 } {
	EndoscopicSetSlicePosition fp 
    } elseif { $Endoscopic(cam,driver) == 1 } {
	EndoscopicSetSlicePosition cam 
    }
    
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateCamera
#       Updates the virtual camera's position, orientation and view angle
#       Calls EndoscopicLightFollowsEndoCamera
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateCamera {} {
    global Endoscopic Model View Path

    # use prior information to prevent the View from flipping at undefined
    # boundary points (i.e when the viewUp and the viewPlaneNormal are 
    # parallel, OrthogonalizeViewUp sometimes produces a viewUp that 
    # flips direction 
    
    $View(endCam) SetViewUp $Endoscopic(cam,viewUpX) $Endoscopic(cam,viewUpY) $Endoscopic(cam,viewUpZ)
    $View(endCam) SetFocalPoint $Endoscopic(fp,x) $Endoscopic(fp,y) $Endoscopic(fp,z)
    $View(endCam) SetPosition $Endoscopic(cam,x) $Endoscopic(cam,y) $Endoscopic(cam,z)
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

    # now move the Camera a percentage of its distance to its Focal Point
    set Endoscopic(cam,x) [expr $Endoscopic(fp,x) - $IO(x) * $Endoscopic(fp,distance)]
    set Endoscopic(cam,y) [expr $Endoscopic(fp,y) - $IO(y) * $Endoscopic(fp,distance)]
    set Endoscopic(cam,z) [expr $Endoscopic(fp,z) - $IO(z) * $Endoscopic(fp,distance)]
    
    Endoscopic(cam,actor) SetPosition $Endoscopic(cam,x) $Endoscopic(cam,y) $Endoscopic(cam,z)
    EndoscopicUpdateCamera
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
# .PROC EndoscopicAddLandmarkAtWorldPos
#
# 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddLandmarkAtWorldPos {x y z} {

    global Endoscopic Model Path

    if { $Path(numLandmarks) < 0} {
	puts "we have a problem!!! numLandmarks < 0"
    }
    set i $Path(numLandmarks)
   
    set Endoscopic(cLand,$i,x) $x
    set Endoscopic(cLand,$i,y) $y
    set Endoscopic(cLand,$i,z) $z
    set Endoscopic(fLand,$i,x) $x
    set Endoscopic(fLand,$i,y) $y
    set Endoscopic(fLand,$i,z) $z
    
    set Endoscopic(positionLandmarkFromEventMgr) 1
    
    EndoscopicAddLandmark
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicAddLandmark
#
# 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddLandmark {} {

    global Endoscopic Model Path View Point EndPath Landmark
    
    # if the path already existing is random, delete it
    if { $Path(random) == 1 } {
	EndoscopicDeletePath
    }
    if { $Path(numLandmarks) < 0} {
	puts "we have a problem!!! numLandmarks < 0"
    }
    set i $Path(numLandmarks)
    
    # cLand and fLand are set to the camera and focal point position by 
    # default, or explicitely in EndoscopicSetLandmark
    if {$Endoscopic(positionLandmarkFromEventMgr) == 0} {
	set Endoscopic(cLand,$i,x) $Endoscopic(cam,x)
	set Endoscopic(cLand,$i,y) $Endoscopic(cam,y)
	set Endoscopic(cLand,$i,z) $Endoscopic(cam,z)
	set Endoscopic(fLand,$i,x) $Endoscopic(fp,x)
	set Endoscopic(fLand,$i,y) $Endoscopic(fp,y)
	set Endoscopic(fLand,$i,z) $Endoscopic(fp,z)
    } else {
	# reset the flag
	set Endoscopic(positionLandmarkFromEventMgr) 0
    }
	
    
    $Path(fLandmarkList) insert end "$Endoscopic(cLand,$i,x) $Endoscopic(cLand,$i,y) $Endoscopic(cLand,$i,z)"
    incr Path(numLandmarks)
    
    EndoscopicAddToMrml $i $Endoscopic(cLand,$i,x) $Endoscopic(cLand,$i,y) $Endoscopic(cLand,$i,z) $Endoscopic(fLand,$i,x) $Endoscopic(fLand,$i,y) $Endoscopic(fLand,$i,z)
    
    MainUpdateMRML
    
    foreach m {c f} {
   
	set numberOfInputPoints $Path(numLandmarks)
	
	# if we are building a new path, reset all the variables
	if { $numberOfInputPoints == 1 } {
	    Endoscopic(${m}Land,aSplineX) RemoveAllPoints
	    Endoscopic(${m}Land,aSplineY) RemoveAllPoints
	    Endoscopic(${m}Land,aSplineZ) RemoveAllPoints
	    Endoscopic(${m}Land,inputPoints) Reset
	    viewRen AddActor Endoscopic(${m}Land,actor)
	    viewRen AddActor Endoscopic(${m}Path,actor)
	    #endRen RemoveActor Endoscopic(${m}Land,actor)
	    #endRen RemoveActor Endoscopic(${m}Path,actor)
	}
	
	set i [expr $numberOfInputPoints - 1]
	Endoscopic(${m}Land,aSplineX) AddPoint $i $Endoscopic(${m}Land,$i,x)
	Endoscopic(${m}Land,aSplineY) AddPoint $i $Endoscopic(${m}Land,$i,y)
	Endoscopic(${m}Land,aSplineZ) AddPoint $i $Endoscopic(${m}Land,$i,z)
	Endoscopic(${m}Land,inputPoints) InsertPoint $i $Endoscopic(${m}Land,$i,x) $Endoscopic(${m}Land,$i,y) $Endoscopic(${m}Land,$i,z)
	set Path(landmarkExists) 1
	
	if { $Path(numLandmarks) > 1 } {

	    EndoscopicComputePath $m
	}
    }
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

    set d [expr [$Path(fLandmarkList) curselection]]
    if { $d == "" } {
	# no selection, so do nothing 
    } else {
	if {$Path(numLandmarks) <= 1} {
	    EndoscopicDeletePath
	} else {
	    set Path(numLandmarks) [expr $Path(numLandmarks) - 1]
	    $Path(fLandmarkList) delete $d
	    
	    
	    MainMrmlUndoAddNode Landmark $Endoscopic(cam,$d,LandmarkNode)

	    for {set i $d} {$i<$Path(numLandmarks)} {incr i 1} {

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
		# we want to remove item d, so rebuild the whole spline
		Endoscopic(${m}Land,inputPoints) Reset
		set numberOfInputPoints $Path(numLandmarks)
		
		# clear the splines
		Endoscopic(${m}Land,aSplineX) RemoveAllPoints
		Endoscopic(${m}Land,aSplineY) RemoveAllPoints
		Endoscopic(${m}Land,aSplineZ) RemoveAllPoints
		
		for {set i 0} {$i<$numberOfInputPoints} {incr i 1} {
		    Endoscopic(${m}Land,aSplineX) AddPoint $i $Endoscopic(${m}Land,$i,x)
		    Endoscopic(${m}Land,aSplineY) AddPoint $i $Endoscopic(${m}Land,$i,y)
		    Endoscopic(${m}Land,aSplineZ) AddPoint $i $Endoscopic(${m}Land,$i,z)
		    Endoscopic(${m}Land,inputPoints) InsertPoint $i $Endoscopic(${m}Land,$i,x) $Endoscopic(${m}Land,$i,y) $Endoscopic(${m}Land,$i,z)
		    
		}
		EndoscopicComputePath $m
	    }
	}
    }
}



 
#-------------------------------------------------------------------------------
# .PROC EndoscopicComputePath
# 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicComputePath {{m ""}} {

    global Endoscopic Model View Path

    set numberOfInputPoints $Path(numLandmarks)
    Endoscopic(${m}Land,polyData) Modified
    Endoscopic(${m}Land,points) Reset
    Endoscopic(${m}Path,lines) Reset 
    Endoscopic(${m}Path,polyData) Modified
    	
    set numberOfOutputPoints [expr $numberOfInputPoints * 10]
    if {$m == "c"} {
	$Path(stepScale) config -to [expr $numberOfOutputPoints - 1]
    }

    if {$numberOfInputPoints > 1 } {
	for {set i 0} {$i< $numberOfOutputPoints} {incr i 1} {
	    #change t
	    set t [expr ( $numberOfInputPoints - 1.0 ) / ( $numberOfOutputPoints - 1) * $i]
	    Endoscopic(${m}Land,points) InsertPoint $i [Endoscopic(${m}Land,aSplineX) Evaluate $t] [Endoscopic(${m}Land,aSplineY) Evaluate $t] [Endoscopic(${m}Land,aSplineZ) Evaluate $t]
	}
	set Path(exists) 1
	
	Endoscopic(${m}Path,lines) InsertNextCell $numberOfOutputPoints
	
	for {set i 0} {$i< $numberOfOutputPoints} {incr i 1} {
	    Endoscopic(${m}Path,lines) InsertCellPoint $i
	}
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

    if { $Path(exists) == 1 } {
	for {set d 0} {$d<$Path(numLandmarks)} {incr d 1} {
	    MainMrmlUndoAddNode Landmark $Endoscopic(cam,$d,LandmarkNode)	   
	}
	
	if { $Endoscopic(cam,PathNode) != ""} {
	    MainMrmlUndoAddNode Path $Endoscopic(cam,PathNode)
	    set Endoscopic(cam,PathNode) ""
	}
	if { $Endoscopic(cam,EndPathNode) != ""} {
	    MainMrmlUndoAddNode EndPath $Endoscopic(cam,EndPathNode)
	    set Endoscopic(cam,EndPathNode) ""
	}
	
	MainUpdateMRML
	
 	set Path(numLandmarks) 0

	MainRemoveActor Endoscopic(cLand,actor)
	MainRemoveActor Endoscopic(fLand,actor)
	MainRemoveActor Endoscopic(cPath,actor)
	MainRemoveActor Endoscopic(fPath,actor)

	
	$Path(fLandmarkList) delete 0 end
	set Path(exists) 0
	set Path(showPath) 0
	set Path(landmarkExists) 0
	set Path(random) 0
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
   
    if { $Path(exists) == 1} {
	EndoscopicDeletePath
    }
	
    for {set i 0} {$i<20} {incr i 1} {
	set x  [expr [Path(math) Random -1 1] * 100]
	set y  [expr [Path(math) Random -1 1] * 100]
	set z  [expr [Path(math) Random -1 1] * 100]
	set Endoscopic(cLand,$i,x) $x
	set Endoscopic(cLand,$i,y) $y
	set Endoscopic(cLand,$i,z) $z
	set Endoscopic(fLand,$i,x) $x
	set Endoscopic(fLand,$i,y) $y
	set Endoscopic(fLand,$i,z) $z
	    
	$Path(fLandmarkList) insert end "$Endoscopic(cLand,$i,x) $Endoscopic(cLand,$i,y) $Endoscopic(cLand,$i,z)"
	set Path(numLandmarks) [expr $i + 1]

	EndoscopicAddToMrml $i $Endoscopic(cLand,$i,x) $Endoscopic(cLand,$i,y) $Endoscopic(cLand,$i,z) $Endoscopic(fLand,$i,x) $Endoscopic(fLand,$i,y) $Endoscopic(fLand,$i,z)	
    }
    MainUpdateMRML
    set Path(random) 1    
    EndoscopicCreateNewPath
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateNewPath
#
# 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicCreateNewPath {} {
    
    global Endoscopic Model View Path View Point
    
    if { $Path(numLandmarks) < 0} {
	puts "we have a problem!!! numLandmarks < 0"
    }
    
    set numberOfInputPoints $Path(numLandmarks)
    
    foreach m {c f} {
	Endoscopic(${m}Land,aSplineX) RemoveAllPoints
	Endoscopic(${m}Land,aSplineY) RemoveAllPoints
	Endoscopic(${m}Land,aSplineZ) RemoveAllPoints
	Endoscopic(${m}Land,inputPoints) Reset
	viewRen AddActor Endoscopic(${m}Land,actor)
	viewRen AddActor Endoscopic(${m}Path,actor)
			
	for {set i 0} {$i<$numberOfInputPoints} {incr i 1} {
	    Endoscopic(${m}Land,aSplineX) AddPoint $i $Endoscopic(${m}Land,$i,x)
	    Endoscopic(${m}Land,aSplineY) AddPoint $i $Endoscopic(${m}Land,$i,y)
	    Endoscopic(${m}Land,aSplineZ) AddPoint $i $Endoscopic(${m}Land,$i,z)
	    Endoscopic(${m}Land,inputPoints) InsertPoint $i $Endoscopic(${m}Land,$i,x) $Endoscopic(${m}Land,$i,y) $Endoscopic(${m}Land,$i,z)
	}
	set Path(landmarkExists) 1
	
	EndoscopicComputePath $m
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicShowPath
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicShowPath {} {
    global Path Endoscopic

    if {$Path(exists) == 1} {
	if {$Path(showPath) == 1} {
	    foreach m {c f} {
		endRen AddActor Endoscopic(${m}Land,actor)
		endRen AddActor Endoscopic(${m}Path,actor)
	    }
	} else {
	    foreach m {c f} {
		endRen RemoveActor Endoscopic(${m}Land,actor)
		endRen RemoveActor Endoscopic(${m}Path,actor)
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
    
    set numPoints [Endoscopic(cLand,points) GetNumberOfPoints]
    if {$Path(random) == 1} {
	set var 4
    } else {
	set var 0
    }
    set u 0
    
    for {set Path(i) $Path(stepStr)} {$Path(i)< $numPoints - $var} {incr Path(i) $Path(speed)} { 
	if {$Path(stop) == "0"} {
	    set Path(stepStr) $Path(i)
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
    global Path 
    	
	set Path(stop) 1
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicResetStopPath
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicResetStopPath {} {
    global Path 
    	
	set Path(stop) 0
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
    set Path(stepStr) 0
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
    
    if { $Path(exists) == 0 } {
	# do nothing
    }
    if { $Path(exists) == 1 } {
	
	if {$Path(random) == 1} {
	    set var 4
	    set which "c"
	} else {
	    set var 0
	    set which "f"
	}
	
	if { $Path(flyDirection) == "Forward" } {
	    
	    set Path(i) $Path(stepStr)
	    set l [Endoscopic(cLand,points) GetPoint $Path(i)] 
	    set l2 [Endoscopic(${which}Land,points) GetPoint [expr $Path(i) + $var ]]
	    EndoscopicSetFocalAndCameraPosition [lindex $l 0] [lindex $l 1] [lindex $l 2] [lindex $l2 0] [lindex $l2 1] [lindex $l2 2]	    

	} elseif { $Path(flyDirection) == "Backward" } {
	    set Path(i) $Path(stepStr)
	    set l [Endoscopic(cLand,points) GetPoint $Path(i)]
	    set l2 [Endoscopic(${which}Land,points) GetPoint [expr $Path(i) - $var ]]
	    EndoscopicSetFocalAndCameraPosition [lindex $l 0] [lindex $l 1] [lindex $l 2] [lindex $l2 0] [lindex $l2 1] [lindex $l2 2]
	}
    }
}
    

#-------------------------------------------------------------------------------
# .PROC EndoscopicAddToMrml id cx cy cz fx fy fz
# creates the Path and EndPath nodes if they don't already exist
# creates a Landmark node at the position i with camera position = cx,cy,cz
# and focalPoint position = fx, fy, fz 
# MainUpdateMrml is not called in this procedure because if a path is created
# automatically, we only want to update the Mrml file once with the new path
# So the developper is responsible for calling MainUpdateMrml once it is 
# appropriate to write the new nodes to the Mrml file
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddToMrml {id cx cy cz fx fy fz} {
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
# .PROC EndoscopicSetFlyDirection
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetFlyDirection {{dir ""}} {
    global Endoscopic Path
    
    if {$dir != ""} {
	if {$dir == "Forward" || $dir == "Backward"} {
	    set Path(flyDirection) $dir
	} else {
	    return
	}   
    }
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetSpeed
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetSpeed {} {
    global Endoscopic Path
    
    set Path(speed) $Path(speedStr)
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
	    $Endoscopic(axis) config -text $axis
	} else {
	    return
	}   
    }
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
    
    set endoLights [endRen GetLights]
    
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
    } else {
	foreach s $Slice(idList) {
	    Slicer SetDriver $s 1
	}
	if { $name == "Camera"} {
	    set m cam
	    set Endoscopic(fp,driver) 0
	    set Endoscopic(cam,driver) 1
	} elseif { $name == "FocalPoint"} {
	    set m fp 
	    set Endoscopic(fp,driver) 1
	    set Endoscopic(cam,driver) 0
	}
	EndoscopicSetSlicePosition $m
    }    
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetSlicePosition
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetSlicePosition {a} {
    global Endoscopic View Slice

    # Force recomputation of the reformat matrix
    Slicer SetDirectNTP \
	    $Endoscopic(cam,viewUpX) $Endoscopic(cam,viewUpY) $Endoscopic(cam,viewUpZ) \
            $Endoscopic(cam,viewPlaneNormalX) $Endoscopic(cam,viewPlaneNormalY) $Endoscopic(cam,viewPlaneNormalZ)  \
	    $Endoscopic($a,x) $Endoscopic($a,y) $Endoscopic($a,z)
#    RenderAll
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


    Mrml(dataTree) ComputeTransforms
    Mrml(dataTree) InitTraversal
    set item [Mrml(dataTree) GetNextItem]
    while { $item != "" } {

	if { [$item GetClassName] == "vtkMrmlPathNode" } {
	    set Path(vtkNodeRead) 1
	    # if the Path is the same than the one we are working on,
	    # don't update anything
	    set pid [$item GetID]
	    if { $Endoscopic(cam,PathNode) != "" && $pid == [$Endoscopic(cam,PathNode) GetID]} {
		break
	    }   
	    # if there is no pathNode yet and we are reading one, set
	    # the Endoscopic(cam,PathNode) variable
	    if { $Endoscopic(cam,PathNode) == ""} {
		set Endoscopic(cam,PathNode) $item  
		#check that a path doesn't exist on the screen
		if {$Path(numLandmarks) != 0} {
		    puts "we are about to create a new path, but one exists on the screen"
		}
	    }
	}
	if { [$item GetClassName] == "vtkMrmlLandmarkNode" } {
	    	    
	    set lid [$item GetID]
	    set i [Landmark($lid,node) GetPathPosition]
	    set l [Landmark($lid,node) GetXYZ]
	    set Endoscopic(cLand,$i,x) [lindex $l 0]
	    set Endoscopic(cLand,$i,y) [lindex $l 1]
	    set Endoscopic(cLand,$i,z) [lindex $l 2]
	    set f [Landmark($lid,node) GetFXYZ]
	    set Endoscopic(fLand,$i,x) [lindex $f 0]
	    set Endoscopic(fLand,$i,y) [lindex $f 1]
	    set Endoscopic(fLand,$i,z) [lindex $f 2]
	    set Endoscopic(cam,$i,LandmarkNode) $item
	    $Path(fLandmarkList) insert end "$l"
	    set Path(numLandmarks) [expr $i + 1]
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
    if {$Path(vtkNodeRead) == 0} {
	 	set Path(numLandmarks) 0


	# get rid of path actors
	
	MainRemoveActor Endoscopic(cLand,actor)
	MainRemoveActor Endoscopic(fLand,actor)
	MainRemoveActor Endoscopic(cPath,actor)
	MainRemoveActor Endoscopic(fPath,actor)	
	
	# reset the pointers to the path nodes
	set Endoscopic(cam,PathNode) ""
	set Endoscopic(cam,EndPathNode) ""

	# clear the scroll text
	$Path(fLandmarkList) delete 0 end
	
	# reset the path variables
	set Path(exists) 0
	set Path(showPath) 0
	set Path(landmarkExists) 0
	set Path(random) 0
    } else {
	# create the path and reset the variable for the next MrmlUpdate
	set Path(vtkNodeRead) 0
	EndoscopicCreateNewPath	
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
	
	set l [$View(endCam) GetPosition]
	set p(x) [lindex $l 0]
	set p(y) [lindex $l 1]
	set p(z) [lindex $l 2]
	
	set Endoscopic(cam,x) $p(x)
	set Endoscopic(cam,y) $p(y)
	set Endoscopic(cam,z) $p(z)
	
	set l [$View(endCam) GetFocalPoint]
	set f(x) [lindex $l 0]
	set f(y) [lindex $l 1]
	set f(z) [lindex $l 2]

	set Endoscopic(fp,x) $f(x)
	set Endoscopic(fp,y) $f(y)
	set Endoscopic(fp,z) $f(z)


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
	

	#*********************************************************************
	#
	# STEP 1: set the focal point actor's position
	#
	#*********************************************************************
	
	Endoscopic(fp,actor) SetPosition $f(x) $f(y) $f(z)
	
	#*********************************************************************
	#
	# STEP 2: set the camera actor's orientation based on the virtual
	#         camera's orientation
	#         then set the position
	#
	#*********************************************************************
	
	vtkMatrix4x4 matrix
	vtkTransform transform
	
	# first set the rotation matrix based on the Virtual Camera's 
    # coordinate axis (orthogonal unit vectors):

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
    # Bottom row
    matrix SetElement 0 3 0
    matrix SetElement 1 3 0
    matrix SetElement 2 3 0
    matrix SetElement 3 3 1
    
    transform Identity

    # Set the vtkTransform to PostMultiply so a concatenated matrix, C,
    # is multiplied by the existing matrix, M: C*M (not M*C)
    transform PostMultiply

    # STEP 2.1: translate the actor back to the origin
    set old [Endoscopic(cam,actor) GetPosition] 
    set oldx [expr -[lindex $old 0]]
    set oldy [expr -[lindex $old 1]]
    set oldz [expr -[lindex $old 2]]
    transform Translate [expr $oldx] [expr $oldy] [expr $oldz]

    # STEP 2.2: rotate the actor according to the rotation of the virtual 
    #         camera
    transform Concatenate matrix

    # STEP 2.3: translate the actor to its new position
    transform Translate [expr $Endoscopic(cam,x)] [expr $Endoscopic(cam,y)] [expr $Endoscopic(cam,z)]
     
    # STEP 2.4: set the user matrix
    transform GetMatrix Endoscopic(actor,matrix)
    Endoscopic(actor,matrix) Modified

    matrix Delete
    transform Delete

    #*******************************************************************
    #
    # STEP 3: if the user decided to have the camera drive the slice, 
    #         then do it!
    #
    #*******************************************************************

    if { $Endoscopic(fp,driver) == 1 } {
	EndoscopicSetSlicePosition fp 
    } elseif { $Endoscopic(cam,driver) == 1 } {
	EndoscopicSetSlicePosition cam 
    }
    
}
}