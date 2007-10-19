#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: Neuroendoscopy.tcl,v $
#   Date:      $Date: 2007/10/19 16:47:06 $
#   Version:   $Revision: 1.1.2.12 $
# 
#===============================================================================
# FILE:        Neuroendoscopy.tcl
# PROCEDURES:  
#   NeuroendoscopyEnter
#   NeuroendoscopyExit
#   NeuroendoscopyInit
#   NeuroendoscopyCreateRenderer renName
#   NeuroendoscopyBuildVTK
#   NeuroendoscopyCreateCamera
#   NeuroendoscopyCameraParams size
#   NeuroendoscopyCreateFocalPoint
#   NeuroendoscopyCreateVTKPath id
#   NeuroendoscopyResetPathVariables id
#   NeuroendoscopyCreatePath id
#   NeuroendoscopyCreateVector id
#   NeuroendoscopyVectorParams id axislen axisrad conelen
#   NeuroendoscopyUpdateVisibility a visibility
#   NeuroendoscopySetPickable a pickability
#   NeuroendoscopyUpdateSize a
#   NeuroendoscopyPopBindings
#   NeuroendoscopyPushBindings
#   NeuroendoscopyCreateBindings
#   NeuroendoscopyBuildGUI
#   NeuroendoscopyShowFlyThroughPopUp x y
#   NeuroendoscopyExecutePathTab  command
#   NeuroendoscopyExecuteRegistrationTab command
#   NeuroendoscopyBuildFlyThroughGUI
#   NeuroendoscopyCreateLabelAndSlider f labelName labelHeight labelText sliderName orient from to length variable commandString entryWidth defaultSliderValue
#   NeuroendoscopyCreateCheckButton ButtonName VariableName Message Command Indicatoron Width
#   NeuroendoscopySetVisibility a
#   NeuroendoscopyCreateAdvancedGUI f a vis col size
#   NeuroendoscopySetActive a b
#   NeuroendoscopyPopupCallback
#   NeuroendoscopyUseGyro
#   NeuroendoscopyGyroMotion actor angle dotprod unitX unitY unitZ
#   NeuroendoscopySetGyroOrientation
#   NeuroendoscopySetWorldPosition x y z
#   NeuroendoscopySetWorldOrientation rx ry rz
#   NeuroendoscopySetCameraPosition value
#   NeuroendoscopyResetCameraPosition
#   NeuroendoscopySetCameraDirection value
#   NeuroendoscopyResetCameraDirection
#   NeuroendoscopyUpdateActorFromVirtualEndoscope vcam
#   NeuroendoscopyUpdateVirtualEndoscope  vcam coordList
#   NeuroendoscopyLightFollowEndoCamera vcam
#   NeuroendoscopySetCameraZoom
#   NeuroendoscopySetCameraViewAngle
#   NeuroendoscopySetCameraAxis axis
#   NeuroendoscopyCameraMotionFromUser
#   NeuroendoscopySetCollision value
#   NeuroendoscopyMoveGyroToLandmark id
#   NeuroendoscopyUpdateVectors id
#   NeuroendoscopyGetAvailableListName model
#   NeuroendoscopyAddLandmarkNoDirectionSpecified x y z list
#   NeuroendoscopyInsertLandmarkNoDirectionSpecified afterPid x y z list
#   NeuroendoscopyAddLandmarkDirectionSpecified coords list
#   NeuroendoscopyInsertLandmarkDirectionSpecified coords list
#   NeuroendoscopyUpdateLandmark
#   NeuroendoscopyBuildInterpolatedPath id
#   NeuroendoscopyDeletePath
#   NeuroendoscopyComputeRandomPath
#   NeuroendoscopyShowPath
#   NeuroendoscopyFlyThroughPath listOfCams listOfPaths
#   NeuroendoscopySetPathFrame listOfCams listOfPaths
#   NeuroendoscopyStopPath
#   NeuroendoscopyResetStopPath
#   NeuroendoscopyResetPath listOfCams listOfPaths
#   NeuroendoscopySetSpeed
#   NeuroendoscopyCheckDriver vcam
#   NeuroendoscopyReformatSlices vcam x y z
#   NeuroendoscopySetSliceDriver name
#   NeuroendoscopyFiducialsPointSelectedCallback fid pid
#   NeuroendoscopyFiducialsPointCreatedCallback type fid pid
#   NeuroendoscopyUpdateMRML
#   NeuroendoscopyStartCallbackFiducialsUpdateMRML
#   NeuroendoscopyEndCallbackFiducialsUpdateMRML
#   NeuroendoscopyCallbackFiducialsUpdateMRML type id listOfPoints
#   NeuroendoscopyCreateAndActivatePath name
#   NeuroendoscopyFiducialsActivatedListCallback type name id
#   NeuroendoscopySelectActivePath id
#   NeuroendoscopyDistanceBetweenTwoPoints p1x p1y p1z p2x p2y p2z
#   NeuroendoscopyUpdateSelectionLandmarkList id
#   NeuroendoscopySetModelsVisibilityInside
#   NeuroendoscopySetSlicesVisibility
#   NeuroendoscopyUpdateNeuroendoscopyViewVisibility
#   NeuroendoscopyUpdateMainViewVisibility
#   NeuroendoscopyAddNeuroendoscopyView
#   NeuroendoscopyAddMainView
#   NeuroendoscopyAddNeuroendoscopyViewRemoveMainView
#   NeuroendoscopyRemoveNeuroendoscopyView
#   NeuroendoscopyRemoveMainView
#   NeuroendoscopyAddMainViewRemoveNeuroendoscopyView
#   NeuroendoscopyAutoSelectSourceSink list
#   NeuroendoscopySetFlatFileName
#   NeuroendoscopyCancelFlatFile
#   NeuroendoscopyAddFlatView
#   NeuroendoscopyRemoveFlatView name
#   NeuroendoscopyCreateFlatBindings widget
#   NeuroendoscopyPopFlatBindings
#   NeuroendoscopyBuildFlatColonLookupTable name
#   NeuroendoscopyBuildFlatBoundary name
#   NeuroendoscopyMouseLocation widget xcoord ycoord
#   NeuroendoscopyStartPan widget xcoord ycoord
#   NeuroendoscopyEndPan widget xcoord ycoord
#   NeuroendoscopyStartZoom widget ycoord
#   NeuroendoscopyEndZoom widget ycoord
#   NeuroendoscopyPickFlatPoint widget xcoord ycoord
#   NeuroendoscopyAddTargetInFlatWindow widget x y z
#   NeuroendoscopyAddTargetFromFlatColon pointId
#   NeuroendoscopyAddTargetFromSlices x y z
#   NeuroendoscopyAddTargetFromWorldCoordinates sx sy sz
#   NeuroendoscopyLoadTargets
#   NeuroendoscopyMainFileCloseUpdated
#   NeuroendoscopyCreateTarget
#   NeuroendoscopyUpdateActiveTarget
#   NeuroendoscopyDeleteActiveTarget
#   NeuroendoscopySelectTarget sT
#   NeuroendoscopySelectNextTarget
#   NeuroendoscopySelectPreviousTarget
#   NeuroendoscopyUpdateTargetsInFlatWindow widget
#   NeuroendoscopyFlatLightElevationAzimuth widget Neuroendoscopy(flatColon,LightElev) Neuroendoscopy(flatColon,LightAzi)
#   NeuroendoscopyMoveCameraX widget Neuroendoscopy(flatColon,xCamDist)
#   NeuroendoscopyMoveCameraY widget Neuroendoscopy(flatColon,yCamDist)
#   NeuroendoscopyMoveCameraZ widget Neuroendoscopy(flatColon,zCamDist)
#   NeuroendoscopyResetFlatCameraDist widget
#   NeuroendoscopyScrollRightFlatColon widget
#   NeuroendoscopyScrollLeftFlatColon widget
#   NeuroendoscopyStopFlatColon
#   NeuroendoscopyResetFlatColon widget
#   NeuroendoscopyResetStop
#   NeuroendoscopySetFlatColonScalarVisibility widget
#   NeuroendoscopySetFlatColonScalarRange widget
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyEnter
# Called when this module is entered by the user.<br>
# Effects: Pushes the event manager for this module and 
#          calls NeuroendoscopyAddNeuroendoscopyView. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyEnter {} {
    global Neuroendoscopy View viewWin viewRen Fiducials Csys Model Module Mrml Matrix
    

    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)

    # push initial bindings
    NeuroendoscopyPushBindings
    


    # show the actors based on their visibility parameter
    #foreach a $Neuroendoscopy(actors) {
    #NeuroendoscopyUpdateVisibility $a
    #}
    foreach a $Neuroendoscopy(actors) {
        viewRen AddActor $a
        NeuroendoscopySetPickable $a 1
    }
    set Csys(active) 1

showAllTextures

    Render3D
    if {$Neuroendoscopy(endoview,visibility) == 1} {
#EndoscopicAddEndoscopicView
#     NeuroendoscopyRemoveMainView
NeuroendoscopyRemoveMainView
NeuroendoscopyAddMainView
NeuroendoscopyRemoveNeuroendoscopyView
NeuroendoscopyAddNeuroendoscopyView
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyExit
# Called when this module is exited by the user.<br>
#
# Removes the endo and/or the flat view if hide on exit flags are set.
# Pops the event manager for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyExit {} {
    global Neuroendoscopy Csys
    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    if {$Neuroendoscopy(endoview,hideOnExit) == 1} {
        # reset the slice driver
        NeuroendoscopySetSliceDriver User
        
        # set all neuroendoscopy actors to be invisible, without changing their 
        # visibility parameters
        foreach a $Neuroendoscopy(actors) {
            viewRen RemoveActor $a
            NeuroendoscopySetPickable $a 0
                    
        }
        set Csys(active) 0
    
        Render3D
        NeuroendoscopyRemoveNeuroendoscopyView
    }
    
    if {$Neuroendoscopy(flatview,hideOnExit) == 1} {
        NeuroendoscopyRemoveFlatView
    }
    NeuroendoscopyPopBindings
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyInit
#  The "Init" procedure is called automatically by the slicer.  
#  <br>
#
#  effects: * It puts information about the module into a global array called 
#             Module. <br>
#           * It adds a renderer called Neuroendoscopy(activeCam) to the global array 
#             View and adds the new renderer to the list of renderers in 
#             Module(renderers) <br> 
#           * It also initializes module-level variables (in the global array 
#             Neuroendoscopy and Path) <br>
#             The global array Neuroendoscopy contains information about the 
#             7 actors created in this module (the list is saved in Neuroendoscopy(actors): <br>
#        regular actors: <br>
#               cam: the neuroendoscopy camera <br>
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
proc NeuroendoscopyInit {} {
    global Neuroendoscopy Module Model Path Advanced View Gui Fiducials
    

    set m Neuroendoscopy
    set Module($m,row1List) "Help Display Registration Camera"
    set Module($m,row1Name) "{Help} {Display} {Registration} {Camera}"
    set Module($m,row1,tab) Help    
 
    set Module($m,depend) ""
    
    # module summary
    set Module($m,overview) "Position an neuroendoscopy camera in the scene and view through the camera's lens in the second window.
                Provides correspondence between 3D Views, 2D Slices, and flattened colon View"
    set Module($m,author) "Delphine Nain, MIT, delfin@ai.mit.edu; Jeanette Meng SPL meng@bwh.harvard.edu"
    set Module($m,category) "Visualisation"
    
    lappend Module(versions) [ParseCVSInfo $m \
    {$Revision: 1.1.2.12 $} {$Date: 2007/10/19 16:47:06 $}] 
       
    # Define Procedures
    #------------------------------------
    
    set Module($m,procVTK) NeuroendoscopyBuildVTK
    set Module($m,procGUI) NeuroendoscopyBuildGUI
    set Module($m,procCameraMotion) NeuroendoscopyCameraMotionFromUser
    set Module($m,procXformMotion) NeuroendoscopyGyroMotion
    set Module($m,procEnter) NeuroendoscopyEnter
    set Module($m,procExit) NeuroendoscopyExit
    set Module($m,procMRML) NeuroendoscopyUpdateMRML

    # jeanette: reset neuroendoscopy global variables
    set Module($m,procMainFileCloseUpdateEntered) NeuroendoscopyMainFileCloseUpdated
    
    # callbacks so that the neuroendoscopy module knows when things happen with the
    # fiducials
    lappend Module($m,fiducialsStartUpdateMRMLCallback) NeuroendoscopyStartCallbackFiducialsUpdateMRML
    lappend Module($m,fiducialsEndUpdateMRMLCallback) NeuroendoscopyEndCallbackFiducialsUpdateMRML
    lappend Module($m,fiducialsCallback) NeuroendoscopyCallbackFiducialsUpdateMRML
    lappend Module($m,fiducialsPointSelectedCallback) NeuroendoscopyFiducialsPointSelectedCallback
    lappend Module($m,fiducialsPointCreatedCallback) NeuroendoscopyFiducialsPointCreatedCallback
    lappend Module($m,fiducialsActivatedListCallback) NeuroendoscopyFiducialsActivatedListCallback
    
    # give a defaultList name for neuroendoscopy Fiducials
    set Fiducials(Neuroendoscopy,Path,defaultList) "path"


    # create a second renderer here in Init so that it is added to the list 
    # of Renderers before MainActorAdd is called anywhere
    # That way any actor added to viewRen (the MainView's renderer) is also 
    # added to neuroendoscopyScreen and will appear on the second window once we decide 
    # to show it
    
    # by default we only know about this renderers, so add it to 
    # the list of the neuroendoscopy renderers we care about

    #NeuroendoscopyCreateRenderer neuroendoscopyScreen2     
    #lappend Module(neuroendoscopyRenderers) neuroendoscopyScreen2
    NeuroendoscopyCreateRenderer neuroendoscopyScreen 
    lappend Module(neuroendoscopyRenderers) neuroendoscopyScreen
    
    
    set Neuroendoscopy(activeCam) [neuroendoscopyScreen GetActiveCamera]
    
    # create the second renderer now so that all things are initialized for
    # it
    # only add it to the list of neuroendoscopy renderers we care about
    # once the user pressed the button in the Sync tab

    
    ### create bindings
    NeuroendoscopyCreateBindings
    
    # Initialize module-level variables
    #------------------------------------
    
    set Neuroendoscopy(count) 0
    set Neuroendoscopy(eventManager)  ""
    set Neuroendoscopy(actors) ""
    set Neuroendoscopy(mbPathList) ""
    set Neuroendoscopy(mPathList) ""
    set Neuroendoscopy(plane,offset) 0      
    set Neuroendoscopy(selectedFiducialPoint) ""
    set Neuroendoscopy(selectedFiducialList) ""
    
    
            # jeanette Initialize flat-view variables
#--------------------------------------------------------------------------------    

    
    # names of the views/models selected
    set Neuroendoscopy(ModelSelect) $Model(idNone)
    set Neuroendoscopy(FlatSelect) ""
    set Neuroendoscopy(flatColon,name) ""
    
    # keep lists of renderers and flatwindows that will be opened
    set Neuroendoscopy(FlatRenderers) ""    
    set Neuroendoscopy(FlatWindows) ""
    
    set Neuroendoscopy(default,lineCount) 0
    
    # flat viewer
    set Neuroendoscopy(RemoveEndo) 0
    
    set Flat(mbActiveList) ""
    set Flat(mActiveList) ""
    
    set Neuroendoscopy(targetList,name) ""
    set Neuroendoscopy(targetList,points) ""
    
    # bounding box parameters for the flat colon
    set Neuroendoscopy(flatColon,xMin) 0.0
    set Neuroendoscopy(flatColon,xMax) 0.0
    set Neuroendoscopy(flatColon,yMin) 0.0 
    set Neuroendoscopy(flatColon,yMax) 0.0
    set Neuroendoscopy(flatColon,zMin) 0.0
    set Neuroendoscopy(flatColon,zMax) 0.0
    
    set Neuroendoscopy(flatColon,zOpt) 0.0
    set Neuroendoscopy(flatColon,yMid) 0.0
    
    set Neuroendoscopy(flatColon,xCamDist) 0.0
    set Neuroendoscopy(flatColon,yCamDist) 5.0
#   trace add variable Neuroendoscopy(flatColon,yCamDist) write NeuroendoscopyTempProc
    set Neuroendoscopy(flatColon,zCamDist) 5.0
    
    set Neuroendoscopy(flatColon,speed) 0.5
    set Neuroendoscopy(flatColon,stop) 0
    
    # lights
    set Neuroendoscopy(flatColon,LightElev) 0
    set Neuroendoscopy(flatColon,LightAzi) 0
    
    # flatcolon scalar visibility and range
    set Neuroendoscopy(flatColon,scalarVisibility) 0
    set Neuroendoscopy(flatColon,scalarLow) 0
    set Neuroendoscopy(flatColon,scalarHigh) 100
    
    # number of targets
    set Neuroendoscopy(totalTargets) 0
    set Neuroendoscopy(selectedTarget) 0
        
#--------------------------------------------------------------------------------    


    set Neuroendoscopy(Cameras) ""
    # path planning
    set Neuroendoscopy(source,exists) 0
    set Neuroendoscopy(sourceButton,on) 0
    set Neuroendoscopy(sinkButton,on) 0

    # Camera variables
    # don't change these default values, change Neuroendoscopy(cam,size) instead
    
    set Neuroendoscopy(cam,name) "Camera"
    set Neuroendoscopy(cam,visibility) 1    
    set Neuroendoscopy(cam,size) 1
    set Neuroendoscopy(cam,boxlength) 30
    set Neuroendoscopy(cam,boxheight) 20
    set Neuroendoscopy(cam,boxwidth)  10
    set Neuroendoscopy(cam,x) 0
    set Neuroendoscopy(cam,y) 0
    set Neuroendoscopy(cam,z) 0
    set Neuroendoscopy(cam,xRotation) 0
    set Neuroendoscopy(cam,yRotation) 0
    set Neuroendoscopy(cam,zRotation) 0
    set Neuroendoscopy(cam,FPdistance) 30
    set Neuroendoscopy(cam,viewAngle) 90
    set Neuroendoscopy(cam,AngleStr) 90
    set Neuroendoscopy(cam,viewUpX) 0 
    set Neuroendoscopy(cam,viewUpY) 0 
    set Neuroendoscopy(cam,viewUpZ) 1
    set Neuroendoscopy(cam,viewPlaneNormalX) 0
    set Neuroendoscopy(cam,viewPlaneNormalY) 1
    set Neuroendoscopy(cam,viewPlaneNormalZ) 0
    set Neuroendoscopy(cam,driver) 0
    set Neuroendoscopy(cam,PathNode) ""
    set Neuroendoscopy(cam,EndPathNode) ""
    set Neuroendoscopy(cam,xStr) 0
    set Neuroendoscopy(cam,yStr) 0
    set Neuroendoscopy(cam,zStr) 0
    set Neuroendoscopy(cam,xStr,old) 0
    set Neuroendoscopy(cam,yStr,old) 0
    set Neuroendoscopy(cam,zStr,old) 0
    set Neuroendoscopy(cam,rxStr) 0
    set Neuroendoscopy(cam,ryStr) 0
    set Neuroendoscopy(cam,rzStr) 0
    set Neuroendoscopy(cam,rxStr,old) 0
    set Neuroendoscopy(cam,ryStr,old) 0
    set Neuroendoscopy(cam,rzStr,old) 0
    set Neuroendoscopy(cam,shape) "oldstyle neuroendoscope"
    set Neuroendoscopy(cam,shapetemp) ""
    

    set Neuroendoscopy(sliderx) ""
    set Neuroendoscopy(slidery) ""
    set Neuroendoscopy(sliderz) ""
    set Neuroendoscopy(Box,name) "Camera Box"
    set Neuroendoscopy(Box,color) "1 .4 .5" 

    set Neuroendoscopy(Box2,name) "Camera Box 2"
    set Neuroendoscopy(Box2,color) "0 0 0" 

    set Neuroendoscopy(Lens,name) "Camera Lens"
    set Neuroendoscopy(Lens,color) ".4 .2 .6" 
    
    set Neuroendoscopy(Lens2,name) "Camera Lens 2"
    set Neuroendoscopy(Lens2,color) "0 0 0"
    
    set Neuroendoscopy(Lens3,name) "Camera Lens 3"
    set Neuroendoscopy(Lens3,color) "0 0 1"

    set Neuroendoscopy(fp,name) "Focal Point"    
    set Neuroendoscopy(fp,visibility) 1    
    set Neuroendoscopy(fp,size) 4
    set Neuroendoscopy(fp,color) ".2 .6 .8"
    set Neuroendoscopy(fp,x) 0
    set Neuroendoscopy(fp,y) 30
    set Neuroendoscopy(fp,z) 0
    set Neuroendoscopy(fp,driver) 0

    set Neuroendoscopy(intersection,driver) 0    
    set Neuroendoscopy(intersection,x) 0    
    set Neuroendoscopy(intersection,y) 0    
    set Neuroendoscopy(intersection,z) 0    
    # if it is absolute, the camera will move along the 
    #  RA/IS/LR axis
    # if it is relative, the camera will move along its
    #  own axis 
    set Neuroendoscopy(cam,axis) relative
    
    # Neuroendoscopy variables
    


    set Neuroendoscopy(path,size) .5 
    set Neuroendoscopy(path,color) ".4 .2 .6" 
    
    set Neuroendoscopy(path,activeId) None
    # keeps track of all the Ids of the path that currently exist
    # on the screen
    set Neuroendoscopy(path,activeIdList) ""
    # keeps track of all path Ids that have ever existed
    set Neuroendoscopy(path,allIdsUsed) ""
    set Neuroendoscopy(path,nextAvailableId) 1
    set Neuroendoscopy(randomPath,nextAvailableId) 1
    

    set Neuroendoscopy(vector,name) "Vectors"
    set Neuroendoscopy(vector,size) 5 
    set Neuroendoscopy(vector,visibility) 1    
    set Neuroendoscopy(vector,color) ".2 .6 .8"
    set Neuroendoscopy(vector,selectedID) 0

    set Neuroendoscopy(gyro,name) "3D Gyro"
    set Neuroendoscopy(gyro,visibility) 1
    set Neuroendoscopy(gyro,size) 100
   
    set Neuroendoscopy(gyro,use) 1
    
    #Advanced variables
    set Neuroendoscopy(ModelsVisibilityInside) 1
    set Neuroendoscopy(SlicesVisibility) 1
    set Neuroendoscopy(collision) 0
    set Neuroendoscopy(collDistLabel) ""
    set Neuroendoscopy(collMenu) ""

    # Path variable
    set Neuroendoscopy(path,flyDirection) "Forward"
    set Neuroendoscopy(path,speed) 1
    set Neuroendoscopy(path,random) 0
    set Neuroendoscopy(path,first) 1
    set Neuroendoscopy(path,i) 0
    set Neuroendoscopy(path,stepStr) 0
    set Neuroendoscopy(path,exists) 0
    set Neuroendoscopy(path,numLandmarks) 0
    set Neuroendoscopy(path,stop) 0
    set Neuroendoscopy(path,vtkNodeRead) 0
    
    set Neuroendoscopy(path,interpolationStr) 1
    # set Colors
    set Neuroendoscopy(path,cColor) [MakeColor "0 204 255"]
    set Neuroendoscopy(path,sColor) [MakeColor "204 255 255"]
    set Neuroendoscopy(path,eColor) [MakeColor "255 204 204"]
    set Neuroendoscopy(path,rColor) [MakeColor "204 153 153"]
    
    set LastX 0
    
     # viewers 
    set Neuroendoscopy(mainview,visibility) 1
    set Neuroendoscopy(endoview,visibility) 1
    set Neuroendoscopy(endoview,hideOnExit) 0
    # jeanette/
    set Neuroendoscopy(flatview,hideOnExit) 0
    set Neuroendoscopy(viewOn) 0i

    set Model(propertyType) STEP1

    set Neuroendoscopy(visibility) 0
    set Neuroendoscopy(rulerVisibility) 0
    set Neuroendoscopy(guide,length) 0
    set Neuroendoscopy(texture,rotate) 90
    set Neuroendoscopy(texture,xcoordi) 0
    set Neuroendoscopy(texture,ycoordi) 1
    set Neuroendoscopy(texture,angle) 25
    set Neuroendoscopy(texture,xpic) 0
    set Neuroendoscopy(texture,ypic) 1
    set Neuroendoscopy(texture,zpic) 0
    

    #how fast the camera will update if its locked to the tracking device
    set Neuroendoscopy(cam,polling) 25

    set Neuroendoscopy(texture,tranform) 0


    set Neuroendoscopy(texture,transform) ""
    set Neuroendoscopy(texture,cone) ""
    set Neuroendoscopy(texture,clipper) ""
    set Neuroendoscopy(texture,tmapper) ""
    set Neuroendoscopy(texture,xform) ""
    set Neuroendoscopy(texture,mapper) ""
    set Neuroendoscopy(texture,pngReader) ""
    set Neuroendoscopy(texture,atext) ""
    set Neuroendoscopy(texture,model) ""
    set Neuroendoscopy(texture,actors,id) 0
    set Neuroendoscopy(redLaser) ""
    set Neuroendoscopy(texture,LaserClipper) ""

    vtkNeuroendoscopy ver
    vtkMatrix4x4 Neuroendoscopy(texture,transmatrix)
    vtkMatrix4x4 Neuroendoscopy(texture,invertedmatrix)
    vtkOpenTracker Neuroendoscopy(OpenTracker,src)
    #vtkOpenTracker Locator(OpenTracker,src)
    vtkMatrix4x4 Neuroendoscopy(texture,identitymatrix)
    Neuroendoscopy(texture,identitymatrix) Identity
    set Neuroendoscopy(connect) ""
    set dir [pwd]
    set Locator(OpenTracker,cfg) [file join $dir defaultConfig.xml] 
    set Locator(OpenTracker,msPoll) 100
    set Locator(OpenTracker,multiBy) 1.0
    set Neuroendoscopy(OpenTracker,cfg) [file join $dir defaultConfig2.xml] 
    set Neuroendoscopy(OpenTracker,msPoll) 100
    set Neuroendoscopy(OpenTracker,multiBy) 1.0
    
    set Neuroendoscopy(tmp) 1.4

    vtkPoints  Neuroendoscopy(TargetPoints)
    vtkPoints  Neuroendoscopy(ControlPoints)
    set Neuroendoscopy(registration,points) 4
    set Neuroendoscopy(registration,tmp) 0
    set Neuroendoscopy(OpenTracker,started) 0
    vtkStructuredGrid Neuroendoscopy(OpenTracker,ICPControlSet)
    vtkStructuredGrid Neuroendoscopy(OpenTracker,ICPTargetSet)
    vtkIterativeClosestPointTransform Neuroendoscopy(ICPTransformation)
    vtkLandmarkTransform Neuroendoscopy(LandmarkTransformation)
    vtkTransform Neuroendoscopy(EndTransformation)
    vtkMatrix4x4 Neuroendoscopy(transmatrix)
    vtkPlaneSource Neuroendoscopy(icpplane1)
    vtkPlaneSource Neuroendoscopy(icpplane2)

    #set myproc [join $dir testset.pt]
    set Neuroendoscopy(fhandle) ""
    set Neuroendoscopy(loop)  0
    vtkCellArray Neuroendoscopy(strips)
    vtkPolyData Neuroendoscopy(pointcloud)
    vtkPolyDataMapper headmapper
    vtkPolyDataMapper headmapper2
    vtkPolyDataMapper headmapper3
    vtkActor actor2
    vtkActor actor
    vtkActor actor3

    set Neuroendoscopy(SnapShot) 0
    set Neuroendoscopy(cam,showAngle) 1

    set Neuroendoscopy(ImageCounter) 0
    set Neuroendoscopy(pointPairList) ""
    set Neuroendoscopy(locator1old) 0
    set Neuroendoscopy(locator2old) 0
    set Neuroendoscopy(locator3old) 0
   set Neuroendoscopy(boolean) 0
   set Neuroendoscopy(shortestDistance) 1
   set Neuroendoscopy(cam,zoom) 1
   set Neuroendoscopy(needle,orientX) 0
   set Neuroendoscopy(needle,orientY) 0
   set Neuroendoscopy(needle,orientZ) 0
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateRenderer
# Creates the named renderer.
# .ARGS
# str renName the name to give this renderer.
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreateRenderer {renName} {

    global Neuroendoscopy $renName View Module

    vtkRenderer $renName
    lappend Module(Renderers) $renName    
    eval $renName SetBackground $View(bgColor)
    
    set cam [$renName GetActiveCamera]
    # so that the camera knows about its renderer
    
    set View($cam,renderer) $renName
    
    $cam SetViewAngle 120
    lappend Neuroendoscopy(Cameras) $cam
    
    vtkLight View($cam,light)
    vtkLight View($cam,light2)
    
    vtkLightKit cView(light3)
    cView(light3) SetKeyLightIntensity 0.7
    cView(light3) SetKeyToFillRatio 1.5
    cView(light3) SetKeyToHeadRatio 1.75
    cView(light3) SetKeyToBackRatio 3.75
    
    #puts "renderer $renName, light obj View($View($camName,cam),light"
    #View($View($camName,cam),light) SetLightTypeToCameraLight
    #View($View($camName,cam),light2) SetLightTypeToSceneLight
#test
   # $renName AddLight View($cam,light)
   # $renName AddLight View($cam,light2)
#test end    
    cView(light3) AddLightsToRenderer $renName
    # initial settings. 
    # These parameters are then set in NeuroendoscopyUpdateVirtualEndoscope
    $cam SetPosition 0 0 0
    $cam SetFocalPoint 0 30 0
    $cam SetViewUp 0 0 1
    $cam ComputeViewPlaneNormal        
    set View(neuroendoscopyClippingRange) ".01 1000"
    eval $cam SetClippingRange $View(neuroendoscopyClippingRange)

}

#############################################################################
#
#     PART 1: create vtk actors and parameters 
#
#############################################################################


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyBuildVTK
#  Creates the vtk objects for this module.<br>
#  
#  Effects: calls NeuroendoscopyCreateFocalPoint, 
#           NeuroendoscopyCreateCamera, NeuroendoscopyCreateLandmarks and 
#           NeuroendoscopyCreatePath, NeuroendoscopyCreateGyro, NeuroendoscopyCreateVector   
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyBuildVTK {} {
    global Neuroendoscopy Model Csys
    

    vtkCellPicker Neuroendoscopy(picker)
    Neuroendoscopy(picker) SetTolerance 0.0001
    vtkMath Neuroendoscopy(path,vtkmath)
    
    # create the 3D mouse
    CsysCreate Neuroendoscopy gyro -1 -1 -1
    lappend Neuroendoscopy(actors) Neuroendoscopy(gyro,actor) 
    
    # create the focal point actor
    NeuroendoscopyCreateFocalPoint
    lappend Neuroendoscopy(actors) Neuroendoscopy(fp,actor) 
    
    # create the camera actor (needs to be created after the gyro
    # since it uses the gyro's matrix as its user matrix
    NeuroendoscopyCreateCamera
    lappend Neuroendoscopy(actors) Neuroendoscopy(cam,actor)
    
    #update the virtual camera
    NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam)
    
    # add the camera, fp, gyro actors only to viewRen, not neuroendoscopyScreen
    # set their visibility to 0 until we enter the module
    #viewRen AddActor Neuroendoscopy(cam,actor)

    Neuroendoscopy(cam,actor) SetVisibility 1
    NeuroendoscopySetPickable Neuroendoscopy(cam,actor) 0
    #viewRen AddActor Neuroendoscopy(fp,actor)
    Neuroendoscopy(fp,actor) SetVisibility 1
    NeuroendoscopySetPickable Neuroendoscopy(fp,actor) 0
    #viewRen AddActor Neuroendoscopy(gyro,actor)
    Neuroendoscopy(gyro,actor) SetVisibility 1

     # create the vectors base look
    vtkCylinderSource ctube
    vtkConeSource carrow
    vtkTransform ctubeXform
    vtkTransform carrowXform
    vtkTransformPolyDataFilter ctubeXformFilter
    ctubeXformFilter SetInput [ctube GetOutput]
    ctubeXformFilter SetTransform ctubeXform
    vtkTransformPolyDataFilter carrowXformFilter
    carrowXformFilter SetInput [carrow GetOutput]
    carrowXformFilter SetTransform carrowXform

    vtkTransform Neuroendoscopy(texture,transform) 
    vtkCone Neuroendoscopy(texture,cone)
    vtkCylinder Neuroendoscopy(texture,cylinder)    
    vtkClipPolyData Neuroendoscopy(texture,clipper) 
    vtkClipPolyData Neuroendoscopy(texture,LaserClipper)
    
    vtkProjectedTexture Neuroendoscopy(texture,tmapper) 
    vtkTransformTextureCoords Neuroendoscopy(texture,xform) 
    vtkDataSetMapper Neuroendoscopy(texture,mapper) 
    vtkPNMReader Neuroendoscopy(texture,pngReader) 
    vtkTexture Neuroendoscopy(texture,atext) 
    vtkPlane Neuroendoscopy(texture,frontPlane)
    vtkPlane Neuroendoscopy(texture,basePlane)
    vtkImplicitBoolean Neuroendoscopy(texture,theCone)
    vtkImplicitBoolean Neuroendoscopy(redLaser)
    
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateCamera
#  Create the Camera vtk actor
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreateCamera {} {
    global Neuroendoscopy
   

#------------------------------------
#old looking camera
#------------------------------------

    vtkCubeSource ccamCube
    vtkTransform cCubeXform        
    vtkCubeSource ccamCube2
    vtkTransform cCubeXform2        
    vtkCubeSource ccamCube3
    vtkTransform cCubeXform3        
    vtkCylinderSource ccamCyl
    vtkTransform cCylXform        
    vtkCylinderSource ccamCyl2
    vtkTransform cCylXform2        
    
    vtkConeSource ccamAngle
    vtkTransform cConeXform



    NeuroendoscopyCameraParams -1 "oldstyle"


    # make the actor (camera)



    
    vtkTransformPolyDataFilter cCylXformFilter
    cCylXformFilter SetInput [ccamCyl GetOutput]
    cCylXformFilter SetTransform cCylXform
    
    vtkTransformPolyDataFilter cCylXformFilter2
    cCylXformFilter2 SetInput [ccamCyl2 GetOutput]
    cCylXformFilter2 SetTransform cCylXform2
    
    vtkTransformPolyDataFilter cCubeXformFilter
    cCubeXformFilter SetInput [ccamCube GetOutput]
    cCubeXformFilter SetTransform cCubeXform
    
    vtkTransformPolyDataFilter cCubeXformFilter2
    cCubeXformFilter2 SetInput [ccamCube2 GetOutput]
    cCubeXformFilter2 SetTransform cCubeXform2
    
    vtkTransformPolyDataFilter cCubeXformFilter3
    cCubeXformFilter3 SetInput [ccamCube3 GetOutput]
    cCubeXformFilter3 SetTransform cCubeXform3
    
    vtkTransformPolyDataFilter cConeXformFilter
    cConeXformFilter SetInput [ccamAngle GetOutput]
    cConeXformFilter SetTransform cConeXform


    #---------------------------------------------------
    # Mapper
    #---------------------------------------------------
    vtkPolyDataMapper cBoxMapper
    cBoxMapper SetInput [cCubeXformFilter GetOutput]
    
    vtkPolyDataMapper cBoxMapper2
    cBoxMapper2 SetInput [cCubeXformFilter2 GetOutput]
    
    vtkPolyDataMapper cBoxMapper3
    cBoxMapper3 SetInput [cCubeXformFilter3 GetOutput]
    
    vtkPolyDataMapper cLensMapper
    cLensMapper SetInput [cCylXformFilter GetOutput]
    
    vtkPolyDataMapper cLensMapper2
    cLensMapper2 SetInput [cCylXformFilter2 GetOutput]
    
    vtkPolyDataMapper cLensMapper3
    cLensMapper3 SetInput [cConeXformFilter GetOutput]
    
    #---------------------------------------------------
    # Actors
    #---------------------------------------------------
    
    vtkActor Neuroendoscopy(Box,actor)
    Neuroendoscopy(Box,actor) SetMapper cBoxMapper
    eval [Neuroendoscopy(Box,actor) GetProperty] SetColor $Neuroendoscopy(Box,color)
    Neuroendoscopy(Box,actor) PickableOff
    
    vtkActor Neuroendoscopy(Box2,actor)
    Neuroendoscopy(Box2,actor) SetMapper cBoxMapper2
    eval [Neuroendoscopy(Box2,actor) GetProperty] SetColor $Neuroendoscopy(Box,color) 
    Neuroendoscopy(Box2,actor) PickableOff
    
    vtkActor Neuroendoscopy(Box3,actor)
    Neuroendoscopy(Box3,actor) SetMapper cBoxMapper3
    eval [Neuroendoscopy(Box3,actor) GetProperty] SetColor $Neuroendoscopy(Box2,color) 
    Neuroendoscopy(Box3,actor) PickableOff
    
    vtkActor Neuroendoscopy(Lens,actor)
    Neuroendoscopy(Lens,actor) SetMapper cLensMapper
    eval [Neuroendoscopy(Lens,actor) GetProperty] SetColor $Neuroendoscopy(Lens,color) 
    Neuroendoscopy(Lens,actor) PickableOff
    
    vtkActor Neuroendoscopy(Lens2,actor)
    Neuroendoscopy(Lens2,actor) SetMapper cLensMapper2
    eval [Neuroendoscopy(Lens2,actor) GetProperty] SetColor $Neuroendoscopy(Lens2,color) 
    Neuroendoscopy(Lens2,actor) PickableOff
    
    vtkActor Neuroendoscopy(Lens3,actor)
    Neuroendoscopy(Lens3,actor) SetMapper cLensMapper3
    eval [Neuroendoscopy(Lens3,actor) GetProperty] SetColor $Neuroendoscopy(Lens3,color) 
    eval [Neuroendoscopy(Lens3,actor) GetProperty] SetOpacity 0.5 
    Neuroendoscopy(Lens3,actor) PickableOff
    
    #---------------------------------------------------
    # Actor addings
    #---------------------------------------------------
    
    set Neuroendoscopy(cam,actor) [vtkAssembly Neuroendoscopy(cam,actor)]
    Neuroendoscopy(cam,actor) AddPart Neuroendoscopy(Box,actor)
    Neuroendoscopy(cam,actor) AddPart Neuroendoscopy(Box2,actor)
    Neuroendoscopy(cam,actor) AddPart Neuroendoscopy(Box3,actor)
    Neuroendoscopy(cam,actor) AddPart Neuroendoscopy(Lens,actor)
    Neuroendoscopy(cam,actor) AddPart Neuroendoscopy(Lens2,actor)
    Neuroendoscopy(cam,actor) AddPart Neuroendoscopy(Lens3,actor)

    Neuroendoscopy(cam,actor) PickableOn

    # set the user matrix of the camera and focal point to be the matrix of
    # the gyro
    # the full matrix of the cam and fp is a concatenation of their matrix and
    # their user matrix
    Neuroendoscopy(cam,actor) SetUserMatrix [Neuroendoscopy(gyro,actor) GetMatrix]
    Neuroendoscopy(fp,actor) SetUserMatrix [Neuroendoscopy(gyro,actor) GetMatrix]

    
    #------------------------------------
    #new looking camera
    #------------------------------------
    
    


}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCameraParams
# effects: Set the size parameters for the camera and the focal point
# .ARGS 
# int size (optional), 1 by default
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCameraParams {{size -1} {shape ""}} {
    global Neuroendoscopy
    
    if { $size == -1 } { 
        set Neuroendoscopy(cam,size) 1
    } else {
        set Neuroendoscopy(cam,size) $size
    }
    # set parameters for cube (body) geometry and transform
    set Neuroendoscopy(cam,shapetemp) $shape
    
    if { $shape == "neuroendoscope" } { 
    #here is the ol
    
    set Neuroendoscopy(cam,boxlength) [expr $Neuroendoscopy(cam,size) * 6]
    set Neuroendoscopy(cam,boxheight) [expr $Neuroendoscopy(cam,size) * 50]
    set Neuroendoscopy(cam,boxwidth)  [expr $Neuroendoscopy(cam,size) * 30]
    
    set unit [expr $Neuroendoscopy(cam,size)]
    
    
    ccamCube SetXLength 0
    ccamCube SetYLength 0
    ccamCube SetZLength 0
    
    cCubeXform Identity
    #cCubeXform RotateZ 90
    
    ccamCube2 SetXLength 0 
    ccamCube2 SetYLength 0
    ccamCube2 SetZLength 0
    
    cCubeXform2 Identity
    cCubeXform2 Translate 0 0 0
    
    ccamCube3 SetXLength 0
    ccamCube3 SetYLength 0
    ccamCube3 SetZLength 0
    
    cCubeXform3 Identity
    cCubeXform3 Translate 0 0 0

    if {$Neuroendoscopy(cam,viewAngle) < 80 && $Neuroendoscopy(cam,showAngle) == 1} {
      ccamAngle SetAngle $Neuroendoscopy(cam,viewAngle)
    
    } else {
      ccamAngle SetAngle 0
    }
    ccamAngle SetResolution 40
    ccamAngle SetHeight 40

    cConeXform Identity
    cConeXform Translate 0 30 0
    cConeXform RotateZ -90

    # set parameters for cylinder (lens) geometry and transform
    ccamCyl SetRadius [expr $Neuroendoscopy(cam,boxlength) /3]
    ccamCyl SetHeight [expr $Neuroendoscopy(cam,boxwidth) ]
    ccamCyl SetResolution 8
    
    cCylXform Identity
    cCylXform Translate 0 [expr $Neuroendoscopy(cam,boxwidth) / 1] 0
    
    ccamCyl2 SetRadius [expr $Neuroendoscopy(cam,boxlength) /2]
    ccamCyl2 SetHeight [expr $Neuroendoscopy(cam,boxwidth)]
    ccamCyl2 SetResolution 8
    
    cCylXform2 Identity
    cCylXform2 Translate 0 [expr {$Neuroendoscopy(cam,boxwidth) / 2} + $unit] 0
    
    #also, set the size of the focal Point actor
    set Neuroendoscopy(fp,size) [expr $Neuroendoscopy(cam,size) * 4]
    Neuroendoscopy(fp,source) SetRadius $Neuroendoscopy(fp,size)
    } else {
 
    
    #this is the old looking camera
    set Neuroendoscopy(cam,boxlength) [expr $Neuroendoscopy(cam,size) * 30]
    set Neuroendoscopy(cam,boxheight) [expr $Neuroendoscopy(cam,size) * 20]
    set Neuroendoscopy(cam,boxwidth)  [expr $Neuroendoscopy(cam,size) * 10]
    
    set unit [expr $Neuroendoscopy(cam,size)]
    
    
    ccamCube SetXLength $Neuroendoscopy(cam,boxlength)
    ccamCube SetYLength $Neuroendoscopy(cam,boxwidth)
    ccamCube SetZLength $Neuroendoscopy(cam,boxheight)
    
    cCubeXform Identity
    #cCubeXform RotateZ 90
    
    ccamCube2 SetXLength [expr $Neuroendoscopy(cam,boxlength) /3] 
    ccamCube2 SetYLength $Neuroendoscopy(cam,boxwidth)
    ccamCube2 SetZLength [expr $Neuroendoscopy(cam,boxheight) /3]
    
    cCubeXform2 Identity
    cCubeXform2 Translate 0 0 [expr {$Neuroendoscopy(cam,boxheight) /2}]
    
    ccamCube3 SetXLength [expr $Neuroendoscopy(cam,boxlength) /6] 
    ccamCube3 SetYLength [expr $Neuroendoscopy(cam,boxwidth) +$unit]
    ccamCube3 SetZLength [expr $Neuroendoscopy(cam,boxheight) /6]
    
    cCubeXform3 Identity
    cCubeXform3 Translate 0 0 [expr {$Neuroendoscopy(cam,boxheight) /2} + $unit]
    
    # set parameters for cylinder (lens) geometry and transform
    ccamCyl SetRadius [expr $Neuroendoscopy(cam,boxlength) / 3.5]
    ccamCyl SetHeight [expr $Neuroendoscopy(cam,boxwidth) * 1.5]
    ccamCyl SetResolution 8
    
    cCylXform Identity
    cCylXform Translate 0 [expr $Neuroendoscopy(cam,boxwidth) / 2] 0
    
    ccamCyl2 SetRadius [expr $Neuroendoscopy(cam,boxlength) / 5]
    ccamCyl2 SetHeight [expr $Neuroendoscopy(cam,boxwidth) * 1.5]
    ccamCyl2 SetResolution 8
    
    cCylXform2 Identity
    cCylXform2 Translate 0 [expr {$Neuroendoscopy(cam,boxwidth) / 2} + $unit] 0

    if {$Neuroendoscopy(cam,viewAngle) < 80 && $Neuroendoscopy(cam,showAngle) == 1} {
      ccamAngle SetAngle $Neuroendoscopy(cam,viewAngle)
    } else {
           ccamAngle SetAngle 0
    }
    ccamAngle SetResolution 40
    ccamAngle SetHeight 40

    cConeXform Identity
    cConeXform Translate 0 30 0
    cConeXform RotateZ -90
    #also, set the size of the focal Point actor
    set Neuroendoscopy(fp,size) [expr $Neuroendoscopy(cam,size) * 4]
    Neuroendoscopy(fp,source) SetRadius $Neuroendoscopy(fp,size)
    }


}
proc NeuroendoscopyCalibration { {filename ""} } {

global Neuroendoscopy

vtkImageCompare Neuroendoscopy(imgcompare)

#imgcompare LoadOriginalPicture "/projects/igtdev/ruetz/slicer2-mrt/slicer2/myfile1.ppm"
#imgcompare SetOutputPicture "myfile_undistort.ppm"
Neuroendoscopy(imgcompare) SetOriginalPicture "/projects/igtdev/ruetz/slicer2-mrt/slicer2/myfile1.ppm"


set XRAD [Neuroendoscopy(imgcompare) GetWidth]
set YRAD [Neuroendoscopy(imgcompare) GetHeight]

puts "$XRAD $XRAD $filename"
Neuroendoscopy(imgcompare) searchCorners 10 


vtkPNMReader Neuroendoscopy(pnmReader2)
Neuroendoscopy(pnmReader2) SetFileName "/projects/igtdev/ruetz/slicer2-mrt/slicer2/myfile1.ppm"
Neuroendoscopy(pnmReader2) Update

vtkImageCanvasSource2D Neuroendoscopy(imageCanvasTexture)

Neuroendoscopy(imageCanvasTexture) SetScalarTypeToUnsignedChar
Neuroendoscopy(imageCanvasTexture) SetNumberOfScalarComponents 3
Neuroendoscopy(imageCanvasTexture) SetExtent 0 640 0 480 0 0

# background black
Neuroendoscopy(imageCanvasTexture) SetDrawColor 255 0 0
Neuroendoscopy(imageCanvasTexture) FillBox 0 640 0 480



Neuroendoscopy(imageCanvasTexture) DrawImage 0 0 [ Neuroendoscopy(pnmReader2) GetOutput ]
set twoDcoord [Neuroendoscopy(imgcompare) getFeatureCoordinates]
for {set i 0} {$i< [$twoDcoord GetNumberOfPoints]} {incr i 1} {

  set coord [$twoDcoord GetPoint $i]
  set coordx [lindex $coord 0]
  set coordy [lindex $coord 1]


#Neuroendoscopy(imageCanvasTexture) DrawCircle [expr $coordx] [expr 480-$coordy] 10
Neuroendoscopy(imageCanvasTexture) DrawSegment [expr $coordx-10] [expr 480-$coordy-10] [expr $coordx+10] [expr 480-$coordy+10]
Neuroendoscopy(imageCanvasTexture) DrawSegment [expr $coordx+10] [expr 480-$coordy-10] [expr $coordx-10] [expr 480-$coordy+10]
}



vtkImageViewer Neuroendoscopy(distortviewer)
  Neuroendoscopy(distortviewer) SetInput [Neuroendoscopy(imageCanvasTexture) GetOutput]
  Neuroendoscopy(distortviewer) SetColorWindow 256.0
  Neuroendoscopy(distortviewer) SetColorLevel 127.5
  #[viewer GetActor2D] SetPosition 1 1

#
# Create second Mandelbrot viewer
#
#vtkPNMReader Neuroendoscopy(texture,pngReader)
Neuroendoscopy(texture,pngReader) SetFileName $filename
Neuroendoscopy(texture,pngReader) Update


vtkImageViewer Neuroendoscopy(distortviewer2)
  Neuroendoscopy(distortviewer2) SetInput [Neuroendoscopy(texture,pngReader) GetOutput]
  Neuroendoscopy(distortviewer2) SetColorWindow 256.0
  Neuroendoscopy(distortviewer2) SetColorLevel 127.5
  #[viewer2 GetActor2D] SetPosition 1 1

#
# Create the GUI: two vtkTkImageViewer widgets and a quit/reset buttons
#
wm withdraw .
set top [toplevel .top] 
wm protocol .top WM_DELETE_WINDOW "deleteDistortWindows;destroy .top"
wm title .top "Distortion Viewer"

set f1 [frame $top.f1] 

pack $f1 \
        -side bottom \
        -fill both -expand f

#
# Create the image viewer widget for set 1
#
set manFrame [frame $f1.man]
set julFrame [frame $f1.jul]

pack $manFrame $julFrame \
        -side left \
        -padx 3 -pady 3 \
        -fill both -expand f

set manView [vtkTkImageViewerWidget $manFrame.view \
        -iv Neuroendoscopy(distortviewer) \
        -width [expr $XRAD] \
        -height [expr $YRAD]]

set manRange [label $manFrame.range \
        -text "Distorted Picture"]

set quit [button $manFrame.quit  \
        -text "Quit" \
        -command "deleteDistortWindows;destroy .top"]

#
# Create the image viewer widget for set 2
#
pack $manView $manRange $quit \
        -side top \
        -padx 2 -pady 2 \
        -fill both -expand f

set julView [vtkTkImageViewerWidget $julFrame.view \
        -iv Neuroendoscopy(distortviewer2) \
        -width [expr $XRAD] \
        -height [expr $YRAD]]

set julRange [label $julFrame.range \
        -text "Undistorted Picture"]

set reset [button $julFrame.reset  \
        -text "Undistort" \
        -command "NeuroendoscopyUndistort $filename"]
set calibdat [button $julFrame.calibdat  \
        -text "Create Calibration file" \
        -command "exec /projects/igtdev/ruetz/daten/tsai/Tsai-method-v3.0b3/ImageCompare myfile1.ppm myfile_undistort.ppm 640 480 calibdata.dat 10"]

pack $julView $julRange $reset $calibdat \
        -side top \
        -padx 2 -pady 2 \
        -fill both -expand f
#
# The equation label
#
set equation [label $top.equation \
        -text "Distort ==> Undistort"]

pack $equation \
        -side bottom \
        -fill x -expand f

#
# Create the default bindings
#
::vtk::bind_tk_imageviewer_widget $manView
::vtk::bind_tk_imageviewer_widget $julView



}

proc deleteDistortWindows { } {
global Neuroendoscopy
set Neuroendoscopy(captWindow) 0
Neuroendoscopy(imgcompare) Delete
Neuroendoscopy(pnmReader2) Delete
#Neuroendoscopy(pnmReader) Delete
Neuroendoscopy(distortviewer) Delete
Neuroendoscopy(distortviewer2) Delete
Neuroendoscopy(imageCanvasTexture) Delete
}
#
# Update the display
#
proc NeuroendoscopyUndistort { {filename ""} } {
  global Neuroendoscopy

Neuroendoscopy(imgcompare) performUndistortion $filename


Neuroendoscopy(texture,pngReader) SetFileName $filename
Neuroendoscopy(texture,pngReader) Modified
Neuroendoscopy(texture,pngReader) Update

  Neuroendoscopy(distortviewer2) SetInput [Neuroendoscopy(texture,pngReader) GetOutput]
  Neuroendoscopy(distortviewer2) SetColorWindow 256.0
  Neuroendoscopy(distortviewer2) SetColorLevel 127.5
  Neuroendoscopy(distortviewer2) Modified
  Neuroendoscopy(distortviewer2) Render

Render3D
}

proc showAllTextures {} {
global Neuroendoscopy Model Module Matrix 


set nitems [Mrml(dataTree) GetNumberOfItems]
set openparse 0
set TextureMatrixIDs 0
set TextureModelIDs 0
set localmatrix 0
set filename ""

for {set i 0} {$i<$nitems} {incr i} {

set node [Mrml(dataTree) GetNthItem $i]
set nodename [$node GetName]
set nodetitle [$node GetTitle]

  if {[string match "StartTrafo*" $nodename]} {
  
   incr Neuroendoscopy(texture,actors,id)
   set openparse 1
  }
  if {$openparse == 1 && [string match "TextureMatrix*" $nodename] == 1} {
    set TextureMatrixIDs [$node GetID]
   #load the matrix (position of the texture)
    set localmatrix [Matrix($TextureMatrixIDs,node) GetMatrix]
    
  }
  if {$openparse == 1 && [string match "TextureModel*" $nodename] == 1} {
    set TextureModelIDs [$node GetID]
    set filename [$node GetDescription]
    
    #load the textures
    
  }

  if {$openparse == 1 && [string match "EndTransform*" $nodetitle] == 1} {
  
   
    vtkProjectedTexture Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) 
   Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetInput $Model($TextureModelIDs,polyData)
  
   Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetPosition [lindex $localmatrix 0] [lindex $localmatrix 1] [lindex $localmatrix 2]
   Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetFocalPoint [lindex $localmatrix 4] [lindex $localmatrix 5] [lindex $localmatrix 6]
  
   Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetUp [lindex $localmatrix 8] [lindex $localmatrix 9] [lindex $localmatrix 10]
   
   #the sides of the pyramid which represent the viewing and the lenght
   set fruLength [ver coneAngle2FrustumLength $Neuroendoscopy(texture,angle)]
   Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetAspectRatio [expr $fruLength*1.33333] $fruLength [expr 50*$Neuroendoscopy(cam,zoom)]


   vtkTransformTextureCoords Neuroendoscopy(texture,xform,$Neuroendoscopy(texture,actors,id))
   Neuroendoscopy(texture,xform,$Neuroendoscopy(texture,actors,id)) SetInput [Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) GetOutput]

 vtkDataSetMapper Neuroendoscopy(texture,mapper,$Neuroendoscopy(texture,actors,id))
   Neuroendoscopy(texture,mapper,$Neuroendoscopy(texture,actors,id)) SetInput [Neuroendoscopy(texture,xform,$Neuroendoscopy(texture,actors,id)) GetOutput]

###load picture
vtkPNMReader tempTextImageReader
tempTextImageReader SetFileName "/projects/igtdev/ruetz/models/TextureImage2.pnm"
puts [Model($TextureModelIDs,node) GetDescription]
  vtkTexture Neuroendoscopy(texture,atext,$Neuroendoscopy(texture,actors,id))
  # Neuroendoscopy(texture,atext,$Neuroendoscopy(texture,actors,id)) SetInput [tempTextImageReader GetOutput]
   Neuroendoscopy(texture,atext,$Neuroendoscopy(texture,actors,id)) SetInput [tempTextImageReader GetOutput]
   Neuroendoscopy(texture,atext,$Neuroendoscopy(texture,actors,id)) InterpolateOn

    foreach r $Module(Renderers) {
       # Model($m,mapper,$r) SetInput [Neuroendoscopy(texture,xform) GetOutput]
     
        Model($TextureModelIDs,actor,$r) SetMapper Neuroendoscopy(texture,mapper,$Neuroendoscopy(texture,actors,id))
        Model($TextureModelIDs,actor,$r) SetTexture Neuroendoscopy(texture,atext,$Neuroendoscopy(texture,actors,id))
    }

   
################################################
tempTextImageReader Delete
   
   set openparse 0
  }
}


}
proc NeuroendoscopyRegisterTexture {} {
global Neuroendoscopy Locator
if {$Neuroendoscopy(textureVisibility) == 1} {
puts "set texture to locator"
LocatorRegisterCallback  NeuroendoscopyTextureInits
} else {
puts "unset texture to locator"
LocatorUnRegisterCallback NeuroendoscopyTextureInits 
}
}


proc NeuroendoscopySetTexture {} {
global Neuroendoscopy Model ver tipActor Locator ModelMaker Module



incr Neuroendoscopy(texture,actors,id)

###save the picture
vtkPNMWriter tmppnmwrite
tmppnmwrite SetInput [Neuroendoscopy(imageStreamer) GetOutput]
tmppnmwrite SetFileName "/projects/igtdev/ruetz/models/TextureImage$Neuroendoscopy(texture,actors,id).pnm"
tmppnmwrite Write
puts "write image TextureImage$Neuroendoscopy(texture,actors,id)"

vtkImageCompare imgcompare
#imgcompare SetOriginalPictureFromArray [Neuroendoscopy(imageStreamer) GetOutput]
imgcompare SetOriginalPicture "/projects/igtdev/ruetz/models/TextureImage$Neuroendoscopy(texture,actors,id).pnm"

imgcompare searchCorners 10 

imgcompare performUndistortion "/projects/igtdev/ruetz/models/TextureImage$Neuroendoscopy(texture,actors,id).pnm"

set startTransform [MainMrmlAddNode Transform]
set trafoid [$startTransform GetID]
$startTransform SetName "StartTrafo$trafoid"
    set prefix "TextureModel$Neuroendoscopy(texture,actors,id)"
    set n [MainMrmlAddNode Model]
    $n SetName  "TextureModel$Neuroendoscopy(texture,actors,id)"
    $n SetColor 1
    $n SetDescription [tmppnmwrite GetFileName]
    # Guess the prefix
   # set ModelMaker(prefix) $ModelMaker(name)

tmppnmwrite Delete
    # Create the model
    set m [$n GetID]
    
    puts "ModelMakerCreate m = $m"
  
    MainModelsCreate $m
    
    $Model($m,polyData) DeepCopy [Neuroendoscopy(texture,clipper) GetOutput]
    $Model($m,polyData) Modified
    $Model($m,polyData) Update

 foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    


    #Mrml(dataTree) AddItem  Model($m,node)
    

    

    #set m $Model(activeID)
    #set ModelMaker(prefix) [MainFileSaveModel $m $prefix]
    if {$prefix == ""} {
        if {$::Module(verbose)} { 
            puts "ModelMakerWrite: empty prefix for model $m" 
        }
        return
    }
    
    # Write
    MainModelsWrite $m $prefix

    # Prefix changed, so update the Models->Props tab
    
   # MainModelsSetActive $m


#ADD VOLUME to hold image somthing is strange here##############################################################
#    set newvol [MainMrmlAddNode Volume]
  
#    set id [Neuroendoscopy(texture,pngReader) GetOutput]
#    $id SetNumberOfScalarComponents 1
#    set v [$newvol GetID]
    
#    $newvol SetName "ImageTexture$Neuroendoscopy(texture,actors,id)"
#    $newvol SetDescription "ImageTexture$Neuroendoscopy(texture,actors,id)" 
    # not for distance vols
#   MainVolumesCreate $v

    #return $n
   



    #eval Volume($v,node) SetSpacing [$id GetSpacing]
    #Volume($i,node) SetScanOrder PA
    #Volume($v,node) SetScanOrder IS
    #Volume($v,node) SetNumScalars [$id GetNumberOfScalarComponents]
    #set ext [$id GetWholeExtent]
    #Volume($v,node) SetImageRange [lindex $ext 4] [lindex $ext 5]
    #Volume($v,node) SetScalarType [$id GetScalarType]
    #Volume($v,node) SetDimensions [lindex [$id GetDimensions] 0] [lindex [$id GetDimensions] 1]
    #Volume($v,node) ComputeRasToIjkFromScanOrder [::Volume($v,node) GetScanOrder]
#    puts "created volume $v"

    # debug
    #puts [[$source GetOutput] Print]



#  Volume($v,vol) SetImageData $id
#      MainUpdateMRML
    # update slicer stuff
   # MainVolumesUpdate $v
#    MainVolumesWrite $v "/projects/igtdev/ruetz/models/ImageTexture$Neuroendoscopy(texture,actors,id)"
    # display this volume so the user knows something happened
   # MainSlicesSetVolumeAll Back $v
###########Save coordinates of displaying the texture#######################################################


    vtkMatrix4x4 tempMatrix
  


  set matrixortho [Neuroendoscopy(texture,transform) GetMatrix] 
  ####SHOULD NORMALY THE COORDINATES OF THE FOCAL POINT OF THE CAMERA!!!!
  #set coordinates [tipActor GetPosition]
  set coordinates [Neuroendoscopy(gyro,actor) GetMatrix]
  set FocalPoint [Neuroendoscopy(fp,actor) GetMatrix]
  #focalpoint
  #tempMatrix SetElement 0 0 0 
  #tempMatrix SetElement 0 1 0 
  #tempMatrix SetElement 0 2 0
  tempMatrix SetElement 0 0 [$coordinates GetElement 0 3]
  tempMatrix SetElement 0 1 [$coordinates GetElement 1 3] 
  tempMatrix SetElement 0 2 [$coordinates GetElement 2 3]
  #focalpoint

  tempMatrix SetElement 1 0 [$FocalPoint GetElement 0 3]
  tempMatrix SetElement 1 1 [$FocalPoint GetElement 1 3]
  tempMatrix SetElement 1 2 [$FocalPoint GetElement 2 3]
  #viewup
  tempMatrix SetElement 2 0 [$matrixortho GetElement 1 0]
  tempMatrix SetElement 2 1 [$matrixortho GetElement 1 1]
  tempMatrix SetElement 2 2 [$matrixortho GetElement 1 2]

  #Position


   

      set TextureMatrix [MainMrmlAddNode Matrix]
set maid [$TextureMatrix GetID]
$TextureMatrix SetName "TextureMatrix$maid"


set strPosition [Matrix($maid,node) GetMatrixToString tempMatrix]
Matrix($maid,node) SetMatrix $strPosition


set endTransform [MainMrmlAddNode EndTransform]
$endTransform SetName "EndTrafo$trafoid"

#puts [lindex $strPosition 0]
#puts $strPosition
  MainUpdateMRML
#RenderAll   





########################################################3 show me

   set tempMatrix [Matrix($maid,node) GetMatrix]
   
   vtkProjectedTexture Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) 
   Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetInput $Model($m,polyData)
   #Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetPosition [$tempMatrix GetElement 2 0] [$tempMatrix GetElement 2 1] [$tempMatrix GetElement 2 2]
   #Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetFocalPoint [$tempMatrix GetElement 0 0] [$tempMatrix GetElement 0 1] [$tempMatrix GetElement 0 2]
   #Neuroendoscopy(texture,tmapper) SetFocalPoint $Neuroendoscopy(texture,xcoordi) $Neuroendoscopy(texture,ycoordi) $Neuroendoscopy(texture,zcoordi)  
   #Neuroendoscopy(texture,tmapper) SetUp $Neuroendoscopy(texture,xpic) $Neuroendoscopy(texture,ypic) $Neuroendoscopy(texture,zpic)
   #set the orientation of the texture
   #Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetUp [$tempMatrix GetElement 1 0] [$tempMatrix GetElement 1 1] [$tempMatrix GetElement 1 2]
  
   Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetPosition [lindex $tempMatrix 0] [lindex $tempMatrix 1] [lindex $tempMatrix 2]
   Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetFocalPoint [lindex $tempMatrix 4] [lindex $tempMatrix 5] [lindex $tempMatrix 6]
   #Neuroendoscopy(texture,tmapper) SetFocalPoint $Neuroendoscopy(texture,xcoordi) $Neuroendoscopy(texture,ycoordi) $Neuroendoscopy(texture,zcoordi)  
   #Neuroendoscopy(texture,tmapper) SetUp $Neuroendoscopy(texture,xpic) $Neuroendoscopy(texture,ypic) $Neuroendoscopy(texture,zpic)
   #set the orientation of the texture
   Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetUp [lindex $tempMatrix 8] [lindex $tempMatrix 9] [lindex $tempMatrix 10]
   
   #the sides of the pyramid which represent the viewing and the lenght
   set fruLength [ver coneAngle2FrustumLength $Neuroendoscopy(texture,angle)]
   Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) SetAspectRatio [expr $fruLength*1.33333] $fruLength [expr 50*$Neuroendoscopy(cam,zoom)]


 vtkTransformTextureCoords Neuroendoscopy(texture,xform,$Neuroendoscopy(texture,actors,id))
   Neuroendoscopy(texture,xform,$Neuroendoscopy(texture,actors,id)) SetInput [Neuroendoscopy(texture,tmapper,$Neuroendoscopy(texture,actors,id)) GetOutput]

 vtkDataSetMapper Neuroendoscopy(texture,mapper,$Neuroendoscopy(texture,actors,id))
   Neuroendoscopy(texture,mapper,$Neuroendoscopy(texture,actors,id)) SetInput [Neuroendoscopy(texture,xform,$Neuroendoscopy(texture,actors,id)) GetOutput]

###load picture
vtkPNMReader tempTextImageReader
tempTextImageReader SetFileName [Model($m,node) GetDescription]

  vtkTexture Neuroendoscopy(texture,atext,$Neuroendoscopy(texture,actors,id))
  # Neuroendoscopy(texture,atext,$Neuroendoscopy(texture,actors,id)) SetInput [tempTextImageReader GetOutput]
   Neuroendoscopy(texture,atext,$Neuroendoscopy(texture,actors,id)) SetInput [tempTextImageReader GetOutput]
   Neuroendoscopy(texture,atext,$Neuroendoscopy(texture,actors,id)) InterpolateOn

    foreach r $Module(Renderers) {
       # Model($m,mapper,$r) SetInput [Neuroendoscopy(texture,xform) GetOutput]
        Model($m,actor,$r) SetMapper Neuroendoscopy(texture,mapper,$Neuroendoscopy(texture,actors,id))
        Model($m,actor,$r) SetTexture Neuroendoscopy(texture,atext,$Neuroendoscopy(texture,actors,id))
    }

   
################################################
tempTextImageReader Delete
 tempMatrix Delete
imgcompare Delete
}
proc NeuroendoscopyVideoTexture {} {
global Neuroendoscopy
 if {$Neuroendoscopy(videoVisibility) == 1} {
   vtkCaptureFrames Neuroendoscopy(capFrames)
 Neuroendoscopy(capFrames) Initialize 640 480

vtkImageDataStreamer Neuroendoscopy(imageStreamer)
 
 } else {
  Neuroendoscopy(imageStreamer) Delete
  Neuroendoscopy(capFrames) Delete
}
}


proc NeuroendoscopyAddPlane {} {
global Neuroendoscopy Model Module ModelMaker

vtkPlaneSource plane
plane SetOrigin -135 0.1 101.75
plane SetPoint1 135 0.1 101.75
plane SetPoint2 -135 0.1 -101.75
plane SetNormal 0 1 0
plane SetXResolution 72
plane SetYResolution 72
plane Modified
plane Update



#add plane in mrml tree
    set prefix "Plane1"
    set n [MainMrmlAddNode Model]
    $n SetName  "Plane1"
    $n SetColor 1
    # Guess the prefix
   # set ModelMaker(prefix) $ModelMaker(name)


    # Create the model
    set m [$n GetID]
    
    puts "ModelMakerCreate m = $m"
  
    MainModelsCreate $m
    #$Model($m,polyData) SetInput [plane GetOutput]
    $Model($m,polyData) DeepCopy [plane GetOutput]
    $Model($m,polyData) Modified
    $Model($m,polyData) Update

MainUpdateMRML

 foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
        Model($m,actor,$r) SetMapper Model($m,mapper,$r)
    }
    


    #Mrml(dataTree) AddItem  Model($m,node)
    

#    vtkDataSetMapper planemapper
#    planemapper SetInput $Model($m,polyData)

#    foreach r $Module(Renderers) {
       # Model($m,mapper,$r) SetInput [Neuroendoscopy(texture,xform) GetOutput]
#        Model($m,actor,$r) SetMapper planemapper

#    }

    #set m $Model(activeID)
    #set ModelMaker(prefix) [MainFileSaveModel $m $prefix]
    if {$prefix == ""} {
        if {$::Module(verbose)} { 
            puts "ModelMakerWrite: empty prefix for model $m" 
        }
        return
    }
    
    # Write
    MainModelsWrite $m $prefix




Render3D
#vtkPolyDataMapper mapper

#    mapper SetInput [plane GetOutput]
    #sphereMapper SetInput [plane GetOutput]
#    mapper GlobalImmediateModeRenderingOn

#vtkActor triangulation
#triangulation SetMapper mapper
#[triangulation GetProperty] SetOpacity 0.5
#[triangulation GetProperty] SetColor 1 0 1

#neuroendoscopyScreen AddActor triangulation
#viewRen AddActor triangulation

plane Delete
}

#
#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateFocalPoint
#  Create the vtk FocalPoint actor
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyTextureInits {{pic ""}} {
global Neuroendoscopy Model ver tipActor Locator ModelMaker Module
if {$Neuroendoscopy(textureVisibility) == 1} {


   #set Neuroendoscopy(texture,model) [lindex $Model(idList) 3]
   set Neuroendoscopy(texture,model) $Model(activeID)

#transformation for the cone to put the clipping cone in the right direction

#vtkTransform Neuroendoscopy(texture,tranform)
#Neuroendoscopy(gyro,actor) SetUserMatrix Locator(normalMatrix)
#    Neuroendoscopy(gyro,actor) SetOrientation 180 0 0
#    Neuroendoscopy(gyro,actor) SetPosition [lindex $coordinates 0] [lindex $coordinates 1] [lindex $coordinates 2]

#set position normaly of camera because we have a little tranformation between sensor and focalpoint 
#set coordinates [tipActor GetPosition]
set coordinates [Neuroendoscopy(gyro,actor) GetMatrix]

 #Neuroendoscopy(texture,transmatrix) Invert Locator(normalMatrix) Neuroendoscopy(texture,invertedmatrix)
#set Neuroendoscopy(texture,transmatrix) Locator(normalMatrix)
#Neuroendoscopy(texture,transmatrix) SetElement 0 0 [Locator(tipMatrix) GetElement 0 0]
#Neuroendoscopy(texture,transmatrix) SetElement 0 1 [Locator(tipMatrix) GetElement 0 1]
#Neuroendoscopy(texture,transmatrix) SetElement 0 2 [Locator(tipMatrix) GetElement 0 2]
#Neuroendoscopy(texture,transmatrix) SetElement 1 0 [Locator(normalMatrix) GetElement 1 0]
#Neuroendoscopy(texture,transmatrix) SetElement 1 1 [Locator(normalMatrix) GetElement 1 1]
#Neuroendoscopy(texture,transmatrix) SetElement 1 2 [Locator(normalMatrix) GetElement 1 2]
#Neuroendoscopy(texture,transmatrix) SetElement 2 0 [Locator(normalMatrix) GetElement 2 0]
#Neuroendoscopy(texture,transmatrix) SetElement 2 1 [Locator(normalMatrix) GetElement 2 1]
#Neuroendoscopy(texture,transmatrix) SetElement 2 2 [Locator(normalMatrix) GetElement 2 2]
#Neuroendoscopy(texture,transmatrix) SetElement 3 0 [lindex $coordinates 0]
#Neuroendoscopy(texture,transmatrix) SetElement 3 1 [lindex $coordinates 1]
#Neuroendoscopy(texture,transmatrix) SetElement 3 2 [lindex $coordinates 2]


#  Neuroendoscopy(texture,transform) SetMatrix Neuroendoscopy(texture,invertedmatrix)
#  Neuroendoscopy(texture,transform) SetMatrix Locator(normalMatrix)
#Neuroendoscopy(texture,transform) SetMatrix Neuroendoscopy(texture,transmatrix)
  
Neuroendoscopy(texture,transform) Identity
#Neuroendoscopy(texture,transform) Concatenate Locator(normalMatrix)
Neuroendoscopy(texture,transform) Concatenate [Neuroendoscopy(gyro,actor) GetMatrix]
#Neuroendoscopy(texture,transform) Translate  0 [expr $Locator(normalLen) / -2.] 0  
# Neuroendoscopy(texture,transform) RotateX $Neuroendoscopy(texture,xcoordi)
Neuroendoscopy(texture,transform) RotateX 180
#Neuroendoscopy(texture,transform) RotateY $Neuroendoscopy(texture,ycoordi)
Neuroendoscopy(texture,transform) RotateY 90
#Neuroendoscopy(texture,transform) RotateZ $Neuroendoscopy(texture,rotate)
Neuroendoscopy(texture,transform) RotateZ -90
Neuroendoscopy(texture,transform) Inverse
  # Neuroendoscopy(texture,transform) RotateZ $Neuroendoscopy(texture,rotate)
Neuroendoscopy(texture,transform) Modified
Neuroendoscopy(texture,transform) Update


#cone which clip the part of interest 
#the angle is between the rotation axis and the edge
#vtkCone Neuroendoscopy(texture,cone)


    Neuroendoscopy(texture,cone) SetAngle $Neuroendoscopy(texture,angle)

    
   
#vtkTransform CylinderTransform
#CylinderTransform RotateZ 90


#Neuroendoscopy(texture,cylinder)  SetRadius 5
#Neuroendoscopy(texture,cylinder)  SetTransform CylinderTransform
   
   #Neuroendoscopy(texture,cone) SetTransform Neuroendoscopy(texture,transform)
   
   #for cutting the cone
    Neuroendoscopy(texture,frontPlane) SetOrigin 300 0 0
    Neuroendoscopy(texture,frontPlane) SetNormal 1 0  0
 
    Neuroendoscopy(texture,basePlane) SetOrigin 0.1 0 0
    Neuroendoscopy(texture,basePlane) SetNormal -1 0 0






    Neuroendoscopy(texture,theCone) SetOperationTypeToIntersection
    Neuroendoscopy(texture,theCone) AddFunction Neuroendoscopy(texture,cone)
    #Neuroendoscopy(texture,theCone) AddFunction Neuroendoscopy(texture,cylinder)
    Neuroendoscopy(texture,theCone) AddFunction Neuroendoscopy(texture,frontPlane)
    Neuroendoscopy(texture,theCone) AddFunction Neuroendoscopy(texture,basePlane)
    Neuroendoscopy(texture,theCone) SetTransform Neuroendoscopy(texture,transform) 
    Neuroendoscopy(texture,theCone) Modified
#vtkClipPolyData Neuroendoscopy(texture,clipper)
    


    Neuroendoscopy(texture,clipper) SetInput $Model($Neuroendoscopy(texture,model),polyData)
    Neuroendoscopy(texture,clipper) SetClipFunction Neuroendoscopy(texture,theCone)
    Neuroendoscopy(texture,clipper) InsideOutOn






## here we will calculate just the visible points of the texture mapper
#    vtkSelectVisiblePoints visPts
#    visPts SetInput [Neuroendoscopy(texture,clipper) GetOutput]
#    visPts SetRenderer neuroendoscopyScreen
   
  set matrixortho [Neuroendoscopy(texture,transform) GetMatrix] 

  #set FocalPointX [$matrixortho GetElement 0 0]
  #set FocalPointY [$matrixortho GetElement 0 1]
  #set FocalPointZ [$matrixortho GetElement 0 2]
  
  set FocalPoint [Neuroendoscopy(fp,actor) GetMatrix]
  

  set ViewUpX [$matrixortho GetElement 1 0]
  set ViewUpY [$matrixortho GetElement 1 1]
  set ViewUpZ [$matrixortho GetElement 1 2]
#tmapper AutomaticPlaneGenerationOn

  lappend focal [$FocalPoint GetElement 0 3] [$FocalPoint GetElement 1 3] [$FocalPoint GetElement 2 3]
  lappend coordsgyro [$coordinates GetElement 0 3] [$coordinates GetElement 1 3] [$coordinates GetElement 2 3]
#the projection of the z axis must be near 0 but not 0 because than we will not see any texture
#vtkProjectedTexture Neuroendoscopy(texture,tmapper)
   Neuroendoscopy(texture,tmapper) SetInput [Neuroendoscopy(texture,clipper) GetOutput]
#Neuroendoscopy(texture,tmapper) SetInput [visPts GetOutput]
   #Neuroendoscopy(texture,tmapper) SetPosition 0 0 0
Neuroendoscopy(texture,tmapper) SetPosition [$coordinates GetElement 0 3] [$coordinates GetElement 1 3] [$coordinates GetElement 2 3] 
   Neuroendoscopy(texture,tmapper) SetFocalPoint [$FocalPoint GetElement 0 3] [$FocalPoint GetElement 1 3] [$FocalPoint GetElement 2 3] 
   
   #Neuroendoscopy(texture,tmapper) SetFocalPoint $Neuroendoscopy(texture,xcoordi) $Neuroendoscopy(texture,ycoordi) $Neuroendoscopy(texture,zcoordi)  
   #Neuroendoscopy(texture,tmapper) SetUp $Neuroendoscopy(texture,xpic) $Neuroendoscopy(texture,ypic) $Neuroendoscopy(texture,zpic)
   #set the orientation of the texture
   Neuroendoscopy(texture,tmapper) SetUp $ViewUpX $ViewUpY $ViewUpZ
   puts " $focal ($coordsgyro)"

   
   
   #the sides of the pyramid which represent the viewing and the lenght

  #NeuroendoscopyCalcDistance 
set Neuroendoscopy(shortestDistance) [ver GetDistance neuroendoscopyScreen $Model($Neuroendoscopy(texture,model),polyData)]
  #[expr 1/$Neuroendoscopy(shortestDistance)]
   puts "short distance $Neuroendoscopy(shortestDistance)"
   set fruLength [ver coneAngle2FrustumLength $Neuroendoscopy(texture,angle)]
   #Neuroendoscopy(texture,tmapper) SetAspectRatio $fruLength $fruLength [expr 1250/$Neuroendoscopy(shortestDistance)]
   Neuroendoscopy(texture,tmapper) SetAspectRatio [expr $fruLength*1.33333] $fruLength [expr 50*$Neuroendoscopy(cam,zoom)]


#vtkTransformTextureCoords Neuroendoscopy(texture,xform)
   Neuroendoscopy(texture,xform) SetInput [Neuroendoscopy(texture,tmapper) GetOutput]

#vtkDataSetMapper Neuroendoscopy(texture,mapper)
   Neuroendoscopy(texture,mapper) SetInput [Neuroendoscopy(texture,xform) GetOutput]
  

#vtkPNGReader Neuroendoscopy(texture,pngReader)
  # Neuroendoscopy(texture,pngReader) SetFileName "/projects/igtdev/ruetz/models/testpic6.png"
 #vtkPNMReader Neuroendoscopy(texture,pngReader) 
#Neuroendoscopy(texture,pngReader) SetFileName "/projects/igtdev/ruetz/slicer2-mrt/slicer2/myfile_undistort1.ppm"
#vtkBMPReader bmpReader
#  bmpReader SetFileName "/projects/igtdev/ruetz/models/Red_Sox.bmp"
  # vtkTexture Neuroendoscopy(texture,atext)


  Neuroendoscopy(imageStreamer) SetInput [Neuroendoscopy(capFrames) getNextFrame]
  Neuroendoscopy(texture,atext) SetInput [Neuroendoscopy(imageStreamer) GetOutput]
  # Neuroendoscopy(texture,atext) SetInput [Neuroendoscopy(texture,pngReader) GetOutput]
   Neuroendoscopy(texture,atext) InterpolateOn

    #set Neuroendoscopy(texture,actors,id) [expr $Neuroendoscopy(texture,actors,id)+1]
    
#    lappend Neuroendoscopy(actors) Neuroendoscopy(texture,actor,$Neuroendoscopy(texture,actors,id))

############CREATE MODEL#################
   vtkActor Neuroendoscopy(texture,actor,0)
    Neuroendoscopy(texture,actor,0) SetMapper Neuroendoscopy(texture,mapper)
    #Neuroendoscopy(texture,actor,0) SetMapper Neuroendoscopy(texture,Clipmapper)
    Neuroendoscopy(texture,actor,0) SetTexture Neuroendoscopy(texture,atext)




#visPts Delete



if {$Neuroendoscopy(boolean) != 1} {

    #create a new Actor
    
    neuroendoscopyScreen AddActor Neuroendoscopy(texture,actor,0)

    viewRen AddActor Neuroendoscopy(texture,actor,0)

}
#Neuroendoscopy(texture,pngReader) Delete
set Neuroendoscopy(boolean) 1

    Neuroendoscopy(texture,actor,0) Delete





} else {
#   if {$Neuroendoscopy(texture,actors,id) != 0 } {
    #delete the actor before
#    neuroendoscopyScreen RemoveActor Neuroendoscopy(texture,actor,$Neuroendoscopy(texture,actors,id))
#    viewRen RemoveActor Neuroendoscopy(texture,actor,$Neuroendoscopy(texture,actors,id))
#    Neuroendoscopy(texture,actor,$Neuroendoscopy(texture,actors,id)) Delete
#    set Neuroendoscopy(texture,actors,id) [expr $Neuroendoscopy(texture,actors,id)-1]
#    }
set Neuroendoscopy(boolean) 0

 }


}


proc NeuroendoscopyCalcDistance {} {
global Neuroendoscopy Model ver tipActor Locator ModelMaker Module

#vtkSelectVisiblePoints visPts
#visPts SetInput [Neuroendoscopy(texture,clipper) GetOutput]
#visPts SetRenderer neuroendoscopyScreen

set renSize [neuroendoscopyScreen GetSize] 
puts " size of $renSize"
set centerview [neuroendoscopyScreen GetCenter] 
puts " size of $centerview"

set zPtr [neuroendoscopyScreen GetZ [expr [lindex $renSize 0]+[lindex $renSize 0]/2] [expr [lindex $renSize 1]/2]]
#set zPtr [neuroendoscopyScreen GetZ 0 0]

set aspects [neuroendoscopyScreen GetAspect]
#puts "aspects $aspects"
set asratio [expr [lindex $aspects 0]/[lindex $aspects 1]]
set cam [neuroendoscopyScreen GetActiveCamera]

#vtkTransform transformDisp 
#transformDisp SetMatrix [$cam GetPerspectiveTransformMatrix $asratio -1 1]
#transformDisp Inverse

#vtkMatrix4x4 matrixDisp
#matrixDisp DeepCopy [transformDips GetMatrix]

#set zNumerator  [matrixDisp GetElement 2 3]
#set zDenomMult  [matrixDisp GetElement 3 2]
#set zDenomAdd   [matrixDisp GetElement 3 3]

#set rangePointer [expr -$zNumerator / ( (($zPtr)*2.0 -1.0) *
#           $zDenomMult + $zDenomAdd )]

#vtkTransform transformView 
#transformView SetMatrix [$cam GetViewTransformMatrix]
#transformView Inverse

#vtkMatrix4x4 viewMatrix
#viewMatrix DeepCopy [transformView GetMatrix] 


#viewMatrix Invert
vtkTransform transformDisp 
transformDisp SetMatrix [$cam GetCompositePerspectiveTransformMatrix $asratio 0 1]
transformDisp Inverse
 
vtkMatrix4x4 matrixDisp
matrixDisp DeepCopy [transformDisp GetMatrix]

set wcoords [matrixDisp MultiplyPoint 0.5 0.5 $zPtr 0]



transformDisp Delete
matrixDisp Delete

set campos [$cam GetPosition]

#ver GetZBuffer neuroendoscopyScreen [expr [lindex $renSize 0]+[lindex $renSize 0]/2] [expr [lindex $renSize 1]/2]
ver GetDistance neuroendoscopyScreen $Model($Neuroendoscopy(texture,model),polyData)
#set Neurorenderwindow [neuroendoscopyScreen GetRenderWindow]
#set wcoords [viewMatrix MultiplyPoint [expr [lindex $renSize 0]+[lindex $renSize 0]/2] [expr [lindex $renSize 1]/2] $zvalue 0]
#set renNeuroSize [$Neurorenderwindow GetSize]
#puts " sizenuro of $renNeuroSize"
#set zdata [$Neurorenderwindow GetZbufferData [expr ([lindex $renNeuroSize 0]/2)-1] [expr ([lindex $renNeuroSize 1]/2)-1] [expr ([lindex $renNeuroSize 0]/2)] [expr ([lindex $renNeuroSize 1]/2)]]
#set zdata [$Neurorenderwindow GetZbufferData 299 299 300 300]
#set Neuroendoscopy(shortestDistance)  [$cam GetDistance]
puts "zbuffer data $wcoords ($zPtr)"
puts "camera coordinates $campos"
#viewMatrix Delete
}
#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateFocalPoint
#  Create the vtk FocalPoint actor
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyTextureGeneration {} {
global Neuroendoscopy Model

vtkPlaneSource plane
plane SetOrigin -250 50 250
plane SetPoint1 250 50 250
plane SetPoint2 -250 50 -250
plane SetNormal 0 1 0
plane SetXResolution 600
plane SetYResolution 600


vtkTransform t
    t RotateZ 90

#angle from the edge of the cone and its rotation axis
vtkCone cone
   cone SetAngle 25
   cone SetTransform t


vtkCubeSource CubeModel
    CubeModel SetCenter .5 .5 .5
    CubeModel SetXLength 150
    CubeModel SetYLength 150
    CubeModel SetZLength 150



vtkSphereSource sphere
  #sphere SetThetaResolution 8
  #sphere SetPhiResolution 16
  sphere SetRadius 90

vtkCylinder cylfunc
    cylfunc SetRadius 3

#vtkCylinder cylfunc2
#    cylfunc2 SetRadius 3
#    cylfunc2 SetCenter 4 4 4
     vtkDelaunay2D  del

del SetInput [plane GetOutput]
#not working with the head
vtkExtractPolyDataGeometry extract
    extract SetInput [plane GetOutput]
    extract SetImplicitFunction cone
    extract ExtractBoundaryCellsOn

vtkPolyDataMapper sphereMapper

    sphereMapper SetInput [extract GetOutput]
    #sphereMapper SetInput [plane GetOutput]
    sphereMapper GlobalImmediateModeRenderingOn


#############Texture######################3333
vtkProjectedTexture tmapper
tmapper SetInput [extract GetOutput]
tmapper SetPosition 0 -1 0
tmapper SetFocalPoint 0 0 0.000000001

#the sides of the pyramid and the lenght
tmapper SetAspectRatio 46.63 46.63 50



#vtkTextureMapToPlane tmapper
#tmapper SetInput [extract GetOutput]
# We scale the texture coordinate to get some repeat patterns.


vtkTransformTextureCoords xform
  xform SetInput [tmapper GetOutput]
  #xform SetScale 1 1 1

vtkDataSetMapper mapper
  mapper SetInput [xform GetOutput]

#vtkBMPReader bmpReader
#  bmpReader SetFileName "/projects/igtdev/ruetz/models/Red_Sox.bmp"

vtkPNGReader pngReader
pngReader SetFileName "/projects/igtdev/ruetz/models/testpic6.png"

vtkTexture atext
  atext SetInput [pngReader GetOutput]
  atext InterpolateOn
#######################################################################
vtkActor triangulation
triangulation SetMapper mapper
triangulation SetTexture atext

neuroendoscopyScreen AddActor triangulation
viewRen AddActor triangulation


}
proc NeuroendoscopyTesttexture2 {} {
###working with head###############################


vtkPlaneSource plane
plane SetOrigin -250 50 250
plane SetPoint1 250 50 250
plane SetPoint2 -250 50 -250
plane SetNormal 0 1 0
plane SetXResolution 600
plane SetYResolution 600


vtkTransform t
    t RotateZ 90
    t Translate 0 0 0


set test [lindex $Model(idList) 3]
vtkCone cone
   cone SetAngle 40
   cone SetTransform t


vtkClipPolyData clipper
    clipper SetInput $Model($test,polyData)
    clipper SetClipFunction cone
    clipper InsideOutOn
    clipper GenerateClippedOutputOn
    clipper SetValue 0

#vtkDataSetMapper mapperclipped
#  mapperclipped SetInput [clipper GetOutput]

#vtkTextureMapToPlane tmapper
#tmapper SetInput [clipper GetOutput]
#tmapper AutomaticPlaneGenerationOn


vtkProjectedTexture tmapper
tmapper SetInput [clipper GetOutput]
tmapper SetPosition 0 0 0.000001
tmapper SetFocalPoint 0 1 0      
tmapper SetCameraModeToPinhole

vtkTransformTextureCoords xform
  xform SetInput [tmapper GetOutput]

vtkDataSetMapper mapper
  mapper SetInput [xform GetOutput]
  

vtkPNGReader pngReader
pngReader SetFileName "/projects/igtdev/ruetz/models/testpic2.png"
#vtkBMPReader bmpReader
#  bmpReader SetFileName "/projects/igtdev/ruetz/models/Red_Sox.bmp"
vtkTexture atext
  atext SetInput [pngReader GetOutput]
  atext InterpolateOn


vtkActor triangulation
  triangulation SetMapper mapper
  triangulation SetTexture atext


neuroendoscopyScreen AddActor triangulation
viewRen AddActor triangulation


####################################

vtkPlaneSource plane
plane SetOrigin -250 50 250
plane SetPoint1 250 50 250
plane SetPoint2 -250 50 -250
plane SetNormal 0 1 0
plane SetXResolution 600
plane SetYResolution 600


vtkTransform t
    t RotateZ 90
    t Translate 0 0 0
    #t Scale
    
    #t SetPosition 0 0 0
    #$cam SetFocalPoint 0 30 0

vtkCone cone
   cone SetAngle 60
   cone SetTransform t


vtkCubeSource CubeModel
    CubeModel SetCenter .5 .5 .5
    CubeModel SetXLength 150
    CubeModel SetYLength 150
    CubeModel SetZLength 150



vtkSphereSource sphere
  #sphere SetThetaResolution 8
  #sphere SetPhiResolution 16
  sphere SetRadius 90

vtkCylinder cylfunc
    cylfunc SetRadius 3

#vtkCylinder cylfunc2
#    cylfunc2 SetRadius 3
#    cylfunc2 SetCenter 4 4 4
     vtkDelaunay2D  del

del SetInput [plane GetOutput]
#not working with the head
vtkExtractPolyDataGeometry extract
    extract SetInput [plane GetOutput]
    extract SetImplicitFunction cone
    extract ExtractBoundaryCellsOn

vtkPolyDataMapper sphereMapper

    sphereMapper SetInput [extract GetOutput]
    #sphereMapper SetInput [plane GetOutput]
    sphereMapper GlobalImmediateModeRenderingOn


#############Texture######################3333
#vtkProjectedTexture tmapper
#tmapper SetCameraModeToPinhole
#tmapper SetInput [extract GetOutput]
#tmapper SetPosition 0 -130 0
#tmapper SetFocalPoint 0 0 0.000000001



vtkTextureMapToPlane tmapper
tmapper SetInput [extract GetOutput]
# We scale the texture coordinate to get some repeat patterns.
vtkTransformTextureCoords xform
  xform SetInput [tmapper GetOutput]
  #xform SetScale 1 1 1

vtkDataSetMapper mapper
  mapper SetInput [xform GetOutput]

#vtkBMPReader bmpReader
#  bmpReader SetFileName "/projects/igtdev/ruetz/models/Red_Sox.bmp"

vtkPNGReader pngReader
pngReader SetFileName "/projects/igtdev/ruetz/models/testpic6.png"

vtkTexture atext
  atext SetInput [pngReader GetOutput]
  atext InterpolateOn
#######################################################################
vtkActor triangulation
triangulation SetMapper mapper
triangulation SetTexture atext

neuroendoscopyScreen AddActor triangulation
viewRen AddActor triangulation

}

proc NeuroendoscopyTesttexture {} {
global Neuroendoscopy Model



vtkPlaneSource plane
plane SetOrigin -250 50 250
plane SetPoint1 250 50 250
plane SetPoint2 -250 50 -250
plane SetNormal 0 1 0
plane SetXResolution 600
plane SetYResolution 600


vtkTransform t
    t RotateZ 90
    t Translate 0 0 0
    #t Scale
    
    #t SetPosition 0 0 0
    #$cam SetFocalPoint 0 30 0

vtkCone cone
   cone SetAngle 30
   cone SetTransform t


vtkCubeSource CubeModel
    CubeModel SetCenter .5 .5 .5
    CubeModel SetXLength 150
    CubeModel SetYLength 150
    CubeModel SetZLength 150



vtkSphereSource sphere
  #sphere SetThetaResolution 8
  #sphere SetPhiResolution 16
  sphere SetRadius 90

vtkCylinder cylfunc
    cylfunc SetRadius 3

#vtkCylinder cylfunc2
#    cylfunc2 SetRadius 3
#    cylfunc2 SetCenter 4 4 4
     vtkDelaunay2D  del

del SetInput [plane GetOutput]
#not working with the head
vtkExtractPolyDataGeometry extract
    extract SetInput [plane GetOutput]
    extract SetImplicitFunction cone
    extract ExtractBoundaryCellsOn

vtkPolyDataMapper sphereMapper

    sphereMapper SetInput [extract GetOutput]
    #sphereMapper SetInput [plane GetOutput]
    sphereMapper GlobalImmediateModeRenderingOn



#############Texture######################3333
vtkProjectedTexture tmapper
tmapper SetInput [extract GetOutput]
tmapper SetPosition 0 0 0
tmapper SetFocalPoint 0 0.5 0.0001    

#vtkTextureMapToPlane tmapper
#tmapper SetInput [extract GetOutput]

# We scale the texture coordinate to get some repeat patterns.
vtkTransformTextureCoords xform
  xform SetInput [tmapper GetOutput]
  #xform SetScale 1 1 1

vtkDataSetMapper mapper
  mapper SetInput [xform GetOutput]


#vtkBMPReader bmpReader
#  bmpReader SetFileName "/projects/igtdev/ruetz/models/Red_Sox.bmp"

vtkPNGReader pngReader
pngReader SetFileName "/projects/igtdev/ruetz/models/testpic.png"

vtkTexture atext
  atext SetInput [pngReader GetOutput]
  atext InterpolateOn
#######################################################################
vtkActor triangulation
triangulation SetMapper mapper
triangulation SetTexture atext

neuroendoscopyScreen AddActor triangulation
viewRen AddActor triangulation

########################################################################3



vtkTransform t
    t RotateZ 90
    t Translate 0 0 0


set test [lindex $Model(idList) 3]
vtkCone cone
   cone SetAngle 40
   #cone SetTransform t

#for cutting the cone
vtkPlane frontPlane
    frontPlane SetOrigin -400 0 0
    frontPlane SetNormal -1 0  0
vtkPlane basePlane
    basePlane SetOrigin -0.1 0 0
    basePlane SetNormal 1 0  0




vtkImplicitBoolean theCone
    theCone SetOperationTypeToIntersection
    theCone AddFunction cone
    theCone AddFunction frontPlane
    theCone AddFunction basePlane
    theCone SetTransform t

vtkClipPolyData clipper
    clipper SetInput $Model($test,polyData)
    clipper SetClipFunction theCone
    clipper GenerateClippedOutputOn
    clipper InsideOutOn


#vtkDataSetMapper mapperclipped
#  mapperclipped SetInput [clipper GetOutput]

vtkTextureMapToPlane tmapper
tmapper SetInput [clipper GetOutput]
tmapper SetNormal 0 1 0 


    
#vtkProjectedTexture tmapper
#tmapper SetInput [clipper GetOutput]
#tmapper SetPosition 0 0 0.000001
#tmapper SetFocalPoint 0 1 0      
#tmapper SetCameraModeToPinhole

vtkTransformTextureCoords xform
  xform SetInput [tmapper GetOutput]

vtkDataSetMapper mapper
  mapper SetInput [xform GetOutput]


vtkPNGReader pngReader
pngReader SetFileName "/projects/igtdev/ruetz/models/testpic2.png"
#vtkBMPReader bmpReader
#  bmpReader SetFileName "/projects/igtdev/ruetz/models/Red_Sox.bmp"
vtkTexture atext
  atext SetInput [pngReader GetOutput]
  atext InterpolateOn

vtkActor triangulation
  triangulation SetMapper mapper
  triangulation SetTexture atext

neuroendoscopyScreen AddActor triangulation
viewRen AddActor triangulation
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateFocalPoint
#  Create the vtk FocalPoint actor
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreateFocalPoint {} {
    
    global Neuroendoscopy

    vtkSphereSource Neuroendoscopy(fp,source)
    Neuroendoscopy(fp,source) SetRadius $Neuroendoscopy(fp,size)
    vtkPolyDataMapper Neuroendoscopy(fp,mapper)
    Neuroendoscopy(fp,mapper) SetInput [Neuroendoscopy(fp,source) GetOutput]
    vtkActor Neuroendoscopy(fp,actor)
    Neuroendoscopy(fp,actor) SetMapper Neuroendoscopy(fp,mapper)
    eval [Neuroendoscopy(fp,actor) GetProperty] SetColor $Neuroendoscopy(fp,color)
    set Neuroendoscopy(fp,distance) [expr $Neuroendoscopy(cam,boxwidth) *3] 
    Neuroendoscopy(fp,actor) SetPosition 0 $Neuroendoscopy(fp,distance) 0
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateVTKPath
# Calls NeuroendoscopyCreatePath and adds the path to the view.
# .ARGS
# int id the id of the neuroendoscopy path
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreateVTKPath {id} {

    global Neuroendoscopy
   
    #create the Path actors 
    NeuroendoscopyCreatePath $id   
    lappend Neuroendoscopy(actors) Neuroendoscopy($id,path,actor)

    #create the Vector actor
    # TODO: need to change from vtkVectors etc to vtkFloatArray and it's friends
    #NeuroendoscopyCreateVector $id
    #lappend Neuroendoscopy(actors) Neuroendoscopy($id,vector,actor)
    
    viewRen AddActor Neuroendoscopy($id,path,actor)
    Neuroendoscopy($id,path,actor) SetVisibility 1
    #viewRen AddActor Neuroendoscopy($id,vector,actor)
    #Neuroendoscopy($id,vector,actor) SetVisibility 1

    lappend Neuroendoscopy(path,allIdsUsed) $id
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyResetPathVariables
# Remove points, and reset vtk vars.
# .ARGS
# int id path id
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyResetPathVariables {id} {
    global Neuroendoscopy View
    
    foreach m {c f} {
        Neuroendoscopy($id,${m}path,Spline,x) RemoveAllPoints
        Neuroendoscopy($id,${m}path,Spline,y) RemoveAllPoints
        Neuroendoscopy($id,${m}path,Spline,z) RemoveAllPoints
        Neuroendoscopy($id,${m}path,keyPoints) Reset
        Neuroendoscopy($id,${m}path,graphicalInterpolatedPoints) Reset
        Neuroendoscopy($id,${m}path,allInterpolatedPoints) Reset
    }
    Neuroendoscopy($id,path,lines) Reset 
    Neuroendoscopy($id,path,polyData) Modified
    #Neuroendoscopy($id,vector,polyData) Modified
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreatePath
#  Create the vtk camera Path and focalPoint Path actors
# .ARGS
# int id path id
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreatePath {id} {
         global Neuroendoscopy

    
    # cpath: variables about the points along the camera path
    # fpath: variables about the points along the focal point path
    foreach m "c f" {
        vtkPoints    Neuroendoscopy($id,${m}path,keyPoints)  
        vtkPoints    Neuroendoscopy($id,${m}path,graphicalInterpolatedPoints)
        vtkPoints    Neuroendoscopy($id,${m}path,allInterpolatedPoints)
        
        vtkCardinalSpline Neuroendoscopy($id,${m}path,Spline,x)
        vtkCardinalSpline Neuroendoscopy($id,${m}path,Spline,y)
        vtkCardinalSpline Neuroendoscopy($id,${m}path,Spline,z)
        
        Neuroendoscopy($id,${m}path,keyPoints)  SetDataTypeToFloat
        Neuroendoscopy($id,${m}path,graphicalInterpolatedPoints) SetDataTypeToFloat
        Neuroendoscopy($id,${m}path,allInterpolatedPoints) SetDataTypeToFloat
    }
    
    # path: variables for the graphical representation of the (camera) path
    # the focal point stays invisible
    
    vtkPolyData         Neuroendoscopy($id,path,polyData)
    vtkCellArray        Neuroendoscopy($id,path,lines)    
    vtkTubeFilter       Neuroendoscopy($id,path,source)
    vtkPolyDataMapper   Neuroendoscopy($id,path,mapper)    
    vtkActor            Neuroendoscopy($id,path,actor)
    
    # set the lines input data
    Neuroendoscopy($id,path,polyData)     SetLines  Neuroendoscopy($id,path,lines)
    # set the ctube info
    Neuroendoscopy($id,path,source)       SetNumberOfSides 8
    Neuroendoscopy($id,path,source)       SetInput Neuroendoscopy($id,path,polyData)
    Neuroendoscopy($id,path,source)       SetRadius $Neuroendoscopy(path,size)
    Neuroendoscopy($id,path,mapper)       SetInput [Neuroendoscopy($id,path,source) GetOutput]
    Neuroendoscopy($id,path,actor)        SetMapper Neuroendoscopy($id,path,mapper)
    
    eval  [Neuroendoscopy($id,path,actor) GetProperty] SetDiffuseColor $Neuroendoscopy(path,color)
    [Neuroendoscopy($id,path,actor) GetProperty] SetSpecular .3
    [Neuroendoscopy($id,path,actor) GetProperty] SetSpecularPower 30
    # connect the path with the camera landmarks
    Neuroendoscopy($id,path,polyData) SetPoints Neuroendoscopy($id,cpath,graphicalInterpolatedPoints)
    
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateVector
#  Create the vtk vector actor
# .ARGS
# int id vector id
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreateVector {id} {

    global Neuroendoscopy

   
    vtkAppendPolyData Neuroendoscopy($id,vector,source)
    Neuroendoscopy($id,vector,source) AddInput [ctubeXformFilter GetOutput]
    Neuroendoscopy($id,vector,source) AddInput [carrowXformFilter GetOutput]
    
    NeuroendoscopyVectorParams $id 10
    
    vtkPolyData         Neuroendoscopy($id,vector,polyData)
    vtkVectors          Neuroendoscopy($id,vector,vectors)
    vtkGlyph3D          Neuroendoscopy($id,vector,glyph)
    vtkPolyDataMapper   Neuroendoscopy($id,vector,mapper)
    vtkActor            Neuroendoscopy($id,vector,actor)
    vtkScalars          Neuroendoscopy($id,vector,scalars)
    
    Neuroendoscopy($id,vector,polyData) SetPoints Neuroendoscopy($id,cpath,keyPoints)
    #    Neuroendoscopy($id,vector,polyData) SetPoints Neuroendoscopy(cLand,graphicalInterpolatedPoints)
    # set the vector glyphs
    [Neuroendoscopy($id,vector,polyData) GetPointData] SetVectors Neuroendoscopy($id,vector,vectors)
    [Neuroendoscopy($id,vector,polyData) GetPointData] SetScalars Neuroendoscopy($id,vector,scalars)
    
    Neuroendoscopy($id,vector,glyph) SetInput Neuroendoscopy($id,vector,polyData)
    Neuroendoscopy($id,vector,glyph) SetSource [Neuroendoscopy($id,vector,source) GetOutput]
    Neuroendoscopy($id,vector,glyph) SetVectorModeToUseVector
    Neuroendoscopy($id,vector,glyph) SetColorModeToColorByScalar
    Neuroendoscopy($id,vector,glyph) SetScaleModeToDataScalingOff

    
    Neuroendoscopy($id,vector,mapper) SetInput [Neuroendoscopy($id,vector,glyph) GetOutput]
    Neuroendoscopy($id,vector,actor) SetMapper Neuroendoscopy($id,vector,mapper)
    
    Neuroendoscopy($id,vector,actor) PickableOn

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyVectorParams
# Set parameters for the vector.
# .ARGS
# int id vector id
# int axislen optional, defaults to 10
# int axisrad optional, defaults to .05 times axislen
# int conelen optional, defaults to .2 times axislen
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyVectorParams {id {axislen -1} {axisrad -1} {conelen -1} } {
    global Neuroendoscopy
    if { $axislen == -1 } { set axislen 10 }
    if { $axisrad == -1 } { set axisrad [expr $axislen*0.05] }
    if { $conelen == -1 } { set conelen [expr $axislen*0.2]}
    set axislen [expr $axislen-$conelen]
    
    set Neuroendoscopy(vector,size) $axislen
    
    # set parameters for cylinder geometry and transform
    ctube SetRadius $axisrad
    ctube SetHeight $axislen
    ctube SetCenter 0 [expr -0.5*$axislen] 0
    ctube SetResolution 8
    ctubeXform Identity
    ctubeXform RotateZ 90
    
    # set parameters for cone geometry and transform
    carrow SetRadius [expr $axisrad * 2.5]
    carrow SetHeight $conelen
    carrow SetResolution 8
    carrowXform Identity
    carrowXform Translate $axislen 0 0
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUpdateVisibility
# 
#  This procedure updates the current visibility of actor a (if specified)
#  and then set that actor to its current visibility
# 
# .ARGS
#  str a  name of the actor Neuroendoscopy($a,actor)
#  int visibility (optional) 0 or 1 
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUpdateVisibility {a {visibility ""}} {
    
    global Neuroendoscopy

    if {$a == "cam"} {
        set a $Neuroendoscopy(cam,actor)
    }
    if {$a =="fp"} {
        set a $Neuroendoscopy(fp,actor)
    }
    if {$a =="gyro"} {
        set a $Neuroendoscopy(gyro,actor)
    }

    if {$visibility == ""} {
        # TODO: check that visibility is a number
        
        set visibility [$a GetVisibility]
    }
    $a SetVisibility $visibility
    if {$a == "Neuroendoscopy(cam,actor)"} {
        set Neuroendoscopy(gyro,visibility) $visibility
        Neuroendoscopy(gyro,actor) SetVisibility $visibility
    }
    NeuroendoscopySetPickable $a $visibility
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetPickable
# 
#  This procedure sets the pickability of actor a to the value specified
# 
# .ARGS
#  str a  name of the actor Neuroendoscopy($a,actor)
#  int pickability 0 or 1 
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetPickable {a pickability} {
    
    global Neuroendoscopy
    if {$pickability == 0} {
        set p Off
    } elseif {$pickability == 1} {
        set p On
    }
    if {$a == "Neuroendoscopy(gyro,actor)"} {
        foreach w "X Y Z" {
            Neuroendoscopy(gyro,${w}actor) Pickable$p
        }
    } else {
        $a Pickable$p 
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUpdateSize
#
# This procedure updates the size of actor a according to Neuroendoscopy(a,size)
#
# .ARGS
#  int a  name of the actor Neuroendoscopy($a,actor)
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetSize {a} {
    global Advanced Neuroendoscopy Path
    
    if { $a == "gyro"} {
        CsysParams Neuroendoscopy gyro $Neuroendoscopy($a,size)
    } elseif { $a == "cam"} {
        set Neuroendoscopy(fp,distance) [expr $Neuroendoscopy(cam,size) * 30]
        NeuroendoscopyCameraParams $Neuroendoscopy($a,size)  $Neuroendoscopy(cam,shapetemp)
        NeuroendoscopySetCameraPosition
    } elseif { $a == "vector" } {
        NeuroendoscopyVectorParams $Neuroendoscopy($a,size)
    } elseif { $Neuroendoscopy(path,exists) == 1 } {
        # a is cPath,fPath,cLand or fLand => set their radius
        #set Neuroendoscopy($a,size) Neuroendoscopy($a,sizeStr)
        Neuroendoscopy($a,source) SetRadius $Neuroendoscopy($a,size)
    }
    Render3D 
}

##############################################################################
#
#       PART 2: Build the Gui
#
##############################################################################

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyPopBindings
# Use the Event bindings, deactiveate the slice 0-2 events.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyPopBindings {} {
    global Ev Csys
    EvDeactivateBindingSet Slice0Events
    EvDeactivateBindingSet Slice1Events
    EvDeactivateBindingSet Slice2Events

    EvDeactivateBindingSet NeuroendoscopySlice0Events
    EvDeactivateBindingSet NeuroendoscopySlice1Events
    EvDeactivateBindingSet NeuroendoscopySlice2Events
    
#    EvDeactivateBindingSet 3DEvents
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyPushBindings
# Use the Event bindings, activate the slice 0-2 events.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyPushBindings {} {
    global Ev Csys

    # push onto the event stack a new event manager that deals with
    # events when the Neuroendoscopy module is active
    EvActivateBindingSet Slice0Events
    EvActivateBindingSet Slice1Events
    EvActivateBindingSet Slice2Events

    EvActivateBindingSet NeuroendoscopySlice0Events
    EvActivateBindingSet NeuroendoscopySlice1Events
    EvActivateBindingSet NeuroendoscopySlice2Events
    
#    EvActivateBindingSet 3DEvents
#  puts "binding pushed"       
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateBindings
# Create event handlers for keys m and t
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreateBindings {} {
    global Gui Ev
    
    # Creates events sets we'll  need for this module
    # create the event manager for the ability to move the gyro
     
# neuroendoscopy events for slice windows (in addition to already existing events)

#  <KeyPress-m> allows the user to move the endoscope quickly to a 3D location defined by the mouse, without using the gyro      
   EvDeclareEventHandler EndoKeyMoveSlicesEvents <KeyPress-m> { if { [SelectPick2D %W %x %y] != 0 } \
   {eval NeuroendoscopySetWorldPosition [lindex $Select(xyz) 0] [lindex $Select(xyz) 1] [lindex $Select(xyz) 2]; Render3D} }

# <KeyPress-t> allows the user to select a target on the 3D colon, and in the 2D slices
   EvDeclareEventHandler EndoKeySelectSlicesEvents <KeyPress-t> { if {[SelectPick2D %W %x %y] != 0} \
   {eval NeuroendoscopyAddTargetFromSlices [lindex $Select(xyz) 0] [lindex $Select(xyz) 1] [lindex $Select(xyz) 2]} }

   EvAddWidgetToBindingSet Slice0Events $Gui(fSl0Win) {{EndoKeySelectSlicesEvents} {EndoKeyMoveSlicesEvents}}
   EvAddWidgetToBindingSet Slice1Events $Gui(fSl1Win) {{EndoKeySelectSlicesEvents} {EndoKeyMoveSlicesEvents}}
   EvAddWidgetToBindingSet Slice2Events $Gui(fSl2Win) {{EndoKeySelectSlicesEvents} {EndoKeyMoveSlicesEvents}}

#  Activated in TkInteractor
#   EvDeclareEventHandler EndoKeySelect3DEvents <KeyPress-t> { if { [SelectPick Neuroendoscopy(picker) %W %x %y] != 0 } \
    { eval NeuroendoscopyAddTargetFromWorldCoordinates [lindex $Select(xyz) 0] [lindex $Select(xyz) 1] [lindex $Select(xyz) 2] $Select(cellId) }}
    
#  Slice View get coordinates
    EvDeclareEventHandler NeuroendoscopySlicesEvents <1> \
        {set xc %x; set yc %y; NeuroendoscopyProcessMouseEvent $xc $yc}

    EvAddWidgetToBindingSet NeuroendoscopySlice0Events $Gui(fSl0Win) {NeuroendoscopySlicesEvents}
    EvAddWidgetToBindingSet NeuroendoscopySlice1Events $Gui(fSl1Win) {NeuroendoscopySlicesEvents}
    EvAddWidgetToBindingSet NeuroendoscopySlice2Events $Gui(fSl2Win) {NeuroendoscopySlicesEvents}    



#   EvAddWidgetToBindingSet 3DEvents $Gui(fViewWin) {EndoKeySelect3DEvents}
}


proc NeuroendoscopyProcessMouseEvent {x y} {
    global Locator Interactor Anno



    # Which slice was picked?
    set s $Interactor(s)
    if {$s == ""} {
        DevErrorWindow "No slice was picked."
        return
    }

    # Get RAS coordinates
    set R [Anno($s,cur1,mapper) GetInput]
    set rl [split $R " "]
    set R [lindex $rl 1]

    set A [Anno($s,cur2,mapper) GetInput]
    set al [split $A " "]
    set A [lindex $al 1]

    set S [Anno($s,cur3,mapper) GetInput]
    set sl [split $S " "]
    set S [lindex $sl 1]

    # One point
    set Locator(entry,slicerCoords) "$R   $A   $S"
}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyBuildGUI
# Create the Graphical User Interface.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyBuildGUI {} {
    global Gui Module Model View Advanced Neuroendoscopy Path PathPlanning Label Mrml Fiducials Locator 
    
    
    set LposTexts "{L<->R} {P<->A} {I<->S}"
    set RposTexts "{Pitch  } {Roll  } {Yaw  }"
    set posAxi "x y z"

    set dirTexts "{X} {Y} {Z}"
    set dirAxi "rx ry rz"

    # first add the Neuroendoscopy label to the View menu in the main Gui
    $Gui(mView) add separator
    $Gui(mView) add command -label "Add Neuroendoscopy View" -command "NeuroendoscopyAddNeuroendoscopyView; Render3D"
    $Gui(mView) add command -label "Remove Neuroendoscopy View" -command "NeuroendoscopyRemoveNeuroendoscopyView; Render3D"
    
  #jeanette: add Flatwindow label to the view menu in the main Gui
  #  $Gui(mView) add command -label "Add Flat Colon View" -command "MainMenu File Import"
  #  $Gui(mView) add command -label "Remove Flat Colon View" -command "NeuroendoscopyRemoveFlatView"

    # A frame has already been constructed automatically for each tab.
    # A frame named "Stuff" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(Neuroendoscopy,fStuff)
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
    This module allows you to position an neuroendoscopy camera in the scene and 
    view through the camera's lens in the second window.
    You can also create a fly-through trajectory and save it in the MRML file by selecting File -> 'Save Scene As'.

    <P>Tab content descriptions:<BR>
    <UL>
    <LI><B>Display</B> 
    <BR> This Tab permits you to set display parameters. It also allows you to open a separate window to view the flattened colon (if you plan to navigate along the flattened colon)
    <BR> 
    <UL>
    <LI><B>Camera</B> 
    <BR>This tab contains controls to navigate the camera and create landmarks.
    <P><B>To move and rotate the camera</B>:
    <BR>- select absolute mode (to move along the world's axis) or relative mode (to move along the camera's axis)
    <BR>- to move the camera with precision, use the sliders or type a world coordinate in the text box, next to the sliders. For the orientation of the camera, the number displayed is the angle of rotation in degrees.
    <BR>- to move the camera intuitively, use the 3D Gyro (read instructions on the tab). 
    <BR>- you can also move the camera by combining the shift or control key with mouse button press and motion. For example: Shift+moving the mouse with left button pressed down controls the camera's 'Pitch'. Shift + middle or right mouse button motion controls the camera's 'Roll' and 'Yaw' respectively. Control + Left, middle, or right mouse button press and motion controls the camera's 'L<->R', 'I<->S', and 'P<->A'. The precise amount of camera motion, as you use the key + mouse motion, can be read off from the sliders in the camera tab.
    <P>
    <BR><B>To create a path</B>:
    <BR><B>Automatically:</B>
    <BR> -If you have a closed 3D model, you can create a path automatically by clicking the button <I>AutoExtractCenterLine</I>. A begin and end point will be automatically selected, and a path generated in between.
    <BR><B>Manually</B>:
    <BR> You have 2 options: you can pick a landmark position on a 2D slice or in the 3D world:
    <BR> * <B>IN 3D</B>
    <BR>- Position the camera where desired
    <BR>- Click the button <I>AddLandmark</I>: this will add a landmark (a sphere) at the current camera position and another landmark at<BR> the current focal point position 
    <BR>- Two paths will automatically be created: one that links all the camera landmarks and another one that links all the focal point landmarks
    <BR> * <B>IN 2D</B>
    <BR> - You need to have greyscale images loaded along with the model, otherwise this option won't work
    <BR> - position the mouse in the slice window of your choice until the cross hair is on top of the region where you want to add a landmark
    <BR> - press the <B>m</B> key, and the endoscope will jump to the corresponding location in 3D. You can then Click the button <I>AddLandmark</I>.
    <BR> - by default, both a camera and focal point landmark will be added at this position
    <BR><B>To constrain the camera on the path</B>:
    <BR> -After a path or centerline is created, you can manually insert visible fiducials on the model while constrain the camera along the path.
    <BR> -Point the cursor on the 3D model or on the 2D slices below, and press the key 't', the endoscope will jump to the closet point along the path, aim at the selected location, and create a visible target. If you have the flattened colon open, just press shift + double click left mouse button on the intended location, and the endoscope will perform the same task. You can also set the camera location and orientation manually and press <I>CreateTarget</I> to insert a visible fiducial point on the 3D model.
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
    <BR><B>FastForward or Backward</B>:
    <BR> -If you have a series of targets along the path that you would look at, you can click the FastForward or Backward button to jump between the targets.
    <BR><B>Command sliders</B>:
    <BR><I>- Frame</I>: positions the camera on any part of the path
    <BR><I>- Speed</I>: controls the speed of the camera's motion along the path (the number corresponds to the number of frames to skip)
    <BR><I>- No of Interpolated Points </I>: this will recompute the path with approximately the number of interpolated points per mm specified in between key points (landmarks).
    <BR><B>To drive the slices</B>:
    <BR> Select which driver you want: 
    <BR> - <B>User</B>: the user selects the slice position and orientation with the user interface
    <BR> - <B>Camera</B>: the slices in the orientation 'InPlane', 'Perp', 'InPlane90' are reformatted to intersect at the camera position and be oriented along the 3 axes of the camera.
    <BR> - <B>Focal Point</B>: same as above with the focal point instead of the camera.
    <BR> - <B>Intersection</B>: the intersection is the point on the surface of the model in the center of the neuroendoscopy screen. It is the intersection between a 'ray' shot from the camera straight ahead and any triangulated surface that is first intersected.
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
    MainHelpApplyTags Neuroendoscopy $help
    MainHelpBuildGUI Neuroendoscopy

    #-------------------------------------------
    # Display frame
    #-------------------------------------------
    set fDisplay $Module(Neuroendoscopy,fDisplay)
    set f $fDisplay
        
    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    frame $f.fMain -bg $Gui(activeWorkspace) -relief groove -bd 2 
    frame $f.fEndo -bg $Gui(activeWorkspace) -relief groove -bd 2 
    frame $f.fTitle2 -bg $Gui(activeWorkspace)
    frame $f.fFlat -bg $Gui(activeWorkspace) 
    # -relief groove -bd 2
    pack $f.fTitle $f.fBtns $f.fMain $f.fEndo $f.fTitle2 $f.fFlat \
    -side top -pady 2 -fill x
    
    
    eval {label $f.fTitle.lTitle -text "
    If your screen allows, you can change 
    the width of the view screen to have 
    a better Neuroendoscopy view:"} $Gui(WLA)
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
    eval {label $f.fexpl -text "Main (Global) View parameter:"} $Gui(WLA)
    eval {checkbutton $f.cMainView -variable Neuroendoscopy(mainview,visibility) -text "Show Main (Global) View" -command "NeuroendoscopyUpdateMainViewVisibility" -indicatoron 0} $Gui(WCA)    

    pack $f.fexpl $f.cMainView -pady 2
    

    set f $fDisplay.fEndo
    eval {label $f.fexpl -text "Neuroendoscopy View parameters:"} $Gui(WLA)
    
    eval {checkbutton $f.cEndoView -variable Neuroendoscopy(endoview,visibility) -text "Show Neuroendoscopy View" -command "NeuroendoscopyUpdateNeuroendoscopyViewVisibility" -indicatoron 0} $Gui(WCA)    
    
    eval {checkbutton $f.cslices -variable Neuroendoscopy(SlicesVisibility) -text "Show 2D Slices" -command "NeuroendoscopySetSlicesVisibility" -indicatoron 0} $Gui(WCA)
    
    eval {checkbutton $f.cExitEndoView -variable Neuroendoscopy(endoview,hideOnExit) -text "Hide Neuroendoscopy View on Exit" -indicatoron 0} $Gui(WCA)
    
    pack $f.fexpl -side top -pady 2
    pack $f.cEndoView  -side top -padx $Gui(pad) -pady 0
    pack $f.cslices -side top -pady 5
    pack $f.cExitEndoView -side top -padx $Gui(pad) -pady 7

 # Fix later: Ask user if they like the parameters and then proceed to either Camera or Path
    # The following 5 lines appears un-finished by Delphine;
    set f $fDisplay
    set text "
    To start, go to the Camera tab 
    If you need help, go to the Help tab"
    eval {label $f.fTitle2.lTitle -text $text} $Gui(WLA)
        
    
    set f $fDisplay.fFlat
    
    eval {label $f.fexpl -text "Flat View parameters:"} $Gui(WLA)
    
  # Fix later: Need some checking mechanism 
      # (1) to see if the flat file comes with the xml
    # (2) to make sure the flat file matches the right model

  
    eval {label $f.fSelectLabel -text "Select a Flattened Colon File (.vtk), click -Apply-"} $Gui(WLA)
    pack $f.fexpl $f.fSelectLabel -pady 2
    
    set f $fDisplay.fFlat 
    DevAddFileBrowse $f Neuroendoscopy FlatSelect "" "NeuroendoscopySetFlatFileName" "vtk"
    
    set f $fDisplay.fFlat
    DevAddButton $f.bChoose "Apply" "NeuroendoscopyAddFlatView"
    DevAddButton $f.bCancel "Cancel" "NeuroendoscopyCancelFlatFile"
    pack $f.bChoose -side left -padx 30
    pack $f.bCancel -side right -padx 30
    
    set f $fDisplay
    eval {checkbutton $f.cExitFlatView -variable Neuroendoscopy(flatview,hideOnExit) -text "Hide Flat View on Exit" -indicatoron 0} $Gui(WCA)
    pack $f.cExitFlatView -side bottom -pady 2
    # eval {checkbutton $f.cFlatView -variable Neuroendoscopy(flatview,visibility) -text "Show Flat View" -command "NeuroendoscopyUpdateFlatViewVisibility" -indicatoron 0} $Gui(WCA)    
    
    
   




 
    #-------------------------------------------
    # Registration frame
    #-------------------------------------------
    set fRegistration $Module(Neuroendoscopy,fRegistration)
    set f $fRegistration
    
    frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fBot -bg $Gui(activeWorkspace) -height 340

    pack $f.fTop -side top -pady $Gui(pad) -padx $Gui(pad) -fill x
    pack $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill both -expand yes

    set PathMenu2 {ICP1 ICP2 ICP3 ICP4 LM1}


    
    #--------------------------------------------
    # Bottom frame
    #------------------------------------------
    set f $fRegistration.fBot

    foreach type $PathMenu2 {
        frame $f.f${type} -bg $Gui(activeWorkspace)
        place $f.f${type} -in $f -relheight 1.0 -relwidth 1.0
        set Model(f${type}) $f.f${type}
    }
   # raise $Model(fSTEP1)
    

    set f $fRegistration.fTop
    set row 0



    foreach {e1 e2 e3 e4} $PathMenu2 {
        frame $f.$row -bg $Gui(activeWorkspace)
        
        foreach e "$e1 $e2 $e3 $e4" {
            # Either make a button for it, or add it to the "more" menu
           
                eval {radiobutton $f.$row.r$e -width 7 \
                    -text "$e" -variable Model(propertyType) -value $e \
                    -command "ModelsSetPropertyType" -indicatoron 0} $Gui(WCA)
                pack $f.$row.r$e -side left -padx 0 -pady 0
            
        }
        pack $f.$row -side top -padx 0 -pady 0
        
        incr row
   }

   #press the first tab button and show the first tab
   $f.0.rICP1 invoke

  
    #-------------------------------------------
    # Registration->Step2
    #-------------------------------------------
    
    set f $fRegistration.fBot.fICP1
    frame $f.fClipping -bg $Gui(activeWorkspace)
    pack $f.fClipping -side top -fill x -pady $Gui(pad) 

    set f $fRegistration.fBot.fICP1.fClipping
    eval {label $f.c1StepLabel -text "1 STEP: Calibration of device for point selection"} $Gui(WLA)

    #---------------------------------------------------------------------
    
    DevAddButton $f.bNext "->->NEXT->->" "NeuroendoscopyICP2" 10
    DevAddButton $f.bCalib "(Re) - Start probe calibration" "NeuroendoscopyICP1"  30

    pack $f.c1StepLabel $f.bCalib $f.bNext -side top -pady 3 -padx 2
   
    #--------------------------------------------
    # Registration->Step3
    #--------------------------------------------
    set f $fRegistration.fBot.fICP2
    frame $f.fData   -bg $Gui(activeWorkspace)
    pack  $f.fData -side top 

    set f $fRegistration.fBot.fICP2.fData
     
    eval {label $f.c1StepLabel -text "2 STEP: Point selection by surgeon"} $Gui(WLA)
    pack $f.c1StepLabel -side top -pady 3 -padx 2

    #---------------------------------------------------------------------
    
    DevAddButton $f.bNext "->->NEXT->->" "NeuroendoscopyICP2" 10
    
    eval {radiobutton $f.bNaviCon -text "Read Points from Tracking device" -width 25  -variable Neuroendoscopy(connect) -command "NeuroendoscopyNaviTrackConnect; LocatorRegisterCallback NeuroendoscopyLoopOpenTracker" } $Gui(WBA)
    #DevAddButton $f.bNaviCon "Connect to Navitrack" "NeuroendoscopyNaviTrackConnect" 10
  
    DevAddButton $f.bCalib "Footswitch Stop Collecting" "LocatorUnRegisterCallback NeuroendoscopyLoopOpenTracker"  30
    DevAddButton $f.bModel "Generate model file" "NeuroendoscopyGenerateTestCoordinateFile skull.dat"  30


    pack  $f.bNaviCon $f.bCalib $f.bModel $f.bNext -side top -pady 3 -padx 2

    #--------------------------------------------
    # Registration->Step4
    #--------------------------------------------


    set f $fRegistration.fBot.fICP3
    frame $f.fData   -bg $Gui(activeWorkspace)
    pack  $f.fData -side top 
    
    set f $fRegistration.fBot.fICP3.fData
     
    eval {label $f.c1StepLabel -text "3 STEP: Coarse Registration"} $Gui(WLA)
    pack $f.c1StepLabel -side top -pady 3 -padx 2

    #---------------------------------------------------------------------
    
    DevAddButton $f.bNext "->->NEXT->->" "NeuroendoscopyICP3" 10
    DevAddButton $f.bCalib "(Re) - Start coarse calibration" "NeuroendoscopyStartICP"  30
    DevAddButton $f.bShowRes "Show me the result!" "NeuroendoscopyShowICP" 30
    pack $f.bCalib $f.bShowRes $f.bNext -side top -pady 3 -padx 2
   
    #--------------------------------------------
    # Registration->Step5
    #--------------------------------------------


     set f $fRegistration.fBot.fICP4
    frame $f.fData   -bg $Gui(activeWorkspace)
    pack  $f.fData -side top 
    
    set f $fRegistration.fBot.fICP4.fData
     
   #--- create blt notebook
    blt::tabset $f.tsNotebook -relief flat -borderwidth 0
    pack $f.tsNotebook -side top

    #--- notebook configure
    $f.tsNotebook configure -width 250
    $f.tsNotebook configure -height 900 
    $f.tsNotebook configure -background $::Gui(activeWorkspace)
    $f.tsNotebook configure -activebackground $::Gui(activeWorkspace)
    $f.tsNotebook configure -selectbackground $::Gui(activeWorkspace)
    $f.tsNotebook configure -tabbackground $::Gui(activeWorkspace)
    $f.tsNotebook configure -highlightbackground $::Gui(activeWorkspace)
    $f.tsNotebook configure -highlightcolor $::Gui(activeWorkspace)
    $f.tsNotebook configure -foreground black
    $f.tsNotebook configure -activeforeground black
    $f.tsNotebook configure -selectforeground black
    $f.tsNotebook configure -tabforeground black
    $f.tsNotebook configure -relief flat
    $f.tsNotebook configure -tabrelief raised

    #--- tab configure
    set i 0
    foreach t "Registration" {
        $f.tsNotebook insert $i $t
        frame $f.tsNotebook.f$t -bg $Gui(activeWorkspace) -bd 2 
        NeuroendoscopyBuildGUIForICP${t} $f.tsNotebook.f$t

        $f.tsNotebook tab configure $t -window $f.tsNotebook.f$t 
        $f.tsNotebook tab configure $t -activebackground $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -selectbackground $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -background $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -fill both -padx 2 -pady 1 

        incr i
    }
    #-----------------------------------------------------------------





    eval {label $f.c1StepLabel -text "4 STEP: Point acquisition"} $Gui(WLA)
    pack $f.c1StepLabel -side top -pady 3 -padx 2

    #---------------------------------------------------------------------
    
    #DevAddButton $f.bNext "->->NEXT->->" "NeuroendoscopySTEP5" 10
    
    DevAddButton $f.bCalibstart "Start aquisition of fiducials" "NeuroendoscopyICP4"  30
    eval {checkbutton $f.bFootsw -width 7 \
                    -text "Footswitch" -variable Neuroendoscopy(footswitch) \
                    -indicatoron 0} $Gui(WCA)
               
    DevAddButton $f.bCalibstop "Stop acquisition of fiducials" "NeuroendoscopyICP4"  30


    pack $f.bCalibstart  $f.bFootsw $f.bCalibstop  -side top -pady 3 -padx 2
    #--------------------------------------------
    # Registration->Landmark Step 1
    #--------------------------------------------

    set f $fRegistration.fBot.fLM1
    frame $f.fPause   -bg $Gui(activeWorkspace)
    frame $f.fData   -bg   $Gui(activeWorkspace) -height 900
    pack   $f.fData $f.fPause -side top 
    
   set f $fRegistration.fBot.fLM1.fPause
    eval {checkbutton $f.cPause \
        -text "Run Locator" -variable Neuroendoscopy(Pause) -width 16 \
        -indicatoron 0 -command "NeuroendoscopyPauseLocator"} $Gui(WCA)

    pack $f.cPause -side top
    set f $fRegistration.fBot.fLM1.fData
    
    
    #-------------------------------------------
    # Server->Bot->OpenTracker frame
    #-------------------------------------------
    #set f $fServer.fBot.fOpenTracker

    #--- create blt notebook
    blt::tabset $f.tsNotebook -relief flat -borderwidth 0
    pack $f.tsNotebook -side top

    #--- notebook configure
    $f.tsNotebook configure -width 250
    $f.tsNotebook configure -height 900 
    $f.tsNotebook configure -background $::Gui(activeWorkspace)
    $f.tsNotebook configure -activebackground $::Gui(activeWorkspace)
    $f.tsNotebook configure -selectbackground $::Gui(activeWorkspace)
    $f.tsNotebook configure -tabbackground $::Gui(activeWorkspace)
    $f.tsNotebook configure -highlightbackground $::Gui(activeWorkspace)
    $f.tsNotebook configure -highlightcolor $::Gui(activeWorkspace)
    $f.tsNotebook configure -foreground black
    $f.tsNotebook configure -activeforeground black
    $f.tsNotebook configure -selectforeground black
    $f.tsNotebook configure -tabforeground black
    $f.tsNotebook configure -relief flat
    $f.tsNotebook configure -tabrelief raised

    #--- tab configure
    set i 0
    foreach t "Registration" {
        $f.tsNotebook insert $i $t
        frame $f.tsNotebook.f$t -bg $Gui(activeWorkspace) -bd 2 
        LocatorBuildGUIFor${t} $f.tsNotebook.f$t

        $f.tsNotebook tab configure $t -window $f.tsNotebook.f$t 
        $f.tsNotebook tab configure $t -activebackground $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -selectbackground $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -background $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -fill both -padx 2 -pady 1 

        incr i
    }

    #--------------------------------------------------------------------------------------
 
    eval {label $f.c1StepLabel -text "1 STEP: ProbeCalibration"} $Gui(WLA)
    pack $f.c1StepLabel -side top -pady 3 -padx 2

    #---------------------------------------------------------------------
    
    DevAddButton $f.bNext "->->NEXT->->" "NeuroendoscopyLM1" 10
    DevAddButton $f.bCalib "Start registration" "NeuroendoscopyLM1"  30

    pack $f.bCalib $f.bNext -side top -pady 3 -padx 2



  
    #----------------------------------------------------------------------
    ##########################################################################
    #-------------------------------------------
    # Camera frame
    #-------------------------------------------
    set fCamera $Module(Neuroendoscopy,fCamera)
    set f $fCamera
    
   
   # set fRegistration $Module(Neuroendoscopy,fRegistration)
   # set f $fRegistration
    
    frame $f.fMenu -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fBot -bg $Gui(activeWorkspace) -height 500
    #FiducialsAddActiveListFrame $f.fBot 400 10  


    pack $f.fMenu -side top -pady $Gui(pad) -padx $Gui(pad) -fill both
    pack $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill both -expand yes
    # here we set what we want to see on the menu button
    set PathMenu2 {Orientation Shape Probe Capture Texture}

    
    #--------------------------------------------
    # Camera->Bottom frame
    #------------------------------------------
    set f $fCamera.fBot

    foreach type $PathMenu2 {
        frame $f.f${type} -bg $Gui(activeWorkspace)
        place $f.f${type} -in $f -relheight 1.0 -relwidth 1.0
        set Model(f${type}) $f.f${type}
    }
    raise $Model(fOrientation)


    set f $fCamera.fMenu
    set row 0



    foreach {e1 e2 e3 e4} $PathMenu2 {
        frame $f.$row -bg $Gui(activeWorkspace)
        
        foreach e "$e1 $e2 $e3 $e4" {
            # Either make a button for it, or add it to the "more" menu
           
                eval {radiobutton $f.$row.r$e -width 10 \
                    -text "$e" -variable Model(propertyType) -value $e \
                    -command "ModelsSetPropertyType" -indicatoron 0} $Gui(WCA)
                pack $f.$row.r$e -side left -padx 0 -pady 0
            
        }
        pack $f.$row -side top -padx 0 -pady 0
        
        incr row
   }


    $f.0.rOrientation invoke

    #-------------------------------------------
    # Camera->Bot frame->Orienation Tab
    #-------------------------------------------
    set f $fCamera.fBot.fOrientation
    frame $f.fTop -bg $Gui(activeWorkspace)
    frame $f.fMid -bg $Gui(activeWorkspace)
    frame $f.fBot -bg $Gui(activeWorkspace)

   
    pack $f.fTop -side top -pady $Gui(pad) -padx $Gui(pad) -fill both -expand yes
    pack $f.fMid -side top -pady $Gui(pad) -padx $Gui(pad) -fill both -expand yes
    pack $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill both -expand yes


    
    #-------------------------------------------
    # Camera->Bot frame->Orienation Tab->Top Fr
    #-------------------------------------------

    set f $fCamera.fBot.fOrientation.fTop

    frame $f.fGyro   -bg $Gui(activeWorkspace)
    frame $f.fGyro2  -bg $Gui(activeWorkspace)
    frame $f.fInfo   -bg $Gui(activeWorkspace)
    frame $f.fInfo2  -bg $Gui(activeWorkspace)
    pack  $f.fGyro $f.fGyro2 $f.fInfo $f.fInfo2 -side top 
    
    
    #-------------------------------------------
    # Camera->Bot->Orienation Tab-->Title frame
    #-------------------------------------------
    
    set f $fCamera.fBot.fOrientation.fTop.fGyro
   

   #eval {checkbutton $f.clocator -variable Neuroendoscopy(Locator,use) -text "Show Locator" -command "NeuroendoscopyUseLocator" -indicatoron 0 } $Gui(WBA)


    eval {checkbutton $f.cgyro -variable Neuroendoscopy(gyro,use) -text "Use the 3D Gyro" -command "NeuroendoscopyUseGyro; Render3D" -indicatoron 0 } $Gui(WBA)

    set Neuroendoscopy(gyro,image,absolute) [image create photo -file \
        [ExpandPath [file join gui endoabsolute.gif]]]
    
    set Neuroendoscopy(gyro,image,relative) [image create photo -file \
        [ExpandPath [file join gui endorelative.gif]]]

    set Neuroendoscopy(imageButton) $fCamera.fBot.fOrientation.fTop.fGyro.cb
    
    eval {checkbutton $Neuroendoscopy(imageButton)  -image $Neuroendoscopy(gyro,image,relative) -variable Neuroendoscopy(cam,navigation,relative) -width 70 -indicatoron 0 -height 70} $Gui(WBA)
 
    eval {button $f.bhow -text "How ?"} $Gui(WBA)
    TooltipAdd $f.bhow "Select an axis by clicking on it with the mouse.
Translate the axis by pressing the left mouse button and moving the mouse.
Rotate the axis by pressing the right mouse button and moving the mouse."

    pack $f.cb $f.cgyro $f.bhow -side left -pady 3 -padx 2

    set f $fCamera.fBot.fOrientation.fTop.fGyro2
    eval {label $f.lgyroOr -text "Orientation"} $Gui(WTA)
    foreach item "absolute relative" {    
        eval {radiobutton $f.f$item -variable Neuroendoscopy(cam,axis) -text $item -value $item -command "$Neuroendoscopy(imageButton) configure -image $Neuroendoscopy(gyro,image,${item}); NeuroendoscopySetGyroOrientation; NeuroendoscopySetCameraAxis $item; Render3D"} $Gui(WBA)
        pack $f.lgyroOr $f.f$item -side left -padx 2 -pady 2
    }
    
    #-------------------------------------------
    # Camera->Bot->Orienation Tab->Mid frame
    #-------------------------------------------
    set f $fCamera.fBot.fOrientation.fMid

    frame $f.fPosTitle   -bg $Gui(activeWorkspace)
    frame $f.fPos   -bg $Gui(activeWorkspace)
    pack $f.fPosTitle $f.fPos -side top
    frame $f.fRotTitle     -bg $Gui(activeWorkspace)
    frame $f.fRot     -bg $Gui(activeWorkspace)
    frame $f.fZoom    -bg $Gui(activeWorkspace)
    pack $f.fRotTitle $f.fRot -side top 
    #    pack $f.fZoom -side top


    set f $fCamera.fBot.fOrientation.fMid.fPosTitle
    
    eval {label $f.l -height 1 -text "Camera Position"} $Gui(WTA)
    eval {button $f.r \
        -text "reset" -width 10 -command "NeuroendoscopyResetCameraPosition; Render3D"} $Gui(WBA)            
    pack $f.l $f.r -padx 1 -pady 1 -side left
    
    set f $fCamera.fBot.fOrientation.fMid.fPos

    #Position Sliders
    foreach slider $posAxi Ltext $LposTexts Rtext $RposTexts orient "horizontal horizontal vertical" {
        NeuroendoscopyCreateLabelAndSlider $f l$slider 0 "$Ltext" $slider $orient -400 400 110 Neuroendoscopy(cam,${slider}Str) "NeuroendoscopySetCameraPosition $slider" 5 0
        set Neuroendoscopy(slider$slider) $f.s$slider
        set Neuroendoscopy(label$slider) $f.l$slider
    }

    

    #-------------------------------------------
    # Camera->Bot->Orienation Tab->Mid->Title frame
    #-------------------------------------------

    set f $fCamera.fBot.fOrientation.fMid.fRotTitle

    eval {label $f.l -height 1 -text "Camera Orientation"} $Gui(WTA)
    eval {button $f.r \
        -text "reset" -width 10 -command "NeuroendoscopyResetCameraDirection; Render3D"} $Gui(WBA)            
    grid $f.l $f.r -padx 1 -pady 1

    #-------------------------------------------
    # Camera->Bot->Orienation Tab->Mid->Dir frame
    #-------------------------------------------
    set f $fCamera.fBot.fOrientation.fMid.fRot

    # Rotation Sliders
    foreach slider $dirAxi Rtext $RposTexts orient "horizontal horizontal vertical" {

        NeuroendoscopyCreateLabelAndSlider $f l$slider 0 "$Rtext" $slider $orient -360 360 110 Neuroendoscopy(cam,${slider}Str) "NeuroendoscopySetCameraDirection $slider" 5 0
        set Neuroendoscopy(slider$slider) $f.s$slider
        set Neuroendoscopy(label$slider) $f.l$slider
    }

    #-------------------------------------------
    # Camera->Bot->Orienation Tab->Mid->Zoom frame
    #-------------------------------------------
    set f $fCamera.fBot.fOrientation.fMid.fZoom

    # Position Sliders

    eval {label $f.l2 -height 2 -text "Camera Zoom"} $Gui(WTA)
    grid $f.l2
    
    NeuroendoscopyCreateLabelAndSlider $f lzoom 0 "In<->Out" $slider horizontal 1 500 110 Neuroendoscopy(cam,zoomStr) "NeuroendoscopySetCameraZoom" 5 30

    set f $fCamera.fBot.fOrientation.fBot

    frame $f.fTop -bg $Gui(activeWorkspace) 
    frame $f.fMid -bg $Gui(activeWorkspace) 
    pack $f.fTop $f.fMid -side top -pady 0 -padx 0 -fill x

    #-------------------------------------------
    # Camera->Shape
    #-------------------------------------------
    set f $fCamera.fBot.fShape
    frame $f.fTop -bg $Gui(activeWorkspace) -height 300

    pack $f.fTop -side top -pady $Gui(pad) -padx $Gui(pad) -fill both -expand yes
   
    #-------------------------------------------
    # Camera->Shape->Mid frame 
    #-------------------------------------------
    set f $fCamera.fBot.fShape.fTop

    frame $f.fAngle  -bg $Gui(activeWorkspace)
    frame $f.fTitle  -bg $Gui(activeWorkspace)
    frame $f.fToggle -bg $Gui(activeWorkspace)
    frame $f.fVis    -bg $Gui(activeWorkspace)
    

    
    pack $f.fAngle $f.fTitle $f.fToggle  $f.fVis  -side top -padx 1 -pady 1
    
   
    
    set f $fCamera.fBot.fShape.fTop.fTitle
    NeuroendoscopyCreateCheckButton $f.cShowAngle Neuroendoscopy(cam,showAngle) "Show Lens Angle"  "NeuroendoscopySetCameraViewAngle; Render3D"
    pack $f.cShowAngle

    set f $fCamera.fBot.fShape.fTop.fToggle
    
    NeuroendoscopyCreateAdvancedGUI $f cam  shape visible noColor size
    
    
   
    
    eval {label $f.lV -text Visibility} $Gui(WLA)
    eval {label $f.lO -text Color} $Gui(WLA)
        eval {label $f.lS -text "  Size"} $Gui(WLA)
    
    grid $f.lV $f.lO $f.lS -pady 2 -padx 2

    # Done in NeuroendoscopyCreateAdvancedGUI



    #NeuroendoscopyCreateAdvancedGUI $f cam shape visible noColor size
    
    NeuroendoscopyCreateAdvancedGUI $f Lens all notvisible color
    NeuroendoscopyCreateAdvancedGUI $f Lens2 all notvisible color
    NeuroendoscopyCreateAdvancedGUI $f Box  all notvisible color
    NeuroendoscopyCreateAdvancedGUI $f Box2 all notvisible color
    NeuroendoscopyCreateAdvancedGUI $f fp   all visible    color
    NeuroendoscopyCreateAdvancedGUI $f gyro all   visible   noColor size
    
    set f $fCamera.fBot.fShape.fTop.fAngle
    NeuroendoscopyCreateLabelAndSlider $f l2 2 "Lens Angle" "Angle" horizontal 0 360 110 Neuroendoscopy(cam,AngleStr) "NeuroendoscopySetCameraViewAngle" 5 90



    #-------------------------------------------
    # Camera->Shape->Mid->Vis frame
    #-------------------------------------------
    
    set f $fCamera.fBot.fShape.fTop.fVis


    
    NeuroendoscopyCreateCheckButton $f.cmodels Neuroendoscopy(ModelsVisibilityInside) "Show Inside Models"  "NeuroendoscopySetModelsVisibilityInside"

    
    pack $f.cmodels

    # reinstating the show path toggle button
    NeuroendoscopyCreateCheckButton $f.cPath Neuroendoscopy(path,showPath) "Show Path" "NeuroendoscopyShowPath; Render3D"
    pack $f.cPath 

      
    #-------------------------------------------
    # Camera->Probe
    #-------------------------------------------
    set f $fCamera.fBot.fProbe
    
    frame $f.fTop -bg $Gui(activeWorkspace)

    pack $f.fTop -pady $Gui(pad) -padx $Gui(pad) -side top  -expand yes -fill both
   
    #-------------------------------------------
    # Camera->Probe->Mid frame 
    #-------------------------------------------
    set f $fCamera.fBot.fProbe.fTop

    frame $f.fProbeActivation  -bg $Gui(activeWorkspace)
    frame $f.fSensorActivation  -bg $Gui(activeWorkspace)
    frame $f.fRuler -bg $Gui(activeWorkspace)
    
    frame $f.fView -bg $Gui(activeWorkspace)
    
    pack  $f.fProbeActivation $f.fSensorActivation $f.fRuler $f.fView -side top 

#-------------------------------------------
    # Camera->Probe->Top frame ->ProbeActivation
    #-------------------------------------------

     set f $fCamera.fBot.fProbe.fTop.fProbeActivation
     eval {checkbutton $f.cLocator \
        -text "Show Sensor" -variable Neuroendoscopy(visibility) -width 16 \
        -indicatoron 0 -command "NeuroendoscopySetLocator; LocatorSetVisibility; Render3D"} $Gui(WCA)
     eval {checkbutton $f.cRuler \
        -text "Show Ruler" -variable Neuroendoscopy(rulerVisibility) -width 16 \
        -indicatoron 0 -command "NeuroendoscopySetRuler; Render3D"} $Gui(WCA)
     eval {checkbutton $f.cSensor \
        -text "Show Probe" -variable Neuroendoscopy(probeVisibility) -width 16 \
        -indicatoron 0 -command "NeuroendoscopySetProbe; Render3D"} $Gui(WCA)

     
   
    

     pack  $f.cLocator $f.cRuler $f.cSensor -side top
     
     set f $fCamera.fBot.fProbe.fTop.fView
     eval {radiobutton $f.ffreeview -variable Neuroendoscopy(cam,probe) -text "Free view" -value "freeview" -command "NeuroendoscopySetFreeView"} $Gui(WBA)
     eval {radiobutton $f.fprobeview -variable Neuroendoscopy(cam,probe) -text "Sensor locked view" -value "probeview" -command "NeuroendoscopySetFreeView; LocatorRegisterCallback NeuroendoscopySetSensorView"} $Gui(WBA)
     eval {radiobutton $f.fsensorview -variable Neuroendoscopy(cam,probe) -text "Probe locked view" -value "sensorview" -command "NeuroendoscopySetFreeView; LocatorRegisterCallback NeuroendoscopySetProbeView"} $Gui(WBA)
     
        eval {label $f.lXCamPos -text "X Position of Camera"} $Gui(WLA)
        eval {entry $f.eNeedleOrientX -width 20 -textvariable Neuroendoscopy(needle,orientX)} $Gui(WEA)
        eval {label $f.lYCamPos -text "Y Position of Camera"} $Gui(WLA)
        eval {entry $f.eNeedleOrientY -width 20 -textvariable Neuroendoscopy(needle,orientY)} $Gui(WEA)
        eval {label $f.lZCamPos -text "Z Position of Camera"} $Gui(WLA)
        eval {entry $f.eNeedleOrientZ -width 20 -textvariable Neuroendoscopy(needle,orientZ)} $Gui(WEA)
        eval {label $f.lCamZoom -text "Zoom Factor"} $Gui(WLA)
    eval {entry $f.eDistance -width 20 -textvariable Neuroendoscopy(cam,zoom)} $Gui(WEA)
    
     eval {checkbutton $f.cCrosshair \
        -text "Show Crosshair" -variable Neuroendoscopy(crossVisibility) -width 16 \
        -indicatoron 0 -command "NeuroendoscopyCrossHairOnOff"} $Gui(WCA)
     eval {checkbutton $f.cDriving \
        -text "Driving" -variable Neuroendoscopy(driving) -width 16 \
        -indicatoron 0 -command "NeuroendoscopyDrivingOnOff"} $Gui(WCA)
    pack  $f.ffreeview $f.fprobeview $f.fsensorview $f.cCrosshair $f.cDriving  $f.lCamZoom $f.eDistance $f.lXCamPos $f.eNeedleOrientX $f.lYCamPos $f.eNeedleOrientY $f.lZCamPos $f.eNeedleOrientZ -side top



    set calcLength [expr $::Locator(guideSteps) * 2]
    set f $fCamera.fBot.fProbe.fTop.fRuler
    NeuroendoscopyCreateLabelAndSlider $f lguidelength 3 "ruler (mm)" "glength" horizontal 0 $calcLength 130 Neuroendoscopy(guide,length) "NeuroendoscopySetRulerLength;  Render3D" 5 1
   
    #-------------------------------------------
    # Camera->Color->Top frame 
    #-------------------------------------------
    set f $fCamera.fBot.fCapture
    frame $f.fTop -bg $Gui(activeWorkspace)

   
    pack $f.fTop -side top -pady $Gui(pad) -padx $Gui(pad) -fill both -expand yes


    set f $fCamera.fBot.fCapture.fTop

    frame $f.fVis     -bg $Gui(activeWorkspace)
    frame $f.fTitle   -bg $Gui(activeWorkspace)
    frame $f.fPos     -bg $Gui(activeWorkspace)
    pack $f.fVis $f.fTitle $f.fPos \
        -side top -padx $Gui(pad) -pady $Gui(pad)

    set f $fCamera.fBot.fCapture.fTop.fVis
    eval {checkbutton $f.cextWindow \
        -text "Open extra Window"  -width 16 \
        -indicatoron 0 -variable Neuroendoscopy(captWindow) -command "NeuroendoscopyMakeCaptureWindow;"} $Gui(WCA)
    eval {checkbutton $f.cextWindow2 \
        -text "Open extra Window CXX"  -width 16 \
        -indicatoron 0 -command "NeuroendoscopyMakeCaptureWindow2;"} $Gui(WCA)
    eval {checkbutton $f.cgenSTL \
        -text "Generate a STL File"  -width 16 \
        -indicatoron 0 -command "NeuroendoscopyGenSTL;"} $Gui(WCA)

    button $f.cextSnapShot -text "SnapShot" -font fixed -bg white \
           -command "set Neuroendoscopy(SnapShot) 1" -width 8 

    
    pack  $f.cextWindow $f.cextWindow2 $f.cgenSTL $f.cextSnapShot -side top

    set f $fCamera.fBot.fCapture.fTop.fTitle
    
    eval {label $f.lTitle -text "Defined point pairs:"} $Gui(WTA)
    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    scrollbar $f.hs -orient horizontal -bg $Gui(activeWorkspace)
    set Neuroendoscopy(PointsVerScroll) $f.vs
    set Neuroendoscopy(PointsHonScroll) $f.hs
    listbox $f.lb \
        -height 5 -width 24 \
        -bg $Gui(activeWorkspace) \
        -xscrollcommand {$::Neuroendoscopy(PointsHonScroll) set} \
        -yscrollcommand {$::Neuroendoscopy(PointsVerScroll) set}
    set Neuroendoscopy(pointPairListBox) $f.lb
    $Neuroendoscopy(PointsHonScroll) configure -command {$Neuroendoscopy(pointPairListBox) xview}
    $Neuroendoscopy(PointsVerScroll) configure -command {$Neuroendoscopy(pointPairListBox) yview}

    blt::table $f \
        0,0 $f.lTitle -padx 10 -pady 7 \
        1,0 $Neuroendoscopy(pointPairListBox) -padx 2 -pady 1 -fill x \
        1,1 $Neuroendoscopy(PointsVerScroll) -fill y -padx 2 -pady 1 \
        2,0 $Neuroendoscopy(PointsHonScroll) -fill x -padx 2 -pady 1


    set f $fCamera.fBot.fCapture.fTop.fPos 
    set cs $f
    
    # now put in a checkbox

    if {[catch "package require iSlicer" errmsg] == 1} {
        puts "Ooops, can't use the ischeckbox"
        set cb [checkbox  $cs.cb]
    } else {
        set cb [iwidgets::ischeckbox $cs.cb -relief sunken -labeltext "Fiducials" -borderwidth 2 -labelmargin 10 -background $Gui(activeWorkspace) -labelfont {helvetica 8}]
        if {$::Module(verbose)} {
            puts "added checkbox $cb"
        }
    }


   # bind $cb <Control-ButtonRelease-1> "FiducialsSelectionFromScroll $frame.fmenu.mbActive $cb yes" 
   # bind $cb <ButtonRelease-1> "FiducialsSelectionFromScroll $frame.fmenu.mbActive $cb no" 


    #--------------------------------------------------
    # Texture Frame
    #--------------------------------------------------

    set f $fCamera.fBot.fTexture
    
    frame $f.fTop -bg $Gui(activeWorkspace)

    pack $f.fTop -pady $Gui(pad) -padx $Gui(pad) -side top  -expand yes -fill both
   
    #-------------------------------------------
    # Camera->Probe->Mid frame 
    #-------------------------------------------
    set f $fCamera.fBot.fTexture.fTop
    
    frame $f.fTextureActivation  -bg $Gui(activeWorkspace)
    frame $f.fxcoord -bg $Gui(activeWorkspace)
    frame $f.fycoord -bg $Gui(activeWorkspace)
    frame $f.fzcoord -bg $Gui(activeWorkspace)
    frame $f.frotate -bg $Gui(activeWorkspace)
    frame $f.fxpic -bg $Gui(activeWorkspace)
    frame $f.fypic -bg $Gui(activeWorkspace)
    frame $f.fzpic -bg $Gui(activeWorkspace)
    
    

    pack  $f.fTextureActivation $f.fxcoord $f.fycoord $f.fzcoord $f.frotate $f.fxpic $f.fypic $f.fzpic -side top 
    
    #-------------------------------------------------
    
    set f $fCamera.fBot.fTexture.fTop.fTextureActivation
    set m $Model(activeID)
    
     eval {label $f.lCurrModel -text "no Model selected" } $Gui(WLA) 
    eval {checkbutton $f.cTexture \
        -text "Add Texture" -variable Neuroendoscopy(addTexture) -width 16 \
        -indicatoron 0 -command "NeuroendoscopySetTexture; Render3D"} $Gui(WCA)

eval {checkbutton $f.cAllTexture \
        -text "Add ALL Texture" -variable Neuroendoscopy(showAllTextures) -width 16 \
        -indicatoron 0 -command "showAllTextures; Render3D"} $Gui(WCA)
    eval {checkbutton $f.cTextureVisible \
        -text "Show Texture" -variable Neuroendoscopy(textureVisibility) -width 16 \
        -indicatoron 0 -command "NeuroendoscopyRegisterTexture; Render3D"} $Gui(WCA)
    
    eval {checkbutton $f.cVideoTextureVisible \
        -text "Show Video Texture" -variable Neuroendoscopy(videoVisibility) -width 16 \
        -indicatoron 0 -command "NeuroendoscopyVideoTexture; Render3D"} $Gui(WCA)
    
   eval {checkbutton $f.cCalibration \
        -text "Calibrate" -variable Neuroendoscopy(calibrationStart) -width 16 \
        -indicatoron 0 -command "NeuroendoscopyCalibration /projects/igtdev/ruetz/slicer2-mrt/slicer2/myfile_undistort.ppm; Render3D"} $Gui(WCA)

   eval {checkbutton $f.cAddPlane \
        -text "Add Plane" -variable Neuroendoscopy(addPlane) -width 16 \
        -indicatoron 0 -command "NeuroendoscopyAddPlane; Render3D"} $Gui(WCA)

    #$Model($head,name)
    pack $f.cTexture $f.cAllTexture $f.cTextureVisible $f.cVideoTextureVisible $f.cCalibration $f.lCurrModel $f.cAddPlane -side top
    
    set f $fCamera.fBot.fTexture.fTop.fxcoord
    NeuroendoscopyCreateLabelAndSlider $f lxcoord 0 "rotateX" "xcoord" horizontal -400 400 110 Neuroendoscopy(texture,xcoordi) "NeuroendoscopyTextureInits; Render3D" 5 90
    set f $fCamera.fBot.fTexture.fTop.fycoord
    NeuroendoscopyCreateLabelAndSlider $f ly 0 "rotateY" "ycoord" horizontal -400 400 110 Neuroendoscopy(texture,ycoordi) "NeuroendoscopyTextureInits; Render3D" 5 0
    set f $fCamera.fBot.fTexture.fTop.frotate
    NeuroendoscopyCreateLabelAndSlider $f lr 0 "rotateZ" "rotate" horizontal -400 400 110 Neuroendoscopy(texture,rotate) "NeuroendoscopyTextureInits; Render3D" 5 0
    set f $fCamera.fBot.fTexture.fTop.fzcoord
    NeuroendoscopyCreateLabelAndSlider $f langle 0 "angle" "angle" horizontal 0 90 110 Neuroendoscopy(texture,angle) "NeuroendoscopyTextureInits; Render3D" 5 25
    set f $fCamera.fBot.fTexture.fTop.fxpic
    NeuroendoscopyCreateLabelAndSlider $f lxpic 3 "xpic" "xpic" horizontal -1 1 130 Neuroendoscopy(texture,xpic) "NeuroendoscopyTextureInits; Render3D" 5 1
    set f $fCamera.fBot.fTexture.fTop.fypic
    NeuroendoscopyCreateLabelAndSlider $f lypic 3 "ypic" "ypic" horizontal -1 1 130 Neuroendoscopy(texture,ypic) "NeuroendoscopyTextureInits; Render3D" 5 1
    set f $fCamera.fBot.fTexture.fTop.fzpic
    NeuroendoscopyCreateLabelAndSlider $f lzpic 3 "zpic" "zpic" horizontal -1 1 130 Neuroendoscopy(texture,zpic) "NeuroendoscopyTextureInits; Render3D" 5 1

   #========================================================
   
   #set Neuroendoscopy(actors) "ruler sensormain senstortip rulerAxis"

   #-----------------------------------------
   # Actor Ruler
   #-----------------------------------------
   set actor ruler 
   MakeVTKObject Cylinder ${actor}
            ${actor}Source SetRadius 1.1 
            ${actor}Source SetHeight 10.
            eval [${actor}Actor GetProperty] SetColor 1 0 0
            ${actor}Actor SetPosition 0 [expr $Locator(normalLen) / -2. - 10 * (0)] 0
            
            ${actor}Actor SetUserMatrix Locator(normalMatrix)
            lappend Locator(actors) $actor
            #${actor}Actor SetVisibility 0



    #---------------------#
    # Ruler axis labels
    #---------------------#    
    set scale [expr $View(fov) * 0.05 ]
    set axis rulerAxis
    
        vtkVectorText ${axis}Text
            ${axis}Text SetText ""
        vtkPolyDataMapper  ${axis}Mapper
            ${axis}Mapper SetInput [${axis}Text GetOutput]
        vtkFollower ${axis}Actor
            ${axis}Actor SetMapper ${axis}Mapper
            ${axis}Actor SetScale  $scale $scale $scale 
            ${axis}Actor SetPickable 0
            if {$View(bgName)=="White"} {
                [${axis}Actor GetProperty] SetColor 1 1 0
            } else {
                [${axis}Actor GetProperty] SetColor 1 1 0
            }
        [${axis}Actor GetProperty] SetDiffuse 0.0
        [${axis}Actor GetProperty] SetAmbient 1.0
        [${axis}Actor GetProperty] SetSpecular 0.0
        # add only to the Main View window
        viewRen AddActor ${axis}Actor


    rulerAxisActor SetPosition 0 [expr $Locator(normalLen) / -2. ] 0

    rulerAxisActor SetCamera $View(viewCam)
    
lappend Locator(actors) $axis
#--------------------
#sensor tip
#--------------------

    set actor sensortip
    MakeVTKObject Sphere ${actor}
        ${actor}Source SetRadius [expr 1.0 * 2] 
        eval [${actor}Actor GetProperty] SetColor "1.0 1.0 0.0"
    ${actor}Actor SetPosition 0 \
                                  [expr $Locator(normalLen) / -2. - 45] \
                                  0
    ${actor}Actor SetUserMatrix Locator(tipMatrix)
     
    ${actor}Actor SetVisibility 0
#lappend Locator(actors) $actor
#--------------------
#sensor
#--------------------

    set actor sensormain 
    MakeVTKObject Cylinder $actor
        ${actor}Source SetRadius 1.1
        ${actor}Source SetHeight 10.
        # expr {$Locator(normalLen) / -2.0 + 3.64} is 
        # the tip location. Adding 5 mm moves the opening
        # to the right place.
        ${actor}Actor SetPosition 0 \
                                  [expr $Locator(normalLen) / -2. - 100] \
                                  0
        # 1.0 1.0 0.0 = yellow
        eval [${actor}Actor GetProperty] SetColor "1.0 1.0 0.0"
        ${actor}Actor SetUserMatrix Locator(normalMatrix)

     
        ${actor}Actor SetVisibility 0
}



#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyBuildGUIForICPRegistration
# Generates a Gui for ICP registration. Scrolling doesn't work well
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc NeuroendoscopyBuildGUIForICPRegistration {parent} {
    global Neuroendoscopy Gui Model

    set f $parent
    foreach x "1 2 3" {
        if {$x != 3} {
            frame $f.f$x -bg $Gui(activeWorkspace) -relief groove -bd 2 
        } else {
            frame $f.f$x -bg $Gui(activeWorkspace)
        }
        pack $f.f$x -side top -pady 1 -fill x 
    }

    set f $parent.f1
    eval {label $f.lTitle -text "Collect Points from Device:"} $Gui(WTA)
 
#    eval {label $f.lPatient -text "Select active Model:"} $Gui(WLA)
    eval {label $f.lPatient -text "Active Model: "} $Gui(BLA)
    eval {menubutton $f.ePatient -text "None" -relief raised -bd 2 -width 20 \
            -menu $f.ePatient.m } $Gui(WMBA)
    eval {menu $f.ePatient.m} $Gui(WMA)
    lappend Model(mbActiveList) $f.ePatient
    lappend Model(mActiveList)  $f.ePatient.m 

    #eval {entry $f.ePatient -width 20 -textvariable Locator(entry,patientCoords)} $Gui(WEA)
    #eval {label $f.lSlicer -text "Slicer space:"} $Gui(WLA)
    #eval {entry $f.eSlicer -width 20 -textvariable Locator(entry,slicerCoords)} $Gui(WEA)
    DevAddButton $f.bGet "Collect"  "Locator(OpenTracker,src) SetICPParams 1 1 0.00000001 150; LocatorRegisterCallback NeuroendoscopyLoopOpenTracker" 10 
    DevAddButton $f.bOK "Stop" "LocatorUnRegisterCallback NeuroendoscopyLoopOpenTracker" 10

    eval {label $f.lAmount -text "Collect Points from Device:" -textvariable Neuroendoscopy(IPCPointsAmount)} $Gui(WTA)


    blt::table $f \
        0,0 $f.lTitle -padx 1 -pady 7 -fill x -cspan 2 \
        1,0 $f.lPatient -padx 1 -pady 1 -anchor e \
        1,1 $f.ePatient -fill x -padx 1 -pady 1 -anchor w \
        2,0 $f.bGet  -fill x -padx 1 -pady 1 -anchor w \
        2,1 $f.bOK -padx 1 -pady 3 -anchor w \
        3,1 $f.lAmount -padx 1 -pady 7 -fill x -cspan 2 



    set f $parent.f2
    foreach x "Up Down" {
        frame $f.f$x -bg $Gui(activeWorkspace) 
        pack $f.f$x -side top 
    }

    set f $parent.f2.fUp
    eval {label $f.lTitle -text "Defined point pairs:"} $Gui(WTA)
    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    scrollbar $f.hs -orient horizontal -bg $Gui(activeWorkspace)
    set Neuroendoscopy(PointsVerScroll) $f.vs
    set Neuroendoscopy(PointsHonScroll) $f.hs
    listbox $f.lb \
        -height 5 -width 24 \
        -bg $Gui(activeWorkspace) \
        -xscrollcommand {$::Neuroendoscopy(PointsHonScroll) set} \
        -yscrollcommand {$::Neuroendoscopy(PointsVerScroll) set}
    set Neuroendoscopy(ICPpointListBox) $f.lb
    $Neuroendoscopy(PointsHonScroll) configure -command {$Neuroendoscopy(ICPpointListBox) xview}
    $Neuroendoscopy(PointsVerScroll) configure -command {$Neuroendoscopy(ICPpointListBox) yview}

    blt::table $f \
        0,0 $f.lTitle -padx 10 -pady 7 \
        1,0 $Neuroendoscopy(ICPpointListBox) -padx 2 -pady 1 -fill x \
        1,1 $Neuroendoscopy(PointsVerScroll) -fill y -padx 2 -pady 1 \
        2,0 $Neuroendoscopy(PointsHonScroll) -fill x -padx 2 -pady 1


    set f $parent.f2.fDown
    DevAddButton $f.bLoad "Load" "NeuroendoscopyLoadICPPoints" 8 
    DevAddButton $f.bSave "Save" "NeuroendoscopySaveICPPoints" 8 
    DevAddButton $f.bDelete "Delete" "NeuroendoscopyDeleteICPPoint" 8 
    grid $f.bLoad $f.bSave $f.bDelete -padx 1 -pady 2

   # DevAddButton $f.bCalib "(Re) - Start coarse calibration" "NeuroendoscopyStartICP"  30
   # DevAddButton $f.bShowRes "Show me the result!" "NeuroendoscopyShowICP" 30
    set f $parent.f3
    DevAddButton $f.bReg "Register" "NeuroendoscopyICPRegister" 8 
    DevAddButton $f.bResult "Result" "NeuroendoscopyShowICP" 8
    DevAddButton $f.bReset "Reset" "LocatorResetRegistration" 8 

    grid $f.bReg $f.bResult $f.bReset -padx 1 -pady 5 

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyLoadICPPoints
# Load ICP points from file
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyLoadICPPoints {} {
    global Neuroendoscopy

    # read data from file
    set fileType {{"Text" *.txt}}
    set fileName [tk_getOpenFile -filetypes $fileType -parent .]

    # if user just wanted to cancel
    if {[string length $fileName] <= 0} {
        return
    }
    
    set fd [open $fileName r]
    set data [read $fd]
    set lines [split $data "\n"]
    foreach line $lines {
        set line [string trim $line]
        eval $line
    }
    close $fd
   set size [llength $Neuroendoscopy(ICPpointList)]
    set Neuroendoscopy(IPCPointsAmount) "Points $size"
}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySaveICPPoints
# Save ICP points to file
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySaveICPPoints {} {
    global Neuroendoscopy

    set dir [pwd]
    set fileName [file join $dir "opentracker_icp_points.txt"]

    set fd [open $fileName w]
    puts $fd "\n\n\n\n\n"
 
    set comment "# This text file saves the user input. Do not edit it.\n"
    puts $fd $comment

    set comment "# the point pair list"
    puts $fd $comment
    set str "set Neuroendoscopy(ICPpointList) \"\"\n"
    puts $fd $str
    set str "\$Neuroendoscopy(ICPpointListBox) delete 0 end\n"
    puts $fd $str

    foreach x $Neuroendoscopy(ICPpointList) {
        set str "lappend Neuroendoscopy(ICPpointList) \{$x\}\n"
        puts $fd $str
        set str "\$Neuroendoscopy(ICPpointListBox) insert end \{$x\}\n" 
        puts $fd $str
    }

    close $fd
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyDeleteICPPoint
# Delete Certain points from list
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyDeleteICPPoint {} {
    global Neuroendoscopy

    set curs [$Neuroendoscopy(ICPpointListBox) curselection]
    if {$curs >= 0} {
        $Neuroendoscopy(ICPpointListBox) delete $curs
        set size [llength $Neuroendoscopy(ICPpointList)]
        set Neuroendoscopy(ICPpointList) \
            [lreplace $Neuroendoscopy(ICPpointList) $curs $curs]
        set size [llength $Neuroendoscopy(ICPpointList)]
        set Neuroendoscopy(IPCPointsAmount) "Points $size"
    } else {
        DevErrorWindow "Select a ICP Point to delete."
    }

}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyAddICPPoint
# Add points to the gui list
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc NeuroendoscopyAddICPPoint { {psc ""} } {
    global Neuroendoscopy


    # Keep the new point in the point list
    lappend Neuroendoscopy(ICPpointList) "$psc"

    # Put the point pair list into the list box
    $Neuroendoscopy(ICPpointListBox) delete 0 end
    foreach x $Neuroendoscopy(ICPpointList) {
        $Neuroendoscopy(ICPpointListBox) insert end $x 
    }
    set size [llength $Neuroendoscopy(ICPpointList)]
    set Neuroendoscopy(IPCPointsAmount) "Points $size"
}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyICPRegister
# Start ICP Registration
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyICPRegister {} {
    global Neuroendoscopy

    set size [llength $Neuroendoscopy(ICPpointList)]
    if {$size < 50} {
        DevErrorWindow "At least 50 points are needed for registration."
        return
    }

    set size [llength $Neuroendoscopy(ICPpointList)]
    Locator(OpenTracker,src) SetNumberOfPoints $size 


    set id 0 
    foreach x $Neuroendoscopy(ICPpointList) {
        set c [string trim $x]
        set i [string first ")" $c]
        if {$i < 0} {
            DevErrorWindow "Wrong point pair found: $c."
            return
        }

        set start 1
        set done [expr $i - 1]
        set pc [string range $c $start $done] 

       

       Locator(OpenTracker,src) AddSourceICPPoint [lindex $pc 0] [lindex $pc 1] [lindex $pc 2]

        puts "$pc "

        incr id
    }


 NeuroendoscopyStartICP


#    set error [Locator(OpenTracker,src) DoRegistration] 
#    if {$error} {
#        DevErrorWindow "Error registration between TargetLandmarks and SourceLandmarks."
#        return
#    }

#    set Locator(lmtMatrix) [Locator(OpenTracker,src) GetLandmarkTransformMatrix] 
#    puts [$Locator(lmtMatrix) Print]

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyPauseLocator
# Pause the locator
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyPauseLocator {} {
global Locator Neuroendoscopy

set Locator(loop) $Neuroendoscopy(Pause)
LocatorLoopOpenTracker
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyDrivingOnOff
# Starts or stops driving of the locator
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyDrivingOnOff {} {
global Slice Anno Neuroendoscopy
if {$Neuroendoscopy(driving) == 1} {

    LocatorSetDriverAll Locator
} else {
    LocatorSetDriverAll User
}
RenderAll
}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCrossHairOnOff
# turns the cross hair on off
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCrossHairOnOff {} {
global Slice Anno Neuroendoscopy
if {$Neuroendoscopy(crossVisibility) == 1} {
MainAnnoSetCrossVisibility  slices 1
MainAnnoSetCrossVisibility mag 1    

 foreach s $Slice(idList) {
        if {$Anno(outline) == 1} {
            Slice($s,outlineActor) SetVisibility $Slice($s,visibility)
        } else {
            Slice($s,outlineActor) SetVisibility 0
        } 
        foreach name "$Anno(orientList)" {
            Anno($s,$name,actor) SetVisibility 0
        }
    }
} else {
 MainAnnoSetCrossVisibility  slices 0
 MainAnnoSetCrossVisibility mag 0    
 

}
RenderAll
}
#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyPointSelection
# add a Point from Navitrack and in Dicom Viewer
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc NeuroendoscopyAddImageCoord { {filename ""} } {
    global Neuroendoscopy

    set pc "$filename"
    set pc [string trim $pc]
    regsub -all {( )+} $pc "   " pc 

    set sc Locator(tipMatrix)
    set sc [string trim $sc]
    regsub -all {( )+} $sc "   " sc 


    set psc "($pc)($sc)"
    set index [lsearch -exact $Neuroendoscopy(pointPairList) $psc]
    if {$index != -1} { 
        DevErrorWindow "The point pair is already added in."
        return
    }

 

    # Keep the new point in the point list
    lappend Neuroendoscopy(pointPairList) $psc

    # Put the point pair list into the list box
    $Neuroendoscopy(pointPairListBox) delete 0 end
    foreach x $Neuroendoscopy(pointPairList) {
        $Neuroendoscopy(pointPairListBox) insert end $x 
    }
}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCrossHairOnOff
# Start the ICP Registration
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc NeuroendoscopyStartICP {} {
  global Neuroendoscopy Fiducials Model Locator


set head1 $Model(activeID) 

set HeadPoly $Model($head1,polyData)

#set size [llength $Locator(pointPairList)]
#    if {$size < 2} {
#        DevErrorWindow "At least 2 pairs of landmarks are needed for registration."
#        return
#    }

#    set size [llength $Locator(pointPairList)]
#    Locator(OpenTracker,src) SetNumberOfPoints $size 


#    set id 0 
 #foreach x $Locator(pointPairList) {
 #       set c [string trim $x]
 #       set i [string first ")" $c]
 #       if {$i < 0} {
 #           DevErrorWindow "Wrong point pair found: $c."
 #           return
 #       }

 #       set start 1
 #       set done [expr $i - 1]
 #       set pc [string range $c $start $done] 

 #       set start [expr $i + 2] 
 #       set sc [string range $c $start end-1] 


 #       Locator(OpenTracker,src) AddPoint $id [lindex $sc 0] [lindex $sc 1] [lindex $sc 2] \
 #                                             [lindex $pc 0] [lindex $pc 1] [lindex $pc 2]  

 #       incr id
 #   puts "put point nr $id"
 #   }

#Locator(OpenTracker,src) BuildSourceModel
Locator(OpenTracker,src) SetTargetModel $HeadPoly
Locator(OpenTracker,src) DoRegistrationICP

#for testing source points representation



}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyShowICP
#show the result of the icp
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc NeuroendoscopyShowICP {} {
 headmapper SetInput [Locator(OpenTracker,src) GetSourceModel]
 actor SetMapper headmapper
 eval [actor GetProperty] SetColor 1 0 0
 viewRen AddActor actor
 
#transformed representation
 headmapper2 SetInput [Locator(OpenTracker,src) GetSourceModel]
 actor2 SetMapper headmapper2
 actor2 SetUserMatrix [Locator(OpenTracker,src) GetLandmarkTransformMatrix]
 eval [actor2 GetProperty] SetColor 1 1 1
 viewRen AddActor actor2
 
 headmapper3 SetInput [Locator(OpenTracker,src) GetLandmarkSourceModel]
 actor3 SetMapper headmapper3
 eval [actor3 GetProperty] SetColor 0 1 0
 viewRen AddActor actor3

 Render3D


}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyMakeCaptureWindow2
# create a capture window, based on xlib
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyMakeCaptureWindow2 {} {
global Neuroendoscopy MultiVolumeReader

  vtkNeuroendoscopy cap
  cap openXWindow


}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyMakeCaptureWindow
# create a capture window based on tcl windows
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyMakeCaptureWindow {} {
    global Neuroendoscopy MultiVolumeReader

 if {$Neuroendoscopy(captWindow) == 1}  {

 #initialize the FrameGrabber
 vtkCaptureFrames Neuroendoscopy(capFrames)
 Neuroendoscopy(capFrames) Initialize 640 480


 vtkImageDataStreamer Neuroendoscopy(imageStreamer)
  Neuroendoscopy(imageStreamer) SetInput [Neuroendoscopy(capFrames) getNextFrame]
 #Neuroendoscopy(imageStreamer) SetInput [Neuroendoscopy(capFrames) startGrabbing]
 #imageStreamer SetInput [shiftScale GetOutput]
 Neuroendoscopy(imageStreamer) SetNumberOfStreamDivisions 20
 # make sure we get the correct translator.
 Neuroendoscopy(imageStreamer) Update



set XRAD             640
set YRAD             480

#
# Create firt Mandelbrot viewer
#


vtkImageViewer Neuroendoscopy(captViewer)
  Neuroendoscopy(captViewer) SetInput [Neuroendoscopy(imageStreamer) GetOutput]
  Neuroendoscopy(captViewer) SetColorWindow 256.0
  Neuroendoscopy(captViewer) SetColorLevel 127.5
  #[viewer GetActor2D] SetPosition 1 1

#
# Create second Mandelbrot viewer
#


vtkImageViewer Neuroendoscopy(captViewer2)
  Neuroendoscopy(captViewer2) SetInput [Neuroendoscopy(imageStreamer) GetOutput]
  Neuroendoscopy(captViewer2) SetColorWindow 256.0
  Neuroendoscopy(captViewer2) SetColorLevel 127.5
  #[viewer2 GetActor2D] SetPosition 1 1


#
# Create the GUI: two vtkTkImageViewer widgets and a quit/reset buttons
#
wm withdraw .
set top [toplevel .top] 
wm protocol .top WM_DELETE_WINDOW "deleteCapWindows;destroy .top"
wm title .top "Distortion Viewer"

set f1 [frame $top.f1] 

pack $f1 \
        -side bottom \
        -fill both -expand f

#
# Create the image viewer widget for set 1
#
set manFrame [frame $f1.man]
set julFrame [frame $f1.jul]

pack $manFrame $julFrame \
        -side left \
        -padx 3 -pady 3 \
        -fill both -expand f

set manView [vtkTkImageViewerWidget $manFrame.view \
        -iv Neuroendoscopy(captViewer) \
        -width [expr $XRAD] \
        -height [expr $YRAD]]

set manRange [label $manFrame.range \
        -text "Movie: ${XRAD}X${YRAD}"]

set quit [button $manFrame.quit  \
        -text "Quit" \
        -command "deleteCapWindows;destroy .top"]

#
# Create the image viewer widget for set 2
#
pack $manView $manRange $quit \
        -side top \
        -padx 2 -pady 2 \
        -fill both -expand f

set julView [vtkTkImageViewerWidget $julFrame.view \
        -iv Neuroendoscopy(captViewer2) \
        -width [expr $XRAD] \
        -height [expr $YRAD]]

set julRange [label $julFrame.range \
        -text "SnapShot"]

set reset [button $julFrame.reset  \
        -text "SnapShot" \
        -command NeuroendoscopyTakeSnapshot]

pack $julView $julRange $reset \
        -side top \
        -padx 2 -pady 2 \
        -fill both -expand f
#
# The equation label
#
set equation [label $top.equation \
        -text "video ===> picture"]

pack $equation \
        -side bottom \
        -fill x -expand f

set cSaveAsFile [checkbutton $top.cSaveAsFile -text "Save as File?" -variable Neuroendoscopy(capture,IsSaveAsFile) -indicatoron 1]
pack $cSaveAsFile \
        -side bottom \
        -fill x -expand f
#
# Create the default bindings
#
::vtk::bind_tk_imageviewer_widget $manView
::vtk::bind_tk_imageviewer_widget $julView

NeuroendoscopyCaptureLoop
#Neuroendoscopy(imageStreamer) Delete


}

#-------------------------------------------------------------------------------
# .PROC deleteCapWindows
# delete the capture window
# .ARGS
# .END
#-------------------------------------------------------------------------------

 proc deleteCapWindows {} {
  global Neuroendoscopy
  set Neuroendoscopy(captWindow) 0
 
  update
  after 500
 
  Neuroendoscopy(captViewer) Delete
  Neuroendoscopy(captViewer2) Delete

 }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCaptureLoop
# makes capture loop
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCaptureLoop {} {
global Neuroendoscopy
if {$Neuroendoscopy(captWindow) == 1} {


Neuroendoscopy(captViewer) SetInput [Neuroendoscopy(capFrames) getNextFrame]
#Neuroendoscopy(ImageActor) Update
  
Neuroendoscopy(captViewer) Modified
Neuroendoscopy(imageStreamer) UpdateInformation
Neuroendoscopy(imageStreamer) Update
Neuroendoscopy(captViewer) Render
#renWin Render
update

       after 50 NeuroendoscopyCaptureLoop

} else {
  

  
  Neuroendoscopy(capFrames) Delete
  Neuroendoscopy(imageStreamer) Delete
}

}  


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyTakeSnapshot
# takes a snapshot from the movie
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyTakeSnapshot {} {
global Neuroendoscopy



set pic [Neuroendoscopy(capFrames) getNextFrame]
Neuroendoscopy(captViewer2) SetInput $pic
Neuroendoscopy(captViewer2) Modified
Neuroendoscopy(captViewer2) Render
if {$Neuroendoscopy(capture,IsSaveAsFile) == 1} {
  incr Neuroendoscopy(ImageCounter) 
  vtkPNMWriter pnmwriter
  pnmwriter SetInput $pic
  #pnmwriter SetFileName "myfile$Neuroendoscopy(ImageCounter).ppm"
pnmwriter SetFileName "myfile1.ppm"
  pnmwriter Write
  puts "writing file myfile1.ppm"
  pnmwriter Delete
 # NeuroendoscopyAddImageCoord
}
} 
 
#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCloseCaptureWindow
# close capture window
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCloseCaptureWindow {} {
    global Neuroendoscopy

    #destroy $Neuroendoscopy(timeCourseGraph)
   # unset -nocomplain Neuroendoscopy(timeCourseGraph)
   # unset -nocomplain Neuroendoscopy(signalCurve)]
   # unset -nocomplain Neuroendoscopy(baselineCurve)]

    destroy $Neuroendoscopy(w)
    unset Neuroendoscopy(w)
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyGenSTL
# generate a stl from actual model
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyGenSTL {} {
global Neuroendoscopy Model

#vtkPolyDataReader input
#  input SetFileName "/projects/igtdev/ruetz/slicer2-mrt/slicer2/Modules/iSlicer/Data/brainImageSmooth.vtk"

set head3 $Model(activeID) 
#set HeadPoly $Model($head1,polyData)

#set head0 [lindex $Model(idList) 0]
#set head1 [lindex $Model(idList) 1]
#set head2 [lindex $Model(idList) 2]
#set head3 [lindex $Model(idList) 3]
#
# generate vectors
#vtkCleanPolyData clean
#  clean SetInput [input GetOutput]

#set headarray0 [$Model($head0,polyData) GetPointData] GetArray 0
#set headarray1 [$Model($head1,polyData) GetPointData] GetArray 0
#set headarray2 [$Model($head2,polyData) GetPointData] GetArray 0
#set headarray3 [$Model($head3,polyData) GetPointData] GetArray 0



#vtkMergeFilter merge
#merge AddField "f0" $Model($head0,polyData)
#merge AddField "f1" $Model($head1,polyData)
#merge AddField "f2" $Model($head2,polyData)
#merge AddField "f3" $Model($head3,polyData)
#merge SetGeometry $Model($head3,polyData)


vtkCleanPolyData clean
  clean SetInput $Model($head3,polyData) 

vtkWindowedSincPolyDataFilter smooth
  smooth SetInput [clean GetOutput]
  smooth SetNumberOfIterations 200
  smooth GenerateErrorVectorsOn
  smooth GenerateErrorScalarsOn
  smooth Update

vtkPolyDataMapper mapper
  mapper SetInput [smooth GetOutput]
  eval mapper SetScalarRange [[smooth GetOutput] GetScalarRange]

vtkActor brain
  brain SetMapper mapper


viewRen AddActor brain
Render3D

   vtkTriangleFilter triangles
   triangles SetInput [clean GetOutput]
   set dir [pwd]
   vtkSTLWriter stl
   stl SetInput $Model($head3,polyData) 
   stl SetFileName "${dir}/${Model(name)}.stl"
   stl Write
   #file delete -force "brain.stl"
   
   vtkSTLWriter stlBinary
   stlBinary SetInput [triangles GetOutput]
   stlBinary SetFileName "${dir}/${Model(name)}Binary.stl"
   stlBinary SetFileType 2
   stlBinary Write
   #file delete -force "brainBinary.stl"

   vtkSTLReader stlread 
   stlread SetFileName "${dir}/${Model(name)}Binary.stl" 

  vtkPolyDataMapper mapper2
  mapper2 SetInput [stlread GetOutput]
  

vtkActor brain2
  brain2 SetMapper mapper2
  brain2 SetPosition -30 -30 30


viewRen AddActor brain2
Render3D

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyStartICP2
# start a icp2 registration
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyStartICP2 {} {
  global Neuroendoscopy Fiducials Model

#set head1 [lindex $Model(idList) 3]
set head1 $Model(activeID) 

set HeadPoly $Model($head1,polyData)





#for {set i 0} {$i< 300} {incr i 1} {
#  set p [$HeadPoly GetPoint [expr {int(rand()*70000)}]]
#  set x [expr [lindex $p 0]+120]
#  set y [expr [lindex $p 1]+120]
#  set z [expr [lindex $p 2]+120]
  
#  puts "$x $y $z"
#  Neuroendoscopy(ControlPoints) InsertNextPoint $x $y $z
#}

set numofpoints [Neuroendoscopy(ControlPoints) GetNumberOfPoints]

#vtkPoints voxelPoints
#  voxelPoints SetNumberOfPoints 4
#  voxelPoints InsertPoint 0 0 0 0
#  voxelPoints InsertPoint 1 1 0 0
#  voxelPoints InsertPoint 2 0 1 0
#  voxelPoints InsertPoint 3 1 1 0




 
Neuroendoscopy(strips) InsertNextCell $numofpoints
for {set i 0} {$i< $numofpoints} {incr i 1} {
  Neuroendoscopy(strips) InsertCellPoint $i
}

  
  Neuroendoscopy(pointcloud) SetPoints Neuroendoscopy(ControlPoints)
  Neuroendoscopy(pointcloud) SetLines Neuroendoscopy(strips)

vtkPolyDataMapper headmapper
headmapper SetInput Neuroendoscopy(pointcloud)


vtkActor actor
actor SetMapper headmapper


viewRen AddActor actor



  Neuroendoscopy(ICPTransformation) SetSource Neuroendoscopy(pointcloud)
  Neuroendoscopy(ICPTransformation) SetTarget $HeadPoly
  Neuroendoscopy(ICPTransformation) StartByMatchingCentroidsOn
  
  set landmark [Neuroendoscopy(ICPTransformation) GetLandmarkTransform ]
  $landmark SetModeToRigidBody
  Neuroendoscopy(ICPTransformation) SetMeanDistanceModeToRMS 
  Neuroendoscopy(ICPTransformation) SetCheckMeanDistance 1
  Neuroendoscopy(ICPTransformation) SetMaximumMeanDistance 0.001
  Neuroendoscopy(ICPTransformation) SetMaximumNumberOfIterations 60
  Neuroendoscopy(ICPTransformation) SetMaximumNumberOfLandmarks $numofpoints
  Neuroendoscopy(ICPTransformation) Update
  Neuroendoscopy(transmatrix) DeepCopy [Neuroendoscopy(ICPTransformation) GetMatrix]
#Neuroendoscopy(transmatrix) DeepCopy [Neuroendoscopy(LandmarkTransformation) GetMatrix]
Neuroendoscopy(EndTransformation) SetInput Neuroendoscopy(ICPTransformation)


headmapper2 SetInput Neuroendoscopy(pointcloud)



actor2 SetMapper headmapper2
actor2 SetUserMatrix Neuroendoscopy(transmatrix)

viewRen AddActor actor2




set meanDis [Neuroendoscopy(transmatrix) GetElement 0 1]
  puts "ICP done $meanDis..."


}
#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyPointSelection
# add a Point from Navitrack and in Dicom Viewer
# .ARGS
# .END
#-------------------------------------------------------------------------------


proc NeuroendoscopyPointSelection {{fid ""}} {
 global Neuroendoscopy Fiducials



after 2000
  
  set fname [Fiducials($fid,node) GetName]
  set insidestring [string first "registration" $fname]

 # set fid $Fiducials($Fiducials(activeList),fid)

    # find out its position in the list
  set index [llength $Fiducials($fid,pointIdList)]
    
  #set pid [[MainMrmlInsertBeforeNode EndFiducials($fid,node) Point] GetID]

  if { [string first "registration" $fname] != -1 } {
   set pid [lindex $Fiducials($fid,pointIdList) [expr $index-1 ] ]
   
        set xyz [Point($pid,node) GetXYZ]
        set x1 [lindex $xyz 0]
        set y1 [lindex $xyz 0]
        set z1 [lindex $xyz 0]
        
        set Gui(progressText) "fiducial creation $xyz"
        puts "fiducial creation $xyz..."
        
        #set corresponding point
        
        
        set controlamount [Neuroendoscopy(ControlPoints) GetNumberOfPoints]
        set targetamount [Neuroendoscopy(TargetPoints) GetNumberOfPoints]
        puts "we have $controlamount Controlpoints..."
        puts "we have $targetamount Targetpoints..."
        #next point from the tracking device
        set test [expr $controlamount - 1 ]
        if {$targetamount ==  $test} {
          Neuroendoscopy(TargetPoints) InsertNextPoint $x1 $y1 $z1
          Neuroendoscopy(TargetPoints) Modified
          if {$Neuroendoscopy(registration,points) != $controlamount} {
            NeuroendoscopyLoopOpenTracker
          } else {
            puts "we all set..."
          }
         
          } else {
          
          if {$controlamount < $test} {
            NeuroendoscopyLoopOpenTracker
          } else {
             puts "create another point plz..."
          }

        }
      

    }

}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyGenerateTestCoordinateFile
# generate a navitrack readable file from the sourcemodel of collected points, random possible 
# by uncomment
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyGenerateTestCoordinateFile { {filename ""} } {
  global Neuroendoscopy Model

  
  set Neuroendoscopy(fhandle) [open $filename w]


#  set head1 [lindex $Model(idList) 3]
  set head1 $Model(activeID) 

  #set HeadPoly $Model($head1,polyData)
set HeadPoly [Locator(OpenTracker,src) GetSourceModel]
  set numofpoints [$HeadPoly GetNumberOfPoints]

 
  for {set i 0} {$i< $numofpoints} {incr i 1} {

    #set p [$HeadPoly GetPoint [expr {int(rand()*$numofpoints)}]]
    set p [$HeadPoly GetPoint $i]
    #set x [expr [lindex $p 0]+120]
    #set y [expr [lindex $p 1]+120]
    #set z [expr [lindex $p 2]+120]
    set x [lindex $p 0]
    set y [lindex $p 1]
    set z [lindex $p 2]
    
    set timestamp [clock seconds]
    set zero "000"
    set timestamp2 "$timestamp$zero"
    set timestamp3 [expr $timestamp2 + $i]
    set laststring [string length "--:$x;$y;$z-"]
    set wholestring [expr 114 + $laststring]
    puts $Neuroendoscopy(fhandle) "0 {$timestamp3.000000-4<$wholestring:unsigned_short.button#1=0,float.confidence#1=1,vector<float>.orientation#11=\[4:0;0;0;0\],vector<float>.position#$laststring=\[3:$x;$y;$z\]}" 
    
  }

  close $Neuroendoscopy(fhandle)
}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyNaviTrackConnect
# Connect Navi Track 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyNaviTrackConnect {} {
  global Neuroendoscopy Locator

        
               Neuroendoscopy(OpenTracker,src) SetMultiRate $Neuroendoscopy(OpenTracker,multiBy)
        Neuroendoscopy(OpenTracker,src) Init $Neuroendoscopy(OpenTracker,cfg)
        Locator(OpenTracker,src) SetMultiRate $Locator(OpenTracker,multiBy)
        Locator(OpenTracker,src) Init $Locator(OpenTracker,cfg)
        #best parameters!!!! 1 1 0.00000001 150
        Locator(OpenTracker,src) SetICPParams 1 1 0.00000001 150 


        set Neuroendoscopy(OpenTracker,started) 1
      #set Neuroendoscopy(fhandle) [open testset.pt w]
        set Neuroendoscopy(loop) 1
        #NeuroendoscopyLoopOpenTracker
       # NeuroendoscopyPointSelection


}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyLoopOpenTracker
# Opentracker loop to read coordinates and send to the gui list
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyLoopOpenTracker {} {
global Neuroendoscopy Locator Gui


#Neuroendoscopy(OpenTracker,src) PollRealtime
Locator(OpenTracker,src) PollRealtime

set buttonvar [Locator(OpenTracker,src) GetButton]

#set locMatrix [Neuroendoscopy(OpenTracker,src) GetLocatorMatrix]
set locMatrix [Locator(OpenTracker,src) GetLocatorMatrix]


    # Read matrix
 
    set locatortest1 [$locMatrix GetElement 0 0]
    set locatortest2 [$locMatrix GetElement 1 0] 
    set locatortest3 [$locMatrix GetElement 2 0]
    set locatortest4 [$locMatrix GetElement 3 0]
    set locatortest4 [$locMatrix GetElement 0 1]
    set locatortest5 [$locMatrix GetElement 1 1]
    set locatortest6 [$locMatrix GetElement 2 1]
    set locatortest7 [$locMatrix GetElement 3 1]
    set locatortest8 [$locMatrix GetElement 0 2]
    set locatortest9 [$locMatrix GetElement 1 2]
set Gui(progressText) "data from Opentracker $buttonvar $locatortest1 $locatortest2 $locatortest3"
if {($locatortest1 != $Neuroendoscopy(locator1old) || $locatortest2 != $Neuroendoscopy(locator2old) || $locatortest3 != $Neuroendoscopy(locator3old)) && ($locatortest1 != 1 && $locatortest2 != 0 && $locatortest3 != 0)} {
  set Gui(progressText) "data from Opentracker $buttonvar $locatortest1 $locatortest2 $locatortest3"
 # puts "Data from Opentracker $buttonvar $locatortest1 $locatortest2 $locatortest3 $locatortest4 $locatortest5 $locatortest6 $locatortest7 $locatortest8 $locatortest9..."
    
   #set Neuroendoscopy(fhandle) [open myproc w]
   #set head1 [lindex $Model(idList) 3]
   
   #puts $Neuroendoscopy(fhandle) "$locatortest1 $locatortest2 $locatortest3"
   #the real points from the tracking device 
   #puts $Neuroendoscopy(fhandle) "$locatortest1 $locatortest2 $locatortest3"
   NeuroendoscopyAddICPPoint "($locatortest1 $locatortest2 $locatortest3)"
  
   # Neuroendoscopy(ControlPoints) InsertNextPoint $locatortest1 $locatortest2 $locatortest3
   # Neuroendoscopy(ControlPoints) Modified
    
   # set amount [Neuroendoscopy(ControlPoints) GetNumberOfPoints]
   # puts "adding tracking point ($locatortest1 $locatortest2 $locatortest3) have now $amount of them"
    #lappend Neuroendoscopy(selectedPoint) $locatortest1 $locatortest2 $locatortest3
    #puts "after target point insertion..."
    #NeuroendoscopyPointSelection
    set Neuroendoscopy(tmp) $buttonvar
    set Neuroendoscopy(locator1old) $locatortest1
    set Neuroendoscopy(locator2old) $locatortest2
    set Neuroendoscopy(locator3old) $locatortest3
  }


#if {$Neuroendoscopy(loop) == 1} {
#   update
#   after $Neuroendoscopy(OpenTracker,msPoll) NeuroendoscopyLoopOpenTracker 
#}
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyStartTracking
# Start tracking
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyStartTracking {} {
global Neuroendoscopy Locator tipActor normalActor openingActor transverseActor

set orient [Neuroendoscopy(EndTransformation) GetOrientation]
set pos [Neuroendoscopy(EndTransformation) GetPosition]

  set xo [lindex $orient 0]
  set yo [lindex $orient 1]
  set zo [lindex $orient 2]

  set xp [lindex $pos 0]
  set yp [lindex $pos 1]
  set zp [lindex $pos 2]

#vtkTransform locator_transform
#    locator_transform PostMultiply

#locator_transform Identity
    # C:
#    locator_transform Translate 0 [expr $Locator(normalLen) / 2.0] 0
    # R:
    #Neuroendoscopy(EndTransformation) PostMultiply
    #Neuroendoscopy(EndTransformation) Concatenate Locator(normalMatrix)
    # T:
    #locator_transform Translate [expr $xp] [expr $yp] [expr $zp]
    #locator_transform GetMatrix Locator(normalMatrix)
    Locator(normalMatrix) Modified

  foreach act $Locator(actors) {
  set orig [Neuroendoscopy(EndTransformation) GetOrientation]
  puts "$orig"
set testmatrix  [${act}Actor GetMatrix]
#  set xorig [expr [lindex $orig 0]+$xp]
#  set yorig [expr [lindex $orig 1]+$yp]
#  set zorig [expr [lindex $orig 2]+$zp]




  set xorig [expr (-1)*$xp]
  set yorig [expr (-1)*$yp]
  set zorig [expr (-1)*$zp]
  #${act}Actor AddPosition $xp $yp $zp
  #${act}Actor SetOrigin $xorig $yorig $zorig
 #${act}Actor AddOrientation $xo $yo $zo
  set orig [${act}Actor GetCenter]
  puts "$orig"
 #${act}Actor SetUserMatrix Neuroendoscopy(transmatrix)
#${act}Actor AddPosition $locatortest1 $locatortest2 $locatortest3
}


}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyNaviTrackStop
# stop navitrack loop
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyNaviTrackStop {} {
global Neuroendoscopy
  
  set Neuroendoscopy(loop) 0
  

}

#-------------------------------------------------------------------------------
# .PROC ModelsSetPropertyType
# Raise the frame with the Model(propertyType).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetFreeView {} {
global Neuroendoscopy Locator
  # Neuroendoscopy(activeCam)
   #vtkMatrix4x4 tempMatrix
   #tempMatrix DeepCopy Locator(normalMatrix)
   LocatorUnRegisterCallback NeuroendoscopySetProbeView
   LocatorUnRegisterCallback NeuroendoscopySetSensorView
   Neuroendoscopy(gyro,actor) SetUserMatrix Neuroendoscopy(texture,identitymatrix)
   #NeuroendoscopyResetCameraPosition
   #Render3D
   #tempMatrix Delete
   NeuroendoscopyUpdateActorFromVirtualEndoscope $Neuroendoscopy(activeCam)
   
       



}
#-------------------------------------------------------------------------------
# .PROC ModelsSetPropertyType
# Raise the frame with the Model(propertyType).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetSensorView {} {
global Neuroendoscopy Locator View tip tipActor
    
    set collision 0
    #Locator(lmtMatrix)
    # get the View plane of the virtual camera because we want to move 
    # in and out along that plane
    set l [$Neuroendoscopy(activeCam) GetViewPlaneNormal]
    set IO(x) [expr -[lindex $l 0]]
    set IO(y) [expr -[lindex $l 1]] 
    set IO(z) [expr -[lindex $l 2]]
    
    
    # get the View up of the virtual camera because we want to move up
    # and down along that plane (and reverse it)
    set l [$Neuroendoscopy(activeCam) GetViewUp]
    set Up(x) [lindex $l 0]
    set Up(y) [lindex $l 1]
    set Up(z) [lindex $l 2]
    
    
    # cross Up and IO to get the vector LR (to slide left and right)
    # LR = Up x IO

    #Cross LR Up IO 
    Cross LR IO Up 
    Normalize LR
    

    set coordinates [tipActor GetPosition]


#    vtkTransform CameraTransform
#    CameraTransform Identity
#    CameraTransform Concatenate Locator(normalMatrix)
#    CameraTransform Translate  0 [expr $Locator(normalLen) / -2.] 30  
#    CameraTransform RotateZ 180 
#    CameraTransform Inverse
#    CameraTransform Modified

#Neuroendoscopy(gyro,actor) SetUserTransform CameraTransform
    Neuroendoscopy(gyro,actor) SetUserMatrix Locator(normalMatrix)
    Neuroendoscopy(gyro,actor) SetOrientation 180 180 0
    Neuroendoscopy(gyro,actor) SetPosition [expr [lindex $coordinates 0] + $Neuroendoscopy(needle,orientY)] [expr $Neuroendoscopy(needle,orientZ) + [lindex $coordinates 1] + [expr $Locator(normalLen) / -2. - 20 * ($Neuroendoscopy(guide,length)/2) + 70]] [expr [lindex $coordinates 2] + $Neuroendoscopy(needle,orientX)]

    NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam)

    #*******************************************************************
    #
    # STEP 3: if the user decided to have the camera drive the slice, 
    #         then do it!
    #
    #*******************************************************************
    NeuroendoscopyCheckDriver $Neuroendoscopy(activeCam)
    
#    CameraTransform Delete
   # Render3D
#EvDeactivateBindingSet bindFlatWindowEvents
    update

   # Render3D
    
   
}




#-------------------------------------------------------------------------------
# .PROC ModelsSetPropertyType
# Raise the frame with the Model(propertyType).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetProbeView {} {
global Neuroendoscopy Locator
   global Neuroendoscopy Locator View
    
    set collision 0
    
    # get the View plane of the virtual camera because we want to move 
    # in and out along that plane
    set l [$Neuroendoscopy(activeCam) GetViewPlaneNormal]
    set IO(x) [expr -[lindex $l 0]]
    set IO(y) [expr -[lindex $l 1]] 
    set IO(z) [expr -[lindex $l 2]]
    
    
    # get the View up of the virtual camera because we want to move up
    # and down along that plane (and reverse it)
    set l [$Neuroendoscopy(activeCam) GetViewUp]
    set Up(x) [lindex $l 0]
    set Up(y) [lindex $l 1]
    set Up(z) [lindex $l 2]
    
    
    # cross Up and IO to get the vector LR (to slide left and right)
    # LR = Up x IO

    #Cross LR Up IO 
    Cross LR IO Up 
    Normalize LR

    
    # if we want to go along the camera's own axis (Relative mode)
    # Neuroendoscopy(cam,XXStr) is set by the slider
    # Neuroendoscopy(cam,XXStr,old) is saved at the end of this proc
    
    # for the next time 
   
        set coordinates [sensormainActor GetPosition]
        #set Neuroendoscopy(cam,x) [lindex $coordinates 0]
        #set Neuroendoscopy(cam,y) [lindex $coordinates 1]
        #set Neuroendoscopy(cam,z) [lindex $coordinates 2]

#    vtkTransform CameraTransform
#    CameraTransform Identity
#    CameraTransform Concatenate Locator(normalMatrix)
#    CameraTransform Translate  0 [expr $Locator(normalLen) / -2.] 30  
#    CameraTransform Inverse
#    CameraTransform Modified

#Neuroendoscopy(gyro,actor) SetUserTransform CameraTransform

    # set position of actor gyro (that will in turn set the position
    # of the camera and fp actor since their user matrix is linked to
    # the matrix of the gyro
    #Neuroendoscopy(gyro,actor) RotateWXYZ 180 1 0 0
    Neuroendoscopy(gyro,actor) SetUserMatrix Locator(normalMatrix)
    Neuroendoscopy(gyro,actor) SetOrientation 0 0 0
    Neuroendoscopy(gyro,actor) SetPosition [expr [lindex $coordinates 0] + $Neuroendoscopy(needle,orientY)] [expr $Neuroendoscopy(needle,orientZ) + [lindex $coordinates 1] + [expr $Locator(normalLen) / -2. - 20 * ($Neuroendoscopy(guide,length)/2) + 70]] [expr [lindex $coordinates 2] + $Neuroendoscopy(needle,orientX)]
    

#CameraTransform Delete
    #################################
    # should not be needed anymore
    #Neuroendoscopy(cam,actor) SetPosition $Neuroendoscopy(cam,x) $Neuroendoscopy(cam,y) $Neuroendoscopy(cam,z)
    #Neuroendoscopy(fp,actor) SetPosition $Neuroendoscopy(fp,x) $Neuroendoscopy(fp,y) $Neuroendoscopy(fp,z)
    ##################################
    # set position of virtual camera
    NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam)

    #*******************************************************************
    #
    # STEP 3: if the user decided to have the camera drive the slice, 
    #         then do it!
    #
    #*******************************************************************
    NeuroendoscopyCheckDriver $Neuroendoscopy(activeCam)

    update
    #Render3D
  
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetGuide
# Raise the frame with the Model(propertyType).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyGetCalcLockedOrientation {} {
    global Neuroendoscopy Locator
   # set Locator(guideVisibility) $Neuroendoscopy(rulerVisibility)

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetGuide
# Raise the frame with the Model(propertyType).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetRuler {} {
    global Neuroendoscopy Locator
   # set Locator(guideVisibility) $Neuroendoscopy(rulerVisibility)

    if {$Neuroendoscopy(rulerVisibility) == 1 } {
      eval [rulerActor GetProperty] SetColor 1 0 0
      eval [rulerAxisActor GetProperty] SetColor 1 1 0
      rulerActor SetVisibility 1
      rulerAxisActor SetVisibility 1
      NeuroendoscopySetRulerLength
    } else {
       rulerActor SetVisibility 0
       rulerAxisActor SetVisibility 0
    }
   # raise $Model(fSTEP2)
    #raise $Model(f$Model(propertyType))
}


#-------------------------------------------------------------------------------
# .PROC ModelsSetPropertyType
# Raise the frame with the Model(propertyType).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetLocator {} {
global Neuroendoscopy Locator
   set Locator(visibility) $Neuroendoscopy(visibility)
}
#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetGuide
# Raise the frame with the Model(propertyType).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetProbe {} {
    global Neuroendoscopy Locator
   # set Locator(guideVisibility) $Neuroendoscopy(rulerVisibility)

    if {$Neuroendoscopy(probeVisibility) == 1 } {
      eval [sensortipActor GetProperty] SetColor 1 0 0
      eval [sensormainActor GetProperty] SetColor 1 1 0
      sensortipActor SetVisibility 1
      sensormainActor SetVisibility 1

    } else {
      sensortipActor SetVisibility 0
      sensormainActor SetVisibility 0
    }
   # raise $Model(fSTEP2)
    #raise $Model(f$Model(propertyType))
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetGuide
# Raise the frame with the Model(propertyType).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetRulerLength {} {
    global Locator Neuroendoscopy
    if {$Neuroendoscopy(rulerVisibility) == 1 } {
#        for {set i 0} {$i < $::Locator(guideSteps)} {incr i} {
#            guide${i}Actor SetVisibility 0
#        }
#        for {set i 0} {$i <= (($Neuroendoscopy(guide,length)/2)-1)} {incr i} {
#            guide${i}Actor SetVisibility 1
#        }

rulerActor SetPosition 0 [expr $Locator(normalLen) / -2. - 10 * ($Neuroendoscopy(guide,length)/2)] 0
rulerSource SetHeight [expr 10*$Neuroendoscopy(guide,length)]

rulerAxisText SetText "$Neuroendoscopy(guide,length) mm"
 rulerAxisActor SetPosition 0 [expr $Locator(normalLen) / -2. - 20 * ($Neuroendoscopy(guide,length)/2) + 70] 0
if {$Neuroendoscopy(cam,probe) == "sensorview"} {
 Neuroendoscopy(gyro,actor) SetPosition 0 [expr $Locator(normalLen) / -2. - 20 * ($Neuroendoscopy(guide,length)/2)] 0
} 




}
   
}
#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUseLocator
# Raise the frame with the Model(propertyType).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUseLocator {} {
    global Module Locator
    $Locator(visibility) invoke
   # raise $Model(fSTEP2)
    #raise $Model(f$Model(propertyType))
}

#-------------------------------------------------------------------------------
# .PROC ModelsSetPropertyType
# Raise the frame with the Model(propertyType).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelsSetPropertyType {} {
    global Model
   # raise $Model(fSTEP2)
    raise $Model(f$Model(propertyType))
}



#-------------------------------------------------------------------------------
# .PROC EndoscopyNEWCount
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyICP1 {} {
global Module  
set fRegistration $Module(Neuroendoscopy,fRegistration)
set f $fRegistration.fTop 

#$f.fRegistration.fTop.fTabbedFrameButtons.f.rSTEP2 configure -takefocus 1
#NeuroendoscopyExecuteRegistrationTab STEP2
#vtkNeuroendoscopy ver
# set v [ver p2t 2 2]
#$Neuroendoscopy(tabbedFrame2).fSTEP1.fData.fData-7.cWidtext configure -text $v
#ver Delete


$f.0.rICP2 invoke


}
#-------------------------------------------------------------------------------
# .PROC EndoscopyNEWCount
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyICP2 {} {
global Module  
set fRegistration $Module(Neuroendoscopy,fRegistration)
set f $fRegistration.fTop
 $f.0.rICP3 invoke
}
#-------------------------------------------------------------------------------
# .PROC EndoscopyNEWCount
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyICP3 {} {
global Module  
set fRegistration $Module(Neuroendoscopy,fRegistration)
set f $fRegistration.fTop
 $f.0.rICP4 invoke
}
#-------------------------------------------------------------------------------
# .PROC EndoscopyNEWCount
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyICP4 {} {
global Module 
set fRegistration $Module(Neuroendoscopy,fRegistration)
set f $fRegistration.fTop
 #$f.1.rSTEP5 invoke

}
#-------------------------------------------------------------------------------
# .PROC EndoscopyNEWCount
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyLM1 {} {
global Module  
set fRegistration $Module(Neuroendoscopy,fRegistration)
set f $fRegistration.fTop
 #$f.1.rSTEP6 invoke

}

#-------------------------------------------------------------------------------
# .PROC EndoscopyNEWCount
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyLM2 {} {
global Module 
set fRegistration $Module(Neuroendoscopy,fRegistration)
set f $fRegistration.fTop
 #$f.1.rSTEP7 invoke
}


proc NeuroendoscopyLM3 {} {
global Module 
set fRegistration $Module(Neuroendoscopy,fRegistration)
set f $fRegistration.fTop
 #$f.1.rSTEP8 invoke
}
proc NeuroendoscopyLM4 {} {
global Module  
set fRegistration $Module(Neuroendoscopy,fRegistration)
set f $fRegistration.fTop
 #$f.1.rSTEP8 invoke
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyShowFlyThroughPopUp
# Build, if necessary, and display the flythrough popup.
# .ARGS
# int x horizontal position of popup, defaults to 100
# int y vertical position of popup, defaults to 100
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyShowFlyThroughPopUp {{x 100} {y 100}} {
    global Gui Neuroendoscopy 
    
    # Recreate popup if user killed it
    if {[winfo exists $Gui(wNeuroendoscopy)] == 0} {
        NeuroendoscopyBuildFlyThroughGUI
    }
    ShowPopup $Gui(wNeuroendoscopy) $x $y
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyExecutePathTab 
# Executes command that is selected in Path Menu Selection
# .ARGS
# str command the command to execute
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyExecutePathTab {command} {
    global Neuroendoscopy
    raise $Neuroendoscopy(tabbedFrame).f$command
    focus $Neuroendoscopy(tabbedFrame).f$command
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyExecuteRegistrationTab 
# Executes command that is selected in Path Menu Selection
# .ARGS
# str command the command to execute
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyExecuteRegistrationTab {command} {
    global Neuroendoscopy
    raise $Neuroendoscopy(tabbedFrame2).f$command
    focus $Neuroendoscopy(tabbedFrame2).f$command
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyBuildFlyThroughGUI
# Build the fly through gui window, .cwEndoFlyThrough
# .ARGS
# .END
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateLabelAndSlider
# Create a label and slider on the given parent frame.
# 
# .ARGS
# framepath f 
# str labelName 
# int labelHeight 
# str labelText 
# str sliderName 
# str orient 
# int from 
# int to 
# int length 
# str variable 
# str commandString 
# int entryWidth 
# int defaultSliderValue
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreateLabelAndSlider {f labelName labelHeight labelText sliderName orient from to length variable commandString entryWidth defaultSliderValue} {

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
# .PROC NeuroendoscopyCreateCheckButton
# Create a check button.
# 
# .ARGS
# str ButtonName 
# str VariableName 
# str Message 
# str Command 
# int Indicatoron optional, defaults to 0
# int Width optional, defaults to length of Message
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreateCheckButton {ButtonName VariableName Message Command {Indicatoron 0} {Width 0} } {
    global Gui
    if {$Width == 0 } {
        set Width [expr [string length $Message]]
    }
    eval  {checkbutton $ButtonName -variable $VariableName -text \
    $Message -width $Width -indicatoron $Indicatoron -command $Command } $Gui(WCA)
} 

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetVisibility
# Set the visibility of neuroendoscopy actors.
# .ARGS
# str a either gyro or the actor id
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetVisibility {a} {

    global Neuroendoscopy

    if {$a == "gyro"} {
        set Neuroendoscopy(gyro,use) $Neuroendoscopy($a,visibility)
        NeuroendoscopyUseGyro 
    } else {
        Neuroendoscopy($a,actor) SetVisibility $Neuroendoscopy($a,visibility)
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateAdvancedGUI
# Create the advanced gui tab.
# .ARGS
# windowpath f where to create elements
# int a endosopic id
# str vis if visible add a visibility button, optional, defaults to empty string
# str col color, optional, defaults to empty string
# str size optional, defaults to empty string
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreateAdvancedGUI {f a {shape ""} {vis ""} {col ""} {size ""}} {
    global Gui Neuroendoscopy Color Advanced
if {$a == "cam"} {
       if {$shape == "shape"} {
         #eval {label $f.m$a.lW -text "Width:"} $Gui(WLA)
        
         eval {menubutton $f.men$a -text "Shapes" -menu $f.men$a.fMenu} $Gui(WMBA)
         TooltipAdd $f.men$a "Shapes"
         eval {menu $f.men$a.fMenu} $Gui(WMA)
         foreach sshape $Neuroendoscopy($a,shape) {
           $f.men$a.fMenu add command  -label $sshape -command "NeuroendoscopyCameraParams -1 $sshape; Render3D"
         } 
       } else {
      eval {label $f.men$a -text $Neuroendoscopy($a,shape)} $Gui(WLA)
       }
    grid $f.men$a -padx $Gui(pad)
}
    # Name / Visible only if vis is not empty
    if {$vis == "visible"} {
        eval {checkbutton $f.c$a \
            -text $Neuroendoscopy($a,name) -wraplength 65 -variable Neuroendoscopy($a,visibility) \
            -width 11 -indicatoron 0\
            -command "NeuroendoscopySetVisibility $a; Render3D"} $Gui(WCA)
    } else {
        eval {label $f.c$a -text $Neuroendoscopy($a,name)} $Gui(WLA)
    }

    # Change colors
    if {$col == "color"} {
        eval {button $f.b$a \
            -width 1 -command "NeuroendoscopySetActive $a $f.b$a; ShowColors NeuroendoscopyPopupCallback; Render3D" \
            -background [MakeColorNormalized $Neuroendoscopy($a,color)]}
    } else {
        eval {label $f.b$a -text " " } $Gui(WLA)
    }
    
    if {$size == "size"} {

       
        eval {entry $f.e$a -textvariable Neuroendoscopy($a,size) -width 3} $Gui(WEA)
        bind $f.e$a  <Return> "NeuroendoscopySetSize $a"
        eval {scale $f.s$a -from 0.0 -to $Neuroendoscopy($a,size) -length 70 \
            -variable Neuroendoscopy($a,size) \
            -command "NeuroendoscopySetSize $a; Render3D" \
            -resolution 0.01} $Gui(WSA) {-sliderlength 10 }    
        grid $f.c$a $f.b$a $f.e$a $f.s$a -pady 0 -padx 0        
    } else {
        grid $f.c$a $f.b$a -pady 0 -padx 0        
    }

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetActive
# Set the active actor and button.
# .ARGS
# int a actor id
# int b button id
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetActive {a b} {

    global Advanced

    set Advanced(ActiveActor) $a
    set Advanced(ActiveButton) $b 
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyPopupCallback
# Get the active actor and set it's properties.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyPopupCallback {} {
    global Label Neuroendoscopy Color Gui Advanced

    set name $Label(name) 

    set a $Advanced(ActiveActor)

    # Use second color by default
    set color [lindex $Color(idList) 1]

    foreach c $Color(idList) {
        if {[Color($c,node) GetName] == $name} {
            set color $c
        }
    }

    [Neuroendoscopy($a,actor) GetProperty] SetAmbient    [Color($color,node) GetAmbient]
    [Neuroendoscopy($a,actor) GetProperty] SetDiffuse    [Color($color,node) GetDiffuse]
    [Neuroendoscopy($a,actor) GetProperty] SetSpecular   [Color($color,node) GetSpecular]
    [Neuroendoscopy($a,actor) GetProperty] SetSpecularPower [Color($color,node) GetPower]
    eval [Neuroendoscopy($a,actor) GetProperty] SetColor    [Color($color,node) GetDiffuseColor]

    set Neuroendoscopy($a,color) [Color($color,node) GetDiffuseColor]

    # change the color of the button on the Advanced GUI
    $Advanced(ActiveButton) configure -background [MakeColorNormalized [[Neuroendoscopy($a,actor) GetProperty] GetColor]]

    if {$Advanced(ActiveActor) == "Box"} {
        
        [Neuroendoscopy(Box2,actor) GetProperty] SetAmbient    [Color($color,node) GetAmbient]
        [Neuroendoscopy(Box2,actor) GetProperty] SetDiffuse    [Color($color,node) GetDiffuse]
        [Neuroendoscopy(Box2,actor) GetProperty] SetSpecular   [Color($color,node) GetSpecular]
        [Neuroendoscopy(Box2,actor) GetProperty] SetSpecularPower [Color($color,node) GetPower]
        eval [Neuroendoscopy(Box2,actor) GetProperty] SetColor    [Color($color,node) GetDiffuseColor]

    }
    Render3D
}

##############################################################################
#
#        PART 3: Selection of actors through key/mouse
#
#############################################################################

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUseGyro
# If using it, set opacity to 1, otherwise 0, and rerender
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUseGyro {} {
    global Neuroendoscopy
    
    if { $Neuroendoscopy(gyro,use) == 1} {
        foreach XX "X Y Z" {
            [Neuroendoscopy(gyro,${XX}actor) GetProperty] SetOpacity 1
        }
        NeuroendoscopySetPickable Neuroendoscopy(gyro,actor) 1
    } else {
        foreach XX "X Y Z" {
            [Neuroendoscopy(gyro,${XX}actor) GetProperty] SetOpacity 0
        }
        NeuroendoscopySetPickable Neuroendoscopy(gyro,actor) 0
    }
    Render3D
}


#############################################################################
#
#      PART 4 : Endoscope movement
#
#############################################################################

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyGyroMotion
# Get the locatin and orientatin of the gyro and set sliders
# .ARGS
# str actor name of actor
# float angle 
# float dotprod 
# float unitX 
# float unitY 
# float unitZ
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyGyroMotion {actor angle dotprod unitX unitY unitZ} {
    
    global Neuroendoscopy
    if {$actor == "Neuroendoscopy(gyro,actor)"} {
        # get the position of the gyro and set the sliders
        set cam_mat [Neuroendoscopy(cam,actor) GetMatrix]   
        set Neuroendoscopy(cam,xStr,old) [$cam_mat GetElement 0 3] 
        set Neuroendoscopy(cam,yStr,old) [$cam_mat GetElement 1 3] 
        set Neuroendoscopy(cam,zStr,old) [$cam_mat GetElement 2 3] 
        set Neuroendoscopy(cam,xStr) [$cam_mat GetElement 0 3] 
        set Neuroendoscopy(cam,yStr) [$cam_mat GetElement 1 3] 
        set Neuroendoscopy(cam,zStr) [$cam_mat GetElement 2 3] 

        # get the orientation of the gyro and set the sliders 
        set or [Neuroendoscopy(gyro,actor) GetOrientation]
        set Neuroendoscopy(cam,rxStr,old) [lindex $or 0]
        set Neuroendoscopy(cam,ryStr,old) [lindex $or 1]
        set Neuroendoscopy(cam,rzStr,old) [lindex $or 2]
        set Neuroendoscopy(cam,rxStr) [lindex $or 0]
        set Neuroendoscopy(cam,ryStr) [lindex $or 1]
        set Neuroendoscopy(cam,rzStr) [lindex $or 2]

        NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam)
        NeuroendoscopyCheckDriver $Neuroendoscopy(activeCam)
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetGyroOrientation
# Set the gyro orientation
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetGyroOrientation {} {
    global Neuroendoscopy
    if {$Neuroendoscopy(cam,axis) == "relative"} {
        vtkTransform tmp
        tmp SetMatrix [Neuroendoscopy(cam,actor) GetMatrix] 
        eval Neuroendoscopy(gyro,actor) SetOrientation [tmp GetOrientation]
        tmp Delete
        Neuroendoscopy(cam,actor) SetOrientation 0 0 0
    } elseif {$Neuroendoscopy(cam,axis) == "absolute"} {
        set or [Neuroendoscopy(gyro,actor) GetOrientation]
        Neuroendoscopy(gyro,actor) SetOrientation 0 0 0
        eval Neuroendoscopy(cam,actor) SetOrientation $or
    }       
}
    
#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetWorldPosition
# Set the position of the gyro actor, and sliders
# .ARGS
# float x new x position for the gyro actor
# float y new y position for the gyro actor
# float z new z position for the gyro actor
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetWorldPosition {x y z} {
    global Neuroendoscopy

    # reset the sliders
    set Neuroendoscopy(cam,xStr) $x
    set Neuroendoscopy(cam,yStr) $y
    set Neuroendoscopy(cam,zStr) $z
    set Neuroendoscopy(cam,xStr,old) $x
    set Neuroendoscopy(cam,yStr,old) $y
    set Neuroendoscopy(cam,zStr,old) $z
    Neuroendoscopy(gyro,actor) SetPosition $x $y $z
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetWorldOrientation
# Set the orientation of the gyro actor
# .ARGS
# float rx
# float ry
# float rz
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetWorldOrientation {rx ry rz} {
    global Neuroendoscopy


    # reset the sliders
    set Neuroendoscopy(cam,rxStr) $rx
    set Neuroendoscopy(cam,ryStr) $ry
    set Neuroendoscopy(cam,rzStr) $rz
    set Neuroendoscopy(cam,rxStr,old) $rx
    set Neuroendoscopy(cam,ryStr,old) $ry
    set Neuroendoscopy(cam,rzStr,old) $rz
    Neuroendoscopy(gyro,actor) SetOrientation $rx $ry $rz
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetCameraPosition
#  This is called when the position sliders are updated. We use the values
#  stored in the slider variables to update the position of the endoscope 
# .ARGS
# str value optional, defaults to empty string
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetCameraPosition {{value ""}} {
    global Neuroendoscopy View Neuroendoscopy
    
    set collision 0
    
    # get the View plane of the virtual camera because we want to move 
    # in and out along that plane
    set l [$Neuroendoscopy(activeCam) GetViewPlaneNormal]
    set IO(x) [expr -[lindex $l 0]]
    set IO(y) [expr -[lindex $l 1]] 
    set IO(z) [expr -[lindex $l 2]]
    
    
    # get the View up of the virtual camera because we want to move up
    # and down along that plane (and reverse it)
    set l [$Neuroendoscopy(activeCam) GetViewUp]
    set Up(x) [lindex $l 0]
    set Up(y) [lindex $l 1]
    set Up(z) [lindex $l 2]
    
    
    # cross Up and IO to get the vector LR (to slide left and right)
    # LR = Up x IO

    #Cross LR Up IO 
    Cross LR IO Up 
    Normalize LR

    
    # if we want to go along the camera's own axis (Relative mode)
    # Neuroendoscopy(cam,XXStr) is set by the slider
    # Neuroendoscopy(cam,XXStr,old) is saved at the end of this proc
    
    # for the next time 
    if { $Neuroendoscopy(cam,axis) == "relative" } {
        
        # this matrix tells us the current position of the cam actor
        set cam_mat [Neuroendoscopy(cam,actor) GetMatrix]   
        # stepXX is the amount to move along axis XX
        set stepX [expr $Neuroendoscopy(cam,xStr,old) - $Neuroendoscopy(cam,xStr)]
        set stepY [expr $Neuroendoscopy(cam,yStr,old) - $Neuroendoscopy(cam,yStr)]
        set stepZ [expr $Neuroendoscopy(cam,zStr,old) - $Neuroendoscopy(cam,zStr)]
        
        set Neuroendoscopy(cam,x) [expr [$cam_mat GetElement 0 3] + \
            $stepX*$LR(x) + $stepY * $IO(x) + $stepZ * $Up(x)] 
        set Neuroendoscopy(cam,y) [expr  [$cam_mat GetElement 1 3] + \
            $stepX * $LR(y) + $stepY * $IO(y) + $stepZ * $Up(y)] 
        set Neuroendoscopy(cam,z) [expr  [$cam_mat GetElement 2 3] + \
            $stepX * $LR(z) +  $stepY * $IO(z) +  $stepZ * $Up(z)] 
    
    } elseif { $Neuroendoscopy(cam,axis) == "absolute" } {
        set Neuroendoscopy(cam,x) $Neuroendoscopy(cam,xStr)
        set Neuroendoscopy(cam,y) $Neuroendoscopy(cam,yStr)
        set Neuroendoscopy(cam,z) $Neuroendoscopy(cam,zStr)
    }
    
    # store current slider
    set Neuroendoscopy(cam,xStr,old) $Neuroendoscopy(cam,xStr)
    set Neuroendoscopy(cam,yStr,old) $Neuroendoscopy(cam,yStr)
    set Neuroendoscopy(cam,zStr,old) $Neuroendoscopy(cam,zStr)

    # set position of actor gyro (that will in turn set the position
    # of the camera and fp actor since their user matrix is linked to
    # the matrix of the gyro
    Neuroendoscopy(gyro,actor) SetPosition $Neuroendoscopy(cam,x) $Neuroendoscopy(cam,y) $Neuroendoscopy(cam,z)
    
    #################################
    # should not be needed anymore
    #Neuroendoscopy(cam,actor) SetPosition $Neuroendoscopy(cam,x) $Neuroendoscopy(cam,y) $Neuroendoscopy(cam,z)
    #Neuroendoscopy(fp,actor) SetPosition $Neuroendoscopy(fp,x) $Neuroendoscopy(fp,y) $Neuroendoscopy(fp,z)
    ##################################
    # set position of virtual camera
    NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam)

    #*******************************************************************
    #
    # STEP 3: if the user decided to have the camera drive the slice, 
    #         then do it!
    #
    #*******************************************************************
    NeuroendoscopyCheckDriver $Neuroendoscopy(activeCam)

    Render3D
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyResetCameraPosition
# Reset the neuroendoscopy camera to the origin
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyResetCameraPosition {} {
    global Neuroendoscopy

    NeuroendoscopySetWorldPosition 0 0 0
    # in case the camera's model matrix is not the identity
    Neuroendoscopy(cam,actor) SetPosition 0 0 0
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetCameraDirection
# Set the neuroendoscopy camera direction.
# .ARGS
# str value one of rx, ry, rz, optional, defaults to empty string
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetCameraDirection {{value ""}} {
    global Neuroendoscopy View Model

    if {$Neuroendoscopy(cam,axis) == "absolute"} {
    
    #Neuroendoscopy(gyro,actor) SetOrientation $Neuroendoscopy(cam,rxStr) $Neuroendoscopy(cam,ryStr) $Neuroendoscopy(cam,rzStr) 
    if {$value == "rx"} {
        set temprx [expr $Neuroendoscopy(cam,rxStr) - $Neuroendoscopy(cam,rxStr,old)]
        set Neuroendoscopy(cam,rxStr,old) $Neuroendoscopy(cam,rxStr)
        Neuroendoscopy(gyro,actor) RotateWXYZ $temprx 1 0 0
    } elseif {$value == "ry"} {
        set tempry [expr $Neuroendoscopy(cam,ryStr) - $Neuroendoscopy(cam,ryStr,old)]
        set Neuroendoscopy(cam,ryStr,old) $Neuroendoscopy(cam,ryStr)
        Neuroendoscopy(gyro,actor) RotateWXYZ $tempry 0 1 0
        #$Neuroendoscopy(activeCam) Roll $tempry
    } elseif {$value == "rz"} {
        set temprz [expr $Neuroendoscopy(cam,rzStr) - $Neuroendoscopy(cam,rzStr,old)]
        set Neuroendoscopy(cam,rzStr,old) $Neuroendoscopy(cam,rzStr)
        Neuroendoscopy(gyro,actor) RotateWXYZ $temprz 0 0 1
    }
    #eval Neuroendoscopy(gyro,actor) SetOrientation [Neuroendoscopy(gyro,actor) GetOrientation]        
    
    } elseif {$Neuroendoscopy(cam,axis) == "relative"} {
        if {$value == "rx"} {
            set temprx [expr $Neuroendoscopy(cam,rxStr) - $Neuroendoscopy(cam,rxStr,old)]
            set Neuroendoscopy(cam,rxStr,old) $Neuroendoscopy(cam,rxStr)
            Neuroendoscopy(gyro,actor) RotateX $temprx
        } elseif {$value == "ry"} {
            set tempry [expr $Neuroendoscopy(cam,ryStr) - $Neuroendoscopy(cam,ryStr,old)]
            set Neuroendoscopy(cam,ryStr,old) $Neuroendoscopy(cam,ryStr)
            Neuroendoscopy(gyro,actor) RotateY $tempry
        } elseif {$value == "rz"} {
            set temprz [expr $Neuroendoscopy(cam,rzStr) - $Neuroendoscopy(cam,rzStr,old)]
            set Neuroendoscopy(cam,rzStr,old) $Neuroendoscopy(cam,rzStr)
            Neuroendoscopy(gyro,actor) RotateZ $temprz
        }
    }
    
    #*******************************************************************
    #
    # if the user decided to have the camera drive the slice, 
    #         then do it!
    #
    #*******************************************************************
    NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam)
    NeuroendoscopyCheckDriver $Neuroendoscopy(activeCam)  

   
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyResetCameraDirection
# Reset the endo camera to default rotation and view.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyResetCameraDirection {} {
    global Neuroendoscopy 

    # we reset the rotation around the absolute y axis
    set Neuroendoscopy(cam,yRotation) 0
    set Neuroendoscopy(cam,viewUpX) 0
    set Neuroendoscopy(cam,viewUpY) 0
    set Neuroendoscopy(cam,viewUpZ) 1

    NeuroendoscopySetWorldOrientation 0 0 0 
    # in case the camera's model matrix is not the identity
    Neuroendoscopy(cam,actor) SetOrientation 0 0 0
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUpdateActorFromVirtualEndoscope
#  First, set the gyro matrix's orientation based on the virtual camera's matrix.
# Then if the user decided to have the camera drive the slice, do so.
# .ARGS
# str vcam virtual endoscope name
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUpdateActorFromVirtualEndoscope {vcam} {
    global Neuroendoscopy View Path Model
        
    #*********************************************************************
    #
    # STEP 1: set the gyro matrix's orientation based on the virtual
    #         camera's matrix
    #         
    #*********************************************************************
    
    # this doesn't work because the virtual camera is rotated -90 degrees on 
    # the y axis originally, so the overall matrix is not correct (off by 90 degrees)
    #eval Neuroendoscopy(gyro,actor) SetOrientation [$Neuroendoscopy(activeCam) GetOrientation]
    #eval Neuroendoscopy(gyro,actor) SetPosition [$Neuroendoscopy(activeCam) GetPosition]

    # so build the matrix ourselves instead, that way we are sure about it
    vtkMatrix4x4 matrix
   
    set vu [$vcam GetViewUp]
    set vpn [$vcam GetViewPlaneNormal]

    # Uy = ViewPlaneNormal
    set Uy(x) [expr - [lindex $vpn 0]]
    set Uy(y) [expr - [lindex $vpn 1]]
    set Uy(z) [expr - [lindex $vpn 2]]
    # Uz = ViewUp
    set Uz(x) [lindex $vu 0]
    set Uz(y) [lindex $vu 1]
    set Uz(z) [lindex $vu 2]
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

    vtkTransform neuroendoscopyTransform
    neuroendoscopyTransform SetMatrix matrix
    set orientation [neuroendoscopyTransform GetOrientation]
    set position [$Neuroendoscopy(activeCam) GetPosition]
    eval Neuroendoscopy(gyro,actor) SetOrientation $orientation
    eval Neuroendoscopy(gyro,actor) SetPosition $position
    neuroendoscopyTransform Delete
    matrix Delete

    # set the sliders
    set Neuroendoscopy(cam,xStr) [lindex $position 0]
    set Neuroendoscopy(cam,yStr) [lindex $position 1]
    set Neuroendoscopy(cam,zStr) [lindex $position 2]
    set Neuroendoscopy(cam,xStr,old) [lindex $position 0]
    set Neuroendoscopy(cam,yStr,old) [lindex $position 1]
    set Neuroendoscopy(cam,zStr,old) [lindex $position 2]

    set Neuroendoscopy(cam,rxStr) [lindex $orientation 0]
    set Neuroendoscopy(cam,ryStr) [lindex $orientation 1]
    set Neuroendoscopy(cam,rzStr) [lindex $orientation 2]
    set Neuroendoscopy(cam,rxStr,old) [lindex $orientation 0]
    set Neuroendoscopy(cam,ryStr,old) [lindex $orientation 1]
    set Neuroendoscopy(cam,rzStr,old) [lindex $orientation 2]
    
    #*******************************************************************
    #
    # STEP 2: if the user decided to have the camera drive the slice, 
    #         then do it!
    #
    #*******************************************************************

    NeuroendoscopyCheckDriver $vcam

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUpdateVirtualEndoscope 
#       Updates the virtual camera's position, orientation and view angle.<br>
#       Calls NeuroendoscopyLightFollowsEndoCamera
# .ARGS
# str vcam name of the virtual endoscope
# str coordList optional, defaults to empty list
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUpdateVirtualEndoscope {vcam {coordList ""}} {
    global Neuroendoscopy Model View Path
    
    #puts "vcam $vcam"
    
    if {$coordList != "" && [llength $coordList] == 6} {
        #puts "in first case"
        # COORDLIST IS NOT EMPTY IF WE WANT TO SET THE VIRTUAL CAMERA ONLY
        # BASED ON INFORMATION ABOUT THE POSITION OF THE ACTOR CAMERA AND
        # ACTOR FOCAL POINT
        # we only have information about the position of the camera and the 
        # focal point. Extrapolate the additional information from that 
        $vcam SetPosition [lindex $coordList 0] [lindex $coordList 1] [expr [lindex $coordList 2] +2]
        $vcam SetFocalPoint [lindex $coordList 3] [lindex $coordList 4] [lindex $coordList 5] 
        # use prior information to prevent the View from flipping at undefined
        # boundary points (i.e when the viewUp and the viewPlaneNormal are 
        # parallel, OrthogonalizeViewUp sometimes produces a viewUp that 
        # flips direction 

        # weird boundary case if user the fp/cam vector is parallel to old
        # view up -- check for that with the dot product

        $vcam SetViewUp $Neuroendoscopy(cam,viewUpX) $Neuroendoscopy(cam,viewUpY) $Neuroendoscopy(cam,viewUpZ)    
        
    } elseif {$coordList == ""} {
        # COORDLIST IS EMPTY IF WE JUST WANT THE VIRTUAL CAMERA TO MIMICK
        # THE CURRENT ACTOR CAMERA
        # we want the virtual camera to be in the same position/orientation 
        # than the endoscope and we have all the information we need
        # so set the position, focal point, and view up (the z unit vector of 
        # the camera actor's orientation [the 3rd column of its world matrix])
        set cam_mat [Neuroendoscopy(cam,actor) GetMatrix]   
        $vcam SetPosition [$cam_mat GetElement 0 3] [$cam_mat GetElement 1 3] [expr [$cam_mat GetElement 2 3] +2]     
        set fp_mat [Neuroendoscopy(fp,actor) GetMatrix]
        $vcam SetFocalPoint [$fp_mat GetElement 0 3] [$fp_mat GetElement 1 3] [$fp_mat GetElement 2 3] 
        $vcam SetViewUp [$cam_mat GetElement 0 2] [$cam_mat GetElement 1 2] [$cam_mat GetElement 2 2] 
    }
    $vcam ComputeViewPlaneNormal        
    $vcam OrthogonalizeViewUp
    # save the current view Up
    set l [$Neuroendoscopy(activeCam) GetViewUp]
    set Neuroendoscopy(cam,viewUpX) [expr [lindex $l 0]]
    set Neuroendoscopy(cam,viewUpY) [expr [lindex $l 1]] 
    set Neuroendoscopy(cam,viewUpZ) [expr [lindex $l 2]]

    # save the current view Plane
    set l [$Neuroendoscopy(activeCam) GetViewPlaneNormal]
    set Neuroendoscopy(cam,viewPlaneNormalX) [expr -[lindex $l 0]]
    set Neuroendoscopy(cam,viewPlaneNormalY) [expr -[lindex $l 1]] 
    set Neuroendoscopy(cam,viewPlaneNormalZ) [expr -[lindex $l 2]]
    
    NeuroendoscopySetCameraViewAngle
    eval $vcam SetClippingRange $View(neuroendoscopyClippingRange)    
    NeuroendoscopyLightFollowEndoCamera $vcam
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyLightFollowEndoCamera
# Adjust the light to follow the camera.
# .ARGS
# str vcam virtual endoscope name
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyLightFollowEndoCamera {vcam} {
    global View Neuroendoscopy
    
    # 3D Viewer
    
    set endoCurrentLight View($vcam,light)
    
    eval $endoCurrentLight SetPosition [$vcam GetPosition]
    eval $endoCurrentLight SetIntensity 1
    eval $endoCurrentLight SetFocalPoint [$vcam GetFocalPoint]
    
    set endoCurrentLight View($vcam,light2)
    eval $endoCurrentLight SetFocalPoint [$Neuroendoscopy(activeCam) GetPosition]
    eval $endoCurrentLight SetIntensity 1
    eval $endoCurrentLight SetConeAngle 180    
    eval $endoCurrentLight SetPosition [$Neuroendoscopy(activeCam) GetFocalPoint]
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetCameraZoom
# Set the neuroendoscopy camera's zoom.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetCameraZoom {} {
    global Neuroendoscopy Model View 
    
    # get the View plane of the virtual camera because we want to move 
    # in and out along that plane
    set l [$Neuroendoscopy(activeCam) GetViewPlaneNormal]
    set IO(x) [expr -[lindex $l 0]]
    set IO(y) [expr -[lindex $l 1]] 
    set IO(z) [expr -[lindex $l 2]]

    set Neuroendoscopy(fp,distance) $Neuroendoscopy(cam,zoomStr)

    # now move the fp a percentage of its distance to the camera
    set Neuroendoscopy(cam,x) [expr $Neuroendoscopy(fp,x) - $IO(x) * $Neuroendoscopy(fp,distance)]
    set Neuroendoscopy(cam,y) [expr $Neuroendoscopy(fp,y) - $IO(y) * $Neuroendoscopy(fp,distance)]
    set Neuroendoscopy(cam,z) [expr $Neuroendoscopy(fp,z) - $IO(z) * $Neuroendoscopy(fp,distance)]
    
    Neuroendoscopy(cam,actor) SetPosition $Neuroendoscopy(cam,x) $Neuroendoscopy(cam,y) $Neuroendoscopy(cam,z)
    Neuroendoscopy(gyro,actor) SetPosition $Neuroendoscopy(cam,x) $Neuroendoscopy(cam,y) $Neuroendoscopy(cam,z)
    NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam)
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetCameraViewAngle
# Set the Neuroendoscopy camera's view angle.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetCameraViewAngle {} {
    global Neuroendoscopy Model View

    set Neuroendoscopy(cam,viewAngle) $Neuroendoscopy(cam,AngleStr)
    $Neuroendoscopy(activeCam) SetViewAngle $Neuroendoscopy(cam,viewAngle)
    
    #Show the viewing angle in the camera
    
    if {$Neuroendoscopy(cam,viewAngle) < 80 && $Neuroendoscopy(cam,showAngle) == 1} {
      ccamAngle SetAngle $Neuroendoscopy(cam,viewAngle)

    } else {
     ccamAngle SetAngle 0
    }
    #Render3D
    
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetCameraAxis
# Set the Neuroendoscopy camera's axis.
# .ARGS
# str axis absolute or relative, optional, defaults to empty string to do nothing
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetCameraAxis {{axis ""}} {
    global Neuroendoscopy Model
    
    if {$axis != ""} {
        if {$axis == "absolute" || $axis == "relative"} {
            set Neuroendoscopy(cam,axis) $axis
            
            # Change button text
            #$Neuroendoscopy(axis) config -text $axis

            
            # update the actual camera position for the slider

            if {$axis == "relative"} {
                $Neuroendoscopy(labelx) configure -text "Left/Right"
                $Neuroendoscopy(labely) configure -text "Forw/Back"
                $Neuroendoscopy(labelz) configure -text "Up/Down"        

            } elseif {$axis == "absolute"} {
                $Neuroendoscopy(labelx) configure -text "L<->R "
                $Neuroendoscopy(labely) configure -text "P<->A "
                $Neuroendoscopy(labelz) configure -text "I<->S "
            }
            set l [$Neuroendoscopy(gyro,actor) GetPosition]
            set Neuroendoscopy(cam,xStr) [expr [lindex $l 0]]
            set Neuroendoscopy(cam,yStr) [expr [lindex $l 1]]
            set Neuroendoscopy(cam,zStr) [expr [lindex $l 2]]
            
            set Neuroendoscopy(cam,xStr,old) [expr [lindex $l 0]]
            set Neuroendoscopy(cam,yStr,old) [expr [lindex $l 1]]
            set Neuroendoscopy(cam,zStr,old) [expr [lindex $l 2]]
            
            $Neuroendoscopy(sliderx) set $Neuroendoscopy(cam,xStr)
            $Neuroendoscopy(slidery) set $Neuroendoscopy(cam,yStr)
            $Neuroendoscopy(sliderz) set $Neuroendoscopy(cam,zStr)
            
            
            vtkTransform tmp
            tmp SetMatrix [$Neuroendoscopy(gyro,actor) GetMatrix] 
            set l [tmp GetOrientation]
            tmp Delete
            set Neuroendoscopy(cam,rxStr) [expr [lindex $l 0]]
            set Neuroendoscopy(cam,ryStr) [expr [lindex $l 1]]
            set Neuroendoscopy(cam,rzStr) [expr [lindex $l 2]]
            
            set Neuroendoscopy(cam,rxStr,old) [expr [lindex $l 0]]
            set Neuroendoscopy(cam,ryStr,old) [expr [lindex $l 1]]
            set Neuroendoscopy(cam,rzStr,old) [expr [lindex $l 2]]
            
            $Neuroendoscopy(sliderrx) set $Neuroendoscopy(cam,rxStr)
            $Neuroendoscopy(sliderry) set $Neuroendoscopy(cam,ryStr)
            $Neuroendoscopy(sliderrz) set $Neuroendoscopy(cam,rzStr)
        
        } else {
            return
        }   
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCameraMotionFromUser
#
# Called whenever the active camera is moved. This routine syncs the position of
# the graphical neuroendoscopy camera with the virtual neuroendoscopy camera
# (i.e if the user changes the view of the neuroendoscopy window with the mouse,
#  we want to change the position of the graphical camera)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCameraMotionFromUser {} {
    
    global View Neuroendoscopy
    global CurrentCamera  
    
    #puts "$CurrentCamera"
    if {$CurrentCamera == $Neuroendoscopy(activeCam)} {
        NeuroendoscopyUpdateActorFromVirtualEndoscope $Neuroendoscopy(activeCam)
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetCollision
# Set Neuroendoscopy array's collision entry and configure the collision gui element.
# .ARGS
# int value 0 for collision detection off, 1 for on
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetCollision {value} {
    global Neuroendoscopy

    set Neuroendoscopy(collision) $value
    if { $value == 0 } {
        $Neuroendoscopy(collMenu) config -text "off"
    } else {
        $Neuroendoscopy(collMenu) config -text "on"
    }
}


############################################################################
#
#       PART 5: Vector Operation
#
###########################################################################

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyMoveGyroToLandmark
# Get the point's location and orientatin and move the gyro there.
# .ARGS
# int id landmark point id
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyMoveGyroToLandmark {id} {
    
    global Neuroendoscopy 
    set xyz [Neuroendoscopy(cLand,keyPoints) GetPoint $id]
    set rxyz [Neuroendoscopy(fLand,keyPoints) GetPoint $id]
    
    NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam) "[lindex $xyz 0] [lindex $xyz 1] [lindex $xyz 2] [lindex $rxyz 0] [lindex $rxyz 1] [lindex $rxyz 2]" 
    NeuroendoscopyUpdateActorFromVirtualEndoscope $Neuroendoscopy(activeCam)
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUpdateVectors
# Update the vectors.
# .ARGS
# int id neuroendoscopy id for path
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUpdateVectors {id} {
        
    global Neuroendoscopy
    set numPoints [Neuroendoscopy($id,cpath,keyPoints) GetNumberOfPoints]
    for {set i 0} {$i < $numPoints} {incr i} {
        set cp [Neuroendoscopy($id,cpath,keyPoints) GetPoint $i]
        set fp [Neuroendoscopy($id,cpath,keyPoints) GetPoint $i]
        
        set cpX [lindex $cp 0]
        set cpY [lindex $cp 1]
        set cpZ [lindex $cp 2]
        
        set fpX [lindex $fp 0]
        set fpY [lindex $fp 1]
        set fpZ [lindex $fp 2]
        
        Neuroendoscopy($id,vector,vectors) InsertVector $i [expr $fpX - $cpX] [expr $fpY - $cpY] [expr $fpZ - $cpZ]
        
        Neuroendoscopy($id,vector,scalars) InsertScalar $i .5 
        Neuroendoscopy($id,vector,polyData) Modified
    }
}

#############################################################################
#
#     PART 6: Landmark Operations
#
#############################################################################


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyGetAvailableListName
# Returns the next available list name
# .ARGS
# string  name of the path
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyGetAvailableListName {model} {
    global Neuroendoscopy

    if {[info exists Neuroendoscopy(${model}Path,nextAvailableId)] == 0} {
        set Neuroendoscopy(${model}Path,nextAvailableId) 1
    }
    set numList $Neuroendoscopy(${model}Path,nextAvailableId)
    set list ${model}$numList
    incr Neuroendoscopy(${model}Path,nextAvailableId)
    return $list
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyAddLandmarkNoDirectionSpecified
#
# This procedure is called when the user adds a landmark at position i 
# on a slice or on a model and we don't know yet what direction of view we 
# should save along with the landmark. There are 2 cases:<br>
#  i = 1 => the direction vector is [0 1 0]<br>
#  i > 1 => The direction vector is tangential to the curve <br>
# [(position of landmark i - 1) - (position of last interpolated point on the path]<br>
# 
# The user can then change the direction vector interactively through the 
# user interface.
# .ARGS
# float x
# float y
# float z
# str list
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyAddLandmarkNoDirectionSpecified {x y z {list ""}} {
    global Neuroendoscopy Point Fiducials
    
    
    ########### GET THE RIGHT LIST TO ADD THE POINT TO ############
    # if the list is not of type neuroendoscopy, create a new neuroendoscopy list
    if { $list == "" } {
        # add point to active path otherwise use the default path
        if { $Neuroendoscopy(path,activeId) == "None" } {
            set numList $Neuroendoscopy(path,nextAvailableId)
            set list Path${numList}_
        NeuroendoscopyCreateAndActivatePath $Path${numList}_
        incr Neuroendoscopy(path,nextAvailableId)
        } else {
        set id $Neuroendoscopy(path,activeId)
        set list $Neuroendoscopy($id,path,name)
    }
    } else {
    if {[info exists Fiducials($list,fid)] == 0} {    
        # if the list doesn't exist, create it
        NeuroendoscopyCreateAndActivatePath $list
    }
    }
    # make that list active
    FiducialsSetActiveList $list

    set pid [FiducialsCreatePointFromWorldXYZ "neuroendoscopy" $x $y $z $list]
    
    # this is now the direction of the vector
    # if i = 0, give it the default direction 0 1 0
    # else if i > 0, give it the tangential direction
    if { $pid == 0 } {
        Point($pid,node) SetFXYZ $x [expr $y + 1]  $z
    } else {
    
        set prev [expr $pid - 1]
        set prevList [Point($prev,node) GetXYZ]
        
        set d(x) [expr ($x - [lindex $prevList 0])]
        set d(y) [expr ($y - [lindex $prevList 1])]
        set d(z) [expr ($z - [lindex $prevList 2])]
        
        Normalize d
        Point($pid,node) SetFXYZ [expr $x + $d(x)] [expr $y + $d(y)] [expr $z + $d(z)]
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyInsertLandmarkNoDirectionSpecified
#
# This procedure is called when the user adds a landmark after the landmark with id "previousPid" 
# on a slice or on a model and we don't know yet what direction of view we 
# should save along with the landmark. There are 2 cases:<br>
#  i = 1 => the direction vector is [0 1 0]<br>
#  i > 1 => The direction vector is tangential to the curve <br>
# [(position of landmark i - 1) - (position of last interpolated point on the path]<br>
# 
# The user can then change the direction vector interactively through the 
# user interface.
# .ARGS
# int afterPid id of the point to add this one after
# float x x position of new landmark
# float y y position of new landmark
# float z z position of new landmark
# str list optional, if empty, add to active path
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyInsertLandmarkNoDirectionSpecified {afterPid x y z {list ""}} {
    global Neuroendoscopy Point Fiducials
    
    
    ########### GET THE RIGHT LIST TO ADD THE POINT TO ############
    # if the list is not of type neuroendoscopy, create a new neuroendoscopy list
    if { $list == "" } {
        # add point to active path otherwise use the default path
        if { $Neuroendoscopy(path,activeId) == "None" } {
            set numList $Neuroendoscopy(path,nextAvailableId)
            set list Path${numList}_
        NeuroendoscopyCreateAndActivatePath $Path${numList}_
        incr Neuroendoscopy(path,nextAvailableId)
        } else {
        set id $Neuroendoscopy(path,activeId)
        set list $Neuroendoscopy($id,path,name)
    }
    } else {
    if {[info exists Fiducials($list,fid)] == 0} {    
        # if the list doesn't exist, create it
        NeuroendoscopyCreateAndActivatePath $list
    }
    }
    # make that list active
    FiducialsSetActiveList $list

    set pid [FiducialsInsertPointFromWorldXYZ "neuroendoscopy" $afterPid $x $y $z $list]
    
    # this is now the direction of the vector
    # if i = 0, give it the default direction 0 1 0
    # else if i > 0, give it the tangential direction
    if { $pid == 0 } {
        Point($pid,node) SetFXYZ $x [expr $y + 1]  $z
    } else {
    
        set prev [expr $pid - 1]
        set prevList [Point($prev,node) GetXYZ]
        
        set d(x) [expr ($x - [lindex $prevList 0])]
        set d(y) [expr ($y - [lindex $prevList 1])]
        set d(z) [expr ($z - [lindex $prevList 2])]
        
        Normalize d
        Point($pid,node) SetFXYZ [expr $x + $d(x)] [expr $y + $d(y)] [expr $z + $d(z)]
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyAddLandmarkDirectionSpecified
#
# This procedure is called when we want to add a landmark at position i and 
# we know that the direction of view to save along with the landmark is the
# current view direction of the endoscope
# 
# .ARGS
# str coords optional, if empty, get from the endo camera
# str list optional, path name, if empty, use the active path
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyAddLandmarkDirectionSpecified {{coords ""} {list ""}} {

    global Neuroendoscopy Point Fiducials 
    
 ########### GET THE RIGHT LIST TO ADD THE POINT TO ############
    # if the list is not of type neuroendoscopy, create a new neuroendoscopy list
    if { $list == "" } {
        # add point to active path otherwise use the default path
        if { $Neuroendoscopy(path,activeId) == "None" } {
            set numList $Neuroendoscopy(path,nextAvailableId)
            set list Path${numList}_
            NeuroendoscopyCreateAndActivatePath $list
            incr Neuroendoscopy(path,nextAvailableId)
        } else {
            set id $Neuroendoscopy(path,activeId)
            set list $Neuroendoscopy($id,path,name)
        }
    } else {
        if {[info exists Fiducials($list,fid)] == 0} {    
            # if the list doesn't exist, create it
            NeuroendoscopyCreateAndActivatePath $list
        }
    }
    # make that list active
    FiducialsSetActiveList $list
    
    ########## GET THE COORDINATES ################
    if { $coords == "" } {
        set cam_mat [Neuroendoscopy(cam,actor) GetMatrix]   
        set fp_mat [Neuroendoscopy(fp,actor) GetMatrix]   
        set x [$cam_mat GetElement 0 3]
        set y [$cam_mat GetElement 1 3]
        set z [$cam_mat GetElement 2 3]
        set fx [$fp_mat GetElement 0 3]
        set fy [$fp_mat GetElement 1 3]
        set fz [$fp_mat GetElement 2 3]
    } else {
        set x [lindex $coords 0]
        set y [lindex $coords 1]
        set z [lindex $coords 2]
        set fx [lindex $coords 3]
        set fy [lindex $coords 4]
        set fz [lindex $coords 5]
    }

    set pid [FiducialsCreatePointFromWorldXYZ "neuroendoscopy" $x $y $z $list]
    if { $pid != "" } {
        Point($pid,node) SetFXYZ $fx $fy $fz
    }

}


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyInsertLandmarkDirectionSpecified
#
# This procedure is called when we want to insert a landmark at position after the currently selected landmark and 
# we know that the direction of view to save along with the landmark is the
# current view direction of the endoscope
# 
# .ARGS
# str coords optional, if empty string, get it from the neuroendoscopy camera
# str list optional, the list to insert into
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyInsertLandmarkDirectionSpecified {{coords ""} {list ""}} {

     global Neuroendoscopy Point Fiducials 
    
    # get the ids of the selected fiducial
    set afterPid $Neuroendoscopy(selectedFiducialPoint) 
    set fid $Neuroendoscopy(selectedFiducialList) 
    

    if { $afterPid == "" } {
        NeuroendoscopyAddLandmarkDirectionSpecified
        tk_messageBox -message "No Landmark selected. Choosing the last landmark on the path by default "
    } elseif { $fid == "" } {
        NeuroendoscopyAddLandmarkDirectionSpecified
        tk_messageBox -message "No Landmark selected. Choosing the last landmark on the path by default "
    } else {
        set list $Fiducials($fid,name) 
        # make that list active
        FiducialsSetActiveList $list
    
    ########## GET THE COORDINATES ################
    if { $coords == "" } {
        set cam_mat [Neuroendoscopy(cam,actor) GetMatrix]   
        set fp_mat [Neuroendoscopy(fp,actor) GetMatrix]   
        set x [$cam_mat GetElement 0 3]
        set y [$cam_mat GetElement 1 3]
        set z [$cam_mat GetElement 2 3]
        set fx [$fp_mat GetElement 0 3]
        set fy [$fp_mat GetElement 1 3]
        set fz [$fp_mat GetElement 2 3]
    } else {
        set x [lindex $coords 0]
        set y [lindex $coords 1]
        set z [lindex $coords 2]
        set fx [lindex $coords 3]
        set fy [lindex $coords 4]
        set fz [lindex $coords 5]
    }

    set pid [FiducialsInsertPointFromWorldXYZ "neuroendoscopy" $afterPid $x $y $z $list]
    if { $pid != "" } {
        Point($pid,node) SetFXYZ $fx $fy $fz
    }
  }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUpdateLandmark
#
# This procedure is called when we want to update the current selected  landmark to 
# the current position and orientation of the camera actor
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUpdateLandmark {} {

    global Neuroendoscopy

    # get the current coordinates of the camera and focal point
    set cam_mat [Neuroendoscopy(cam,actor) GetMatrix]  
    set fp_mat [Neuroendoscopy(fp,actor) GetMatrix]   

    # update the selected pid
    set pid $Neuroendoscopy(selectedFiducialPoint) 
    if {$pid != ""} {
    Point($pid,node) SetXYZ [$cam_mat GetElement 0 3] [$cam_mat GetElement 1 3] [$cam_mat GetElement 2 3]
    Point($pid,node) SetFXYZ [$fp_mat GetElement 0 3] [$fp_mat GetElement 1 3] [$fp_mat GetElement 2 3]
    MainUpdateMRML
    }
}

#############################################################################
#
#    PART 7: Path Operation
#
############################################################################

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyBuildInterpolatedPath
#
# This procedure creates a new path model by:<br>
# creating a path containing all landmarks <br>
#    from i = 0 to i = # of points added with NeuroendoscopyAddLandmark*<br>
#
# It is much faster to create a path by iteratively calling <br>
# NeuroendoscopyAddLandmark* for each new landmark -> NeuroendoscopyBuildInterpolatedPath<br>
# 
# Then by iteratively calling<br>
# (NeuroendoscopyAddLandmark* -> NeuroendoscopyBuildInterpolatedPath) for each new landmark<br>
#
# But the advantage of the latter is that the user can see the path being<br>
# created iteratively (the former is used when loading mrml paths).
#
# .ARGS
# int id neuroendoscopy path id
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyBuildInterpolatedPath {id} {
    
    global Neuroendoscopy Fiducials 
    
    # create vtk variables
    set numLandmarks [Neuroendoscopy($id,cpath,keyPoints) GetNumberOfPoints]
    set i $numLandmarks
    
    # only build the path if there are at least 2 landmarks
    if { $numLandmarks > 1 } {

        set numberOfkeyPoints [Neuroendoscopy($id,cpath,keyPoints) GetNumberOfPoints]

        #evaluate the first point of the spline (0)    

        Neuroendoscopy($id,cpath,graphicalInterpolatedPoints) InsertPoint 0 \
                [Neuroendoscopy($id,cpath,Spline,x) Evaluate 0] \
                [Neuroendoscopy($id,cpath,Spline,y) Evaluate 0] \
                [Neuroendoscopy($id,cpath,Spline,z) Evaluate 0]
        
        Neuroendoscopy($id,cpath,allInterpolatedPoints) InsertPoint 0 \
                [Neuroendoscopy($id,cpath,Spline,x) Evaluate 0] \
                [Neuroendoscopy($id,cpath,Spline,y) Evaluate 0] \
                [Neuroendoscopy($id,cpath,Spline,z) Evaluate 0]
        
        Neuroendoscopy($id,fpath,allInterpolatedPoints) InsertPoint 0 \
                [Neuroendoscopy($id,fpath,Spline,x) Evaluate 0] \
                [Neuroendoscopy($id,fpath,Spline,y) Evaluate 0] \
                [Neuroendoscopy($id,fpath,Spline,z) Evaluate 0]

        
        # now build the rest of the spline
        for {set i 0} {$i< [expr $numberOfkeyPoints - 1]} {incr i 1} {
            
            set pci [Neuroendoscopy($id,cpath,keyPoints) GetPoint $i]
            set pcni [Neuroendoscopy($id,cpath,keyPoints) GetPoint [expr $i+1]]
            set pfi [Neuroendoscopy($id,fpath,keyPoints) GetPoint $i]
            set pfni [Neuroendoscopy($id,fpath,keyPoints) GetPoint [expr $i+1]]
            
            # calculate the distance di between key point i and i+1 for both 
            # the camera path and fp path, keep the highest one
            
            set Neuroendoscopy($id,cpath,dist,$i) [eval NeuroendoscopyDistanceBetweenTwoPoints $pci $pcni]
            set Neuroendoscopy($id,fpath,dist,$i) [eval NeuroendoscopyDistanceBetweenTwoPoints $pfi $pfni]
            
            
            
            if {$Neuroendoscopy($id,cpath,dist,$i) >= $Neuroendoscopy($id,fpath,dist,$i)} {
                set di $Neuroendoscopy($id,cpath,dist,$i)
            } else {
                set di $Neuroendoscopy($id,fpath,dist,$i)
            }
            # take into account the interpolation factor
        # di is the distance between 2 control points in mm
            # step is the number of interpolated landmarks between each 
        # control point
        # interpolationStr is the number of landmarks per mm
            # i.e if interpolationStr = 1, and di = 10mm, 
        # this means we have a step of 1/10 
            # if interpolation = 2, and di = 10mm
        # this means that we have a step of 1/20
        # (note that the distance between interpolated landmarks is given 
        # by interpolationStr * di).

            if { $di <.1 } {
                set step 0
            } else {
                set step [expr 1/($Neuroendoscopy(path,interpolationStr) * $di)]
            }

            # if no interpolation wanted or distance is too small, only
            # do one iteration
            if {$step == 0} {
                set step 1
            }

            # evaluate the spline di times start after i, finish at i+1
            for {set j $step} {$j <= 1} {set j [expr $j + $step]} {
                set t [expr $i + $j]
                
                # add the points for the graphical lines
                if {$Neuroendoscopy($id,cpath,dist,$i) !=0} {
                    set numPoints [Neuroendoscopy($id,cpath,graphicalInterpolatedPoints) GetNumberOfPoints]
                    
                    Neuroendoscopy($id,cpath,graphicalInterpolatedPoints) InsertPoint [expr $numPoints] [Neuroendoscopy($id,cpath,Spline,x) Evaluate $t] [Neuroendoscopy($id,cpath,Spline,y) Evaluate $t] [Neuroendoscopy($id,cpath,Spline,z) Evaluate $t]
                }

                foreach m "c f" {
                    # add the points for the landmark record
                    set numPoints [Neuroendoscopy($id,${m}path,allInterpolatedPoints) GetNumberOfPoints]
                    Neuroendoscopy($id,${m}path,allInterpolatedPoints) InsertPoint [expr $numPoints] [Neuroendoscopy($id,${m}path,Spline,x) Evaluate $t] [Neuroendoscopy($id,${m}path,Spline,y) Evaluate $t] [Neuroendoscopy($id,${m}path,Spline,z) Evaluate $t]
                }
            }
        }
        
        set numberOfOutputPoints [Neuroendoscopy($id,cpath,allInterpolatedPoints) GetNumberOfPoints]
        set Neuroendoscopy(path,exists) 1
        
        # since that is where the camera is
        # add cell data
        
        set numberOfOutputPoints [Neuroendoscopy($id,cpath,graphicalInterpolatedPoints) GetNumberOfPoints]
        Neuroendoscopy($id,path,lines) InsertNextCell $numberOfOutputPoints
        for {set i 0} {$i< $numberOfOutputPoints} {incr i 1} {
            Neuroendoscopy($id,path,lines) InsertCellPoint $i
        }    

        Neuroendoscopy($id,path,source) Modified
        # now update the vectors
        #NeuroendoscopyUpdateVectors $id
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyDeletePath
# Delete the active fiducials list.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyDeletePath {} {
    global Neuroendoscopy Fiducials

    FiducialsDeleteList $Fiducials(activeList)
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyComputeRandomPath
# Create 20 random points in a path.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyComputeRandomPath {} {
    global Neuroendoscopy Fiducials
    
    for {set i 0} {$i<20} {incr i 1} {
        set x  [expr [Neuroendoscopy(path,vtkmath) Random -1 1] * 100]
        set y  [expr [Neuroendoscopy(path,vtkmath) Random -1 1] * 100]
        set z  [expr [Neuroendoscopy(path,vtkmath) Random -1 1] * 100] 
        NeuroendoscopyAddLandmarkNoDirectionSpecified $x $y $z "random$Neuroendoscopy(randomPath,nextAvailableId)"
    }
    incr Neuroendoscopy(randomPath,nextAvailableId)
    MainUpdateMRML
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyShowPath
# Add or remove actors from the neuroendoscopyScreen, if showPath is 1 or not.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyShowPath {} {
    global Path Neuroendoscopy

    if {$Neuroendoscopy(path,exists) == 1} {
        #jeanette    
        set id $Neuroendoscopy(path,activeId)
        
        if {$Neuroendoscopy(path,showPath) == 1} {
            neuroendoscopyScreen AddActor Neuroendoscopy($id,path,actor)
        } else {
            neuroendoscopyScreen RemoveActor Neuroendoscopy($id,path,actor)
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyFlyThroughPath
# Move through the path.
# .ARGS
# str listOfCams
# str listOfPaths
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyFlyThroughPath {listOfCams listOfPaths} {
    global Neuroendoscopy Model View Path Module 
    
    
    if {[lindex $listOfPaths 0] == "None"} {
        return
    }
    
    # for now assume they have as many points, so get it from the first
    # path on the list 
    set id [lindex $listOfPaths 0]
    
    set numPoints [Neuroendoscopy($id,cpath,allInterpolatedPoints) GetNumberOfPoints]
    for {set Neuroendoscopy(path,i) $Neuroendoscopy(path,stepStr)} {$Neuroendoscopy(path,i)< $numPoints} {incr Neuroendoscopy(path,i) $Neuroendoscopy(path,speed)} { 
        if {$Neuroendoscopy(path,stop) == "0"} {
            set Neuroendoscopy(path,stepStr) $Neuroendoscopy(path,i)
            
            NeuroendoscopySetPathFrame $listOfCams $listOfPaths
            
            update            
            Render3D    
        } else {    
            NeuroendoscopyResetStopPath
            break
        }    
    }

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetPathFrame
# Update the actors from the virtual endoscope.
# .ARGS
# str listOfCams
# str listOfPaths
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetPathFrame {listOfCams listOfPaths} {
    global Neuroendoscopy Model View Path 


    if {[lindex $listOfPaths 0] == "None"} {
        return
    }
    
    foreach cam $listOfCams id $listOfPaths {
        
        set Neuroendoscopy(path,i) $Neuroendoscopy(path,stepStr)
        set l [Neuroendoscopy($id,cpath,allInterpolatedPoints) GetPoint $Neuroendoscopy(path,i)] 
        set l2 [Neuroendoscopy($id,fpath,allInterpolatedPoints) GetPoint $Neuroendoscopy(path,i)]
        
        NeuroendoscopyUpdateVirtualEndoscope $cam "[lindex $l 0] [lindex $l 1] [lindex $l 2] [lindex $l2 0] [lindex $l2 1] [lindex $l2 2]"
        NeuroendoscopyUpdateActorFromVirtualEndoscope $cam 
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyStopPath
# Stop fly through.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyStopPath {} {
    global Path Neuroendoscopy
        
    set Neuroendoscopy(path,stop) 1
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyResetStopPath
# Resume fly through.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyResetStopPath {} {
    global Neuroendoscopy
        
    set Neuroendoscopy(path,stop) 0
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyResetPath
# Stop the flythrough and reset the camera view.
# .ARGS
# str listOfCams
# str listOfPaths
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyResetPath {listOfCams listOfPaths} {
    global Neuroendoscopy Path 


    if {[lindex $listOfPaths 0] == "None"} {
        return
    }
        
    set Neuroendoscopy(cam,viewUpX) 0
    set Neuroendoscopy(cam,viewUpY) 0
    set Neuroendoscopy(cam,viewUpZ) 1
    NeuroendoscopyStopPath
    
    set Neuroendoscopy(path,stepStr) 0
    NeuroendoscopySetPathFrame $listOfCams $listOfPaths
    NeuroendoscopyResetStopPath
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetSpeed
# Set the speed of the fly through.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetSpeed {} {
    global Neuroendoscopy Path
    
    set Neuroendoscopy(path,speed) $Neuroendoscopy(path,speedStr)
}


#############################################################################
#
#     PART 8:  Slice driver operations
#
#############################################################################


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCheckDriver
# This procedure is called once the position of the endoscope is updated. <br>
# It checks to see if there is a driver for the slices and calls NeuroendoscopyReformatSlices 
# with the right argument to update the position of the slices.
# .ARGS
# str vcam the name of the virtual endoscope
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCheckDriver {vcam} {

    global Neuroendoscopy View Slice MainAnno Anno Module


    if { $Neuroendoscopy(fp,driver) == 1 } {
        eval NeuroendoscopyReformatSlices $vcam [$vcam GetFocalPoint]

    } elseif { $Neuroendoscopy(cam,driver) == 1 } {
        eval NeuroendoscopyReformatSlices $vcam [$vcam GetPosition]
    } elseif { $Neuroendoscopy(intersection,driver) == 1 } {
        set l [$View($vcam,renderer) GetCenter]
        set l0 [expr [lindex $l 0]]
        set l1 [expr [lindex $l 1]]
        if { [llength $l] > 2 } {
            set l2 [expr [lindex $l 2]]
        } else {
            set l2 0
        }
        set p [Neuroendoscopy(picker) Pick $l0 $l1 $l2 neuroendoscopyScreen]
        if { $p == 1} {
            set selPt [Neuroendoscopy(picker) GetPickPosition]
            set x [expr [lindex $selPt 0]]
            set y [expr [lindex $selPt 1]]
            set z [expr [lindex $selPt 2]]
            NeuroendoscopyReformatSlices $vcam $x $y $z
        
        set Anno(cross) 1
            set Anno(crossIntersect) 1
        
        MainAnnoSetVisibility
        MainAnnoUpdateFocalPoint $x $y $z
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyReformatSlices
# Get the endo camera's view up and plane normal then recomput the reformat matrix by 
# calling Slicer's SetDirectNTP
# .ARGS
# str vcam the name of the neuroendoscopy camera
# float x in call to SetDirectNTP, this is P's x value
# float y in call to SetDirectNTP, this is P's y value
# float z in call to SetDirectNTP, this is P's z value
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyReformatSlices {vcam x y z} {
    global Neuroendoscopy View Slice
    
    set vu [$vcam GetViewUp]
    set vpn [$vcam GetViewPlaneNormal]
    # Force recomputation of the reformat matrix
    Slicer SetDirectNTP \
        [lindex $vu 0] [lindex $vu 1] [lindex $vu 2] \
        [expr -[lindex $vpn 0]] [expr -[lindex $vpn 1]] [expr -[lindex $vpn 2]]  \
        $x $y $z
    RenderSlices
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetSliceDriver
# Set who's driving which slices are being currently displayed
# .ARGS
# str name who's driving now? User, Camera, FocalPoint, Intersection
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetSliceDriver {name} {
    global Neuroendoscopy Model View Slice


    # Change button text
  #  $Neuroendoscopy(mbDriver) config -text $name
    
    
    if { $name == "User" } {
        foreach s $Slice(idList) {
            Slicer SetDriver $s 0
        }
        set Neuroendoscopy(fp,driver) 0
        set Neuroendoscopy(cam,driver) 0
        set Neuroendoscopy(intersection,driver) 0
    } else {
        foreach s $Slice(idList) {
            Slicer SetDriver $s 1
        }
        if { $name == "Camera"} {
            set m cam
            set Neuroendoscopy(fp,driver) 0
            set Neuroendoscopy(cam,driver) 1
            set Neuroendoscopy(intersection,driver) 0
        } elseif { $name == "FocalPoint"} {
            set m fp 
            set Neuroendoscopy(fp,driver) 1
            set Neuroendoscopy(cam,driver) 0
            set Neuroendoscopy(intersection,driver) 0
        } elseif { $name == "Intersection"} {
            set m intersection 
            set Neuroendoscopy(fp,driver) 0
            set Neuroendoscopy(cam,driver) 0
            set Neuroendoscopy(intersection,driver) 1
        }
        
        MainSlicesSetOrientAll "Orthogonal"
        NeuroendoscopyCheckDriver $Neuroendoscopy(activeCam)
        RenderSlices
    }    
}


#############################################################################
#
#     PART 9: Fiducials operations
#
#############################################################################


#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyFiducialsPointSelectedCallback
# Select this point in the endo module, reset the camera if necessary to look at it,
# does not call Render3D, as that should be called by the proc firing off the callbacks.
# .ARGS
# int fid id of the fiducials list
# int pid point id on the list
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyFiducialsPointSelectedCallback {fid pid} {
    
    global Neuroendoscopy Fiducials Select Module Model Point

    if {$::Module(verbose)} {
        puts "NeuroendoscopyFiducialsPointSelectedCallback fid = $fid, pid = $pid, active list id = $Fiducials(activeListID)"
    }

    set Neuroendoscopy(selectedFiducialPoint) $pid
    set Neuroendoscopy(selectedFiducialList) $fid
    # only reset the active list if it's changed
    if {$Fiducials(activeListID) != $Neuroendoscopy(selectedFiducialList)} {
        FiducialsSetActiveList $Fiducials($fid,name)
        if {$::Module(verbose)} {
            puts "NeuroendoscopyFiducialsPointSelectedCallback: after fid set active list to $fid"
        }
    }

    # if the source or sink button are on, then make that point the 
    if { $Neuroendoscopy(sourceButton,on) == 1 } {
        PathPlanningSetSource $fid $pid
        # get the selected actor, make it active
        if {$Select(actor) != ""} {
            foreach ren $Module(Renderers) {
                foreach id $Model(idList) {
                    if { [$Select(actor) GetProperty] == [Model($id,actor,$ren) GetProperty]} {
                        MainModelsSetActive $id
                    }
                }
            }
        }
        $Neuroendoscopy(sourceLabel) configure -text "[Point($pid,node) GetName]"
        set Neuroendoscopy(source,exists) 1    
        set Neuroendoscopy(sourceButton,on) 0
    } elseif { $Neuroendoscopy(sinkButton,on) == 1 } {
        PathPlanningSetSink $fid $pid
        # get the selected actor, make it active
        if {$Select(actor) != ""} {
            foreach ren $Module(Renderers) {
                foreach id $Model(idList) {
                    if { [$Select(actor) GetProperty] == [Model($id,actor,$ren) GetProperty]} {
                        MainModelsSetActive $id
                    }
                }
            }
        }
        $Neuroendoscopy(sinkLabel) configure -text "[Point($pid,node) GetName]"
        set Neuroendoscopy(sink,exists) 1    
        set Neuroendoscopy(sinkButton,on) 0
    }


    # if the point is of type neuroendoscopy, use the actual xyz and fxyz info
    if {[info command Fiducials($fid,node)] != ""} {
        if {[Fiducials($fid,node) GetType] == "neuroendoscopy" } {
            NeuroendoscopyResetCameraDirection    
            NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam) [concat [Point($pid,node) GetXYZ] [Point($pid,node) GetFXYZ]]
            
            #set test [Point($pid,node) GetFXYZ]
            #puts $test
            
        } else {
            # look at the point instead
            NeuroendoscopyResetCameraDirection    
            NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam) [concat [Point($pid,node) GetFXYZ] [Point($pid,node) GetXYZ]]
        }
    } else {
        DevErrorWindow "NeuroendoscopyFiducialsPointSelectedCallback: Fiducials($fid,node) does not exist"
    }
    NeuroendoscopyUpdateActorFromVirtualEndoscope $Neuroendoscopy(activeCam)
    # Render will get called in the proc going through the callback functions
#    Render3D
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyFiducialsPointCreatedCallback
# This procedures is a callback procedule called when a Fiducial Point is
# created.<br>
# If the Point is part of the "reformat" Fiducial list, then the procedure 
# selects the right number of fiducials as they are created based on which 
# step we are in
# .ARGS 
# str type counts if it's neuroendoscopy
# int fid fiducials list id
# int pid point id 
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyFiducialsPointCreatedCallback {type fid pid} {

    global Neuroendoscopy Fiducials Select Module Model
    if {$::Module(verbose)} {
        puts "NeuroendoscopyFiducialsPointCreatedCallback - is it red?"
        # after 2000
    }

    # jump to that point only if it is not an neuroendoscopy point
    if { $type != "neuroendoscopy" } {
        NeuroendoscopyFiducialsPointSelectedCallback $fid $pid
    }

    # if the point is added to the reformat list and the Volumes/Reformat Tab 
    # is currently active, then select the last 2 or 3 points (depending on what step we're in)

    
    set module $Module(activeID) 
    set row $Module($module,row) 
    set tab $Module($module,$row,tab) 

    if { $module == "Neuroendoscopy" && $tab == "Path"} {
        if { $Fiducials($fid,name) == "path" } {
            # select the point
            FiducialsSelectionUpdate $fid $pid 1 
            # toggle between the source and the sink
            if {$Neuroendoscopy(source,exists) == 0} {
                PathPlanningSetSource $fid $pid
                # get the selected actor, make it active
                if {$Select(actor) != ""} {
                    foreach ren $Module(Renderers) {
                        foreach id $Model(idList) {
                            if { [$Select(actor) GetProperty] == [Model($id,actor,$ren) GetProperty]} {
                                MainModelsSetActive $id
                            }
                        }
   
                    }
                }
                $Neuroendoscopy(sourceLabel) configure -text "[Point($pid,node) GetName]"
                set Neuroendoscopy(source,exists) 1
            } else {
                FiducialsSelectionUpdate $fid $pid 1
                PathPlanningSetSink $fid $pid
                $Neuroendoscopy(sinkLabel) configure -text "[Point($pid,node) GetName]"
                set Neuroendoscopy(source,exists) 0
            }
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUpdateMRML
# Called to update the Endo module's nodes in the mrml tree, turns off backface culling,
# and calls Render3D (which won't do anything if we're in a MainUpdateMRML session).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUpdateMRML {} {

    global Models Model Neuroendoscopy

    if {$::Module(verbose)} {
        puts "NeuroendoscopyUpdateMRML"
    }

    # turn off backface culling for models in neuroendoscopyScreen
    foreach m $Model(idList) {
        $Model($m,prop,neuroendoscopyScreen) SetBackfaceCulling 0
    }
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyStartCallbackFiducialsUpdateMRML
# Called at the beginning of FiducialsUpdateMRML, reset the path variables and
# menu
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyStartCallbackFiducialsUpdateMRML {} {

    global Neuroendoscopy

    if {$::Module(verbose)} {
        puts "NeuroendoscopyStartCallbackFiducialsUpdateMRML"
    }

    # reset the variables for all the paths we know about
    foreach id $Neuroendoscopy(path,activeIdList) {
        NeuroendoscopyResetPathVariables $id
    }
    
    # keep a list of currently existing path so make it empty for now
    set Neuroendoscopy(path,activeIdList) ""
    
    #reset the menu
    foreach m $Neuroendoscopy(mPathList) {
        $m delete 0 end
    }
    #$Neuroendoscopy(mPath4Fly) delete 0 end
    # FIXME?
    #set Neuroendoscopy(path,activeId) None
    
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyEndCallbackFiducialsUpdateMRML
# Called at the end, does nothing. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyEndCallbackFiducialsUpdateMRML {} {
    
    global Neuroendoscopy
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCallbackFiducialsUpdateMRML
#
# Called when Updated Fiducials are of type neuroendoscopy
# .ARGS
# str type the type of the fiducial, if not neuroendoscopy, return.
# int id id of the path
# list listOfPoints points in the path.
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCallbackFiducialsUpdateMRML {type id listOfPoints} {
    global Mrml Path Neuroendoscopy
    
    if { $type != "neuroendoscopy"} {
        return
    }

    if {$::Module(verbose)} {
        puts "NeuroendoscopyCallbackFiducialsUpdateMRML"
    }

    # if we never heard about this Id, then this is a new path
    if {[lsearch $Neuroendoscopy(path,allIdsUsed) $id] == -1} {
        NeuroendoscopyCreateVTKPath $id 
        set Neuroendoscopy($id,path,name) [Fiducials($id,node) GetName]
    }
    lappend Neuroendoscopy(path,activeIdList) $id

    # update the name field (different paths in time will have the same id)
    set Neuroendoscopy($id,path,name) [Fiducials($id,node) GetName]
    
    set i -1

    foreach lid $listOfPoints {
        # path position
        set i [expr $i + 1]
        set l [Point($lid,node) GetXYZ]
        set cx [lindex $l 0]
        set cy [lindex $l 1]
        set cz [lindex $l 2]
        set f [Point($lid,node) GetFXYZ]
        set fx [lindex $f 0]
        set fy [lindex $f 1]
        set fz [lindex $f 2]
        
        # update vtk
        # FIXME find a more elegant solution ??
        if {$cx == $fx && $cy == $fy && $cz == $fz} {
            set fx [expr $fx + 1]
        }
        Neuroendoscopy($id,cpath,Spline,x) AddPoint $i $cx
        Neuroendoscopy($id,cpath,Spline,y) AddPoint $i $cy
        Neuroendoscopy($id,cpath,Spline,z) AddPoint $i $cz
        Neuroendoscopy($id,cpath,keyPoints) InsertPoint $i $cx $cy $cz
            
        Neuroendoscopy($id,fpath,Spline,x) AddPoint $i $fx
        Neuroendoscopy($id,fpath,Spline,y) AddPoint $i $fy
        Neuroendoscopy($id,fpath,Spline,z) AddPoint $i $fz
        Neuroendoscopy($id,fpath,keyPoints) InsertPoint $i $fx $fy $fz
    }
    NeuroendoscopyBuildInterpolatedPath $id   
    
    # update the menus
    set name $Neuroendoscopy($id,path,name)
    
    foreach m $Neuroendoscopy(mPathList) {
        $m add command -label $name -command "NeuroendoscopySelectActivePath $id" 
    }
    # only add to the fly-through menu if the path has more than one point
    if {[Neuroendoscopy($id,cpath,keyPoints) GetNumberOfPoints] > 1} {
        $Neuroendoscopy(mPath4Fly) add command -label $name -command "NeuroendoscopySelectActivePath $id; NeuroendoscopyResetPath $Neuroendoscopy(activeCam) $id" 
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateAndActivatePath
# Calls FiducialsCreateFiducialsList and makes it active.
# .ARGS
# str name the name of the fiducial list to create
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreateAndActivatePath {name} {
    global Neuroendoscopy Fiducials
    
    set id [FiducialsCreateFiducialsList "neuroendoscopy" $name]
    # check to see if that exists already
    set ext 1
    while {$id == -1} {
        set id [FiducialsCreateFiducialsList "neuroendoscopy" ${name}($ext)]
        set ext [expr $ext + 1]
    }
    set name [Fiducials($id,node) GetName]
    set type [Fiducials($id,node) GetType]
    FiducialsSetActiveList $name
    NeuroendoscopyFiducialsActivatedListCallback "neuroendoscopy" $name $id
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyFiducialsActivatedListCallback
# This is a callback from the fiducials module telling us which list
# is active.<br>
# We update the path menus only if the active list is an neuroendoscopy
# one.
# .ARGS
# str type the type of the fiducial, if not neuroendoscopy, return.
# str name name of the list
# int id id of the path to be set active
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyFiducialsActivatedListCallback {type name id} {
    global Neuroendoscopy Fiducials

    # if an neuroendoscopy list is activated, tell all the menus in the 
    # neuroendoscopy displays
    if {$type == "neuroendoscopy" } {
    
        set Neuroendoscopy(path,activeId) $id
    
        # change the text on menu buttons
        foreach mb $Neuroendoscopy(mbPathList) {
            $mb config -text $Neuroendoscopy($id,path,name) 
        }
        if {[Neuroendoscopy($id,cpath,keyPoints) GetNumberOfPoints] > 1} {
            $Neuroendoscopy(mbPath4Fly) config -text $Neuroendoscopy($id,path,name) 
            # configure the scale
            set Neuroendoscopy(path,stepStr) 0
            set numberOfOutputPoints [Neuroendoscopy($id,cpath,graphicalInterpolatedPoints) GetNumberOfPoints]
            $Neuroendoscopy(path,stepScale) config -to [expr $numberOfOutputPoints - 1]
    
       } else {
           set Neuroendoscopy(path,activeId) "None"
           # change the text on menu buttons
           foreach mb $Neuroendoscopy(mbPathList) {
               $mb config -text "None"
           }
           $Neuroendoscopy(mbPath4Fly) config -text "None"
       }
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySelectActivePath
# Set the active list, change the text on the menu buttons
# .ARGS
# int id path id
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySelectActivePath {id} {
    
    global Neuroendoscopy Fiducials
    set Neuroendoscopy(path,activeId) $id
    # make that list active
    FiducialsSetActiveList $Neuroendoscopy($id,path,name)
    

    # change the text on menu buttons
    foreach mb $Neuroendoscopy(mbPathList) {
        $mb config -text $Neuroendoscopy($id,path,name) 
    }
    if {[Neuroendoscopy($id,cpath,keyPoints) GetNumberOfPoints] > 1} {
        $Neuroendoscopy(mbPath4Fly) config -text $Neuroendoscopy($id,path,name) 
        # configure the scale
        set Neuroendoscopy(path,stepStr) 0
        set numberOfOutputPoints [Neuroendoscopy($id,cpath,graphicalInterpolatedPoints) GetNumberOfPoints]
        $Neuroendoscopy(path,stepScale) config -to [expr $numberOfOutputPoints - 1]
    }
        
}

############################################################################
#
#  PART 10: Helper functions     
#
############################################################################

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyDistanceBetweenTwoPoints
# Returns the distance between two points.
# .ARGS
# float p1x first point x
# float p1y first point y
# float p1z first point z
# float p2x second point x
# float p2y second point y
# float p2z second point z
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyDistanceBetweenTwoPoints {p1x p1y p1z p2x p2y p2z} {

    return [expr sqrt((($p2x - $p1x) * ($p2x - $p1x)) + (($p2y - $p1y) * ($p2y - $p1y)) + (($p2z - $p1z) * ($p2z - $p1z)))]
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUpdateSelectionLandmarkList
# Update the landmark list by selecting just this point.
# .ARGS
# int id path id
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUpdateSelectionLandmarkList {id} {
    
    global Neuroendoscopy
    set sel [$Neuroendoscopy(path,fLandmarkList) curselection] 
    if {$sel != ""} {
        $Neuroendoscopy(path,fLandmarkList) selection clear $sel $sel
    }
    $Neuroendoscopy(path,fLandmarkList) selection set $id
    $Neuroendoscopy(path,fLandmarkList) see $id
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetModelsVisibilityInside
# Check the value of Neuroendoscopy(ModelsVisibilityInside) and call MainModelsSetCulling 
# for each model.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetModelsVisibilityInside {} {
    global View Model Neuroendoscopy

    if { $Neuroendoscopy(ModelsVisibilityInside) == 0 } {
        set value 1
    } else {
        set value 0
    }
    
    foreach m $Model(idList) {
        MainModelsSetCulling $m $value
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetSlicesVisibility
# Add or remove slice actors from the neuroendoscopy renderers depending on the value
# of Neuroendoscopy(SlicesVisibility).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetSlicesVisibility {} {
    global View Neuroendoscopy Module
    
    if { $Neuroendoscopy(SlicesVisibility) == 0 } {
        foreach s "0 1 2" {
            foreach ren $Module(neuroendoscopyRenderers) {
                $ren RemoveActor Slice($s,outlineActor)
                $ren RemoveActor Slice($s,planeActor)
            }
        }
    } else {
        foreach s "0 1 2" {
            foreach ren $Module(neuroendoscopyRenderers) {
                $ren AddActor Slice($s,outlineActor)
                $ren AddActor Slice($s,planeActor)
            }
        }
    }
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUpdateNeuroendoscopyViewVisibility
#  Makes the neuroendoscopy view appear or disappear based on the variable 
# Neuroendoscopy(mainview,visibility) <br> Note: there is a check to make sure that the 
# neuroendoscopy view cannot disappear if the main view is not visible.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUpdateNeuroendoscopyViewVisibility {} {
    global View viewWin Gui Neuroendoscopy 

    if {$Neuroendoscopy(endoview,visibility) == 1 && $Neuroendoscopy(mainview,visibility) == 1} {
        NeuroendoscopyAddNeuroendoscopyView
    } elseif {$Neuroendoscopy(endoview,visibility) == 0 && $Neuroendoscopy(mainview,visibility) == 1} {
        NeuroendoscopyRemoveNeuroendoscopyView
    } elseif {$Neuroendoscopy(endoview,visibility) == 1 && $Neuroendoscopy(mainview,visibility) == 0} {
        NeuroendoscopyAddNeuroendoscopyView
        NeuroendoscopyRemoveMainView
    }
    Render3D
    # for the rest do nothing
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUpdateMainViewVisibility
# Makes the main view appear or disappear based on the variable 
# Neuroendoscopy(mainview,visibility) <br> Note: there is a check to make sure that the 
# main view cannot disappear if the neuroendoscopy view is not visible.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUpdateMainViewVisibility {} {
    global View viewWin Gui Neuroendoscopy 

    if {$Neuroendoscopy(mainview,visibility) == 1 && $Neuroendoscopy(endoview,visibility) == 1} {
        NeuroendoscopyAddMainView
    } elseif {$Neuroendoscopy(mainview,visibility) == 0 && $Neuroendoscopy(endoview,visibility) == 1} {
        NeuroendoscopyRemoveMainView
    } elseif {$Neuroendoscopy(mainview,visibility) == 1 && $Neuroendoscopy(endoview,visibility) == 0} {
        NeuroendoscopyAddMainView
        NeuroendoscopyRemoveNeuroendoscopyView
    }
    Render3D
    # for the rest do nothing
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyAddNeuroendoscopyView
#  Add the neuroendoscopy renderer to the right of the main view 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyAddNeuroendoscopyView {} {
    global View viewWin Gui Neuroendoscopy

    if {$Neuroendoscopy(viewOn) == 0} {

        NeuroendoscopySetSliceDriver User
        # set the neuroendoscopy actors' visibility according to their prior visibility
            
        NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam)
            $viewWin AddRenderer neuroendoscopyScreen    
        viewRen SetViewport 0 0 .5 1
            neuroendoscopyScreen SetViewport .5 0 1 1
            MainViewerSetSecondViewOn
            MainViewerSetMode $View(mode) 
        set Neuroendoscopy(viewOn) 1
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyAddMainView
#  Add the main view to the left of the neuroendoscopy view
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyAddMainView {} {
    global View viewWin Gui Neuroendoscopy

    $viewWin AddRenderer viewRen    
    viewRen SetViewport 0 0 .5 1
    neuroendoscopyScreen SetViewport .5 0 1 1
    MainViewerSetSecondViewOn
    set Neuroendoscopy(viewOn) 1
    MainViewerSetMode $View(mode) 
    
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyAddNeuroendoscopyViewRemoveMainView
#  Makes the main view invisible/the neuroendoscopy view visible
#  (so switch between the 2 views)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyAddNeuroendoscopyViewRemoveMainView {} {
    global View viewWin Gui Neuroendoscopy  
   
    if {$Neuroendoscopy(viewOn) == 0} {

        NeuroendoscopySetSliceDriver Camera
        # set the neuroendoscopy actors' visibility according to their prior visibility
        
        foreach a $Neuroendoscopy(actors) {
            NeuroendoscopyUpdateVisibility $a
        }
        Render3D
        NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam)
        $viewWin AddRenderer neuroendoscopyScreen
        $viewWin RemoveRenderer viewRen
        neuroendoscopyScreen SetViewport 0 0 1 1
        MainViewerSetSecondViewOn
        set Neuroendoscopy(viewOn) 1
        MainViewerSetMode $View(mode) 
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyRemoveNeuroendoscopyView
#  Remove the neuroendoscopy view
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyRemoveNeuroendoscopyView {} {
    global Neuroendoscopy View viewWin Gui

    if { $Neuroendoscopy(viewOn) == 1} {


        $viewWin RemoveRenderer neuroendoscopyScreen    
        viewRen SetViewport 0 0 1 1
        MainViewerSetSecondViewOff
        set Neuroendoscopy(viewOn) 0
        MainViewerSetMode $View(mode) 
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyRemoveMainView
#  Remove the main view
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyRemoveMainView {} {
    global Neuroendoscopy View viewWin viewRen neuroendoscopyScreen 
    
    $viewWin RemoveRenderer viewRen    
    neuroendoscopyScreen SetViewport 0 0 1 1
    MainViewerSetSecondViewOn
    set Neuroendoscopy(viewOn) 1
    MainViewerSetMode $View(mode) 

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyAddMainViewRemoveNeuroendoscopyView
#  Makes the main view visible/the neuroendoscopy view invisible
#  (so switch between the 2 views)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyAddMainViewRemoveNeuroendoscopyView {} {
    global Neuroendoscopy View viewWin Gui

    if { $Neuroendoscopy(viewOn) == 1} {

        # reset the slice driver
        NeuroendoscopySetSliceDriver User

        # set all neuroendoscopy actors to be invisible, without changing their visibility 
        # parameters
        foreach a $Neuroendoscopy(actors) {
            Neuroendoscopy($a,actor) SetVisibility 0
            NeuroendoscopySetPickable $a 0
        }
        Render3D
        $viewWin AddRenderer viewRen
        $viewWin RemoveRenderer neuroendoscopyScreen    
        viewRen SetViewport 0 0 1 1
        MainViewerSetSecondViewOff
        set Neuroendoscopy(viewOn) 0
        MainViewerSetMode $View(mode) 
    }
}


##########################################################################

### SYNCHRONIZED

##########################################################################


#-------------------------------------------------------------------------------
#  .PROC NeuroendoscopyAutoSelectSourceSink
# Automatically set PathPlanningSetSource and PathPlanningSetSink
# .ARGS
# str list name of the list, defaults to an empty string
# .END 
#-------------------------------------------------------------------------------
proc NeuroendoscopyAutoSelectSourceSink {{list ""}} {
    global Point Fiducials Select Neuroendoscopy Model PathPlanning

   set model $Model(activeID)
   if {$model != ""} {
  
       # the reason to use Select(actor) is because the Fiducial call back 
       # below checks for it in the begining.
       # the actor info is then passed on to orient the direction of the camera.
    
       set Select(actor) Model($model,actor,viewRen)
  
       set polyData $Model($model,polyData)

       set cellNum [$polyData GetNumberOfCells]
       set randId  [expr round( [expr [expr rand()]*1000])]
       set minId $randId
       set maxId [expr [expr $cellNum -1] - $randId] 
       
       set Select(cellId) $minId
       
       set cell [$polyData GetCell $minId]
       set points [$cell GetPoints]
       set point2 [$points GetPoint 1]
       
       set x [lindex $point2 0]
       set y [lindex $point2 1]
       set z [lindex $point2 2]
       
       set pid [FiducialsCreatePointFromWorldXYZ "default" $x $y $z $list]
       if { $pid != "" } {
           set fid $Fiducials($Fiducials(activeList),fid)
           
           Fiducials($fid,node) SetTextSize 0.0
           Fiducials($fid,node) SetSymbolSize 1.0
           
           PathPlanningSetSource $fid $pid
       }
       
       # MainUpdateMRML
       # just update the models, don't care about anything else
       MainModelsUpdateMRML

       # don't need to call this twice
       if {$::Module(verbose)} {
           puts "NeuroendoscopyAutoSelectSourceSink: skipping first render call"
       }
       # Render3D
       
       set Select(cellId) $maxId
       
       set cell [$polyData GetCell $maxId]
       set points [$cell GetPoints]
       set point2 [$points GetPoint 1]
       
       set x [lindex $point2 0]
       set y [lindex $point2 1]
       set z [lindex $point2 2]
       
       set pid [FiducialsCreatePointFromWorldXYZ "default" $x $y $z $list]
       if { $pid != "" } {
           set fid $Fiducials($Fiducials(activeList),fid)
           PathPlanningSetSink $fid $pid
       }
       
       MainUpdateMRML
       Render3D
       
   }
}


#=========================================================================
#  Part 11  Procedures related to displaying the flattened colon
#=========================================================================

#-------------------------------------------------------------------------------
# .PROC  NeuroendoscopySetFlatFileName
# Set file name from Neuroendoscopy(FlatSelect)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetFlatFileName {} {
    global Neuroendoscopy

    # do nothing if the user cancelled in the browser box
    if {$Neuroendoscopy(FlatSelect) == ""} {
        set Neuroendoscopy(flatColon,name) ""
        set Neuroendoscopy(FlatSelect) ""
        
        return
    }

    # name the flattened view based on the entered file name.
    set Neuroendoscopy(flatColon,filePath) [file root $Neuroendoscopy(FlatSelect)]
    #puts "file path is $root"
    set Neuroendoscopy(flatColon,name) [file root [file tail $Neuroendoscopy(FlatSelect)]]
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCancelFlatFile
# Remove file name and reset variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCancelFlatFile {} {
   global Neuroendoscopy

   set Neuroendoscopy(FlatSelect) ""
   set Neuroendoscopy(flatColon,name) ""
    
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyAddFlatView
# Open a separate render window to display the flattened colon.<br>
# Simple Gui are also written here with parameters tailored to the size of the flatttened colon.<br>
# Bindings are pushed here.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyAddFlatView {} {
    global Gui Neuroendoscopy View viewWin MainViewer Slice Module

    # deny if the user clicks 'Choose' when the selection box is empty
    if {$Neuroendoscopy(FlatSelect) == ""} {
        DevWarningWindow "Please select a file to display."
        return
    }

    set name $Neuroendoscopy(flatColon,name)

    # deny if user tries to display an already displayed file
    if {[lsearch -exact $Neuroendoscopy(FlatWindows) $name] != -1} {
        DevWarningWindow "Please select a file that is not already displayed."
        return
    }
    
    # add the new window to the list of open windows
    lappend Neuroendoscopy(FlatWindows) $name
    
    # package require vtkinteraction
    
    # create new toplevel for the flattened image
    toplevel .t$name -visual best
    wm title .t$name $name
    wm geometry .t$name -0+0
    wm protocol .t$name WM_DELETE_WINDOW "NeuroendoscopyRemoveFlatView $name"
    wm withdraw .
 
    # view frame
    frame .t$name.fView
    set f .t$name.fView    

    # create a vtkTkRenderWidget to draw the flattened image into
    vtkRenderWindow Neuroendoscopy(flatColon,renWin,$name)
    set Neuroendoscopy(flatColon,renWidget,$name) [vtkTkRenderWidget $f.flatRenderWidget$name -width 600 -height $View(viewerHeightNormal) -rw Neuroendoscopy(flatColon,renWin,$name)]
    
    set Neuroendoscopy($f.flatRenderWidget$name,name) $name
    
    # control frames
    frame .t$name.controls
   
    # x frame: Scroll, Stop, Reset, Progress, Speed, and Close Flat Colon
    set xfrm [frame .t$name.controls.xfrm]
    
    set playfrm [frame .t$name.controls.xfrm.playfrm]
    set scrLbut [button $playfrm.scrLbut -text "<<" -font {helvetica 10 bold} \
    -command "NeuroendoscopyScrollLeftFlatColon $f.flatRenderWidget$name"]
    set stpbut [button $playfrm.stpbut -text "Stop" -font {helvetica 10 bold} \
    -command "NeuroendoscopyStopFlatColon"]
    set scrRbut [button $playfrm.scrRbut -text ">>" -font {helvetica 10 bold} \
    -command "NeuroendoscopyScrollRightFlatColon $f.flatRenderWidget$name"]
    set resbut [button $playfrm.resbut -text "Reset" -font {helvetica 10 bold} \
    -command "NeuroendoscopyResetFlatColon $f.flatRenderWidget$name"]
    pack $scrLbut -side left -padx 2 -expand yes -fill x
    pack $stpbut -side left -padx 2 -expand yes -fill x
    pack $scrRbut -side left -padx 2 -expand yes -fill x
    pack $resbut -side left -padx 2 -expand yes -fill x
    
    
    set progfrm [frame .t$name.controls.xfrm.progfrm]
    set proglbl [label $progfrm.lbl -text "Progress:  " -font {helvetica 10 bold} ]
    set Neuroendoscopy(flatScale,progress) [scale $progfrm.progress -from 0 -to 250 -res 0.5 -orient horizontal \
                                     -variable Neuroendoscopy(flatColon,xCamDist)  -bg $Neuroendoscopy(path,sColor) ]
    pack $proglbl -side left
    pack $Neuroendoscopy(flatScale,progress) -side right
    
    
    set speedfrm [frame .t$name.controls.xfrm.speedfrm]
    set speedlbl [label $speedfrm.lbl -text "Speed:      " -font {helvetica 10 bold} ]
    set Neuroendoscopy(flatScale,speed) [scale $speedfrm.speed -from 0.1 -to 5 -res 0.1 -orient horizontal \
                                     -variable Neuroendoscopy(flatColon,speed)  -bg $Neuroendoscopy(path,sColor) ]
    pack $speedlbl -side left
    pack $Neuroendoscopy(flatScale,speed) -side right
    
    
    set updatefrm [frame .t$name.controls.xfrm.updatefrm]
    set updatebut [button $updatefrm.updatebut -text "Update Targets on Flat Colon" -font {helvetica 10 bold} \
    -command "NeuroendoscopyUpdateTargetsInFlatWindow $f.flatRenderWidget$name"]
    pack $updatebut -side left -padx 2 -expand yes


    #pack xfrm
    pack $playfrm -side top -pady 4 -expand yes
    pack $progfrm -side top -pady 4 -expand yes
    pack $speedfrm -side top -pady 4 -expand yes
    pack $updatefrm -side top -pady 4 -expand yes
    
 
    
    # y frame: Zoom, Center Viewpoint, and Close Flat Colon
    set yfrm [frame .t$name.controls.yfrm]
    
    set zoomfrm [frame .t$name.controls.yfrm.zoomfrm]        
    set camZoomlbl [label $zoomfrm.lbl -text "Zoom In<-->Zoom Out" -font {helvetica 10}]
    set Neuroendoscopy(flatScale,camZoom) [scale $zoomfrm.camZoom -from 0 -to 100 -res 0.5 -orient horizontal  -variable Neuroendoscopy(flatColon,zCamDist)]
    pack $camZoomlbl -side top
    pack $Neuroendoscopy(flatScale,camZoom) -side top
    
      
    set optzfrm [frame $yfrm.optzfrm]
    set optzbut [button $optzfrm.optzbut -text "Center Viewpoint" -font {helvetica 10 bold} \
    -command "NeuroendoscopyResetFlatCameraDist $f.flatRenderWidget$name"]
    pack $optzbut -side top -pady 4 -expand yes -fill x
    
    
    set scalarOnOfffrm [frame $yfrm.scalarOnOfffrm]
    set scaOnOfflbl [label $scalarOnOfffrm.lbl -text "Curvature Analysis:" -font {helvetica 10 bold}]
    pack $scaOnOfflbl -side left -padx 2
    
    foreach text "{On} {Off}" \
            value "1 0" \
            width "4 4" {
        eval {radiobutton $scalarOnOfffrm.$value -width $width \
            -font {helvetica 10 bold}\
                -text "$text" -value "$value" -variable Neuroendoscopy(flatColon,scalarVisibility) \
                -command "NeuroendoscopySetFlatColonScalarVisibility $f.flatRenderWidget$name" \
                -indicatoron 0}
        pack $scalarOnOfffrm.$value -side left -padx 2
    }

    
    
    set scalarRangefrm [frame $yfrm.scalarRangefrm]
    set scaRangelbl [label $scalarRangefrm.lbl -text "Scalar Range:" -font {helvetica 10 bold}]
    eval {entry $scalarRangefrm.eLo -width 6 -textvariable Neuroendoscopy(flatColon,scalarLow) }
    bind $scalarRangefrm.eLo <Return> "NeuroendoscopySetFlatColonScalarRange $f.flatRenderWidget$name"
    bind $scalarRangefrm.eLo <FocusOut> "NeuroendoscopySetFlatColonScalarRange $f.flatRenderWidget$name"
    eval {entry $scalarRangefrm.eHi -width 6 -textvariable Neuroendoscopy(flatColon,scalarHigh) }
    bind $scalarRangefrm.eHi <Return> "NeuroendoscopySetFlatColonScalarRange $f.flatRenderWidget$name"
    bind $scalarRangefrm.eHi <FocusOut> "NeuroendoscopySetFlatColonScalarRange $f.flatRenderWidget$name"
    pack $scaRangelbl $scalarRangefrm.eLo $scalarRangefrm.eHi -side left -padx 2

    


    #pack yfrm
    pack $zoomfrm -side top -pady 2 -expand yes
    pack $optzfrm -side top -pady 2 -expand yes
    pack $scalarOnOfffrm -side top -pady 2 -expand yes
    pack $scalarRangefrm -side top -pady 2 -expand yes
   

    # the following scale is left un-packed, because it will be removed eventually
    set udlbl [label $yfrm.lbl -text "Up/Down" -font {helvetica 10} ]
    set Neuroendoscopy(flatScale,panud) [scale $yfrm.updown -from -10 -to 10 -res 0.5 -orient vertical -variable Neuroendoscopy(flatColon,yCamDist)]
    #pack $udlbl $Neuroendoscopy(flatScale,panud) -side top -pady 4 -expand yes

   

    # light frame: change the light positions, elevation and azimuth
    set lfrm [frame .t$name.controls.lfrm]
    
    set elefrm [frame .t$name.controls.lfrm.elefrm]
    set elelbl [label $elefrm.lbl -text "-90<- Elevation ->90" -font {helvetica 10}]

    set Neuroendoscopy(flatScale,elevation) [scale $elefrm.elevation -from -90 -to 90 -res 5.0 -orient horizontal  -variable Neuroendoscopy(flatColon,LightElev) ]
    pack $elelbl -side top
    pack $Neuroendoscopy(flatScale,elevation) -side top
    
 
    set azifrm [frame .t$name.controls.lfrm.azifrm]        
    set azilbl [label $azifrm.lbl -text "-90<- Azimuth ->90" -font {helvetica 10}]
    set Neuroendoscopy(flatScale,azimuth) [scale $azifrm.azimuth -from -90 -to 90 -res 5.0 -orient horizontal  -variable Neuroendoscopy(flatColon,LightAzi)]
    pack $azilbl -side top
    pack $Neuroendoscopy(flatScale,azimuth) -side top
    
    
    set quitfrm [frame .t$name.controls.lfrm.quitfrm]
    set quitbut [button $quitfrm.quitbut -text "Close Flat Colon" -font {helvetica 10 bold} \
    -command "NeuroendoscopyRemoveFlatView $name"]
    pack $quitbut -side bottom -pady 4 -expand yes -fill x
    
    
    #the following is left unpacked, since the test function will be removed eventually
    #set coordfrm [frame .t$name.controls.lfrm.coordfrm]
    #set coordlbl [label $coordfrm.lbl -text "x: 0 y: "]
    #pack $coordlbl -side top
    #pack $coordfrm -side top -pady 4 -expand yes


    pack $elefrm -side top -pady 2 -expand yes
    pack $azifrm -side top -pady 2 -expand yes
    pack $quitfrm -side top -pady 2 -expand yes
    
    
    # pack control frames
    pack .t$name.controls.xfrm -side left  -padx 1 -anchor n -expand yes
    pack .t$name.controls.yfrm -side left -padx 1 -anchor n -expand yes
    pack .t$name.controls.lfrm -side right -padx 1 -anchor n -expand yes
    
    # pack render widget and controls    
    pack $f.flatRenderWidget$name -side left -padx 3 -pady 3 -fill both -expand t
    pack $f -fill both -expand t
    pack .t$name.controls -pady 4 -fill both -expand t

   
    # add a vtkRenderer to the vtkRenderWindow   
    vtkRenderer Neuroendoscopy($name,renderer)
    Neuroendoscopy($name,renderer) SetBackground 0.5 0.5 0.5
    Neuroendoscopy(flatColon,renWin,$name) AddRenderer Neuroendoscopy($name,renderer)
    
    lappend $Neuroendoscopy(FlatRenderers) Neuroendoscopy($name,renderer)
    
    #activate interactor
#    vtkRenderWindowInteractor iren
#    iren SetRenderWindow [$f.flatRenderWidget$name GetRenderWindow]


    # set and activate event bindings for this widget
     NeuroendoscopyCreateFlatBindings $f.flatRenderWidget$name
     
    # create lookup table for mapping scalars representing the curvature analysis
     NeuroendoscopyBuildFlatColonLookupTable $name
    
    # Update targets (Fiducials) that were saved in the MRML Tree
 #   $updatebut config -command "NeuroendoscopyUpdateTargetsInFlatWindow $f.flatRenderWidget$name"

    # create a vtkPolyDataReader
    vtkPolyDataReader TempPolyReader   
    TempPolyReader SetFileName $Neuroendoscopy(FlatSelect)

    # create a vtkPolyDataMapper
    vtkPolyDataMapper Neuroendoscopy($name,FlatColonMapper)
    Neuroendoscopy($name,FlatColonMapper) SetInput [TempPolyReader GetOutput]
#    Neuroendoscopy($name,FlatColonMapper) ScalarVisibilityOn
#    Neuroendoscopy($name,FlatColonMapper) SetScalarRange 0 100
    Neuroendoscopy($name,FlatColonMapper) ScalarVisibilityOff
#    Neuroendoscopy($name,FlatColonMapper) SetScalarMaterialModeToAmbientAndDiffuse
        
    # save the polydata where we can find it later
    set Neuroendoscopy($name,polyData) [TempPolyReader GetOutput]
    $Neuroendoscopy($name,polyData) UpdateData
      
    # create a vtkActor for the flatcolon, and set its properties
    vtkActor Neuroendoscopy($name,FlatColonActor)
   [Neuroendoscopy($name,FlatColonActor) GetProperty] SetInterpolationToPhong
   [Neuroendoscopy($name,FlatColonActor) GetProperty] SetColor 1.0 0.8 0.7
   [Neuroendoscopy($name,FlatColonActor) GetProperty] SetAmbient 0.55
   [Neuroendoscopy($name,FlatColonActor) GetProperty] SetDiffuse 0.4
   [Neuroendoscopy($name,FlatColonActor) GetProperty] SetSpecular 0.5
   [Neuroendoscopy($name,FlatColonActor) GetProperty] SetSpecularPower 100
   
   [Neuroendoscopy($name,FlatColonActor) GetProperty] BackfaceCullingOff
    
    Neuroendoscopy($name,FlatColonActor) SetMapper Neuroendoscopy($name,FlatColonMapper)
    Neuroendoscopy($name,renderer) AddActor Neuroendoscopy($name,FlatColonActor)
    
    # create 2 vtkActors for the boundary of the flatcolon, since the flat colon we loaded above has overlap
    NeuroendoscopyBuildFlatBoundary $name
#    Neuroendoscopy($name,renderer) AddActor Neuroendoscopy(flatColon,$name,aPolyLineActor1)
    
    
    #create Outline
    vtkOutlineFilter colonOutline
      colonOutline SetInput [TempPolyReader GetOutput]
    vtkPolyDataMapper outlineMapper
      outlineMapper SetInput [colonOutline GetOutput]
    vtkActor Neuroendoscopy($name,outlineActor)
      Neuroendoscopy($name,outlineActor) SetMapper outlineMapper
      
  #    Neuroendoscopy($name,outlineActor) SetScale 1.0 1. 1.0
  #    Neuroendoscopy($name,outlineActor) SetPosition 0.0 0.0 0.0
      
      [Neuroendoscopy($name,outlineActor) GetProperty] SetColor 0 0 0
    Neuroendoscopy($name,renderer) AddActor Neuroendoscopy($name,outlineActor)
    
    set size [Neuroendoscopy($name,renderer) GetSize]
        
    set outline [$Neuroendoscopy($name,polyData) GetBounds]
    set Neuroendoscopy(flatColon,xMin) [lindex $outline 0]
    set Neuroendoscopy(flatColon,xMax) [lindex $outline 1]
    set Neuroendoscopy(flatColon,yMin) [lindex $outline 2]
    set Neuroendoscopy(flatColon,yMax) [lindex $outline 3]
    set Neuroendoscopy(flatColon,zMin) [lindex $outline 4]
    set Neuroendoscopy(flatColon,zMax) [lindex $outline 5]  

    # normalize the initial camera position   
    set y [expr $Neuroendoscopy(flatColon,yMax) - $Neuroendoscopy(flatColon,yMin)]
    set y [expr abs($y)]
    set Neuroendoscopy(flatColon,zOpt) [expr [expr 5*$y]/8]
    
    set Neuroendoscopy(flatColon,yMid) [expr [expr $Neuroendoscopy(flatColon,yMin) + $Neuroendoscopy(flatColon,yMax)]/2]

    # re-config the scale according to the size of the flat colon        
    $Neuroendoscopy(flatScale,progress) config -from [expr [expr floor($Neuroendoscopy(flatColon,xMin))]-1] -to [expr ceil($Neuroendoscopy(flatColon,xMax))]
    $Neuroendoscopy(flatScale,panud) config -from [expr [expr ceil($Neuroendoscopy(flatColon,yMin))]-2] -to [expr [expr ceil($Neuroendoscopy(flatColon,yMax))]+2]
    $Neuroendoscopy(flatScale,camZoom) config -from 0 -to [expr [expr ceil($Neuroendoscopy(flatColon,zOpt))] + 300]

    # set initial camera position   
    set Neuroendoscopy(flatColon,xCamDist) [expr [expr floor($Neuroendoscopy(flatColon,xMin))]-1]
    $Neuroendoscopy(flatScale,progress) set $Neuroendoscopy(flatColon,xCamDist)
    
    set Neuroendoscopy(flatColon,yCamDist) $Neuroendoscopy(flatColon,yMid)
    $Neuroendoscopy(flatScale,panud) set $Neuroendoscopy(flatColon,yCamDist)
    
    set Neuroendoscopy(flatColon,zCamDist) $Neuroendoscopy(flatColon,zOpt) 
    $Neuroendoscopy(flatScale,camZoom) set $Neuroendoscopy(flatColon,zCamDist)
    
    set Neuroendoscopy($name,camera) [Neuroendoscopy($name,renderer) GetActiveCamera]
    $Neuroendoscopy($name,camera) SetClippingRange 0.01 4000
    $Neuroendoscopy($name,camera) SetFocalPoint $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) $Neuroendoscopy(flatColon,zMin)
    $Neuroendoscopy($name,camera) SetPosition $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) $Neuroendoscopy(flatColon,zCamDist)
    $Neuroendoscopy($name,camera) SetViewAngle 90
    # $Navigator($name,camera) Zoom 1
  
    # add command for moving the colon in x y directions and zooming.  
    $Neuroendoscopy(flatScale,progress) config -command "NeuroendoscopyMoveCameraX $f.flatRenderWidget$name"
    $Neuroendoscopy(flatScale,panud) config -command "NeuroendoscopyMoveCameraY $f.flatRenderWidget$name" 
    $Neuroendoscopy(flatScale,camZoom) config -command "NeuroendoscopyMoveCameraZ $f.flatRenderWidget$name"
    
     # lightKit
     
     set Neuroendoscopy(flatColon,LightElev) 0
     set Neuroendoscopy(flatColon,LightAzi) -45

     vtkLightKit Neuroendoscopy($name,lightKit)
     Neuroendoscopy($name,lightKit) SetKeyLightWarmth 0.5
     Neuroendoscopy($name,lightKit) SetHeadlightWarmth 0.5
     Neuroendoscopy($name,lightKit) SetKeyLightIntensity 0.7
     Neuroendoscopy($name,lightKit) SetKeyLightElevation $Neuroendoscopy(flatColon,LightElev)
     Neuroendoscopy($name,lightKit) SetKeyLightAzimuth $Neuroendoscopy(flatColon,LightAzi)
#     Neuroendoscopy($name,lightKit) SetKeyLightAngle 30 -30
     Neuroendoscopy($name,lightKit) SetKeyToFillRatio 5
     Neuroendoscopy($name,lightKit) AddLightsToRenderer Neuroendoscopy($name,renderer)
     
     #light
#     vtkLight Neuroendoscopy($name,light)
#     Neuroendoscopy($name,light) SetIntensity 0.3
#     Neuroendoscopy($name,light) SetPosition [expr $Neuroendoscopy(flatColon,xCamDist)-10] $Neuroendoscopy(flatColon,yCamDist) $Neuroendoscopy(flatColon,zCamDist)
#     Neuroendoscopy($name,light) SetFocalPoint $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) $Neuroendoscopy(flatColon,zMin)
#     Neuroendoscopy($name,light) SetDirectionAngle 0 0
#     Neuroendoscopy($name,renderer) AddLight Neuroendoscopy($name,light)
     
     # add command for changing the light's elevation and azimuth
     $Neuroendoscopy(flatScale,elevation) config -command "NeuroendoscopyFlatLightElevationAzimuth $f.flatRenderWidget$name"
     $Neuroendoscopy(flatScale,azimuth) config -command "NeuroendoscopyFlatLightElevationAzimuth $f.flatRenderWidget$name"
  
    # initialize and reinitialize
    set Neuroendoscopy($name,lineCount) 0
    #set Neuroendoscopy(FlatSelect) ""
    
    set Neuroendoscopy(flatColon,speed) 3.0

    #Render
    #[$f.flatRenderWidget$name GetRenderWindow] Render  
    Neuroendoscopy(flatColon,renWin,$name) Render

    TempPolyReader Delete
#    TempMapper Delete
    colonOutline Delete
    outlineMapper Delete

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyRemoveFlatView
# Remove the flattened colon window and everything related to it.<br>
# Pop Bindings here
# .ARGS
# string name defaults to empty string
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyRemoveFlatView {{name ""}} {
    global Module Neuroendoscopy View

    # if clicked 'Quit' button
    if {$name != ""} {
    
        set f .t$name.fView
    
        # destroy all parts of the flat view
        #puts "window close $name"    
        Neuroendoscopy($name,FlatColonActor) Delete
        Neuroendoscopy($name,FlatColonMapper) Delete
        
        Neuroendoscopy(flatColon,lookupTable,$name) Delete
        
        # actor for flattened image
        Neuroendoscopy($name,outlineActor) Delete
        Neuroendoscopy($name,lightKit) Delete
        #    Neuroendoscopy($name,light) Delete
        
        Neuroendoscopy(flatColon,$name,Line1Actor) Delete
        Neuroendoscopy(flatColon,$name,Line2Actor) Delete
        
    
        for {set linecount 0} {$linecount < $Neuroendoscopy($name,lineCount)} {incr linecount} {
            Neuroendoscopy($name,aLineActor,$linecount) Delete
        }
        set $Neuroendoscopy($name,lineCount) 0
    
        set index [lsearch -exact $Neuroendoscopy(FlatWindows) $name]
        set Neuroendoscopy(FlatWindows) [lreplace $Neuroendoscopy(FlatWindows) $index $index]
        set Neuroendoscopy(FlatRenderers) [lreplace $Neuroendoscopy(FlatRenderers) $index $index]
        
        Neuroendoscopy($name,renderer) Delete
        Neuroendoscopy(flatColon,renWin,$name) Delete
        # Neuroendoscopy(flatColon,renWidget,$name) Delete
        
        
        destroy .t$name
        
    } else {
        
        #        if ending all
        
        foreach frame $Neuroendoscopy(FlatWindows) {
            destroy .t$frame
            #puts "frame close $frame"      
            Neuroendoscopy($frame,renderer) Delete
            Neuroendoscopy($frame,FlatColonActor) Delete
            Neuroendoscopy($frame,outlineActor) Delete
            Neuroendoscopy($frame,lightKit) Delete
            Neuroendoscopy($frame,light) Delete
        }
        
        set index [lsearch -exact $Neuroendoscopy(FlatWindows) $name]
        set Neuroendoscopy(FlatWindows) [lreplace $Neuroendoscopy(FlatWindows) $index $index]
        set Neuroendoscopy(FlatRenderers) [lreplace $Neuroendoscopy(FlatRenderers) $index $index]
        
    }
    
   
    set Neuroendoscopy(FlatSelect) ""
    set Neuroendoscopy(flatColon,name) ""
    
    set Neuroendoscopy(flatColon,scalarVisibility) 0
    set Neuroendoscopy(flatColon,scalarLow) 0    
    set Neuroendoscopy(flatColon,scalarHigh) 100
        
    # de-activate bindings for the flat window   
    NeuroendoscopyPopFlatBindings
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateFlatBindings
# Create and Activate binding sets that allow the user to double click at a location on the flattened colon,
# move the endoscope to the corresponding location in 3D, and sychronize the 2D slices as well
# .ARGS
# windowelement widget the window to add the bindings to
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreateFlatBindings {widget} {
    global Neuroendoscopy Ev

 # the following 2 lines are for testing the bindings the old fashioned way.
 #    bind $widget <ButtonRelease-1> {puts Button}
 #    bind $widget <KeyPress-c> {NeuroendoscopyPickFlatPoint %W %x %y}
 
# FlatWindow Expose Pan and Zoom bindings
    EvDeclareEventHandler FlatWindowExpose <Expose> {%W Render}
    EvDeclareEventHandler FlatWindowStartPan <ButtonPress-2> {NeuroendoscopyStartPan %W %x %y}
    EvDeclareEventHandler FlatWindowB2Motion <B2-Motion> {NeuroendoscopyEndPan %W %x %y}
    EvDeclareEventHandler FlatWindowEndPan <ButtonRelease-2> {NeuroendoscopyEndPan %W %x %y}
    EvDeclareEventHandler FlatWindowStartZoom <ButtonPress-3> {NeuroendoscopyStartZoom %W %y}
    EvDeclareEventHandler FlatWindowB3Motion <B3-Motion> {NeuroendoscopyEndZoom %W %y}
    EvDeclareEventHandler FlatWindowEndZoom <ButtonRelease-3> {NeuroendoscopyEndZoom %W %y}

#FlatWindow Pick target binding    
    EvDeclareEventHandler FlatWindowEvents <Shift-Double-1> {NeuroendoscopyPickFlatPoint %W %x %y}

#test binding to track mouse position
#    EvDeclareEventHandler FlatMotionEvents <Motion> {NeuroendoscopyMouseLocation %W %x %y}

    #add various events to the binding set for the flat window
    EvAddWidgetToBindingSet bindFlatWindowEvents $widget {{FlatWindowEvents} {FlatWindowExpose} \
    {FlatWindowStartPan} {FlatWindowB2Motion} {FlatWindowEndPan}}
    
# the zoom function with B3 is temporarily disabled, pending feed back from user
# to activate: append the following 3 bindings to  EvAddWidgetToBindingSet  
#     {FlatWindowStartZoom} {FlatWindowB3Motion} {FlatWindowEndZoom}

    #activate the binding set for the flat window
    EvActivateBindingSet bindFlatWindowEvents
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyPopFlatBindings
# Deactivate binding sets when the user closes the flat colon view.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyPopFlatBindings {} {
    global Ev Csys Neuroendoscopy
   
    EvDeactivateBindingSet bindFlatWindowEvents
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyBuildFlatColonLookupTable
# Builds a colour look up talbe for the flat colon view
# .ARGS
# string name partial name for the colour table, defaults to empty string
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyBuildFlatColonLookupTable {{name ""}} {
    global Neuroendoscopy
     
    vtkLookupTable Neuroendoscopy(flatColon,lookupTable,$name)
    #actual values are from 0 to 255
    Neuroendoscopy(flatColon,lookupTable,$name) SetNumberOfColors 256
    Neuroendoscopy(flatColon,lookupTable,$name) Build
     
    set colon 64
    set polyp 64
    set transL $colon
    set transH [expr [expr 256 - $polyp] + 1]
    
    # for the first 64 slots in the table, set the color to be the same as the colon (skin rgb: 1.0 0.8 0.7)
    for {set i 0} {$i < $colon} {incr i} {
        eval Neuroendoscopy(flatColon,lookupTable,$name) SetTableValue $i 1.0 0.8 0.7 1
    }
    
    # for the next 128 slots in the table, set the color to be in transition from skin to green
    # i.e., green value increases to 1, and both the red and the blue values decrease to 0
    for {set i $transL} {$i < $transH} {incr i} {
        
        set numSteps [expr [expr $transH - $transL] + 1]
        
        set redDecr [expr 1.0 / $numSteps]
        set greenIncr [expr [expr 1.0 - 0.8] / $numSteps]
        set blueDecr [expr 0.7 / $numSteps]
        
        set red [expr 1.0 - [expr [expr $i - $transL] * $redDecr]]
        set green [expr 0.8 + [expr [expr $i - $transL] * $greenIncr]]
        set blue [expr 0.7 - [expr [expr $i - $transL] * $blueDecr]]
        
        eval Neuroendoscopy(flatColon,lookupTable,$name) SetTableValue $i $red $green $blue 0.9
    }
    
    # for the last 64 slots in the table, set the color to be green (rgb: 0 1 0)
    for {set i $transH} {$i < 256} {incr i} {
        eval Neuroendoscopy(flatColon,lookupTable,$name) SetTableValue $i 0.0 1.0 0.0 1
    }   
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyBuildFlatBoundary
# Create the boundary line around the flat view.
# .ARGS
# string name name of the line actor, defaults to empty string
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyBuildFlatBoundary {{name ""}} {

     global Neuroendoscopy
     
     set line1name $Neuroendoscopy(flatColon,filePath)
     set line2name $Neuroendoscopy(flatColon,filePath)
     append line1name _Line1.txt
     append line2name _Line2.txt
     # Create the 1st boundary line     
     set fp1 [open $line1name r]
     set data1 [read $fp1]
     set line1s [split [string trim $data1] "\n"]
     
     set line1numP [lindex $line1s 0]
#     puts "line1 number of points: $line1numP"
     
     vtkPoints Line1Points
     Line1Points SetNumberOfPoints $line1numP
     for {set i 1} {$i <= $line1numP} {incr i} {
         set point [lindex $line1s $i]
     
         set x [lindex $point 0]
         set y [lindex $point 1]
         set z [lindex $point 2]
         
         Line1Points InsertPoint $i $x $y $z
     }
     
     vtkCellArray Line1Cells
     Line1Cells InsertNextCell $line1numP
     
     for {set i 1} {$i <= $line1numP} {incr i} {
         Line1Cells InsertCellPoint $i
     }
     
     vtkPolyData Line1
     Line1 SetPoints Line1Points
     Line1 SetLines Line1Cells
     
     
     vtkPolyDataMapper Line1Mapper
     Line1Mapper SetInput Line1
     
     vtkActor Neuroendoscopy(flatColon,$name,Line1Actor)
     Neuroendoscopy(flatColon,$name,Line1Actor) SetMapper Line1Mapper
     
     [Neuroendoscopy(flatColon,$name,Line1Actor) GetProperty] SetColor 0 0 0
     
     Neuroendoscopy($name,renderer) AddActor Neuroendoscopy(flatColon,$name,Line1Actor)
     
     Line1Points Delete
     Line1Cells Delete
     Line1 Delete
     Line1Mapper Delete
     
     # Create the 2nd boundary line
     
     set fp2 [open $line2name r]
     set data2 [read $fp2]
     set line2s [split [string trim $data2] "\n"]
     
     set line2numP [lindex $line2s 0]
     #     puts "line2 number of points: $line2numP"
     
     vtkPoints Line2Points
     Line2Points SetNumberOfPoints $line2numP
     for {set i 1} {$i <= $line2numP} {incr i} {
         set point [lindex $line2s $i]
         
         set x [lindex $point 0]
         set y [lindex $point 1]
         set z [lindex $point 2]
         
         Line2Points InsertPoint $i $x $y $z
     }
     
     vtkCellArray Line2Cells
     Line2Cells InsertNextCell $line2numP
     
     for {set i 1} {$i <= $line2numP} {incr i} {
         Line2Cells InsertCellPoint $i
     }
     
     vtkPolyData Line2
     Line2 SetPoints Line2Points
     Line2 SetLines Line2Cells
     
     
     vtkPolyDataMapper Line2Mapper
     Line2Mapper SetInput Line2
     
     vtkActor Neuroendoscopy(flatColon,$name,Line2Actor)
     Neuroendoscopy(flatColon,$name,Line2Actor) SetMapper Line2Mapper
     
     [Neuroendoscopy(flatColon,$name,Line2Actor) GetProperty] SetColor 0 0 0
     
     Neuroendoscopy($name,renderer) AddActor Neuroendoscopy(flatColon,$name,Line2Actor)
     
     Line2Points Delete
     Line2Cells Delete
     Line2 Delete
     Line2Mapper Delete     
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyMouseLocation
# Test proc to track mouse location in the flat window
# .ARGS
# windowelement widget the flat window
# int xcoord mouse location in x
# int ycoord mouse location in y
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyMouseLocation {widget xcoord ycoord} {
    global Neuroendoscopy
    
    set name $Neuroendoscopy($widget,name)
    .t$name.controls.lfrm.coordfrm.lbl configure -text "x: $xcoord  y: $ycoord"

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyStartPan
# Set the x and y start points for panning.
# .ARGS
# windowelement widget flat window
# int xcoord start panning here in x
# int ycoord start panning here in y
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyStartPan {widget xcoord ycoord} {
     global Neuroendoscopy

    set name $Neuroendoscopy($widget,name)

    set Neuroendoscopy($name,pan,xstart) $xcoord
    set Neuroendoscopy($name,pan,ystart) $ycoord     
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyEndPan
# End panning through the flat view
# .ARGS
# windowelement widget flat window
# int xcoord x point for panning
# int ycoord y point for panning
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyEndPan {widget xcoord ycoord} {
     global Neuroendoscopy
     
    set name $Neuroendoscopy($widget,name)

    set xstart $Neuroendoscopy($name,pan,xstart)
    set ystart $Neuroendoscopy($name,pan,ystart)

    set dx [expr $xcoord - $xstart]
    set dy [expr $ycoord - $ystart]

    set position [$Neuroendoscopy($name,camera) GetPosition]
    set x1 [lindex $position 0]
    set y1 [lindex $position 1]
    set z1 [lindex $position 2]

    set x2 [expr $x1 - [expr $dx * .01]]
    set y2 [expr $y1 + [expr $dy * .01]]
    
    if {$x2 < [expr ceil($Neuroendoscopy(flatColon,xMax))] } {
    
        $Neuroendoscopy($name,camera) SetFocalPoint $x2 $y2 0
        $Neuroendoscopy($name,camera) SetPosition $x2 $y2 $z1
    
        set Neuroendoscopy(flatColon,xCamDist) $x2
        set Neuroendoscopy(flatColon,yCamDist) $y2
        
        [$widget GetRenderWindow] Render
    
    } elseif {$x2 < [expr [expr floor($Neuroendoscopy(flatColon,xMin))]-0.5] } {
        set Neuroendoscopy(flatColon,xCamDist) [expr floor($Neuroendoscopy(flatColon,xMin))]
    } else {
        set Neuroendoscopy(flatColon,xCamDist) [expr ceil($Neuroendoscopy(flatColon,xMax))]
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyStartZoom
# Set the zoom y start position.
# .ARGS
# windowelement widget flat window
# int ycoord zoom's y start position
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyStartZoom {widget ycoord} {
     global Neuroendoscopy
    
    set name $Neuroendoscopy($widget,name)
    set Neuroendoscopy($name,zoom,ystart) $ycoord     
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyEndZoom
# End zooming in flat window.
# .ARGS
# windowelement widget flat window
# int ycoord zoom y start position
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyEndZoom {widget ycoord} {
    global Neuroendoscopy

    set name $Neuroendoscopy($widget,name)

    set ystart $Neuroendoscopy($name,zoom,ystart)
    set dy [expr $ycoord - $ystart]
    
    # log base b of x = log(x) / log(b)
    set b      1.02
    set zPrev  1.5
    set dyPrev [expr log($zPrev) / log($b)]

    set zoom [expr pow($b, ($dy + $dyPrev))]
    if {$zoom < 0.01} {
        set zoom 0.01
    }
    set z [format "%.2f" $zoom]

    $Neuroendoscopy($name,camera) Zoom $z
    
    set position [$Neuroendoscopy($name,camera) GetPosition]
    set Neuroendoscopy(flatColon,zCamDist) [lindex $position 2]
    
    [$widget GetRenderWindow] Render     
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyPickFlatPoint
# Use vtkCellPicker to identify the cell Id and location of interest.
# .ARGS
# windowelement widget flat window
# int xcoord picked x location
# int ycoord picked y location
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyPickFlatPoint {widget xcoord ycoord} {
    global Select Neuroendoscopy Model Fiducials
    
     set model $Model(activeID)
     if {$model != ""} {
         set polyData3D $Model($model,polyData)
     }
     
    set numP [$polyData3D GetNumberOfPoints]
    
    #puts "start pick: clock format [clock seconds]"
    set name $Neuroendoscopy($widget,name)
    
    vtkCellPicker TempCellPicker
    TempCellPicker SetTolerance 0.001
    
    
    #get the target coordinates in the flat window to draw a crosshair
    if {[SelectPick TempCellPicker $widget $xcoord $ycoord] != 0} {
    
        set fx [lindex $Select(xyz) 0]
        set fy [lindex $Select(xyz) 1]
        set fz [lindex $Select(xyz) 2]
        
        #puts "end pick : clock format [clock seconds]"
        
        
        #get the picked pointId from the picker, and pass the pointId to the 3D model in slicer
        
        set name $Neuroendoscopy(flatColon,name)  
        set polyData $Neuroendoscopy($name,polyData)
        
        #reduce point number by half
        #    set numP [$polyData GetNumberOfPoints]
        # puts "nump for the doubled model is: $numP"
        #    set numP  [expr $numP/2] 
        # puts "nump for the single model is: $numP"    
        #puts "start locate point: clock format [clock seconds]"     
        vtkPointLocator tempPointLocator
        tempPointLocator SetDataSet $polyData
        
        #    set x [lindex $Select(xyz) 0]
        #    set y [lindex $Select(xyz) 1]
        #    set z [lindex $Select(xyz) 2]
        
        set pointId [tempPointLocator FindClosestPoint $fx $fy $fz]
        # puts "picked pointId from the double model is $pointId"
        #puts "end locate point: clock format [clock seconds]"    
        # check if the pointId is larger than numP
        if {$pointId > $numP} {
            set pointId [expr $pointId - $numP]
        }
        
        # puts "picked pointId from the single model is $pointId"   
        
        
        NeuroendoscopyAddTargetInFlatWindow $widget $fx $fy $fz
        
        
        NeuroendoscopyAddTargetFromFlatColon $pointId
        
    } else {
        return
    }
   
    TempCellPicker Delete
    tempPointLocator Delete
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyAddTargetInFlatWindow
# Draw a vertical red line behind the flattened colon at locations of interest selected by the user.
# .ARGS
# windowelement widget flat window
# int x x of selected point
# int y y of selected point
# int z z of selected point
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyAddTargetInFlatWindow {widget x y z} {

    global Select Neuroendoscopy
#puts  "start draw in flatwindow: clock format [clock seconds]"
    set name $Neuroendoscopy($widget,name)
    set renderer [[[$widget GetRenderWindow] GetRenderers] GetItemAsObject 0]
    set count $Neuroendoscopy($name,lineCount)

# make top verticle line of the cursor
    vtkLineSource aLineT
    
    aLineT SetPoint1 $x [expr $y - 20]  [expr $z + 0.01]
    aLineT SetPoint2 $x [expr $y - 10]  [expr $z + 0.01]

    aLineT SetResolution 20
    vtkPolyDataMapper aLineTMapper
    aLineTMapper SetInput [aLineT GetOutput]

    vtkActor Neuroendoscopy($name,aLineActor,$count)
    Neuroendoscopy($name,aLineActor,$count) SetMapper aLineTMapper 
    [Neuroendoscopy($name,aLineActor,$count) GetProperty] SetColor 1 0 0    
    
    $renderer AddActor Neuroendoscopy($name,aLineActor,$count)

    set Neuroendoscopy($name,lineCount) [expr $Neuroendoscopy($name,lineCount) + 1]
    
    aLineT Delete
    aLineTMapper Delete
    
# make bottom verticle line of the cursor   
    
    set count $Neuroendoscopy($name,lineCount)
    vtkLineSource aLineB

    aLineB SetPoint1 $x [expr $y + 10]  [expr $z + 0.01]
    aLineB SetPoint2 $x [expr $y + 20]  [expr $z + 0.01]  

    aLineB SetResolution 20
    vtkPolyDataMapper aLineBMapper
    aLineBMapper SetInput [aLineB GetOutput]

    vtkActor Neuroendoscopy($name,aLineActor,$count)
    Neuroendoscopy($name,aLineActor,$count) SetMapper aLineBMapper 
    [Neuroendoscopy($name,aLineActor,$count) GetProperty] SetColor 1 0 0    
    
    $renderer AddActor Neuroendoscopy($name,aLineActor,$count)

    set Neuroendoscopy($name,lineCount) [expr $Neuroendoscopy($name,lineCount) + 1]
    
    aLineB Delete
    aLineBMapper Delete
    
# make left horizontal line of the cursor   
    
    set count $Neuroendoscopy($name,lineCount)
    vtkLineSource aLineL

    aLineL SetPoint1 [expr $x - 20] $y [expr $z + 0.01]
    aLineL SetPoint2 [expr $x - 10] $y [expr $z + 0.01]  

    aLineL SetResolution 20
    vtkPolyDataMapper aLineLMapper
    aLineLMapper SetInput [aLineL GetOutput]

    vtkActor Neuroendoscopy($name,aLineActor,$count)
    Neuroendoscopy($name,aLineActor,$count) SetMapper aLineLMapper 
    [Neuroendoscopy($name,aLineActor,$count) GetProperty] SetColor 1 0 0    
    
    $renderer AddActor Neuroendoscopy($name,aLineActor,$count)

    set Neuroendoscopy($name,lineCount) [expr $Neuroendoscopy($name,lineCount) + 1]
    
    aLineL Delete
    aLineLMapper Delete

# make right horizontal line of the cursor   
    
    set count $Neuroendoscopy($name,lineCount)
    vtkLineSource aLineR

    aLineR SetPoint1 [expr $x + 10] $y [expr $z + 0.01]
    aLineR SetPoint2 [expr $x + 20] $y [expr $z + 0.01]  

    aLineR SetResolution 20
    vtkPolyDataMapper aLineRMapper
    aLineRMapper SetInput [aLineR GetOutput]

    vtkActor Neuroendoscopy($name,aLineActor,$count)
    Neuroendoscopy($name,aLineActor,$count) SetMapper aLineRMapper 
    [Neuroendoscopy($name,aLineActor,$count) GetProperty] SetColor 1 0 0    
    
    $renderer AddActor Neuroendoscopy($name,aLineActor,$count)
    [$widget GetRenderWindow] Render

    set Neuroendoscopy($name,lineCount) [expr $Neuroendoscopy($name,lineCount) + 1]
    
    aLineR Delete
    aLineRMapper Delete
#puts "end draw in flatwindow: clock format [clock seconds]"
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyAddTargetFromFlatColon
# Find and sychronize the corresponing location in 3D colon and 2D slices that 
# corresponds to the location selected on the flattened colon
# .ARGS
# int pointId the id of the fiducial point
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyAddTargetFromFlatColon {pointId} {

    global Neuroendoscopy Point Fiducials Select Model View Path Slice Volume Volumes
    
#  If no path selected, do nothing.

    if {$Neuroendoscopy(path,activeId) == "None"} {
        return
    }    
    
    set pointId $pointId

# puts "pointId from FlatColon is $pointId"

# get the xyz coordinates of the point (picked from the flat colon) in the 3D colon.  
       set model $Model(activeID)
       if {$model != ""} {

           set polyData $Model($model,polyData)
       
           set point(xyz) [$polyData GetPoint $pointId]

           set actor Model($model,actor,viewRen)

       }

#  active list (should be the default path just created from the automatic tab)
    set fid $Fiducials($Fiducials(activeList),fid)
    set listName $Fiducials(activeList)

    FiducialsGetPointIdListFromName $listName
    
    set list $Fiducials($fid,pointIdList)
    set numP [llength $list]

    set numP [expr $numP - 1]

# compute the distance between the selected point and each point on the path.
    for {set i 0} {$i <= $numP} {incr i} {
    set pid [lindex $list $i]
    set pidCoord [FiducialsGetPointCoordinates $pid]
    set dist 0
      for {set ii 0} {$ii<3} {incr ii} {
      set diff [expr [lindex $point(xyz) $ii] - [lindex $pidCoord $ii]]
      set diff [expr $diff * $diff]
      set dist [expr $dist + $diff]
      }
    set dist [expr sqrt( $dist )]
    lappend distlist $dist    
    }

# findout the pid of the point that has the shortest distance to the selected point
    set oldlist $distlist
    set newlist [lsort -real -increasing $oldlist]
    set mindist [lindex $newlist 0]

    set minDistIndex [lsearch -exact $oldlist $mindist]
    set closestPid [lindex $list $minDistIndex]

# change the FXYZ of the closestPid to the coordinates of point(xyz) obtained from the picked point.
    set fx [lindex $point(xyz) 0]
    set fy [lindex $point(xyz) 1]
    set fz [lindex $point(xyz) 2]

    Point($closestPid,node) SetFXYZ $fx $fy $fz
# use the SetDescription to store and save $pointId information.
    Point($closestPid,node) SetDescription $pointId
#puts "start 1st mrml update: clock format [clock seconds]"
#    MainUpdateMRML
#puts "end 1st mrml update: clock format [clock seconds]"   
    NeuroendoscopyFiducialsPointSelectedCallback $fid $closestPid
    
# set the original axial slice to that location
    #set driver User
    #NeuroendoscopySetSliceDriver $driver

    MainSlicesSetOrientAll Slices
    set vol $Volume(activeID)
    set oriSlice [[Volume($vol,node) GetRasToIjk] MultiplyPoint $fx $fy $fz 1]
    set scanOrder [Volume($vol,node) GetScanOrder]
    set dim [Volume($vol,node) GetDimensions]
    
    if {$scanOrder == "IS" } {
    MainSlicesSetOffset 0 [expr round([lindex $oriSlice 2])]
    MainSlicesSetOffset 1 [expr round([expr [lindex $dim 0] - [lindex $oriSlice 0]])]
    MainSlicesSetOffset 2 [expr round([lindex $oriSlice 1])]

    } 
        
    RenderAll
    
# create targets   
    NeuroendoscopyCreateTargets
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyAddTargetFromSlices
#  When a user selects a location on the 2D slices, the Endoscope in 3D jumps to the corresponding location
# .ARGS
# int x User selected x
# int y User selected y
# int z User selected z
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyAddTargetFromSlices {x y z} {

    global Neuroendoscopy Point Fiducials Select Model View Slice Volume Volumes
    
#  If no path selected, do nothing.

    if {$Neuroendoscopy(path,activeId) == "None"} {

        return
    }

# get the pointId from the 3D model    
    if {[info exists Select(actor)] != 0} {
        set actor $Select(actor)
    } else {
        set actor ""
    }

     set model $Model(activeID)
     if {$model != ""} {
     set polyData $Model($model,polyData)
     }
   
     vtkPointLocator tempPointLocator
     tempPointLocator SetDataSet $polyData

     set pointId [tempPointLocator FindClosestPoint $x $y $z]
     set point(xyz) [$polyData GetPoint $pointId]

#puts "pointId from Slices is: $pointId"

#  active list (should be the default path just created from the automatic tab)
    set fid $Fiducials($Fiducials(activeList),fid)
    set listName $Fiducials(activeList)
    
    FiducialsGetPointIdListFromName $listName

    set list $Fiducials($fid,pointIdList)
    set numP [llength $list]
    set numP [expr $numP - 1]

# compute the distance between the selected point and each point on the path.
    for {set i 0} {$i <= $numP} {incr i} {
    set pid [lindex $list $i]
    set pidCoord [FiducialsGetPointCoordinates $pid]
    set dist 0
      for {set ii 0} {$ii<3} {incr ii} {
      set diff [expr [lindex $point(xyz) $ii] - [lindex $pidCoord $ii]]
      set diff [expr $diff * $diff]
      set dist [expr $dist + $diff]
      }
    set dist [expr sqrt( $dist )]
    lappend distlist $dist    
    }

# findout the pid of the point that has the shortest dist to the selected point
    set oldlist $distlist
    set newlist [lsort -real -increasing $oldlist]
    set mindist [lindex $newlist 0]

    set minDistIndex [lsearch -exact $oldlist $mindist]
    set closestPid [lindex $list $minDistIndex]
    
# change the FXYZ of the closestPid to the coordinates of point(xyz) obtained from the picked point.
    set fx [lindex $point(xyz) 0]
    set fy [lindex $point(xyz) 1]
    set fz [lindex $point(xyz) 2]

    Point($closestPid,node) SetFXYZ $fx $fy $fz
# use the SetDescription to store and save $pointId information.
    Point($closestPid,node) SetDescription $pointId
    
#    MainUpdateMRML
    
    NeuroendoscopyFiducialsPointSelectedCallback $fid $closestPid
    
tempPointLocator Delete

# set the original axial slice to that location
    #set driver User
    #NeuroendoscopySetSliceDriver $driver

    MainSlicesSetOrientAll Slices
    set vol $Volume(activeID)
    set oriSlice [[Volume($vol,node) GetRasToIjk] MultiplyPoint $fx $fy $fz 1]
    set scanOrder [Volume($vol,node) GetScanOrder]
    set dim [Volume($vol,node) GetDimensions]
    
    if {$scanOrder == "IS" } {
    MainSlicesSetOffset 0 [expr round([lindex $oriSlice 2])]
    MainSlicesSetOffset 1 [expr round([expr [lindex $dim 0] - [lindex $oriSlice 0]])]
    MainSlicesSetOffset 2 [expr round([lindex $oriSlice 1])]

    } 
        
    RenderAll
    

# Create Targets    
    NeuroendoscopyCreateTargets
    
# update targets in the flat window
    if {$Neuroendoscopy(FlatWindows) != ""} {

# Fix later: as of now I am assuming the user are working with the first flat model
# if the user want to work with several flat models at the same time, they either has to be really careful or
# insert some checking mechanism to prevent them from adding or selecting targets in the wrong flat window.    
    set name [lindex $Neuroendoscopy(FlatWindows) 0]
    
    set widget .t$name.fView.flatRenderWidget$name
    NeuroendoscopyUpdateTargetsInFlatWindow $widget
    }
 
  
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyAddTargetFromWorldCoordinates
#  When a user selects a location on the 3D model, the 2D slices sychronises.
# .ARGS
# int sx world x
# int sy world y
# int sz world z
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyAddTargetFromWorldCoordinates {sx sy sz} {
     global Neuroendoscopy Point Fiducials Select Model View Slice Volume Volumes

#  If no path selected, do nothing.

    if {$Neuroendoscopy(path,activeId) == "None"} {

    return
    }

           
     if {[info exists Select(actor)] != 0} {
        set actor $Select(actor)
        #set cellId $cellId
    } else {
        set actor ""
        #set cellId ""
    }

     set Select(xyz) [list $sx $sy $sz]
      
     set model $Model(activeID)
     if {$model != ""} {
     set polyData $Model($model,polyData)
     }
   
     vtkPointLocator tempPointLocator
     tempPointLocator SetDataSet $polyData

     set pointId [tempPointLocator FindClosestPoint $sx $sy $sz]
     set point(xyz) [$polyData GetPoint $pointId]

#puts "pointId from 3D colon is: $pointId"

      tempPointLocator Delete
      
# get active path-fiducial list
      set fid $Fiducials($Fiducials(activeList),fid)
      set listName $Fiducials(activeList)

      FiducialsGetPointIdListFromName $listName

      set list $Fiducials($fid,pointIdList)
      set numP [llength $list]
      set numP [expr $numP - 1]

# compute the distance between the selected point and each point on the path.
     for {set i 0} {$i <= $numP} {incr i} {
        set pid [lindex $list $i]
        set pidCoord [FiducialsGetPointCoordinates $pid]
        set dist 0
        for {set ii 0} {$ii<3} {incr ii} {
        set diff [expr [lindex $point(xyz) $ii] - [lindex $pidCoord $ii]]
        set diff [expr $diff * $diff]
        set dist [expr $dist + $diff]
     }
    set dist [expr sqrt( $dist )]
    lappend distlist $dist    
    }

# findout the pid of the point that has the shortest dist to the selected point
    set oldlist $distlist
    set newlist [lsort -real -increasing $oldlist]
    set mindist [lindex $newlist 0]

    set minDistIndex [lsearch -exact $oldlist $mindist]
    set closestPid [lindex $list $minDistIndex]

# change the FXYZ of the closestPid to the coordinates of point(xyz) obtained from the picked point.
    set fx [lindex $point(xyz) 0]
    set fy [lindex $point(xyz) 1]
    set fz [lindex $point(xyz) 2]

    Point($closestPid,node) SetFXYZ $fx $fy $fz    
# use the SetDescription to store and save $pointId information.
    Point($closestPid,node) SetDescription $pointId
    
#    MainUpdateMRML

    NeuroendoscopyFiducialsPointSelectedCallback $fid $closestPid

# set the original axial slice to that location
    #set driver User
    #NeuroendoscopySetSliceDriver $driver

    MainSlicesSetOrientAll Slices
    set vol $Volume(activeID)
    set oriSlice [[Volume($vol,node) GetRasToIjk] MultiplyPoint $fx $fy $fz 1]
    set scanOrder [Volume($vol,node) GetScanOrder]
    set dim [Volume($vol,node) GetDimensions]
    
    if {$scanOrder == "IS" } {
    MainSlicesSetOffset 0 [expr round([lindex $oriSlice 2])]
    MainSlicesSetOffset 1 [expr round([expr [lindex $dim 0] - [lindex $oriSlice 0]])]
    MainSlicesSetOffset 2 [expr round([lindex $oriSlice 1])]

    } 
        
    RenderAll
    
    
# Create Targets    
    NeuroendoscopyCreateTargets
#puts "target created??"    
# update targets in the flat window
    if {$Neuroendoscopy(FlatWindows) != ""} {

# Fix later: as of now I am assuming the user are working with the first flat model
# if the user want to work with several flat models at the same time, they either has to be really careful or
# insert some checking mechanism to prevent them from adding or selecting targets in the wrong flat window.    
    set name [lindex $Neuroendoscopy(FlatWindows) 0]
    
    set widget .t$name.fView.flatRenderWidget$name
    NeuroendoscopyUpdateTargetsInFlatWindow $widget
    }
 
  
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyLoadTargets
# Given a particular path, this allows the user to reload reviously saved targets in the MRML tree.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyLoadTargets { }  {

     global Neuroendoscopy Fiducials
     
     if {$Neuroendoscopy(path,activeId) == "None"} {
       tk_messageBox -type ok -message "Please select a path that corresponds to the existing targets"

     } else {
 
#check to see if the path comes with a targetlist
      set id $Neuroendoscopy(path,activeId)
      set targetlistname $Neuroendoscopy($id,path,name)
      append targetlistname -targets
    
         if {[lsearch $Fiducials(listOfNames) $targetlistname] != -1} {
        
         set targetlist [FiducialsGetPointIdListFromName $targetlistname]
         set num [llength $targetlist] 
         set Neuroendoscopy(selectedTarget) 1
     
         NeuroendoscopySelectTarget $Neuroendoscopy(selectedTarget)
     
         set Neuroendoscopy(totalTargets) $num
     
                if {$Neuroendoscopy(FlatWindows) != ""} {

# Fix later: as of now I am assuming the user are working with the first flat model
# if the user want to work with several flat models at the same time, they either has to be really careful or
# insert some checking mechanism to prevent them from adding or selecting targets in the wrong flat window.    
                 set name [lindex $Neuroendoscopy(FlatWindows) 0]
    
                 set widget .t$name.fView.flatRenderWidget$name
                 NeuroendoscopyUpdateTargetsInFlatWindow $widget
   
                 } else {
                 return
                 }
       }
      }

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyMainFileCloseUpdated
# Reset variables when one closes a scene.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyMainFileCloseUpdated {}  {
     global Neuroendoscopy Model Module
     
     if {$Neuroendoscopy(totalTargets) != 0} {
     
         set Neuroendoscopy(totalTargets) 0
         set Neuroendoscopy(selectedTarget) 0
         
         set Neuroendoscopy(path,activeId) "None"
         # change the text on menu buttons
         foreach mb $Neuroendoscopy(mbPathList) {
             $mb config -text "None"
         }
         $Neuroendoscopy(mbPath4Fly) config -text "None"
     }
     
     set Neuroendoscopy(flatColon,scalarVisibility) 0
     set Neuroendoscopy(flatColon,scalarLow) 0
     set Neuroendoscopy(flatColon,scalarHigh) 100

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyCreateTarget
# Create a target fiducial at the look at point, the look from point is located at the neuroendoscopy camera
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyCreateTargets {} {

    global Neuroendoscopy Point Fiducials Module
#puts "start create target: clock format [clock seconds]"
# get the ids of the selected fiducial
    set pid $Neuroendoscopy(selectedFiducialPoint) 
    set fid $Neuroendoscopy(selectedFiducialList)
   
# save its XYZ FXYZ (Note: Camera fp_actor location is not always the same as the FXYZ of the Point)
    set savedFXYZ [Point($pid,node) GetFXYZ] 
    set savedXYZ [Point($pid,node) GetXYZ]
# save its pointId
    set pointId [Point($pid,node) GetDescription]
   
# append the listname with "-targets"
    set listname $Fiducials($fid,name)
    append listname -targets

# check if that name exists, if not, create new list
    if {[lsearch $Fiducials(listOfNames) $listname] == -1} {

    set targetfid [[MainMrmlAddNode Fiducials] GetID]

    Fiducials($targetfid,node) SetType "default"
    Fiducials($targetfid,node) SetName $listname
    Fiducials($targetfid,node) SetTextSize 0.0
    Fiducials($targetfid,node) SetSymbolSize 4.0
                                
    MainMrmlAddNode EndFiducials
    MainUpdateMRML
    } else {

    set targetfid $Fiducials($listname,fid)
    }


########## GET THE COORDINATES ################
     set cam_mat [Neuroendoscopy(cam,actor) GetMatrix]   
     set fp_mat [Neuroendoscopy(fp,actor) GetMatrix]
   
     set x [$cam_mat GetElement 0 3]
     set y [$cam_mat GetElement 1 3]
     set z [$cam_mat GetElement 2 3]
     set fx [lindex $savedFXYZ 0]
     set fy [lindex $savedFXYZ 1]
     set fz [lindex $savedFXYZ 2]

########## Make the Fiducial Point ###################

    set index [llength $Fiducials($targetfid,pointIdList)]
  
    set targetpid [[MainMrmlInsertBeforeNode EndFiducials($targetfid,node) Point] GetID]
   
    Point($targetpid,node) SetXYZ $fx $fy $fz
    Point($targetpid,node) SetFXYZ $x $y $z
    Point($targetpid,node) SetIndex $index
   
    set index [expr $index + 1]
    Point($targetpid,node) SetName [concat $Fiducials($targetfid,name) $index]
# use the SetDescription to store and save $pointId information.
    Point($targetpid,node) SetDescription $pointId
    
#puts "start 2nd mrml update: clock format [clock seconds]"
    MainUpdateMRML
#puts "end 2nd mrml update: clock format [clock seconds]"

########## Get Number of Targets ################

    set Neuroendoscopy(totalTargets) [llength $Fiducials($targetfid,pointIdList)]
    set Neuroendoscopy(selectedTarget) $index

#puts " Target $index, pointId is $pointId"

#puts "end create target: clock format [clock seconds]"
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUpdateActiveTarget
# This is called when the user modifies the endoscope's position and orientation to look at an existing fiducial point.<br>
# i.e., the user can adjust the ideal view point for a particular polyp, and then save that information.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUpdateActiveTarget {} {
    global Fiducials Neuroendoscopy Point

# get the pid of the selected target
    if {$Neuroendoscopy(path,activeId) == "None"} {
        return
    }

    set index $Neuroendoscopy(selectedTarget)
    set cT $Neuroendoscopy(selectedTarget)
    
    set id $Neuroendoscopy(path,activeId)
    set listname $Neuroendoscopy($id,path,name)
    append listname -targets
    set list [FiducialsGetPointIdListFromName $listname]

    set index [expr $index - 1]
    set pid [lindex $list $index]
    set fid $Fiducials($listname,fid)
    
# get the new current camera position, i.e. the user set an ideal view point for looking at the selected target.
     set cam_mat [Neuroendoscopy(cam,actor) GetMatrix]   
   
     set x [$cam_mat GetElement 0 3]
     set y [$cam_mat GetElement 1 3]
     set z [$cam_mat GetElement 2 3]

   
#    Point($targetpid,node) SetXYZ $fx $fy $fz
     Point($pid,node) SetFXYZ $x $y $z

     MainUpdateMRML
     
     set Neuroendoscopy(selectedTarget) $cT
     NeuroendoscopySelectTarget $Neuroendoscopy(selectedTarget)

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyDeleteActiveTarget
# Delete an existing target from the fiducial list.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyDeleteActiveTarget {} {
    global Fiducials Neuroendoscopy Point
    
    if {$Neuroendoscopy(path,activeId) == "None"} {

        return
    }

    set index $Neuroendoscopy(selectedTarget)
    set pT $Neuroendoscopy(selectedTarget)
    
# if the user want to delete the first target on the list, and there are more than 2 targets on this list
    if {$pT == 1 && $Neuroendoscopy(totalTargets) >= 2 } {
         set pT 1
         set Neuroendoscopy(totalTargets) [expr $Neuroendoscopy(totalTargets)-1]
     
    } elseif { $pT == 1 && $Neuroendoscopy(totalTargets) < 2 } { 

# if there is only 1 target on the list, and the user decides to delete it
         set pT 0
         set Neuroendoscopy(selectedTarget) 0
         set Neuroendoscopy(totalTargets) 0
    } elseif {$pT > 1} {

# any other situations, the endoscope will jump to the previous target
         set pT [expr $Neuroendoscopy(selectedTarget)-1]
         set Neuroendoscopy(totalTargets) [expr $Neuroendoscopy(totalTargets)-1]
    }
    
    
    if {$Neuroendoscopy(totalTargets) != 0} {
    
        set id $Neuroendoscopy(path,activeId)
        set listname $Neuroendoscopy($id,path,name)
        append listname -targets
        set list [FiducialsGetPointIdListFromName $listname]

        set index [expr $index - 1]
        set pid [lindex $list $index]
        set fid $Fiducials($listname,fid)
    
# the following 3 lines are a kludge
        set cellId [Point($pid,node) GetDescription]  
        set Point($pid,cellId) $cellId
        set Point($pid,actor) dummyactor
   
        FiducialsDeletePoint $fid $pid
    
        set list [FiducialsGetPointIdListFromName $listname]
        set Neuroendoscopy(totalTargets) [llength $list] 
    
        set Neuroendoscopy(selectedTarget) $pT
    NeuroendoscopySelectTarget $Neuroendoscopy(selectedTarget)
    
    } else {
    
# procede to delete the fiducial list all together
         set id $Neuroendoscopy(path,activeId)
         set listname $Neuroendoscopy($id,path,name)
         append listname -targets
         FiducialsDeleteList $listname
    }
    
# delete the corresponding target in the flat window
    if {$Neuroendoscopy(FlatWindows) != ""} {

# Fix later: as of now I am assuming the user are working with the first flat model
# if the user want to work with several flat models at the same time, they either has to be really careful or
# insert some checking mechanism to prevent them from adding or selecting targets in the wrong flat window.    
    set name [lindex $Neuroendoscopy(FlatWindows) 0]
    
    set widget .t$name.fView.flatRenderWidget$name
    NeuroendoscopyUpdateTargetsInFlatWindow $widget
   
    }

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySelectTarget
# When a target is selected, the endoscope jumps to the optimum location 
# (decided by the user) and aims itself at the target.
# .ARGS
# int sT selected target
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySelectTarget {sT} {

    global Fiducials Neuroendoscopy Point Volume Volumes

# select target in 3D
    if {$Neuroendoscopy(path,activeId) == "None"} {
        return
    }

    set $Neuroendoscopy(selectedTarget) $sT
    set index $Neuroendoscopy(selectedTarget)

    if {$index > 0} {

    set id $Neuroendoscopy(path,activeId)
    set listname $Neuroendoscopy($id,path,name)
    append listname -targets
    set list [FiducialsGetPointIdListFromName $listname]

    set index [expr $index - 1]
    set pid [lindex $list $index]
  
    set point(xyz) [Point($pid,node) GetXYZ]
    set fx [lindex $point(xyz) 0]
    set fy [lindex $point(xyz) 1]
    set fz [lindex $point(xyz) 2]

#test
    set pointId [Point($pid,node) GetDescription]
#puts "the id of the verticie is $pointId"
#test end        


    NeuroendoscopyResetCameraDirection    
    NeuroendoscopyUpdateVirtualEndoscope $Neuroendoscopy(activeCam) [concat [Point($pid,node) GetFXYZ] [Point($pid,node) GetXYZ]]
   
    NeuroendoscopyUpdateActorFromVirtualEndoscope $Neuroendoscopy(activeCam)
#    Render3D
    
# set the original axial slice to that location
    #set driver User
    #NeuroendoscopySetSliceDriver $driver

    MainSlicesSetOrientAll Slices
    set vol $Volume(activeID)
    set oriSlice [[Volume($vol,node) GetRasToIjk] MultiplyPoint $fx $fy $fz 1]
    set scanOrder [Volume($vol,node) GetScanOrder]
    set dim [Volume($vol,node) GetDimensions]
    
    if {$scanOrder == "IS" } {
    MainSlicesSetOffset 0 [expr round([lindex $oriSlice 2])]
    MainSlicesSetOffset 1 [expr round([expr [lindex $dim 0] - [lindex $oriSlice 0]])]
    MainSlicesSetOffset 2 [expr round([lindex $oriSlice 1])]

    } 
        
    RenderAll
    


# select the corresponding target in the flat window
    if {$Neuroendoscopy(FlatWindows) != ""} {

# Fix later: as of now I am assuming the user are working with the first flat model
# if the user want to work with several flat models at the same time, they either has to be really careful or
# insert some checking mechanism to prevent them from adding or selecting targets in the wrong flat window.    
    set name [lindex $Neuroendoscopy(FlatWindows) 0]
    
    set widget .t$name.fView.flatRenderWidget$name
   
    set pointId [Point($pid,node) GetDescription]
    
    set polyData $Neuroendoscopy($name,polyData)
    set point(xyz) [$polyData GetPoint $pointId]

    set camx [lindex $point(xyz) 0]

    set position [$Neuroendoscopy($name,camera) GetPosition]
  
    set Neuroendoscopy(flatColon,xCamDist) $camx
    set Neuroendoscopy(flatColon,yCamDist) [lindex $position 1]
    set Neuroendoscopy(flatColon,zCamDist) [lindex $position 2]
   
    $Neuroendoscopy($name,camera) SetFocalPoint $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) 0
    $Neuroendoscopy($name,camera) SetPosition $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) $Neuroendoscopy(flatColon,zCamDist)
  
    $Neuroendoscopy(flatScale,progress) set $Neuroendoscopy(flatColon,xCamDist)
    $Neuroendoscopy(flatScale,panud) set $Neuroendoscopy(flatColon,yCamDist)
    $Neuroendoscopy(flatScale,camZoom) set $Neuroendoscopy(flatColon,zCamDist)
 
    [$widget GetRenderWindow] Render

    } else {

    return
    }
    } else {
    NeuroendoscopyResetPath $Neuroendoscopy(activeCam) $Neuroendoscopy(path,activeId)
    }
    
} 

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySelectNextTarget
# select the next target
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySelectNextTarget {} {

    global Neuroendoscopy

    if {$Neuroendoscopy(path,activeId) == "None"} {
        return
    }

    if {$Neuroendoscopy(selectedTarget) < $Neuroendoscopy(totalTargets)} {

        set Neuroendoscopy(selectedTarget) [expr $Neuroendoscopy(selectedTarget) + 1]

        NeuroendoscopySelectTarget $Neuroendoscopy(selectedTarget)
        
    } else {
        #   tk_messageBox -message "No More Targets"
        return
    }
}  

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySelectPreviousTarget
# Select the previous target.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySelectPreviousTarget {} {

    global Neuroendoscopy
    
    if {$Neuroendoscopy(path,activeId) == "None"} {
        return
    }

    if {$Neuroendoscopy(selectedTarget) > 1} {

        set Neuroendoscopy(selectedTarget) [expr $Neuroendoscopy(selectedTarget) - 1]

        NeuroendoscopySelectTarget $Neuroendoscopy(selectedTarget)
    
    } else {
        return
        # tk_messageBox -message "No More Targets"
    }

}  

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyUpdateTargetsInFlatWindow
# This is called from the update button in the flat colon window to load 
# the targets from the MRML tree into the flat colon window 
# .ARGS
# windowelement widget the flat window
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyUpdateTargetsInFlatWindow {widget} {

    global Neuroendoscopy Fiducials Point Model
    
     set model $Model(activeID)
     if {$model != ""} {
         set polyData3D $Model($model,polyData)
     }
     
     set numP3D [$polyData3D GetNumberOfPoints]
    

    set name $Neuroendoscopy($widget,name)

    # delete all the lines that's already in the flat window
    for {set linecount 0} {$linecount < $Neuroendoscopy($name,lineCount)} {incr linecount} {
        set renderer [[[$widget GetRenderWindow] GetRenderers] GetItemAsObject 0]
        $renderer RemoveActor Neuroendoscopy($name,aLineActor,$linecount)
        Neuroendoscopy($name,aLineActor,$linecount) Delete
    }
    
    set Neuroendoscopy($name,lineCount) 0
    
    [$widget GetRenderWindow] Render

    #get the active path name, and find the corresponding target list
    if {$Neuroendoscopy(path,activeId) != "None"} {
    
           set fid $Fiducials($Fiducials(activeList),fid)
           set listname $Fiducials($fid,name)
           append listname -targets
    
           # if there is no list, then the user have not inserted any targets
           # else get the cellid from the target pid, and make the line in the flat window.
           # note to myself: the cellId is stored by Point($pid,node) SetDescription when the target was inserted
           
         if {[lsearch $Fiducials(listOfNames) $listname] == -1} {
             return
             #  tk_messageBox -message "You have not inserted any target along this path"
    
         } else {

             set targetfid $Fiducials($listname,fid)
             set list $Fiducials($targetfid,pointIdList)
             set numP [llength $list]
             
             for {set i 0} {$i < $numP} {incr i} {
                 set pid [lindex $list $i]
                 set pointIdT1 [Point($pid,node) GetDescription]
                 # puts "p was $pointIdT1"     
                 set pointIdT2 [expr $pointIdT1 + $numP3D]
                 # puts "p is $pointIdT2"
                 set polyData $Neuroendoscopy($name,polyData)
                 set pointT1(xyz) [$polyData GetPoint $pointIdT1]
                 set pointT2(xyz) [$polyData GetPoint $pointIdT2]
                 
                 # find the position of the 2 points in both copies of the flat colon
                 
                 set x1 [lindex $pointT1(xyz) 0]
                 set y1 [lindex $pointT1(xyz) 1]
                 set z1 [lindex $pointT1(xyz) 2]
                 
                 set x2 [lindex $pointT2(xyz) 0]
                 set y2 [lindex $pointT2(xyz) 1]
                 set z2 [lindex $pointT2(xyz) 2]
                 
                 # compare the y position of the 2 points with the center of the flat colon \
                     assume that the point is within the boundary when the diff is smaller for that point \
                     draw target hashes around that point
                 
                 set diff1 [expr abs([expr $y1 - $Neuroendoscopy(flatColon,yMid)])]
                 set diff2 [expr abs([expr $y2 - $Neuroendoscopy(flatColon,yMid)])]
                 
                 if {$diff1 <= $diff2} {
                     NeuroendoscopyAddTargetInFlatWindow $widget $x1 $y1 $z1
                 } else {
                     NeuroendoscopyAddTargetInFlatWindow $widget $x2 $y2 $z2
                 }
                 
             }
         }
           
       } else {
           
           tk_messageBox -message "Please select a path first"
       }
    
    if {$Neuroendoscopy(selectedTarget) == 0} {
        NeuroendoscopyLoadTargets
    } else {
        NeuroendoscopySelectTarget $Neuroendoscopy(selectedTarget)
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyFlatLightElevationAzimuth
# Set the light key light eleveation and azimuth from the inputs
# .ARGS
# windowelement widget the flat window
# string Neuroendoscopy(flatColon,LightElev) light elevation, defaults to empty string
# string Neuroendoscopy(flatColon,LightAzi) light azimuth, defaults to empty string
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyFlatLightElevationAzimuth {widget {Neuroendoscopy(flatColon,LightElev)"" Neuroendoscopy(flatColon,LightAzi)""}} {
     global Neuroendoscopy
     
     set name $Neuroendoscopy($widget,name)
     
     Neuroendoscopy($name,lightKit) SetKeyLightElevation $Neuroendoscopy(flatColon,LightElev) 
     Neuroendoscopy($name,lightKit) SetKeyLightAzimuth $Neuroendoscopy(flatColon,LightAzi)
     
     [$widget GetRenderWindow] Render

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyMoveCameraX
# Move the camera in the flat colon window along the x axis
# .ARGS
# windowelement widget 
# string Neuroendoscopy(flatColon,xCamDist) distance to move the camera, defaults to emtpy string
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyMoveCameraX {widget {Neuroendoscopy(flatColon,xCamDist)""}} {
    global Neuroendoscopy
 
    set name $Neuroendoscopy($widget,name)
 
    set position [$Neuroendoscopy($name,camera) GetPosition]
  
    set Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,xCamDist)
    set Neuroendoscopy(flatColon,yCamDist) [lindex $position 1]
    set Neuroendoscopy(flatColon,zCamDist) [lindex $position 2]
  
     $Neuroendoscopy($name,camera) SetFocalPoint $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) 0
     $Neuroendoscopy($name,camera) SetPosition $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) $Neuroendoscopy(flatColon,zCamDist)
 
    [$widget GetRenderWindow] Render
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyMoveCameraY
# Move the camera in the flat colon window along the y axis
# .ARGS
# windowelement widget  the flat window
# string Neuroendoscopy(flatColon,yCamDist) distance to move the camera, defaults to emtpy string
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyMoveCameraY {widget {Neuroendoscopy(flatColon,yCamDist)""}} {
    global Neuroendoscopy
 
    set name $Neuroendoscopy($widget,name)

    set position [$Neuroendoscopy($name,camera) GetPosition]
    
    set Neuroendoscopy(flatColon,xCamDist) [lindex $position 0]
    set Neuroendoscopy(flatColon,yCamDist) $Neuroendoscopy(flatColon,yCamDist)
    set Neuroendoscopy(flatColon,zCamDist) [lindex $position 2]
    
    $Neuroendoscopy($name,camera) SetFocalPoint $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) 0
    $Neuroendoscopy($name,camera) SetPosition $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) $Neuroendoscopy(flatColon,zCamDist)
    
    [$widget GetRenderWindow] Render
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyMoveCameraZ
# Zoom the camera in the flat colon window along the z axis (in and out of the screen)
# .ARGS
# windowelement widget the flat window
# string Neuroendoscopy(flatColon,zCamDist) distance to move the camera, defaults to empty string
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyMoveCameraZ {widget {Neuroendoscopy(flatColon,zCamDist)""}} {
    global Neuroendoscopy
 
    set name $Neuroendoscopy($widget,name)
    set position [$Neuroendoscopy($name,camera) GetPosition]
    
    set Neuroendoscopy(flatColon,xCamDist) [lindex $position 0]
    set Neuroendoscopy(flatColon,yCamDist) [lindex $position 1]
    set Neuroendoscopy(flatColon,zCamDist) $Neuroendoscopy(flatColon,zCamDist)
    
    $Neuroendoscopy($name,camera) SetFocalPoint $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) 0
    $Neuroendoscopy($name,camera) SetPosition $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) $Neuroendoscopy(flatColon,zCamDist)
    
    [$widget GetRenderWindow] Render
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyResetFlatCameraDist
# Reset the camera in the flat colon window to an optimum location along the z axis based on the size of the colon
# .ARGS
# windowelement widget the flat window
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyResetFlatCameraDist {widget} {

    global Neuroendoscopy
    
    set name $Neuroendoscopy($widget,name)
    set position [$Neuroendoscopy($name,camera) GetPosition]
    
    set Neuroendoscopy(flatColon,xCamDist) [lindex $position 0]
    set Neuroendoscopy(flatColon,yCamDist) $Neuroendoscopy(flatColon,yMid)
    set Neuroendoscopy(flatColon,zCamDist) $Neuroendoscopy(flatColon,zOpt)
    
    $Neuroendoscopy($name,camera) SetFocalPoint $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) 0
    $Neuroendoscopy($name,camera) SetPosition $Neuroendoscopy(flatColon,xCamDist) $Neuroendoscopy(flatColon,yCamDist) $Neuroendoscopy(flatColon,zCamDist)
    
    $Neuroendoscopy(flatScale,progress) set $Neuroendoscopy(flatColon,xCamDist)
    $Neuroendoscopy(flatScale,panud) set $Neuroendoscopy(flatColon,yCamDist)
    $Neuroendoscopy(flatScale,camZoom) set $Neuroendoscopy(flatColon,zCamDist)
    
    [$widget GetRenderWindow] Render
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyScrollRightFlatColon
# Scroll the flat colon window to the right.
# .ARGS
# windowelement widget the flat window
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyScrollRightFlatColon {widget} {

    global Neuroendoscopy

    set name $Neuroendoscopy($widget,name)
    set position [$Neuroendoscopy($name,camera) GetPosition]
    
    set Neuroendoscopy(flatColon,xCamDist) [lindex $position 0]
    
    while {$Neuroendoscopy(flatColon,xCamDist) < $Neuroendoscopy(flatColon,xMax)} {
        
        if {$Neuroendoscopy(flatColon,stop) == "0"} {
            
            set  Neuroendoscopy(flatColon,xCamDist) [expr  $Neuroendoscopy(flatColon,xCamDist) + $Neuroendoscopy(flatColon,speed)]
            
            NeuroendoscopyMoveCameraX  $widget $Neuroendoscopy(flatColon,xCamDist)
            
            update
            
        } else {
            
            NeuroendoscopyResetStop
            
            break
            
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyScrollLeftFlatColon
# Scroll the flat window to the left
# .ARGS
# windowelement widget the flat window
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyScrollLeftFlatColon {widget} {

    global Neuroendoscopy

   
    set name $Neuroendoscopy($widget,name)
    set position [$Neuroendoscopy($name,camera) GetPosition]
    
    set Neuroendoscopy(flatColon,xCamDist) [lindex $position 0]
    
    while {$Neuroendoscopy(flatColon,xCamDist) > $Neuroendoscopy(flatColon,xMin)} {
        
        if {$Neuroendoscopy(flatColon,stop) == "0"} {
            
            set  Neuroendoscopy(flatColon,xCamDist) [expr  $Neuroendoscopy(flatColon,xCamDist) - $Neuroendoscopy(flatColon,speed)]
            
            NeuroendoscopyMoveCameraX  $widget $Neuroendoscopy(flatColon,xCamDist)
            
            update
            
        } else {
            
            NeuroendoscopyResetStop
            
            break
            
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyStopFlatColon
# Sets Neuroendoscopy(flatColon,stop) to one
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyStopFlatColon {} {

    global Neuroendoscopy
    
    set Neuroendoscopy(flatColon,stop) 1

}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyResetFlatColon
# Resets Neuroendoscopy(flatColon,xCamDist) to minimum x value
# .ARGS
# windowelement widget the flat window
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyResetFlatColon {widget} {

    global Neuroendoscopy
    
    set Neuroendoscopy(flatColon,xCamDist) [expr [expr floor($Neuroendoscopy(flatColon,xMin))]-1] 
    
    NeuroendoscopyMoveCameraX  $widget $Neuroendoscopy(flatColon,xCamDist)
    
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopyResetStop
# Sets  Neuroendoscopy(flatColon,stop) to zero
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopyResetStop {} {

     global Neuroendoscopy
     
     set Neuroendoscopy(flatColon,stop) 0
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetFlatColonScalarVisibility
# Sets the scalar visibility in the flat window according to the value of
# Neuroendoscopy(flatColon,scalarVisibility).
# .ARGS
# windowelement widget the flat window
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetFlatColonScalarVisibility {widget} {
     
     global Neuroendoscopy
     
     set name $Neuroendoscopy($widget,name)
     
     if {$Neuroendoscopy(flatColon,scalarVisibility) == 0} {
     
         Neuroendoscopy($name,FlatColonMapper) ScalarVisibilityOff
         Neuroendoscopy($name,lightKit) SetKeyLightIntensity 0.7
         [Neuroendoscopy($name,FlatColonActor) GetProperty] SetAmbient 0.55
         [Neuroendoscopy($name,FlatColonActor) GetProperty] SetDiffuse 0.4
         [Neuroendoscopy($name,FlatColonActor) GetProperty] SetSpecular 0.5
         [Neuroendoscopy($name,FlatColonActor) GetProperty] SetSpecularPower 100
     
     } else {
     
         [Neuroendoscopy($name,FlatColonActor) GetProperty] SetAmbient 0.2
         [Neuroendoscopy($name,FlatColonActor) GetProperty] SetDiffuse 1.0
         [Neuroendoscopy($name,FlatColonActor) GetProperty] SetSpecular 0.5
         [Neuroendoscopy($name,FlatColonActor) GetProperty] SetSpecularPower 100
         Neuroendoscopy($name,lightKit) SetKeyLightIntensity 0.6
         Neuroendoscopy($name,FlatColonMapper) ScalarVisibilityOn
         
         #     Neuroendoscopy($name,FlatColonMapper) SetScalarMaterialModeToAmbientAndDiffuse
         
         Neuroendoscopy($name,FlatColonMapper) SetLookupTable Neuroendoscopy(flatColon,lookupTable,$name)
         Neuroendoscopy($name,FlatColonMapper) SetScalarRange $Neuroendoscopy(flatColon,scalarLow) $Neuroendoscopy(flatColon,scalarHigh)
     }
     
     
    [$widget GetRenderWindow] Render
}

#-------------------------------------------------------------------------------
# .PROC NeuroendoscopySetFlatColonScalarRange
# If scalars are visible, set the properties of the flat colon actor, lookup table and scalar range.
# Otherwise make them visible.
# .ARGS
# windowelement widget the flat window
# .END
#-------------------------------------------------------------------------------
proc NeuroendoscopySetFlatColonScalarRange {widget} {
     
     global Neuroendoscopy
     
     set name $Neuroendoscopy($widget,name)
     
     if {$Neuroendoscopy(flatColon,scalarVisibility) == 1} {
     
         [Neuroendoscopy($name,FlatColonActor) GetProperty] SetAmbient 0.2
         [Neuroendoscopy($name,FlatColonActor) GetProperty] SetDiffuse 1.0
         [Neuroendoscopy($name,FlatColonActor) GetProperty] SetSpecular 0.5
         [Neuroendoscopy($name,FlatColonActor) GetProperty] SetSpecularPower 100
         Neuroendoscopy($name,lightKit) SetKeyLightIntensity 0.6

         Neuroendoscopy($name,FlatColonMapper) ScalarVisibilityOn
         
         #     Neuroendoscopy($name,FlatColonMapper) SetScalarMaterialModeToAmbientAndDiffuse
         
         Neuroendoscopy($name,FlatColonMapper) SetLookupTable Neuroendoscopy(flatColon,lookupTable,$name)
         Neuroendoscopy($name,FlatColonMapper) SetScalarRange $Neuroendoscopy(flatColon,scalarLow) $Neuroendoscopy(flatColon,scalarHigh)
         
     } else {
         
         set Neuroendoscopy(flatColon,scalarVisibility) 1
         NeuroendoscopySetFlatColonScalarVisibility $widget
         #     Neuroendoscopy($name,FlatColonMapper) ScalarVisibilityOn
         #     Neuroendoscopy($name,FlatColonMapper) SetScalarRange $Neuroendoscopy(flatColon,scalarLow) $Neuroendoscopy(flatColon,scalarHigh)
     }
     
     [$widget GetRenderWindow] Render
}
