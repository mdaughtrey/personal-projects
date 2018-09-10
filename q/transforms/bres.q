/bresl:{[x0;y0;x1;y1] show ("bresl ";x0;y0;x0;y1);}
/bresh:{[x0;y0;x1;y1] show ("bresh ";x0;y0;x0;y1);}
bresh:{[x0;y0;x1;y1] 
/    show ("bresh entry ";x0;y0;x1;y1);
    .sp: `dx`dy`xi`x!(x1-x0;y1-y0;1;x0);
    if[.sp[`dx]<0; .sp[`xi]:neg 1; .sp[`dx]: neg .sp[`dx]];
    .sp[`D]: (2*.sp[`dx])-.sp[`dy];
/    show (".sp 1 ";.sp);
    res: {[y]
/        show ("bresh x ";y);
        res: (.sp[`x];y);
        if[.sp[`D]>0; .sp[`x]+:.sp[`xi]; .sp[`D]-:2*.sp[`dy]];
/        show "bresh 1";
        .sp[`D]+:2*.sp[`dx];
/        show "bresh 2";
        :res;
    } each y0 + til (y1 - y0 - 1);
/    show ("bresh returns ";res);
    :res
    }


bresl:{[x0;y0;x1;y1]
/    show ("bresl entry ";x0;y0;x1;y1);
    .sp: `dx`dy`yi`y!(x1-x0;y1-y0;1;y0);
    if[.sp[`dy]<0; .sp[`yi]:neg 1; .sp[`dy]: neg .sp[`dy]];
    .sp[`D]: (2*.sp[`dy])-.sp[`dx];
/    show (".sp 1 ";.sp);
    res: {[x]
/        show ("bresl x ";x);
        res: (x;.sp[`y])
        if[.sp[`D]>0; .sp[`y]+:.sp[`yi]; .sp[`D]-:2*.sp[`dx]];
/        show "bresl 1";
        .sp[`D]+:2*.sp[`dx];
/        show "bresl 2";
        :res;
    } each x0 + til (x1 - x0) - 1;
/    show ("bresl returns ";res);
    :res
    }

bres:{[v0;v1] / bres
    if[v0~v1;:()];

    x0:v0[0];
    y0:v0[1];
    x1:v1[0];
    y1:v1[1];
    / test for horizontal line
    if[y0~y1; if[x0>x1; xx:x0; x0:x1; x1:xx]; :(x0 + til 1+x1-x0),\:y0];
    / test for vertical line
    if[x0~x1; if[y0>y1; xx:y0; y0:y1; y1:xx]; :x0,/:y0 + til 1+y1-y0];
    res:$[abs[y1-y0]<abs[x1-x0];
        $[x0>x1; bresl[x1;y1;x0;y0]; bresl[x0;y0;x1;y1]];
        $[y0>y1; bresh[x1;y1;x0;y0]; bresh[x0;y0;x1;y1]]];
    show ("bres returns ";res);
    :res
    }

v:((0;0);(10;5);(20;0));

t:{:(last v)bresa':v;}

bres0:{[obj] :(last obj[`p])bres':obj[`p];}

show "bres init done"
/u: {a:11;b:3;
/    :$[a=10;
/        $[b=2;20;-20];
/        $[b=2;30;-30]];
/    }
