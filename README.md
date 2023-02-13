# MFT-aQC

This repository was designed to help perform asynchronous QC checks on the MFT. It allows to:
- Download QC histograms (data or MC) for a selected (period,run,pass) combination. This is implemented in 'aQC.cxx'.
- Plot QC histograms for a selected (period,run,pass) combination. Implemented in 'notebook_aQC_singlePass.ipynb'
- Show a comparison of QC histograms between different passes of a single run. Implemented in 'notebook_aQC_compPasses.ipynb'

In order to use the script, make sure you meet the following:
- O2 is installed on your machine.
- Jupyter Notebook (https://jupyter.org/install) is installed.
- Papermill python module is installed (https://papermill.readthedocs.io/en/latest/installation.html#installing-papermill).
- You can use the tunnel connection to lxplus.cern.ch (https://security.web.cern.ch/recommendations/en/ssh_tunneling.shtml). 

Usage:
1. If QC histograms need to be downloaded, open a Terminal window and connect via lxtunnel.
2. Choose an input file, i.e., a text file containing the following information on separate lines: **period**, **passes** (up to 3), **total # of runs**, and the **run list** (each run on a new line). 
    - If you want to compare asynchronous passes also with online QC plots, set the third pass name to `online`. 
    - For passes which you don't need, select `none`. 
    - Examples of input files are available in 'inputRunLists/'.
3. Store your input file in 'inputRunLists/' and modify the filename in 'aQC_selectedRunList.sh' accordingly.
4. Now you are ready to run it (step 5). If needed, the following settings can be configured:
    - If you have already downloaded the QC files and do not wish to re-write them, use `bool rewriteFiles = false` in 'aQC.cxx'.
    - An option to use the 'old' path to QC histograms can be selected using `bool oldPath = true` in 'aQC.cxx'.
    - If you want the plots from online QC to be included in the notebook showing the comparison, use `online` for the third pass in the input file and set `_drawOnlineQC = True` in 'notebook_aQC_compPasses.ipynb'
    - If you do not want the plots in the output .html files to be interactive, remove `%jsroot on` from the jupyter notebooks
5. Open another Terminal window, load O2 and execute 'aQC_selectedRunList.sh'
6. Downloaded files and printed notebooks will be stored in the 'results/' folder.