# Ibalance
## Introduction
This project is for CS6491. The main introduction has been stated in the submission document. Here are some additional notes.

This demo actually comes from a project I am working on.The main idea that using ILP to balance the SV graph is proposed by my supervisor Dr. Li. 

The **output** content includes some test files. 

my.checked.lh: The original file to construct a graph.

myseek.lh and myseek.mps.gz: Input file for Cbc ILP solver.

myseek.sol: Solution offered by Cbc

my.balance.lh: Final graph balanced by Cbc(generated by parseILP script)

This project is constructed by cmake. If you want to run this demo project, you need install Cbs lib(https://github.com/coin-or/Cbc)

## Run
step 1: Ibalance check output/my.checked.lh output/myseek

step2: cbc output/myseek.lp solve solu output/myseek.sol

step3: python main.py parseILP -i output/my.checked.lh -s output/myseek.sol -o output/my.balanced.lh
