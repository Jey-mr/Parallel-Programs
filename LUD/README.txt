There is a bash file named compile, it will compile and run three programs with various inputs
and threads(there will be three warnings it is nothing but clearing the output file). There will 
be no output printed on display rather it will be saved in three files such as output, pthread, 
openmp.

The following algorithm has been implemented:

  inputs: a(n,n)
  outputs: p(n), l(n,n), and u(n,n)

  initialize p as a vector of length n
  initialize u as an n x n matrix with 0s below the diagonal
  initialize l as an n x n matrix with 1s on the diagonal and 0s above the diagonal

  for i = 1 to n
  	p[i] = i

  for k = 1 to n
  	max = 0
 	for i = k to n
 		if max < |a(i,k)|
 			max = |a(i,k)|
 			k’ = i

 	if max == 0
 		error (singular matrix)

 	swap p[k] and p[k’]
 	swap a(k,:) and a(k’,:)
 	swap l(k,1:k-1) and l(k’,1:k-1)

 	u(k,k) = a(k,k)

 	for i = k+1 to n
		l(i,k) = a(i,k)/u(k,k)
 		u(k,i) = a(k,i)

 	for i = k+1 to n
 		for j = k+1 to n
 			a(i,j) = a(i,j) - l(i,k)*u(k,j)

Here, the vector p is a compact representation of a permutation matrix p(n,n),
which is very sparse. For the ith row of p, p(i) stores the column index of
the sole position that contains a 1.

