show "init 0";
\l bres.q
.renderW: 30
.renderH: 30
.canvasW: 10000
.canvasH: 10000
.scaleW: .renderW%.canvasW
.scaleH: .renderH%.canvasH

.out: (.renderW;.renderH) # "."
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
square2d: {[w;h] :(`p`v`s`r)!(((0;0);(0;h);(w;h);(w;0));
    (`x`y`z!1 1 1);
    (`x`y`z!0.0 0.0 0.0);
    (`x`y`z!0.0 0.0 0.0));
    }

show "init 1";
 triangle2d: {[w;h] :(`p`v`s`r)!(((0;0);(w%2;h);(w;0));
    (`x`y`z!1 1 1);
    (`x`y`z!0.0 0.0 0.0);
    (`x`y`z!0.0 0.0 0.0));
    }
show "init 1a";


show "init 4";
bres0:{[obj] :raze (last obj[`p])bres':obj[`p];}

translate: {[x;y;obj] obj[`p]: obj[`p]+\:(x;y); :obj }
scale: {[x;y;obj] obj[`p]: obj[`p]*\:(x;y); :obj }

render:{[p] 
    / naive clip
    p: p[where(p[;0] in til .renderW)&(p[;1] in til .renderH)];
    p:.[p;(::;1);{.renderH-x}];
    .out: (.renderW;.renderH)#"."; {.[`.out;reverse x;:;"@"]} each p; }

outhtml: {{(" " sv string x),"<br>"} each .out}

/ Behaviours
bhBounce:{[obj]
/    show ("bhBounce before test 1 ";obj);
    if[(obj[`v;`x]+max obj[`p;;0])>(.canvasW-1); obj[`v;`x]: neg `int$1%.scaleW];
    if[obj[`v;`x]>=min obj[`p;;0]; obj[`v;`x]: `int$1%.scaleW];
/    show ("bhBounce after test 1 ";obj);

    if[(obj[`v;`y]+max obj[`p;;1])>(.canvasH-1); obj[`v;`y]: neg `int$1%.scaleH];
    if[obj[`v;`y]>=min obj[`p;;1]; obj[`v;`y]: `int$1%.scaleH];
/    show ("bhBounce after test 2 ";obj);

    :translate[obj[`v;`x];obj[`v;`y];obj];
    }

show "init 5";
.bh.throb: `xScale`yScale!1.0 1.0
bhThrob:{[obj]
    :scale[.bh.throb[`xScale];1f;obj];
    }

/ Pipeline
/.objs: { square2d[x*500;y*500] } ./:raze (5?6),\:/:(5?6)
show "init 6";
.objs: enlist triangle2d[4000;2000];
pipeline: {[objs] 
    show ("objs presented to pipeline";objs);
/    objs: bhThrob each objs;
/    show ("objs after bhThrob";objs);
    .objs:bhBounce each objs; 
    show (".objs after bhBounce";.objs);
    / convert to int, Deduplicate points and scale to render window
    :`long$scale[.scaleW;.scaleH;] each .objs;
    }
doit: {render distinct raze bres0 each pipeline .objs; }
d2: {:raze bres0 each pipeline .objs; }

\p 5042
.z.wo:{`requestor set x; system "t 100";}
.z.ts:{ doit[]; neg[requestor] -8!outhtml[];}

show "init"
