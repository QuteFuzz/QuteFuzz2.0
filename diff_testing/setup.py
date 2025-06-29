from setuptools import setup, find_packages

setup(
    name='diff_testing',
    version='0.1',
    packages=find_packages(),
    install_requires=['qiskit', 'pytket', 'cirq', 'pytket-qiskit', 'matplotlib', 'sympy', 'z3-solver', 'pytket-qiskit', 'pytket-quantinuum', 'pygame'],
)
