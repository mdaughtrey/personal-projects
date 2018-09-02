.renderW: 30
.renderH: 30
.canvasW: 10000
.canvasH: 10000
.scaleW: .renderW%.canvasW
.scaleH: .renderH%.canvasH

.out: (.renderW;.renderH) # "."
.plindex: 0 / pipeline index

/         |
/  (-1,1) | (1,1)
/         |
/ --------+----------
/         |
/  (-1,-1)| (1,-1)
/         |
square2d: {[w;h] :((0;0);(0;h);(w;h);(w;0)) }

/ Bresenham line algorithm
bresHigh:{[x0;y0;x1;y1]
    show "bresHigh ",x0,y0,x1,y1
    }

bresLow:{[x0;y0;x1;y1]
    show "bresLow ",x0,y0,x1,y1
    }

bres:{[v0;v1]
    /show ("bres v0 ";v0;" v1 ";v1)
    if[v0~v1;:()]
    x0:v0 0;
    y0:v0 1;
    x1:v1 0;
    y1:v1 1;
    / test for horizontal line
    if[y0~y1; if[x0>x1; xx:x0; x0:x1; x1:xx]; :(x0 + til 1+x1-x0),\:y0];
    / test for vertical line
    if[x0~x1; if[y0>y1; xx:y0; y0:y1; y1:xx]; :x0,/:y0 + til 1+y1-y0];
/    :[abs[y1-y0]>abs[x1-x0]
/        :[x0>x1; bresLow[x1, y1, x0, y0]; bresLow[x0, y0, x1, y1]]
/        :[y0>y1; bresHigh[x1, y1, x0, y0]; bresHigh[x0, y0, x1, y1]]
/        ];
    /:((x0,y0);(x1,y1))
    }

translate: {[x;y;points] :.[points;();+\:;(x;y)]; }
scale: {[x;y;points] :points*\:(x;y); }

/echo: {show "x=",x}
render:{[points] 
    / naive clip
    points: points[where(points[;0] in til .renderW)&(points[;1] in til .renderH)];
    .out: (.renderW;.renderH)#"."; {.[`.out;x;:;"@"]} each points; }

outhtml: {{(" " sv string x),"<br>"} each .out}

/ Behaviours
.bh.bounce: `xOffset`xDir`yOffset`yDir!0 1 0 1 
bhBounce:{[obj]
    show ("bhBounce ";.bh.bounce)
    if[(.bh.bounce[`xOffset] + max obj[;0])>=(.canvasW-1); .bh.bounce[`xDir]: neg `int$1%.scaleW];
    if[0>=.bh.bounce[`xOffset] + min obj[;0]; .bh.bounce[`xDir]: `int$1%.scaleW];
    if[(.bh.bounce[`yOffset] + max obj[;1])>=(.canvasH-1); .bh.bounce[`yDir]: neg `int$1%.scaleH];
    if[0>=.bh.bounce[`yOffset] + min obj[;1]; .bh.bounce[`yDir]: `int$1%.scaleW];
    .bh.bounce[`xOffset`yOffset]+:.bh.bounce[`xDir`yDir];

    :translate[.bh.bounce[`xOffset];.bh.bounce[`yOffset];obj];
    }

.bh.throb: `xScale`yScale!1.0 1.0
bhThrob:{[obj]
    :scale[.bh.throb[`xScale];1f;obj];
    }


/ Pipeline
sq: square2d[3000;4000]
pipeline: {[objs] 
    show ("objs presented to pipeline";objs);
    objs: bhThrob each objs;
    show ("objs after bhThrob";objs);
    res:bhBounce each objs; 
    show ("res after bhBounce";res);
    / convert to int, Deduplicate points and scale to render window
    :`long$scale[.scaleW;.scaleH;] each res;
    }
doit: { render distinct raze raze {(last x)bres':x} each pipeline (sq;sq); }

\p 5042
.z.wo:{`requestor set x; system "t 100";}
.z.ts:{ doit[]; neg[requestor] -8!outhtml[];}

