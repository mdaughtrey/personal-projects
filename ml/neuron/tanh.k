#!/bin/env k.exe
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
    .k.b:0.0 / *(1 _draw r)%r
    .k.lr:0.1
	`0: ,,/$("Init .k.w: ";5:.k.w)

    do[iter
    {[d]
        p: n[.k.w;.k.b;*d]                          
        `0: ,/$(" p [";5:p;"] x [";5:d[1];"]")
        e: d[1]-p
        mse:e*e
        `0: ,/$(" mse [";mse;"]")
        adj: +/'(*d)*e*ad p                     
        `0: ,,/$(" adj [";5:adj;"]")
        .k.w+:adj*.k.lr}'d
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
