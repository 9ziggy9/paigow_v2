// const EXCEPTION_HAND_MAP = {
//     "0xA500": 0x050A
// }

function initCFFI(m) {
    m.ccall("wasm_ok", null, [], []);
    return {
	sort_hand_by_points : h => m.ccall(
	    "sort_hand_by_points",
	    "number",
	    ["number"],
	    [h]
	),
	read_raw_tiles  : (n) => m.ccall(
	    "read_raw_tiles",
	    null,
	    ["number"],
	    [n]
	),
    }
}

function initTileState(tileGridId) {
    console.log("Initializing tile state.");
    const _GET_NTH_TILE = (h, n) => ((h >> ((3 - n) * 4)) & 0xF)
    const _TILE_BIT_MAP = {
	"teen": 0xF, "day": 0xE, "yun": 0xD, "gor": 0xC, "muuy": 0xB,
	"chong": 0xA, "bon": 0x9, "fu": 0x8, "ping": 0x7, "chit": 0x6,
	"look": 0x5, "chopgow": 0x4, "chopbot": 0x3, "chopchit": 0x2,
	"chopng": 0x1, "geejun": 0x0
    };

    const _TILE_ID_MAP = [
	"tl_geejun", "tl_chopng", "tl_chopchit", "tl_chopbot",
	"tl_chopgow", "tl_look", "tl_chit", "tl_ping", "tl_fu",
	"tl_bon", "tl_chong", "tl_muuy", "tl_gor", "tl_yun",
	"tl_day", "tl_teen"
    ];
    
    let _tile_refs = document.getElementById(tileGridId).querySelectorAll("span");
    let _num_selected_tiles = 0;
    let _selected_tiles = new Set();
    let _hand = {
	lo: [],
	hi: [],
    };
    return {
	utility: {
	    tileIdMap: (id) => _TILE_ID_MAP[id],
	    tileBitMap: (b) => _TILE_BIT_MAP[b],
	    tileIdToBitRep: (t) => _TILE_BIT_MAP[t.id.split("_")[1]],
	    composeTiles: (t1,t2,t3,t4) => (t1 << 12) |
		(t2 << 8) |
		(t3 << 4) |
		(t4 << 0),
	},
	refs:    () => _tile_refs,
	count:   () => _num_selected_tiles,
	tileSet: () => _selected_tiles,
	selectTile: el => {
	    _selected_tiles.add(el);
	    return ++_num_selected_tiles;
	},
	deselectTile: el => {
	    _selected_tiles.delete(el);
	    return --_num_selected_tiles;
	},
	reset: () => {
	    _num_selected_tiles = 0;
	    _selected_tiles.clear();
	    _hand = {};
	},
	syncHandState: (bitwiseRep) => {
	    const selectedArray = Array.from(_selected_tiles);
	    const tileGroups = {};

	    selectedArray.forEach(el => {
		const baseName = el.id.split('_').slice(0, 2).join('_'); // "tl_teen"
		if (!tileGroups[baseName]) tileGroups[baseName] = [];
		tileGroups[baseName].push(el);
	    });

	    const orderedElements = [0, 1, 2, 3].map(n => {
		const nibble = _GET_NTH_TILE(bitwiseRep, n);
		const tileId = _TILE_ID_MAP[nibble];

		// Pop first available tile with this name
		return tileGroups[tileId]?.shift();
	    });

	    _hand = {
		hi: orderedElements.slice(0, 2),
		lo: orderedElements.slice(2, 4),
	    };
	},
	hand: () => _hand,
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

    const bitwiseRep = tileState.utility.composeTiles(
	tileState.utility.tileIdToBitRep(h1), tileState.utility.tileIdToBitRep(h2),
	tileState.utility.tileIdToBitRep(l1), tileState.utility.tileIdToBitRep(l2)
    );

    const sortedHandRepr = engine.sort_hand_by_points(bitwiseRep);
    tileState.syncHandState(sortedHandRepr);

    const houseWayHand = tileState.hand();

    uiTileSetCheckmarks(
	hiHandContainer.appendChild(houseWayHand.hi[1].cloneNode(true)), false
    );
    uiTileSetCheckmarks(
	hiHandContainer.appendChild(houseWayHand.hi[0].cloneNode(true)), false
    );
    uiTileSetCheckmarks(
	loHandContainer.appendChild(houseWayHand.lo[1].cloneNode(true)), false
    );
    uiTileSetCheckmarks(
	loHandContainer.appendChild(houseWayHand.lo[0].cloneNode(true)), false
    );
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
