show "init 0";
\l bres.q
.renderW: 30
.renderH: 30
.canvasW: 10000
.canvasH: 10000
.scaleW: .renderW%.canvasW
.scaleH: .renderH%.canvasH
show "init 0a";

/         |
/  (-1,1) | (1,1)
/         |
/ --------+----------
/         |
/  (-1,-1)| (1,-1)
/         |
/ obj.p = points
/ obj.v = vector (position, direction)
/ obj.s = scale
/ obj.r = rotation 
/ obj.b = behaviors
/ obj.re = rendered points
/ obj.sc = scratchpad (dict)
.objs: flip (`l`p`v`s`r`b`re`sc)!()
/.sc: flip(`o`k`v)!()
.sc:()!()
show "init 0b";

.debug:1
.d:{[x]$[.debug;show x;:0];}

.out: (.renderW;.renderH) # "."
translate: {[o;k;x;y]
    l:o[`l];
    if[0~count key .sc;
        .sc[l]:(enlist k)!enlist (`x`y)!(0;0)];
    if[not k in key .sc[l];
        .sc[l]:(enlist k)!enlist (`x`y)!(0;0)];
/    .d ("sc pre tx ";.sc);
    .sc[l;k;`x`y]+:(x;y);
/    .d ("sc post tx ";.sc);
/    .d ("tx pre ";o[`re]);
    o[`re]:o[`re]+\:(.sc[l;k;`x`y]);
/    .d ("tx post ";o[`re]);
    :o }
show "init 0c";

scale: {[x;y;obj] obj[`re]: obj[`re]*\:(x;y); :obj }
show "init 0d";
scale2: {[x;y;obj] obj[`re]: obj[`re]*\:(x;y); :obj }
show "init 0e";
noop:{[o] break; :o }

rotate2:{[o;k;r]
    l:o[`l];
/    .d ("rotate2 ";r);
/    .d ("r2 pre ";o[`re]);
/    .d (".sc #1 in rotate2 is ";-3!.sc);
/    .d ("count key ";count key .sc);
    if[0~count key .sc;
        .sc[l]:(enlist k)!(enlist (enlist `r)!(enlist 0))];
    if[not k in key .sc[l];
        .sc[l]:(enlist k)!(enlist (enlist `r)!(enlist 0))];
    .sc[l;k;`r]+:r;
    r: .sc[l;k;`r]%57.2958;  
    o[`re]: {[x;y;c;s]:((x*c)-(y*s);(x*s)+(y*c))}[;;cos r;sin r]./:o[`re];
/    .d ("total r ";r);
/    .d ("r2 post ";o[`re]);
    :o }   

/ Behaviours
bhBounce:{[obj]
    if[(obj[`v;`x]+max obj[`p;;0])>(.canvasW-1); obj[`v;`x]: neg `int$1%.scaleW];
    if[obj[`v;`x]>=min obj[`p;;0]; obj[`v;`x]: `int$1%.scaleW];
    if[(obj[`v;`y]+max obj[`p;;1])>(.canvasH-1); obj[`v;`y]: neg `int$1%.scaleH];
    if[obj[`v;`y]>=min obj[`p;;1]; obj[`v;`y]: `int$1%.scaleH];
    .d ("pre translate ";obj[`p]);
    res:translate[obj[`v;`x];obj[`v;`y];0;obj];
    .d ("post translate ";res);
    :res
    }

square2d: {[l;w;h;b]
   .objs,:`l`p`v`s`r`b`re`sc!(l;((0;0);(0;h);(w;h);(w;0));  / l,p
    (`x`y!1 1);                                         / v
    (`x`y!0.0 0.0);                                     / s
    0.0;                                                / r
    b;                                                  / b
    enlist (0;0);                                     / re
    ()!());                                          / sc
    }

.d "init 1";
triangle2d: {[w;h;b] :(`p`v`s`r`b)!(((0;0);(w%2;h);(w;0));
    (`x`y!1 1);
    (`x`y!0.0 0.0);
    0.0;
    b);
    }
.d "init 1a";
point2d: {[x;y;b] :(`p`v`s`r`b`sc)!(enlist(x;y);
    (`x`y!1 1);
    (`x`y!0.0 0.0);
    0.0;
    b;
    ()!());
    }

.d "init 4";
bres0:{[re]
    p:distinct floor 0.5+re*\:(.scaleH;.scaleW);
    .d ("bres0 ";re);
    res:raze (last p)bres':p;
    .d ("bres0 returns ";res);
    :res}

render:{[p] 
    / naive clip
    p: p[where(p[;0] in til .renderW)&(p[;1] in til .renderH)];
    / render (0;0) is top left so flip the points vertically
    .d ("render pre invert ";p);
    p:.[p;(::;0);{.renderH-x+1}];
    .d ("render post invert ";p);
    .out: (.renderW;.renderH)#".";
    .[`.out;;:;"@"] each p;
    }

outhtml: {{(" " sv string x),"<br>"} each .out}

.d "init 6";
/ Pipeline
dopipe: {[o]
    o[`re]: o[`p];
    p:('[;])over o[`b];
    o: p[o];
    .objs[.objs[`l]?o[`l]]: o;
    :o[`re] }

/d: {render distinct raze bres0 each dopipe each .objs[`l]; }
d: { render distinct bres0 raze dopipe each .objs}

b0:{x*2}
b1:{x+1}
p: ('[;]) over (b0;b1)

.d "init 7";
/.objs[`asquare]: enlist square2d[1000;2000;enlist rotate2[;`r;5]];
.d (".sc in main is ";-3!.sc);
square2d[`asquare;2000;4000;(translate[;`tx;10;10]; rotate2[;`rotate;5])]
.d "init 8";

\p 5042
.z.wo:{`requestor set x; system "t 200";}
.z.ts:{ d[]; neg[requestor] -8!outhtml[];}

\C 10 10
.d "init"
