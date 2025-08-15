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
    llvm-14 \
    llvm-14-dev \
    libllvm14 \
    libpolly-14-dev \
    && apt clean && rm -rf /var/lib/apt/lists/*

# Create and activate virtual environment
RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

# Set LLVM environment variable for Rust packages
ENV LLVM_SYS_140_PREFIX="/usr/lib/llvm-14"

# TODO: Change tket2 to tket in future
RUN pip install pytket qiskit pytket-qiskit matplotlib sympy z3-solver cirq pytket-quantinuum[pecos] tket qirrunner
RUN pip install selene-sim 

# Install latest guppylang from main branch on GitHub
RUN python3 -m pip install git+https://github.com/CQCL/guppylang.git@main#subdirectory=guppylang
# Install latest hugr-qir from main branch on GitHub
RUN python3 -m pip install git+https://github.com/CQCL/hugr-qir.git@main

# Allow configurable working directory
ARG WORKDIR_PATH=/qutefuzz
WORKDIR ${WORKDIR_PATH}

COPY . .

ENV PYTHONPATH=${WORKDIR_PATH}

CMD ["/bin/bash"]