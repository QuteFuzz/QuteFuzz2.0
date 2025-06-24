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

from typing import List, Tuple
from collections import Counter
from itertools import zip_longest
from scipy.stats import ks_2samp
from numpy.typing import NDArray
import numpy as np
import sys
import argparse
import matplotlib.pyplot as plt

from pytket.circuit import Circuit
from pytket.extensions.qiskit import AerBackend
from pytket.extensions.qiskit import AerStateBackend
from pytket.extensions.quantinuum import QuantinuumBackend

class Base():

    def __init__(self):
        super().__init__()
        self.plot : bool = False
        self.parser = argparse.ArgumentParser()
        self.parser.add_argument('--plot', action='store_true',help='Plot results after running circuit')
        self.args = self.parser.parse_args()

    def preprocess_counts(self, counts : Counter[Tuple[str, ...], int]) -> Counter[int, int]:
        '''
            Given a dict mapping binary values to number of times they appear, return a sorted dict with each binary tuple converted into a base 10 int
            Dict is sorted by key
        '''
        out : Counter[int, int] = {}

        for k in counts.keys():
            out[int(k.replace(' ', ''), 2)] = counts[k]
            
        return dict(sorted(out.items()))
    
    def ks_test(counts1 : Counter[int, int], counts2 : Counter[int, int], total_shots : int) -> float:
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

    def plot_histogram(self, res : Counter[int, int]):
        values = list(res.keys())
        freqs = list(res.values())

        bar_width = 0.5
        plt.bar(values, freqs, width=bar_width, edgecolor='black')
        
        plt.xticks(values)
        plt.xlabel("Possible results")
        plt.ylabel("Number of occurances")
        plt.title("Results")
        plt.show()

class pytketTesting(Base):
    def __init__(self):
        super().__init__()
    
    def run_circ(circuit : Circuit):
        pass



if __name__ == "__main__":
    b = Base()
    b.plot_histogram({1 : 10, 2 : 20})

