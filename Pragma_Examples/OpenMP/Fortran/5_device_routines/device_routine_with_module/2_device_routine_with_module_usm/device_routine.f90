! This example was created by Johanna Potyka
! Copyright (c) 2025 AMD HPC Application Performance Team
! MIT License

      program device_routine
      !----------------------------
      ! description: this program demonstrates
      !              how to call a device subroutine
         use omp_lib
         use computemod, only: compute

         implicit none

         !$omp requires unified_shared_memory

         !---variables
         integer,parameter :: N=1000
         !N                   number of values in x array
         integer,parameter :: rk=kind(1.0d0)
         !rk                  kind of real
         integer :: k, err_stat
         !k         index
         !err_stat  status variable


         real(kind=rk),dimension(:),ALLOCATABLE :: x
         !x                                        array
         real(kind=rk) :: sum
         !sum             used to sum up x


         allocate(x(1:N), STAT=err_stat)
         if(err_stat /= 0) then
             write(*,*) "error during allocation"
             STOP
         end if

         !---initialisation
         !$omp target teams distribute parallel do
         do k=1,N
           x(k) = -1.0_rk
         end do
         !--- call a device subroutine in kernel
         !$omp target teams distribute parallel do
         do k=1,N
            call compute(x(k))
         end do

         !--- initialize sum
        sum = 0.0_rk;

        !--- sum up x to sum on device with reduction
        !$omp target teams distribute parallel do reduction(+:sum)
        do k=1,N
           sum = sum + x(k)
        end do
        !$omp end target teams distribute parallel do
        !--- print result
        Write(*,'(A,F0.12)') "Result: sum of x is ",sum

        deallocate(x)
      end program device_routine
