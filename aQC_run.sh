# shell script must be first allowed: chmod +x aQC_run.sh
#!/bin/bash
# to run it do (inside ali shell):
# ./aQC_run.sh

period="LHC22o"
pass1="apass1"
pass2="apass2"
run=526647

papermill -p _period $period -p _run $run notebook_aQC_compPasses.ipynb MFTaQC_${run}_${pass1}_vs_${pass2}.ipynb
jupyter nbconvert --to html MFTaQC_${run}_${pass1}_vs_${pass2}.ipynb --no-input

