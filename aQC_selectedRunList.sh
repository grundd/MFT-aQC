# shell script must be first allowed: chmod +x aQC_run.sh
#!/bin/bash
# to run it do (inside ali shell):
# ./aQC_run.sh

inputFile='singleRun.txt'
## aQC meeting 2023-01-24:
#inputFile='LHC22i1_passMC_O2-3486.txt'
#inputFile='LHC22o_apass2_O2-3496_group1.txt'
#inputFile='LHC22o_apass2_O2-3496_group2.txt'
## aQC meeting 2023-01-31:
#inputFile='LHC22o_apass2_O2-3496_group3.txt'
#inputFile='LHC22r_apass2_O2-3526_group1.txt'
## aQC meeting 2023-02-07:
#inputFile='LHC22p_apass2_O2-3529_group1.txt'
#inputFile='LHC22r_apass2_O2-3526_group2.txt'
#inputFile='LHC22t_apass2_O2-3530_group1.txt'
## aQC meeting 2023-02-14:
#inputFile='LHC22o_apass2_O2-3496_group4a.txt'
#inputFile='LHC22o_apass2_O2-3496_group4b.txt'
#inputFile='LHC22o_apass2_O2-3496_group4c.txt'
#inputFile='LHC22o_apass2_test_O2-3496.txt'
#inputFile='LHC22o_HIR_apass2_O2-3496.txt'
#inputFile='LHC22q_apass2_O2-3548_group1.txt'
#inputFile='LHC22r_apass2_O2-3526_group3.txt'
#inputFile='LHC22t_apass2_O2-3530_group2.txt'
## aQC meeting 2023-02-21:

root -q 'aQC.cxx("'inputRunLists/$inputFile'")'

n=1 
period=""
pass1=""
pass2=""
pass3=""
nRuns=0
run=500000
currentPass=""

printNotebook_singlePass () {
    file=MFTaQC_${run}_${currentPass}
    papermill -p _period $period -p _pass $currentPass -p _run $run notebook_aQC_singlePass.ipynb ${file}.ipynb
    if [ $currentPass = "passMC" ] ; then
        jupyter nbconvert --to html ${file}.ipynb --no-input 
    else
        jupyter nbconvert --to html ${file}.ipynb --no-input --TagRemovePreprocessor.remove_cell_tags onlyMC
    fi
    rm -r ${file}.ipynb
    mv ${file}.html results/${period}/notebook_singlePass/${run}_${currentPass}.html
}

while read line; do
    if (( $n == 1 )) ; then
        period=$line
        echo "period: $period"
    elif (( $n == 2 )) ; then
        pass1=$line
        echo "pass1:  $pass1"
    elif (( $n == 3 )) ; then
        pass2=$line
        echo "pass2:  $pass2"
    elif (( $n == 4 )) ; then
        pass3=$line
        echo "pass3:  $pass3"
    elif (( $n == 5 )) ; then
        nRuns=$line
        echo "# runs: $nRuns"
    else
        run=$line
        echo ""
        echo "run $run"
        # QC plots for individual passes
        mkdir -p results/${period}/notebook_singlePass/
        # pass one
        if [ $pass1 != "none" ] ; then
            currentPass=$pass1
            printNotebook_singlePass
        fi
        # pass two
        if [ $pass2 != "none" ] ; then
            currentPass=$pass2
            printNotebook_singlePass
        fi
        # pass three
        if [ $pass3 != "none" ] ; then
            currentPass=$pass3
            printNotebook_singlePass
        fi
        # if other passes available, do the comparison
        if [ $pass2 == "none" ] ; then
            echo "No passes selected for comparison."
        else
            # comparison of the two passes
            mkdir -p results/${period}/notebook_compPasses/
            file=MFTaQC_${run}_${pass1}_vs_${pass2}
            papermill -p _period $period -p _pass1 $pass1 -p _pass2 $pass2 -p _pass3 $pass3 -p _run $run notebook_aQC_compPasses.ipynb ${file}.ipynb
            jupyter nbconvert --to html ${file}.ipynb --no-input
            rm -r ${file}.ipynb
            mv ${file}.html results/${period}/notebook_compPasses/${run}_${pass1}_vs_${pass2}.html
        fi
    fi
    n=$((n+1))
done < inputRunLists/$inputFile

## sources:
# https://stackoverflow.com/questions/36901154/how-export-a-jupyter-notebook-to-html-from-the-command-line
# https://stackoverflow.com/questions/49907455/hide-code-when-exporting-jupyter-notebook-to-html
# https://www.wrighters.io/parameters-jupyter-notebooks-with-papermill/
# https://pypi.org/project/jupyter-runner/ 
# https://stackoverflow.com/questions/31517194/how-to-hide-one-specific-cell-input-or-output-in-ipython-notebook