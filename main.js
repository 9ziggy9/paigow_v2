function initCFFI(m) {
    // JavaScript glue
    const TILE_BIT_MAP = {
	"teen"      : 0xF,
	"day"       : 0xE,
	"yun"       : 0xD,
	"gor"       : 0xC,
	"muuy"      : 0xB,
	"chong"     : 0xA,
	"bon"       : 0x9,
	"fu"        : 0x8,
	"ping"      : 0x7,
	"chit"      : 0x6,
	"look"      : 0x5,
	"chopgow"   : 0x4,
	"chopbot"   : 0x3,
	"chopchit"  : 0x2,
	"chopng"    : 0x1,
	"geejun"    : 0x0,
    }
    const _JSTileIdToBitRep = (tl) => TILE_BIT_MAP[tl.id.split("_")[1]];
    const _JSComposeTiles   = (t1, t2, t3, t4) =>
	  (t1 << 12) | (t2 << 8) | (t3 << 4) | (t4 << 0);

    m.ccall("wasm_ok", null, [], []);

    return {
	JSTileIdToBitRep: _JSTileIdToBitRep,
	JSComposeTiles  : _JSComposeTiles,
    }
}

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

function uiTileGrid(tileState, engine) {
    const ts = tileState.refs();
    ts.forEach(t => {
	t.addEventListener("click", () => {
	    if (tileState.tileSet().has(t)) tileState.deselectTile(t);
	    else tileState.selectTile(t);
	    uiTileGridUpdateSelections(tileState);
	    if (tileState.count() === 4) uiOpenModal(tileState, engine);
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

function uiOpenModal(tileState, engine) {
    const modalBg = document.getElementById("tile-modal");
    modalBg.classList.remove("hidden");
    const [h1,h2,l1,l2] = tileState.tileSet();

    const hiHandContainer = document.getElementById("tile-modal-hi-hand");
    const loHandContainer = document.getElementById("tile-modal-lo-hand");
    hiHandContainer.innerHTML = "";
    loHandContainer.innerHTML = "";

    const bitwiseRep = engine.JSComposeTiles(
	engine.JSTileIdToBitRep(h1), engine.JSTileIdToBitRep(h2),
	engine.JSTileIdToBitRep(l1), engine.JSTileIdToBitRep(l2)
    );
    console.log("0x" + bitwiseRep.toString(16));

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
    console.log("Module exists?", typeof Module !== 'undefined');
    console.log("Module:", Module);
    
    if (typeof Module !== 'undefined') {
        Module.onRuntimeInitialized = function() {
            console.log("WASM module loaded!");
            const engine = initCFFI(Module);
            const tileState = initTileState("tile-grid");
            uiTileGrid(tileState, engine);
            uiCloseModal(tileState);
            uiModalClick();
        };
    } else {
        console.error("Module not found - is WASM script loaded?");
    }
}


window.onload = main;
