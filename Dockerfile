# base image

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
    clang \
    cmake \
    graphviz \
    git \
    curl \
    build-essential \
    python3 \
    python3-pip \
    python3-venv \
    gdb \
    && apt clean && rm -rf /var/lib/apt/lists/*

RUN python3 -m pip install pytket qiskit pytket-qiskit matplotlib sympy z3-solver pytket-quantinuum cirq --break-system-packages

# Install latest guppylang from main branch on GitHub
RUN python3 -m pip install git+https://github.com/CQCL/guppylang.git@main --break-system-packages

WORKDIR /qutefuzz

COPY . .

ENV PYTHONPATH=/qutefuzz

CMD ["/bin/bash"]