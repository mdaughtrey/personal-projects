\d .nn
\l "../nnlib"
\d .k

\e 1

train: -1 784 # _ic 16_ 6: "train-images-idx3-ubyte"
labels: _ic 8_ 6: "train-labels-idx1-ubyte"
.nn.init[#*train; 10; #*labels; 4000]
w: .()
w: .nn.train[w; train; labels]
/.nn.test[w] .' +(x;y)

