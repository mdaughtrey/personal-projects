/ https://medium.com/swlh/how-to-build-a-neural-network-from-scratch-b712d59ae641
\e 0
. "\\r ",$_t
sigmoid:{1%1+_exp[-x]}
sigmoidd:{_exp[-x]%(1+_exp[-x])^2}
.k.dp:{+/''+x*\:/:+y}

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

/ iin = a1 -> z1
/ iact = z1 -> a1
/ hin = a2 -> z2 -> ypred
/ hact = ypred
fprop:{[x0;w] / fprop
    w[`iout]: w[`b1]+/:_dot[w`w1]'x0       / input layer output
    w[`iact]:sigmoid'w[`iout]              / input layer activated
    w[`hout]: w[`b2]+/:_dot[w`w2]'w`iact   / hidden layer output
    w[`hact]:sigmoid'w[`hout]              / hidden layer activated
    :w
}

bprop:{[x0; y0; w] / bprop
    d2: (-(y0-w`hact)) * sigmoidd'w`hout
    w[`d2]: _dot[,/d2;w`iact]
    d1: (_dot[+w`w2]'d2) * sigmoidd'w[`iout]
    w[`d1]:_dot[d1]'+x0
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

        / Tweak the weights
        w[`w1]-:alpha * w[`d1]
        w[`w2]-:alpha * w[`d2]

        / Track the cost
        .k.cost,: c:0.5 * +/(y0 - w`hact)^2
        `0: ,,/$("Cost ";5:c)
    ]
    .k.w: w
}

test:{[i;p]
    r: fprop[,i;.k.w]`hact
    `0: ,,/$("Input ";5:i;" predicted ";p;" result ";5:*r)
}

\b 2
/\b doit *
/\b fprop *
/\b bprop *

doit`
test .' +(x0;y0)

