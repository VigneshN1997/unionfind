mpirun -pernode -np 11 -hostfile ~/hostlist ./simple 10000000 5000000
mpirun -pernode -np 11 -hostfile ~/hostlist ./pc 10000000 5000000
mpirun -pernode -np 11 -hostfile ~/hostlist ./mod 10000000 5000000