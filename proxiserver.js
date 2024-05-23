const http = require('http');
const https = require('https');
const url = require('url');

// Кэш в оперативной памяти
const cache = {};

// Функция для получения данных с внешнего сервера и кэширования их
function fetchAndCache(requestUrl, callback) {
    const options = url.parse(requestUrl);
    const protocol = options.protocol === 'https:' ? https : http;

    protocol.get(options, (res) => {
        let data = '';

        // Сборка данных по мере их получения
        res.on('data', (chunk) => {
            data += chunk;
        });

        // После получения всех данных
        res.on('end', () => {
            // Сохранение данных в кэш
            cache[requestUrl] = {
                data: data,
                headers: res.headers,
                statusCode: res.statusCode,
                timestamp: Date.now()
            };

            console.log(`получено и кэшировано: ${requestUrl}`);
            callback(null, cache[requestUrl]);
        });
    }).on('error', (err) => {
        console.error(`Ошибка при получении: ${requestUrl} - ${err.message}`);
        callback(err);
    }).on('response', (res) => {
        if (res.statusCode >= 400) {
            console.error(`Ошибка ответа: ${requestUrl} - Статус код: ${res.statusCode}`);
        }
    });
}

// Создание HTTP-сервера
const server = http.createServer((req, res) => {
    const requestUrl = req.url.slice(1); // Убираем ведущий '/'
    
    // Игнорируем запросы к favicon.ico
    if (requestUrl === 'favicon.ico') {
        res.writeHead(204); // Нет содержимого
        res.end();
        return;
    }

    console.log(`Запрос для: ${requestUrl}`);

    // Проверка кэша
    if (cache[requestUrl]) {
        // Обслуживание из кэша
        const cachedResponse = cache[requestUrl];
        console.log(`Взято из кэша: ${requestUrl} (Кэшировано: ${new Date(cachedResponse.timestamp).toLocaleString()})`);
        res.writeHead(cachedResponse.statusCode, cachedResponse.headers);
        res.end(cachedResponse.data);
    } else {
        // Запрос к внешнему серверу и кэширование ответа
        console.log(`Запрос к внешнему серверу для: ${requestUrl}`);
        fetchAndCache(requestUrl, (err, fetchedResponse) => {
            if (err) {
                res.writeHead(500);
                res.end('Internal Server Error');
                return;
            }

            res.writeHead(fetchedResponse.statusCode, fetchedResponse.headers);
            res.end(fetchedResponse.data);
        });
    }
});

const port = 3001;
server.listen(port, () => {
    console.log(`Proxy server is listening on port ${port}`);
});
