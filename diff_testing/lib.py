"""
    Minimal differential testing library
    
    There are 2 forms of differential testing:
    - probability distribution comparison by ks-test after running on simulator
    - statvector comparison

    What is being tested?
    - results after running circuit through specific compiler passes
    - results after running circuit through compiler at different optimisation levels

    Results:
    - probability distributions plotted
    - ks value printed in logs
"""

from typing import List, Tuple, Any
from collections import Counter
from itertools import zip_longest
from scipy.stats import ks_2samp
from numpy.typing import NDArray
import numpy as np
import sys
import argparse
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import traceback
import pathlib
import shutil

# Pytket imports
from pytket.circuit import Circuit
from pytket.passes import *
from pytket.extensions.qiskit import AerBackend
from pytket.extensions.qiskit import AerStateBackend
from pytket.extensions.quantinuum import QuantinuumBackend

# Qiskit imports
from qiskit import QuantumCircuit, transpile

# Guppylang imports
from guppylang import guppy
from guppylang import enable_experimental_features
enable_experimental_features()


class Base():
    # Define the plots directory as a class variable
    OUTPUT_DIR = (pathlib.Path(__file__).parent.parent.parent / "outputs").resolve()

    def __init__(self):
        super().__init__()
        self.parser = argparse.ArgumentParser()
        self.parser.add_argument('--plot', action='store_true',help='Plot results after running circuit')
        self.args = self.parser.parse_args()
        self.plot : bool = self.args.plot

    def preprocess_counts(self, counts : Counter[Tuple[str, ...], int]) -> Counter[int, int]:
        '''
            Given a dict mapping binary values to number of times they appear, return a sorted dict with each binary tuple/string converted into a base 10 int
            Dict is sorted by key
        '''
        out : Counter[int, int] = {}
        for k in counts.keys():
            # k can be a tuple of bits or a string of bits
            if isinstance(k, tuple):
                key_str = ''.join(str(x) for x in k).replace(' ', '')
            else:
                key_str = str(k).replace(' ', '')
            out[int(key_str, 2)] = counts[k]
        
        return dict(sorted(out.items()))
    
    def ks_test(self, counts1 : Counter[int, int], counts2 : Counter[int, int], total_shots : int) -> float:
        '''
        Carries out K-S test on two frequency lists
        '''
        sample1, sample2 = [], []

        for p1, p2 in zip_longest(counts1.items(), counts2.items(), fillvalue=None):
            if(p1):
                sample1 += p1[1] * [p1[0]]
            
            if(p2):
                sample2 += p2[1] * [p2[0]]

        assert (len(sample1) == total_shots) and (len(sample2) == total_shots), "Sample size does not match number of shots"

        ks_stat, p_value = ks_2samp(sorted(sample1), sorted(sample2))

        return p_value
    
    def compare_statevectors(self, sv1 : NDArray[np.complex128], sv2 : NDArray[np.complex128]):
        return np.dot(sv1, sv2)

    def plot_histogram(self, res : Counter[int, int], title : str, compilation_level : int, circuit_number : int = 0):
        # Check the number of existing plots in plots_path (if exists), then increment the number of the plot
        # and save the plot with that number
        plots_dir = self.OUTPUT_DIR / f"circuit{circuit_number}"
        if not plots_dir.exists():
            plots_dir.mkdir(parents=True, exist_ok=True)
        
        plots_path = plots_dir /f"output{circuit_number}_{compilation_level if compilation_level else 'uncompiled'}.png"
        
        # Plot the histogram
        values = list(res.keys())
        freqs = list(res.values())

        bar_width = 0.5
        plt.bar(values, freqs, width=bar_width, edgecolor='black')
        ax = plt.gca()
        ax.xaxis.set_major_locator(ticker.MaxNLocator(nbins=10, integer=True))
        plt.xlabel("Possible results")
        plt.ylabel("Number of occurances")
        plt.title(title)
        plt.tight_layout()  # Adjust layout to prevent clipping of ylabel and title
        plt.savefig(plots_path)
        plt.close()  # Close the plot to free up memory

class pytketTesting(Base):
    def __init__(self):
        super().__init__()
    
    def run_circ(self, circuit : Circuit, circuit_number : int) -> float:
        '''
        Runs circuit on pytket simulator and returns counts
        '''
        backend = AerBackend()
        # Get original circuit shots
        uncompiled_circ = backend.get_compiled_circuit(circuit, optimisation_level=0)
        handle1 = backend.process_circuit(uncompiled_circ, n_shots=1000)
        result1 = backend.get_result(handle1)
        counts1 = self.preprocess_counts(result1.get_counts())

        # Compile circuit at 3 different optimisation levels
        for i in range(3):
            compiled_circ = backend.get_compiled_circuit(circuit, optimisation_level=i+1)
        
            # Process the compiled circuit
            handle2 = backend.process_circuit(compiled_circ, n_shots=1000)
            result2 = backend.get_result(handle2)
            counts2 = self.preprocess_counts(result2.get_counts())

            # Run the kstest on the two results
            ks_value = self.ks_test(counts1, counts2, 1000)
            print(f"Optimisation level {i+1} ks-test p-value: {ks_value}")

            # plot results
            if self.plot:
                self.plot_histogram(counts1, "Uncompiled Circuit Results", 0, circuit_number)
                self.plot_histogram(counts2, "Compiled Circuit Results", i+1, circuit_number)

        return ks_value
    
    def run_circ_statevector(self, circuit : Circuit, test_pass : BasePass ) -> NDArray[np.complex128]:
        '''
        Runs circuit on pytket simulator and returns statevector
        '''
        try:
            # circuit with no passes
            no_pass_statevector = circuit.get_statevector()
            
            # Put it through the pass specified, modifying it in-place
            test_pass.apply(circuit)

            # circuit after pass
            pass_statevector = circuit.get_statevector()
                
            if np.round(abs(np.vdot(no_pass_statevector, pass_statevector)), 6)==1:
                print("Statevectors are the same\n")
            else:
                print ("Statevectors not the same")
                print("Dot product: ", np.round(abs(np.vdot(no_pass_statevector, pass_statevector)), 6))

        except Exception:
            print("Exception :", traceback.format_exc())

class qiskitTesting(Base):
    def __init__(self):
        super().__init__()
    
    def run_circ(self, circuit : Circuit, circuit_number : int) -> float:
        '''
        Runs circuit on qiskit simulator and returns counts
        '''

        from qiskit_aer import AerSimulator
        backend = AerSimulator()
        
        # Get original circuit shots
        uncompiled_circ = transpile(circuit, backend, optimization_level=0)
        counts1 = self.preprocess_counts(backend.run(uncompiled_circ, shots=1000).result().get_counts())

        # Compile circuit at 3 different optimisation levels
        for i in range(3):
            compiled_circ = transpile(circuit, backend, optimization_level=i+1)
            counts2 = self.preprocess_counts(backend.run(compiled_circ, shots=1000).result().get_counts())

            # Run the kstest on the two results
            ks_value = self.ks_test(counts1, counts2, 1000)
            print(f"Optimisation level {i+1} ks-test p-value: {ks_value}")

            # plot results
            if self.plot:
                self.plot_histogram(counts1, "Uncompiled Circuit Results", 0, circuit_number)
                self.plot_histogram(counts2, "Compiled Circuit Results", i+1, circuit_number)

        return ks_value
        
    
class cirqTesting(Base):
    def __init__(self):
        super().__init__()
    
    def run_circ(self, circuit : Circuit) -> Counter[int, int]:
        '''
        Runs circuit on cirq simulator and returns counts
        '''


class guppyTesting(Base):
    def __init__(self):
        super().__init__()

    def run_circ(self, circuit : Any, circuit_number : int) -> float:
        '''
        Compiles and runs guppy program on simulator and returns counts
        '''
        guppy.compile(circuit)

if __name__ == "__main__":
    b = Base()
    b.plot_histogram({1 : 10, 2 : 20})

