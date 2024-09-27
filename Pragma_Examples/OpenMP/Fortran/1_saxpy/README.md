Porting saxpy step by step:

This excercise will show in a step by step solution how to port a your first kernels. 
This simple example will not use a Makefile to practice how to compile for the GPU or APU. 
All following excercises will use a Makefile.

prepare the environment:

module load rocm-afar-drop

For now, set
export HSA_XNACK=1
to make use of the APU programming model.

There are 6 different enumerated folders. (Recoomendation: vimdiff saxpy.f90 ../X_saxpy_version/saxpy.f90 may help you to see the differences):

Porting yourself:
0) the serial CPU code.
cd 0_saxpy_serial_portyourself
vi saxpy.f90
 Try to port this yourself. If you are stuck, use the step by step solution in folders 1-6 and read the instructions for those excersices below. Recommendation for your first port: use !$omp requires unified_shared memory (in the code) and export HSA_XNACK (before running) that you do not have to worry about map clauses. Steps 1-3 of the solution assume unified shared memory. Map clauses and investigating the behaviour of HSA_XNACK=0 or 1 is added in the later steps.

- Compile the serial version. Note that -fopenmp is required as omp_get_wtime is used to time the loop execution.
flang-new -fopenmp saxpy.F90 -o saxpy
- Run the serial version.
./saxpy

Step by step solution:
1) Move the computation to the device
cd 1_saxpy_omptarget
vi saxpy.f90
add !$omp target to move the loop in the saxpy subroutine to the device.
- Compile this first GPU version. Make sure you add --offload-arch=gfx942 (MI300A) or gfx90a (MI250X)
flang-new -fopenmp --ofload-arch=gfx942 saxpy.F90 -o saxpy
- Run 
./saxpy
The observed time is much larger than for the CPU version. More parallelism is required!

2) Add parallelism 
cd 2_saxpy_teamsdistribute
vi saxpy.f90
add "teams distribute"
- Compile again
flang-new -fopenmp --ofload-arch=gfx942 saxpy.F90 -o saxpy
- run again
./saxpy
The observed time is a bit better than in case 1 but still not the full parallelism is used.

3) Add multi-level parallelism
cd 3_saxpy_paralleldosimd
vi saxpy.f90 
add "parallel do" for more parellelism
- Compile again
flang-new -fopenmp --ofload-arch=gfx942 saxpy.F90 -o saxpy
- run again
./saxpy
The observed time is much better than all previous versions.
Note that the initialization kernel is a warm-up kernel here. If we do not have a warm-up kernel, the observed performance would be significantly worse. Hence the benefit of the accelerator is usually seen only after the first kernel. You can try this by commenting the !$omp target... in the initialize subroutine, then the meassured kernel is the first.

4) Explore impact of unified memory:
cd 4_saxpy_nousm
vi saxpy.f90
The !$omp requires... line is removed.
- Compile again
flang-new -fopenmp --ofload-arch=gfx942 saxpy.F90 -o saxpy
- run again
./saxpy
 
so far we worked with unfied shared memory and the APU programming model. This allows good performance on MI300A, but not on discrete GPUs. In case you will work on discrete GPUs or want to write portable code for both discrete GPUs and APUs, you have to focus on data management, too.
export HSA_XNACK=0
to get similar behaviour like on discrete GPUs (with memory copies).
Compiling and running this version without any map clauses will result in much worse performance than with unified shared memory and HSA_XNACK=1 (no memory copies on MI300A, you can validate with export LIBOMPTARGET_INFO=-1 and compare output for HSA_XNACK=0 or 1, don't forget to set it back to =0 later on, if you do not want to see all this information on the data movement and kernels anymore).

5) this version introduces  map clauses for each kernel. 
cd 5_saxpy_map 
vi saxpy.f90
see where the map clasues where added. The x vector only has to be maped "to".
- Compile again
flang-new -fopenmp --ofload-arch=gfx942 saxpy.F90 -o saxpy
- run again
./saxpy
The performance is not much better than version 4.

6) with enter and exit data clauses the memory is only moved once at the beginning the time to solution should be roughly in the order of magnitude of the unified shared memory version. 
cd 6_saxpy_targetdata 
vi saxpy.f90
- Compile again
flang-new -fopenmp --ofload-arch=gfx942 saxpy.F90 -o saxpy
- run again
./saxpy
What happens to the result, if you comment the !$omp target update clause? (don't forget to recompile after commenting it)
The results will be wrong. This shows, that proper validation of results is crutial when porting! Before you port a large app, think about your validation strategy before you start. Incremental testing is essential to capture such errors like missing data movement.

7) experiment with num_teams
cd 7_saxpy_numteams
vi saxpy.f90
specify num_teams(...) choose a number of teams you want to test 
- Compile again
flang-new -fopenmp --ofload-arch=gfx942 saxpy.F90 -o saxpy
- run again
./saxpy
investigating different numbers of teams you will find that the compiler default (without setting this) was already leading to good performance. saxpy is a very simple kernel, this finding may differ for very complex kernels.