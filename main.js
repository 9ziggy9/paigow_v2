function initTileState(tileGridId) {
    console.log("Initializing tile state.");
    let _tile_refs = document.getElementById(tileGridId).querySelectorAll("span");
    let _num_selected_tiles = 0;
    let _selected_tiles     = new Set();
    return {
	refs:    () => _tile_refs,
	count:   () => _num_selected_tiles,
	tileSet: () => _selected_tiles,
	selectTile: el => {
	    _selected_tiles.add(el);
	    console.log("Selected tile: ", el);
	    return ++_num_selected_tiles;
	},
	deselectTile: el => {
	    _selected_tiles.delete(el);
	    console.log("Deselected tile: ", el);
	    return --_num_selected_tiles;
	},
	reset: () => {
	    _num_selected_tiles = 0;
	    _selected_tiles.clear();
	}
    }
}

function uiTileGridUpdateSelections(tileState) {
    tileState.refs().forEach(t => uiTileSetCheckmarks(t, false));
    tileState.tileSet().forEach(t => uiTileSetCheckmarks(t, true));
}

function uiTileGrid(tileState) {
    const ts = tileState.refs();
    ts.forEach(t => {
	t.addEventListener("click", () => {
	    if (tileState.tileSet().has(t)) tileState.deselectTile(t);
	    else tileState.selectTile(t);
	    uiTileGridUpdateSelections(tileState);
	    if (tileState.count() === 4) uiOpenModal(tileState);
	});
    });
}

function uiTileSetCheckmarks(t, b) {
    if (b) {
	t.classList.add("checkmark-overlay");
	t.classList.add("dimmed");
    } else {
	t.classList.remove("checkmark-overlay");
	t.classList.remove("dimmed");
    }
}

function uiOpenModal(tileState) {
    const modalBg = document.getElementById("tile-modal");
    modalBg.classList.remove("hidden");
    const [h1,h2,l1,l2] = tileState.tileSet();

    const hiHandContainer = document.getElementById("tile-modal-hi-hand");
    const loHandContainer = document.getElementById("tile-modal-lo-hand");
    hiHandContainer.innerHTML = "";
    loHandContainer.innerHTML = "";

    callEngine();

    uiTileSetCheckmarks(hiHandContainer.appendChild(h1.cloneNode(true)), false);
    uiTileSetCheckmarks(hiHandContainer.appendChild(h2.cloneNode(true)), false);
    uiTileSetCheckmarks(loHandContainer.appendChild(l1.cloneNode(true)), false);
    uiTileSetCheckmarks(loHandContainer.appendChild(l2.cloneNode(true)), false);
}

function uiCloseModal(tileState) {
    const modalBg = document.getElementById("tile-modal");
    modalBg.addEventListener("click", ev => {
        if (ev.target === modalBg) {
	    tileState.reset();
	    uiTileGridUpdateSelections(tileState);
            modalBg.classList.toggle("hidden");
        }
    });
}

function uiModalClick() {
    const modal = document.getElementById("tile-modal-content");
    modal.addEventListener("click", ev => {
	ev.stopPropagation();
    });
}

function callEngine() {
    console.log("Calling WebAssembly engine.");
}

function main() {
    console.log("main.js is loaded");
    const tileState = initTileState("tile-grid");
    uiTileGrid(tileState);
    uiCloseModal(tileState);
    uiModalClick();
}

window.onload = main;
