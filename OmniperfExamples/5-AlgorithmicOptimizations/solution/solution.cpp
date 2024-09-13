#include"hip/hip_runtime.h"
#include<chrono>
#include"hipCheck.h"
#include<cmath>

__global__ 
__launch_bounds__(256) void yax(double* y,
		                double* A,
		                double* x,
		                int n, int m, 
		                double* result){
  __shared__ double res;
  if(threadIdx.x == 0) res = 0;
  
  for(int i = blockIdx.x; i < n; i += gridDim.x){
    double temp = 0.0;

    for(int j = threadIdx.x; j < m; j+=blockDim.x){
      temp += A[i*m + j] * x[j];
    }
    unsafeAtomicAdd(&res,temp);
    if(threadIdx.x == 0) res *= y[i];
  }
  if(threadIdx.x==0) unsafeAtomicAdd(&result[0],res);
}

int main(int argc, char** argv){
  dim3 grid = dim3(2048,1,1);
  dim3 block = dim3(64,1,1);
  int n = 2<<14;
  int m = 2<<14;
  
  double* y;
  double* x;
  double* A;
  double* result;
  
  hipCheck(hipMalloc(&y, n*sizeof(double)));
  hipCheck(hipMalloc(&x, m*sizeof(double)));
  hipCheck(hipMalloc(&A, n*m*sizeof(double)));
  hipCheck(hipMalloc(&result, sizeof(double)));
  
  for(int i = 0; i < n; i++){
    y[i] = 1;
  }
  for(int i = 0; i < m; i++){
    x[i] = 1;
  }
  for(int i = 0; i < n*m; i++){
    A[i] = 1;
  }
  result[0] = 0.0;


  yax<<<grid,block>>>(y,A,x,n,m,result);
  hipDeviceSynchronize();
  result[0] = 0.0;
  
  auto start = std::chrono::high_resolution_clock::now();
  yax<<<grid,block>>>(y,A,x,n,m,result);
  hipDeviceSynchronize();
  auto stop = std::chrono::high_resolution_clock::now();
  
  double expected = (double)n * (double)m;
  if(std::abs(result[0] - (double)n*(double)m) >= 0.0001) {
    printf("Answer is incorrect!\n");
    printf("result = %f, expected = %f\n",result[0],expected);
  } else {
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() * 1e-6;
    printf("yAx time: %f milliseconds\n", time);
  }

  hipFree(y);
  hipFree(x);
  hipFree(A);

  return 0;
}

