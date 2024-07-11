fetch('https://ivancarlini.bitbucket.io/articles/book-embedding/assets/json/graph.json')
    .then(response => response.json())
    .then(graph => {
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
                generateGraph(graph, order);
            }
        };
    })
    .catch(error => console.error('Errore:', error));

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
        layout: {
            hierarchical: {
                direction: 'UD' // Direzione del layout (da sinistra a destra)
            }
        },
        edges: {
            color: "#000000",
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
