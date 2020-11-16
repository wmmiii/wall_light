
function setEffect(effect) {
  fetch(`/effect/${effect}`, {
    method: 'PUT'
  });
}

function setHue(hue) {
  fetch(`/hue/${hue}`, {
    method: 'PUT'
  });
}

function setHSV(h, s, v) {
  fetch(`/hue/${h}-${s}-${v}`, {
    method: 'PUT'
  });
}

function setCycle(hue) {
  fetch(`/cycle`, {
    method: 'PUT'
  });
}