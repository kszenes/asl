import pandas as pd
import subprocess
import numpy as np

mem_bytes = []
ns = np.linspace(100, 1500, 15, dtype=np.int32)
unopt = []
unvec = []
opt = []


for n in ns:
    mem_bytes.append(2 * n * n * 8)
    proc = subprocess.run(["./optimized", f"{n}"], capture_output=True, encoding="utf-8")
    opt.append(float(proc.stdout))
    proc = subprocess.run(["./unvectorized", f"{n}"], capture_output=True, encoding="utf-8")
    unvec.append(float(proc.stdout))
    proc = subprocess.run(["./unoptimized", f"{n}"], capture_output=True, encoding="utf-8")
    unopt.append(float(proc.stdout))


print(f"{mem_bytes = }")
print(f"{unopt = }")
print(f"{unvec = }")
print(f"{opt = }")

df = pd.DataFrame(data=[ns, mem_bytes, unopt, unvec, opt]).T
df.columns = ["n", "Memory[bytes]", "unopt[flops/cycle]", "unvec[flops/cycle]", "opt[flops/cycle]"]
print(df)
df.to_csv("mm-perf.csv", sep=' ', index=None)



#df = pd.read_csv("mm-perf.csv", delim_whitespace=True, skiprows=1)
#long = pd.melt(df, id_vars="n", value_vars=["optimized", "unvectorized", "unoptimized"], value_name="flops/cycle")

#sns.lineplot(data=long, x="n", y="flops/cycle", hue="variable", marker="o")
#plt.show()
