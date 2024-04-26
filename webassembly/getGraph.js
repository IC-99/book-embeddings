fetch('graph.json')
    .then(response => response.json())
    .then(graph => {
        // Ora puoi accedere alle informazioni del grafo
        // Crea un nuovo oggetto DataSet con i dati del grafo
        console.log(graph);

        const nodes = new vis.DataSet();
        
        const edges = new vis.DataSet();

        const mapping = new Map()

        i = 0
        graph["order"].forEach(label => {
            mapping.set(label, i)
            i += 1
        });

        graph["nodes"].forEach(node => {
            nodes.add({ id: mapping.get(node.id), label: node.label, level: 0 });
        });

        graph["edges"].forEach(edge => {
            edges.add({ from: mapping.get(edge.from), to: mapping.get(edge.to) });
        });

        // Definisci le opzioni del grafo
        const options = {
            layout: {
            hierarchical: {
                direction: 'LR', // Direzione del layout (da sinistra a destra)
                //sortMethod: 'directed' // Metodo di ordinamento (per evitare sovrapposizioni)
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
  })
  .catch(error => console.error('Errore nel caricamento del file JSON:', error));