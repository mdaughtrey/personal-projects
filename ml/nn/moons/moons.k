#!/bin/env k.exe
/ https://stackabuse.com/creating-a-neural-network-from-scratch-in-python/
/\d .nn
/\l "../nnlib"
/\d .k
/
\e 1
/train: -1 784 # _ic 16_ 6: "t10k-images-idx3-ubyte"
/labels: _ic 8_ 6: "t10k-labels-idx1-ubyte"
/.nn.init[#*train; 10; #*labels; 4000]
/w: .()
/w: .nn.train[w; train; labels]
//.nn.test[w] .' +(x;y)

/ https://realpython.com/python-ai-neural-network/#the-process-to-train-a-neural-network

. "\\r ",$-__t%1000
/p 12

/ Sigmoid Activation
a:{1%1+_exp -x}
ad:{:a[x]*(1-a[x])}

/a:{_tanh x}
/ad:{1.0 - _tanh[x]^2}
/dotp:{+|+/''x*\:/:|+y}

n:{[w;b;i]
    :a@b++/w*i
}

train:{[d;iter]
    ni:#**d
    /`0: ,,/$("num inputs ";ni)
    .k.w:(2*(ni _draw r)%r:1000000)-1
    .k.b: *(1 _draw r)%r
    .k.lr:0.1
	/`0: ,,/$("Init .k.w: ";5:.k.w)

    do[iter
    {[d]
        / Feedforward
        p: n[.k.w;.k.b;*d]                          
        /`0: ,/$(" p [";5:p;"] x [";5:d[1];"]")
        / Backprop 1
        e: p-d[1]
        /`0: ,/$(" mse [";e*e;"]")
        / Backprop 2
        adj: +/'(*d)*zd:e*ad p                     
        /`0: ,,/$(" adj [";5:adj;"]")
        .k.w-:adj*.k.lr
        .k.b-:zd}'d
    ]
}

train_and_test:{[d]
    train[d;1000]
/    p:n[.k.w;.k.b;*x]
    errors:_abs'(n[.k.w;.k.b]'d[;0])-d[;1]
    `0: ,,/$("minerr ";*errors@<errors;" maxerr ";*errors@>errors;" avgerr ";(+/errors)%#errors)

/    {`0: ,,/$("input ";5:*x;" predicted ";5:p;" expected ";x 1;" error ";p:n[.k.w;.k.b;*x]-x 1)}'d
}

test_moons:{
    cut:{1_'(&y=x)_ y:x,y}
    features: 0.0$(cut[","]'0:"features.csv")[;0 1]
    labels: 0$*:'cut[","]'0:"labels.csv"
    train_and_test[(,:'features),/:'labels]
/    1/0
}

test_moons[]

_exit 0

