
const huffmanInput = document.getElementById('huffman-input');
const btnCompress = document.getElementById('btn-compress');
const btnDecompress = document.getElementById('btn-decompress');
const huffmanBitstream = document.getElementById('huffman-bitstream');
const huffmanRatio = document.getElementById('huffman-ratio');
const huffmanDecoded = document.getElementById('huffman-decoded');

const canvas = document.getElementById('paint-canvas');
const ctx = canvas.getContext('2d');  // for canvas
const btnClearCanvas = document.getElementById('btn-clear-canvas');
const btnTrain = document.getElementById('btn-train');
const btnPredict = document.getElementById('btn-predict');
const nnEpochs = document.getElementById('nn-epochs');
const nnLr = document.getElementById('nn-lr');
const nnConsoleOutput = document.getElementById('nn-console-output');
const nnPredictionDigit = document.getElementById('nn-prediction-digit');


//drawing logic 
let isDrawing = false;


ctx.lineWidth = 16; // had to make it thick so as data set also has thick strokes
ctx.lineCap = 'round'; // for smooth edges
ctx.strokeStyle = '#ffffff'; // white bacgrund

function startDrawing(e) { // e is for eventlistening 
    isDrawing = true;
    draw(e);
}

function stopDrawing() {
    isDrawing = false;
    ctx.beginPath(); // Resets path so lines don't interconnect randomly
}

function draw(e) {
    if (!isDrawing) return; //incase the mouse is moving without a click

    // Calculate mouse coordinates relative to the canvas bounding box
    const rect = canvas.getBoundingClientRect(); //edges based on the current web window

    //coordinate relatative to corner
    const x = e.clientX - rect.left; 
    const y = e.clientY - rect.top;

    //drawing logic 
    ctx.lineTo(x, y);
    ctx.stroke();
    ctx.beginPath();
    ctx.moveTo(x, y);
}

// event listeners for drawing 
canvas.addEventListener('mousedown', startDrawing); //clikcs
canvas.addEventListener('mouseup', stopDrawing); // lets go 

canvas.addEventListener('mousemove', draw); // moves
canvas.addEventListener('mouseleave', stopDrawing); //leaves the canvas

// Clear the sketchpad
btnClearCanvas.addEventListener('click', () => { //arrow notation cause why not 
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    nnPredictionDigit.innerText = '-'; //reset place hoolder
});

btnCompress.addEventListener('click', async () => { //async so we don't freeze
    const textValue = huffmanInput.value;
    if (!textValue) return alert("Please type some text first!");

    try {
        const response = await fetch('http://website-pykn.onrender.com/compress', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ text: textValue }) // convert json to string
        });

        if (!response.ok) throw new Error('Backend compilation failure.');
        const data = await response.json(); //convert data into json

        // Update the text
        huffmanBitstream.innerText = data.compressed_bits;
        huffmanRatio.innerText = `${data.compression_ratio} bits saved`;
    } catch (err) {
        huffmanBitstream.innerText = `Error: ${err.message}`;
    }
});

btnDecompress.addEventListener('click', async () => {
    // 1. Grab the raw bitstream from the screen text layout
    const bitstreamValue = huffmanBitstream.innerText;
    
    // Safety check to ensure we aren't sending empty strings or hyphens
    if (bitstreamValue === '-' || bitstreamValue.startsWith('Error') || !bitstreamValue) {
        return alert("No valid bitstream found to decode!");
    }

    // 🔍 DEBUG LINE: This prints exactly what your variable holds BEFORE sending it!
    console.log("What JavaScript is trying to send to Python:", bitstreamValue);

    try {
        const response = await fetch('http://website-pykn.onrender.com/decompress', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            // 🔥 CRITICAL: It must be exactly 'bits' in lowercase!
            body: JSON.stringify({ bits: bitstreamValue }) 
        });

        const data = await response.json();
        console.log("Decompress response payload object:", data);

        if (data.detail) {
            huffmanDecoded.innerText = `Error: ${JSON.stringify(data.detail)}`;
        } else {
            huffmanDecoded.innerText = data.decoded_text;
        }
        
    } catch (err) {
        huffmanDecoded.innerText = `Decoding execution failed: ${err.message}`;
    }
});


//neural network
btnPredict.addEventListener('click', async () => {
    // Create an off-screen helper canvas to downscale our 280x280 canvas to MNIST 28x28
    const tempCanvas = document.createElement('canvas');
    tempCanvas.width = 28;
    tempCanvas.height = 28;
    const tempCtx = tempCanvas.getContext('2d');
    

    tempCtx.drawImage(canvas, 0, 0, 28, 28); //down scales it automatically
    
   
    const imgData = tempCtx.getImageData(0, 0, 28, 28); //get raw data (r,g,b, alpha)
    const pixels = imgData.data; // 28*28*4
    
    let grayscaleStrings = [];
    
    //extract only alpha
    for (let i = 0; i < pixels.length; i += 4) {
        let alpha = pixels[i + 3]; 

        if (alpha > 30) {
        alpha = Math.min(255, alpha * 1.5); // Boost faint edges
    }
        grayscaleStrings.push(alpha);
    }
    
    // everyvalue is sperated by space
    const pixelPayloadString = grayscaleStrings.join(' ');

    nnPredictionDigit.innerText = "Calculating...";

    try {
        const response = await fetch('http://website-pykn.onrender.com/predict', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ pixels: pixelPayloadString })
        });

        const data = await response.json();
        nnPredictionDigit.innerText = data.prediction;
    } catch (err) {
        nnPredictionDigit.innerText = "Err";
    }
});

btnTrain.addEventListener('click', async () => {
    // 1. Grab the values from your input number boxes
    const epochsValue = parseInt(nnEpochs.value);
    const lrValue = parseFloat(nnLr.value);

    // Safety guard validation
    if (isNaN(epochsValue) || isNaN(lrValue)) {
        return alert("Please enter valid numbers for both Epochs and Learning Rate!");
    }

    // Change UI state to let the user know the C++ binary is processing
    nnConsoleOutput.innerText = "Starting engine training pipeline...\nExecuting C++ gradient descent iterations...";
    btnTrain.disabled = true; // Temporary lock so the user doesn't spam click

    try {
        const response = await fetch('http://website-pykn.onrender.com/train', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ 
                epochs: epochsValue, 
                learning_rate: lrValue 
            })
        });

        const data = await response.json();

        if (!response.ok) {
            throw new Error(data.detail || "Training process terminated unexpectedly.");
        }

        // 2. Dump the compiled standard output terminal log right into your UI console box!
        nnConsoleOutput.innerText = data.log;

    } catch (err) {
        nnConsoleOutput.innerText = `Training Failed:\n${err.message}`;
    } finally {
        // Unlock the button when processing completes
        btnTrain.disabled = false;
    }
});