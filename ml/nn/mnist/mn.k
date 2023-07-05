
doit:{
    .v.itrain: 0.01+'(0.99%255)*'0N 784 # 16_ `i$1:"train-images-idx3-ubyte"
    .v.itest: 0.01+'(0.99%255)*'0N 784 # 16_ `i$1:"t10k-images-idx3-ubyte"

    .v.ltrain: 8_ `i$1:"train-labels-idx1-ubyte"
    .v.ltest:  8_ `i$1:"t10k-labels-idx1-ubyte"
}

trainimages:doit[]
\l /home/matt/ngnk/k/repl.k
//1/0
