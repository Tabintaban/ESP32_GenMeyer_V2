// Complete project details: https://randomnerdtutorials.com/esp32-web-server-websocket-sliders/

var gateway = `ws://${window.location.hostname}/ws`; //Шлюз — это точка входа в интерфейс 'WebSocket.окно.местоположение.имя хоста' получает текущий адрес страницы (IP-адрес веб-сервера).
var websocket;                                       //Глобальная переменная
window.addEventListener('load', onload);             //Прослушиватель события который будет вызывать функцию onload() при загрузке 'load' страницы 

function onload(event) {                             //onload() вызывает initWebSocket() инициализирует вэбсокет и соединяется с сервером
    initWebSocket();
}

function getValues(){                                //
    websocket.send("getValues");                     //отправляет на сервер значение всез ползунков
}

function initWebSocket() {                                 //инициализация вэбсокета
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);                    //создает объект вэбсокет
    websocket.onopen = onOpen;                             //открывает вэбсокет и передает ему функцию onOpen()
    websocket.onclose = onClose;                           //закрывает вэбсокет и передает ему функцию onClose()
    websocket.onmessage = onMessage;                       //получение сообщения и передает ему функцию onMessage()
}

function onOpen(event) {                                   //когда соединение открыто вызываем getValues()
    console.log('Connection opened');
    getValues();                                           //передаем на сервер значение всех слайдеров
}

function onClose(event) {                                  //когда соединение закрываем, после ждет 2 секунды
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function updateSliderPWM(element) {                                        //запускается при перемещении ползунков
    var sliderNumber = element.id.charAt(element.id.length-1);             
    var sliderValue = document.getElementById(element.id).value;
    document.getElementById("sliderValue"+sliderNumber).innerHTML = sliderValue;
    console.log(sliderValue);
    websocket.send(sliderNumber+"s"+sliderValue.toString());               //отправляет на сервер номер слайдера и значение слайдера
}

function onMessage(event) {                                //обрабатываем полученое сообщение по вэбсокет протоколу на onMessage()
    console.log(event.data);                               //сервер отправляет данные в JSON формате
    var myObj = JSON.parse(event.data);                    // {
    var keys = Object.keys(myObj);                         //   slider1, 50;
                                                           //   slider2, 30;
    for (var i = 0; i < keys.length; i++){                 // }
        var key = keys[i];                                 // onMessage() переберает все значения ползунков и вставляет в нужные места html страницы
        document.getElementById(key).innerHTML = myObj[key];
        document.getElementById("slider"+ (i+1).toString()).value = myObj[key];
    }
}
