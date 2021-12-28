/ https://medium.com/swlh/how-to-build-a-neural-network-from-scratch-b712d59ae641
/ nn0 converted to a reusable k lib

sigmoid:{1%1+_exp[-x]}
sigmoidd:{_exp[-x]%(1+_exp[-x])^2}


config:.((`alpha; 0.05)
(`iterations; 4000)
(`numin; 0)
(`numhidden; 0)
(`numout; 0)
(`iterations; 4000)
(`actfun; sigmoid)
(`actfund; sigmoidd)
(`alpha; 0.05))

cost:()
gaussian:{[u0;u1]   / gaussian
    pi: 3.14159265358
    z0:_sqrt[(-2)*_log[u0]]*_cos[2*pi*u1]
    z1:_sqrt[(-2)*_log[u0]]*_sin[2*pi*u1]
    :(z0;z1)
}

/ iin = a1 -> z1
/ iact = z1 -> a1
/ hin = a2 -> z2 -> ypred
/ hact = ypred
fprop:{[x;w] / fprop
    w[`iout]: w[`b1]+/:_dot[w`w1]'x       / input layer output
    w[`iact]:config.actfun'w[`iout]    / input layer activated
    w[`hout]: w[`b2]+/:_dot[w`w2]'w`iact   / hidden layer output
    w[`hact]:config.actfun'w[`hout]    / hidden layer activated
    :w
}

bprop:{[x; y; w] / bprop
    1/0
    d2: (-(y-w`hact)) * config.actfund'w`hout
    w[`d2]: _dot[w`iact]',/d2

    d1: (_dot[,/d2]'+w`w2) * config.actfund'w[`iout]
    
/    w[`d2]: _dot[,/d2;w`iact]
/    d1: (_dot[+w`w2]'d2) * config.actfund'w[`iout]
    w[`d1]:_dot[d1]'+x
    :w
}

train:{[w; x; y] / train
    i: config.numin
    h: config.numhidden
    o: config.numout
    w[`w1]:(i;h)#,/gaussian .' -1 2 #((i*h) _draw _1e6)%1e6
    w[`b1]:h#1.0
    w[`w2]:(h;o)#,/gaussian .' -1 2 #((h*o) _draw _1e6)%1e6
    w[`b2]:o#1.0

    do[config.iterations
        / fprop propagation
        `0: "fprop..."
        w: fprop[x; w]
        / Back propagation
        `0: ,"bprop"
        w: bprop[x; y; w]

        / Tweak the weights
        w[`w1]-:config.alpha * w[`d1]
        w[`w2]-:config.alpha * w[`d2]

        / Track the cost
        1/0
        cost,: c:0.5 * +/(y - w`hact)^2
        `0: ,,/$("Cost ";5:c;", ";5:_s)

    ]
    "weights" 1: w
    :w
}

init:{[input; hidden; output; iterations] / init
    config[`numin`numhidden`numout`iterations]: (input; hidden; output; iterations)
    . "\\r ",$_t
}

test:{[w;i;p] / test
    r: fprop[,i;w]`hact
    `0: ,,/$("Input ";5:i;" predicted ";p;" result ";5:*r)
}

