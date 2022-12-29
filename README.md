# cubicFirmware
A repo for the firmware of my Cubic robot
## Phase 1
Initially, I attemped to jump straight into code. This is really stupid. Here is what went wrong:
- kd was far too high, response was overdamped. This is unstable and quickly saturated the reaction wheels. To "solve" this, I made the setpoint move proportional to the wheel rate. It worked, but it was far from robust.
- My state estimator definitely introduced too much delay and is what caused me to increase kd to damp the unstable occilations.
- When balancing on the corner, there are 2 angles (x,y) and 3 wheels. When it was on the edge, wheel rate was part of the state. This breaks when going for the corner because the 2 states must share the 3 wheels. Each state cannot have its own wheel.
Overall, I learned a ton. I realized I need to start over spend more time planning
## Phase 2
Here is my plan to do this properly:
1. Create simplified ODEs to model the system and convert them to a transfer function
2. Analyze the unstable open loop system
3. Close the loop with a PID controller
4. Tune PID with loop shaping
5. Analyze stability of closed loop system
If the time margin is too small:
6. Model the gyroscopic effect and other known disturbances and convert to a transfer function
7. Add feedforward control to account for the nonlinearities and increase time margin
8. Design a state estimator with a known latency well below the time margin.
9. Analyze performance with expected delay
10. Implement and test in hardware

Here is a simple block diagram to show the system with feedforward
``` mermaid
    direction LR
    
    kalman: State Estimator
    ff: Feed Forward
    Disturbances --> Plant
    Disturbances --> ff
    ff --> Plant : u
    
    PID --> Plant : u
    Plant --> kalman : y
    kalman --> PID : x
```
