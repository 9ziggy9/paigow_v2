function initTileState(tileGridId) {
    console.log("Initializing tile state.");
    let _tile_refs = document.getElementById(tileGridId).querySelectorAll("span");
    let _num_selected_tiles = 0;
    let _selected_tiles     = new Set();
    return {
	refs: () => _tile_refs,
	count: () => _num_selected_tiles,
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
    })
}

function uiTileGrid(tileState) {
    const ts = tileState.refs();
    ts.forEach(t => {
	t.addEventListener("click", () => {
	    if (tileState.tileSet().has(t)) tileState.deselectTile(t);
	    else tileState.selectTile(t);
	    uiTileGridUpdateSelections(tileState);
	});
    });
}

function uiCloseModal() {
    const modalBg = document.getElementById("tile-modal");
    modalBg.addEventListener("click", ev => {
        if (ev.target === modalBg) {
            console.log("Hiding modal.");
            modalBg.classList.toggle("hidden");
	    NUM_SELECTED_TILES = 0;
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
    uiCloseModal();
    uiModalClick();
}

window.onload = main;
