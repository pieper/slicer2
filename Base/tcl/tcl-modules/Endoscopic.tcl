#=auto==========================================================================
# Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use, copy, (but NOT distribute) this
# software and its documentation for any NON-COMMERCIAL purpose, provided
# that existing copyright notices are retained verbatim in all copies.
# The authors grant permission to modify this software and its documentation 
# for any NON-COMMERCIAL purpose, provided that such modifications are not 
# distributed without the explicit consent of the authors and that existing
# copyright notices are retained in all copies. Some of the algorithms
# implemented by this software are patented, observe all applicable patent law.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
# DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
# OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
# EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
# 'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
#===============================================================================
# FILE:        Endoscopic.tcl
# PROCEDURES:  
#   EndoscopicInit
#   EndoscopicBuildGUI
#   EndoscopicCount
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
#  This module is an example for developers.  It shows how to add a module 
#  to the Slicer.  To find it when you run the Slicer, click on More->Endoscopic.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  Variables
#  These are the variables defined by this module.
# 
#  int Endoscopic(count) counts the button presses for the demo 
#  list Endoscopic(eventManager)  list of event bindings used by this module
#-------------------------------------------------------------------------------
    
#-------------------------------------------------------------------------------
# .PROC EndoscopicEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
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
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
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
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicInit {} {
	global Endoscopic Module Volume Model Path Advanced View Gui

	# Define Tabs
	#------------------------------------
	# Description:
	#   Each module is given a button on the Slicer's main menu.
	#   When that button is pressed a row of tabs appear, and there is a panel
	#   on the user interface for each tab.  If all the tabs do not fit on one
	#   row, then the last tab is automatically created to say "More", and 
	#   clicking it reveals a second row of tabs.
	#
	#   Define your tabs here as shown below.  The options are:
	#   
	#   row1List = list of ID's for tabs. (ID's must be unique single words)
	#   row1Name = list of Names for tabs. (Names appear on the user interface
	#              and can be non-unique with multiple words.)
	#   row1,tab = ID of initial tab
	#   row2List = an optional second row of tabs if the first row is too small
	#   row2Name = like row1
	#   row2,tab = like row1 
	#

	set m Endoscopic
	set Module($m,row1List) "Help Init Camera Path Advanced"
        set Module($m,row1Name) "{Help} {Init} {Camera} {Path} {Advanced}"
	set Module($m,row1,tab) Init

	# Define Procedures
	#------------------------------------
	# Description:
	#   The Slicer sources all *.tcl files, and then it calls the Init
	#   functions of each module, followed by the VTK functions, and finally
	#   the GUI functions. A MRML function is called whenever the MRML tree
	#   changes due to the creation/deletion of nodes.
	#   
	#   While the Init procedure is required for each module, the other 
	#   procedures are optional.  If they exist, then their name (which
	#   can be anything) is registered with a line like this:
	#
	#   set Module($m,procVTK) EndoscopicBuildVTK
	#
	#   All the options are:
	#
	#   procGUI   = Build the graphical user interface
	#   procVTK   = Construct VTK objects
	#   procMRML  = Update after the MRML tree changes due to the creation
	#               of deletion of nodes.
	#   procEnter = Called when the user enters this module by clicking
	#               its button on the main menu
	#   procExit  = Called when the user leaves this module by clicking
	#               another modules button
	#   procStorePresets  = Called when the user holds down one of the Presets
	#               buttons.
	#   procRecallPresets  = Called when the user clicks one of the Presets buttons
	#               
	#   Note: if you use presets, make sure to give a preset defaults
	#   string in your init function, of the form: 
	#   set Module($m,presets) "key1='val1' key2='val2' ..."
	#   
        
        set Module($m,procVTK) EndoscopicBuildVTK
	set Module($m,procGUI) EndoscopicBuildGUI
	set Module($m,procEnter) EndoscopicEnter
	set Module($m,procExit) EndoscopicExit
	set Module($m,procMRML) EndoscopicUpdateMRML

        # create a second renderer here in Init so that it is added to the list of
        # Renderers before MainActorAdd is called anywhere
        # That way any actor added to viewRen (the MainView's renderer) is also added 
        # to endRen and will appear on the second window once we decide to show it

        vtkRenderer endRen
        vtkLight endLight
        vtkLight endLight2
        endRen AddLight endLight
        endRen AddLight endLight2
 	set View(endCam) [endRen GetActiveCamera]

	lappend Module(Renderers) endRen

	# Define Dependencies
	#------------------------------------
	# Description:
	#   Record any other modules that this one depends on.  This is used 
	#   to check that all necessary modules are loaded when Slicer runs.
	#   
	set Module($m,depend) ""

        # Set version info
	#------------------------------------
	# Description:
	#   Record the version number for display under Help->Version Info.
	#   The strings with the $ symbol tell CVS to automatically insert the
	#   appropriate revision number and date when the module is checked in.
	#  	lappend Module(versions) [ParseCVSInfo $m \
		{$Revision: 1.4 $} {$Date: 2001/02/14 23:11:33 $}]

	# Initialize module-level variables
	#------------------------------------
	# Description:
	#   Keep a global array with the same name as the module.
	#   This is a handy method for organizing the global variables that
	#   the procedures in this module and others need to access.
	#

    set Endoscopic(count) 0
    set Endoscopic(Volume1) $Volume(idNone)
    set Endoscopic(Model1)  $Model(idNone)
    set Endoscopic(eventManager)  ""
    
    # Camera variables
    
    set Model(cam,tempX) 0
    set Model(cam,tempY) 0
    set Model(cam,tempZ) 0
    
    # don't change these default values, change Model(cam,size) instead
    
    set Model(cam,name) "Camera"
    set Model(cam,visibility) 1    
    set Model(cam,size) 1
    set Model(cam,sizeStr) 1     
    set Model(cam,boxlength) 30
    set Model(cam,boxheight) 20
    set Model(cam,boxwidth)  10
    set Model(cam,x) 0
    set Model(cam,y) 0
    set Model(cam,z) 0
    set Model(cam,xRotation) 0
    set Model(cam,yRotation) 0
    set Model(cam,zRotation) 0
    set Model(cam,FPdistance) 30
    set Model(cam,viewAngle) 90
    set Model(cam,AngleStr) 90
    set Model(cam,viewUpX) 0 
    set Model(cam,viewUpY) 0 
    set Model(cam,viewUpZ) 1
    set Model(cam,driver) 0
    set Model(cam,PathNode) ""
    set Model(cam,EndPathNode) ""

    set Model(Box,name) "Camera Box"
    set Model(Box,color) "1 .4 .5" 

    set Model(Lens,name) "Camera Lens"
    set Model(Lens,color) ".4 .2 .6" 

    set Model(fp,name) "Focal Point"    
    set Model(fp,visibility) 1    
    set Model(fp,size) 4
    set Model(fp,color) ".2 .6 .8"
    set Model(fp,x) 0
    set Model(fp,y) 30
    set Model(fp,z) 0
    set Model(fp,driver) 0
    
    # if it is absolute, the camera will move along the 
    #  RA/IS/LR axis
    # if it is relative, the camera will move along its
    #  own axis 
    set Model(cam,axis) absolute
    
    # Model variables
    

    set Model(cPath,name) "Camera Path"
    set Model(cPath,size) .5 
    set Model(cPath,sizeStr) .5     
    set Model(cPath,visibility) 1    
    set Model(cPath,color) ".4 .2 .6" 

    set Model(cLand,name) "Camera Landmarks"
    set Model(cLand,size) 1 
    set Model(cLand,sizeStr) 1 
    set Model(cLand,visibility) 1    
    set Model(cLand,color) ".4 .2 .6"

    set Model(fPath,name) "Focal Path"
    set Model(fPath,size) .5 
    set Model(fPath,visibility) 1    
    set Model(fPath,color) ".2 .6 .8"
    set Model(fPath,sizeStr) .5 

    set Model(fLand,name) "Focal Landmarks"
    set Model(fLand,size) 1 
    set Model(fLand,visibility) 1    
    set Model(fLand,color) ".2 .6 .8"
    set Model(fLand,sizeStr) 1 
    
    #Advanced variables
    set Advanced(ModelsVisibilityInside) 1

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
#
# Create the vtk objects for this module (i.e the camera)
# .END
#-------------------------------------------------------------------------------

proc EndoscopicBuildVTK {} {
        global Endoscopic Model

        # set the default background for the endoscopic view
	EndoscopicViewSetBackgroundColor

        # create the focal point actor
        EndoscopicCreateFocalPoint

        # create the camera actor
        EndoscopicCreateCamera

        # create the Landmarks actors
        EndoscopicCreateLandmarks

        #create the Path actors 
	EndoscopicCreatePath    

        # add the CameraActor only to viewRen, not endRen
        viewRen AddActor Model(cam,actor)
	viewRen AddActor Model(fp,actor)

        vtkMath Path(math)

    # force the camera and fp to be invisible until the user enters the 
    # endoscopic module

    Model(cam,actor) SetVisibility 0
    Model(fp,actor) SetVisibility 0
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateCamera
#
# Create the Camera actor
# .END
#-------------------------------------------------------------------------------

	proc EndoscopicCreateCamera {} {
	    global Model
	    

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

       	    vtkActor Model(Box,actor)
	      Model(Box,actor) SetMapper BoxMapper
	      eval [Model(Box,actor) GetProperty] SetColor $Model(Box,color)
	      Model(Box,actor) PickableOn

       	    vtkActor Model(Box2,actor)
	      Model(Box2,actor) SetMapper BoxMapper2
	      eval [Model(Box2,actor) GetProperty] SetColor $Model(Box,color) 
	      Model(Box2,actor) PickableOn

       	    vtkActor Model(Box3,actor)
	      Model(Box3,actor) SetMapper BoxMapper3
	      eval [Model(Box3,actor) GetProperty] SetColor 0 0 0 
	      Model(Box3,actor) PickableOn
	    
	    vtkActor Model(Lens,actor)
	      Model(Lens,actor) SetMapper LensMapper
	      eval [Model(Lens,actor) GetProperty] SetColor $Model(Lens,color) 
	      Model(Lens,actor) PickableOn

	    vtkActor Model(Lens2,actor)
	      Model(Lens2,actor) SetMapper LensMapper2
	      [Model(Lens2,actor) GetProperty] SetColor 0 0 0
	      Model(Lens2,actor) PickableOn

	    set Model(cam,actor) [vtkAssembly Model(cam,actor)]
	      Model(cam,actor) AddPart Model(Box,actor)
       	      Model(cam,actor) AddPart Model(Box2,actor)
	      Model(cam,actor) AddPart Model(Box3,actor)
	      Model(cam,actor) AddPart Model(Lens,actor)
	      Model(cam,actor) AddPart Model(Lens2,actor)
	      Model(cam,actor) PickableOff
	    
	    Model(cam,actor) SetVisibility $Model(cam,visibility)

	}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetParamCamera
#
# Create the Camera actor
# .END
#-------------------------------------------------------------------------------

	proc EndoscopicSetParamCamera {} {
	    global Model

	    # set parameters for cube (body) geometry and transform
	    set Model(cam,boxlength) [expr $Model(cam,size) * 30]
	    set Model(cam,boxheight) [expr $Model(cam,size) * 20]
	    set Model(cam,boxwidth)  [expr $Model(cam,size) * 10]

	    set unit [expr $Model(cam,size)]

	    camCube SetXLength $Model(cam,boxlength)
	      camCube SetYLength $Model(cam,boxwidth)
	      camCube SetZLength $Model(cam,boxheight)
	    
	    CubeXform Identity
	      #CubeXform RotateZ 90

	    camCube2 SetXLength [expr $Model(cam,boxlength) /3] 
	      camCube2 SetYLength $Model(cam,boxwidth)
	      camCube2 SetZLength [expr $Model(cam,boxheight) /3]
            
	    CubeXform2 Identity
	      CubeXform2 Translate 0 0 [expr {$Model(cam,boxheight) /2}]

	    camCube3 SetXLength [expr $Model(cam,boxlength) /6] 
	      camCube3 SetYLength [expr $Model(cam,boxwidth) +$unit]
	      camCube3 SetZLength [expr $Model(cam,boxheight) /6]
            
	    CubeXform3 Identity
	      CubeXform3 Translate 0 0 [expr {$Model(cam,boxheight) /2} + $unit]

	    # set parameters for cylinder (lens) geometry and transform
	    camCyl SetRadius [expr $Model(cam,boxlength) / 3.5]
	    camCyl SetHeight [expr $Model(cam,boxwidth) * 1.5]
	    camCyl SetResolution 8
	    
	    CylXform Identity
	      CylXform Translate 0 [expr $Model(cam,boxwidth) / 2] 0
	    
	    camCyl2 SetRadius [expr $Model(cam,boxlength) / 5]
	      camCyl2 SetHeight [expr $Model(cam,boxwidth) * 1.5]
	      camCyl2 SetResolution 8
	    
	    CylXform2 Identity
  	      CylXform2 Translate 0 [expr {$Model(cam,boxwidth) / 2} + $unit] 0

	    #also, set the size of the focal Point actor
	    set Model(fp,size) [expr 4 * $Model(cam,size)]
	    Model(fp,source) SetRadius $Model(fp,size)
	    
	    
	    EndoscopicUpdateCamera
	}


#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateFocalPoint
#
# Create the Focal Point Actor.
# .END
#-------------------------------------------------------------------------------

proc EndoscopicCreateFocalPoint {} {
         global Model


            vtkSphereSource Model(fp,source)
               Model(fp,source) SetRadius $Model(fp,size)
            vtkPolyDataMapper Model(fp,mapper)
               Model(fp,mapper) SetInput [Model(fp,source) GetOutput]
            vtkActor Model(fp,actor)
               Model(fp,actor) SetMapper Model(fp,mapper)
	       eval [Model(fp,actor) GetProperty] SetColor $Model(fp,color)
	       Model(fp,actor) SetVisibility $Model(fp,visibility)
               set Model(fp,distance) [expr $Model(cam,boxwidth) *3] 
               Model(fp,actor) SetPosition 0 $Model(fp,distance) 0
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateLandmarks
#
# Create the CameraLandmarks and FocalPLandmarks Actors.
# .END
#-------------------------------------------------------------------------------

proc EndoscopicCreateLandmarks {} {
         global Model

    foreach l "cLand fLand" {

	vtkFloatPoints    Model($l,inputPoints)  
	vtkFloatPoints    Model($l,points)
	vtkPolyData       Model($l,polyData)       
	vtkSphereSource   Model($l,source)        
	vtkGlyph3D        Model($l,glyph)        
	vtkPolyDataMapper Model($l,mapper)
	vtkActor          Model($l,actor)
	vtkCardinalSpline Model($l,aSplineX)
	vtkCardinalSpline Model($l,aSplineY)
	vtkCardinalSpline Model($l,aSplineZ)
	
	Model($l,polyData)   SetPoints Model($l,inputPoints)
	Model($l,source)     SetRadius $Model($l,size)
	Model($l,source)     SetPhiResolution 10
	Model($l,source)     SetThetaResolution 10
	
	Model($l,glyph)      SetInput Model($l,polyData)
	Model($l,glyph)      SetSource [Model($l,source) GetOutput]
	Model($l,mapper)     SetInput [Model($l,glyph) GetOutput]
	Model($l,actor)      SetMapper Model($l,mapper)
	
	eval [Model($l,actor) GetProperty] SetDiffuseColor $Model($l,color)
	[Model($l,actor) GetProperty] SetSpecular .3
	[Model($l,actor) GetProperty] SetSpecularPower 30

    }
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicCreatePath
#
# Create the CameraPath and FocalPPath Actors.
# .END
#-------------------------------------------------------------------------------

proc EndoscopicCreatePath {} {
         global Model

        foreach p "cPath fPath" {
	    vtkPolyData         Model($p,polyData)
	    vtkCellArray        Model($p,lines)    
	    vtkTubeFilter       Model($p,source)
	    vtkPolyDataMapper   Model($p,mapper)    
	    vtkActor            Model($p,actor)

	    Model($p,polyData)     SetLines  Model($p,lines)
	    Model($p,source)       SetNumberOfSides 8
	    Model($p,source)       SetInput Model($p,polyData)
	    Model($p,source)       SetRadius $Model($p,size)
	    Model($p,mapper)       SetInput [Model($p,source) GetOutput]
	    Model($p,actor)        SetMapper Model($p,mapper)

      eval  [Model($p,actor) GetProperty] SetDiffuseColor $Model($p,color)
            [Model($p,actor) GetProperty] SetSpecular .3
            [Model($p,actor) GetProperty] SetSpecularPower 30
	}	
    	    Model(cPath,polyData)     SetPoints Model(cLand,points)
	    Model(fPath,polyData)     SetPoints Model(fLand,points)
    }

#-------------------------------------------------------------------------------
# .PROC EndoscopicBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------

proc EndoscopicBuildGUI {} {
	global Gui Endoscopic Path Module Volume Model Advanced Slice View


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
<BR><B>To create a path</B>:
<BR>- Position the camera where desired
<BR>- Click the button <I>Add</I>: this will add a landmark (a sphere) at the current camera position and another landmark at<BR> the current focal point position 
<BR>- Two paths will automatically be created: one that links all the camera landmarks and another one that links all the focal point landmarks
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
<P>
<LI><B>Advanced</B>
<BR>This Tab allows you to change color and size parameters for the camera, focal point, landmarks and path
<BR><B>Camera Direction</B>:
<BR>- Absolute: the camera moves along the SI, RL and PA axis
<BR>- Relative: the camera moves along its own axis, depending on its orientation

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
Now go to start, go to the Camera tab 
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

        EndoscopicCreateLabelAndSlider $f l2 2 "Lens Angle" "Angle" horizontal 0 360 110 Model(cam,AngleStr) "EndoscopicSetCameraAngle" 5 90


	#-------------------------------------------
	# Advanced->Mid->Vis frame
	#-------------------------------------------
	
        set f $fAdvanced.fMid.fVis

       EndoscopicCreateCheckbutton $f "Path" "Show Inside Models" Advanced(ModelsVisibilityInside) 18 0 "EndoscopicSetModelsVisibilityInside"


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
		-text "reset" -width 10 -command "EndoscopicResetCameraPosition; Render3D"} $Gui(WBA)            
	grid $f.l $f.r -padx 1 -pady 1

	#-------------------------------------------
	# Camera->Top->Pos frame
	#-------------------------------------------
	set f $fCamera.fTop.fPos


	# Position Sliders
	foreach slider $posAxi Ltext $LposTexts Rtext $RposTexts orient "horizontal horizontal vertical" {

	    EndoscopicCreateLabelAndSlider $f l$slider 0 "$Ltext" $slider $orient -180 180 110 Model(cam,${slider}Str) "EndoscopicSetCameraPosition $slider" 5 0

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

	eval {label $f.l -height 1 -text "Camera Rotation"} $Gui(WTA)
	eval {button $f.r \
		-text "reset" -width 10 -command "EndoscopicResetCameraDirection; Render3D"} $Gui(WBA)            
	grid $f.l $f.r -padx 1 -pady 1

	#-------------------------------------------
	# Camera->Mid->Pos frame
	#-------------------------------------------
	set f $fCamera.fMid.fPos

	# Rotation Sliders
	foreach slider $dirAxi Rtext $RposTexts orient "horizontal horizontal vertical" {

	    EndoscopicCreateLabelAndSlider $f l$slider 0 "$Rtext" $slider $orient -180 180 110 Model(cam,${slider}Str) "EndoscopicSetCameraDirection $slider" 5 0
	
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
	
	    EndoscopicCreateLabelAndSlider $f lzoom 0 "In<->Out" $slider horizontal 1 500 110 Model(cam,zoomStr) "EndoscopicSetCameraZoom" 5 30


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

	grid $f.fPath $f.fReset -padx $Gui(pad) -pady $Gui(pad)
	grid $f.fStop $f.rForward $f.rBackward -padx $Gui(pad) -pady $Gui(pad)
	
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

	eval {button $f.cRandPath \
		-text "Compute Random path" -width 20 -command "EndoscopicComputeRandomPath; Render3D"} $Gui(WBA) {-bg $Path(rColor)}           

	eval {button $f.dRandPath \
		-text "Delete Random path" -width 20 -command "EndoscopicDeletePath; Render3D"} $Gui(WBA) {-bg $Path(rColor)}           
	
	eval {checkbutton $f.cPath \
		-text "RollerCoaster" -variable Path(rollerCoaster) -width 18 -indicatoron 0 -command "Render3D"} $Gui(WBA) {-bg $Path(rColor)}             
	pack $f.cRandPath $f.dRandPath $f.cPath -padx $Gui(pad) -pady $Gui(pad)
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
		-resolution 1} $Gui(WSA)
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
# .PROC EndoscopicUpdateGUI
# 
# This procedure is called to update the buttons
# due to such things as volumes or models being added or subtracted.
# (Note: to do this, this proc must be this module's procMRML.  Right now,
# these buttons are being updated automatically since they have been added
# to lists updated in VolumesUpdateMRML and ModelsUpdateMRML.  So this procedure
# is not currently used.)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateGUI {} {
	global Endoscopic Volume

   DevUpdateSelectButton Volume Endoscopic Volume1 Volume1 DevSelect
   DevUpdateSelectButton Model  Endoscopic Model1  Model1  DevSelect
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicAddEndoscopicView
# 
# .ARGS
# .END
#----------------------------------------------------------------------------

proc EndoscopicAddEndoscopicView {} {
    global View viewWin Gui

    if {$View(EndoscopicViewOn) == 0} {

	# set the endoscopic actors' visibility according to their prior visibility
	EndoscopicSetVisibility cam
	EndoscopicSetVisibility cLand
	EndoscopicSetVisibility cPath
	EndoscopicSetVisibility fp
	EndoscopicSetVisibility fLand
	EndoscopicSetVisibility fPath
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
#----------------------------------------------------------------------------

proc EndoscopicRemoveEndoscopicView {} {
    global Endoscopic View viewWin Gui

    if { $View(EndoscopicViewOn) == 1} {

	# reset the slice driver
	EndoscopicSetSliceDriver User

	# set all endoscopic actors to be invisible, without changing their visibility 
	# parameters
	Model(cam,actor) SetVisibility 0
	Model(cLand,actor) SetVisibility 0
	Model(cPath,actor) SetVisibility 0
	Model(fp,actor) SetVisibility 0
	Model(fLand,actor) SetVisibility 0
	Model(fPath,actor) SetVisibility 0
	
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
#----------------------------------------------------------------------------

proc EndoscopicCreateAdvancedGUI {f a {vis ""} {col ""} {size ""}} {
    global Gui Model Color Advanced

    
    # Name / Visible only if vis is not empty
    if {$vis == "visible"} {
	eval {checkbutton $f.c$a \
		-text $Model($a,name) -wraplength 65 -variable Model($a,visibility) \
		-width 11 -indicatoron 0\
		-command "EndoscopicSetVisibility $a; Render3D"} $Gui(WCA)
    } else {
	eval {label $f.c$a -text $Model($a,name)} $Gui(WLA)
    }

    # Change colors
    if {$col == "color"} {
	eval {button $f.b$a \
		-width 1 -command "EndoscopicSetActive $a $f.b$a; ShowColors EndoscopicPopupCallback; Render3D" \
		-background [MakeColorNormalized $Model($a,color)]}
    } else {
	eval {label $f.b$a -text " " } $Gui(WLA)
    }
    
    if {$size == "size"} {
	
    	eval {entry $f.e$a -textvariable Model($a,sizeStr) -width 3} $Gui(WEA)
    	bind $f.e$a  <Return> "EndoscopicSetSize $a; Render3D"
    	bind $f.e$a  <FocusOut> "EndoscopicSetSize $a; Render3D"
    	eval {scale $f.s$a -from 0.0 -to 10.0 -length 70 \
		-variable Model($a,sizeStr) \
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
	global Label Model Color Gui Advanced

        set name $Label(name) 

        set a $Advanced(ActiveActor)

	# Use second color by default
	set color [lindex $Color(idList) 1]

	foreach c $Color(idList) {
		if {[Color($c,node) GetName] == $name} {
			set color $c
		}
	}

	[Model($a,actor) GetProperty] SetAmbient    [Color($color,node) GetAmbient]
	[Model($a,actor) GetProperty] SetDiffuse    [Color($color,node) GetDiffuse]
	[Model($a,actor) GetProperty] SetSpecular   [Color($color,node) GetSpecular]
	[Model($a,actor) GetProperty] SetSpecularPower [Color($color,node) GetPower]
	eval [Model($a,actor) GetProperty] SetColor    [Color($color,node) GetDiffuseColor]

	set Model($a,color) [Color($color,node) GetDiffuseColor]

	# change the color of the button on the Advanced GUI
	$Advanced(ActiveButton) configure -background [MakeColorNormalized [[Model($a,actor) GetProperty] GetColor]]

	if {$Advanced(ActiveActor) == "Box"} {
	    
	    [Model(Box2,actor) GetProperty] SetAmbient    [Color($color,node) GetAmbient]
	    [Model(Box2,actor) GetProperty] SetDiffuse    [Color($color,node) GetDiffuse]
	    [Model(Box2,actor) GetProperty] SetSpecular   [Color($color,node) GetSpecular]
	    [Model(Box2,actor) GetProperty] SetSpecularPower [Color($color,node) GetPower]
	    eval [Model(Box2,actor) GetProperty] SetColor    [Color($color,node) GetDiffuseColor]

	}
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetVisibility
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetVisibility {a} {

	global Model

    Model($a,actor) SetVisibility $Model($a,visibility)
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetSize
#
# 
# 
# .END
#-------------------------------------------------------------------------------

proc EndoscopicSetSize {a} {
    global Advanced Model Path
    
    set Model($a,size) $Model($a,sizeStr)

    if { $a == "cam"} {
	set Model(fp,distance) [expr 30 * $Model(cam,size)]
	EndoscopicSetParamCamera
	EndoscopicSetCameraPosition
    } elseif { $Path(exists) == 1 } {   
	Model($a,source) SetRadius $Model($a,size)
    }
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetCameraPosition
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc EndoscopicSetCameraPosition {{value ""}} {
	global Endoscopic View Model

	if {[ValidateFloat $Model(cam,xStr)] == 0} {
		tk_messageBox -message "LR is not a floating point number."
		return
	}
	if {[ValidateFloat $Model(cam,yStr)] == 0} {
		tk_messageBox -message "PA is not a floating point number."
		return
	}
	if {[ValidateFloat $Model(cam,zStr)] == 0} {
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

	if { $Model(cam,axis) == "relative" } {
	    set Model(cam,x) [expr $Model(cam,x) + ($Model(cam,tempX) - $Model(cam,xStr)) * $LR(x) + ($Model(cam,tempY) - $Model(cam,yStr)) * $IO(x) + ($Model(cam,tempZ) - $Model(cam,zStr)) * $Up(x)] 
	    set Model(cam,y) [expr $Model(cam,y) + ($Model(cam,tempX) - $Model(cam,xStr)) * $LR(y) + ($Model(cam,tempY) - $Model(cam,yStr)) * $IO(y) + ($Model(cam,tempZ) - $Model(cam,zStr)) * $Up(y)] 
	    set Model(cam,z) [expr $Model(cam,z) + ($Model(cam,tempX) - $Model(cam,xStr)) * $LR(z) + ($Model(cam,tempY) - $Model(cam,yStr)) * $IO(z) + ($Model(cam,tempZ) - $Model(cam,zStr)) * $Up(z)] 
	

	# else if we want to go along the absolute RA, IS, LR axis

	} elseif { $Model(cam,axis) == "absolute" } {
	    set Model(cam,x) $Model(cam,xStr) 
	    set Model(cam,y) $Model(cam,yStr) 
	    set Model(cam,z) $Model(cam,zStr) 
	}

	set Model(fp,x) [expr $Model(cam,x) + $IO(x) * $Model(fp,distance)]
	set Model(fp,y) [expr $Model(cam,y) + $IO(y) * $Model(fp,distance)]
	set Model(fp,z) [expr $Model(cam,z) + $IO(z) * $Model(fp,distance)]
	

	# store current x,y,z string
	set Model(cam,tempX) $Model(cam,xStr)
	set Model(cam,tempY) $Model(cam,yStr)
	set Model(cam,tempZ) $Model(cam,zStr)


	Model(cam,actor) SetPosition $Model(cam,x) $Model(cam,y) $Model(cam,z)
	Model(fp,actor) SetPosition $Model(fp,x) $Model(fp,y) $Model(fp,z)
	EndoscopicUpdateCamera

}


#-------------------------------------------------------------------------------
# .PROC EndoscopicResetCameraPosition
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicResetCameraPosition {} {
    global Model

    EndoscopicSetFocalAndCameraPosition 0 0 0 [expr $Model(fp,x) - $Model(cam,x)] [expr $Model(fp,y) - $Model(cam,y)] [expr $Model(fp,z) - $Model(cam,z)]
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicResetCameraDirection
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicResetCameraDirection {} {
    global Model

    # we reset the rotation around the absolute y axis
    set Model(cam,yRotation) 0
    set Model(cam,viewUpX) 0
    set Model(cam,viewUpY) 0
    set Model(cam,viewUpZ) 1
    set y [expr $Model(cam,y) + $Model(cam,FPdistance)]
    EndoscopicSetFocalAndCameraPosition $Model(cam,x) $Model(cam,y) $Model(cam,z) $Model(cam,x) $y $Model(cam,z)
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicSetCameraDirection
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc EndoscopicSetCameraDirection {{value ""}} {
	global Endoscopic View Model

	if {[ValidateFloat $Model(cam,rxStr)] == 0} {
		tk_messageBox -message "LR is not a floating point number."
		return
	}
	if {[ValidateFloat $Model(cam,ryStr)] == 0} {
		tk_messageBox -message "PA $Model(cam,ryStr) is not a floating point number."
		return
	}
	if {[ValidateFloat $Model(cam,rzStr)] == 0} {
		tk_messageBox -message "IS is not a floating point number."
		return
	}


	Model(cam,actor) SetOrientation $Model(cam,rxStr) $Model(cam,ryStr) $Model(cam,rzStr) 
	# this is the current amount of rotation
	set Model(cam,xRotation) $Model(cam,rxStr) 
	set Model(cam,yRotation) $Model(cam,ryStr) 
	set Model(cam,zRotation) $Model(cam,rzStr) 

	# this is the current amount of rotation in rad
	set Model(cam,xRotationRad) [expr $Model(cam,xRotation) *3.14 / 180]
	set Model(cam,yRotationRad) [expr $Model(cam,yRotation) *3.14 / 180]
	set Model(cam,zRotationRad) [expr $Model(cam,zRotation) *3.14 / 180]

	if { $value == "rx" } {

	    # position in the yz plane (rotation by x) 
	    set Model(fp,x) [expr $Model(cam,x)]
	    set Model(fp,y) [expr $Model(cam,y) + $Model(fp,distance) * {cos($Model(cam,xRotationRad))}] 
	    set Model(fp,z) [expr $Model(cam,z) + $Model(fp,distance) * {sin($Model(cam,xRotationRad))}] 
	}

	if { $value == "rz" || $value == "rx"} {

	    # distance from camera in the xy plane 
	    set dz [expr { sqrt (($Model(fp,y) - $Model(cam,y)) * ($Model(fp,y) - $Model(cam,y)) + ($Model(fp,x) - $Model(cam,x)) * ($Model(fp,x) - $Model(cam,x)))}]
 
	    if { $Model(cam,xRotation) > 90 || $Model(cam,xRotation) < -90 } {
		set dz [expr -$dz]
	    }

	    # position in the xy plane (rotation by z)
	    set Model(fp,x) [expr $Model(cam,x) + $dz * {sin($Model(cam,zRotationRad))} * -1]
	    set Model(fp,y) [expr $Model(cam,y) + $dz * {cos($Model(cam,zRotationRad))}] 
	}
	
	if { $value == "ry" } {
	    # change the View up
	    set Model(cam,viewUpX) [expr sin(-$Model(cam,yRotationRad))]
	    set Model(cam,viewUpZ) [expr cos(-$Model(cam,yRotationRad))]
	}	
	    
	Model(fp,actor) SetPosition $Model(fp,x) $Model(fp,y) $Model(fp,z)
	EndoscopicUpdateCamera

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

    set Model(fp,distance) $Model(cam,zoomStr)

    # now move the Camera a percentage of its distance to its Focal Point
    set Model(cam,x) [expr $Model(fp,x) - $IO(x) * $Model(fp,distance)]
    set Model(cam,y) [expr $Model(fp,y) - $IO(y) * $Model(fp,distance)]
    set Model(cam,z) [expr $Model(fp,z) - $IO(z) * $Model(fp,distance)]
    
    Model(cam,actor) SetPosition $Model(cam,x) $Model(cam,y) $Model(cam,z)
    EndoscopicUpdateCamera
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetCameraAngle
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc EndoscopicSetCameraAngle {} {
    global Endoscopic Model View

    set Model(cam,viewAngle) $Model(cam,AngleStr)
    $View(endCam) SetViewAngle $Model(cam,viewAngle)
    
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicSetFocalAndCameraPosition
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EndoscopicSetFocalAndCameraPosition {x y z FPx FPy FPz} {
	global Endoscopic View Path Model


    set Model(cam,xStr) $x
    set Model(cam,yStr) $y
    set Model(cam,zStr) $z

    set Model(cam,x) $x
    set Model(cam,y) $y
    set Model(cam,z) $z

    set Model(fp,x) $FPx
    set Model(fp,y) $FPy
    set Model(fp,z) $FPz


    Model(cam,actor) SetPosition $Model(cam,x) $Model(cam,y) $Model(cam,z)
    Model(fp,actor) SetPosition $Model(fp,x) $Model(fp,y) $Model(fp,z)
    
    EndoscopicUpdateCamera

    # now that the view up and the view plane are updated, set the right
    # camera directions (and position for roller coaster)


    if {$Path(rollerCoaster) == 1} { 
	set l [$View(endCam) GetViewUp]
	set U(x) [expr [lindex $l 0]]
	set U(y) [expr [lindex $l 1]] 
	set U(z) [expr [lindex $l 2]]

	set Model(cam,x) [expr $Model(cam,x) + $U(x)*2*$Model(cPath,size)] 
	set Model(cam,y) [expr $Model(cam,y) + $U(y)*2*$Model(cPath,size)] 
	set Model(cam,z) [expr $Model(cam,z) + $U(z)*2*$Model(cPath,size)]
	Model(cam,actor) SetPosition $Model(cam,x) $Model(cam,y) $Model(cam,z) 
	EndoscopicUpdateCamera
    }

    # get the view plane normal and orient the camera that way
    
    set l [$View(endCam) GetViewPlaneNormal]
    set T(x) [expr -[lindex $l 0]]
    set T(y) [expr -[lindex $l 1]] 
    set T(z) [expr -[lindex $l 2]]

    # Find angle between T, V(0,1,0) on the xy plane from their dot product
    # [T(x) * 0 + T(y) * 1] / [|T|*|V|=1]
    set dot [expr $T(y)]
    set Model(cam,zRotation) [expr acos($dot) * 180 / 3.14]

    # we have to be careful with the sign if we are turning left or right
    # (if the x coord is positive then the angle of rotation around z is neg)
    if { $T(x) > 0 } {
	set Model(cam,zRotation) -$Model(cam,zRotation)
    }

    # Find angle between T, N(0,0,1) on the yz from their dot product
    set dot [expr $T(z)]
    set Model(cam,xRotation) [expr (acos($dot) * 180 / 3.14)]
    # the problem is we calculate the angle of rotation from the z axis,
    # we want it from the y axis, so the real xRot is 90 degrees - xRot
    set Model(cam,xRotation) [expr 90 - $Model(cam,xRotation)]

    if { $Model(cam,xRotation) <= 0.05 && $Model(cam,xRotation) >= 0 } {
	set Model(cam,xRotation) 0
    }
    if { $Model(cam,xRotation) >= -0.05 && $Model(cam,xRotation) <= 0 } {
	set Model(cam,xRotation) 0
    }
    if { $Model(cam,zRotation) == 0.0} {
	set Model(cam,zRotation) 0
    }
    set Model(cam,rxStr)  $Model(cam,xRotation)
    set Model(cam,ryStr)  $Model(cam,yRotation)
    set Model(cam,rzStr)  $Model(cam,zRotation)    
    Model(cam,actor) SetOrientation $Model(cam,xRotation) $Model(cam,yRotation) $Model(cam,zRotation)
    

}

#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateCamera
# Updates the (virtual) camera position and focal point 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc EndoscopicUpdateCamera {} {
    global Endoscopic Model View
   
	$View(endCam) SetFocalPoint $Model(fp,x) $Model(fp,y) $Model(fp,z)
	$View(endCam) SetPosition $Model(cam,x) $Model(cam,y) $Model(cam,z)
	$View(endCam) SetViewUp $Model(cam,viewUpX) $Model(cam,viewUpY) $Model(cam,viewUpZ) 
        EndoscopicSetCameraAngle
	eval $View(endCam) SetClippingRange $View(endoscopicClippingRange)
	$View(endCam) ComputeViewPlaneNormal
	$View(endCam) OrthogonalizeViewUp

	EndoscopicLightFollowEndoCamera

    # if the user decided to have the camera drive the slice, then do it!
    if { $Model(fp,driver) == 1 } {
	EndoscopicSetSlicePosition fp 
    } elseif { $Model(cam,driver) == 1 } {
	EndoscopicSetSlicePosition cam 
    }
}



#-------------------------------------------------------------------------------
# .PROC EndoscopicAddLandmark
#
# 
# 
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddLandmark {} {

    global Endoscopic Model Path View Fiducials EndFiducials Point EndPath Landmark
    
    # if the path already existing is random, delete it
    if { $Path(random) == 1 } {
	EndoscopicDeletePath
    }
    if { $Path(numLandmarks) < 0} {
	puts "we have a problem!!! numLandmarks < 0"
    }
    set i $Path(numLandmarks)
    
    set Model(cLand,$i,x) $Model(cam,x)
    set Model(cLand,$i,y) $Model(cam,y)
    set Model(cLand,$i,z) $Model(cam,z)
    set Model(fLand,$i,x) $Model(fp,x)
    set Model(fLand,$i,y) $Model(fp,y)
    set Model(fLand,$i,z) $Model(fp,z)
    
    
    $Path(fLandmarkList) insert end "$Model(cLand,$i,x) $Model(cLand,$i,y) $Model(cLand,$i,z)"
    incr Path(numLandmarks)
    
    EndoscopicAddToMrml $i $Model(cLand,$i,x) $Model(cLand,$i,y) $Model(cLand,$i,z) $Model(fLand,$i,x) $Model(fLand,$i,y) $Model(fLand,$i,z)
    
    MainUpdateMRML
    
    foreach m {c f} {
   
	set numberOfInputPoints $Path(numLandmarks)
	
	# if we are building a new path, reset all the variables
	if { $numberOfInputPoints == 1 } {
	    Model(${m}Land,aSplineX) RemoveAllPoints
	    Model(${m}Land,aSplineY) RemoveAllPoints
	    Model(${m}Land,aSplineZ) RemoveAllPoints
	    Model(${m}Land,inputPoints) Reset
	    viewRen AddActor Model(${m}Land,actor)
	    viewRen AddActor Model(${m}Path,actor)
	    endRen RemoveActor Model(${m}Land,actor)
	    endRen RemoveActor Model(${m}Path,actor)
	}
	
	set i [expr $numberOfInputPoints - 1]
	Model(${m}Land,aSplineX) AddPoint $i $Model(${m}Land,$i,x)
	Model(${m}Land,aSplineY) AddPoint $i $Model(${m}Land,$i,y)
	Model(${m}Land,aSplineZ) AddPoint $i $Model(${m}Land,$i,z)
	Model(${m}Land,inputPoints) InsertPoint $i $Model(${m}Land,$i,x) $Model(${m}Land,$i,y) $Model(${m}Land,$i,z)
	set Path(landmarkExists) 1
	
	EndoscopicComputePath $m
    }
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicDeleteLandmark
#
# 
# 
# .END
#-------------------------------------------------------------------------------
proc EndoscopicDeleteLandmark {} {
    global Endoscopic Model View Path EndPath Landmark Fiducials EndFiducials Point View

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
	    
	    
	    MainMrmlUndoAddNode Landmark $Model(cam,$d,LandmarkNode)

	    for {set i $d} {$i<$Path(numLandmarks)} {incr i 1} {

		set Model(cLand,$i,x) $Model(cLand,[expr $i + 1],x)
		set Model(cLand,$i,y) $Model(cLand,[expr $i + 1],y)
		set Model(cLand,$i,z) $Model(cLand,[expr $i + 1],z)
		
		set Model(fLand,$i,x) $Model(fLand,[expr $i + 1],x)
		set Model(fLand,$i,y) $Model(fLand,[expr $i + 1],y)
		set Model(fLand,$i,z) $Model(fLand,[expr $i + 1],z)
		
		set Model(cam,$i,LandmarkNode) $Model(cam,[expr $i + 1],LandmarkNode)
		$Model(cam,$i,LandmarkNode) SetPathPosition $i
	    }
	    

	    MainUpdateMRML

	    foreach m {c f} {
		# we want to remove item d, so rebuild the whole spline
		Model(${m}Land,inputPoints) Reset
		set numberOfInputPoints $Path(numLandmarks)
		
		# clear the splines
		Model(${m}Land,aSplineX) RemoveAllPoints
		Model(${m}Land,aSplineY) RemoveAllPoints
		Model(${m}Land,aSplineZ) RemoveAllPoints
		
		for {set i 0} {$i<$numberOfInputPoints} {incr i 1} {
		    Model(${m}Land,aSplineX) AddPoint $i $Model(${m}Land,$i,x)
		    Model(${m}Land,aSplineY) AddPoint $i $Model(${m}Land,$i,y)
		    Model(${m}Land,aSplineZ) AddPoint $i $Model(${m}Land,$i,z)
		    Model(${m}Land,inputPoints) InsertPoint $i $Model(${m}Land,$i,x) $Model(${m}Land,$i,y) $Model(${m}Land,$i,z)
		    
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
    Model(${m}Land,polyData) Modified
    Model(${m}Land,points) Reset
    Model(${m}Path,lines) Reset 
    Model(${m}Path,polyData) Modified
    	
    set numberOfOutputPoints [expr $numberOfInputPoints * 10]
    if {$m == "c"} {
	$Path(stepScale) config -to [expr $numberOfOutputPoints - 1]
    }
    
    if { $numberOfInputPoints > 1 } {

	for {set i 0} {$i< $numberOfOutputPoints} {incr i 1} {
	    set t [expr ( $numberOfInputPoints - 1.0 ) / ( $numberOfOutputPoints - 1) * $i]
	    Model(${m}Land,points) InsertPoint $i [Model(${m}Land,aSplineX) Evaluate $t] [Model(${m}Land,aSplineY) Evaluate $t] [Model(${m}Land,aSplineZ) Evaluate $t]
	}
	set Path(exists) 1
	    
	Model(${m}Path,lines) InsertNextCell $numberOfOutputPoints
	
	for {set i 0} {$i< $numberOfOutputPoints} {incr i 1} {
	    Model(${m}Path,lines) InsertCellPoint $i
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
	global Endoscopic Path Fiducials EndFiducials Point Model Landmark EndPath

    if { $Path(exists) == 1 } {
	for {set d 0} {$d<$Path(numLandmarks)} {incr d 1} {
	    MainMrmlUndoAddNode Landmark $Model(cam,$d,LandmarkNode)	   
	}
	
	if { $Model(cam,PathNode) != ""} {
	    MainMrmlUndoAddNode Path $Model(cam,PathNode)
	    set Model(cam,PathNode) ""
	}
	if { $Model(cam,EndPathNode) != ""} {
	    MainMrmlUndoAddNode EndPath $Model(cam,EndPathNode)
	    set Model(cam,EndPathNode) ""
	}
	
	MainUpdateMRML
	
 	set Path(numLandmarks) 0

	foreach m {c f} {
	    MainRemoveActor Model(cLand,actor)
	    MainRemoveActor Model(fLand,actor)
	    MainRemoveActor Model(cPath,actor)
	    MainRemoveActor Model(fPath,actor)
	}
	
	$Path(fLandmarkList) delete 0 end
	set Path(exists) 0
	set Path(landmarkExists) 0
	set Path(random) 0
    }
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicComputeRandomPath
# 
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
	set Model(cLand,$i,x) $x
	set Model(cLand,$i,y) $y
	set Model(cLand,$i,z) $z
	set Model(fLand,$i,x) $x
	set Model(fLand,$i,y) $y
	set Model(fLand,$i,z) $z
	    
	$Path(fLandmarkList) insert end "$Model(cLand,$i,x) $Model(cLand,$i,y) $Model(cLand,$i,z)"
	set Path(numLandmarks) [expr $i + 1]

	EndoscopicAddToMrml $i $Model(cLand,$i,x) $Model(cLand,$i,y) $Model(cLand,$i,z) $Model(fLand,$i,x) $Model(fLand,$i,y) $Model(fLand,$i,z)	
    }
    MainUpdateMRML
    set Path(random) 1    
    EndoscopicCreateNewPath
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicViewPath
# 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc EndoscopicViewPath {} {
    global Endoscopic Model View Path Module
    
    set numPoints [Model(cLand,points) GetNumberOfPoints]
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
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc EndoscopicStopPath {} {
    global Endoscopic Model View Path Module
    	
	set Path(stop) 1
}

proc EndoscopicResetStopPath {} {
    global Endoscopic Model View Path Module
    	
	set Path(stop) 0
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicResetPath
# 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc EndoscopicResetPath {} {
    global Endoscopic Model View Path Module
    	
        EndoscopicStopPath
	set Path(stepStr) 0
        EndoscopicSetPathFrame
        EndoscopicResetStopPath
}


    
#-------------------------------------------------------------------------------
# .PROC EndoscopicSetPathFrame
# 
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
	    set l [Model(cLand,points) GetPoint $Path(i)] 
	    set l2 [Model(${which}Land,points) GetPoint [expr $Path(i) + $var ]]
	    EndoscopicSetFocalAndCameraPosition [lindex $l 0] [lindex $l 1] [lindex $l 2] [lindex $l2 0] [lindex $l2 1] [lindex $l2 2]	    
	} elseif { $Path(flyDirection) == "Backward" } {
	    set Path(i) $Path(stepStr)
	    set l [Model(cLand,points) GetPoint $Path(i)]
	    set l2 [Model(${which}Land,points) GetPoint [expr $Path(i) - $var ]]
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
# .END
#-------------------------------------------------------------------------------
proc EndoscopicAddToMrml {id cx cy cz fx fy fz} {
    global Model Landmark Path EndPath
    
    # do if there is none
    if { $Model(cam,PathNode) == "" && $Model(cam,EndPathNode) == "" } {
	set Model(cam,PathNode) [MainMrmlAddNode Path]  
	set Model(cam,EndPathNode) [MainMrmlAddNode EndPath]  
    }
    

    set Model(cam,$id,LandmarkNode) [MainMrmlInsertBeforeNode $Model(cam,EndPathNode) Landmark]
    set pid [$Model(cam,$id,LandmarkNode) GetID]
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
	    set Model(cam,axis) $axis
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
# .PROC EndoscopicViewSetBackgroundColor
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc EndoscopicViewSetBackgroundColor {} {
    global View
    
    # set the background
    eval endRen SetBackground $View(bgColor)
    
}


#-------------------------------------------------------------------------------
# .PROC EndoscopicSetModelsVisibilityInside
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc EndoscopicSetModelsVisibilityInside {} {
    global View Model Advanced

    if { $Advanced(ModelsVisibilityInside) == 0 } {
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

proc EndoscopicSetSliceDriver {a} {
    global Endoscopic Model View Slice


    # Change button text
    $Endoscopic(mbDriver) config -text $a
    
    
    if { $a == "User" } {
	foreach s $Slice(idList) {
		Slicer SetDriver $s 0
	}
	set Model(fp,driver) 0
	set Model(cam,driver) 0
    } else {
	foreach s $Slice(idList) {
	    Slicer SetDriver $s 1
	}
	if { $a == "Camera"} {
	    set m cam
	    set Model(fp,driver) 0
	    set Model(cam,driver) 1
	} elseif { $a == "FocalPoint"} {
	    set m fp 
	    set Model(fp,driver) 1
	    set Model(cam,driver) 0
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
    global Model View Slice

    # Force recomputation of the reformat matrix
    Slicer SetDirectNTP \
	    0 0 0  \
            0 0 0  \
	    $Model($a,x) $Model($a,y) $Model($a,z)
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicUpdateMRML
#
# Update the Fiducials
# .END
#-------------------------------------------------------------------------------
proc EndoscopicUpdateMRML {} {
	global Mrml Path Model


    Mrml(dataTree) ComputeTransforms
    Mrml(dataTree) InitTraversal
    set item [Mrml(dataTree) GetNextItem]
    while { $item != "" } {

	if { [$item GetClassName] == "vtkMrmlPathNode" } {
	    set Path(vtkNodeRead) 1
	    # if the Path is the same than the one we are working on,
	    # don't update anything
	    set pid [$item GetID]
	    if { $Model(cam,PathNode) != "" && $pid == [$Model(cam,PathNode) GetID]} {
		break
	    }   
	    # if there is no pathNode yet and we are reading one, set
	    # the Model(cam,PathNode) variable
	    if { $Model(cam,PathNode) == ""} {
		set Model(cam,PathNode) $item  
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
	    set Model(cLand,$i,x) [lindex $l 0]
	    set Model(cLand,$i,y) [lindex $l 1]
	    set Model(cLand,$i,z) [lindex $l 2]
	    set f [Landmark($lid,node) GetFXYZ]
	    set Model(fLand,$i,x) [lindex $f 0]
	    set Model(fLand,$i,y) [lindex $f 1]
	    set Model(fLand,$i,z) [lindex $f 2]
	    set Model(cam,$i,LandmarkNode) $item
	    $Path(fLandmarkList) insert end "$l"
	    set Path(numLandmarks) [expr $i + 1]
	}

	if { [$item GetClassName] == "vtkMrmlEndPathNode" } {
	    # if there is no EndPathNode yet and we are reaading one, set
	    # the Model(cam,PathNode) variable
	    if { $Model(cam,EndPathNode) == ""} {
		set Model(cam,EndPathNode) $item  
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

	puts "in delete path"

	# get rid of path actors
	foreach m {c f} {
	    MainRemoveActor Model(cLand,actor)
	    MainRemoveActor Model(fLand,actor)
	    MainRemoveActor Model(cPath,actor)
	    MainRemoveActor Model(fPath,actor)
	}
	
	# reset the pointers to the path nodes
	set Model(cam,PathNode) ""
	set Model(cam,EndPathNode) ""

	# clear the scroll text
	$Path(fLandmarkList) delete 0 end
	
	# reset the path variables
	set Path(exists) 0
	set Path(landmarkExists) 0
	set Path(random) 0
    } else {
	# create the path and reset the variable for the next MrmlUpdate
	set Path(vtkNodeRead) 0
	EndoscopicCreateNewPath	
    }
}

#-------------------------------------------------------------------------------
# .PROC EndoscopicCreateNewPath
#
# 
# 
# .END
#-------------------------------------------------------------------------------
proc EndoscopicCreateNewPath {} {
    
    global Endoscopic Model View Path View Fiducials EndFiducials Point
    
    if { $Path(numLandmarks) < 0} {
	puts "we have a problem!!! numLandmarks < 0"
    }
    
    set numberOfInputPoints $Path(numLandmarks)
    
    foreach m {c f} {
	Model(${m}Land,aSplineX) RemoveAllPoints
	Model(${m}Land,aSplineY) RemoveAllPoints
	Model(${m}Land,aSplineZ) RemoveAllPoints
	Model(${m}Land,inputPoints) Reset
	viewRen AddActor Model(${m}Land,actor)
	viewRen AddActor Model(${m}Path,actor)
	endRen RemoveActor Model(${m}Land,actor)
	endRen RemoveActor Model(${m}Path,actor)
	
	
	for {set i 0} {$i<$numberOfInputPoints} {incr i 1} {
	    Model(${m}Land,aSplineX) AddPoint $i $Model(${m}Land,$i,x)
	    Model(${m}Land,aSplineY) AddPoint $i $Model(${m}Land,$i,y)
	    Model(${m}Land,aSplineZ) AddPoint $i $Model(${m}Land,$i,z)
	    Model(${m}Land,inputPoints) InsertPoint $i $Model(${m}Land,$i,x) $Model(${m}Land,$i,y) $Model(${m}Land,$i,z)
	}
	set Path(landmarkExists) 1
	
	EndoscopicComputePath $m
    }
}




