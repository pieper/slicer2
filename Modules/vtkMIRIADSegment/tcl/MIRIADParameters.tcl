
#
# MIRIAD Parameters - testing harness to drive the MIRIADSegment process
#


package require iSlicer


proc MIRIADParametersDefaults {} {

    upvar #0 MIRIADParameters mp  ;# for typing simplicity and readability

    set mp(pclasses) {
        Air
        OtherTissue
        CSF
        CorticalGrayMatter
        SubCorticalGrayMatter
        NormalWhiteMatter
        LesionedWhiteMatter
    }

    # some possibly reasonable default values
    foreach pc $mp(pclasses) {
        set mp($pc,prob) 0.2
        set mp($pc,ProbDataWeight) 1.0
        set mp($pc,InputChannelWeight,PD) 1.0
        set mp($pc,InputChannelWeight,T2) 1.0
        set mp($pc,logmeans,PD) 4.5678 
        set mp($pc,logmeans,T2) 4.2802 
        set mp($pc,logcov,PD) 7.6805 
        set mp($pc,logcov,cross) 5.0207
        set mp($pc,logcov,T2) 7.3293
    }

    set mp(Air,prob) 0.05
    set mp(Air,logmeans,PD) 2.6544 
    set mp(Air,logmeans,T2) 2.5259
    set mp(Air,logcov,PD) 0.7816
    set mp(Air,logcov,cross) 0.6958
    set mp(Air,logcov,T2) 1.1833

    set mp(OtherTissue,prob) 0.2
    set mp(OtherTissue,logmeans,PD) 6.3429 
    set mp(OtherTissue,logmeans,T2) 5.1534
    set mp(OtherTissue,logcov,PD) 0.1243
    set mp(OtherTissue,logcov,cross) 0.1911
    set mp(OtherTissue,logcov,T2) 0.3905

    set mp(CSF,prob) 0.25
    set mp(CSF,logmeans,PD) 4.5678 
    set mp(CSF,logmeans,T2) 4.2802
    set mp(CSF,logcov,PD) 7.6805
    set mp(CSF,logcov,cross) 5.0207
    set mp(CSF,logcov,T2) 7.3293
    set mp(CSF,InputChannelWeight,PD) 0.2
    set mp(CSF,InputChannelWeight,T2) 1.0

    set mp(NormalWhiteMatter,prob) 0.25
    set mp(NormalWhiteMatter,logmeans,PD) 4.5678 
    set mp(NormalWhiteMatter,logmeans,T2) 4.2802
    set mp(NormalWhiteMatter,logcov,PD) 7.6805
    set mp(NormalWhiteMatter,logcov,cross) 5.0207
    set mp(NormalWhiteMatter,logcov,T2) 7.3293
    set mp(NormalWhiteMatter,InputChannelWeight,PD) 0.2
    set mp(NormalWhiteMatter,InputChannelWeight,T2) 1.0

    set mp(LesionedWhiteMatter,prob) 0.25
    set mp(LesionedWhiteMatter,logmeans,PD) 6.3836
    set mp(LesionedWhiteMatter,logmeans,T2) 5.1253
    set mp(LesionedWhiteMatter,logcov,PD) 0.0026
    set mp(LesionedWhiteMatter,logcov,cross) 0.004
    set mp(LesionedWhiteMatter,logcov,T2) 0.0562
    set mp(LesionedWhiteMatter,InputChannelWeight,PD) 0.2
    set mp(LesionedWhiteMatter,InputChannelWeight,T2) 1.0
}

proc MIRIADParameters {} {


    catch "destroy .mp"






}

