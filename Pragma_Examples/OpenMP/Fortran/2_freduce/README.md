porting exercise reduction:

This excercise focusses on two things:
how to port a reduction to the GPU
be careful with mapping clauses on discrete GPUs / XNACK=0

in this folder there are two versions:
0) a version to port yourself. Only port the Makefile and the reduction itself. This excercise focusses on how to implement a reduction, not on porting the full example. Decide if you want to use unified memory (export HSA_XNACK=1) or not (export HSA_XNACK=0), but for this excercise this is of minor importance.

1) a sample solution for discrete GPUs (correct output: each element 1010)
2) a sample solution for unified shared memory / APU programming model (correct output: each element 1010)
3) this example intentionally does the mapping wrong (from instead of to). You can see how the result changes (output 1000 instead of 1010) when you use export XSA_XNACK=0. No error is shown, but the result is wrong. 
Test the same wrong code with export HSA_XNACK=1, then the result is correct again as mapping clauses are ignored.
Take home message: if you develop for both APUs and discrete GPUs on MI300A, check if the results are the same for HSA_XNACK=0 and 1 as map clauses will be ignored with HSA_XNACK=1! This is great for code portability and performance without code changes, but be careful to include proper validation checks during development.