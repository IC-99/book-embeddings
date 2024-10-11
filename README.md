# Enumeration of 1-Page Book Embeddings of Directed Acyclic Graphs

This repository contains the source code and documentation related to the enumeration of 1-stack layouts for Directed Acyclic Graphs (DAGs). The code includes both a WebAssembly-based web system and C++ examples for using the `OneStackLayoutsEnumerator` class.

## Repository Structure

- **code/**: Contains the C++ source code in two versions:
  - The first version is used for compiling the WebAssembly file required for the web system.
  - The second version provides an example of how to use the `OneStackLayoutsEnumerator` class directly in C++.
  - This folder also includes shell scripts (`.sh`) for compiling and running the code or for generating the WebAssembly file.
  
- **doc/**: Includes documentation files for the `OneStackLayoutsEnumerator` class, detailing its usage and functionalities.

- **graph file examples/**: Contains a set of example input graph files that can be used with the code.

- **thesis/**: Contains my master's thesis along with additional related files.

- **web-system/**: Contains the source code for the web system available at [this link](https://ic-99.github.io/book-embeddings/), where the algorithm for enumerating 1-stack layouts is implemented and visualized.

## Requirements

The code requires the following libraries:
- **OGDF (Open Graph Drawing Framework)**: Necessary for compiling and running the C++ code. You can download it from [here](https://ogdf.uos.de/).

## Usage

### Compiling the Code
To compile and run the code or to generate the WebAssembly file, navigate to the `code` folder and use the provided shell scripts:

- `compile-wasm.sh`: Generates the WebAssembly file for the web system.

Make sure OGDF is installed and properly configured before running the scripts.

### Web System
The web system source code is available in the `web-system` folder. It allows users to interact with the algorithm directly from a browser without requiring any additional software installation. The system is accessible at [this link](https://ic-99.github.io/book-embeddings/).

### Documentation
Refer to the `doc` folder for detailed instructions on how to use the `OneStackLayoutsEnumerator` class in your projects.

## License
This project is licensed under the MIT License. See the LICENSE file for details.

