# shell script must be first allowed: chmod +x aQC_selectedRunList.sh
#!/bin/bash
# to run it do (inside ali shell):
# ./aQC_selectedRunList.sh

# settings:
plotSingle=""
plotComparison="True"

# input file selection:
#inputFile='singleRun.txt'
## aQC meeting 2023-01-24:
#inputFile='LHC22i1_passMC_O2-3486.txt'
#inputFile='LHC22o_O2-3496_gr01.txt'
#inputFile='LHC22o_O2-3496_gr02.txt'
## aQC meeting 2023-01-31:
#inputFile='LHC22o_O2-3496_gr03.txt'
#inputFile='LHC22r_O2-3526_gr01.txt'
## aQC meeting 2023-02-07:
#inputFile='LHC22p_O2-3529_gr01.txt'
#inputFile='LHC22r_O2-3526_gr02.txt'
#inputFile='LHC22t_O2-3530_gr01.txt'
## aQC meeting 2023-02-14:
#inputFile='LHC22o_O2-3496_gr04a.txt'
#inputFile='LHC22o_O2-3496_gr04b.txt'
#inputFile='LHC22o_O2-3496_gr04c.txt'
#inputFile='LHC22o_O2-3496_test.txt
#inputFile='LHC22o_O2-3496_HIR.txt'
#inputFile='LHC22q_O2-3548_gr01.txt'
#inputFile='LHC22r_O2-3526_gr03.txt'
#inputFile='LHC22t_O2-3530_gr02.txt'
#inputFile='LHC22o_O2-3496_rest.txt'
## aQC meeting 2023-02-21:
# nothing
## aQC meeting 2023-02-28:
#inputFile='LHC22m_O23570_gr01.txt'
#inputFile='LHC22p_O2-3573_gr01.txt'
#inputFile='LHC22q_O2-3569_gr01.txt'
#inputFile='LHC22r_O2-3574_gr01.txt'
#inputFile='LHC22t_O2-3578_gr01.txt'
## aQC meeting 2023-03-07
#inputFile='LHC22o_O2-3597.txt'
#inputFile='LHC22r_O2-3574.txt'
#inputFile='LHC22t_O2-3578.txt'
## aQC meeting 2023-03-14
#inputFile='LHC22o_O2-3597_01.txt'
#inputFile='LHC22o_O2-3597_02.txt'
inputFile='LHC22o_O2-3597_03.txt'
#inputFile='LHC22r_O2-3574.txt'
#inputFile='LHC22t_O2-3578.txt'

root -q 'aQC_downloadHistos.cxx("'inputRunLists/$inputFile'")'
root -q 'aQC_plotRunGroup.cxx("'inputRunLists/$inputFile'")'

n=1 
period=""
pass0=""
pass1=""
pass2=""
pass3=""
pass4=""
nRuns=0
run=500000
currentPass=""

printNotebook_singlePass () {
    file=${run}_${currentPass}
    papermill -p _period $period -p _pass $currentPass -p _run $run notebook_aQC_singlePass.ipynb ${file}.ipynb
    if [ $currentPass = "passMC" ] ; then
        jupyter nbconvert --to html ${file}.ipynb --no-input 
    else
        jupyter nbconvert --to html ${file}.ipynb --no-input --TagRemovePreprocessor.remove_cell_tags onlyMC
    fi
    rm -r ${file}.ipynb
    mv ${file}.html results/${period}/notebook_singlePass/${file}.html
}

while read line; do
    if (( $n == 1 )) ; then
        period=$line
        echo "period: $period"
    elif (( $n == 2 )) ; then
        pass0=$line
        echo "pass 0: $pass0"
    elif (( $n == 3 )) ; then
        pass1=$line
        echo "pass 1: $pass1"
    elif (( $n == 4 )) ; then
        pass2=$line
        echo "pass 2: $pass2"
    elif (( $n == 5 )) ; then
        pass3=$line
        echo "pass 3: $pass3"
    elif (( $n == 6 )) ; then
        pass4=$line
        echo "pass 4: $pass4"
    elif (( $n == 7 )) ; then
        nRuns=$line
        echo "# runs: $nRuns"
    else
        run=$line
        echo ""
        echo "run $run"
        # QC plots for individual passes
        mkdir -p results/${period}/notebook_singlePass/
        # pass zero
        if [[ $pass0 != "none" && $plotSingle == "True" ]] ; then
            currentPass=$pass0
            printNotebook_singlePass
        fi
        # pass one
        if [[ $pass1 != "none" && $plotSingle == "True" ]] ; then
            currentPass=$pass1
            printNotebook_singlePass
        fi
        # pass two
        if [[ $pass2 != "none" && $plotSingle == "True" ]] ; then
            currentPass=$pass2
            printNotebook_singlePass
        fi
        # pass three
        if [[ $pass3 != "none" && $plotSingle == "True" ]] ; then
            currentPass=$pass3
            printNotebook_singlePass
        fi
        # pass four
        if [[ $pass4 != "none" && $plotSingle == "True" ]] ; then
            currentPass=$pass4
            printNotebook_singlePass
        fi
        # do the comparison
        if [[ $plotComparison == "False" ]] ; then
            echo "No passes selected for comparison."
        else
            mkdir -p results/${period}/notebook_compPasses/
            file=${run}_comp
            # create the notebook for this configuration (pass all the arguments)
            papermill -p _period $period -p _pass1 $pass0 -p _pass1 $pass1 -p _pass2 $pass2 -p _pass3 $pass3 -p _pass4 $pass4 -p _run $run notebook_aQC_compPasses.ipynb ${file}.ipynb
            # convert it to html
            jupyter nbconvert --to html ${file}.ipynb --no-input
            rm -r ${file}.ipynb
            mv ${file}.html results/${period}/notebook_compPasses/${file}.html
            
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