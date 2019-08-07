window.addEventListener('DOMContentLoaded', () => {

    const buttons = document.querySelectorAll('.regulator > button'),
        inputs = document.querySelectorAll('.regulator > input'),
        scripts = document.querySelectorAll('.script');

    buttons.forEach(function(btn, i) {
        btn.addEventListener('click', () =>{
            sendValue(i);
        });
    });

    scripts.forEach(function(script, i) {
        script.addEventListener('click', function() {
            sendScript(i);
        });
    });

    function sendScript(i) {
        fetch(`/script?script-num=${i}`);
    }

    function sendValue(i) {
        let value = +inputs[i].value;
        if (value < 180 && value > 0 && value !== 'а') {
            fetch(`/servo?servo=${i}&angle=${value}`);
            console.log(`/servo?servo=${i}&angle=${value}`);
            inputs[i].value = '';
        } else {
            console.log('что-то пошло не так...');
            inputs[i].value = '';
        }
    };
})