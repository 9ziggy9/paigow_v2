function main() {
    console.log("main.js is loaded");
    document
	.getElementById("btn-hello-js")
	.addEventListener("click", () => console.log("Hello from JS"));
    document
	.getElementById("btn-hello-wasm")
	.addEventListener("click", () =>{
	    Module.ccall('hello_from_wasm', null, [], []);
	});
}

window.onload = main;
