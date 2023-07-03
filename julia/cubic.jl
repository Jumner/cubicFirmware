# include("cubic.jl")
# println("Starting...")
# using ControlSystems
# using Plots
# using LinearAlgebra: I # Matrix(I,3,2)
# using LaTeXStrings # L"\frac{1}{x}"
# println("Libraries Loaded")

# P = tf(1.0,[1,0.5,1])
# A = []
# B = []
# C = []
# D = []
# P = ss()
# println("System $P")
# u(x,t) = [t>=10]
# t = 0:0.1:30
# x0 = [0.25,0]
# result = lsim(P,u,t,x0=x0)
# _, b, c, _ = result # y,t,x,u
# display(plot(b,c', label=["Position" "Velocity"], xlabel="Time (s)"))

# println("Done")

# A\[1;2;3] solves the system

# Sys

using DifferentialEquations, Plots

# Parameters
g = 9.81       # Acceleration due to gravity (m/s^2)
L = 0.1234          # Length of the pendulum (m)
m = 1.219          # Mass of the pendulum bob (kg)
I = 0.0233    # Moment of inertia of the pendulum bob (kg*m^2)
Iw = 0.00025

# System Dynamics
function f(dθ, θ, p, t)
    torque = 0.0625 - 0.000743 * θ[3] - 0.00000348 * θ[3]^2 + 0.0000000429 * θ[3]^3
    u = - 6.902 * (θ[1] + 0.01 - θ[3]*0.0001 - θ[4]) - 0.4584 * θ[2]
    u = max(min(u, torque), -torque)
    dθ[1] = θ[2]
    dθ[2] = (u + m * g * L * sin(θ[1])) / I
    dθ[3] = u / Iw
    dθ[4] = θ[3] * 0.00003 # 0.00015 0.0002
    # 0.00001 0.00005
end

# Initial Conditions
# x0 = [0.0, 0.0, 50.0, 0.01]  # Initial state ([θ, θ_dot])
x0 = [0.0, 0.0, 0.0, 0.0]  # Initial state ([θ, θ_dot])

# Time Span
tspan = (0.0, 10.0)  # Simulation time span

# Define the ODE Problem
prob = ODEProblem(f, x0, tspan)

# Solve the ODE
sol = solve(prob)

# Plotting Results
plot(sol)
