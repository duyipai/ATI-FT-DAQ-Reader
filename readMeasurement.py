# Below is an example to read the FT measurement.txt file to numpy array
import numpy as np

while True:
    with open("measurement.txt", 'r') as file:
        data = file.read().split(' ')
        if data[0] != '':
            data = np.array(data, dtype=np.float64)
            print(data)
