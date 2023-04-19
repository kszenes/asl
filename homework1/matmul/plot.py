import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

sns.set()

df = pd.read_csv("mm-perf.csv", delim_whitespace=True)
print(df.columns)
df.columns = [x.split('[')[0] for x in df.columns]
print(df)
long = pd.melt(df, id_vars="n", value_vars=["opt", "unvec", "unopt"], value_name="flops/cycle")

sns.lineplot(data=long, x="n", y="flops/cycle", hue="variable", marker="o")

l2 = 256_000
l3 = 6_144_000
n_l2 = np.sqrt(l2 / 2 / 8)
n_l3 = np.sqrt(l3 / 2 / 8)

plt.vlines([n_l2, n_l3], 0, 4)



plt.show()
