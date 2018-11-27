.renderW: 20
.renderH: 20

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

translate: {[x;y;points] (x;y)+/:raze points }

/echo: {show "x=",x}
render:{[points] {.[`.out;x;:;1]} each points }

.out: (.renderW;.renderH) # 0 
sq: square2d[3;4]
/{.[`.out;x;:;1]} each square2d[3;4]
/render translate[2;2;] (last sq)bres':sq
.z.ts: {
    1 "Hi!\n"
}


