Local Extrema Algorithm
(of a function under a given condition)
Step 1:
Glue the lagrange function:
L = f + λF

!! (we also have at this step to store in memory the number of parameters;
ex: pairs of (x,y); (x,y,z) etc and then.. we will add λ as parameter too)

p = number of parameters
m = number of outputted functions

condition for this to work: m < p.

for each parameter in the set of parameters (ex: x,y,z,λ; x,y,λ)
we have to do all the partial derivatives with respect to each parameter.
!!! NOTE !!! the number of param is equal with p + 1 (+1 is from λ)

then we make each equation equal with 0 and solve the system. (this is inside the for loop)

Step 2:
Use lambda that we have found (in the solved system) in order to determine critical points.
!!! this step may be devided in smaller steps, where we treat lambda 1, lambda 2 and so on.
(this applies if we have more values of lambda that give more crit points)
