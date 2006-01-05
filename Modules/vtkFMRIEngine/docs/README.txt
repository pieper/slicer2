This file describes how to use it.

1. Load/select a sequence of fMRI volumes

Click the Sequence tab and you should see Select and Load sub-tabs.
The Load tab is used to load a new sequence of volumes while Select 
tab is used to select a sequence which is already loaded from ibrowser
module or within fMRIEngine itself.

1) Select tab

On Select tab, press Update button to see if you have any sequence
available. If you really have, you should see sequence names listed
in the box. If you have only one sequence, press Select button to 
select it. If you have multiple sequences available, highlight one
in the box and then press the Select button.

Slide Vol Index scale to navigate the selected multi-volume sequence.

Set window, level and low/high threshold for the first volume within 
the selected sequence. Press the "Set Window/Level/Thresholds" button to 
set the same values for the entire sequence.

2) Load tab

If you need load a sequence, go to Load tab. Currently you can load
the sequences represented by the following:
a. A single volume by a pair of 3D Analyze files (hdr/img) 
b. A list of 3D Analyze pair files in the same directory. Each pair is a volume.
c. A single pair of 4D Analyze files (hdr/img)
d. A single bxh file which can represent a single volume or a list of 
volumes in one of the following format: 3D Analyze and dicom 

Thus, you can Browse an hdr or bxh file and choose a file filter:
Load a single file: Only read the above input file.
Load multiple files: Read files in the same directory matching the pattern 
in the Filter field. For instance, if you have following files in a directory:

image_003.hdr
image_003.img
image_004.hdr
image_004.img
......

rimage_003.hdr
rimage_003.img
rimage_004.hdr
rimage_004.img
......

You may input r*, r*.*, or r*.hdr to load "rimage_*" sequence. 

Hit Apply button or press Enter in the Filter field to start reading files. 
Active Voume Name should be updated during loading.

When loading is done, the first volume of the sequence is displayed in the
View window and the Volume Index slider is updated. Slide the slider to
navigate thru the volume sequence.

Adjust the properties of the first volume (window, level, thresholds) as
needed. The button "Set Win/Lev/Thresholds" should set the adjustments for
all volumes in the sequence.


2. Compute activation

There are also two sub-tabs in Compute tab: Paradigm and Detectors. Click
Paradigm to load your paradigm text file or input values from the interface.

On "Input from user" tab, press Done button after you finish typing values.
If you want to save those values for later use, press Save button to save
them into a file.

We will implement multiple detectors; however, the GLM is only available
at this moment. So, you don't need to do anything with "Choose a detector" 
for now on Detectors tab. Input a volume name for your activation. Hit Compute
button to run.


3. Display activation

Upon completion of computation, the activation volume is by default
active. Set properties for it so that you can see activation overlay on
the background volume (either structural or fmri). Make sure the low 
threshold is set to at least 1.

Go to Display tab. Slide the scale and you see activation changes
according to p value and t Statistic.

Choose your time course plotting option. If your option is not None, you should
get a plot as you move the mouse over any of the three slice windows.

