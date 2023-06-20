#!/bin/env k.exe
/ https://realpython.com/python-ai-neural-network/#the-process-to-train-a-neural-network

. "\\r ",$-__t%1000

/ Sigmoid Activation
a:{1%1+_exp -x}
ad:{:a[x]*(1-a[x])}

/ perceptron
/p:{[i;w;t] : :[t>+/i*w;0;1]}
n:{[i;w;b]
    :a b+ _dot[i;.k.w]
}
/
// is non-negative
/inn:{:p[,x;,1;0]}
/
/notf:{[x] :p[,x;,-1;-0.5] }
/
/demo_not:{
/`0: ,,/$("NOT 0: ";notf 0)
/`0: ,,/$("NOT 1: ";notf 1)
/}
/
/
train:{[d;iter]
    ni:#**d
    `0: ,,/$("num inputs ";ni)
    .k.w:(ni _draw r)%r:1000000
    .k.b:0.0 / *(1 _draw r)%r
    .k.lr:0.01
	`0: ,,/$("Init .k.w: ";5:.k.w)

    do[iter
	`0: ,"---------------"
    `0: ,/$(".k.w [";5:.k.w;"]")
    p: a'_dot[.k.w]'d[;0]
	/ pp: a'+/'.k.w*/:d[;0]
    /`0: ,,/$(" p [";5:p;"] pp [";5:pp;" x [";5:d[;1];"]")
    `0: ,,/$(" p [";5:p;"] x [";5:d[;1];"]")
    e: d[;1]-p
   `0: ,/$("e [";5:e;"]")
	adj: _dot[e*ad p]'+d[;0]
    `0: ,,/$(" adj [";5:adj;"]")
    .k.w+:adj
    ]

    / One run on one datapoint
/    train1:{[d]
/        e:p:n[d 0;.k.w;.k.b]-d 1
/        adj:2*e*d 0
/        .k.w-:adj
/        `0: ,,/$("i ";5:d 0;" p ";p;" x ";d 1;" e ";5:e;" adj ";5:adj;" .k.w ";5:.k.w;" mse ";e*e;" .k.b ";5:.k.b)
/        :e}
/
/    / One run on all datapoints
/    /while[iter & 0<+/~0=errors:train1'data;`0: ,,/$("iter ";iter;": errors ";5:errors);iter-:1]
/    /do[iter;train1'data;`0: ,,/$("iter ";iter;": errors ";5:errors);iter-:1]
/    train1[data]
/    `0: ,,/$("Weights ";5:.k.w)
/    `0: ,,/$("Bias ";.k.b)
}

test_and:{
    `0: ,"train and"
    data_and:(((0;0);0);((0;1);0);((1;0);0);((1;1);1))
    train[data_and;1000]
    `0: ,,/$("0,0 ";n[(0;0);.k.w;.k.b])
    `0: ,,/$("0,1 ";n[(0;1);.k.w;.k.b])
    `0: ,,/$("1,0 ";n[(1;0);.k.w;.k.b])
    `0: ,,/$("1,1 ";n[(1;1);.k.w;.k.b])
    `0: ,"test_and"
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
test_and[]
/test_or[]

_exit 0
