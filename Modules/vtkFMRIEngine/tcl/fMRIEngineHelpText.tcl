#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        fMRIEngineHelpText.tcl
# PROCEDURES:  
#   fMRIEngineHelpLoadSequence
#   fMRIEngineHelpLoadVolumeAdjust
#   fMRIEngineHelpSetupChooseDetector
#   fMRIEngineHelpSetup
#   fMRIEngineHelpSetupBlockEventMixed
#   fMRIEngineHelpSetupWaveform
#   fMRIEngineHelpSetupHRFConvolution
#   fMRIEngineHelpSetupTempDerivative
#   fMRIEngineHelpSetupHighpassFilter
#   fMRIEngineHelpSetupLowpassFilter
#   fMRIEngineHelpSetupGlobalFX
#   fMRIEngineHelpSetupCustomFX
#   fMRIEngineHelpSetupEstimate
#   fMRIEngineHelpSetupContrasts
#   fMRIEngineHelpComputeActivationVolume
#   fMRIEngineHelpViewActivationThreshold
#   fMRIEngineHelpViewPlotting
#==========================================================================auto=

proc fMRIEngineGetHelpWinID { } {
    if {! [info exists ::fMRIEngine(newID) ]} {
        set ::fMRIEngine(newID) 0
    }
    incr ::fMRIEngine(newID)
    return $::fMRIEngine(newID)
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpLoadSequence
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpLoadSequence { } {
    #--- Sequence->Load
    #--- loading sequences
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Loading sequences</H3>
 <P> A single file may be loaded by selecting the <I> single file </I> radio button within the Load GUI, and either typing the filename (including its complete path) or using the <I> Browse </I> button to select the file.
<P> A sequence of files may be loaded by selecting the <I> multiple files </I> radio button and specifying an appropriate file filter in the Load GUI, and then using the <I> Browse </I> button to select one of the files.
<P><B>Supported file formats</B>
<P> Currently the fMRIEngine supports the loading of Analyze, DICOM and BXH single- and multi-volume sequences."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 18 $txt
}

#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpLoadVolumeAdjust
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpLoadVolumeAdjust { } {
    #--- Sequence->Select
    #--- applying window/level/threshold.
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Window-Level-Threshold</H3>
<P> To adjust the window, level or threshold of an entire sequence, view one of its volumes in Slicer's main Viewer, go to the Volumes module's <I>Display</I< tab. There, the volume's window, level, threshold and palette may be adjusted. To apply these adjustments to the entire sequence, go immediately back to the fMRIEngine's Sequence->Select tab and use the <I>Select Window/Level/Threshold </I> button.
<P><B>Brain masking</B>
<P> The non-brain background in a sequence may be masked by thresholding the sequence in the manner described above. This action prevents spurious activations outside the brain from being computed.
<P><B>Color-coded display of activation</B>
<P> To use color to represent different p-values in the computed activation volume, go to the Volumes module's Display tab and choose the <I>Rainbow</I> or <I>Iron</I> palettes."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt
}

#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpSetupChooseDetector
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpSetupChooseDetector { } {
    #--- Setup
    #--- Choose a method of activation detection
    set i [ fMRIEngineGetHelpWinID ]    
    set txt "<H3>Methods of activation detection</H3>
<P> The fMRIEngine is being developed to offer different methods of computing brain activations from fMRI data, and to act as a platform for comparing their performance. Currently this module only supports the commonly used general linear model approach to fMRI analysis {1,2,3}. An activation detector based on Mutual Information, and the ability to incorporate spatial priors into either detection method are currently being developed. Once multiple detectors become available, the Setup tab's GUI will present different and appropriate input options for each.
<P><B>Linear modeling</B>
<P> This method fits a linear model, consisting of both active responses to the stimulus paradigm and noise from various sources, to the observed voxel timecourse data. To build the model, a paradigm is specified in the <I>Paradigm specification</I> GUI, and a corresponding collection of variables representing the BOLD signal and some added noise are designed in the <I>Signal Modeling</I> GUI. This approach relies on the assumption that the observed data's underlying distribution is Gaussian, and independent and identically distributed (i.i.d).
<P><B>1.</B> K.J. Friston, P. Jezzard, and R. Turner, 'The analysis of functional MRI time-series,' <I>Human Brain Mapping,</I> vol. 1, pp. 153-171, 1994.
<P><B>2.</B> K.J. Friston, A.P. Holmes, K.J. Worsley, J.P. Poline, C.D. Frith, and R.S.J. Frackowiak, 'Statistical parametric maps in functional imaging: A general linear approach,' <I>Human Brain Mapping,</I> vol. 2, pp. 189-210, 1995.
<P><B>3.</B> K.J.Friston, 'Statistical parametric mapping and other analysis of functional imaging data,' in <I>Brain Mapping: The Methods,</I> pp. 363-385. Academic Press, 1996."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt
}

#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpSetup
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpSetup { } {
    #--- Setup ->Specify [paradigm design, signal modeling, contrasts ]
    #--- Explain what we are specifying
    set i [ fMRIEngineGetHelpWinID ]    
    set txt "<H3>Specifying the model and contrasts</H3>
<P> Three steps are grouped together in this GUI panel: paradigm design, signal modeling and contrast specification. Each of these steps is described below.
<P><B>1. Paradigm design</B>
<P> In this step, the stimulus protocol for an experiment is described. Whether the design is a <I>blocked, event-related or mixed</I> design is chosen. The first volume of the loaded sequence to be used in the analysis (to eliminate any dummy scans) and the repetition time TR are specified. The <I>number of runs</I> to be included in the analysis is entered and, if more than one run will be used, whether the conditions in each are <I>identical</I> is noted. If all runs are identical, then each condition can be entered once and will be duplicated across runs. If runs do not contain the same conditions with identical timing parameters, then all conditions for each must be individually specified.
<P> Then, for each experimental condition, a <I>name</I> for the condition is entered along with a number of vectors that describe it: a vector of stimulus <I>onsets</I> is entered, in which each element indicates the number of TRs at which each stimulus presentation was initiated; a vector of <I>durations</I> is entered, in which each element indicates the number of TRs that the stimulus persists for each presentation (for event-related designs, this vector should contain a '1' for each presentation); and a vector of <I>intensities</I> is entered, in which each element indicates the intensity of the stimulus at each presentation. By default, this vector contains '1's, and currently the fMRIEngine does not include linear, exponential or polynomial changes over the course of the experiment; so changes to this vector currently have no effect.
<P> Once data has been entered for a condition, the <I>OK</I> button will add that condition to a list displayed at the bottom of the GUI panel. Any condition in this list may be selected and either edited or deleted using the associated <I>edit</I> and <I>delete</I> buttons. Once all conditions for all runs have been specified, both their representation and that of nuisance signals can be modeled in the next step.
<P><B>2. Signal modeling</B>
<P> For the linear modeling approach to activation detection, signal modeling uses conditions from the paradigm specification as a basis to model the physiological response to them that an experimenter expects to observe. The modeling options provided here attempt to simulate a signal that more closely resembles the hemodyanamic response than does the raw paradigm alone. The raw paradigm can still be used by simply making sure the boxcar waveform and no other modeling option is selected. Once conditions have been modeled, they are referred to as <I>explanatory variables</I> (or EV's), since they are intended to explain different processes resident in the data. Each EV is represented as a column in the design matrix, which can be displayed using the <I>Show model</I> button.
<P><I>2a. Stimulus modeling options</I>
<P> Modeling options include the specification of a stimulus function, which is a basic signal waveform meant to correspond to a stimulus time-course. Selecting the <I>boxcar</I> function produces an EV with a sharp 'on-and-off' signal describing each stimulus presentation, and the <I>half-sine</I> function produces an EV signal with a smoother and symmetric shape during each stimulus presentation. Each stimulus function may next be convolved with the <I>hemodynamic response function</I> (HRF). This option is intended to simulate the brain's BOLD response to the input stimulus. In the fMRIEngine, the HRF is modeled as a difference of two gamma functions:
<P> h(t) = (t/d1)^d1 exp(-(t-d1)/b1)   -  c(t/d2)^d1 exp(-(t-d2)b2)
<P>with d1,d2,b1,b2, and c as specified in {1}. The result of convolving the stimulus function with the HRF yields an EV signal with a slight delay at stimulus onset, blur, and an undershoot at the end of each stimulus presentation. Adding temporal derivatives is a another way of creating an EV with a small onset delay for each stimulus presentation.
<P><I>2b. Noise modeling options</I>
<P> Low frequency scanner drift is often observable in the voxel timecourse as slow changes in intensity. Various methods of modeling drift components exist, though it remains difficult to specify a drift model that is equally valid for every fMRI dataset. The fMRIEngine currently provides a set of commonly used Discrete Cosine Transform basis functions to model this low frequency noise. Choosing to high-pass filter the data with the default <I>DCbasis</I> automatically generates additional explanatory variables to capture frequency variations in the observed data, which exist below the frequency corresponding to twice the longest epoch spacing within any of the conditions.
<P> Once a condition has been modeled, clicking the <I>OK</I> button will add the resulting EV to a list displayed at the bottom of the GUI panel. Any EV in this list may be selected and either edited or deleted using the associated <I>edit</I> and <I>delete</I> buttons. Once all conditions have been modeled and any EVs for noise modeling have been specified, the model can be estimated by clicking the <I>estimate</I> button.
<P><B>3. Contrast specification</B>
<P> In this step, scientific questions about the data are specified. Detecting evidence of an effect by looking for significant differences between conditions is accomplished by performing a T-test. This statistical test computes the ratio of the effect to its standard error; large values of T indicate evidence of an effect. The particular differences among parameters that describe the effect in question are specified by a <I>contrast</I> vector. Individual contrasts can be defined by entering a name and a vector whose number of elements equals the number of columns in the design matrix, and whose individual elements describe the comparison among its columns. The fMRIEngine will automatically add any non-specified trailing zeros to a contrast vector, so only the elements up to and including the last non-zero element need to be entered.
<P> Once each contrast has been named and defined, clicking the <I>OK</I> button will add the resulting contrast to a list displayed at the bottom of the GUI panel. Any contrast in this list may be selected and either edited or deleted using the associated <I>edit</I> or <I>delete</I> buttons. It is useful to use the design matrix as visual reference (by clicking the <I>Show model</I> button) while specifying each contrast vector. As contrasts are specified, clicking the <I>update</I> button on the popup model view window will visually display all defined contrast below the design matrix.
<P> After these three steps have been completed, the fMRIEngine's <I>Compute</I> tab GUI permits individual brain activation volumes that correspond to each defined T-test to be computed. 

<P><B>1.</B> G.H.Glover, 'Deconvolution of impulse response in event-related BOLD fMRI,' <I>Neuroimage,</I> vol. 9, pp. 416-20, 1999."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt
}

#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpSetupBlockEventMixed
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpSetupBlockEventMixed { } {
    #--- Setup->Paradigm
    #--- Design type: blocked/event-related/mixed
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Experimental paradigms</H3>
<P><B>Blocked design </B>
<P> In a <I>blocked</I> task paragidm, a given condition may consist of a series of stimulus presentations, each of which persists for a discrete epoch of time. Here, block paradigms are specified by entering the number of the first volume to be included in the analysis (to eliminate dummy scans) the scan repetition time TR, a vector of stimulus presentation onsets (in multples of TR), a vector of epoch durations (in multiples of TR), and a vector of intensities (currently all vector elements are assumed to be '1').
<P><B>Event-related design </B>
<P> In a <I>event-related</I> task paradigm, individual stimulus presentations -- or even different components of an individual stimulus presentation are measured at an instant in time, rather than as persisting throughout some epoch of time. Event-related paradigms are specified by entering the number of the first volume to be included in the analysis (to eliminate dummy scans), the scan repetition time TR, a vector of stimulus presentation onsets (in multiples of TR), and a vector of stimulus intensities (currently all vector elements are assumed to be '1'); here elements of the vector of stimulus durations is also assumed to be '1'.
<P><B>Mixed design </B>
<P> In <I>mixed</I> task paradigms, the conventions for blocked and event-related paradigm description above are followed, with each event-related stimulus presentation having a duration of 1 TR, while blocked presentations have longer duration."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt
}

#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpSetupWaveform
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpSetupWaveform { } {
    #--- Setup->Signal
    #--- What is waveform?
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Stimulus function</H3>
<P> Modeling options include the specification of a stimulus function, which is a basic signal waveform meant to correspond to a stimulus time-course. The <I>boxcar</I> function is selected by default, and produces an EV with a sharp 'on-and-off' signal describing each stimulus presentation. The <I>half-sine</I> function may be selected instead; it produces an EV signal with a smoother and symmetric shape during each stimulus presentation.
<P> Once a waveform has been selected, additional modeling options may be applied. When signal modeling is complete, clicking the <I>OK</I> button will add the resulting EV to a list displayed at the bottom of the GUI panel. Any EV in this list may be selected and either edited or deleted using the associated <I>edit</I> and <I>delete</I> buttons. By default, an EV that represents the <I>baseline</I> for every run will be automatically added to this list. If this EV is not desireable, it may be selected and deleted.
<P>When all conditions have been modeled and any EVs for noise modeling have been specified, the model can be estimated by clicking the <I>estimate</I> button."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt
}

#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpSetupHRFConvolution
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpSetupHRFConvolution { } {
    #--- Setup->Signal
    #--- What is HRF convolution?
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Hemodynamic response</H3>
<P> Any stimulus function may be convolved with the <I>hemodynamic response function</I> (HRF). This option is intended to simulate the brain's BOLD response to the input stimulus, and to therefore better fit the observed voxel timecourse data. A BOLD response resident within a particular voxel timecourse usually occurs between 3 and 10 seconds after the stimulus onset, peaking at about 6 seconds. In the fMRIEngine, the HRF is modeled as a difference of two gamma functions:
<P> h(t) = (t/d1)^d1 exp(-(t-d1)/b1)   -  c(t/d2)^d1 exp(-(t-d2)b2)
<P>with d1,d2,b1,b2, and c as specified in {1}. The result of convolving the stimulus function with the HRF yields an EV signal with a slight delay at stimulus onset, blur, and an undershoot at the end of each stimulus presentation.
<P> Once all signal modeling for an EV has been completed, clicking the <I>OK</I> button will add the resulting EV to a list displayed at the bottom of the GUI panel. Any EV in this list may be selected and either edited or deleted using the associated <I>edit</I> and <I>delete</I> buttons. When all conditions have been modeled and any EVs for noise modeling have been specified, the model can be estimated by clicking the <I>estimate</I> button."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpSetupTempDerivative
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpSetupTempDerivative { } {
    #--- Setup->Signal
    #--- What is adding temporal derivatives?
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Temporal derivatives</H3>
<P> Adding temporal derivatives to a stimulus function, or a stimulus function convolved with the HRF is a another way of creating an EV with a small onset delay for each stimulus presentation, in effort to better fit the observed voxel timecourse data.
<P> Once all signal modeling for an EV has been completed, clicking the <I>OK</I> button will add the resulting EV to a list displayed at the bottom of the GUI panel. Any EV in this list may be selected and either edited or deleted using the associated <I>edit</I> and <I>delete</I> buttons. When all conditions have been modeled and any EVs for noise modeling have been specified, the model can be estimated by clicking the <I>estimate</I> button."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt
}

#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpSetupHighpassFilter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpSetupHighpassFilter { } {
    #--- Setup->Signal
    #--- What is highpass filtering?
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>High-pass filtering</H3>
<P> In fMRI data, the voxel timecourse may contain drift that manifests as a slow intensity variation over time. One way to remove this nuisance signal is to introduce low frequency EVs into the linear model which will capture slowly varying processes in the data. Cosines, polynomials, splines or exploratory functions are often used for this purpose, though it remains difficult to specify a drift model that is equally valid for every fMRI dataset.
<P> The fMRIEngine currently provides a set of commonly used Discrete Cosine Transform basis functions to model drift. High-pass filtering the data with the default <I>DCbasis</I> generates a set of seven additional explanatory variables that separate low frequencies (below the frequency corresponding to the lowest rate of stimuli presentation in the entire paradigm) from the signal of interest in the observed data. These additional EVs will show up as extra columns in the design matrix.
<P> The default cutoff period for high-pass filtering is set (as recommended in S.M. Smith, 'Preparing fMRI data for statistical analysis', in <I>Functional MRI, an introduction to methods</I>, P. Jezzard, P.M. Matthews, and S.M. Smith, Eds., 2002, Oxford University Press) at 1.5 times the maximum time interval between the most infrequently occuring event or epoch in the paradigm, multiplied by TR. (The reciprocal of this value represents the cutoff frequency in Hz.) The cutoff period is displayed in the entry widget, where the user may also specify a different cutoff period instead.
<P> Once a condition has been modeled, clicking the <I>OK</I> button will add the resulting EV to a list displayed at the bottom of the GUI panel. Any EV in this list may be selected and either edited or deleted using the associated <I>edit</I> and <I>delete</I> buttons. Once all conditions have been modeled and any EVs for noise modeling have been specified, the model can be estimated by clicking the <I>estimate</I> button."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpSetupLowpassFilter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpSetupLowpassFilter { } {
    #--- Setup->Signal
    #--- What is Lowpass filter?
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Low-pass filtering</H3>
<P> Not yet available."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpSetupGlobalFX
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpSetupGlobalFX { } {
    #--- Setup->Signal
    #--- What is remove global effects?
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Removing global effects</H3>
<P> Not yet available"
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpSetupCustomFX
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpSetupCustomFX { } {
    #--- Setup->Signal
    #--- What are custom regresssors?
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Specifying Custom Regressors</H3>
<P> Not yet available."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpSetupEstimate
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpSetupEstimate { } {
    #--- Setup->Signal
    #--- Estimate the model's fit to the data.
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Model estimation</H3>
<P> Estimating the model computes the best fit of the design matrix to the observed data, determining the amount that each column in the design matrix contributes to the overall observed voxel timecourse. Currently, the fMRIEngine uses multi-parameter fitting functionality provided by the Gnu Scientific Library (www.gnu.org) to perform a least-squares fit to a general linear model y = XB + e at each voxel.
<P> In this equation, y is the observed voxel timecourse; X is the matrix of explanatory variables and B are the unknown best-fit parameters to be estimated. GSL finds the best fit by minimizing the sum of squared residuals, chi^2, with respect to the parameters B. The result is a set estimated parameters B^ which represent the experimental effect for each column of the design matrix, and which are used in all subsequent statistical testing of the associated model.
<P> The model may be estimated after signal modeling is complete and either before or after specifying contrasts; but estimation must occur before any activation volumes can be generated."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt    
}

#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpSetupContrasts
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpSetupContrasts { } {
    #--- Setup->Contrasts
    #--- Number of elements = columns in design matrix
    #--- and autofill with zeros
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Contrast specification</H3>
<P> To determine whether one EV is more related to the data than another, known as <I>contrasting</I> EVs, a <I>contrast vector</I> containing one element for every EV in the design matrix is used to describe which EVs to compare.
<P> For instance, a contrast vector that compares one stimulus type (e.g. EV1) to the baseline uses '1' for the vector element representing the design matrix column in which EV1 appears, and '0' for vector elements representing every other column. To compare EV1 to another stimulus type (e.g. EV2), EV1's contrast value may be specified as '-1', EV2's contrast value as '1', and all vector elements representing other columns of the design matrix are specified as '0'. All contrasts are given a name and a vector specification in the Contrasts GUI panel; the fMRIEngine will automatically add any non-specified trailing zeros to a contrast vector, so only the elements up to and including the last non-zero element need to be entered.
<P> Once each contrast has been named and defined, clicking the <I>OK</I> button will add the resulting contrast to a list displayed at the bottom of the GUI panel. Any contrast in this list may be selected and either edited or deleted using the associated <I>edit</I> or <I>delete</I> buttons. It is useful to use the design matrix as visual reference (by clicking the <I>Show model</I> button) while specifying each contrast vector. As contrasts are specified, clicking the <I>update</I> button on the popup model view window will visually display all defined contrasts below the design matrix.
<P> All defined contrasts are used to determine evidence of effects in the <I>Compute</I> tab's GUI panel."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt    
}

#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpComputeActivationVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpComputeActivationVolume { } {
    #--- Compute->computing
    #--- How to threshold activation?
    #--- Relationship between p-value, t-value
    #--- what is activation scale?
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Computing an activation volume</H3>
<P> After both estimating the model (computing the parameters B^) and defining contrasts, a two-sided T-test can be computed to test for evidence of each effect of interest. The output of each statistical test is a volume of T-statistics that represent the probability of brain activation at individual voxels.
<P> To produce a statistical map from the fMRIEngine's <I>Compute</I> GUI, a contrast is selected and an associated output activation volume is named. The <I>compute</I> button generates the named volume of statistics and displays it in Slicer's main Viewer using a rainbow color palette; blue color indicates voxels with a lower T-score and red color indicates voxels with a high T-score. To view brain activation in the context of the subject's anatomy, this volume may be overlayed onto a co-registered high resolution structural scan in Slicer's Viewer. For display purposes, the window, level and threshold of the activation volume may be adjusted -- without altering the underlying T-statistics -- using the sliders in the <I>Display</I> tab of the Volumes module.
<P>In the fMRIEngine's <I>View</I> panel, the p-values may be thresholded to an appropriate significance level."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt    
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpViewActivationThreshold
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpViewActivationThreshold { } {
    #--- View 
    #--- How to threshold activation?
    #--- Relationship between p-value, t-value
    #--- what is activation scale?
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Activation thresholding</H3>
<P> Once a statistical map of T-values has been generated, the volume may be thresholded to determine, at a given level of significance, which voxels in the brain were activated. There are different methods for thresholding the activation volume; currently the fMRIEngine uses the simplest method which applies the chosen threshold to every voxel in the map. This method is likely to generate some false positives, however, given the large number of voxels being tested. Options to reduce the number of false positives, such as applying a Bonferroni correction, are currently being developed.
<P> Note: adjusting the window, level and threshold of this volume for display purposes (using the <I>Display</I> tab of the Volumes module) has no numerical effect on the underlying T-statistics represented in the volume."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt    

}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpViewHighPassFiltering
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpViewHighPassFiltering { } {
    #--- View 
    #--- Default cutoff frequency
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Default cutoff frequency</H3>
<P> If high pass filtering is turned on but the cutoff frequency is not valid, a default frequency will be provided. The following steps describe how this value is computed:<BR><BR>
<B>1.</B> From the paradigm design, find the minimum interval (<B>mi</B>) between consecutive presentation of trials of the same type<BR>
<B>2.</B> Cutoff period (<B>cp</B>) = 2 * <B>mi</B> (seconds) <BR>
<B>3.</B> Cutoff frequency (<B>cf</B>) = 1 / <B>cp</B> (cycles per second) <BR>"
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt    
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineHelpViewPlotting
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineHelpViewPlotting { } {
    #--- View 
    #--- Types of plotting
    set i [ fMRIEngineGetHelpWinID ]
    set txt "<H3>Voxel timecourse plotting</H3>
<P> Sometimes visual inspection of the voxel timecourse can reveal trends in the data that may not be well represented by an activation detection algorithm; looking at an individual voxel's response, or the collective response of a cluster of voxels in the form of a timecourse plot might help to disambiguate false positives from actual brain activations, for instance. Several modes of voxel timecourse plotting are available: <I>timecourse</I>, <I>peristimulus historgram</I>, and <I>ROI</I>.
<P><B>Timecourse plot</B>
<P> In this plotting mode (previously called <I>voxel-natural</I>), the observed timecourse of a selected voxel is plotted horizontally along the time axis, and superimposed on a plot of the paradigm signal as reference. Different stimulus conditions within the paradigm are represented by different colors in the plot. Clicking on the voxel timecourse plot itself reveals the numerical voxel values at that timepoint.
<P><B>Peristimulus histogram plot</B>
<P> In this plotting mode (previously called <I>voxel-combined</I>), samples from the timecourse of a selected voxel are divided into bins representing each stimulus condition in the paradigm. Each bin contains the combined spans of time during which a stimulus condition of one type was presented. The values of samples spanning the timecourse of each bin are averaged together to depict an average response to the associated stimulus, and response maximum and minimum values are plotted around the average for each sample point.
<P><B>ROI plot</B>
<P> In the ROI plot (which is not yet implemented), timecourses of all voxels defined within a region of interest are averaged and plotted horizontally along the time axis, and superimposed on a plot of the paradigm signal as reference. As in the natural-timecourse plot, different conditions within the paradigm are represented by different colors in the plot. Clicking on the ROI averaged timecourse plot itself reveals the numerical voxel values at that timepoint."
    DevCreateTextPopup infowin$i "fMRIEngine information" 100 100 25 $txt    
}

