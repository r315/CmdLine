
let str_desc = {
    'SIZE' : '',
    'TYPE' : '',
    'STRING' : ''
}

function toHex(number){
    let pad = ''
    if (number < 16)
        pad = '0'
    return `0x${pad}${number.toString(16).toUpperCase()}`
}

function strDescToByte(descriptor){
    let str = [...descriptor.STRING]
    let size = str.length * 2 + 2 
    console.log(`${toHex(size)},`)
    console.log(descriptor.TYPE + ',')
    str.forEach(c => process.stdout.write(`'${c}',0,`))
	console.log("\n");
}

str_desc.TYPE = 'DESC_STRING'
str_desc.STRING = process.argv[2]

strDescToByte(str_desc)
