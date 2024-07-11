// Funzione per avviare il processo dopo il caricamento di Module
async function startProcess() {
    const fileInput = document.getElementById('fileInput');
    if (fileInput.files.length === 0) {
        alert('Choose a file');
        return;
    }

    const file = fileInput.files[0];
    const arrayBuffer = await file.arrayBuffer();
    const uint8Array = new Uint8Array(arrayBuffer);

    // Carica il modulo WebAssembly
    const Module = await Module();
    const memory = Module._malloc(uint8Array.length);
    Module.HEAPU8.set(uint8Array, memory);

    // Chiamare la funzione init per inizializzare la memoria
    Module._init(memory, uint8Array.length);

    // Chiamare la funzione Wasm per processare il file
    const resultPointer = Module._processFile(memory, uint8Array.length);

    // Recuperare i risultati dalla memoria Wasm
    const result = Module.HEAPU8.subarray(resultPointer, resultPointer + uint8Array.length);
    const resultText = new TextDecoder().decode(result);

    document.getElementById('output').textContent = resultText;

    // Libera la memoria allocata
    Module._free(memory);
}

// Aggiungi l'event listener solo dopo che il documento è stato caricato completamente
document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('processButton').addEventListener('click', startProcess);
});