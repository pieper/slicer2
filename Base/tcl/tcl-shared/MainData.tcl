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
# FILE:        MainData.tcl
# PROCEDURES:  
#   MainDataInit
#   MainDataUpdateMRML
#   MainDataCreate
#   MainDataRead
#   MainDataDelete
#   MainDataSetActive
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC MainDataInit
# Call this procedure at the top of your module's Init proc
# in order to set up standard variables needed to use the rest
# of the procedures in this file.  
# Warning: this proc cannot set up your module's version info,
# (see Custom.tcl for more info) so please do that in your 
# module's own init procedure.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainDataInitialize {ModuleArray} {

    # If the module is not loaded in the Slicer, do nothing.
    #--------------------------------------------------------
    if {[IsModule $ModuleArray] == "0"} {
        ## not all data types are module names, so don't return - steve & raul 2004-02-24
        #return
    }

    # Get access to the global module array
    #--------------------------------------------------------
    upvar #0 $ModuleArray Array
    
    # Init standard module variables needed to keep track of 
    # the data objects in the slicer
    #--------------------------------------------------------
    # ID indicating no data object selected
    set Array(idNone) -1
    # ID indicating currently selected data object
    set Array(activeID) ""
    # Whether we are currently frozen (adding a new data object).
    set Array(freeze) ""
    # The ID number of the next object we add
    set Array(nextID) 0
    # List of all the data objects in the slicer currently
    set Array(idList) ""
    # List of the data objects to delete next time we update mrml
    set Array(idListDelete) ""
    
    # Lauren these extra things are for volumes only
    # Lauren standardize idNone?
    #set Volume(idNone) 0
    #set Volume(idList) 0
    #set Volume(nextID) 1

    # Lists of GUI menus to refresh during UpdateMRML
    #--------------------------------------------------------
    set Array(mbActiveList) ""
    #set Array(mActiveList)  ""

    # Check the developer has created essential procs
    #--------------------------------------------------------
    set procedures ""
    lappend procedures Main${ModuleArray}GetAllVariablesFromNode
    lappend procedures Main${ModuleArray}SetAllVariablesToNode
    lappend procedures Main${ModuleArray}GetAllVariablesFromData
    lappend procedures Main${ModuleArray}SetAllVariablesToData

    # create dummy procs to give warnings if called
    foreach procedure $procedures {
        if {[info procs $procedure] != $procedure} {
            set message "SLICER WARNING: Developer has not written \
                proc $procedure needed by MainData.tcl."
            set command "proc $procedure {args} {puts \"$message\"}"
            eval $command
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC MainDataUpdateMRML
#  General UpdateMRML procedure useable by all datatypes in the slicer.
#  This procedure will create any new data objects and delete any old ones.
#  If your datatype needs to handle datatype-specific things when
#  there is a change in MRML, write a wrapper proc for this procedure.
# .ARGS
# 
# .END
#-------------------------------------------------------------------------------
proc MainDataUpdateMRML {ModuleArray} {


    #puts "Lauren in MainDataUpdateMRML $ModuleArray "

    # If the module is not loaded in the Slicer, do nothing.
    #--------------------------------------------------------
    if {[IsModule $ModuleArray] == "0"} {
        ## not all data types are module names, so don't return - steve & raul 2004-02-24
        #return
    }

    # Get access to the global module array
    #--------------------------------------------------------
    upvar #0 $ModuleArray Array

    # Build any new data objects
    #--------------------------------------------------------
    foreach d $Array(idList) {
        if {[MainDataCreate $ModuleArray $d] > 0} {

            # Lauren improve this on the fly thing using MRML DATA object
            # Mark it as not being created on the fly 
            # since it was added from the Data module or read in from MRML
            set Array($d,fly) 0
            
            # Read
            if {[MainDataRead $ModuleArray $d] < 0} {
                # Let the user know about the error
                # Lauren general filename we can print from node/data object?
                tk_messageBox -message "Could not read [$Array($d,node) GetTitle]"
                # Remove the objects we have created
                MainMrmlDeleteNodeDuringUpdate $ModuleArray $d
            }
        }
    }
    
    # Delete any old data objects
    #--------------------------------------------------------
    foreach d $Array(idListDelete) {
        MainDataDelete $ModuleArray $d
    }
    # Did we delete the active data?
    if {[lsearch $Array(idList) $Array(activeID)] == -1} {
        MainDataSetActive $ModuleArray [lindex $Array(idList) 0]
    }
    
    # Update any menus that list all data objects 
    #--------------------------------------------------------
    if {[info exists Array(mActiveList)] == "1"} {
    foreach menu $Array(mActiveList) {
        # clear out menu
        $menu delete 0 end
        # add all current data objects to menu
        foreach d $Array(idList) {
            set node ${ModuleArray}($d,node)
            $menu add command -label [$node GetName] \
                -command "Main${ModuleArray}SetActive $ModuleArray $d"
        }
    }
    } else {
        if {$::Module(verbose)} {
            puts "Developer: you have not put menus on ModuleArray(mActiveList),\
            which is a convenience for updating menus listing all \
            $ModuleArray objects.  See MainData.tcl, proc MainDataUpdateMRML \
            for information on how to stop this message from appearing."
        }
    }

    # In case we changed the name of the active data object
    MainDataSetActive $ModuleArray $Array(activeID)
}

#-------------------------------------------------------------------------------
# .PROC MainDataCreate
#  Actually create a data object.  Called from MainDataUpdateMRML.
#  Each module must add its own actor to the scene after
#  using this procedure to create a vtkMrmlData* object for it.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainDataCreate {ModuleArray d {objectType ""}} {

    #puts "Lauren in MainData create $ModuleArray $d"

    # If the module is not loaded in the Slicer, do nothing.
    #--------------------------------------------------------
    if {[IsModule $ModuleArray] == "0"} {
        ## not all data types are module names, so don't return - steve & raul 2004-02-24
        #return
    }

    # Default value of vtkMrmlData subclass to create
    #--------------------------------------------------------
    if {$objectType == ""} {
        set objectType $ModuleArray
    }

    # Get access to the global module array
    #--------------------------------------------------------
    upvar #0 $ModuleArray Array

    # Get MRML node
    #--------------------------------------------------------
    set node ${ModuleArray}($d,node)

    # Get MRML data
    #--------------------------------------------------------
    set data ${ModuleArray}($d,data)

    # See if this data object already exists
    #--------------------------------------------------------
    if {[info command $data] != ""} {
        puts "MainDataCreate: $ModuleArray $d data exists"
        return 0
    }

    # Create vtkMrmlData* object 
    #--------------------------------------------------------
    #vtkMrmlData${ModuleArray} $data
    vtkMrmlData${objectType} $data

    # Connect data object with the MRML node
    #--------------------------------------------------------
    $data SetMrmlNode $node

    # Progress methods
    #--------------------------------------------------------
    $data AddObserver StartEvent       MainStartProgress
    $data AddObserver ProgressEvent   "MainShowProgress $data"
    $data AddObserver EndEvent         MainEndProgress

    # Here the module can hook in to set the new Data
    # object's properties (as defined by user in GUI)
    #--------------------------------------------------------
    Main${ModuleArray}SetAllVariablesToData $d

    # Mark the object as unsaved and created on the fly.
    # If it actually isn't being created on the fly, I can't tell that from
    # inside this procedure, so the "fly" variable will be set to 0 in the
    # MainDataUpdateMRML procedure.
    set Array($d,dirty) 1
    set Array($d,fly) 1
    
    return 1
}

#-------------------------------------------------------------------------------
# .PROC MainDataRead
#  Read in a vtkMrmlData object.  Called from MainDataUpdateMRML.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainDataRead {ModuleArray d} {

    #puts "Lauren in MainDataRead"

    # If the module is not loaded in the Slicer, do nothing.
    #--------------------------------------------------------
    if {[IsModule $ModuleArray] == "0"} {
        ## not all data types are module names, so don't return - steve & raul 2004-02-24
        #return
    }

    # Get access to the global module array
    #--------------------------------------------------------
    upvar #0 $ModuleArray Array

    # Get MRML node
    #--------------------------------------------------------
    set node ${ModuleArray}($d,node)

    # Get MRML data
    #--------------------------------------------------------
    set data ${ModuleArray}($d,data)

    # Check FileName
    #--------------------------------------------------------
    # this test works in simple case where node has 1 filename
    # for volumes this test can't work and should
    # be handled by data object instead
    set fileName ""
    catch {set fileName [$node GetFileName]}
    if {$fileName != ""} {
        if {[CheckFileExists $fileName] == 0} {
            return -1
        }
    }
    
    # Display appropriate text over progress bar while reading
    #--------------------------------------------------------
    set Gui(progressText) "Reading [$node GetName]"

    # Read using vtkMrmlData object
    #--------------------------------------------------------
    puts "Reading [$node GetTitle]..."
    $data Read
    $data Update
    puts "...finished reading [$node GetTitle]"

    set Gui(progressText) ""

    # Lauren: models did pipeline stuff here
    # Now we either do pipeline junk here,
    # or we write vtkMrmlSlicerTensors to handle it

    # Mark this tensor as saved already
    set Array($d,dirty) 0

    # Return success code 
    #--------------------------------------------------------
    return 1
}

#-------------------------------------------------------------------------------
# .PROC MainDataDelete
#  Delete a vtkMrmlData object.  Called from MainDataUpdateMRML.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainDataDelete {ModuleArray d} {

    # If the module is not loaded in the Slicer, do nothing.
    #--------------------------------------------------------
    if {[IsModule $ModuleArray] == "0"} {
        ## not all data types are module names, so don't return - steve & raul 2004-02-24
        #return
    }

    # Get access to the global module array
    #--------------------------------------------------------
    upvar #0 $ModuleArray Array  

    # Get MRML data
    #--------------------------------------------------------
    set data ${ModuleArray}($d,data)

    # Make sure we are not deleting the idNone
    #--------------------------------------------------------
    if {$d == $Array(idNone)} {
        puts "Warning: MainDataDelete, trying to delete the none $moduleArray"
        return 0
    }

    # See if this data object exists
    #--------------------------------------------------------
    if {[info command $data] != ""} {
        return 0
    }

    # Remove actors from renderers
    #--------------------------------------------------------
    # Actor handling is not general and must be done by the module
    #MainRemoveActor $d
    
    # Delete VTK objects 
    #--------------------------------------------------------
    $data  Delete
    #puts "Lauren does deleting the volume really delete the node?"
    
    # Delete all TCL variables of the form: Array($d,<whatever>)
    #--------------------------------------------------------
    foreach name [array names Array] {
        if {[string first "$d," $name] == 0} {
            unset Array($name)
        }
    }
    
    return 1
}

#-------------------------------------------------------------------------------
# .PROC MainDataSetActive
#  Select a data object as active, and update the GUI to 
#  display properties of this object.  This proc calls the
#  procedure Main${ModuleArray}GetAllVariablesFromNode,
#  which the developer should write.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainDataSetActive {ModuleArray d} {
    
    # If the module is not loaded in the Slicer, do nothing.
    #--------------------------------------------------------
    if {[IsModule $ModuleArray] == "0"} {
        ## not all data types are module names, so don't return - steve & raul 2004-02-24
        #return
    }

    # Get access to the global module array
    #--------------------------------------------------------
    upvar #0 $ModuleArray Array

    # Are we currently frozen (in process of adding a 
    # new data object, so unable to activate another)?
    #--------------------------------------------------------
    if {$Array(freeze) == 1} {
        puts "Frozen: can't activate $ModuleArray $d"
        return
    }
    
    # Lauren this is from volumes.  1st time this proc is
    # called, sets active id to none.  this is okay since the
    # none volume exists.  then its node is used below to get name.
    # this is a difference btwn volumes and models...
    #if {$d == ""} {
    #set Array(activeID) $Array(idNone)
    #} else {
    #set Array(activeID) $d
    #}
    
    # Update ID number of active object
    #--------------------------------------------------------
    set Array(activeID) $d    

    # Decide which button text and node settings to use
    #--------------------------------------------------------
    if {$d == "NEW"} {
        # If d is NEW, we are starting to add a new node.

        # Menu button text reads "NEW"
        set mbText "NEW"
        # Update the Module's GUI with default node settings
        set nodeID "default"

    } elseif {$d == ""} {

        # Menu button text reads "None"
        set mbText "None"
        # Update the Module's GUI with default node settings
        set nodeID "default"
    
    } else {

        # Get current MRML node
        set node ${ModuleArray}($d,node)

        # Menu button text shows active object's name
        set mbText [$node GetName]
        # Update the Module's GUI with settings from active node
        set nodeID $d
    }

    # Change menu button text
    #--------------------------------------------------------
    foreach mb $Array(mbActiveList) {
        $mb config -text $mbText
    }
    
    # Update the Module's GUI (all the tcl vars that shadow the node)
    #--------------------------------------------------------
    Main${ModuleArray}GetAllVariablesFromNode $nodeID
}
