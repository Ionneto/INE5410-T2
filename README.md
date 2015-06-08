# INE5410-T2
A weird mix of bucket sort, bubble sort and MPI.

it generates a randon array of numbers, split it into buckets, send the buckets to slvae processes trough MPI, runs bubble sort in every bucket, sens all of them back to the master and then join then in a single organized array.

Use mpicc and mpirun for it to work properly, or at all actually.
