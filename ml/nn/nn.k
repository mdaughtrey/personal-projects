\e 1
`0: ,,/$("nn")
. "\\r ",$_t
sigmoid:{1%1+_exp(-x)}
dp:{+/''+x*\:/:+y}

x0:((0 0 0);(0 0 1);(0 1 0);(0 1 1);(1 0 0);(1 0 1);(1 1 0);(1 1 1))
y0:((0);(1);(0);(1);(0);(1);(1);(1))

inputs: 3
outputs: 1
hiddens: 4

doit:{
    w1: .[;();%;1e6](inputs;hiddens) # (inputs*hiddens) _draw _1e6
    b1: hiddens#1.0

    w2: .[;();%;1e6](hiddens;outputs) # (hiddens*outputs) _draw _1e6
    b2: outputs#1.0

    a1: x0 _dot w1
/    a1:dp[x0;w1] + b1
    1/0
}




