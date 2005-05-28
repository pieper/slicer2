proc DTMRIMaskInit {} {
    
    global DTMRI Volume

    # upper and lower values allowable when thresholding
    set DTMRI(thresh,threshold,rangeLow) 0
    set DTMRI(thresh,threshold,rangeHigh) 500

    # type of thresholding to use to reduce number of DTMRIs
    set DTMRI(mode,threshold) None
    set DTMRI(mode,thresholdList) {None Trace LinearMeasure SphericalMeasure PlanarMeasure}
    set DTMRI(mode,thresholdList,tooltips) {{No thresholding.  Display all DTMRIs.} {Only display DTMRIs where the trace is between the threshold values.}  {Only display DTMRIs where the anisotropy is between the threshold values.}}

    # type of masking to use to reduce volume of DTMRIs
    set DTMRI(mode,mask) None
    set DTMRI(mode,maskList) {None MaskWithLabelmap}
    set DTMRI(mode,maskList,tooltips) {{No masking.  Display all DTMRIs.} {Only display DTMRIs where the mask labelmap shows the chosen label value.}}
    set DTMRI(mode,maskLabel,tooltip) "The ROI colored with this label will be used to mask DTMRIs.  DTMRIs will be shown only inside the ROI."

    # labelmap to use as mask
    set DTMRI(MaskLabelmap) $Volume(idNone)


}




#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateThreshold
# If we are thresholding the glyphs to display a subvolume and
# the user requests a new threshold range this is called.
# .ARGS
# string not_used Not used
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateThreshold {{not_used ""}} {
    global DTMRI
    
    DTMRI(vtk,thresh,threshold)  ThresholdBetween \
    $DTMRI(thresh,threshold,lower) \
    $DTMRI(thresh,threshold,upper)

    # Update pipelines
    Render3D
}


#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateMaskLabel
# If we are masking the glyphs to display a subvolume and
# the user requests a new mask label value this is called. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateMaskLabel {} {
    global Label

    LabelsFindLabel

    # this label becomes 1 in the mask
    set thresh DTMRI(vtk,mask,threshold)
    $thresh ThresholdBetween $Label(label) $Label(label)

    # Update pipelines
    Render3D
}

