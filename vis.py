#簡易ビジュアライザ

import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
filename = "dump.txt"

with open(filename) as f:
    n = int(f.readline())

d = np.loadtxt(filename, dtype="int64", skiprows=1)
d1 = d[:n]
d2 = d[n:n*2]
d2 = np.where(d2 == 0, -5, d2)

plt.figure()
sns.heatmap(d2, annot=True, cmap="bone")
plt.savefig("vis.png")

plt.figure()
sns.heatmap(d1, annot=True, cmap="bone")
plt.savefig("vis1.png")
