#!/bin/env k.exe
/ https://stackabuse.com/creating-a-neural-network-from-scratch-in-python/
/ https://towardsdatascience.com/coding-a-neural-network-from-scratch-in-numpy-31f04e4d605

/train: -1 784 # _ic 16_ 6: "t10k-images-idx3-ubyte"
/labels: _ic 8_ 6: "t10k-labels-idx1-ubyte"
/.nn.init[#*train; 10; #*labels; 4000]
/w: .()
/w: .nn.train[w; train; labels]
//.nn.test[w] .' +(x;y)

/ https://realpython.com/python-ai-neural-network/#the-process-to-train-a-neural-network

\e 1
. "\\r ",$-_t
\p 15

/ Sigmoid Activation and derivative
act:{1%1+_exp -x}
ad:{:act[x]*(1-act[x])}

/a:{_tanh x}
/ad:{1.0 - _tanh[x]^2}
/dotp:{+|+/''x*\:/:|+y}

n:{[w;b;x]
    :a@b++/w*x
}

/.k.dotp:{[x;y] :{_dot[x]'y}'x}
.k.sigmoid.Z:{[w;b;x] :b++/w*x }
.k.sigmoid.A: {1%1+_exp -x}
.k.sigmoid.AD: {:.k.sigmoid.A[x]*(1-.k.sigmoid.A[x])}

/ X = inputs
/ y = labels
/ W = weights
/ b = bias
/ Z = dot product (X*W) + b
/ A = activation(Z)
/ k = number of classes
/ lower case vector, upper case matrix


train:{[d;iter]
    AD: .k.sigmoid.AD
    A: .k.sigmoid.A
    Z: .k.sigmoid.Z
    Z: {[w;b;x] :+/''w*\:/:x}

    .k.d:d
    X:d[;0]
    y:,:'d[;1]

    nX:#*X / number of inputs
    nH: 4 / number of hidden layer neurons
    nO: 1 / number of output neurons
    r:_1e9

/   .k.hW:((0.78854551 0.05684808 0.69699724 0.7786954)
/       (0.77740756 0.25942256 0.37381314 0.58759964))
    .k.oW:(,0.2728219;,0.3708528;,0.19705428;,0.45985588)

    /.k.hW: (nX;-1)#(2*((nX*nH) _draw r)%r)-1    / weights for input->hidden
    .k.hW: (nX;-1)#((nX*nH) _draw r)%r    / weights for input->hidden
    /f:{ (nX;-1)#((nX*nH) _draw r)%r}    / weights for input->hidden
    .k.hb: ((nX*nH) _draw r)%r                       / hidden bias
/    .k.oW: (nH;-1)#((nO*nH) _draw r)%r   / weights for hidden->output
    .k.ob: ((nO*nH) _draw r)%r                       / output bias
    .k.lr: 0.5
	`0: ,,/$("Init .k.hW: ";5:.k.hW;".k.hB: ";5:.k.hB)
	`0: ,,/$("Init .k.oW: ";5:.k.oW;".k.oB: ";5:.k.oB)

    do[iter
		/ Feedforward
		hA: A'hZ:{_dot[x]'+.k.hW}'X
		oA: A'oZ:{_dot[x]'+.k.oW}'hA
        `0: ,,/$("MSE ";5:+/(oA-y)^2)
        / Backprop phase 1
		dcostO:_dot[(oA-y)*AD oZ]'+hA
        / Backprop phase 2
		m: _dot[+.k.oW]'(oA-y)*AD oZ
        dcostH:_dot[(AD hZ)*m]'+X
       .k.hW-:dcostH*.k.lr
       .k.oW-:dcostO*.k.lr
    ]
}

train_and_test:{[d]
    train[d;200000]
/    p:n[.k.w;.k.b;*x]
/    errors:_abs'(n[.k.hw;.k.hb]'d[;0])-d[;1]
/    `0: ,,/$("minerr ";*errors@<errors;" maxerr ";*errors@>errors;" avgerr ";(+/errors)%#errors)
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

