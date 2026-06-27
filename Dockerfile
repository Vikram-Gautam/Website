FROM python:3.10-slim

# Install C++ compiler tools
RUN apt-get update && apt-get install -y \
    g++ \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app

RUN pip install --no-cache-dir -r requirements.txt

# Create separate engine bin folders
RUN mkdir -p huffman_engine/bin digit_engine/bin

# 1. Compile the Huffman Engine
RUN g++ -O3 huffman_engine/huffman_main.cpp huffman_engine/Huffman.cpp -o huffman_engine/bin/huffman

# 2. Compile the Digit Neural Network Engine
RUN g++ -O3 digit_engine/main.cpp digit_engine/NeuralNetwork.cpp digit_engine/Matrix.cpp digit_engine/MNISTLoader.cpp -o digit_engine/bin/digit_nn

EXPOSE 8000

CMD ["uvicorn", "app:app", "--host", "0.0.0.0", "--port", "8000"]