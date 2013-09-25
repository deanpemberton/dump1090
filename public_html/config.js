// --------------------------------------------------------
//
// This file is to configure the configurable settings.
// Load this file before script.js file at gmap.html.
//
// --------------------------------------------------------

// -- JSON-feed -------------------------------------------
CONST_JSON = '/dump1090/data.json'; // Default '/dump1090/data.json'

// -- Output Settings -------------------------------------
// Show metric values
Metric = false; // true or false

// -- Map settings ----------------------------------------
// The Latitude and Longitude in decimal format
CONST_CENTERLAT = 45.0;
CONST_CENTERLON = 9.0;
// The google maps zoom level, 0 - 16, lower is further out
CONST_ZOOMLVL   = 5;

// -- Marker settings -------------------------------------
// The default marker color
MarkerColor	  = "rgb(127, 127, 127)";
SelectedColor = "rgb(225, 225, 225)";

// -- Site Settings ---------------------------------------
SiteShow    = false; // true or false
// The Latitude and Longitude in decimal format
SiteLat     = 45.0;
SiteLon     = 9.0;

SiteCircles = true; // true or false (Only shown if SiteShow is true)
// In nautical miles or km (depending settings value 'Metric')
SiteCirclesDistances = new Array(5,10,50,100,150,200);

// -- METAR data ------------------------------------------
// ICAO codes separated with comma
MetarIcaoCode = "EGLL,EGLC";

// -- Antenna Data Collection -----------------------------
AntennaDataCollect  = false;
AntennaDataShow     = false;
AntennaDataOpacity  = 0.3;

