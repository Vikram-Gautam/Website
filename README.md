# High-Performance Web Portfolio Engines (C++ & FastAPI)

A hybrid, full-stack portfolio platform combining low-level high-performance calculations in native C++ with a modern, asynchronous web API built using Python and FastAPI. 

This repository hosts two core engines:
1. **MNIST Neural Network Engine:** A custom, low-level feedforward Neural Network (Multi-Layer Perceptron) built completely from scratch in C++ to recognize handwritten digits.
2. **Huffman Data Compressor:** A text utility that implements Huffman Coding tree structures in C++ to compress and decompress data payloads instantly.

---

## 🏗️ Architecture Overview

The platform uses a decoupled micro-engine design. The frontend client transmits layout data via JSON payloads to a Python FastAPI gateway. Python securely spawns localized native sub-processes to hand computation-heavy matrix math and string token parsing directly over to highly optimized C++ binary modules.