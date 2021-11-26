\e 1
`0: ,,/$("nn")
. "\\r ",$_t
sigmoid:{1%1+_exp(-x)}
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

doit:{
    w1:(inputs;hiddens)#,/gaussian .' -1 2 #((inputs*hiddens) _draw _1e6)%1e6
/    w1:((0.37231356 -0.35163537 0.03774282 -0.29926134)
/        (-0.69703429 -0.80576603 0.42974398 -0.70439871)
/        (0.59554499 2.16106115 -1.00385174 0.84285487))
    b1: hiddens#1.0

    w2:(hiddens;outputs)#,/gaussian .' -1 2 #((hiddens*outputs) _draw _1e6)%1e6
    b2: outputs#1.0

    a1: b1+/:dp[x0;w1]
    z1:sigmoid'a1
    a2: b2+/:dp[z1;w2]
    ypred:sigmoid'a2
    1/0
}
doit`




