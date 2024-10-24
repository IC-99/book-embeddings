var graph = {
    nodes: [{"id": "0","label": "0"},{"id": "1","label": "1"},{"id": "2","label": "2"},{"id": "3","label": "3"},{"id": "4","label": "4"},{"id":"5","label": "5"},{"id": "6","label": "6"},{"id": "7","label": "7"},{"id": "8","label": "8"},{"id": "9","label": "9"},{"id":"10","label": "10"}],
    edges: [{"from": "0","to": "1"},{"from": "0","to": "2"},{"from": "0","to": "8"},{"from": "1","to": "2"},{"from": "1","to": "3"},{"from": "1","to": "10"},{"from": "4","to": "3"},{"from": "5","to": "3"},{"from": "6","to": "3"},{"from": "7","to": "3"},{"from": "7","to": "6"},{"from": "9","to": "6"}]
};
var layouts = [["0","8","1","10","4","5","7","9","6","3","2"],["0","8","1","4","5","7","9","6","3","10","2"],["0","8","1","10","4","7","9","6","5","3","2"],["0","8","1","4","7","9","6","5","3","10","2"],["0","8","1","10","5","4","7","9","6","3","2"],["0","8","1","5","4","7","9","6","3","10","2"],["0","8","1","10","5","7","9","6","4","3","2"],["0","8","1","5","7","9","6","4","3","10","2"],["0","8","1","10","7","9","6","4","5","3","2"],["0","8","1","7","9","6","4","5","3","10","2"],["0","8","1","10","7","9","6","5","4","3","2"],["0","8","1","7","9","6","5","4","3","10","2"],["0","1","10","4","5","7","9","6","3","2","8"],["0","1","4","5","7","9","6","3","10","2","8"],["0","1","10","4","7","9","6","5","3","2","8"],["0","1","4","7","9","6","5","3","10","2","8"],["0","1","10","5","4","7","9","6","3","2","8"],["0","1","5","4","7","9","6","3","10","2","8"],["0","1","10","5","7","9","6","4","3","2","8"],["0","1","5","7","9","6","4","3","10","2","8"],["0","1","10","7","9","6","4","5","3","2","8"],["0","1","7","9","6","4","5","3","10","2","8"],["0","1","10","7","9","6","5","4","3","2","8"],["0","1","7","9","6","5","4","3","10","2","8"]];
var currentIndex = 0;
var numberOfLayouts = 24;

updateStatistics();
drawLayout(graph, layouts[currentIndex]);

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
                const fileString = reader.result; // file content as a string
                resolve(fileString);
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

document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('run').addEventListener('click', function() {
        fileToString()
            .then(fileString => {
                graph = getGraphFromFileString(fileString);
                layouts = [];
                currentIndex = 0;
                callMain([fileString]);
                if (numberOfLayouts == 0) {
                    currentIndex = -1;
                    updateStatistics();
                    drawLayout({nodes: [], edges: []}, []);
                }
                else {
                    updateStatistics();
                    drawLayout(graph, layouts[currentIndex]); 
                }               
            })
            .catch(error => {
                console.error('Errore durante la lettura del file:', error);
            });
    });
});

document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('next').addEventListener('click', function() {
        fileToString()
            .then(fileString => {
                if (currentIndex == layouts.length - 1) {
                    var computeNextLayout = Module["_printNextLayout"];
                    computeNextLayout();
                }
                if (currentIndex < numberOfLayouts - 1) {
                    currentIndex++;
                    updateStatistics();
                    drawLayout(graph, layouts[currentIndex]); 
                }                              
            })
            .catch(error => {
                if (currentIndex < numberOfLayouts - 1) {
                    currentIndex++;
                    updateStatistics();
                    drawLayout(graph, layouts[currentIndex]); 
                }   
            });
    });
});

document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('prev').addEventListener('click', function() {
        if (currentIndex > 0) {
            currentIndex--;
            updateStatistics();
            drawLayout(graph, layouts[currentIndex]);
        }
    });
});

document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('downloadGraph').addEventListener('click', function() {

        graphString = getGraphString();

        const blob = new Blob([graphString], { type: 'text/plain' });
        const url = URL.createObjectURL(blob);

        const a = document.createElement('a');
        a.href = url;
        a.download = 'graph';  // name of the file
        document.body.appendChild(a);
        a.click();

        document.body.removeChild(a);
            URL.revokeObjectURL(url); 
    });
});

document.getElementById('fileInput').addEventListener('change', function() {
    var fileName = this.files[0].name;
    document.getElementById('fileName').textContent = 'Selected file: ' + fileName;
    var runButton = document.getElementById("run");
    runButton.style.display = "block";
});

function getGraphString() {
    graphString = "";
    
    graph.nodes.forEach((node) => graphString += node.id + "\n");
    graph.edges.forEach((edge) => graphString += edge.from + "," + edge.to + "\n");
    return graphString;
}

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
                const fileString = reader.result; // file content as a string
                resolve(fileString);
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
        else if (fileString[i] != '\r') {
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

function getDataFromWasm(message) {
    console.log(message);
    
    if (typeof message === 'string' && message.startsWith('RESULT: ')) {
        let layout = message.replace('RESULT: ', '').split(' ').map(item => item.trim());
        layout.pop(); // remove the last element ' '
        layouts.push(layout);
    }
    if (typeof message === 'string' && message.startsWith('NUMBER OF LAYOUTS: ')) {
        numberOfLayouts = parseInt(message.replace('NUMBER OF LAYOUTS: ', '').split(' ').map(item => item.trim())[0]);
        updateStatistics();
    }
    return;
}

function updateStatistics() {
    document.getElementById("index").innerText = "layout " + (currentIndex + 1) + " of " + numberOfLayouts;
}

function drawLayout(graph, layout) {

    const nodes = new vis.DataSet();
    const edges = new vis.DataSet();
    const mapping = new Map();

    let i = 0;
    layout.forEach(label => {
        mapping.set(label, i);
        i += 1;
    });

    graph.nodes.forEach(node => {
        nodes.add({ id: mapping.get(node.label), label: node.label, x: mapping.get(node.label) * 75, y: 0 });
    });

    graph.edges.forEach(edge => {
        edges.add({ from: mapping.get(edge.from), to: mapping.get(edge.to) });
    });

    const options = {
        nodes: {
            color: {
                background: "#76e0f5",
                border: "black",
                highlight: {
                    background: "#bbffff",
                    border: "black"
                }
            },
            font: {
                color: "black",
                size: 16
            },
            borderWidth: 1
        },
        edges: {
            color: "black",
            arrows: {
                to: true
            },
            smooth: {
                type: 'curvedCW'
            }
        },
        interaction: {
            dragNodes: false
        },
        physics: {
            enabled: false
        }
    };

    const container = document.getElementById("network");
    const data = { nodes: nodes, edges: edges };
    const network = new vis.Network(container, data, options);
    const containerWidth = container.offsetWidth;
    const containerHeight = container.offsetHeight;

    network.setSize(containerWidth, containerHeight);

    network.on("doubleClick", function(params) {
        network.fit();
    });

    network.on("afterDrawing", function(ctx) {
        var dataURL = ctx.canvas.toDataURL();
        document.getElementById('canvasImg').href = dataURL;
    });
}