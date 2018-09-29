show "init 0";
\l bres.q
.renderW: 30
.renderH: 30
.canvasW: 10000
.canvasH: 10000
.scaleW: .renderW%.canvasW
.scaleH: .renderH%.canvasH

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

.debug:1
.d:{[x]$[.debug;show x;:0];}

.out: (.renderW;.renderH) # "."
translate: {[o;x;y]
    k: `translate;
/    show "translate";
    if[o[`sc;k]~(); o[`sc;k]:`x`y!0 0];
/    o[`sc;k]:`x`y!0 0;
    o[`sc;k;`x`y]+:(x;y);
/    show ("translate ";x;" y ";y);
    .d ("tx pre ";o[`re]);
    o[`re]:o[`re]+\:(x;y);
    .d ("tx post ";o[`re]);
    .d ("translate ";o[`re]);
    :o }

scale: {[x;y;obj] obj[`re]: obj[`re]*\:(x;y); :obj }
scale2: {[x;y;obj] obj[`re]: obj[`re]*\:(x;y); :obj }
noop:{[o] break; :o }

rotate2:{[o;k;r]
    o[k]+:r;
    .d ("rotating ";r;" degs");
    r: o[k]%57.2958;  
    .d ("r2 pre ";o[`re]);
    o[`re]: @[o[`re];::;{[x;y;c;s]:((x*c)-(y*s);(x*s)+(y*c))}[;;cos r;sin r] .];
    .d ("r2 post ";o[`re]);
    :o
    }   
/wrotate2:{[l;k;r;o] show ("wrapped ";l;" ";o[`re]); res: rotate2[k;r;o]; show ("end wrap ";l;" ";res[`re]); :res; }

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

show "init 1";
triangle2d: {[w;h;b] :(`p`v`s`r`b)!(((0;0);(w%2;h);(w;0));
    (`x`y!1 1);
    (`x`y!0.0 0.0);
    0.0;
    b);
    }
show "init 1a";
point2d: {[x;y;b] :(`p`v`s`r`b`sc)!(enlist(x;y);
    (`x`y!1 1);
    (`x`y!0.0 0.0);
    0.0;
    b;
    ()!());
    }

/save: {[o] show ("save ";o); }

show "init 4";
bres0:{[obj]
    p:distinct floor 0.5+obj[`re]*\:(.scaleH;.scaleW);
    .d ("bres0 ";obj[`re]);
    res:raze (last p)bres':p;
    .d ("bres0 returns ";res);
    :res}

render:{[p] 
    / naive clip
    p: p[where(p[;0] in til .renderW)&(p[;1] in til .renderH)];
    / render (0;0) is top left so flip the points vertically
    p:.[p;(::;1);{.renderH-x+1}];
    .out: (.renderW;.renderH)#".";
    .[`.out;;:;"@"] each p;
    }
   / {.[`.test;x;:;"."]} each aa

outhtml: {{(" " sv string x),"<br>"} each .out}

show "init 6";
/ Pipeline
dopipe: {[o]
    o[`re]: o[`p];
    p:('[;])over o[`b];
    o: p[o];
    .objs[.objs[`l]?o[`l]]: o;
    :o }

/persist:{[objs] .objs: objs; :objs}

/.p.rotate:{[obj]
/    }

/d: {render distinct raze bres0 each dopipe each .objs[`l]; }
d: { .xx: dopipe each .objs; }

b0:{x*2}
b1:{x+1}
p: ('[;]) over (b0;b1)

show "init 7";
/.objs[`asquare]: enlist square2d[1000;2000;enlist rotate2[;`r;5]];
square2d[`asquare;1000;2000;(translate[;1000;1000]; rotate2[;`r;5])]
show "init 8";

\p 5042
.z.wo:{`requestor set x; system "t 3000";}
.z.ts:{ d[]; neg[requestor] -8!outhtml[];}

show "init"
