/bresl:{[x0;y0;x1;y1] show ("bresl ";x0;y0;x0;y1);}
/bresh:{[x0;y0;x1;y1] show ("bresh ";x0;y0;x0;y1);}
bresh:{[x0;y0;x1;y1] 
    show ("bresh entry ";x0;y0;x1;y1);
    .sp: `dx`dy`xi`x!(x1-x0;y1-y0;1;x0);
    if[.sp[`dx]<0; .sp[`xi]:neg 1; .sp[`dx]: neg .sp[`dx]];
    .sp[`D]: (2*.sp[`dx])-.sp[`dy];
    show (".sp 1 ";.sp);
    res: {[y]
        show ("bresh x ";y);
        res: (.sp[`x];y);
        if[.sp[`D]>0; .sp[`x]+:.sp[`xi]; .sp[`D]-:2*.sp[`dy]];
        show "bresh 1";
        .sp[`D]+:2*.sp[`dx];
        show "bresh 2";
        :res;
    } each y0 + til (y1 - y0);
    show ("bresh returns ";res);
    :res
    }


bresl:{[x0;y0;x1;y1]
    show ("bresl entry ";x0;y0;x1;y1);
    .sp: `dx`dy`yi`y!(x1-x0;y1-y0;1;y0);
    if[.sp[`dy]<0; .sp[`yi]:neg 1; .sp[`dy]: neg .sp[`dy]];
    .sp[`D]: (2*.sp[`dy])-.sp[`dx];
    show (".sp 1 ";.sp);
    res: {[x]
        show ("bresl x ";x);
        res: (x;.sp[`y])
        if[.sp[`D]>0; .sp[`y]+:.sp[`yi]; .sp[`D]-:2*.sp[`dx]];
        show "bresl 1";
        .sp[`D]+:2*.sp[`dx];
        show "bresl 2";
        :res;
    } each x0 + til (x1 - x0);
    show ("bresl returns ";res);
    :res
    }

bres:{[v0;v1] / bres
    show ("bres v0 ";-3!v0;" v1 ";-3!v1);
    if[v0~v1;:()];

    x0:v0[0];
    y0:v0[1];
    x1:v1[0];
    y1:v1[1];
    / test for horizontal line
    show "htest";
    if[y0~y1; if[x0>x1; xx:x0; x0:x1; x1:xx]; aa:(x0 + til 1+x1-x0),\:y0; show ("htest aa";aa);:aa];
    / test for vertical line
    show "vtest";
    if[x0~x1; if[y0>y1; xx:y0; y0:y1; y1:xx]; :x0,/:y0 + til 1+y1-y0];
    show "diag";
    res:$[abs[y1-y0]<abs[x1-x0];
        $[x0>x1; [show ("bresl1 ";x1;y1;x0;y0); bresl[x1;y1;x0;y0]]; [show ("bresl2 ";x0;y0;x1;y1);bresl[x0;y0;x1;y1]]];
        $[y0>y1; [show ("bresh1 ";x1;y1;x0;y0); bresh[x1;y1;x0;y0]]; [show ("bresh2 ";x0;y0;x1;y1);bresh[x0;y0;x1;y1]]]];
    show ("bres2 res ";res);
    :res
    }

v:((0;0);(10;5);(20;0));

t:{:(last v)bres':v;}

bres0:{[obj] :(last obj[`p])bres':obj[`p];}

show "bres init done"
/u: {a:11;b:3;
/    :$[a=10;
/        $[b=2;20;-20];
/        $[b=2;30;-30]];
/    }
