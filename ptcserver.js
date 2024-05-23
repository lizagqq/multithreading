const net = require('net'); //модуль net для работы с сетевым соединением

const server = net.createServer((socket) => {  //socket нужен для передачи данных
  console.log('Client connected');
  console.log(`Client IP: ${socket.remoteAddress}, Port: ${socket.remotePort}`);

  socket.on('data', (data) => {
    console.log(`Получено: ${data}`);
    socket.write(`Вы ввели: ${data}`);  
  });

  socket.on('end', () => {
    console.log('Client disconnected');
  });

  socket.on('error', (err) => {
    console.error(`Socket error: ${err.message}`);
  });

  socket.on('close', (hadError) => {
    console.log(`Connection closed${hadError ? ' due to an error' : ''}`);
  });
});

const PORT = 8080;
server.listen(PORT, () => {  //server.listen запускает сервер на порту который мы указали
  console.log(`Server listening on port ${PORT}`);
});

server.on('error', (err) => {
  console.error(`Server error: ${err.message}`);
});
