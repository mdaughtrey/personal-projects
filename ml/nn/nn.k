\e 1
. "\\r ",$_t
sigmoid:{1%1+_exp[-x]}
sigmoidd:{_exp[-x]%(1+_exp[-x])^2}
dp:{+/''+x*\:/:+y}

/def gaussian(u1,u2):
/  z1 = sqrt(-2*log(u1))*cos(2*pi*u2)
/    z2 = sqrt(-2*log(u1))*sin(2*pi*u2)
/      return z1,z2

x0:((0 0 0);(0 0 1);(0 1 0);(0 1 1);(1 0 0);(1 0 1);(1 1 0);(1 1 1))
y0:((0);(1);(0);(1);(0);(1);(1);(1))

inputs: 3
outputs: 1
hiddens: 4

gaussian:{[u0;u1]
    pi: 3.14159265358
    z0:_sqrt[(-2)*_log[u0]]*_cos[2*pi*u1]
    z1:_sqrt[(-2)*_log[u0]]*_sin[2*pi*u1]
    :(z0;z1)
}

alpha: 0.05
iterations: 4000
cost:()

doit:{
    w1:(inputs;hiddens)#,/gaussian .' -1 2 #((inputs*hiddens) _draw _1e6)%1e6
    b1:hiddens#1.0

    w2:(hiddens;outputs)#,/gaussian .' -1 2 #((hiddens*outputs) _draw _1e6)%1e6
    b2:outputs#1.0

    do[iterations
        / Forward propagation
        a1: b1+/:dp[x0;w1]
        z1:sigmoid'a1
        a2: b2+/:dp[z1;w2]
        ypred:sigmoid'a2

        / Back propagation
        d2: (-(y0-ypred)) * sigmoidd'a2
        d_jw2: dp[+z1;d2]
        d1: dp[d2; +w2] * sigmoidd'a1
        d_jw1: dp[+x0;d1]

        / Tweak the weights
        w1-:alpha * d_jw1
        w2-:alpha * d_jw2

        / Track the cost
        cost,: 0.5 * +/(y0 - ypred)^2   / j = cost
    ]
    .k.w1: w1
    .k.w2: w2
    `0: ,,/$("w1: ";5:w1)
    `0: ,,/$("w2: ";5:w2)
}

doit`
/ cost
