EXECUTABLE = ./interop
all: $(EXECUTABLE) 

OBJECTS = interop.o interop_hip.o

ROCM_GPU ?= $(strip $(shell rocminfo |grep -m 1 -E gfx[^0]{1} | sed -e 's/ *Name: *//'))
OPENMP_FLAGS = -fopenmp --offload-arch=${ROCM_GPU}

CXXFLAGS = -g -O2 -fPIC ${OPENMP_FLAGS} -D __HIP_PLATFORM_AMD__
HIPCC_FLAGS = -g -O2 -DNDEBUG -fPIC -I${ROCM_PATH}/include 

HIPCC ?= hipcc

ifeq ($(HIPCC), nvcc)
   HIPCC_FLAGS += -x cu
   LDFLAGS = -lcudadevrt -lcudart_static -lrt -lpthread -ldl
endif
ifeq ($(HIPCC), hipcc)
   HIPCC_FLAGS += -munsafe-fp-atomics 
   LDFLAGS = ${OPENMP_FLAGS} -L${ROCM_PATH}/lib -lamdhip64
endif

interop_hip.o: interop.hip
	$(HIPCC) $(HIPCC_FLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm -f $(EXECUTABLE)
	rm -f $(OBJECTS)
