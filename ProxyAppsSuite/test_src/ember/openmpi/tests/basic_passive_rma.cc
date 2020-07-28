/*
  This simple program tests the MPI Window memory model for the system
  and MPI implementation and does one passive, non-blocking RMA operation.
  by Alex Long
  3/30/2016
*/

#include <iostream>
#include <mpi.h>

using std::cout;
using std::endl;

int main (int argc, char *argv[]) {

  MPI_Init(&argc, &argv);

  int rank, n_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_rank);

  // Get the size of MPI_DOUBLE
  int size_mpi_double;
  MPI_Type_size(MPI_DOUBLE, &size_mpi_double);
  
  double * data;
  //int n_double = 10;
  MPI_Aint n_double = 10;
  MPI_Win window;

  // allocate memory and create window
  MPI_Win_allocate(n_double*(MPI_Aint)size_mpi_double, size_mpi_double, MPI_INFO_NULL,
    MPI_COMM_WORLD, &data, &window);

  // print out the memory model of the window
  if (rank==0) {
    int *memory_model;
    int flag;
    MPI_Win_get_attr(window, MPI_WIN_MODEL, &memory_model, &flag);

    if (!flag) 
      cout<<"Key not recognized"<<endl;
    else 
      cout<<"Memory model Value: "<<*memory_model<<endl;

    if(*memory_model == MPI_WIN_SEPARATE) 
      cout<<"Memory model is separate"<<endl;
    else if(*memory_model == MPI_WIN_UNIFIED) 
      cout<<"Memory model is unified"<<endl;
  }

  // begin access epoch on all windows
  int assert_flag = 0;
  MPI_Win_lock_all(assert_flag, window);

  // request for non-blocking get 
  MPI_Request rget_request;

  //set the data in the window the rank ID
  *data = double(rank+1000);

  double recv_buffer;
  int target_rank;
  
  //select target rank
  if ( rank == n_rank-1) target_rank =0;
  else target_rank = rank+1;

  // Barrier to make sure both ranks have initialized "data"
  MPI_Barrier(MPI_COMM_WORLD);

  //read from off rank through passive, non-blocking RMA
  MPI_Rget(&recv_buffer, 1, MPI_DOUBLE, target_rank, 0L, 1, MPI_DOUBLE, window, 
    &rget_request);
  
  //MPI_Wait(&rget_request, MPI_STATUS_IGNORE);
  int finished = 0;
  while(!finished) {
    MPI_Test(&rget_request, &finished, MPI_STATUS_IGNORE);
  }

  // Print value read for each rank
  cout.flush();
  for (uint32_t i=0;i<n_rank;++i) {
    if (rank==i) {
      cout<<"Rank "<<i<<" read from rank "<<target_rank<<" the value: ";
      cout<<recv_buffer<<endl;
    }
    cout.flush();
    MPI_Barrier(MPI_COMM_WORLD);
  } 

  // unlock all windows 
  MPI_Win_unlock_all(window);

  // free memory allocated by window
  MPI_Win_free(&window);

  MPI_Finalize();
}
