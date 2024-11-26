.renderW: 20
.renderH: 20

render: {[vlist]
    .out: (.renderW;.renderH) # 0 
    {.[`.out;y;:;1]} each vlist
    }
