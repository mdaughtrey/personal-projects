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

/ Sigmoid Activation and derivative
act:{1%1+_exp -x}
ad:{:act[x]*(1-act[x])}

/a:{_tanh x}
/ad:{1.0 - _tanh[x]^2}
/dotp:{+|+/''x*\:/:|+y}

n:{[w;b;i]
    :a@b++/w*i
}

n0:{[w;b;i] :b++/w*i }


train:{[d;iter]
    ni:#**d / number of inputs
    nh: 4 / number of hidden layer neurons
    no: 1 / number of output neurons
    /`0: ,,/$("num inputs ";ni)
    r:1000000
    .k.hw: (nh;-1)#(2*((ni*nh) _draw r)%r)-1    / hidden weights
    .k.hb: (nh _draw r)%r                       / hidden bias
    .k.ow: *(no;-1)#(2*((no*nh) _draw r)%r)-1   / output weights
    .k.ob: (no _draw r)%r                       / output bias
    .k.lr:0.5
	`0: ,,/$("Init .k.hw: ";5:.k.hw;".k.hb: ";5:.k.hb)
	`0: ,,/$("Init .k.ow: ";5:.k.ow;".k.ob: ";5:.k.ob)

    do[iter
        {[d]
            / Feedforward
            ha: act'hdp:n0 .' (+.k[`hw`hb]),\:,*d / hdp = hidden dot product, ha = hidden activation
            `0: ,,/$("hdp ";5:hdp;" ha ";5:ha)
            oa: act'odp:*n0[.k.ow;.k.ob;ha]       / odp = output dot product, oa = output activation
            mse:(d[1]-oa)^2
            `0: ,,/$("Output activation ";5:oa;" expected ";d[1];" MSE ";mse)
            / Back propogation for the output layer
            c: oa-d[1]        / cost
            dodp: ad odp       / derivative odp
            cow: +/ha*(c*dodp) / cost for output weight
            `0: ,,/$("c ";c;" dodp ";5:dodp;" cow ";5:cow)

            / Back propogation for the hidden layer
            l:c*ad odp
            k:+/l*.k.ow
            j:(ad hdp)*k
            i:+/d[0]*j

            chw: +/d[0]*a

            .k.hw-:chw*.k.lr
            .k.ow-:cow*.k.lr

        }',*d
    ]
    _exit 0

/    do[iter
/    {[d]
/        / Feedforward
/        p: n[.k.w;.k.b;*d]                          
/        /`0: ,/$(" p [";5:p;"] x [";5:d[1];"]")
/        / Backprop 1
/        e: p-d[1]
/        /`0: ,/$(" mse [";e*e;"]")
/        / Backprop 2
/        adj: +/'(*d)*zd:e*ad p                     
/        /`0: ,,/$(" adj [";5:adj;"]")
/        .k.w-:adj*.k.lr
/        .k.b-:zd}'d
/    ]
}

train_and_test:{[d]
    train[d;10]
/    p:n[.k.w;.k.b;*x]
    errors:_abs'(n[.k.hw;.k.hb]'d[;0])-d[;1]
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

