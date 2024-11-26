#!/bin/env k.exe
. "\\r ",$-__t%1000

/ perceptron
p:{[i;w;t] : :[t>+/i*w;0;1]}
/ is non-negative
inn:{:p[,x;,1;0]}

notf:{[x] :p[,x;,-1;-0.5] }

demo_not:{
`0: ,,/$("NOT 0: ";notf 0)
`0: ,,/$("NOT 1: ";notf 1)
}


train:{[data;iter]
    ni:#**data
    `0: ,,/$("num inputs ";ni)
    .k.w:(ni _draw r)%r:1000000
    .k.t:*(1 _draw r)%r
    .k.lr:0.1
    `0: ,,/$("Initial w ";5:.k.w;" initial threshold ";.k.t)

    / One run on one datapoint
    train1:{[d]
        e:(d 1)-o:p[d 0;.k.w;.k.t]
        `0: ,,/$("e ";e;" .k.w ";5:.k.w;" .k.t ";5:.k.t)
        if[e;.k.w+:.k.lr*e*d 0; .k.t-:.k.lr*e]
        :e}

    / One run on all datapoints
    while[iter & 0<+/~0=errors:train1'data;`0: ,,/$("iter ";iter;": errors ";5:errors);iter-:1]
    `0: ,,/$("Weights ";5:.k.w)
    `0: ,,/$("Threshold ";.k.t)
}

test_and:{
    `0: ,"train and"
    data_and:(((0;0);0);((0;1);0);((1;0);0);((1;1);1))
    train[data_and;1000]
    `0: ,,/$("0,0 ";p[(0;0);.k.w;.k.t])
    `0: ,,/$("0,1 ";p[(0;1);.k.w;.k.t])
    `0: ,,/$("1,0 ";p[(1;0);.k.w;.k.t])
    `0: ,,/$("1,1 ";p[(1;1);.k.w;.k.t])
    `0: ,"test_and"
}

test_or:{
    `0: ,"train_or"
    data_or:(((0;0);0);((0;1);1);((1;0);1);((1;1);1))
    train[data_or;1000]
    `0: ,,/$("0,0 ";p[(0;0);.k.w;.k.t])
    `0: ,,/$("0,1 ";p[(0;1);.k.w;.k.t])
    `0: ,,/$("1,0 ";p[(1;0);.k.w;.k.t])
    `0: ,,/$("1,1 ";p[(1;1);.k.w;.k.t])
    `0: ,"test_or"
}
test_and[]
test_or[]



