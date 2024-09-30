import pandas as pd
import matplotlib.pyplot as plt
from io import StringIO

raw_data = """
1000,1,0.000005,0.000005,1.060573
1000,1,0.000005,0.000007,0.694599
1000,1,0.000005,0.000005,1.040647
1000,2,0.000005,0.000028,0.169100
1000,2,0.000005,0.000032,0.158180
1000,3,0.000005,0.000198,0.024538
1000,4,0.000005,0.000038,0.128896
1000,4,0.000005,0.000212,0.023896
1000,5,0.000005,0.000286,0.016792
1000,5,0.000005,0.000253,0.019581
1000,6,0.000005,0.000055,0.100117
1000,6,0.000005,0.000251,0.019417
1000,7,0.000005,0.000056,0.086372
1000,8,0.000005,0.000056,0.089841
1000,12,0.000007,0.000101,0.072365
1000,12,0.000006,0.000163,0.034765
1000000,1,0.004608,0.004605,1.000644
1000000,1,0.004586,0.004605,0.995875
1000000,2,0.004567,0.002320,1.968565
1000000,2,0.004586,0.002335,1.964383
1000000,3,0.004576,0.001715,2.667716
1000000,3,0.004622,0.001756,2.632334
1000000,4,0.004726,0.001630,2.899018
1000000,4,0.004655,0.001200,3.879395
1000000,5,0.004623,0.001090,4.243586
1000000,5,0.004567,0.000967,4.722133
1000000,6,0.004674,0.001088,4.296003
1000000,6,0.004621,0.000971,4.757254
1000000,7,0.004564,0.000947,4.821721
1000000,7,0.004563,0.000788,5.792745
1000000,8,0.004601,0.000947,4.855775
1000000,8,0.007097,0.000970,7.317223
1000000,12,0.007104,0.001102,6.448223
1000000,12,0.007087,0.001105,6.414516
100000000,1,0.456001,0.456984,0.997850
100000000,1,0.456577,0.455828,1.001642
100000000,2,0.459968,0.229580,2.003523
100000000,2,0.464370,0.230923,2.010928
100000000,3,0.456242,0.152860,2.984696
100000000,3,0.459093,0.153623,2.988441
100000000,4,0.455960,0.115307,3.954331
100000000,4,0.483347,0.115014,4.202498
100000000,5,0.456573,0.092299,4.946694
100000000,5,0.461925,0.092101,5.015433
100000000,6,0.458585,0.077243,5.936932
100000000,6,0.466735,0.077317,6.036648
100000000,7,0.459183,0.066327,6.923042
100000000,7,0.462098,0.067141,6.882522
100000000,8,0.459620,0.058282,7.886171
100000000,8,0.462991,0.063479,7.293551
100000000,12,0.705360,0.061343,11.498657
100000000,12,0.704533,0.061033,11.543451
"""

df = pd.read_csv(StringIO(raw_data), header=None, names=['N', 'processes', 'seq_time', 'par_time', 'speedup'])

df_avg = df.groupby(['N', 'processes'])['speedup'].mean().reset_index()

Ns = [1000, 1000000, 100000000]
processes = [1, 2, 3, 4, 5, 6, 7, 8, 12]
colors = ['b', 'g', 'r']

plt.figure(figsize=(10, 6))

for i, N in enumerate(Ns):
    df_n = df_avg[df_avg['N'] == N]

    if 1 not in df_n['processes'].values:
        df_n = pd.concat([pd.DataFrame({'N': [N], 'processes': [1], 'speedup': [1]}), df_n])

    df_n = df_n.sort_values('processes')

    full_df = pd.DataFrame({'processes': processes})
    full_df = full_df.merge(df_n, on='processes', how='left')
    full_df['speedup'] = full_df['speedup'].interpolate()

    plt.plot(full_df['processes'], full_df['speedup'], marker='o', linestyle='-', color=colors[i], label=f'N = {N}')

plt.xlabel('Number of Processes')
plt.ylabel('Speedup')
plt.title('MPI Integration Speedup Graph')
plt.legend()
plt.grid(True)
plt.xscale('linear')
plt.yscale('linear')
plt.xticks(processes)


plt.tight_layout()
plt.savefig('speedup_graph.png', dpi=300)
plt.show()

for N in Ns:
    print(f"\nAverage speedups for N = {N}:")
    df_n = df_avg[df_avg['N'] == N].sort_values('processes')
    print(df_n)