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
    tileState.refs().forEach(t => {
	t.classList.remove("dimmed");
	t.classList.remove("checkmark-overlay");
    });
    tileState.tileSet().forEach(t => {
	t.classList.add("dimmed");
	t.classList.add("checkmark-overlay");
    });
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

function uiOpenModal(tileState) {
    const modalBg = document.getElementById("tile-modal");
    modalBg.classList.remove("hidden");
    const ts = tileState.tileSet();
    ts.forEach(t => {
	t.classList.remove("checkmark-overlay");
	t.classList.remove("dimmed");
    });

    const [h1,h2,l1,l2] = ts;

    const hiHandContainer = document.getElementById("tile-modal-hi-hand");
    const loHandContainer = document.getElementById("tile-modal-lo-hand");
    hiHandContainer.innerHTML = "";
    loHandContainer.innerHTML = "";

    hiHandContainer.appendChild(h1.cloneNode(true));
    hiHandContainer.appendChild(h2.cloneNode(true));
    loHandContainer.appendChild(l1.cloneNode(true));
    loHandContainer.appendChild(l2.cloneNode(true));
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

function main() {
    console.log("main.js is loaded");
    const tileState = initTileState("tile-grid");
    uiTileGrid(tileState);
    uiCloseModal(tileState);
    uiModalClick();
}

window.onload = main;
