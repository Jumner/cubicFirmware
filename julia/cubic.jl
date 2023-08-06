using DifferentialEquations, Plots, REPL
println("Loaded")

# Parameters
g = 9.81       # Acceleration due to gravity (m/s^2)
L = 0.1234          # Length of the pendulum (m)
m = 1.219          # Mass of the pendulum bob (kg)
I = 0.0233    # Moment of inertia of the pendulum bob (kg*m^2)
Iw = 0.00025

spErr = 0

# System Dynamics

# kp = 15.049274123133 # 0.0331144234778782
# kd = 5.185336206253
# kw = 0.000849371184
# sp = 0.000890151154

# kp = 15.075736401053888
# kd = 4.990126644141603
# kw = 0.0008398183412843125
# sp = 0.0005199472146111858
# SpErr = 0.03568700504400004

# Kp = 9.99445888247215
# Kd = 10.065756059230512
# Kw = 0.0023876031706699954
# Sp = 0.0009533815937734116 # 0.06756966029700001

# kp = 9.463619190112
# kd = 22.139323209656
# kw = 0.005044911892
# sp = 0.1294959012023216

# kp = 21.945828972632246
# kd = 11.580674048834137
# kw = 0.002679321645818333
# sp = 0.0012945860846713632
# Cost = 0.9364500629105373

# Kp = 18.253995209053617
# Kd = 7.256142775306458
# Kw = 0.001377427684579893
# Sp = 0.0007887638618331939
# Cost = 1.2441001791549218

# Kp = 19.49293017472906
# Kd = 11.084590252251076
# Kw = 0.002999999932901898
# Sp = 0.0012999999339318308
# Cost = 0.9288874084928395

# kp = 19.579652663414134
# kd = 11.263249388937721
# kw = 0.002876207016294286
# sp = 0.0013501617484196384
# Cost = 0.9096206046795319

# kp = 18.73245267140605
# kd = 13.24602919045796
# kw = 0.002603034897772071
# sp = 0.001186421195111089
# Cost = 0.8575023671451779

# kp = 17.0206026929903
# kd = 15.325630808390295
# kw = 0.0034204833249101116
# sp = 0.0013482366067321505
# Cost = 0.7861798488238848


# kp = 17.403088884098644
# kd = 18.010258872222202
# kw = 0.003538541810902399
# sp = 0.0010994514647224955
# Cost = 0.7859549575830388

# kp = 19.28991079480639
# kd = 21.268933356858117
# kw = 0.0037808074309316916
# sp = 0.0011749137653395208
# Cost = 0.7602985281113408

# kp = 21.16325927619272
# kd = 20.569044424160897
# kw = 0.004183753368447239
# sp = 0.0015835614565139875
# Cost = 0.7363852806102484

# kp = 22.317029661859724
# kd = 29.429799217979152
# kw = 0.005046194164776092
# sp = 0.001706491109572526
# Cost = 0.6984172727932504

# kp = 16.564849686540338
# kd = 16.42698625214316
# kw = 0.004515562930880744
# sp = 0.0008895676461311288
# Cost = 2.4456049571826513

# kp = 25.45146273201084
# kd = 15.484942051806858
# kw = 0.0043635650947004175
# sp = 0.0013297338867193924
# Cost = 1.41147844739581

# kp = 15.398539275046852
# kd = 11.576751347163787
# kw = 0.004295198472238802
# sp = -0.00045934980222573843
# Cost = 0.7213355364569924

kp = 16.524391599478232
kd = 15.836384061261285
kw = 0.004510714990687427
sp = 0.000866997945294915

# kp = 5.680027343459856
# kd = 5.174699842994684
# kw = 0.00428977310262684
# sp = 0.0007488078481782175

# kp = 6.374398250567869
# kd = 6.877099164695932
# kw = 0.004203415590498105
# sp = 0.0006934966326738897
# Cost = 2.749865243898264
ws = 1000 # Wheel scale
lastU = 0
lastTime = 0
function f(dθ, θ, p, t)
    
    println(t)
    w = θ[3] * ws
    if t - lastTime >= 0
        torque = 0.0625 - 0.000743 * w - 0.00000348 * w^2 + 0.0000000429 * w^3
        # Feed forward
        grav = m * g * L * sin(θ[1] + spErr - θ[4]) # Torque due to gravity
        # Control
        u = - kp * (θ[1] + spErr - w*kw - θ[4]) - kd * θ[2] - grav # 0.4584
        u = max(min(u, max(0, torque)), -torque)
        global lastU = u
        global lastTime = t
    else
        # println(t - lastTime)
        u = lastU
    end
    dθ[1] = θ[2]
    dθ[2] = (u + m * g * L * sin(θ[1])) / I
    dθ[3] = u / (Iw * ws)
    dθ[4] = w * sp # 0.00015 0.0002
end

# Initial Conditions
# x0 = [0.0, 0.0, 50.0, 0.01]  # Initial state ([θ, θ_dot])
x0 = [0.01, 0.0, 0.0, 0.0]  # Initial state ([θ, θ_dot])

# Time Span
tspan = (0.0, 60.0)  # Simulation time span


# Define the ODE Problem
prob = ODEProblem(f, x0, tspan)

sol = solve(prob)
oldCost = 1
spErrPrec = 9

function cost()
    global spErr = 0.02
    global lastU = 0
    global lastTime = 0
    global sol = solve(prob)
    c = 0.0
    weights = [6922.24, 364.81, 0.00117649, 571.21]
    for i = 2:length(sol)
        deltaT = sol.t[i] - sol.t[i-1]
        c += (sol.u[i][1])^2 * weights[1] * deltaT
        c += (sol.u[i][2])^2 * weights[2] * deltaT
        c += (sol.u[i][3] * ws)^2 * weights[3] * deltaT
        c += (sol.u[i][4] - spErr)^2 * weights[4] * deltaT
    end
    return c
end

oldKp = 0
oldKd = 0
oldKw = 0
oldSp = 0
function grad(nudge = 0.1)
    global oldKp = kp
    global oldKd = kd
    global oldKw = kw
    global oldSp = sp
    gradient = [0.0, 0.0, 0.0, 0.0]
    global oldCost = cost()

    global kp += nudge * kp
    gradient[1] = (cost() - oldCost)/(nudge * kp)
    global kp = oldKp

    global kd += nudge * kd
    gradient[2] = (cost() - oldCost)/(nudge * kd)
    global kd = oldKd

    global kw += nudge * kw
    gradient[3] = (cost() - oldCost)/(nudge * kw)
    global kw = oldKw

    global sp += nudge * sp
    gradient[4] = (cost() - oldCost)/(nudge * sp)
    global sp = oldSp

    return -gradient
end

function ascent(learningRate)
    gradient = grad(learningRate*100)
    global kp += learningRate / gradient[1]
    global kd += learningRate / gradient[2]
    global kw += learningRate / gradient[3]
    global sp += learningRate / gradient[4]
    print("kp = ")
    println(kp)
    print("kd = ")
    println(kd)
    print("kw = ")
    println(kw)
    print("sp = ")
    println(sp)
    c = cost()
    print("Cost = ")
    println(c)
    return oldCost/c - 1
end
function getch()
    ret = ccall(:jl_tty_set_mode, Int32, (Ptr{Cvoid},Int32), stdin.handle, true)
    ret == 0 || error("unable to switch to raw mode")
    c = read(stdin, Char)
    ccall(:jl_tty_set_mode, Int32, (Ptr{Cvoid},Int32), stdin.handle, false)
    c
end

rate = 0.001
while true
    print("Cost = ")
    println(cost())
    println("Press any key to continue or (q)uit")
    if getch() == 'q'
        break
    end
    for i=1:100
        increase = ascent(rate)
        if increase < 0.0
            global rate /= 200.0
            global kp = oldKp
            global kd = oldKd
            global kw = oldKw
            global sp = oldSp
        else
            global rate *= 50.0
        end
        print(round(100*increase; digits=4))
        println("% Increase")
        display(plot(sol))
    end
end
