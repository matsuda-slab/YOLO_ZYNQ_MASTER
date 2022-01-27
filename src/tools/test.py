import numpy as np

param = np.array([range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64),
                  range(0, 64)
                  ])

param_dict = dict()
param_dict['conv8.weight'] = param[:, :, np.newaxis, np.newaxis]

dict_array = np.array(param_dict)
np.save('hoge.npy', dict_array)

param = np.load('hoge.npy', allow_pickle=True)
param = param.item()
print(param)
print(param['conv8.weight'].shape)
