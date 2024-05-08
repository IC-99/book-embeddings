fetch('graph.json')
    .then(response => response.json())
    .then(graph => {
        const sortableList = document.getElementById('sortable');

        // Genera la lista ordinata a partire dai nodi del grafo
        const sortedNodes = graph.nodes.sort((a, b) => a.id - b.id);
        sortedNodes.forEach(node => {
            const listItem = document.createElement('li');
            listItem.textContent = node.label; // Usa la proprietà "label" del nodo come testo dell'elemento
            sortableList.appendChild(listItem); // Aggiungi l'elemento alla lista
        });

        // Inizializza il drag and drop dopo aver caricato tutti gli elementi
        initDragAndDrop(graph);
        // Genera il grafo iniziale
        generateGraph(graph);
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
            generateGraph(graph);
        }
    });
}

// Funzione per generare il grafo utilizzando l'ordinamento corrente degli elementi nella lista
function generateGraph(graph) {
    // Ottieni la lista ordinabile
    const sortableList = document.getElementById('sortable');
    const items = Array.from(sortableList.querySelectorAll('li'));
    const order = items.map(item => item.textContent.trim());

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
                direction: 'LR', // Direzione del layout (da sinistra a destra)
            }
        },
        edges: {
            color: "#000000",
            arrows: {
                to: true // Indica che gli archi sono diretti verso il nodo di arrivo
            },
            smooth: {
                type: 'curvedCCW', // Imposta il tipo di curvatura dell'arco
                forceDirection: 'horizontal', // Forza la direzione orizzontale
            }
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
}