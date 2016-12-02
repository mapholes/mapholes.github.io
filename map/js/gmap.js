 var map; //the google map element
        var directionDisplay;
        var directionsService = new google.maps.DirectionsService();
        var waypts = [];
        var marker ;
        var key = 1;
        var toggle = 0;
        var totalEstimatedTime = 0;
        var totalWorkTime = 0;
        var tableHTML = '<div id="tableHeader" title="Hide Table" onClick="toggleTable();" style="background-color: #DD8324;color: SADDLEBROWN;border-radius: 5px 5px 0px 0px; height: 16px;"><span id="arrow" style="text-align: left; font-size: 12">▶</span> <span style="text-align: center; width:990px;"></span></div>';
        tableHTML += '<table><tr><th>Key</th><th>Type</th><th>Location</th><th>Tools Required</th><th>Estimated Time</th></tr>';

function toggleTable(){

  if (toggle == 0) {
    toggle = 1
    $('#arrow').text('◀')
    $('#tableHeader').attr('title', 'Show Table')
    $('#infoTable').animate({
            right : "-985px"
        }, 'fast');   
  } else {
    toggle = 0
    $('#arrow').text('▶')
    $('#tableHeader').attr('title', 'Hide Table')
    $('#infoTable').animate({
            right : "5px"
        }, 'fast');  

  }
}

$(document).ready(function(){
  $('#infoTable').hide();
  // $('#travel_data').hide();
});

        function initMap() {
            var mapOptions = {
                center: new google.maps.LatLng(40.604050, -74.000557), 
                zoom:13,
                styles:[
                {elementType: 'geometry', stylers: [{color: '#ebe3cd'}]},
                {elementType: 'labels.text.fill', stylers: [{color: '#523735'}]},
                {elementType: 'labels.text.stroke', stylers: [{color: '#f5f1e6'}]},
                {
                  featureType: 'administrative',
                  elementType: 'geometry.stroke',
                  stylers: [{color: '#c9b2a6'}]
                },
                {
                  featureType: 'administrative.land_parcel',
                  elementType: 'geometry.stroke',
                  stylers: [{color: '#dcd2be'}]
                },
                {
                  featureType: 'administrative.land_parcel',
                  elementType: 'labels.text.fill',
                  stylers: [{color: '#ae9e90'}]
                },
                {
                  featureType: 'landscape.natural',
                  elementType: 'geometry',
                  stylers: [{color: '#dfd2ae'}]
                },
                {
                  featureType: 'poi',
                  elementType: 'geometry',
                  stylers: [{color: '#dfd2ae'}]
                },
                {
                  featureType: 'poi',
                  elementType: 'labels.text.fill',
                  stylers: [{color: '#93817c'}]
                },
                {
                  featureType: 'poi.park',
                  elementType: 'geometry.fill',
                  stylers: [{color: '#a5b076'}]
                },
                {
                  featureType: 'poi.park',
                  elementType: 'labels.text.fill',
                  stylers: [{color: '#447530'}]
                },
                {
                  featureType: 'road',
                  elementType: 'geometry',
                  stylers: [{color: '#f5f1e6'}]
                },
                {
                  featureType: 'road.arterial',
                  elementType: 'geometry',
                  stylers: [{color: '#fdfcf8'}]
                },
                {
                  featureType: 'road.highway',
                  elementType: 'geometry',
                  stylers: [{color: '#f8c967'}]
                },
                {
                  featureType: 'road.highway',
                  elementType: 'geometry.stroke',
                  stylers: [{color: '#e9bc62'}]
                },
                {
                  featureType: 'road.highway.controlled_access',
                  elementType: 'geometry',
                  stylers: [{color: '#e98d58'}]
                },
                {
                  featureType: 'road.highway.controlled_access',
                  elementType: 'geometry.stroke',
                  stylers: [{color: '#db8555'}]
                },
                {
                  featureType: 'road.local',
                  elementType: 'labels.text.fill',
                  stylers: [{color: '#806b63'}]
                },
                {
                  featureType: 'transit.line',
                  elementType: 'geometry',
                  stylers: [{color: '#dfd2ae'}]
                },
                {
                  featureType: 'transit.line',
                  elementType: 'labels.text.fill',
                  stylers: [{color: '#8f7d77'}]
                },
                {
                  featureType: 'transit.line',
                  elementType: 'labels.text.stroke',
                  stylers: [{color: '#ebe3cd'}]
                },
                {
                  featureType: 'transit.station',
                  elementType: 'geometry',
                  stylers: [{color: '#dfd2ae'}]
                },
                {
                  featureType: 'water',
                  elementType: 'geometry.fill',
                  stylers: [{color: '#b9d3c2'}]
                },
                {
                  featureType: 'water',
                  elementType: 'labels.text.fill',
                  stylers: [{color: '#92998d'}]
                }
                ]
            }


            directionsDisplay = new google.maps.DirectionsRenderer({
                suppressMarkers: true
            });

            map = new google.maps.Map(document.getElementById('map'), mapOptions);
            directionsDisplay.setMap(map);


        // Create a <script> tag and set the USGS URL as the source.
        var script = document.createElement('script');
        // script.src = 'https://raw.githubusercontent.com/mapholes/mapholes.github.io/master/data/manholes_geojson.js';
        script.src = 'https://rawgit.com/mapholes/mapholes.github.io/master/data/manholes_geojson.js';
        document.getElementsByTagName('head')[0].appendChild(script);

    }

            // Loop through the results array and place a marker for each
      // set of coordinates.
      window.eqfeed_callback = function(results) {

        var iconBase = 'https://raw.githubusercontent.com/mapholes/mapholes.github.io/master/img/';
        var icons = {
          high_priority: {
            icon: iconBase + 'high_priority.png'
        },
        medium_priority: {
            icon: iconBase + 'medium_priority.png'
        },
        low_priority: {
            icon: iconBase + 'low_priority.png'
        },
        company: {
            icon: iconBase + 'companyIcon.png'
        }
    };


    function addSelectedPoint(position){
      waypts.push({
                location: position,
                stopover: true
            });
    }

    function createMarker(latlng, iconForLocation, label, complaintType, address, tools, estimatedTime, i) {
        var marker = new google.maps.Marker({
            position: latlng,
            icon: iconForLocation,
            map: map,
            title: label
        });

        var infowindow = new google.maps.InfoWindow({
          content: label
        });

        marker.addListener('click', function(event) {
          addSelectedPoint(latlng);
          marker.setIcon("https://raw.githubusercontent.com/mapholes/mapholes.github.io/master/img/selected.png"); 

          tableHTML += '<tr><td>'
          + key
          + '</td><td>'
          + complaintType 
          + '</td><td>'
          + address 
          + '</td><td>'
          + tools
          + '</td><td>'
          + estimatedTime
          + '</td></tr>';

          totalEstimatedTime += estimatedTime;
          key += 1;

        });

        marker.addListener('mouseover', function() {
        infowindow.open(map, marker);
        });

        marker.addListener('mouseout', function() {
          infowindow.close();
        });

    }

    var company = new google.maps.LatLng(40.687917, -73.980670);
    createMarker(company, 'https://raw.githubusercontent.com/mapholes/mapholes.github.io/master/img/companyIcon.png');

    for (var i = 0; i < results.features.length; i++) {

        var coords = results.features[i].geometry.coordinates;
        var latLng = new google.maps.LatLng(coords[1],coords[0]);
        var label = results.features[i].properties.complainttype;
        var complaintType = results.features[i].properties.complainttype;
        var address = results.features[i].properties.address + ' ' + results.features[i].properties.city;
        var tools = results.features[i].properties.ToolsRequired;
        var estimatedTime = results.features[i].properties.EstimatedTime;

        // tableHTML += '<tr><td>'+i+'</td><td>'
        //           + results.features[i].properties.complainttype 
        //           + '</td><td>'
        //           + results.features[i].properties.address + ',' + results.features[i].properties.city
        //           + '</td><td>'
        //           + results.features[i].properties.ToolsRequired
        //           + '</td><td>'
        //           + results.features[i].properties.EstimatedTime
        //           + '</td></tr>';

        var iconForLocation;

        if (results.features[i].properties.priority == 'High'){
        iconForLocation = icons['high_priority'].icon;
        }

        else if (results.features[i].properties.status == 'Company'){
        iconForLocation = icons['company'].icon;
        }

        else{
            iconForLocation = icons['low_priority'].icon;
        }
        createMarker(latLng, iconForLocation, label, complaintType, address, tools, estimatedTime, i);
    }


    $('#submitButton').click(function() {

      tableHTML += '</table>';  
      // alert(tableHTML);
      $('#infoTable').html(tableHTML);
      $('#infoTable').show('normal');

        directionsService.route({
            origin: "30 Flatbush Ave, Brooklyn, NY 11217",
            destination: "30 Flatbush Ave, Brooklyn, NY 11217",
            waypoints: waypts,
            optimizeWaypoints: true,
            travelMode: 'DRIVING'
                }, function(response, status) {
                  if (status === 'OK') {
                    directionsDisplay.setDirections(response);
                    var route = response.routes[0];
                    var summaryPanel = document.getElementById('travel_data');
                    summaryPanel.innerHTML = '';
                    // For each route, display summary information.
                    
                    var totalDistance = 0;
                    var totalTime = 0;

                    for (var i = 0; i < route.legs.length; i++) {
                      // var routeSegment = i + 1;

                      totalTime += parseInt(route.legs[i].duration.text);
                      // alert(route.legs[i].duration.text);                     
                      totalDistance += parseFloat(route.legs[i].distance.text);
                      // summaryPanel.innerHTML += '<b>Route Segment: ' + routeSegment +
                      //     '</b><br>';
                      // summaryPanel.innerHTML += route.legs[i].start_address + ' to ';
                      // summaryPanel.innerHTML += route.legs[i].end_address + '<br>';
                      // summaryPanel.innerHTML += parseInt(route.legs[i].distance.text) + '<br><br>';
                    }
                    totalWorkTime = (totalEstimatedTime + (totalTime/60)).toFixed(2);
                    summaryPanel.innerHTML = '<p><b>Total Distance:</b> ' + totalDistance.toFixed(2) + ' mi</p>'
                                           + '<p><b>Travel Time:</b> ' + (totalTime/60).toFixed(2) + ' hrs</p>'
                                           + '<p><b>Total Estimated Work Time:</b> ' + totalEstimatedTime.toFixed(2) + ' hrs</p>'
                                           + '<p><b>Total Time:</b> ' + totalWorkTime + ' hrs</p>';
                  } else {
                    window.alert('Directions request failed due to ' + status);
                  }
        });

    });



}


      // DrawingManager GUI for us to draw polygons, rectangles, polylines, circles, and markers on the map.

      // var drawingManager = new google.maps.drawing.DrawingManager();
      //   drawingManager.setMap(map);
