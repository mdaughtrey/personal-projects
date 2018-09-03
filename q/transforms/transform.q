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
/ triangle2d: {[w;h] :(`p`v`s`r)!(((0;0);(w%2;h);(w;0));
/    (`x`y`z!1 1 1);
/    (`x`y`z!0.0 0.0 0.0);
/    (`x`y`z!0.0 0.0 0.0));
/    }
show "init 1a";

/ Bresenham line algorithm
/ bresHigh:{[x0;y0;x1;y1]}
/    show "bresHigh ",x0,y0,x1,y1;
/    }

show "init 1f";
show "init 2";
/bresLow:{[x0;y0;x1;y1]
/    show "bresLow ",x0,y0,x1,y1;
/    break;
/    dx:x1-x0;
/    dy:y1-y0;
/    yi:1;
/    if[dy<0; yi:neg 1; dy: neg dy];
/    D:(2*dy) -dx;
/    y:y0;
/    res: {[x;y;D;dx;dy]
/        if[D>0; y+:yi; D-:2*dx];
/        D+:2*dy;
/        :(x;y);
/    }[;`y;`D;`dx;`dy] each x0 + til (x1 - x0);
/    show ("bresLow ";res);
/    :res
/    }

show "init 3";
bres2:{[v0;v1] / bres
/    show ("bres v0 ";-3!v0;" v1 ";-3!v1);
    if[v0~v1;:()];
    x0:v0 0;
    y0:v0 1;
    x1:v1 0;
    y1:v1 1;
    / test for horizontal line
    if[y0~y1; if[x0>x1; xx:x0; x0:x1; x1:xx]; :(x0 + til 1+x1-x0),\:y0];
    / test for vertical line
    if[x0~x1; if[y0>y1; xx:y0; y0:y1; y1:xx]; :x0,/:y0 + til 1+y1-y0];
/    :[abs[y1-y0]<abs[x1-x0]
/        :[x0>x1; bresLow[x1;y1;x0;y0]; bresHigh[x0;y0;x1;y1]];
/        :[y0>y1; bresLow[x1;y1;x0;y0]; bresHigh[x0;y0;x1;y1]]];
    :(v0;v1);
    }

show "init 4";
bres:{[obj] :(last obj[`p])bres2':obj[`p];}

translate: {[x;y;obj] obj[`p]: obj[`p]+\:(x;y); :obj }
scale: {[x;y;obj] obj[`p]: obj[`p]*\:(x;y); :obj }

render:{[p] 
    / naive clip
    p: p[where(p[;0] in til .renderW)&(p[;1] in til .renderH)];
    .out: (.renderW;.renderH)#"."; {.[`.out;x;:;"@"]} each p; }

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
.objs: (square2d[1000;1600]; square2d[1700;900]);
show "init 6";
/.objs: enlist triangle2d[4000;200];
pipeline: {[objs] 
    show ("objs presented to pipeline";objs);
/    objs: bhThrob each objs;
/    show ("objs after bhThrob";objs);
    .objs:bhBounce each objs; 
    show (".objs after bhBounce";.objs);
    / convert to int, Deduplicate points and scale to render window
    :`long$scale[.scaleW;.scaleH;] each .objs;
    }
doit: {render distinct raze raze bres each pipeline .objs; }

\p 5042
.z.wo:{`requestor set x; system "t 100";}
.z.ts:{ doit[]; neg[requestor] -8!outhtml[];}

show "init"
