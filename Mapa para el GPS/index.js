// MAPA
var map = L.map('map').setView([19.24925389025252, -103.6973901430436], 18);

L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
    attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
}).addTo(map);

var popup = L.popup();

function onMapClick(e) {
    popup
        .setLatLng(e.latlng)
        .setContent("Diste click en el mapa en " + e.latlng.toString())
        .openOn(map);

    var coordinates = {
        x: e.latlng.lat,
        y: e.latlng.lng
    };

    console.log(coordinates);

    client.publish('esligodfv', JSON.stringify(coordinates));

}

map.on('click', onMapClick);