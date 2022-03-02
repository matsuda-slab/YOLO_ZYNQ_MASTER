import sys
import numpy as np

param = np.load(sys.argv[1], allow_pickle=True)
param = param.item()

for key in param.keys():
    print(key)

for x in param['conv10.bias']:
    print(x)
