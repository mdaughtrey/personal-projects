/ https://welcomege.github.io/code-a-neural-network-from-scratch/
\e 1
. "\\r ",$_t
sigmoid:{1%1+_exp[-x]}
sigmoidd:{_exp[-x]%(1+_exp[-x])^2}
.k.dp:{+/''+x*\:/:+y}

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

fprop:{[x0;w] / fprop
    w[`z1]: w[`b1]+/:.k.dp[x0;w[`w1]]
    w[`h1]:sigmoid'w[`z1]
    w[`z2]: w[`b2]+/:.k.dp[w[`h1];w[`w2]]
    w[`h2]:sigmoid'w[`z2]
    :w
}

/bprop:{[x0; y0; w] / bprop
/    d2: (-(y0-w[`ypred])) * sigmoidd'w[`a2]
/    w[`d_jw2]: .k.dp[+w[`z1];d2]
/    d1: .k.dp[d2; +w[`w2]] * sigmoidd'w[`a1]
/    w[`d_jw1]: .k.dp[+x0;d1]
/    :w

bprop:{[x0; y0; w] / bprop
    d2: (w[`h2]-y0) * w[`h2] * (1-w[`h2])
    d1: .k.dp[d2;+w[`w2]] * w[`h1] * (1 - w[`h1])
    w[`dw2]: .k.dp[+w[`h1]; d2]
    w[`db2]: +/d2
    w[`dw1]: .k.dp[+x0;d1]
    w[`db1]: +/d1
    :w
}

.k.cost: ()

doit:{
    w: .()
    w[`w1]:(inputs;hiddens)#,/gaussian .' -1 2 #((inputs*hiddens) _draw _1e6)%1e6
    w[`b1]:hiddens#1.0
    w[`w2]:(hiddens;outputs)#,/gaussian .' -1 2 #((hiddens*outputs) _draw _1e6)%1e6
    w[`b2]:outputs#1.0

    do[iterations
        / fprop propagation
        w: fprop[x0; w]

        / Back propagation
        w: bprop[x0; y0; w]

        / adjust
        w[`w1`b1`w2`b2]-: alpha *' w[`dw1`db1`dw2`db2]
/        w[`w1]-: alpha * w[`dw1]
/        w[`b1]-: alpha * w[`db1]
/        w[`w2]-: alpha * w[`dw2]
/        w[`b2]-: alpha * w[`db2]

        / Tweak the weights
/        w[`w1]-:alpha * w[`d_jw1]
/        w[`w2]-:alpha * w[`d_jw2]

        / Track the cost
        .k.cost,: 0.5 * +/(y0 - w[`h2])^2   / j = cost
    ]
    .k.w: w
/    `0: ,,/$("w1: ";5:w1)
/    `0: ,,/$("w2: ";5:w2)
}

predict:{[a;w]
    z1: _dot[a; w[`w1]] + w[`b1]
    h1: sigmoid z1
    z2: .k.dp[w[`h1];h[`w2]] + w[`b2]
    h2: sigmoid z2
    :h2
}

\b 2
/\b doit *
/\b fprop *
/\b bprop *

doit`
predict[x0 2;.k.w]
/ cost
