from setuptools import setup, find_packages

setup(
    name='QuteFuzz',
    version='2.0',
    packages=find_packages(),
    install_requires=['qiskit', 'pytket', 'cirq', 'pytket-qiskit', 'matplotlib', 'sympy', 'z3-solver', 'pytket-qiskit', 'pytket-quantinuum', 'pygame'],
)
