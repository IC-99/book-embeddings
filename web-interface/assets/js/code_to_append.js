function fileToString() {
    return new Promise((resolve, reject) => {
        const fileInput = document.getElementById('fileInput');
        if (fileInput.files.length === 0) {
            reject('Choose a file');
            return;
        }

        const file = fileInput.files[0];
        const reader = new FileReader();

        reader.onload = function() {
            try {
                const fileString = reader.result; // Contenuto del file come stringa
                resolve(fileString); // Risolve la Promise con la stringa JSON
            } catch (error) {
                reject('Error reading file: ' + error);
            }
        };

        reader.onerror = function() {
            reject('Error reading file: ' + reader.error);
        };

        reader.readAsText(file);
    });
}

// Aggiungi l'event listener solo dopo che il documento è stato caricato completamente
document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('run').addEventListener('click', function() {
        fileToString()
            .then(fileString => {
                // Qui puoi fare qualcosa con la stringa JSON
                console.log('File letto:', fileString);
                callMain([fileString]);
            })
            .catch(error => {
                // Gestisci gli errori qui
                console.error('Errore durante la lettura del file:', error);
            });
    });
});