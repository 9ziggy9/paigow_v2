function main() {
    console.log("main.js is loaded");
    document
	.getElementById("btn-hello-js")
	.addEventListener("click", () => console.log("Hello from JS"));
}

window.onload = main;
