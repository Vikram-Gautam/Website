import subprocess
from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware #By default, web browsers block a local HTML file on your desktop from communicating with a server running on a port
from pydantic import BaseModel   
import os                   

app = FastAPI(title="High-Performance Portfolio API")

# Enable CORS(Cross-Origin Resource Sharing) so the frontend HTML webpage can talk to this API seamlessly
app.add_middleware( 
    CORSMiddleware,
    allow_origins=["https://front-end-1fch.onrender.com",  # Explicitly allow your frontend site
        "http://localhost:5500",               # Keeps local development working if needed
        "http://127.0.0.1:5500"],  
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)



class DigitPredictData(BaseModel): #pydantic inforces some rules on the data, as the data arrives in JSON
    pixels: str  # A space-separated string of 784 pixel values (0-255)

class DigitTrainData(BaseModel):
    epochs: int
    learning_rate: float

class HuffmanEncodeData(BaseModel):
    text: str    # The raw string text to compress

class HuffmanDecodeData(BaseModel):
    bits: str    # The 0s and 1s bitstream to decompress


#JSon syntax for future ref
#Data is in name/value pairs
#Data is separated by commas
#Curly braces hold objects
#Square brackets hold arrays




@app.post("/predict")
def predict_digit(data: DigitPredictData): #the data is of the class DigitPredictData which stores pixels in the form of string
    try:
        # Pass "predict" as argv[1] and the pixel sequence string as argv[2]
    
        base_dir = os.path.dirname(os.path.abspath(__file__))
        
        # 2. Build absolute paths to the folder and the executable file directly
        engine_dir = os.path.join(base_dir, "digit_engine", "data") # cwd here so it finds data/weights
        exe_path = os.path.join(base_dir, "digit_engine", "bin", "digit_nn")

        result = subprocess.run(
            [exe_path, "predict", data.pixels],
            cwd=engine_dir,
            capture_output=True,
            text=True,
            check=True
        )
        prediction = result.stdout.strip()
        return {"prediction": prediction} #json format

    except subprocess.CalledProcessError as e: #execution fail    f is a format string literal
        raise HTTPException(status_code=500, detail=f"C++ NN Predict Error: {e.stderr}") #some special error code format
    except FileNotFoundError: #if it can't find the file
        raise HTTPException(status_code=500, detail="digit_nn.exe binary not found in backend_engines/ folder.")
#500 is internal server error

@app.post("/train")
def train_model(data: DigitTrainData):
    try:
   # 1. Get the exact absolute folder path of app.py
        base_dir = os.path.dirname(os.path.abspath(__file__))
        
        # 2. Build absolute paths to the folder and the executable file directly
        engine_dir = os.path.join(base_dir, "digit_engine", "data") # cwd here so it finds data/weights
        exe_path = os.path.join(base_dir, "digit_engine", "bin", "digit_nn")

        # 3. Run using the absolute path to the executable file
        result = subprocess.run(
            [
                exe_path, 
                "train", 
                str(data.epochs), 
                str(data.learning_rate)
            ],
            cwd=engine_dir, # This ensures the C++ app finds its train byte files
            capture_output=True,
            text=True,
            check=True
        )
        
        # Capture the printed epoch accuracy updates
        console_log = result.stdout.strip()
        return {"status": "Training Complete", "log": console_log}

    except subprocess.CalledProcessError as e:
        raise HTTPException(status_code=500, detail=f"C++ NN Training Error: {e.stderr}")
    except FileNotFoundError:
        raise HTTPException(status_code=500, detail="digit_nn.exe binary not found in backend_engines/ folder.")



@app.post("/compress")
def compress_text(data: HuffmanEncodeData):
    try:
        # Pass the raw text as argv[1] and mode "encode" as argv[2]
        base_dir = os.path.dirname(os.path.abspath(__file__))
        huffman_exe = os.path.join(base_dir, "huffman_engine", "bin", "huffman")
    
        result = subprocess.run(
            [huffman_exe, data.text, "encode"],
            capture_output=True,
            text=True,
            check=True
        )
        
        bitstream = result.stdout.strip()
        
        # for to caluclate thee rationi
        original_bits = len(data.text) * 8
        compressed_bits = len(bitstream)
        
        if original_bits > 0:
            ratio_percentage = int((1 - (compressed_bits / original_bits)) * 100)
            if ratio_percentage < 0: 
                ratio_percentage = 0  # Safety buffer for edge cases
        else:
            ratio_percentage = 0

        return {
            "compressed_bits": bitstream,
            "compression_ratio": f"{ratio_percentage}%"
        }

    except subprocess.CalledProcessError as e:
        raise HTTPException(status_code=500, detail=f"C++ Huffman Encoder Error: {e.stderr}")
    except FileNotFoundError:
        raise HTTPException(status_code=500, detail="huffman.exe binary not found in backend_engines/ folder.")


@app.post("/decompress")
def decompress_bits(data: HuffmanDecodeData):
    try:
        # Pass the bitstream as argv[1] and mode "decode" as argv[2]
        base_dir = os.path.dirname(os.path.abspath(__file__))
        huffman_exe = os.path.join(base_dir, "huffman_engine", "bin", "huffman")
        result = subprocess.run(
            [huffman_exe, data.bits, "decode"],
            capture_output=True,
            text=True,
            check=True
        )
        
        decoded_text = result.stdout.strip()
        return {"decoded_text": decoded_text}
        
    except subprocess.CalledProcessError as e:
        raise HTTPException(status_code=500, detail=f"C++ Huffman Decoder Error: {e.stderr}")
    except FileNotFoundError:
        raise HTTPException(status_code=500, detail="huffman.exe binary not found in backend_engines/ folder.")