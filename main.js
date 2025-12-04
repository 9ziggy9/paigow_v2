function uiTileCheckmarks() {
    const tiles = document.getElementById("tile-grid").querySelectorAll("span");
    tiles.forEach(t => {
	t.addEventListener("click", () => {
	    console.log("clicked thing");
	    t.classList.toggle("checkmark-overlay");
	    t.classList.toggle("dimmed");
	});
    });
}

function main() {
    console.log("main.js is loaded");
    uiTileCheckmarks();
}

window.onload = main;
