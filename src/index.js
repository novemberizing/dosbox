import dosbox from './dosbox';
import url from './dosbox.wasm';
import axios from 'axios';

const o = {
    run: async function(canvas){
        const bin = await axios.get(url, {responseType: 'arraybuffer'});

        if(!canvas) {
            canvas = document.createElement('canvas');
            canvas.id = 'dosbox';
            canvas.tabIndex = -1;
            canvas.width=640;
            canvas.height=400;
        }

        canvas.addEventListener("oncontextmenu", function(e) {
            e.preventDefault();
        });
        canvas.addEventListener("webglcontextlost", function(e) {
            alert('WebGL context lost. You will need to reload the page.');
            e.preventDefault();
        }, false);

        if(!document.getElementById(canvas.id)) {
            document.body.appendChild(canvas);
        }

        return await dosbox({canvas, wasmBinary: bin.data });
    }
};

export default o;