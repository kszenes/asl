import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
sns.set()


df = pd.read_csv("mm-perf.csv", delim_whitespace=True, skiprows=1)
long = pd.melt(df, id_vars="n", value_vars=["optimized", "unvectorized", "unoptimized"], value_name="flops/cycle")

sns.lineplot(data=long, x="n", y="flops/cycle", hue="variable", marker="o")
plt.show()