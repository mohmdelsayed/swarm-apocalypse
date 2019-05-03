f(r,sigma,epsilon, exponent, AttractionGain)=4*epsilon*r*((sigma/r)**(exponent*2)-AttractionGain*(sigma/r)**exponent)

sigma=250
epsilon=10 # represents number of neighbors agents
exponent = 2
AttractionGain = 0.9


set label 1 "(r_0, {/Symbol e}_0)" at first 3,0 center offset 0,1

set xlabel "Distance r_0 (cm)" 
set ylabel "Energy E (J/mol)

set xr [100:1000]
set yr [-5000:1000]

start=0.0; end=1.0; inc=0.1
list = ""; a=start-inc; while (a<end) {list=list.sprintf(" %.3f",a=a+inc)}

plot for [epsi in list] f(x,sigma,epsi, exponent, AttractionGain) title sprintf("MLJ   {/Symbol e}=%s   {/Symbol s}=%.1d",epsi,sigma)

set term eps
set output "gain.eps" 
replot
set term x11
