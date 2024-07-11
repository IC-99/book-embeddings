$(document).ready(function() {
    $("audio").mediaelementplayer({
      alwaysShowControls: true,
      features: ["playpause", "current","progress", "duration", "volume"],
      audioVolume: "vertical",
      audioWidth: 445,
      audioHeight: 35
    });
  });
  