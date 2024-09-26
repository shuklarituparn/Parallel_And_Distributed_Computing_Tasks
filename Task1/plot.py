import pandas as pd
import matplotlib.pyplot as plt

# Load data from the text file
data = pd.read_csv('speedup_data.txt', header=None)
data.columns = ['N', 'Processes', 'Sequential Time', 'Parallel Time', 'Speedup']

# Set up the figure
plt.figure(figsize=(10, 6))

# Create a list of N values to plot
N_values_to_plot = [1000, 1000000, 100000000]

# Plot Speedup for different N values
for N_value in N_values_to_plot:
    subset = data[data['N'] == N_value]
    plt.plot(subset['Processes'], subset['Speedup'], marker='o', label=f'N = {N_value}')

# Adding labels and title
plt.title('Speedup vs Number of Processes')
plt.xlabel('Number of Processes (p)')
plt.ylabel('Speedup (S)')
plt.xticks(subset['Processes'])  # Ensure ticks correspond to processes
plt.legend(title='Number of Trapezoids (N)', loc='upper left')
plt.grid()
plt.tight_layout()

# Save or show the plot
plt.savefig('speedup_graph.png')  # Save the figure
plt.show()  # Display the plot
