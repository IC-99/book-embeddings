async function getData(url) {    
    const response = await fetch(url);
    return response.json();
}

function initialize_filter(data) {
    var types = ["select type"];
    var genres = ["select genre"];
    var moods = ["select mood"];
    for(i in data) {
        var song = data[i]
        if(!types.includes(song["type"], 1)) {
            types.push(song["type"]);
        }
        if(!genres.includes(song["genre1"], 1)) {
            genres.push(song["genre1"]);
        }
        if(!genres.includes(song["genre2"], 1)) {
            genres.push(song["genre2"]);
        }
        if(!moods.includes(song["mood"], 1)) {
            moods.push(song["mood"]);
        }
    }

    var type_select = '<select id="filter-type">\n';
    var genre_select = '<select id="filter-type">\n';
    var mood_select = '<select id="filter-type">\n';

    for(i in types) {
        type_select = type_select + '<option>' + types[i] + '</option>\n';
    }
    for(i in genres) {
        genre_select = genre_select + '<option>' + genres[i] + '</option>\n';
    }
    for(i in moods) {
        mood_select = mood_select + '<option>' + moods[i] + '</option>\n';
    }

    type_select = type_select + '</select>\n';
    genre_select = genre_select + '</select>\n';
    mood_select = mood_select + '</select>\n';

    document.querySelector('#filter-type').innerHTML = type_select;
    document.querySelector('#filter-genre').innerHTML = genre_select;
    document.querySelector('#filter-mood').innerHTML = mood_select;
}


function createList(data) {
    var result = '';
    for(i in data) {
        var song = data[i]
        var partial_result =
            '<div class="list-element">\n\
                <div class="list-element-title">\n\
                    <h2>' + song["title"] + '</h2>\n\
                </div>\n\
                <div class="list-element-info">\n\
                    <span class="list-element-author">' + song["author"] + '</span>\n\
                    <span class="list-element-arrangement">' + song["type"] + '</span>\n\
                    <span class="list-element-primary-genre">' + song["genre1"] + '</span>\n\
                    <span class="list-element-secondary-genre">' + song["genre2"] + '</span>\n\
                    <span class="list-element-mood">' + song["mood"] + '</span>\n\
                </div>\n\
                <div class="list-element-technical-info">\n\
                    <span class="list-element-technical-BPM">BPM</span>\n\
                    <span class="list-element-technical-BPM-value">' + song["bpm"] + '</span>\n\
                </div>\n\
                <div class="list-element-audio-player">\n\
                    <audio controls>\n\
                        <source src="assets/music/' + song["title"] + '.mp3" type="audio/mpeg"/>\n\
                    </audio>\n\
                </div>\n\
                <div class="list-element-contact">\n\
                    <span class="list-element-contact-email">' + song["email"] + '</span>\n\
                </div>\n\
                <div class="list-element-button-panel">\n\
                    <a href="assets/music/' + song["title"] + '.mp3" download="' + song["title"] + '-soundtrackmatch' + '" target="_blank">\n\
                        <img id="button-download" class="button-icon" src="images/download_icon.png" alt="">\n\
                    </a>\n\
                </div>\n\
            </div>\n';
        var result = result + partial_result;
    }
    return result;
}

function filter_data(data, key, value) {
    var filtered_data = [];
    for(i in data){
        var song = data[i];
        if(song[key] == value) {
            filtered_data.push(song);
        }
    }
    return filtered_data;
}

function filter_data_by_genre(data, value) {
    var filtered_data = [];
    for(i in data){
        var song = data[i];
        if(song["genre1"] == value || song["genre2"] == value) {
            filtered_data.push(song);
        }
    }
    return filtered_data;
}

// Definisci la tua funzione che sarà eseguita quando l'utente fa clic sul link
function handleClick(event, data) {
    event.preventDefault(); // Previeni il comportamento predefinito del link (nella maggior parte dei casi, seguire il link)
  
    // Aggiungi qui la logica o le azioni che desideri eseguire quando il link viene cliccato
    console.log("Hai cliccato sul link!");
    var filtered_data = filter_data(data, "type", "vocal");
    var list = createList(filtered_data)
    document.querySelector('#list').innerHTML = list;
}

function handleFilter(event, data) {
    event.preventDefault(); // Previeni l'invio predefinito del form

    // Ottieni i valori dei campi inseriti dall'utente
    const type_input = document.getElementById("filter-type");
    const genre_input = document.getElementById("filter-genre");
    const mood_input = document.getElementById("filter-mood");

    const type = type_input.value;
    const genre = genre_input.value;
    const mood = mood_input.value;

    // Esegui azioni in base ai dati inseriti dall'utente
    // console.log("type inserito:", type);
    // console.log("genre inserito:", genre);
    // console.log("mood inserito:", mood);

    var filtered_data = data

    if(type != "select type") {
        filtered_data = filter_data(filtered_data, "type", type)
    }
    if(genre != "select genre") {
        filtered_data = filter_data_by_genre(filtered_data, genre)
    }
    if(mood != "select mood") {
        filtered_data = filter_data(filtered_data, "mood", mood)
    }

    var list = createList(filtered_data)
    document.querySelector('#list').innerHTML = list;    
}

async function main() {
    const url_data = "https://ivancarlini.bitbucket.io/articles/soundtracks/assets/json/songs.json";
    let data
    try {
        data = await getData(url_data);
    } catch (error) {
        console.error("Si è verificato un errore durante il recupero dei dati:", error);
    }
    
    initialize_filter(data)
    var list = createList(data)
    document.querySelector('#list').innerHTML = list;

    // Ottieni il riferimento al link tramite il suo ID
    const myLink = document.getElementById("myLink");

    // Assegna la funzione "handleClick" all'evento "click" del link
    myLink.addEventListener("click", event => handleClick(event, data));

    const filter = document.getElementById("filter-form"); 
    filter.addEventListener("submit", event => handleFilter(event, data));
}

main();