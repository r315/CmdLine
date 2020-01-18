
//v(t) = sin(2pi ft) * A

let signal = []
let freq = 1000
let points = 200
let amplitude = 512
let offset = 512

function printSignal(signal){
    signal.forEach( (v) => {
        console.log(" ".repeat(v*10) + '#')
    })
}

function signalTable(name, signal){
    console.log(`const uint16_t ${name} [] = {`)
    console.log(`\t${signal.join(',')}\n};`)
}

for(i = 0; i <= points; i++ ){
    let value = ((Math.sin(2 * Math.PI * i / points) * amplitude) + offset).toFixed(0)
    value = ( value > 1023 ) ? 1023 : value
    signal.push( value << 6 ) 
    //sinetab.push(((Math.random() * amplitude) + offset).toFixed(3))
}

console.log(`Sine ${points} Points`)
//printSignal(signal);
signalTable("sine", signal)
//console.log(sinetab)

signal = []

for(i = 0; i <= points; i++ ){
    signal.push((
        ( i / points * amplitude) + offset
        ).toFixed(3))
}
//console.log(`Triangle ${points} Points`)
//printSignal(signal);
//console.log(signal)
signal = []
for(i = 0; i <= points; i++ ){
    let value = ((i & 1) ==  1 ? 1 : -1) * amplitude + offset
    value = ( value > 1023 ) ? 1023 : value
    signal.push( value << 6) 
}
signalTable("square", signal)