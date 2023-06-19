#!/bin/env k.exe
/ https://realpython.com/python-ai-neural-network/#the-process-to-train-a-neural-network

. "\\r ",$-__t%1000

/ Sigmoid Activation
a:{1%1+_exp -x}
ad:{:a[x]*(1-a[x])}

/ perceptron
/p:{[i;w;t] : :[t>+/i*w;0;1]}
n:{[i;w;b]
/    :a b++/i*w
    :b++/i*w
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
train:{[data;iter]
    ni:#**data
    `0: ,,/$("num inputs ";ni)
    .k.w:(ni _draw r)%r:1000000
    .k.b:0.0 / *(1 _draw r)%r
    .k.lr:0.01
    `0: ,,/$("Initial w ";5:.k.w;" initial bias ";.k.b)

    / One run on one datapoint
    train1:{[d]
        e:(d 1)-o:n[d 0;.k.w;.k.b]
        mse:e*e
        .k.w-:2*e / 2*e is the derivative of mse (e^2) .k.lr*e / *d 0 / ; .k.b-:.k.lr*e
        `0: ,,/$("i ";5:d 0;" o ";5:o;" e ";5:e;" .k.w ";5:.k.w;" mse ";mse;" .k.b ";5:.k.b)
        :e}

    / One run on all datapoints
    /while[iter & 0<+/~0=errors:train1'data;`0: ,,/$("iter ";iter;": errors ";5:errors);iter-:1]
    do[iter;train1'data;`0: ,,/$("iter ";iter;": errors ";5:errors);iter-:1]
    `0: ,,/$("Weights ";5:.k.w)
    `0: ,,/$("Bias ";.k.b)
}

test_and:{
    `0: ,"train and"
    data_and:(((0;0);0);((0;1);0);((1;0);0);((1;1);1))
    train[data_and;100]
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
