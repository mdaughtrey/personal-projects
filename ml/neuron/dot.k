/(Pdb) print(adjustment)
/[[2.11620262]
/ [1.52418942]
/ [2.96015544]]
/(Pdb) print(train_inputs.T)
/[[0 1 1 0]
/ [0 1 0 1]
/ [1 1 1 1]]
/(Pdb) print(error*self.tanh_derivative(output))
/[[0.44767655]
/ [1.12791315]
/ [0.98828947]
/ [0.39627627]]
/(Pdb)

dotp:{+|+/''x*\:/:|+y}
i:((0 1 1 0);(0 1 0 1);(0 1 0 1);(1 1 1 1))
j:(,-4.27995231e-01;,6.62051890e-06;,3.37192706e-04;,-4.20145276e-01)
