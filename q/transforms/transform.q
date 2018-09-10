show "init 0";
\l bres.q
.renderW: 30
.renderH: 30
.canvasW: 10000
.canvasH: 10000
.scaleW: .renderW%.canvasW
.scaleH: .renderH%.canvasH

.debug:1
.d:{[x]$[.debug;show x;:0];}

.out: (.renderW;.renderH) # "."
translate: {[x;y;obj] obj[`p]: obj[`p]+\:(x;y); :obj }
scale: {[x;y;obj] obj[`p]: obj[`p]*\:(x;y); :obj }
scale2: {[x;y;obj] obj[`p]: obj[`p]*\:(x;y); :obj }
/rotate2:{[x;y;r] r%:57.2958; s:sin r; c:cos r; :((x*c)-(x*s);(y*s)+(y*c)); }   
rotate2:{[r;obj] r%:57.2958;  / s:sin r; c:cos r;
    .d ("r2 pre ";obj[`p]);
    obj[`p]: @[obj[`p];::;{[x;y;c;s]:((x*c)-(x*s);(y*s)+(y*c))}[;;cos r;sin r] .];
    .d ("r2 post ";obj[`p]);
    :obj; }   

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
square2d: {[w;h;b] :(`p`v`s`r`b)!(((0;0);(0;h);(w;h);(w;0));
    (`x`y!1 1);
    (`x`y!0.0 0.0);
    (`x`y!0.0 0.0);
    b);
    }

show "init 1";
 triangle2d: {[w;h;t] :(`p`v`s`r`b)!(((0;0);(w%2;h);(w;0));
    (`x`y!1 1);
    (`x`y!0.0 0.0);
    (`x`y!0.0 0.0);
    t);
    }
show "init 1a";
point2d: {[x;y;t] :(`p`v`s`r`b)!(((x;y));
    (`x`y!1 1);
    (`x`y!0.0 0.0);
    (`x`y!0.0 0.0);
    t);
    }



show "init 4";
bres0:{[obj] p:distinct floor 0.5+obj[`p]*\:(.scaleH;.scaleW);
    :raze (last p)bres':p; }

render:{[p] 
    / naive clip
    p: p[where(p[;0] in til .renderW)&(p[;1] in til .renderH)];
    p:.[p;(::;1);{.renderH-x+1}];
    .out: (.renderW;.renderH)#"."; {.[`.out;reverse x;:;"@"]} each p; }

outhtml: {{(" " sv string x),"<br>"} each .out}

show "init 6";
/ Pipeline
dopipe: {[obj]
    .d ("dopipe obj ";obj);
    res: first {:x[y]}[;obj] each obj[`b];
    .d ("dopipe res ";res);
    :res
    }

persist:{[objs]  .objs: objs; :objs}

.p.rotate:{[obj]
     
    }


d: {render distinct raze bres0 each persist dopipe each .objs; }

/.objs: enlist triangle2d[4000;2000;enlist rotate2[1;]];
.objs: enlist point2d[10;10;enlist rotate2[5;]];

\p 5042
.z.wo:{`requestor set x; system "t 2000";}
.z.ts:{ d[]; neg[requestor] -8!outhtml[];}

show "init"

/r:{[x;y;r] a:x; b:y; s:sin r%57.2958; c:cos r%57.2958; :((a*c)-(b*s);(b*s)+(b*c)); }   

