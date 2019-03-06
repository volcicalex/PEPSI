# import modules
import matplotlib.pyplot as plt
import pandas as pd

# Load data
data = pd.read_csv('call0.txt', header = None)

plt.hist(data,normed=1)
plt.show()




