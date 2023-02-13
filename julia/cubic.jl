# include("cubic.jl")
println("Starting...")
using ControlSystems
using Plots
using LinearAlgebra: I # Matrix(I,3,2)
using LaTeXStrings # L"\frac{1}{x}"
println("Libraries Loaded")

P = tf(1.0,[1,0.5,1])
println("System $P")
u(x,t) = [t>=10]
t = 0:0.1:30
x0 = [0.25,0]
result = lsim(P,u,t,x0=x0)
_, b, c, _ = result # y,t,x,u
display(plot(b,c', label=["Position" "Velocity"], xlabel="Time (s)"))

println("Done")

# A\[1;2;3] solves the system
