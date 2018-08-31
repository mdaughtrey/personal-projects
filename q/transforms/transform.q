.renderW: 20
.renderH: 20

.out: (.renderW;.renderH) # 0 
.plindex: 0 / pipeline index

/         |
/  (-1,1) | (1,1)
/         |
/ --------+----------
/         |
/  (-1,-1)| (1,-1)
/         |
square2d: {[w;h] :((0;h);(0;0);(w;0);(w;h)) }

/ Bresenham line algorithm
bresHigh:{[x0;y0;x1;y1]
    show "bresHigh ",x0,y0,x1,y1
    }

bresLow:{[x0;y0;x1;y1]
    show "bresLow ",x0,y0,x1,y1
    }

bres:{[v0;v1]
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
    }

translate: {[x;y;points] :.[points;();+\:;(x;y)]; }


/echo: {show "x=",x}
render:{[points] 
    / naive clip
    points: points[where(points[;0] in til .renderW)&(points[;1] in til .renderH)];
    .out: (.renderW;.renderH)#0; {.[`.out;x;:;1]} each points; }

outhtml: {{(" " sv string x),"<br>"} each .out}

/ Behaviours
.bh.bounce: `xOffset`xDir`yOffset`yDir!0 1 0 1
bhBounce:{[obj]
    maxX: .bh.bounce[`xOffset] + max obj[;0];
    minX: .bh.bounce[`xOffset] + min obj[;0];
    maxY: .bh.bounce[`yOffset] + max obj[;1];
    minY: .bh.bounce[`yOffset] + min obj[;1];
    1 "" sv string "minY ",minY," minX ",minX;
    if[maxX>=(.renderW-1); .bh.bounce[`xDir]: neg 1];
    if[0>=minX; .bh.bounce[`xDir]: 1];
    if[maxY>=(.renderH-1); .bh.bounce[`yDir]: neg 1];
    if[0>=minY; .bh.bounce[`yDir]: 1];
    .bh.bounce[`xOffset]+: .bh.bounce[`xDir];
    .bh.bounce[`yOffset]+: .bh.bounce[`yDir];
    0N!.bh.bounce;
    res:translate[.bh.bounce[`xOffset];.bh.bounce[`yOffset];obj];
    :res;
    }

/ Pipeline
sq: square2d[3;4]
pipeline: {[objs] 
    :bhBounce each objs; 
    }
doit: {  render distinct raze raze {(last x)bres':x} each pipeline enlist sq; }

\p 5042
.z.wo:{`requestor set x; system "t 500";}
.z.ts:{ doit[]; neg[requestor] -8!outhtml[];}

/badpoints: ((1;1);(2;2);(3;3);(4;5);(20;30);(5;40);(-1;-5);(-2;3);(0;100);(4;-20))
