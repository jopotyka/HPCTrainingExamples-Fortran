all:openmp_code

ROCM_GPU ?= $(strip $(shell rocminfo |grep -m 1 -E gfx[^0]{1} | sed -e 's/ *Name: *//'))

FC1=$(notdir $(FC))

ifneq ($(findstring amdflang, $(FC1)),)
  OPENMP_FLAGS = -fopenmp --offload-arch=${ROCM_GPU}
  FREE_FORM_FLAG = -ffree-form
else ifneq ($(findstring flang, $(FC1)),)
  OPENMP_FLAGS = -fopenmp --offload-arch=${ROCM_GPU}
  FREE_FORM_FLAG = -ffree-form
else ifneq ($(findstring gfortran,$(FC1)),)
  OPENMP_FLAGS = -fopenmp -foffload=-march=${ROCM_GPU}
  FREE_FORM_FLAG = -ffree-form
else ifneq ($(findstring ftn,$(FC1)),)
  OPENMP_FLAGS = -fopenmp
endif

FFLAGS = -g -O3 ${FREE_FORM_FLAG} ${OPENMP_FLAGS}
ifeq (${FC1},gfortran-13)
  LDFLAGS = ${OPENMP_FLAGS} -fno-lto
else
  LDFLAGS = ${OPENMP_FLAGS}
endif

openmp_code.o: openmp_code.F90
	$(FC) -c $(FFLAGS) $^

openmp_code: openmp_code.o
	$(FC) $(LDFLAGS) $^ -o $@

# Cleanup
clean:
	rm -f *.o openmp_code *.mod
	rm -rf build
