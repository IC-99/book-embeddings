fetch('graph.json')
    .then(response => response.json())
    .then(graph => {
        // Ora puoi accedere alle informazioni del grafo
        // Crea un nuovo oggetto DataSet con i dati del grafo
        console.log(graph);

        const nodes = new vis.DataSet();
        
        const edges = new vis.DataSet(graph.edges);

        graph["order"].forEach(id => {
            const node = graph.nodes.find(node => node.id === id);
            nodes.add({ id: node.id, label: node.label, level: 0 });
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


  })
  .catch(error => console.error('Errore nel caricamento del file JSON:', error));