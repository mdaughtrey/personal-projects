#!/bin/env k.exe
/ https://realpython.com/python-ai-neural-network/#the-process-to-train-a-neural-network

. "\\r ",$-__t%1000
/p 12

/ Sigmoid Activation
/a:{1%1+_exp -x}
/ad:{:a[x]*(1-a[x])}

a:{_tanh x}
ad:{1.0 - _tanh[x]^2}
/dotp:{+|+/''x*\:/:|+y}

n:{[w;b;i]
    :b+a@+/w*i
}

train:{[d;iter]
    ni:#**d
    `0: ,,/$("num inputs ";ni)
    .k.w:(2*(ni _draw r)%r:1000000)-1
    .k.b:0.0 / *(1 _draw r)%r
    .k.lr:0.01
	`0: ,,/$("Init .k.w: ";5:.k.w)

    do[iter
    {[d]
        p: n[.k.w;.k.b;*d]                          
        `0: ,/$(" p [";5:p;"] x [";5:d[1];"]")
        e: d[1]-p                                   
        `0: ,/$(" e [";5:e;"]")
        adj: +/'(*d)*e*ad p                     
        `0: ,,/$(" adj [";5:adj;"]")
        .k.w+:adj}'d
    ]
}

test_tanh:{
    `0: ,"test_tanh and"
    data_test:(((0 0 1);0);((1 1 1);1);((1 0 1);1);((0 1 1);0))
    /data_and:(((0;0);0);((0;1);0);((1;0);0);((1;1);1))
    train[data_test;100]

    {`0: ,,/$("input ";5:*x;" predicted ";5:n[.k.w;.k.b;*x];" expected ";x 1)}'data_test
/    `0: n[(1;0;0);.k.w;.k.b]
/    `0: ,,/$("0,0 ";5:n[,(0;0);.k.w;.k.b])
/    `0: ,,/$("0,1 ";5:n[,(0;1);.k.w;.k.b])
/    `0: ,,/$("1,0 ";5:n[,(1;0);.k.w;.k.b])
/    `0: ,,/$("1,1 ";5:n[,(1;1);.k.w;.k.b])
/    `0: ,"test_and"
}


test_and:{
    `0: ,"train and"
    data_test:(((0;0);0);((0;1);0);((1;0);0);((1;1);1))
    train[data_test;1000]
    {`0: ,,/$("input ";5:*x;" predicted ";5:n[.k.w;.k.b;*x];" expected ";x 1)}'data_test
}

/test_or:{
/    `0: ,"train_or"
/    data_or:(((0;0);0);((0;1);1);((1;0);1);((1;1);1))
/    train[data_or;1000]
/    `0: ,,/$("0,0 ";p[(0;0);.k.w;.k.b])
/    `0: ,,/$("0,1 ";p[(0;1);.k.w;.k.b])
/    `0: ,,/$("1,0 ";p[(1;0);.k.w;.k.b])
/    `0: ,,/$("1,1 ";p[(1;1);.k.w;.k.b])
/    `0: ,"test_or"
/}
/test_and[]
/test_or[]
test_tanh[]

_exit 0
