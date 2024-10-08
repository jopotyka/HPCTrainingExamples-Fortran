//**************************************************************************
//* Copyright (c) 2023, Advanced Micro Devices, Inc. All rights reserved.
//**************************************************************************

#include "Jacobi.hpp"

// AU_i,j = (-U_i+1,j + 2U_i,j - U_i-1,j)/dx^2 +
//          (-U_i,j+1 + 2U_i,j - U_i,j-1)/dy^2

void BoundaryConditions(mesh_t& mesh,
                        const dfloat _1bydx2,
                        const dfloat _1bydy2,
                        dfloat* U,
                        dfloat* AU) {

  const int Nx = mesh.Nx;
  const int Ny = mesh.Ny;
  for (int id=0;id<2*Nx+2*Ny-2;id++) {

    //get the (i,j) coordinates of node based on how large id is
    int i, j;
    if (id < Nx) { //bottom
      i = id;
      j = 0;
    } else if (id<2*Nx) { //top
      i = id - Nx;
      j = Ny-1;
    } else if (id < 2*Nx + Ny-1) { //left
      i = 0;
      j = id - 2*Nx + 1;
    } else { //right
      i = Nx-1;
      j = id - 2*Nx - Ny + 2;
    }

    const int iid = i+j*Nx;

    //check if node is one the boundary and use haloBuffer's data (=0 here) if so
    const dfloat U_d = (j==0)    ?  0.0 : U[iid - Nx];
    const dfloat U_u = (j==Ny-1) ?  0.0 : U[iid + Nx];
    const dfloat U_l = (i==0)    ?  0.0 : U[iid - 1];
    const dfloat U_r = (i==Nx-1) ?  0.0 : U[iid + 1];

     AU[iid] = (-U_l + 2*U[iid] - U_r)*_1bydx2 +
               (-U_d + 2*U[iid] - U_u)*_1bydy2;
  }
}
