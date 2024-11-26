#!/bin/env k.exe
/ https://stackabuse.com/creating-a-neural-network-from-scratch-in-python/
/\d .nn
/\l "../nnlib"
/\d .k
/
/\e 1
/train_images: 0.01+'(0.99%255)*' -1 784 # _ic 16_ 6: "t10k-images-idx3-ubyte"
train_images: 0N 784 # 16_ `i$1:"t10k-images-idx3-ubyte"
\train_images
1/0
train_images: -1 784 # 0.01+(0.99%255) * _ic 16_ 6: "t10k-images-idx3-ubyte"
1/0

train_labels: _ic 8_ 6: "t10k-labels-idx1-ubyte"
m:1+*train_labels@>train_labels
train_onehot:{.[m#0.01;x;:;0.99]}'train_labels
1/0

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
    `0: ,,/$("num inputs ";ni)
    .k.w:(2*(ni _draw r)%r:1000000)-1
    .k.b: *(1 _draw r)%r
    .k.lr:0.1
	`0: ,,/$("Init .k.w: ";5:.k.w)

    do[iter
    {[d]
        / Feedforward
        p: n[.k.w;.k.b;*d]                          
        `0: ,/$(" p [";5:p;"] x [";5:d[1];"]")
        / Backprop 1
        e: p-d[1]
        `0: ,/$(" mse [";e*e;"]")
        / Backprop 2
        adj: +/'(*d)*zd:e*ad p                     
        `0: ,,/$(" adj [";5:adj;"]")
        .k.w-:adj*.k.lr
        .k.b-:zd}'d
    ]
}

train_and_test:{[d]
    train[d;1000]
    {`0: ,,/$("input ";5:*x;" predicted ";5:n[.k.w;.k.b;*x];" expected ";x 1)}'d
}

test_tanh:{
    train_and_test@(((0 0 1);0);((1 1 1);1);((1 0 1);1);((0 1 1);0))
}


test_and:{
    train_and_test@(((0;0);0);((0;1);0);((1;0);0);((1;1);1))
}

test_or:{
    train_and_test@(((0;0);0);((0;1);1);((1;0);1);((1;1);1))
}

test_bcd:{
    train_and_test@((0 0;0);(0 1;0.25);(1 0;0.5);(1 1;0.75))
}
/test_and[]
test_or[]
/test_tanh[]
/test_bcd[]

_exit 0

