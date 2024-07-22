console.log("WebAssembly is ready!");

const go = new Go();

let wasmInstance = null;

WebAssembly.instantiateStreaming(fetch("binary.wasm"), go.importObject).then((result) => {
    wasmInstance = result.instance;
    go.run(wasmInstance);
    console.log("WebAssembly module loaded.");
}).catch((err) => {
    console.error("Failed to instantiate WebAssembly module: " + err);
});

// Monitor for unhandled exceptions inside the WebAssembly module
function captureWasmExceptions() {
    const memory = wasmInstance.exports.memory;
    const originalAbort = go._passObjectToWasm;
    go._passObjectToWasm = function(obj) {
        try {
            return originalAbort(obj);
        } catch (e) {
            console.error("WebAssembly exception:", e);
            throw e;
        }
    };
}

// Call captureWasmExceptions after the module has been instantiated
WebAssembly.instantiateStreaming(fetch("binary.wasm"), go.importObject).then((result) => {
    wasmInstance = result.instance;
    captureWasmExceptions();
    go.run(wasmInstance);
}).catch((err) => {
    console.error("Failed to instantiate WebAssembly module: " + err);
});
