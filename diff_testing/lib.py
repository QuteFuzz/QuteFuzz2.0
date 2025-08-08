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
from concurrent.futures import ThreadPoolExecutor, TimeoutError as FuturesTimeoutError

# Pytket imports
from pytket.circuit import Circuit
from pytket.passes import *
from pytket.extensions.qiskit import AerBackend
from pytket.extensions.qiskit import AerStateBackend

# Qiskit imports
from qiskit import QuantumCircuit, transpile

# Guppylang imports
from guppylang import guppy
from guppylang import enable_experimental_features
from guppylang.std.quantum import *
from guppylang.std.qsystem import *
from guppylang.std.builtins import result, array
enable_experimental_features()
from selene_sim import build, Quest
from hugr.qsystem.result import QsysResult


class Base():
    # Define the plots directory as a class variable
    OUTPUT_DIR = (pathlib.Path(__file__).parent.parent / "outputs").resolve()

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

        ks_stat, p_value = ks_2samp(sorted(sample1), sorted(sample2), method='asymp')

        return p_value
    
    def compare_statevectors(self, sv1 : NDArray[np.complex128], sv2 : NDArray[np.complex128], precision: int = 6) -> float:
        return np.round(abs(np.vdot(sv1, sv2)), precision)

    def save_interesting_circuit(self, circuit_number: int, interesting_dir: pathlib.Path) -> None:
        '''
        Saves an interesting circuit file to the specified directory
        '''

        interesting_dir.mkdir(parents=True, exist_ok=True)
        
        circuit_source_path = self.OUTPUT_DIR / f"circuit{circuit_number}" / "circuit.py"
        circuit_dest_path = interesting_dir / f"circuit{circuit_number}.py"
        
        if circuit_source_path.exists():
            try:
                shutil.copy2(circuit_source_path, circuit_dest_path)
                print(f"Circuit file saved to: {circuit_dest_path}")
            except Exception as e:
                print(f"Error copying circuit file: {e}")
        else:
            print(f"Warning: Circuit file not found at {circuit_source_path}")

    def plot_histogram(self, res : Counter[int, int], title : str, compilation_level : int, circuit_number : int = 0):
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
    
    def ks_diff_test(self, circuit : Circuit, circuit_number : int) -> None:
        '''
        Runs circuit on pytket simulator and returns counts
        '''
        backend = AerBackend()

        try:
            # Get original circuit shots
            uncompiled_circ = backend.get_compiled_circuit(circuit, optimisation_level=0)
            handle1 = backend.process_circuit(uncompiled_circ, n_shots=100000)
            result1 = backend.get_result(handle1)
            counts1 = self.preprocess_counts(result1.get_counts())
            is_testcase_interesting = False
            consistency_counter = 0

            # Compile circuit at 3 different optimisation levels
            for i in range(3):
                compiled_circ = backend.get_compiled_circuit(circuit, optimisation_level=i+1)
            
                # Process the compiled circuit
                handle2 = backend.process_circuit(compiled_circ, n_shots=100000)
                result2 = backend.get_result(handle2)
                counts2 = self.preprocess_counts(result2.get_counts())

                # Run the kstest on the two results
                ks_value = self.ks_test(counts1, counts2, 100000)
                print(f"Optimisation level {i+1} ks-test p-value: {ks_value}")

                # Heuristic to determine if the testcase is interesting
                if ks_value < 0.2 :
                    consistency_counter += 1
                if ks_value < 0.05 or consistency_counter >= 2:
                    is_testcase_interesting = True
                
                # plot results
                if self.plot:
                    self.plot_histogram(counts1, "Uncompiled Circuit Results", 0, circuit_number)
                    self.plot_histogram(counts2, "Compiled Circuit Results", i+1, circuit_number)

        except Exception as e:
            print("Exception :", traceback.format_exc())
            is_testcase_interesting = True

        # Dump files to a "interesting circuits" folder if found interesting testcase
        if is_testcase_interesting:
            print(f"Interesting circuit found: {circuit_number}")
            self.save_interesting_circuit(circuit_number, self.OUTPUT_DIR / "interesting_circuits")

    def run_circ_statevector(self, circuit : Circuit, circuit_number : int) -> NDArray[np.complex128]:
        '''
        Runs circuit on pytket simulator and returns statevector
        '''
        try:
            backend = AerStateBackend()
            # circuit with no passes
            uncompiled_circ = backend.get_compiled_circuit(circuit.copy(), optimisation_level=0)
            no_pass_statevector = uncompiled_circ.get_statevector()

            # Get statevector after every optimisation level
            for i in range(3):
                compiled_circ = backend.get_compiled_circuit(circuit.copy(), optimisation_level=i+1)
                pass_statevector = compiled_circ.get_statevector()
                dot_prod = self.compare_statevectors(no_pass_statevector, pass_statevector, 6)

                if dot_prod == 1:
                    print("Statevectors are the same\n")
                else:
                    print ("Statevectors not the same")
                    print("Dot product: ", dot_prod)

        except Exception:
            print("Exception :", traceback.format_exc())

    def run_guppy_pytket_diff(self, circuit : Circuit, circuit_number : int, qubit_defs_list : list[int]) -> None:
        '''
        Loads pytket circuit as a guppy circuit and runs it, comparing the results
        with normal AerBackend
        '''
        pytket_circ_copy = circuit.copy()
        guppy_circuit = guppy.load_pytket("guppy_circuit", circuit)

        @guppy.comptime
        def main() -> None:
            qubit_variables = []
            
            for qubit_def in qubit_defs_list:
                if qubit_def > 0:
                    qubit_array = [qubit() for _ in range(qubit_def)]
                    qubit_variables.append(qubit_array)
                else:
                    single_qubit = qubit()
                    qubit_variables.append(single_qubit)

            guppy_circuit(*qubit_variables)
            for r in range(len(qubit_variables)):
                if isinstance(qubit_variables[r], list):
                    result(f"c{r}", measure_array(qubit_variables[r]))

        try:
            # Getting guppy circuit results
            compiled_circ = main.compile()
            runner = build(compiled_circ)
            results = QsysResult(
                runner.run_shots(Quest(), n_qubits=circuit.n_qubits, n_shots=10000)
            )
            counts_guppy = results.collated_counts()
            counts_guppy = Counter({key[0][1]: value for key, value in counts_guppy.items()})

            # Getting uncompiled pytket circuit results
            backend = AerBackend()
            uncompiled_pytket_circ = backend.get_compiled_circuit(pytket_circ_copy, optimisation_level=0)
            handle = backend.process_circuit(uncompiled_pytket_circ, n_shots=10000)
            result_pytket = backend.get_result(handle)
            counts_pytket = self.preprocess_counts(result_pytket.get_counts())

            # Run the kstest on the two results
            ks_value = self.ks_test(counts_guppy, counts_pytket, 10000)
            print(f"Guppy vs Pytket ks-test p-value: {ks_value}")

            # Heuristic to determine if the testcase is interesting
            if ks_value < 0.05:
                print(f"Interesting circuit found: {circuit_number}")
                self.save_interesting_circuit(circuit_number, self.OUTPUT_DIR / "interesting_circuits")
            
            if self.plot:
                self.plot_histogram(counts_guppy, "Guppy Circuit Results", 0, circuit_number)
                self.plot_histogram(counts_pytket, "Pytket Circuit Results", 0, circuit_number)

        except Exception as e:
            from guppylang_internals.error import GuppyError
            if isinstance(e, GuppyError):
                from guppylang_internals.diagnostic import DiagnosticsRenderer
                from guppylang_internals.engine import DEF_STORE

                renderer = DiagnosticsRenderer(DEF_STORE.sources)
                renderer.render_diagnostic(e.error)
                sys.stderr.write("\n".join(renderer.buffer))
                sys.stderr.write("\n\nGuppy compilation failed due to 1 previous error\n")
            else:
                # If it's not a GuppyError, fall back to default hook
                print("Error during compilation:", e)
                print("Exception :", traceback.format_exc())

class qiskitTesting(Base):
    def __init__(self):
        super().__init__()

    def ks_diff_test(self, circuit : Circuit, circuit_number : int) -> float:
        '''
        Runs circuit on qiskit simulator and returns counts
        '''

        from qiskit_aer import AerSimulator
        backend = AerSimulator()
        
        # Get original circuit shots
        uncompiled_circ = transpile(circuit, backend, optimization_level=0)
        counts1 = self.preprocess_counts(backend.run(uncompiled_circ, shots=10000).result().get_counts())

        # Compile circuit at 3 different optimisation levels
        for i in range(3):
            compiled_circ = transpile(circuit, backend, optimization_level=i+1)
            counts2 = self.preprocess_counts(backend.run(compiled_circ, shots=10000).result().get_counts())

            # Run the kstest on the two results
            ks_value = self.ks_test(counts1, counts2, 10000)
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

    def ks_diff_test(self, circuit : Any, circuit_number : int) -> None:
        '''
        Compiles guppy circuit
        '''
        is_testcase_interesting = False
        timeout_seconds = 60
        
        def compile_circuit():
            return circuit.compile()
        
        # Run the compile with timeout using ThreadPoolExecutor
        try:
            with ThreadPoolExecutor(max_workers=1) as executor:
                future = executor.submit(compile_circuit)
                result = future.result(timeout=timeout_seconds)
                
        except FuturesTimeoutError:
            print(f"Compilation timed out after {timeout_seconds} seconds")
            is_testcase_interesting = True
        except Exception as e:
            from guppylang_internals.error import GuppyError
            if isinstance(e, GuppyError):
                from guppylang_internals.diagnostic import DiagnosticsRenderer
                from guppylang_internals.engine import DEF_STORE

                renderer = DiagnosticsRenderer(DEF_STORE.sources)
                renderer.render_diagnostic(e.error)
                sys.stderr.write("\n".join(renderer.buffer))
                sys.stderr.write("\n\nGuppy compilation failed due to 1 previous error\n")
                return

            # If it's not a GuppyError, fall back to default hook
            print("Error during compilation:", e)
            is_testcase_interesting = True

        # Dump files to a "interesting circuits" folder if found interesting testcase
        if is_testcase_interesting:
            print(f"Interesting circuit found: {circuit_number}")
            interesting_dir = self.OUTPUT_DIR / "interesting_circuits"
            self.save_interesting_circuit(circuit_number, interesting_dir)

