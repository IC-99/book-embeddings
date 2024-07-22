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

function getGraphFromFileString(fileString) {
	G = {
        nodes: [],
        edges: []
    };

	var i = 0;
	var current = "";
	var readingNodes = true;
	var sourceNode = -1;

	while (i < fileString.length) {
		if (fileString[i] == '\n') {
			if (readingNodes) {
			    G.nodes.push({id: current, label: current});
			}
			else {
                G.edges.push({from: sourceNode, to: current});
			}
			current = "";
		}
		else if (fileString[i] == ',') {
			sourceNode = current;
			current = "";
			readingNodes = false;
		}
		else {
			current = current + fileString[i];
		}
		i++;
	}
    if (current.length > 0) {
		if (readingNodes) {
			G.nodes.push({id: current, label: current});
		}
		else {
			G.edges.push({from: sourceNode, to: current});
		}
	}
    return G;
}

function getOrderAndDraw(graph) {
    // Intercetta i messaggi di log per ottenere l'ordine calcolato
    const originalConsoleLog = console.log;
    let order = [];

    console.log = function(message) {
        originalConsoleLog.apply(console, arguments);
        // Cerca il messaggio che contiene l'ordine (adatta questa parte alla tua specifica formattazione)
        if (typeof message === 'string' && message.startsWith('RISULTATO: ')) {
            order = message.replace('RISULTATO: ', '').split(' ').map(item => item.trim());
            console.log = originalConsoleLog; // Ripristina console.log
            // Genera il grafo con l'ordine estratto
            order.pop(); // rimuove l'elemento vuoto dovuto allo ' ' finale
            generateGraph(graph, order);
        }
    };
}

// Aggiungi l'event listener solo dopo che il documento è stato caricato completamente
document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('run').addEventListener('click', function() {
        fileToString()
            .then(fileString => {
                const graph = getGraphFromFileString(fileString);
                getOrderAndDraw(graph);
            })
            .catch(error => {
                // Gestisci gli errori qui
                console.error('Errore durante la lettura del file:', error);
            });
    });
})

// Funzione per inizializzare il drag and drop
function initDragAndDrop(graph) {
    // Ottieni la lista ordinabile
    const sortableList = document.getElementById('sortable');

    // Inizializza il drag and drop
    new Sortable(sortableList, {
        animation: 150, // Durata dell'animazione
        onEnd: function (evt) {
            // Quando il drag and drop è completato, generiamo il grafo
            //generateGraph(graph); // Rimuoviamo questa linea poiché ora aspettiamo l'ordine dai log
        }
    });
}

// Funzione per generare il grafo utilizzando l'ordinamento corrente degli elementi nella lista
function generateGraph(graph, order) {
    console.log(order);

    const nodes = new vis.DataSet();
    const edges = new vis.DataSet();
    const mapping = new Map();

    let i = 0;
    order.forEach(label => {
        mapping.set(label, i);
        i += 1;
    });

    graph.nodes.forEach(node => {
        nodes.add({ id: mapping.get(node.id), label: node.label, level: 0 });
    });

    graph.edges.forEach(edge => {
        edges.add({ from: mapping.get(edge.from), to: mapping.get(edge.to) });
    });

    // Definisci le opzioni del grafo
    const options = {
        nodes: {
            color: "#76e0f5",
            font: {
                color: "black"
            }
        },
        layout: {
            hierarchical: {
                direction: 'UD' // Direzione del layout (da sinistra a destra)
            }
        },
        edges: {
            color: "black",
            arrows: {
                to: true // Indica che gli archi sono diretti verso il nodo di arrivo
            },
            smooth: {
                type: 'curvedCW' // Imposta il tipo di curvatura dell'arco
                //forceDirection: 'horizontal', // Forza la direzione orizzontale
            },
            physics: true
        }
    };

    // Crea un oggetto Network utilizzando i dati e le opzioni definite sopra
    const container = document.getElementById("network");
    const data = { nodes: nodes, edges: edges };
    const network = new vis.Network(container, data, options);
    const containerWidth = container.offsetWidth;
    const containerHeight = container.offsetHeight;

    // Aggiorna le dimensioni del grafo
    network.setSize(containerWidth, containerHeight);

    network.on("dragEnd", function(params) {
        // Muovi il grafo al centro
        network.moveTo({ position: { x: 0, y: 0 }, scale: 0.8 });
    });

    network.on("afterDrawing", function(ctx) {
        var dataURL = ctx.canvas.toDataURL();
        document.getElementById('canvasImg').href = dataURL;
    })
}
