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

using DifferentialEquations, Plots, REPL
println("Loaded")

# Parameters
g = 9.81       # Acceleration due to gravity (m/s^2)
L = 0.1234          # Length of the pendulum (m)
m = 1.219          # Mass of the pendulum bob (kg)
I = 0.0233    # Moment of inertia of the pendulum bob (kg*m^2)
Iw = 0.00025

# System Dynamics
# kp    = 14.7438 # (14.7438)
# kd    = 0.458400 # 764kp/24573 (0.458400)
# kw    = 0.000106228 # (0.000106206+0.000106250)/2
# sp    = 0.00005094275 # (0.0000420729+0.0000598126)/2
# kp = 15.05
# kd = 5.185336
# kw = 0.0008493
# sp = 0.0008901
# kp = 13.225482506
# kd = 1.294508716
# kw = 0.000177323
# sp = 0.00042414

# kp = 15.049274123133 # 0.0331144234778782
# kd = 5.185336206253
# kw = 0.000849371184
# sp = 0.000890151154

# Kp = 15.075736401053888
# Kd = 4.990126644141603
# Kw = 0.0008398183412843125
# Sp = 0.0005199472146111858
# SpErr = 0.03568700504400004

kp = 11.33294219907 # 0.0658250119820011
kd = 10.53018003246
kw = 0.002297301121
sp = 0.000971963255

# Kp = 9.99445888247215
# Kd = 10.065756059230512
# Kw = 0.0023876031706699954
# Sp = 0.0009533815937734116 # 0.06756966029700001

# kp = 9.463619190112
# kd = 22.139323209656
# kw = 0.005044911892
# sp = 0.1294959012023216

# kp = 15
# kd = 5
# kw = 0.0008
# sp = 0.0009
ws = 1000 # Wheel scale
function f(dθ, θ, p, t)
    
    w = θ[3] * ws
    torque = 0.0625 - 0.000743 * w - 0.00000348 * w^2 + 0.0000000429 * w^3
    # Feed forward
    grav = m * g * L * sin(θ[1] + spErr - θ[4]) # Torque due to gravity
    # Control
    u = - kp * (θ[1] + spErr - w*kw - θ[4]) - kd * θ[2] - grav # 0.4584
    u = max(min(u, torque), -torque)
    dθ[1] = θ[2]
    dθ[2] = (u + m * g * L * sin(θ[1])) / I
    dθ[3] = u / (Iw * ws)
    dθ[4] = w * sp # 0.00015 0.0002
end

# Initial Conditions
# x0 = [0.0, 0.0, 50.0, 0.01]  # Initial state ([θ, θ_dot])
x0 = [0.0, 0.0, 0.0, 0.0]  # Initial state ([θ, θ_dot])

# Time Span
tspan = (0.0, 10.0)  # Simulation time span

# Define the ODE Problem
prob = ODEProblem(f, x0, tspan)

# Solve the ODE

function stable(sol)
    last = sol[length(sol)]
    if abs(last[1]) > 0.01
        return false
    elseif abs(last[2]) > 0.1
        return false
    elseif abs(last[3] * ws) > 5
        return false
    end
    return true
end

sol = solve(prob)
function kLimit(n, digits, max=1) # 0->kp, 1->kd, 2->kw, 3->sp, 4->spErr, Max is 1 if max -1 if min
    if n == 0
        global spErr = 0.0001
    end
    if (digits < 0)
        return
    end
    kLimit(n, digits-1, max)
    i = 0
    while true
        global sol = solve(prob)
        i = i + 1
        val = 10.0^-digits * max * (stable(sol)*2-1) # Flip for min or when we overshoot
        if n == 0
            global spErr += val
        elseif n == 1
            global kp += val
        elseif n == 2
            global kd += val
        elseif n == 3
            global kw += val
        elseif n == 4
            global sp += val
        end
        if !stable(sol)
            break
        end
    end
    if n == 0
        return spErr
    elseif n == 1
        return kp
    elseif n == 2
        return kd
    elseif n == 3
        return kw
    elseif n == 4
        return sp
    end
end

oldSpErr = 0

spErrPrec = 9

function grad(nudge = 0.001)
    gradient = [0.0, 0.0, 0.0, 0.0]

    global oldSpErr = kLimit(0,spErrPrec)
    global kp += nudge
    gradient[1] = (kLimit(0,spErrPrec) - oldSpErr)/nudge
    global kp -= nudge

    global oldSpErr = kLimit(0,spErrPrec)
    global kd += nudge
    gradient[2] = (kLimit(0,spErrPrec) - oldSpErr)/nudge
    global kd -= nudge

    global oldSpErr = kLimit(0,spErrPrec)
    global kw += nudge
    gradient[3] = (kLimit(0,spErrPrec) - oldSpErr)/nudge
    global kw -= nudge

    global oldSpErr = kLimit(0,spErrPrec)
    global sp += nudge
    gradient[4] = (kLimit(0,spErrPrec) - oldSpErr)/nudge
    global sp -= nudge

    return gradient
end

function ascent(learningRate)
    gradient = grad(0.0001)
    global kp += learningRate / gradient[1]
    global kd += learningRate / gradient[2]
    global kw += learningRate / gradient[3]
    global sp += learningRate / gradient[4]
    print("Kp = ")
    println(kp)
    print("Kd = ")
    println(kd)
    print("Kw = ")
    println(kw)
    print("Sp = ")
    println(sp)
    print("SpErr = ")
    println(kLimit(0,spErrPrec))
    return spErr/oldSpErr - 1
end
function getch()
    ret = ccall(:jl_tty_set_mode, Int32, (Ptr{Cvoid},Int32), stdin.handle, true)
    ret == 0 || error("unable to switch to raw mode")
    c = read(stdin, Char)
    ccall(:jl_tty_set_mode, Int32, (Ptr{Cvoid},Int32), stdin.handle, false)
    c
end

rate = 0.00001
while true
    println("Press any key to continue or (q)uit")
    if getch() == 'q'
        break
    end
    for i=1:100
        increase = ascent(rate)
        if increase < 0
            global rate /= 2.0
        elseif increase < 0.001
            global rate *= 1.1
        end
        print(round(100*increase; digits=2))
        println("% Increase")
        # global spErr = 0.02
        p = plot(solve(prob))
        display(p)
    end
end
