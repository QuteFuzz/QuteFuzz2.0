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

# Create and activate virtual environment
RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

# TODO: Change tket2 to tket in future
RUN pip install pytket qiskit pytket-qiskit matplotlib sympy z3-solver cirq pytket-quantinuum tket
RUN pip install selene-sim 

# Install latest guppylang from main branch on GitHub
RUN python3 -m pip install git+https://github.com/CQCL/guppylang.git@main

# Allow configurable working directory
ARG WORKDIR_PATH=/qutefuzz
WORKDIR ${WORKDIR_PATH}

COPY . .

ENV PYTHONPATH=${WORKDIR_PATH}

CMD ["/bin/bash"]